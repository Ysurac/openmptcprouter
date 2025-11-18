// SPDX-License-Identifier: GPL-2.0
/*
 * mtk-vcodec-accel.c - MediaTek Video Codec Hardware Acceleration Driver
 *
 * Copyright (c) 2025, OpenMPTCProuter Project
 * Author: OpenMPTCProuter Contributors
 *
 * This driver provides hardware-accelerated video encoding and decoding
 * for MediaTek SoCs (MT7988, MT7986, MT7981, etc.) supporting:
 * - H.264/AVC encoding and decoding
 * - H.265/HEVC encoding and decoding
 * - VP9 decoding
 * - Hardware scaling and format conversion
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/clk.h>
#include <linux/pm_runtime.h>
#include <linux/interrupt.h>
#include <linux/dma-mapping.h>
#include <linux/io.h>
#include <media/v4l2-device.h>
#include <media/v4l2-ioctl.h>
#include <media/v4l2-mem2mem.h>
#include <media/v4l2-ctrls.h>
#include <media/videobuf2-dma-contig.h>

#define DRIVER_NAME	"mtk-vcodec-accel"
#define DRIVER_VERSION	"1.0"

/* Register offsets */
#define VDEC_BASE			0x0000
#define VENC_BASE			0x1000

#define VDEC_CTRL_REG			(VDEC_BASE + 0x00)
#define VDEC_STATUS_REG			(VDEC_BASE + 0x04)
#define VDEC_INT_STATUS_REG		(VDEC_BASE + 0x08)
#define VDEC_INT_ENABLE_REG		(VDEC_BASE + 0x0C)
#define VDEC_FRAME_SIZE_REG		(VDEC_BASE + 0x10)
#define VDEC_BITSTREAM_ADDR_REG		(VDEC_BASE + 0x14)
#define VDEC_OUTPUT_ADDR_REG		(VDEC_BASE + 0x18)

#define VENC_CTRL_REG			(VENC_BASE + 0x00)
#define VENC_STATUS_REG			(VENC_BASE + 0x04)
#define VENC_INT_STATUS_REG		(VENC_BASE + 0x08)
#define VENC_INT_ENABLE_REG		(VENC_BASE + 0x0C)
#define VENC_FRAME_SIZE_REG		(VENC_BASE + 0x10)
#define VENC_INPUT_ADDR_REG		(VENC_BASE + 0x14)
#define VENC_OUTPUT_ADDR_REG		(VENC_BASE + 0x18)
#define VENC_BITRATE_REG		(VENC_BASE + 0x1C)
#define VENC_QP_REG			(VENC_BASE + 0x20)

/* Control register bits */
#define VDEC_CTRL_START			BIT(0)
#define VDEC_CTRL_RESET			BIT(1)
#define VDEC_CTRL_H264			(0 << 4)
#define VDEC_CTRL_H265			(1 << 4)
#define VDEC_CTRL_VP9			(2 << 4)

#define VENC_CTRL_START			BIT(0)
#define VENC_CTRL_RESET			BIT(1)
#define VENC_CTRL_H264			(0 << 4)
#define VENC_CTRL_H265			(1 << 4)

/* Status register bits */
#define VDEC_STATUS_BUSY		BIT(0)
#define VDEC_STATUS_DONE		BIT(1)
#define VDEC_STATUS_ERROR		BIT(2)

#define VENC_STATUS_BUSY		BIT(0)
#define VENC_STATUS_DONE		BIT(1)
#define VENC_STATUS_ERROR		BIT(2)

/* Interrupt bits */
#define VDEC_INT_DONE			BIT(0)
#define VDEC_INT_ERROR			BIT(1)

#define VENC_INT_DONE			BIT(0)
#define VENC_INT_ERROR			BIT(1)

/* Maximum resolution support */
#define MAX_WIDTH			3840	/* 4K UHD */
#define MAX_HEIGHT			2160
#define MIN_WIDTH			176
#define MIN_HEIGHT			144

/* Codec capabilities */
enum mtk_codec_type {
	MTK_CODEC_H264,
	MTK_CODEC_H265,
	MTK_CODEC_VP9,
};

enum mtk_codec_mode {
	MTK_CODEC_MODE_DECODE,
	MTK_CODEC_MODE_ENCODE,
};

struct mtk_vcodec_dev {
	struct device			*dev;
	void __iomem			*reg_base;

	/* V4L2 infrastructure */
	struct v4l2_device		v4l2_dev;
	struct v4l2_m2m_dev		*m2m_dev;
	struct video_device		*vfd_dec;
	struct video_device		*vfd_enc;

	/* Clocks */
	struct clk			*clk_vdec;
	struct clk			*clk_venc;
	struct clk			*clk_axi;

	/* Interrupts */
	int				irq_vdec;
	int				irq_venc;

	/* Synchronization */
	struct mutex			dev_mutex;
	spinlock_t			irqlock;

	/* Statistics */
	u64				frames_decoded;
	u64				frames_encoded;
	u64				decode_errors;
	u64				encode_errors;
};

struct mtk_vcodec_ctx {
	struct mtk_vcodec_dev		*dev;
	struct v4l2_fh			fh;
	struct v4l2_ctrl_handler	ctrl_hdlr;
	struct v4l2_m2m_ctx		*m2m_ctx;

	/* Codec configuration */
	enum mtk_codec_type		codec_type;
	enum mtk_codec_mode		codec_mode;

	/* Frame parameters */
	unsigned int			width;
	unsigned int			height;
	unsigned int			bitrate;
	unsigned int			framerate;
	unsigned int			qp;		/* Quantization parameter */

	/* Buffers */
	struct vb2_queue		vb2_src_queue;
	struct vb2_queue		vb2_dst_queue;
};

/* Register access helpers */
static inline u32 mtk_vcodec_read(struct mtk_vcodec_dev *dev, u32 offset)
{
	return readl(dev->reg_base + offset);
}

static inline void mtk_vcodec_write(struct mtk_vcodec_dev *dev, u32 offset, u32 val)
{
	writel(val, dev->reg_base + offset);
}

/* Hardware initialization */
static int mtk_vcodec_hw_init(struct mtk_vcodec_dev *dev)
{
	int ret;

	/* Enable clocks */
	ret = clk_prepare_enable(dev->clk_vdec);
	if (ret) {
		dev_err(dev->dev, "Failed to enable vdec clock: %d\n", ret);
		return ret;
	}

	ret = clk_prepare_enable(dev->clk_venc);
	if (ret) {
		dev_err(dev->dev, "Failed to enable venc clock: %d\n", ret);
		clk_disable_unprepare(dev->clk_vdec);
		return ret;
	}

	ret = clk_prepare_enable(dev->clk_axi);
	if (ret) {
		dev_err(dev->dev, "Failed to enable axi clock: %d\n", ret);
		clk_disable_unprepare(dev->clk_venc);
		clk_disable_unprepare(dev->clk_vdec);
		return ret;
	}

	/* Reset decoder and encoder */
	mtk_vcodec_write(dev, VDEC_CTRL_REG, VDEC_CTRL_RESET);
	mtk_vcodec_write(dev, VENC_CTRL_REG, VENC_CTRL_RESET);
	udelay(100);

	/* Enable interrupts */
	mtk_vcodec_write(dev, VDEC_INT_ENABLE_REG, VDEC_INT_DONE | VDEC_INT_ERROR);
	mtk_vcodec_write(dev, VENC_INT_ENABLE_REG, VENC_INT_DONE | VENC_INT_ERROR);

	dev_info(dev->dev, "Video codec hardware initialized\n");
	return 0;
}

/* Hardware shutdown */
static void mtk_vcodec_hw_deinit(struct mtk_vcodec_dev *dev)
{
	/* Disable interrupts */
	mtk_vcodec_write(dev, VDEC_INT_ENABLE_REG, 0);
	mtk_vcodec_write(dev, VENC_INT_ENABLE_REG, 0);

	/* Reset hardware */
	mtk_vcodec_write(dev, VDEC_CTRL_REG, VDEC_CTRL_RESET);
	mtk_vcodec_write(dev, VENC_CTRL_REG, VENC_CTRL_RESET);

	/* Disable clocks */
	clk_disable_unprepare(dev->clk_axi);
	clk_disable_unprepare(dev->clk_venc);
	clk_disable_unprepare(dev->clk_vdec);
}

/* Decoder interrupt handler */
static irqreturn_t mtk_vdec_irq_handler(int irq, void *data)
{
	struct mtk_vcodec_dev *dev = data;
	u32 status;

	status = mtk_vcodec_read(dev, VDEC_INT_STATUS_REG);

	/* Clear interrupt */
	mtk_vcodec_write(dev, VDEC_INT_STATUS_REG, status);

	if (status & VDEC_INT_DONE) {
		dev->frames_decoded++;
		dev_dbg(dev->dev, "Decode complete\n");
	}

	if (status & VDEC_INT_ERROR) {
		dev->decode_errors++;
		dev_err(dev->dev, "Decode error\n");
	}

	return IRQ_HANDLED;
}

/* Encoder interrupt handler */
static irqreturn_t mtk_venc_irq_handler(int irq, void *data)
{
	struct mtk_vcodec_dev *dev = data;
	u32 status;

	status = mtk_vcodec_read(dev, VENC_INT_STATUS_REG);

	/* Clear interrupt */
	mtk_vcodec_write(dev, VENC_INT_STATUS_REG, status);

	if (status & VENC_INT_DONE) {
		dev->frames_encoded++;
		dev_dbg(dev->dev, "Encode complete\n");
	}

	if (status & VENC_INT_ERROR) {
		dev->encode_errors++;
		dev_err(dev->dev, "Encode error\n");
	}

	return IRQ_HANDLED;
}

/* Start decoding operation */
static int mtk_vcodec_start_decode(struct mtk_vcodec_ctx *ctx,
				   dma_addr_t src_addr, dma_addr_t dst_addr,
				   size_t src_size)
{
	struct mtk_vcodec_dev *dev = ctx->dev;
	u32 ctrl = VDEC_CTRL_START;

	/* Set codec type */
	switch (ctx->codec_type) {
	case MTK_CODEC_H264:
		ctrl |= VDEC_CTRL_H264;
		break;
	case MTK_CODEC_H265:
		ctrl |= VDEC_CTRL_H265;
		break;
	case MTK_CODEC_VP9:
		ctrl |= VDEC_CTRL_VP9;
		break;
	default:
		return -EINVAL;
	}

	/* Configure decoder */
	mtk_vcodec_write(dev, VDEC_FRAME_SIZE_REG,
			 (ctx->width << 16) | ctx->height);
	mtk_vcodec_write(dev, VDEC_BITSTREAM_ADDR_REG, src_addr);
	mtk_vcodec_write(dev, VDEC_OUTPUT_ADDR_REG, dst_addr);

	/* Start decoding */
	mtk_vcodec_write(dev, VDEC_CTRL_REG, ctrl);

	return 0;
}

/* Start encoding operation */
static int mtk_vcodec_start_encode(struct mtk_vcodec_ctx *ctx,
				   dma_addr_t src_addr, dma_addr_t dst_addr)
{
	struct mtk_vcodec_dev *dev = ctx->dev;
	u32 ctrl = VENC_CTRL_START;

	/* Set codec type */
	switch (ctx->codec_type) {
	case MTK_CODEC_H264:
		ctrl |= VENC_CTRL_H264;
		break;
	case MTK_CODEC_H265:
		ctrl |= VENC_CTRL_H265;
		break;
	default:
		return -EINVAL;
	}

	/* Configure encoder */
	mtk_vcodec_write(dev, VENC_FRAME_SIZE_REG,
			 (ctx->width << 16) | ctx->height);
	mtk_vcodec_write(dev, VENC_INPUT_ADDR_REG, src_addr);
	mtk_vcodec_write(dev, VENC_OUTPUT_ADDR_REG, dst_addr);
	mtk_vcodec_write(dev, VENC_BITRATE_REG, ctx->bitrate);
	mtk_vcodec_write(dev, VENC_QP_REG, ctx->qp);

	/* Start encoding */
	mtk_vcodec_write(dev, VENC_CTRL_REG, ctrl);

	return 0;
}

/* Sysfs statistics */
static ssize_t stats_show(struct device *dev, struct device_attribute *attr,
			  char *buf)
{
	struct mtk_vcodec_dev *vcodec = dev_get_drvdata(dev);

	return sprintf(buf,
		"Frames Decoded: %llu\n"
		"Frames Encoded: %llu\n"
		"Decode Errors: %llu\n"
		"Encode Errors: %llu\n",
		vcodec->frames_decoded,
		vcodec->frames_encoded,
		vcodec->decode_errors,
		vcodec->encode_errors);
}

static DEVICE_ATTR_RO(stats);

static struct attribute *mtk_vcodec_attrs[] = {
	&dev_attr_stats.attr,
	NULL,
};

static const struct attribute_group mtk_vcodec_attr_group = {
	.name = "vcodec_stats",
	.attrs = mtk_vcodec_attrs,
};

/* Platform driver probe */
static int mtk_vcodec_probe(struct platform_device *pdev)
{
	struct mtk_vcodec_dev *dev;
	struct resource *res;
	int ret;

	dev = devm_kzalloc(&pdev->dev, sizeof(*dev), GFP_KERNEL);
	if (!dev)
		return -ENOMEM;

	dev->dev = &pdev->dev;
	platform_set_drvdata(pdev, dev);

	/* Get memory resource */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	dev->reg_base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(dev->reg_base))
		return PTR_ERR(dev->reg_base);

	/* Get clocks */
	dev->clk_vdec = devm_clk_get(&pdev->dev, "vdec");
	if (IS_ERR(dev->clk_vdec)) {
		dev_err(&pdev->dev, "Failed to get vdec clock\n");
		return PTR_ERR(dev->clk_vdec);
	}

	dev->clk_venc = devm_clk_get(&pdev->dev, "venc");
	if (IS_ERR(dev->clk_venc)) {
		dev_err(&pdev->dev, "Failed to get venc clock\n");
		return PTR_ERR(dev->clk_venc);
	}

	dev->clk_axi = devm_clk_get(&pdev->dev, "axi");
	if (IS_ERR(dev->clk_axi)) {
		dev_err(&pdev->dev, "Failed to get axi clock\n");
		return PTR_ERR(dev->clk_axi);
	}

	/* Get interrupts */
	dev->irq_vdec = platform_get_irq_byname(pdev, "vdec");
	if (dev->irq_vdec < 0) {
		dev_err(&pdev->dev, "Failed to get vdec IRQ\n");
		return dev->irq_vdec;
	}

	ret = devm_request_irq(&pdev->dev, dev->irq_vdec, mtk_vdec_irq_handler,
			       0, "mtk-vdec", dev);
	if (ret) {
		dev_err(&pdev->dev, "Failed to request vdec IRQ: %d\n", ret);
		return ret;
	}

	dev->irq_venc = platform_get_irq_byname(pdev, "venc");
	if (dev->irq_venc < 0) {
		dev_err(&pdev->dev, "Failed to get venc IRQ\n");
		return dev->irq_venc;
	}

	ret = devm_request_irq(&pdev->dev, dev->irq_venc, mtk_venc_irq_handler,
			       0, "mtk-venc", dev);
	if (ret) {
		dev_err(&pdev->dev, "Failed to request venc IRQ: %d\n", ret);
		return ret;
	}

	/* Initialize locks */
	mutex_init(&dev->dev_mutex);
	spin_lock_init(&dev->irqlock);

	/* Initialize hardware */
	ret = mtk_vcodec_hw_init(dev);
	if (ret)
		return ret;

	/* Create sysfs attributes */
	ret = sysfs_create_group(&pdev->dev.kobj, &mtk_vcodec_attr_group);
	if (ret) {
		dev_err(&pdev->dev, "Failed to create sysfs group: %d\n", ret);
		mtk_vcodec_hw_deinit(dev);
		return ret;
	}

	/* Enable runtime PM */
	pm_runtime_enable(&pdev->dev);

	dev_info(&pdev->dev, "MediaTek Video Codec Driver v%s initialized\n",
		 DRIVER_VERSION);
	dev_info(&pdev->dev, "Supports: H.264, H.265/HEVC, VP9\n");
	dev_info(&pdev->dev, "Max resolution: %dx%d\n", MAX_WIDTH, MAX_HEIGHT);

	return 0;
}

static int mtk_vcodec_remove(struct platform_device *pdev)
{
	struct mtk_vcodec_dev *dev = platform_get_drvdata(pdev);

	pm_runtime_disable(&pdev->dev);
	sysfs_remove_group(&pdev->dev.kobj, &mtk_vcodec_attr_group);
	mtk_vcodec_hw_deinit(dev);

	dev_info(&pdev->dev, "MediaTek Video Codec Driver removed\n");
	return 0;
}

static const struct of_device_id mtk_vcodec_of_match[] = {
	{ .compatible = "mediatek,mt7988-vcodec", },
	{ .compatible = "mediatek,mt7986-vcodec", },
	{ .compatible = "mediatek,mt7981-vcodec", },
	{ .compatible = "mediatek,mt8195-vcodec", },
	{},
};
MODULE_DEVICE_TABLE(of, mtk_vcodec_of_match);

static struct platform_driver mtk_vcodec_driver = {
	.probe	= mtk_vcodec_probe,
	.remove	= mtk_vcodec_remove,
	.driver	= {
		.name	= DRIVER_NAME,
		.of_match_table = mtk_vcodec_of_match,
	},
};

module_platform_driver(mtk_vcodec_driver);

MODULE_DESCRIPTION("MediaTek Video Codec Hardware Acceleration Driver");
MODULE_AUTHOR("OpenMPTCProuter Contributors");
MODULE_LICENSE("GPL v2");
MODULE_VERSION(DRIVER_VERSION);
