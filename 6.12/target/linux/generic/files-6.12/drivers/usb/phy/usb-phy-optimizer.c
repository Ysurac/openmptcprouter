// SPDX-License-Identifier: GPL-2.0
/*
 * usb-phy-optimizer.c - USB PHY Optimization Driver for OpenMPTCProuter
 *
 * Copyright (c) 2025, OpenMPTCProuter Project
 * Author: OpenMPTCProuter Contributors
 *
 * This driver provides USB PHY optimizations for router platforms including:
 * - Automatic PHY tuning for different device types
 * - Power optimization for USB modems
 * - Signal integrity improvements
 * - Enhanced compatibility with cellular modems
 * - USB 2.0 and USB 3.0 support
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/usb.h>
#include <linux/usb/phy.h>
#include <linux/usb/otg.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/clk.h>
#include <linux/reset.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/workqueue.h>

#define DRIVER_NAME	"usb-phy-optimizer"
#define DRIVER_VERSION	"1.0"

/* PHY Register offsets (generic USB PHY layout) */
#define PHY_CTRL_REG			0x00
#define PHY_STATUS_REG			0x04
#define PHY_TUNE_REG			0x08
#define PHY_DEBUG_REG			0x0C

/* USB 2.0 PHY tuning registers */
#define USB2_PHY_TUNE_HSDISCHARGE	0x10
#define USB2_PHY_TUNE_SQUELCH		0x14
#define USB2_PHY_TUNE_TXPREEMPH		0x18
#define USB2_PHY_TUNE_TXRISE		0x1C
#define USB2_PHY_TUNE_TXVREF		0x20
#define USB2_PHY_TUNE_TXFSLSTUNE	0x24

/* USB 3.0 PHY tuning registers */
#define USB3_PHY_TUNE_AMPLITUDE		0x30
#define USB3_PHY_TUNE_DEEMPHASIS	0x34
#define USB3_PHY_TUNE_RXEQ		0x38
#define USB3_PHY_TUNE_LOOPBACK		0x3C

/* PHY Control bits */
#define PHY_CTRL_RESET			BIT(0)
#define PHY_CTRL_ENABLE			BIT(1)
#define PHY_CTRL_SUSPEND		BIT(2)
#define PHY_CTRL_USB2_ENABLE		BIT(4)
#define PHY_CTRL_USB3_ENABLE		BIT(5)

/* PHY Status bits */
#define PHY_STATUS_READY		BIT(0)
#define PHY_STATUS_USB2_ACTIVE		BIT(1)
#define PHY_STATUS_USB3_ACTIVE		BIT(2)

/* Tuning profiles for different device types */
enum usb_device_profile {
	PROFILE_DEFAULT,
	PROFILE_MODEM_4G,
	PROFILE_MODEM_5G,
	PROFILE_STORAGE_HDD,
	PROFILE_STORAGE_SSD,
	PROFILE_HUB,
};

/* USB PHY optimization parameters */
struct phy_tune_params {
	/* USB 2.0 tuning */
	u8	squelch_threshold;	/* 0-7: Signal detection threshold */
	u8	tx_preemphasis;		/* 0-3: Pre-emphasis amplitude */
	u8	tx_rise_fall;		/* 0-3: Rise/fall time tuning */
	u8	tx_vref;		/* 0-15: Reference voltage */
	u8	hs_disconnect;		/* 0-7: Disconnect threshold */

	/* USB 3.0 tuning */
	u8	tx_amplitude;		/* 0-127: TX swing amplitude */
	u8	tx_deemphasis;		/* 0-63: De-emphasis level */
	u8	rx_equalization;	/* 0-7: RX equalizer setting */

	/* Power optimization */
	bool	low_power_mode;		/* Enable power saving */
	u16	idle_timeout_ms;	/* Suspend timeout */
};

struct usb_phy_optimizer {
	struct device			*dev;
	struct usb_phy			*phy;
	void __iomem			*base;

	/* Clocks and resets */
	struct clk			*clk_phy;
	struct clk			*clk_ref;
	struct reset_control		*reset;

	/* Current configuration */
	enum usb_device_profile		profile;
	struct phy_tune_params		tune;
	bool				usb3_enabled;
	bool				usb2_enabled;

	/* Device tracking */
	struct notifier_block		usb_nb;
	struct work_struct		tune_work;
	struct usb_device		*current_dev;

	/* Statistics */
	u64				usb2_devices;
	u64				usb3_devices;
	u64				modem_devices;
	u64				storage_devices;
	u64				tune_events;
};

/* Predefined tuning profiles */
static const struct phy_tune_params profile_configs[] = {
	[PROFILE_DEFAULT] = {
		.squelch_threshold	= 4,
		.tx_preemphasis		= 1,
		.tx_rise_fall		= 2,
		.tx_vref		= 8,
		.hs_disconnect		= 4,
		.tx_amplitude		= 90,
		.tx_deemphasis		= 20,
		.rx_equalization	= 3,
		.low_power_mode		= false,
		.idle_timeout_ms	= 3000,
	},
	[PROFILE_MODEM_4G] = {
		.squelch_threshold	= 5,
		.tx_preemphasis		= 2,
		.tx_rise_fall		= 2,
		.tx_vref		= 10,
		.hs_disconnect		= 5,
		.tx_amplitude		= 100,
		.tx_deemphasis		= 15,
		.rx_equalization	= 4,
		.low_power_mode		= false,	/* Keep active for modem */
		.idle_timeout_ms	= 0,		/* Never suspend */
	},
	[PROFILE_MODEM_5G] = {
		.squelch_threshold	= 6,
		.tx_preemphasis		= 3,
		.tx_rise_fall		= 1,
		.tx_vref		= 12,
		.hs_disconnect		= 6,
		.tx_amplitude		= 110,
		.tx_deemphasis		= 25,
		.rx_equalization	= 5,
		.low_power_mode		= false,	/* Keep active for modem */
		.idle_timeout_ms	= 0,		/* Never suspend */
	},
	[PROFILE_STORAGE_HDD] = {
		.squelch_threshold	= 4,
		.tx_preemphasis		= 1,
		.tx_rise_fall		= 2,
		.tx_vref		= 9,
		.hs_disconnect		= 4,
		.tx_amplitude		= 95,
		.tx_deemphasis		= 18,
		.rx_equalization	= 3,
		.low_power_mode		= true,
		.idle_timeout_ms	= 5000,
	},
	[PROFILE_STORAGE_SSD] = {
		.squelch_threshold	= 5,
		.tx_preemphasis		= 2,
		.tx_rise_fall		= 1,
		.tx_vref		= 10,
		.hs_disconnect		= 5,
		.tx_amplitude		= 105,
		.tx_deemphasis		= 22,
		.rx_equalization	= 4,
		.low_power_mode		= true,
		.idle_timeout_ms	= 10000,
	},
	[PROFILE_HUB] = {
		.squelch_threshold	= 4,
		.tx_preemphasis		= 1,
		.tx_rise_fall		= 2,
		.tx_vref		= 8,
		.hs_disconnect		= 4,
		.tx_amplitude		= 92,
		.tx_deemphasis		= 20,
		.rx_equalization	= 3,
		.low_power_mode		= false,
		.idle_timeout_ms	= 1000,
	},
};

/* Register access helpers */
static inline u32 phy_read(struct usb_phy_optimizer *opt, u32 offset)
{
	return readl(opt->base + offset);
}

static inline void phy_write(struct usb_phy_optimizer *opt, u32 offset, u32 val)
{
	writel(val, opt->base + offset);
}

/* Apply PHY tuning parameters */
static int apply_phy_tuning(struct usb_phy_optimizer *opt)
{
	struct phy_tune_params *tune = &opt->tune;

	dev_info(opt->dev, "Applying PHY tuning for profile %d\n", opt->profile);

	/* USB 2.0 PHY tuning */
	if (opt->usb2_enabled) {
		phy_write(opt, USB2_PHY_TUNE_SQUELCH, tune->squelch_threshold);
		phy_write(opt, USB2_PHY_TUNE_TXPREEMPH, tune->tx_preemphasis);
		phy_write(opt, USB2_PHY_TUNE_TXRISE, tune->tx_rise_fall);
		phy_write(opt, USB2_PHY_TUNE_TXVREF, tune->tx_vref);
		phy_write(opt, USB2_PHY_TUNE_HSDISCHARGE, tune->hs_disconnect);
	}

	/* USB 3.0 PHY tuning */
	if (opt->usb3_enabled) {
		phy_write(opt, USB3_PHY_TUNE_AMPLITUDE, tune->tx_amplitude);
		phy_write(opt, USB3_PHY_TUNE_DEEMPHASIS, tune->tx_deemphasis);
		phy_write(opt, USB3_PHY_TUNE_RXEQ, tune->rx_equalization);
	}

	opt->tune_events++;

	return 0;
}

/* Detect device type and select appropriate profile */
static enum usb_device_profile detect_device_profile(struct usb_device *udev)
{
	/* Check for cellular modems */
	if (udev->descriptor.bDeviceClass == USB_CLASS_COMM ||
	    udev->descriptor.bDeviceClass == USB_CLASS_VENDOR_SPEC) {
		/* Heuristic: 5G modems typically support USB 3.0 */
		if (udev->speed >= USB_SPEED_SUPER)
			return PROFILE_MODEM_5G;
		else
			return PROFILE_MODEM_4G;
	}

	/* Check for storage devices */
	if (udev->descriptor.bDeviceClass == USB_CLASS_MASS_STORAGE) {
		/* Heuristic: SSDs typically support USB 3.0 */
		if (udev->speed >= USB_SPEED_SUPER)
			return PROFILE_STORAGE_SSD;
		else
			return PROFILE_STORAGE_HDD;
	}

	/* Check for hubs */
	if (udev->descriptor.bDeviceClass == USB_CLASS_HUB)
		return PROFILE_HUB;

	return PROFILE_DEFAULT;
}

/* USB device notification handler */
static void usb_tune_work(struct work_struct *work)
{
	struct usb_phy_optimizer *opt = container_of(work,
						      struct usb_phy_optimizer,
						      tune_work);
	enum usb_device_profile new_profile;

	if (!opt->current_dev)
		return;

	/* Detect device type */
	new_profile = detect_device_profile(opt->current_dev);

	/* Update statistics */
	if (opt->current_dev->speed >= USB_SPEED_SUPER)
		opt->usb3_devices++;
	else
		opt->usb2_devices++;

	switch (new_profile) {
	case PROFILE_MODEM_4G:
	case PROFILE_MODEM_5G:
		opt->modem_devices++;
		break;
	case PROFILE_STORAGE_HDD:
	case PROFILE_STORAGE_SSD:
		opt->storage_devices++;
		break;
	default:
		break;
	}

	/* Apply new profile if different */
	if (new_profile != opt->profile) {
		opt->profile = new_profile;
		opt->tune = profile_configs[new_profile];
		apply_phy_tuning(opt);

		dev_info(opt->dev, "Switched to profile %d for device %04x:%04x\n",
			 new_profile,
			 le16_to_cpu(opt->current_dev->descriptor.idVendor),
			 le16_to_cpu(opt->current_dev->descriptor.idProduct));
	}
}

static int usb_phy_notifier(struct notifier_block *nb, unsigned long action,
			    void *data)
{
	struct usb_phy_optimizer *opt = container_of(nb,
						      struct usb_phy_optimizer,
						      usb_nb);
	struct usb_device *udev = data;

	switch (action) {
	case USB_DEVICE_ADD:
		opt->current_dev = udev;
		schedule_work(&opt->tune_work);
		break;

	case USB_DEVICE_REMOVE:
		opt->current_dev = NULL;
		/* Revert to default profile */
		opt->profile = PROFILE_DEFAULT;
		opt->tune = profile_configs[PROFILE_DEFAULT];
		apply_phy_tuning(opt);
		break;
	}

	return NOTIFY_OK;
}

/* Sysfs attributes */
static ssize_t profile_show(struct device *dev, struct device_attribute *attr,
			    char *buf)
{
	struct usb_phy_optimizer *opt = dev_get_drvdata(dev);
	const char *profile_names[] = {
		"default", "modem_4g", "modem_5g",
		"storage_hdd", "storage_ssd", "hub"
	};

	if (opt->profile < ARRAY_SIZE(profile_names))
		return sprintf(buf, "%s\n", profile_names[opt->profile]);

	return sprintf(buf, "unknown\n");
}

static ssize_t stats_show(struct device *dev, struct device_attribute *attr,
			  char *buf)
{
	struct usb_phy_optimizer *opt = dev_get_drvdata(dev);

	return sprintf(buf,
		"USB 2.0 Devices: %llu\n"
		"USB 3.0 Devices: %llu\n"
		"Modem Devices: %llu\n"
		"Storage Devices: %llu\n"
		"Tune Events: %llu\n"
		"Current Profile: %d\n"
		"USB3 Enabled: %s\n"
		"USB2 Enabled: %s\n",
		opt->usb2_devices,
		opt->usb3_devices,
		opt->modem_devices,
		opt->storage_devices,
		opt->tune_events,
		opt->profile,
		opt->usb3_enabled ? "yes" : "no",
		opt->usb2_enabled ? "yes" : "no");
}

static DEVICE_ATTR_RO(profile);
static DEVICE_ATTR_RO(stats);

static struct attribute *usb_phy_optimizer_attrs[] = {
	&dev_attr_profile.attr,
	&dev_attr_stats.attr,
	NULL,
};

static const struct attribute_group usb_phy_optimizer_attr_group = {
	.name = "phy_optimization",
	.attrs = usb_phy_optimizer_attrs,
};

/* Initialize PHY hardware */
static int usb_phy_hw_init(struct usb_phy_optimizer *opt)
{
	u32 ctrl;
	int ret;

	/* Enable clocks */
	ret = clk_prepare_enable(opt->clk_phy);
	if (ret) {
		dev_err(opt->dev, "Failed to enable PHY clock: %d\n", ret);
		return ret;
	}

	ret = clk_prepare_enable(opt->clk_ref);
	if (ret) {
		dev_err(opt->dev, "Failed to enable reference clock: %d\n", ret);
		clk_disable_unprepare(opt->clk_phy);
		return ret;
	}

	/* Reset PHY */
	reset_control_assert(opt->reset);
	udelay(10);
	reset_control_deassert(opt->reset);
	udelay(100);

	/* Enable PHY */
	ctrl = PHY_CTRL_ENABLE;
	if (opt->usb2_enabled)
		ctrl |= PHY_CTRL_USB2_ENABLE;
	if (opt->usb3_enabled)
		ctrl |= PHY_CTRL_USB3_ENABLE;

	phy_write(opt, PHY_CTRL_REG, ctrl);

	/* Wait for PHY ready */
	ret = readl_poll_timeout(opt->base + PHY_STATUS_REG, ctrl,
				 ctrl & PHY_STATUS_READY, 10, 10000);
	if (ret) {
		dev_err(opt->dev, "PHY failed to become ready\n");
		return ret;
	}

	/* Apply default tuning */
	opt->profile = PROFILE_DEFAULT;
	opt->tune = profile_configs[PROFILE_DEFAULT];
	apply_phy_tuning(opt);

	dev_info(opt->dev, "USB PHY initialized (USB2: %s, USB3: %s)\n",
		 opt->usb2_enabled ? "enabled" : "disabled",
		 opt->usb3_enabled ? "enabled" : "disabled");

	return 0;
}

/* Platform driver probe */
static int usb_phy_optimizer_probe(struct platform_device *pdev)
{
	struct usb_phy_optimizer *opt;
	struct resource *res;
	int ret;

	opt = devm_kzalloc(&pdev->dev, sizeof(*opt), GFP_KERNEL);
	if (!opt)
		return -ENOMEM;

	opt->dev = &pdev->dev;
	platform_set_drvdata(pdev, opt);

	/* Get memory resource */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	opt->base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(opt->base))
		return PTR_ERR(opt->base);

	/* Get clocks */
	opt->clk_phy = devm_clk_get(&pdev->dev, "phy");
	if (IS_ERR(opt->clk_phy)) {
		dev_err(&pdev->dev, "Failed to get PHY clock\n");
		return PTR_ERR(opt->clk_phy);
	}

	opt->clk_ref = devm_clk_get(&pdev->dev, "ref");
	if (IS_ERR(opt->clk_ref)) {
		dev_err(&pdev->dev, "Failed to get reference clock\n");
		return PTR_ERR(opt->clk_ref);
	}

	/* Get reset control */
	opt->reset = devm_reset_control_get(&pdev->dev, "phy");
	if (IS_ERR(opt->reset)) {
		dev_err(&pdev->dev, "Failed to get reset control\n");
		return PTR_ERR(opt->reset);
	}

	/* Check USB 2.0/3.0 support from device tree */
	opt->usb2_enabled = of_property_read_bool(pdev->dev.of_node, "usb2-enable");
	opt->usb3_enabled = of_property_read_bool(pdev->dev.of_node, "usb3-enable");

	/* Default to USB 2.0 if nothing specified */
	if (!opt->usb2_enabled && !opt->usb3_enabled)
		opt->usb2_enabled = true;

	/* Initialize work */
	INIT_WORK(&opt->tune_work, usb_tune_work);

	/* Initialize hardware */
	ret = usb_phy_hw_init(opt);
	if (ret)
		return ret;

	/* Register USB notifier */
	opt->usb_nb.notifier_call = usb_phy_notifier;
	usb_register_notify(&opt->usb_nb);

	/* Create sysfs attributes */
	ret = sysfs_create_group(&pdev->dev.kobj, &usb_phy_optimizer_attr_group);
	if (ret) {
		dev_err(&pdev->dev, "Failed to create sysfs group: %d\n", ret);
		usb_unregister_notify(&opt->usb_nb);
		return ret;
	}

	dev_info(&pdev->dev, "USB PHY Optimizer v%s initialized\n", DRIVER_VERSION);
	dev_info(&pdev->dev, "Automatic tuning for modems and storage devices\n");

	return 0;
}

static int usb_phy_optimizer_remove(struct platform_device *pdev)
{
	struct usb_phy_optimizer *opt = platform_get_drvdata(pdev);

	sysfs_remove_group(&pdev->dev.kobj, &usb_phy_optimizer_attr_group);
	usb_unregister_notify(&opt->usb_nb);
	cancel_work_sync(&opt->tune_work);

	/* Disable PHY */
	phy_write(opt, PHY_CTRL_REG, 0);

	clk_disable_unprepare(opt->clk_ref);
	clk_disable_unprepare(opt->clk_phy);

	dev_info(&pdev->dev, "USB PHY Optimizer removed\n");
	return 0;
}

static const struct of_device_id usb_phy_optimizer_of_match[] = {
	{ .compatible = "generic,usb-phy-optimizer", },
	{ .compatible = "openmptcp,usb-phy-optimizer", },
	{},
};
MODULE_DEVICE_TABLE(of, usb_phy_optimizer_of_match);

static struct platform_driver usb_phy_optimizer_driver = {
	.probe	= usb_phy_optimizer_probe,
	.remove	= usb_phy_optimizer_remove,
	.driver	= {
		.name	= DRIVER_NAME,
		.of_match_table = usb_phy_optimizer_of_match,
	},
};

module_platform_driver(usb_phy_optimizer_driver);

MODULE_DESCRIPTION("USB PHY Optimization Driver for OpenMPTCProuter");
MODULE_AUTHOR("OpenMPTCProuter Contributors");
MODULE_LICENSE("GPL v2");
MODULE_VERSION(DRIVER_VERSION);
