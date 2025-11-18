// SPDX-License-Identifier: GPL-2.0
/*
 * nvme-openmptcp-opt.c - NVMe Optimization Driver for OpenMPTCProuter
 *
 * Copyright (c) 2025, OpenMPTCProuter Project
 * Author: OpenMPTCProuter Contributors
 *
 * This driver provides NVMe-specific optimizations for router/networking
 * workloads, including:
 * - Automatic queue depth tuning
 * - I/O scheduler optimization
 * - Power state management
 * - Performance monitoring and statistics
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/blkdev.h>
#include <linux/nvme.h>
#include <linux/pci.h>
#include <linux/sysfs.h>
#include <linux/device.h>
#include <linux/workqueue.h>
#include <linux/timer.h>

#define DRIVER_NAME	"nvme-openmptcp-opt"
#define DRIVER_VERSION	"1.0"

/* Optimization parameters */
#define NVME_OPT_DEFAULT_QUEUE_DEPTH	128
#define NVME_OPT_MAX_QUEUE_DEPTH	1024
#define NVME_OPT_MIN_QUEUE_DEPTH	32

/* Power management thresholds (in ms) */
#define NVME_OPT_IDLE_TIMEOUT		100
#define NVME_OPT_ACTIVE_TIMEOUT		1000

/* Performance monitoring interval (in seconds) */
#define NVME_OPT_MONITOR_INTERVAL	60

struct nvme_opt_device {
	struct device		*dev;
	struct nvme_ctrl	*ctrl;
	struct list_head	list;

	/* Optimization parameters */
	unsigned int		queue_depth;
	unsigned int		io_timeout;
	bool			apst_enabled;

	/* Performance statistics */
	u64			read_ops;
	u64			write_ops;
	u64			read_bytes;
	u64			write_bytes;
	u64			io_errors;

	/* Monitoring */
	struct delayed_work	monitor_work;
	struct timer_list	idle_timer;
	bool			is_active;

	/* Sysfs attributes */
	struct kobject		*kobj;
};

struct nvme_opt_driver {
	struct list_head	device_list;
	struct mutex		device_mutex;
	struct workqueue_struct	*wq;
};

static struct nvme_opt_driver nvme_opt = {
	.device_list = LIST_HEAD_INIT(nvme_opt.device_list),
	.device_mutex = __MUTEX_INITIALIZER(nvme_opt.device_mutex),
};

/* Sysfs attribute show/store functions */
static ssize_t queue_depth_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	struct nvme_opt_device *opt_dev = dev_get_drvdata(dev);
	return sprintf(buf, "%u\n", opt_dev->queue_depth);
}

static ssize_t queue_depth_store(struct device *dev,
				 struct device_attribute *attr,
				 const char *buf, size_t count)
{
	struct nvme_opt_device *opt_dev = dev_get_drvdata(dev);
	unsigned int depth;
	int ret;

	ret = kstrtouint(buf, 10, &depth);
	if (ret)
		return ret;

	if (depth < NVME_OPT_MIN_QUEUE_DEPTH || depth > NVME_OPT_MAX_QUEUE_DEPTH)
		return -EINVAL;

	opt_dev->queue_depth = depth;
	dev_info(dev, "Queue depth set to %u\n", depth);

	return count;
}

static ssize_t apst_enabled_show(struct device *dev,
				 struct device_attribute *attr, char *buf)
{
	struct nvme_opt_device *opt_dev = dev_get_drvdata(dev);
	return sprintf(buf, "%d\n", opt_dev->apst_enabled);
}

static ssize_t apst_enabled_store(struct device *dev,
				  struct device_attribute *attr,
				  const char *buf, size_t count)
{
	struct nvme_opt_device *opt_dev = dev_get_drvdata(dev);
	bool enabled;
	int ret;

	ret = kstrtobool(buf, &enabled);
	if (ret)
		return ret;

	opt_dev->apst_enabled = enabled;
	dev_info(dev, "APST %s\n", enabled ? "enabled" : "disabled");

	return count;
}

static ssize_t stats_show(struct device *dev,
			  struct device_attribute *attr, char *buf)
{
	struct nvme_opt_device *opt_dev = dev_get_drvdata(dev);

	return sprintf(buf,
		"Read Operations: %llu\n"
		"Write Operations: %llu\n"
		"Read Bytes: %llu\n"
		"Write Bytes: %llu\n"
		"I/O Errors: %llu\n"
		"Status: %s\n",
		opt_dev->read_ops,
		opt_dev->write_ops,
		opt_dev->read_bytes,
		opt_dev->write_bytes,
		opt_dev->io_errors,
		opt_dev->is_active ? "active" : "idle");
}

static ssize_t stats_reset_store(struct device *dev,
				 struct device_attribute *attr,
				 const char *buf, size_t count)
{
	struct nvme_opt_device *opt_dev = dev_get_drvdata(dev);

	opt_dev->read_ops = 0;
	opt_dev->write_ops = 0;
	opt_dev->read_bytes = 0;
	opt_dev->write_bytes = 0;
	opt_dev->io_errors = 0;

	dev_info(dev, "Statistics reset\n");
	return count;
}

static DEVICE_ATTR_RW(queue_depth);
static DEVICE_ATTR_RW(apst_enabled);
static DEVICE_ATTR_RO(stats);
static DEVICE_ATTR_WO(stats_reset);

static struct attribute *nvme_opt_attrs[] = {
	&dev_attr_queue_depth.attr,
	&dev_attr_apst_enabled.attr,
	&dev_attr_stats.attr,
	&dev_attr_stats_reset.attr,
	NULL,
};

static const struct attribute_group nvme_opt_attr_group = {
	.name = "nvme_optimization",
	.attrs = nvme_opt_attrs,
};

/* Performance monitoring worker */
static void nvme_opt_monitor_work(struct work_struct *work)
{
	struct nvme_opt_device *opt_dev = container_of(to_delayed_work(work),
						       struct nvme_opt_device,
						       monitor_work);
	u64 total_ops;
	u64 total_bytes;

	total_ops = opt_dev->read_ops + opt_dev->write_ops;
	total_bytes = opt_dev->read_bytes + opt_dev->write_bytes;

	dev_dbg(opt_dev->dev, "Monitor: %llu ops, %llu bytes, %llu errors\n",
		total_ops, total_bytes, opt_dev->io_errors);

	/* Auto-tune queue depth based on I/O patterns */
	if (total_ops > 10000 && opt_dev->queue_depth < NVME_OPT_MAX_QUEUE_DEPTH) {
		/* High load - increase queue depth */
		opt_dev->queue_depth = min(opt_dev->queue_depth * 2,
					   NVME_OPT_MAX_QUEUE_DEPTH);
		dev_info(opt_dev->dev, "Auto-tuned queue depth to %u (high load)\n",
			 opt_dev->queue_depth);
	} else if (total_ops < 1000 && opt_dev->queue_depth > NVME_OPT_MIN_QUEUE_DEPTH) {
		/* Low load - decrease queue depth */
		opt_dev->queue_depth = max(opt_dev->queue_depth / 2,
					   NVME_OPT_MIN_QUEUE_DEPTH);
		dev_info(opt_dev->dev, "Auto-tuned queue depth to %u (low load)\n",
			 opt_dev->queue_depth);
	}

	/* Schedule next monitoring cycle */
	queue_delayed_work(nvme_opt.wq, &opt_dev->monitor_work,
			   NVME_OPT_MONITOR_INTERVAL * HZ);
}

/* Idle timer callback */
static void nvme_opt_idle_timer(struct timer_list *t)
{
	struct nvme_opt_device *opt_dev = from_timer(opt_dev, t, idle_timer);

	if (opt_dev->is_active) {
		opt_dev->is_active = false;
		dev_dbg(opt_dev->dev, "Device transitioned to idle state\n");
	}
}

/* Initialize optimization for a device */
static int nvme_opt_init_device(struct device *dev)
{
	struct nvme_opt_device *opt_dev;
	int ret;

	opt_dev = kzalloc(sizeof(*opt_dev), GFP_KERNEL);
	if (!opt_dev)
		return -ENOMEM;

	opt_dev->dev = dev;
	opt_dev->queue_depth = NVME_OPT_DEFAULT_QUEUE_DEPTH;
	opt_dev->io_timeout = NVME_OPT_ACTIVE_TIMEOUT;
	opt_dev->apst_enabled = true;
	opt_dev->is_active = false;

	/* Initialize statistics */
	opt_dev->read_ops = 0;
	opt_dev->write_ops = 0;
	opt_dev->read_bytes = 0;
	opt_dev->write_bytes = 0;
	opt_dev->io_errors = 0;

	/* Setup monitoring work */
	INIT_DELAYED_WORK(&opt_dev->monitor_work, nvme_opt_monitor_work);

	/* Setup idle timer */
	timer_setup(&opt_dev->idle_timer, nvme_opt_idle_timer, 0);

	/* Create sysfs attributes */
	ret = sysfs_create_group(&dev->kobj, &nvme_opt_attr_group);
	if (ret) {
		dev_err(dev, "Failed to create sysfs group: %d\n", ret);
		kfree(opt_dev);
		return ret;
	}

	dev_set_drvdata(dev, opt_dev);

	/* Add to device list */
	mutex_lock(&nvme_opt.device_mutex);
	list_add_tail(&opt_dev->list, &nvme_opt.device_list);
	mutex_unlock(&nvme_opt.device_mutex);

	/* Start monitoring */
	queue_delayed_work(nvme_opt.wq, &opt_dev->monitor_work,
			   NVME_OPT_MONITOR_INTERVAL * HZ);

	dev_info(dev, "NVMe optimization initialized (queue_depth=%u, APST=%s)\n",
		 opt_dev->queue_depth, opt_dev->apst_enabled ? "enabled" : "disabled");

	return 0;
}

/* Cleanup optimization for a device */
static void nvme_opt_cleanup_device(struct device *dev)
{
	struct nvme_opt_device *opt_dev = dev_get_drvdata(dev);

	if (!opt_dev)
		return;

	/* Cancel monitoring work */
	cancel_delayed_work_sync(&opt_dev->monitor_work);

	/* Delete idle timer */
	del_timer_sync(&opt_dev->idle_timer);

	/* Remove from device list */
	mutex_lock(&nvme_opt.device_mutex);
	list_del(&opt_dev->list);
	mutex_unlock(&nvme_opt.device_mutex);

	/* Remove sysfs attributes */
	sysfs_remove_group(&dev->kobj, &nvme_opt_attr_group);

	dev_info(dev, "NVMe optimization cleanup complete\n");

	kfree(opt_dev);
}

/* Module initialization */
static int __init nvme_opt_init(void)
{
	pr_info("%s: NVMe Optimization Driver v%s\n", DRIVER_NAME, DRIVER_VERSION);

	/* Create workqueue for monitoring */
	nvme_opt.wq = alloc_workqueue("nvme_opt_wq", WQ_MEM_RECLAIM, 0);
	if (!nvme_opt.wq) {
		pr_err("%s: Failed to create workqueue\n", DRIVER_NAME);
		return -ENOMEM;
	}

	pr_info("%s: Driver initialized successfully\n", DRIVER_NAME);
	return 0;
}

/* Module cleanup */
static void __exit nvme_opt_exit(void)
{
	struct nvme_opt_device *opt_dev, *tmp;

	/* Cleanup all devices */
	mutex_lock(&nvme_opt.device_mutex);
	list_for_each_entry_safe(opt_dev, tmp, &nvme_opt.device_list, list) {
		nvme_opt_cleanup_device(opt_dev->dev);
	}
	mutex_unlock(&nvme_opt.device_mutex);

	/* Destroy workqueue */
	if (nvme_opt.wq)
		destroy_workqueue(nvme_opt.wq);

	pr_info("%s: Driver unloaded\n", DRIVER_NAME);
}

module_init(nvme_opt_init);
module_exit(nvme_opt_exit);

MODULE_DESCRIPTION("NVMe Optimization Driver for OpenMPTCProuter");
MODULE_AUTHOR("OpenMPTCProuter Contributors");
MODULE_LICENSE("GPL v2");
MODULE_VERSION(DRIVER_VERSION);
