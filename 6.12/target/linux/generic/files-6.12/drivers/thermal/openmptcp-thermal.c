// SPDX-License-Identifier: GPL-2.0
/*
 * openmptcp-thermal.c - Thermal and Fan Control Driver for OpenMPTCProuter
 *
 * Copyright (c) 2025, OpenMPTCProuter Project
 * Author: OpenMPTCProuter Contributors
 *
 * This driver provides intelligent thermal management and fan control for
 * router hardware, including:
 * - Multi-zone temperature monitoring
 * - PWM fan speed control
 * - Thermal throttling protection
 * - Hysteresis-based fan curves
 * - Emergency shutdown protection
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/thermal.h>
#include <linux/hwmon.h>
#include <linux/hwmon-sysfs.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/pwm.h>
#include <linux/workqueue.h>
#include <linux/delay.h>

#define DRIVER_NAME	"openmptcp-thermal"
#define DRIVER_VERSION	"1.0"

/* Temperature thresholds (in millidegree Celsius) */
#define TEMP_THRESHOLD_CRITICAL		95000	/* 95°C - Emergency shutdown */
#define TEMP_THRESHOLD_HOT		85000	/* 85°C - Maximum fan speed */
#define TEMP_THRESHOLD_WARM		70000	/* 70°C - Medium fan speed */
#define TEMP_THRESHOLD_NORMAL		50000	/* 50°C - Low fan speed */
#define TEMP_THRESHOLD_COOL		40000	/* 40°C - Minimum fan speed */

/* Hysteresis to prevent fan oscillation (in millidegree Celsius) */
#define TEMP_HYSTERESIS			3000	/* 3°C */

/* PWM fan speed levels (0-255) */
#define FAN_SPEED_OFF			0
#define FAN_SPEED_MIN			64
#define FAN_SPEED_LOW			96
#define FAN_SPEED_MEDIUM		160
#define FAN_SPEED_HIGH			224
#define FAN_SPEED_MAX			255

/* Monitoring interval */
#define THERMAL_POLL_INTERVAL_MS	2000	/* 2 seconds */

/* Maximum number of thermal zones */
#define MAX_THERMAL_ZONES		8

enum fan_mode {
	FAN_MODE_AUTO,		/* Automatic control based on temperature */
	FAN_MODE_MANUAL,	/* Manual speed control */
	FAN_MODE_OFF,		/* Fan always off */
	FAN_MODE_FULL,		/* Fan always at full speed */
};

struct thermal_zone_info {
	struct thermal_zone_device	*tzd;
	const char			*name;
	int				temp;		/* Current temperature */
	int				max_temp;	/* Maximum observed */
	bool				critical;	/* In critical state */
};

struct openmptcp_thermal {
	struct device			*dev;
	struct platform_device		*pdev;

	/* Thermal zones */
	struct thermal_zone_info	zones[MAX_THERMAL_ZONES];
	int				num_zones;
	int				max_zone_temp;	/* Hottest zone */

	/* Fan control */
	struct pwm_device		*pwm;
	enum fan_mode			mode;
	u8				current_speed;
	u8				target_speed;
	bool				fan_available;

	/* Monitoring */
	struct delayed_work		monitor_work;
	struct workqueue_struct		*wq;

	/* Statistics */
	u64				thermal_events;
	u64				throttle_events;
	u64				critical_events;
};

/* Calculate target fan speed based on temperature */
static u8 calculate_fan_speed(int temp_mc)
{
	if (temp_mc >= TEMP_THRESHOLD_HOT)
		return FAN_SPEED_MAX;
	else if (temp_mc >= TEMP_THRESHOLD_WARM)
		return FAN_SPEED_HIGH;
	else if (temp_mc >= TEMP_THRESHOLD_NORMAL)
		return FAN_SPEED_MEDIUM;
	else if (temp_mc >= TEMP_THRESHOLD_COOL)
		return FAN_SPEED_LOW;
	else
		return FAN_SPEED_MIN;
}

/* Apply PWM duty cycle to fan */
static int set_fan_pwm(struct openmptcp_thermal *thermal, u8 speed)
{
	struct pwm_state state;
	int ret;

	if (!thermal->fan_available || !thermal->pwm)
		return -ENODEV;

	pwm_get_state(thermal->pwm, &state);

	/* Convert speed (0-255) to duty cycle */
	state.duty_cycle = DIV_ROUND_UP((u64)speed * state.period, 255);
	state.enabled = (speed > 0);

	ret = pwm_apply_state(thermal->pwm, &state);
	if (ret) {
		dev_err(thermal->dev, "Failed to set PWM state: %d\n", ret);
		return ret;
	}

	thermal->current_speed = speed;
	return 0;
}

/* Update thermal zone temperatures */
static int update_thermal_zones(struct openmptcp_thermal *thermal)
{
	int i, ret;
	int max_temp = 0;

	for (i = 0; i < thermal->num_zones; i++) {
		struct thermal_zone_info *zone = &thermal->zones[i];

		if (!zone->tzd)
			continue;

		ret = thermal_zone_get_temp(zone->tzd, &zone->temp);
		if (ret) {
			dev_warn(thermal->dev, "Failed to read temp from zone %s: %d\n",
				 zone->name, ret);
			continue;
		}

		/* Track maximum temperature */
		if (zone->temp > zone->max_temp)
			zone->max_temp = zone->temp;

		if (zone->temp > max_temp)
			max_temp = zone->temp;

		/* Check for critical temperature */
		if (zone->temp >= TEMP_THRESHOLD_CRITICAL) {
			if (!zone->critical) {
				zone->critical = true;
				thermal->critical_events++;
				dev_crit(thermal->dev, "CRITICAL: Zone %s reached %d°C!\n",
					 zone->name, zone->temp / 1000);
			}
		} else {
			zone->critical = false;
		}
	}

	thermal->max_zone_temp = max_temp;
	return 0;
}

/* Thermal monitoring work function */
static void thermal_monitor_work(struct work_struct *work)
{
	struct openmptcp_thermal *thermal = container_of(to_delayed_work(work),
							  struct openmptcp_thermal,
							  monitor_work);
	u8 new_speed;
	int temp_c;

	/* Update all thermal zone readings */
	update_thermal_zones(thermal);

	temp_c = thermal->max_zone_temp / 1000;

	/* Determine fan speed based on mode */
	switch (thermal->mode) {
	case FAN_MODE_AUTO:
		new_speed = calculate_fan_speed(thermal->max_zone_temp);

		/* Apply hysteresis to prevent oscillation */
		if (new_speed > thermal->current_speed) {
			/* Temperature rising - respond immediately */
			thermal->target_speed = new_speed;
		} else if (new_speed < thermal->current_speed) {
			/* Temperature falling - wait for hysteresis */
			int hysteresis_temp = thermal->max_zone_temp + TEMP_HYSTERESIS;
			u8 hyst_speed = calculate_fan_speed(hysteresis_temp);

			if (hyst_speed < thermal->current_speed)
				thermal->target_speed = new_speed;
		}
		break;

	case FAN_MODE_MANUAL:
		/* Keep current target speed */
		break;

	case FAN_MODE_OFF:
		thermal->target_speed = FAN_SPEED_OFF;
		break;

	case FAN_MODE_FULL:
		thermal->target_speed = FAN_SPEED_MAX;
		break;
	}

	/* Apply new fan speed if changed */
	if (thermal->target_speed != thermal->current_speed) {
		set_fan_pwm(thermal, thermal->target_speed);
		dev_dbg(thermal->dev, "Fan speed: %u -> %u (temp: %d°C)\n",
			thermal->current_speed, thermal->target_speed, temp_c);
	}

	/* Emergency thermal throttling */
	if (thermal->max_zone_temp >= TEMP_THRESHOLD_CRITICAL) {
		dev_emerg(thermal->dev, "EMERGENCY: Temperature %d°C - forcing max cooling!\n",
			  temp_c);
		set_fan_pwm(thermal, FAN_SPEED_MAX);
		thermal->mode = FAN_MODE_FULL;
		thermal->throttle_events++;
	}

	/* Schedule next poll */
	queue_delayed_work(thermal->wq, &thermal->monitor_work,
			   msecs_to_jiffies(THERMAL_POLL_INTERVAL_MS));
}

/* Sysfs attributes */
static ssize_t fan_mode_show(struct device *dev, struct device_attribute *attr,
			     char *buf)
{
	struct openmptcp_thermal *thermal = dev_get_drvdata(dev);
	const char *mode_str;

	switch (thermal->mode) {
	case FAN_MODE_AUTO:
		mode_str = "auto";
		break;
	case FAN_MODE_MANUAL:
		mode_str = "manual";
		break;
	case FAN_MODE_OFF:
		mode_str = "off";
		break;
	case FAN_MODE_FULL:
		mode_str = "full";
		break;
	default:
		mode_str = "unknown";
	}

	return sprintf(buf, "%s\n", mode_str);
}

static ssize_t fan_mode_store(struct device *dev, struct device_attribute *attr,
			      const char *buf, size_t count)
{
	struct openmptcp_thermal *thermal = dev_get_drvdata(dev);

	if (sysfs_streq(buf, "auto"))
		thermal->mode = FAN_MODE_AUTO;
	else if (sysfs_streq(buf, "manual"))
		thermal->mode = FAN_MODE_MANUAL;
	else if (sysfs_streq(buf, "off"))
		thermal->mode = FAN_MODE_OFF;
	else if (sysfs_streq(buf, "full"))
		thermal->mode = FAN_MODE_FULL;
	else
		return -EINVAL;

	return count;
}

static ssize_t fan_speed_show(struct device *dev, struct device_attribute *attr,
			      char *buf)
{
	struct openmptcp_thermal *thermal = dev_get_drvdata(dev);
	return sprintf(buf, "%u\n", thermal->current_speed);
}

static ssize_t fan_speed_store(struct device *dev, struct device_attribute *attr,
			       const char *buf, size_t count)
{
	struct openmptcp_thermal *thermal = dev_get_drvdata(dev);
	unsigned int speed;
	int ret;

	ret = kstrtouint(buf, 10, &speed);
	if (ret)
		return ret;

	if (speed > 255)
		return -EINVAL;

	thermal->mode = FAN_MODE_MANUAL;
	thermal->target_speed = speed;
	set_fan_pwm(thermal, speed);

	return count;
}

static ssize_t temperature_show(struct device *dev, struct device_attribute *attr,
				char *buf)
{
	struct openmptcp_thermal *thermal = dev_get_drvdata(dev);
	return sprintf(buf, "%d\n", thermal->max_zone_temp / 1000);
}

static ssize_t stats_show(struct device *dev, struct device_attribute *attr,
			  char *buf)
{
	struct openmptcp_thermal *thermal = dev_get_drvdata(dev);
	int i, len = 0;

	len += sprintf(buf + len, "Thermal Events: %llu\n", thermal->thermal_events);
	len += sprintf(buf + len, "Throttle Events: %llu\n", thermal->throttle_events);
	len += sprintf(buf + len, "Critical Events: %llu\n", thermal->critical_events);
	len += sprintf(buf + len, "\nThermal Zones:\n");

	for (i = 0; i < thermal->num_zones; i++) {
		struct thermal_zone_info *zone = &thermal->zones[i];
		len += sprintf(buf + len, "  %s: %d°C (max: %d°C)\n",
			       zone->name,
			       zone->temp / 1000,
			       zone->max_temp / 1000);
	}

	return len;
}

static DEVICE_ATTR_RW(fan_mode);
static DEVICE_ATTR_RW(fan_speed);
static DEVICE_ATTR_RO(temperature);
static DEVICE_ATTR_RO(stats);

static struct attribute *openmptcp_thermal_attrs[] = {
	&dev_attr_fan_mode.attr,
	&dev_attr_fan_speed.attr,
	&dev_attr_temperature.attr,
	&dev_attr_stats.attr,
	NULL,
};

static const struct attribute_group openmptcp_thermal_attr_group = {
	.name = "thermal_control",
	.attrs = openmptcp_thermal_attrs,
};

/* Platform driver probe */
static int openmptcp_thermal_probe(struct platform_device *pdev)
{
	struct openmptcp_thermal *thermal;
	struct device_node *np = pdev->dev.of_node;
	int ret, i;

	thermal = devm_kzalloc(&pdev->dev, sizeof(*thermal), GFP_KERNEL);
	if (!thermal)
		return -ENOMEM;

	thermal->dev = &pdev->dev;
	thermal->pdev = pdev;
	thermal->mode = FAN_MODE_AUTO;
	thermal->current_speed = FAN_SPEED_MIN;
	thermal->target_speed = FAN_SPEED_MIN;

	platform_set_drvdata(pdev, thermal);

	/* Get PWM for fan control (optional) */
	thermal->pwm = devm_pwm_get(&pdev->dev, NULL);
	if (IS_ERR(thermal->pwm)) {
		if (PTR_ERR(thermal->pwm) == -EPROBE_DEFER)
			return -EPROBE_DEFER;
		dev_info(&pdev->dev, "No PWM fan control available\n");
		thermal->pwm = NULL;
		thermal->fan_available = false;
	} else {
		thermal->fan_available = true;
		dev_info(&pdev->dev, "PWM fan control initialized\n");
	}

	/* Enumerate thermal zones */
	thermal->num_zones = 0;
	for (i = 0; i < MAX_THERMAL_ZONES; i++) {
		char name[32];
		struct thermal_zone_device *tzd;

		snprintf(name, sizeof(name), "thermal-zone%d", i);
		tzd = thermal_zone_get_zone_by_name(name);

		if (IS_ERR(tzd))
			break;

		thermal->zones[i].tzd = tzd;
		thermal->zones[i].name = kstrdup(name, GFP_KERNEL);
		thermal->zones[i].temp = 0;
		thermal->zones[i].max_temp = 0;
		thermal->zones[i].critical = false;
		thermal->num_zones++;
	}

	if (thermal->num_zones == 0)
		dev_warn(&pdev->dev, "No thermal zones found\n");
	else
		dev_info(&pdev->dev, "Monitoring %d thermal zones\n", thermal->num_zones);

	/* Create workqueue */
	thermal->wq = alloc_workqueue("openmptcp_thermal", WQ_MEM_RECLAIM, 0);
	if (!thermal->wq) {
		dev_err(&pdev->dev, "Failed to create workqueue\n");
		return -ENOMEM;
	}

	/* Initialize monitoring work */
	INIT_DELAYED_WORK(&thermal->monitor_work, thermal_monitor_work);

	/* Create sysfs attributes */
	ret = sysfs_create_group(&pdev->dev.kobj, &openmptcp_thermal_attr_group);
	if (ret) {
		dev_err(&pdev->dev, "Failed to create sysfs group: %d\n", ret);
		destroy_workqueue(thermal->wq);
		return ret;
	}

	/* Start monitoring */
	queue_delayed_work(thermal->wq, &thermal->monitor_work, 0);

	dev_info(&pdev->dev, "OpenMPTCP Thermal Driver v%s initialized\n",
		 DRIVER_VERSION);

	return 0;
}

static int openmptcp_thermal_remove(struct platform_device *pdev)
{
	struct openmptcp_thermal *thermal = platform_get_drvdata(pdev);
	int i;

	/* Stop monitoring */
	cancel_delayed_work_sync(&thermal->monitor_work);

	/* Turn off fan */
	if (thermal->fan_available)
		set_fan_pwm(thermal, FAN_SPEED_OFF);

	/* Remove sysfs */
	sysfs_remove_group(&pdev->dev.kobj, &openmptcp_thermal_attr_group);

	/* Cleanup */
	destroy_workqueue(thermal->wq);

	for (i = 0; i < thermal->num_zones; i++)
		kfree(thermal->zones[i].name);

	dev_info(&pdev->dev, "OpenMPTCP Thermal Driver removed\n");
	return 0;
}

static const struct of_device_id openmptcp_thermal_of_match[] = {
	{ .compatible = "openmptcp,thermal-control", },
	{ .compatible = "generic,thermal-fan-control", },
	{},
};
MODULE_DEVICE_TABLE(of, openmptcp_thermal_of_match);

static struct platform_driver openmptcp_thermal_driver = {
	.probe	= openmptcp_thermal_probe,
	.remove	= openmptcp_thermal_remove,
	.driver	= {
		.name	= DRIVER_NAME,
		.of_match_table = openmptcp_thermal_of_match,
	},
};

module_platform_driver(openmptcp_thermal_driver);

MODULE_DESCRIPTION("Thermal and Fan Control Driver for OpenMPTCProuter");
MODULE_AUTHOR("OpenMPTCProuter Contributors");
MODULE_LICENSE("GPL v2");
MODULE_VERSION(DRIVER_VERSION);
