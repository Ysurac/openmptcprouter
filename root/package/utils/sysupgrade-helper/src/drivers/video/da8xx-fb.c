/*
 * Porting to u-boot:
 *
 * (C) Copyright 2011
 * Stefano Babic, DENX Software Engineering, sbabic@denx.de.
 *
 * Copyright (C) 2008-2009 MontaVista Software Inc.
 * Copyright (C) 2008-2009 Texas Instruments Inc
 *
 * Based on the LCD driver for TI Avalanche processors written by
 * Ajay Singh and Shalom Hai.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option)any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <common.h>
#include <malloc.h>
#include <video_fb.h>
#include <linux/list.h>
#include <linux/fb.h>

#include <asm/errno.h>
#include <asm/io.h>
#include <asm/arch/hardware.h>

#include "videomodes.h"
#include <asm/arch/da8xx-fb.h>

#define DRIVER_NAME "da8xx_lcdc"

/* LCD Status Register */
#define LCD_END_OF_FRAME1		(1 << 9)
#define LCD_END_OF_FRAME0		(1 << 8)
#define LCD_PL_LOAD_DONE		(1 << 6)
#define LCD_FIFO_UNDERFLOW		(1 << 5)
#define LCD_SYNC_LOST			(1 << 2)

/* LCD DMA Control Register */
#define LCD_DMA_BURST_SIZE(x)		((x) << 4)
#define LCD_DMA_BURST_1			0x0
#define LCD_DMA_BURST_2			0x1
#define LCD_DMA_BURST_4			0x2
#define LCD_DMA_BURST_8			0x3
#define LCD_DMA_BURST_16		0x4
#define LCD_END_OF_FRAME_INT_ENA	(1 << 2)
#define LCD_DUAL_FRAME_BUFFER_ENABLE	(1 << 0)

/* LCD Control Register */
#define LCD_CLK_DIVISOR(x)		((x) << 8)
#define LCD_RASTER_MODE			0x01

/* LCD Raster Control Register */
#define LCD_PALETTE_LOAD_MODE(x)	((x) << 20)
#define PALETTE_AND_DATA		0x00
#define PALETTE_ONLY			0x01
#define DATA_ONLY			0x02

#define LCD_MONO_8BIT_MODE		(1 << 9)
#define LCD_RASTER_ORDER		(1 << 8)
#define LCD_TFT_MODE			(1 << 7)
#define LCD_UNDERFLOW_INT_ENA		(1 << 6)
#define LCD_PL_ENABLE			(1 << 4)
#define LCD_MONOCHROME_MODE		(1 << 1)
#define LCD_RASTER_ENABLE		(1 << 0)
#define LCD_TFT_ALT_ENABLE		(1 << 23)
#define LCD_STN_565_ENABLE		(1 << 24)

/* LCD Raster Timing 2 Register */
#define LCD_AC_BIAS_TRANSITIONS_PER_INT(x)	((x) << 16)
#define LCD_AC_BIAS_FREQUENCY(x)		((x) << 8)
#define LCD_SYNC_CTRL				(1 << 25)
#define LCD_SYNC_EDGE				(1 << 24)
#define LCD_INVERT_PIXEL_CLOCK			(1 << 22)
#define LCD_INVERT_LINE_CLOCK			(1 << 21)
#define LCD_INVERT_FRAME_CLOCK			(1 << 20)

/* LCD Block */
struct da8xx_lcd_regs {
	u32	revid;
	u32	ctrl;
	u32	stat;
	u32	lidd_ctrl;
	u32	lidd_cs0_conf;
	u32	lidd_cs0_addr;
	u32	lidd_cs0_data;
	u32	lidd_cs1_conf;
	u32	lidd_cs1_addr;
	u32	lidd_cs1_data;
	u32	raster_ctrl;
	u32	raster_timing_0;
	u32	raster_timing_1;
	u32	raster_timing_2;
	u32	raster_subpanel;
	u32	reserved;
	u32	dma_ctrl;
	u32	dma_frm_buf_base_addr_0;
	u32	dma_frm_buf_ceiling_addr_0;
	u32	dma_frm_buf_base_addr_1;
	u32	dma_frm_buf_ceiling_addr_1;
};

#define LCD_NUM_BUFFERS	1

#define WSI_TIMEOUT	50
#define PALETTE_SIZE	256
#define LEFT_MARGIN	64
#define RIGHT_MARGIN	64
#define UPPER_MARGIN	32
#define LOWER_MARGIN	32

#define calc_fbsize() (panel.plnSizeX * panel.plnSizeY * panel.gdfBytesPP)

static struct da8xx_lcd_regs *da8xx_fb_reg_base;

DECLARE_GLOBAL_DATA_PTR;

/* graphics setup */
static GraphicDevice gpanel;
static const struct da8xx_panel *lcd_panel;
static struct fb_info *da8xx_fb_info;
static int bits_x_pixel;

static inline unsigned int lcdc_read(u32 *addr)
{
	return (unsigned int)readl(addr);
}

static inline void lcdc_write(unsigned int val, u32 *addr)
{
	writel(val, addr);
}

struct da8xx_fb_par {
	u32			 p_palette_base;
	unsigned char *v_palette_base;
	dma_addr_t		vram_phys;
	unsigned long		vram_size;
	void			*vram_virt;
	unsigned int		dma_start;
	unsigned int		dma_end;
	struct clk *lcdc_clk;
	int irq;
	unsigned short pseudo_palette[16];
	unsigned int palette_sz;
	unsigned int pxl_clk;
	int blank;
	int			vsync_flag;
	int			vsync_timeout;
};


/* Variable Screen Information */
static struct fb_var_screeninfo da8xx_fb_var = {
	.xoffset = 0,
	.yoffset = 0,
	.transp = {0, 0, 0},
	.nonstd = 0,
	.activate = 0,
	.height = -1,
	.width = -1,
	.pixclock = 46666,	/* 46us - AUO display */
	.accel_flags = 0,
	.left_margin = LEFT_MARGIN,
	.right_margin = RIGHT_MARGIN,
	.upper_margin = UPPER_MARGIN,
	.lower_margin = LOWER_MARGIN,
	.sync = 0,
	.vmode = FB_VMODE_NONINTERLACED
};

static struct fb_fix_screeninfo da8xx_fb_fix = {
	.id = "DA8xx FB Drv",
	.type = FB_TYPE_PACKED_PIXELS,
	.type_aux = 0,
	.visual = FB_VISUAL_PSEUDOCOLOR,
	.xpanstep = 0,
	.ypanstep = 1,
	.ywrapstep = 0,
	.accel = FB_ACCEL_NONE
};

static const struct display_panel disp_panel = {
	QVGA,
	16,
	16,
	COLOR_ACTIVE,
};

static const struct lcd_ctrl_config lcd_cfg = {
	&disp_panel,
	.ac_bias		= 255,
	.ac_bias_intrpt		= 0,
	.dma_burst_sz		= 16,
	.bpp			= 16,
	.fdd			= 255,
	.tft_alt_mode		= 0,
	.stn_565_mode		= 0,
	.mono_8bit_mode		= 0,
	.invert_line_clock	= 1,
	.invert_frm_clock	= 1,
	.sync_edge		= 0,
	.sync_ctrl		= 1,
	.raster_order		= 0,
};

/* Enable the Raster Engine of the LCD Controller */
static inline void lcd_enable_raster(void)
{
	u32 reg;

	reg = lcdc_read(&da8xx_fb_reg_base->raster_ctrl);
	if (!(reg & LCD_RASTER_ENABLE))
		lcdc_write(reg | LCD_RASTER_ENABLE,
			&da8xx_fb_reg_base->raster_ctrl);
}

/* Disable the Raster Engine of the LCD Controller */
static inline void lcd_disable_raster(void)
{
	u32 reg;

	reg = lcdc_read(&da8xx_fb_reg_base->raster_ctrl);
	if (reg & LCD_RASTER_ENABLE)
		lcdc_write(reg & ~LCD_RASTER_ENABLE,
			&da8xx_fb_reg_base->raster_ctrl);
}

static void lcd_blit(int load_mode, struct da8xx_fb_par *par)
{
	u32 start;
	u32 end;
	u32 reg_ras;
	u32 reg_dma;

	/* init reg to clear PLM (loading mode) fields */
	reg_ras = lcdc_read(&da8xx_fb_reg_base->raster_ctrl);
	reg_ras &= ~(3 << 20);

	reg_dma  = lcdc_read(&da8xx_fb_reg_base->dma_ctrl);

	if (load_mode == LOAD_DATA) {
		start    = par->dma_start;
		end      = par->dma_end;

		reg_ras |= LCD_PALETTE_LOAD_MODE(DATA_ONLY);
		reg_dma |= LCD_END_OF_FRAME_INT_ENA;

#if (LCD_NUM_BUFFERS == 2)
		reg_dma |= LCD_DUAL_FRAME_BUFFER_ENABLE;
		lcdc_write(start, &da8xx_fb_reg_base->dma_frm_buf_base_addr_0);
		lcdc_write(end, &da8xx_fb_reg_base->dma_frm_buf_ceiling_addr_0);
		lcdc_write(start, &da8xx_fb_reg_base->dma_frm_buf_base_addr_1);
		lcdc_write(end, &da8xx_fb_reg_base->dma_frm_buf_ceiling_addr_1);
#else
		reg_dma &= ~LCD_DUAL_FRAME_BUFFER_ENABLE;
		lcdc_write(start, &da8xx_fb_reg_base->dma_frm_buf_base_addr_0);
		lcdc_write(end, &da8xx_fb_reg_base->dma_frm_buf_ceiling_addr_0);
		lcdc_write(0, &da8xx_fb_reg_base->dma_frm_buf_base_addr_1);
		lcdc_write(0, &da8xx_fb_reg_base->dma_frm_buf_ceiling_addr_1);
#endif

	} else if (load_mode == LOAD_PALETTE) {
		start    = par->p_palette_base;
		end      = start + par->palette_sz - 1;

		reg_ras |= LCD_PALETTE_LOAD_MODE(PALETTE_ONLY);
		reg_ras |= LCD_PL_ENABLE;

		lcdc_write(start, &da8xx_fb_reg_base->dma_frm_buf_base_addr_0);
		lcdc_write(end, &da8xx_fb_reg_base->dma_frm_buf_ceiling_addr_0);
	}

	lcdc_write(reg_dma, &da8xx_fb_reg_base->dma_ctrl);
	lcdc_write(reg_ras, &da8xx_fb_reg_base->raster_ctrl);

	/*
	 * The Raster enable bit must be set after all other control fields are
	 * set.
	 */
	lcd_enable_raster();
}

/* Configure the Burst Size of DMA */
static int lcd_cfg_dma(int burst_size)
{
	u32 reg;

	reg = lcdc_read(&da8xx_fb_reg_base->dma_ctrl) & 0x00000001;
	switch (burst_size) {
	case 1:
		reg |= LCD_DMA_BURST_SIZE(LCD_DMA_BURST_1);
		break;
	case 2:
		reg |= LCD_DMA_BURST_SIZE(LCD_DMA_BURST_2);
		break;
	case 4:
		reg |= LCD_DMA_BURST_SIZE(LCD_DMA_BURST_4);
		break;
	case 8:
		reg |= LCD_DMA_BURST_SIZE(LCD_DMA_BURST_8);
		break;
	case 16:
		reg |= LCD_DMA_BURST_SIZE(LCD_DMA_BURST_16);
		break;
	default:
		return -EINVAL;
	}
	lcdc_write(reg, &da8xx_fb_reg_base->dma_ctrl);

	return 0;
}

static void lcd_cfg_ac_bias(int period, int transitions_per_int)
{
	u32 reg;

	/* Set the AC Bias Period and Number of Transisitons per Interrupt */
	reg = lcdc_read(&da8xx_fb_reg_base->raster_timing_2) & 0xFFF00000;
	reg |= LCD_AC_BIAS_FREQUENCY(period) |
		LCD_AC_BIAS_TRANSITIONS_PER_INT(transitions_per_int);
	lcdc_write(reg, &da8xx_fb_reg_base->raster_timing_2);
}

static void lcd_cfg_horizontal_sync(int back_porch, int pulse_width,
		int front_porch)
{
	u32 reg;

	reg = lcdc_read(&da8xx_fb_reg_base->raster_timing_0) & 0xf;
	reg |= ((back_porch & 0xff) << 24)
	    | ((front_porch & 0xff) << 16)
	    | ((pulse_width & 0x3f) << 10);
	lcdc_write(reg, &da8xx_fb_reg_base->raster_timing_0);
}

static void lcd_cfg_vertical_sync(int back_porch, int pulse_width,
		int front_porch)
{
	u32 reg;

	reg = lcdc_read(&da8xx_fb_reg_base->raster_timing_1) & 0x3ff;
	reg |= ((back_porch & 0xff) << 24)
	    | ((front_porch & 0xff) << 16)
	    | ((pulse_width & 0x3f) << 10);
	lcdc_write(reg, &da8xx_fb_reg_base->raster_timing_1);
}

static int lcd_cfg_display(const struct lcd_ctrl_config *cfg)
{
	u32 reg;

	reg = lcdc_read(&da8xx_fb_reg_base->raster_ctrl) & ~(LCD_TFT_MODE |
						LCD_MONO_8BIT_MODE |
						LCD_MONOCHROME_MODE);

	switch (cfg->p_disp_panel->panel_shade) {
	case MONOCHROME:
		reg |= LCD_MONOCHROME_MODE;
		if (cfg->mono_8bit_mode)
			reg |= LCD_MONO_8BIT_MODE;
		break;
	case COLOR_ACTIVE:
		reg |= LCD_TFT_MODE;
		if (cfg->tft_alt_mode)
			reg |= LCD_TFT_ALT_ENABLE;
		break;

	case COLOR_PASSIVE:
		if (cfg->stn_565_mode)
			reg |= LCD_STN_565_ENABLE;
		break;

	default:
		return -EINVAL;
	}

	/* enable additional interrupts here */
	reg |= LCD_UNDERFLOW_INT_ENA;

	lcdc_write(reg, &da8xx_fb_reg_base->raster_ctrl);

	reg = lcdc_read(&da8xx_fb_reg_base->raster_timing_2);

	if (cfg->sync_ctrl)
		reg |= LCD_SYNC_CTRL;
	else
		reg &= ~LCD_SYNC_CTRL;

	if (cfg->sync_edge)
		reg |= LCD_SYNC_EDGE;
	else
		reg &= ~LCD_SYNC_EDGE;

	if (cfg->invert_line_clock)
		reg |= LCD_INVERT_LINE_CLOCK;
	else
		reg &= ~LCD_INVERT_LINE_CLOCK;

	if (cfg->invert_frm_clock)
		reg |= LCD_INVERT_FRAME_CLOCK;
	else
		reg &= ~LCD_INVERT_FRAME_CLOCK;

	lcdc_write(reg, &da8xx_fb_reg_base->raster_timing_2);

	return 0;
}

static int lcd_cfg_frame_buffer(struct da8xx_fb_par *par, u32 width, u32 height,
		u32 bpp, u32 raster_order)
{
	u32 reg;

	/* Set the Panel Width */
	/* Pixels per line = (PPL + 1)*16 */
	/*0x3F in bits 4..9 gives max horisontal resolution = 1024 pixels*/
	width &= 0x3f0;
	reg = lcdc_read(&da8xx_fb_reg_base->raster_timing_0);
	reg &= 0xfffffc00;
	reg |= ((width >> 4) - 1) << 4;
	lcdc_write(reg, &da8xx_fb_reg_base->raster_timing_0);

	/* Set the Panel Height */
	reg = lcdc_read(&da8xx_fb_reg_base->raster_timing_1);
	reg = ((height - 1) & 0x3ff) | (reg & 0xfffffc00);
	lcdc_write(reg, &da8xx_fb_reg_base->raster_timing_1);

	/* Set the Raster Order of the Frame Buffer */
	reg = lcdc_read(&da8xx_fb_reg_base->raster_ctrl) & ~(1 << 8);
	if (raster_order)
		reg |= LCD_RASTER_ORDER;
	lcdc_write(reg, &da8xx_fb_reg_base->raster_ctrl);

	switch (bpp) {
	case 1:
	case 2:
	case 4:
	case 16:
		par->palette_sz = 16 * 2;
		break;

	case 8:
		par->palette_sz = 256 * 2;
		break;

	default:
		return -EINVAL;
	}

	return 0;
}

static int fb_setcolreg(unsigned regno, unsigned red, unsigned green,
			      unsigned blue, unsigned transp,
			      struct fb_info *info)
{
	struct da8xx_fb_par *par = info->par;
	unsigned short *palette = (unsigned short *) par->v_palette_base;
	u_short pal;
	int update_hw = 0;

	if (regno > 255)
		return 1;

	if (info->fix.visual == FB_VISUAL_DIRECTCOLOR)
		return 1;

	if (info->var.bits_per_pixel == 8) {
		red >>= 4;
		green >>= 8;
		blue >>= 12;

		pal = (red & 0x0f00);
		pal |= (green & 0x00f0);
		pal |= (blue & 0x000f);

		if (palette[regno] != pal) {
			update_hw = 1;
			palette[regno] = pal;
		}
	} else if ((info->var.bits_per_pixel == 16) && regno < 16) {
		red >>= (16 - info->var.red.length);
		red <<= info->var.red.offset;

		green >>= (16 - info->var.green.length);
		green <<= info->var.green.offset;

		blue >>= (16 - info->var.blue.length);
		blue <<= info->var.blue.offset;

		par->pseudo_palette[regno] = red | green | blue;

		if (palette[0] != 0x4000) {
			update_hw = 1;
			palette[0] = 0x4000;
		}
	}

	/* Update the palette in the h/w as needed. */
	if (update_hw)
		lcd_blit(LOAD_PALETTE, par);

	return 0;
}

static void lcd_reset(struct da8xx_fb_par *par)
{
	/* Disable the Raster if previously Enabled */
	lcd_disable_raster();

	/* DMA has to be disabled */
	lcdc_write(0, &da8xx_fb_reg_base->dma_ctrl);
	lcdc_write(0, &da8xx_fb_reg_base->raster_ctrl);
}

static void lcd_calc_clk_divider(struct da8xx_fb_par *par)
{
	unsigned int lcd_clk, div;

	/* Get clock from sysclk2 */
	lcd_clk = clk_get(2);

	div = lcd_clk / par->pxl_clk;
	debug("LCD Clock: 0x%x Divider: 0x%x PixClk: 0x%x\n",
		lcd_clk, div, par->pxl_clk);

	/* Configure the LCD clock divisor. */
	lcdc_write(LCD_CLK_DIVISOR(div) |
			(LCD_RASTER_MODE & 0x1), &da8xx_fb_reg_base->ctrl);
}

static int lcd_init(struct da8xx_fb_par *par, const struct lcd_ctrl_config *cfg,
		const struct da8xx_panel *panel)
{
	u32 bpp;
	int ret = 0;

	lcd_reset(par);

	/* Calculate the divider */
	lcd_calc_clk_divider(par);

	if (panel->invert_pxl_clk)
		lcdc_write((lcdc_read(&da8xx_fb_reg_base->raster_timing_2) |
			LCD_INVERT_PIXEL_CLOCK),
			 &da8xx_fb_reg_base->raster_timing_2);
	else
		lcdc_write((lcdc_read(&da8xx_fb_reg_base->raster_timing_2) &
			~LCD_INVERT_PIXEL_CLOCK),
			&da8xx_fb_reg_base->raster_timing_2);

	/* Configure the DMA burst size. */
	ret = lcd_cfg_dma(cfg->dma_burst_sz);
	if (ret < 0)
		return ret;

	/* Configure the AC bias properties. */
	lcd_cfg_ac_bias(cfg->ac_bias, cfg->ac_bias_intrpt);

	/* Configure the vertical and horizontal sync properties. */
	lcd_cfg_vertical_sync(panel->vbp, panel->vsw, panel->vfp);
	lcd_cfg_horizontal_sync(panel->hbp, panel->hsw, panel->hfp);

	/* Configure for disply */
	ret = lcd_cfg_display(cfg);
	if (ret < 0)
		return ret;

	if (QVGA != cfg->p_disp_panel->panel_type)
		return -EINVAL;

	if (cfg->bpp <= cfg->p_disp_panel->max_bpp &&
	    cfg->bpp >= cfg->p_disp_panel->min_bpp)
		bpp = cfg->bpp;
	else
		bpp = cfg->p_disp_panel->max_bpp;
	if (bpp == 12)
		bpp = 16;
	ret = lcd_cfg_frame_buffer(par, (unsigned int)panel->width,
				(unsigned int)panel->height, bpp,
				cfg->raster_order);
	if (ret < 0)
		return ret;

	/* Configure FDD */
	lcdc_write((lcdc_read(&da8xx_fb_reg_base->raster_ctrl) & 0xfff00fff) |
		       (cfg->fdd << 12), &da8xx_fb_reg_base->raster_ctrl);

	return 0;
}

static void lcdc_dma_start(void)
{
	struct da8xx_fb_par *par = da8xx_fb_info->par;
	lcdc_write(par->dma_start,
		&da8xx_fb_reg_base->dma_frm_buf_base_addr_0);
	lcdc_write(par->dma_end,
		&da8xx_fb_reg_base->dma_frm_buf_ceiling_addr_0);
	lcdc_write(0,
		&da8xx_fb_reg_base->dma_frm_buf_base_addr_1);
	lcdc_write(0,
		&da8xx_fb_reg_base->dma_frm_buf_ceiling_addr_1);
}

static u32 lcdc_irq_handler(void)
{
	struct da8xx_fb_par *par = da8xx_fb_info->par;
	u32 stat = lcdc_read(&da8xx_fb_reg_base->stat);
	u32 reg_ras;

	if ((stat & LCD_SYNC_LOST) && (stat & LCD_FIFO_UNDERFLOW)) {
		debug("LCD_SYNC_LOST\n");
		lcd_disable_raster();
		lcdc_write(stat, &da8xx_fb_reg_base->stat);
		lcd_enable_raster();
		return LCD_SYNC_LOST;
	} else if (stat & LCD_PL_LOAD_DONE) {
		debug("LCD_PL_LOAD_DONE\n");
		/*
		 * Must disable raster before changing state of any control bit.
		 * And also must be disabled before clearing the PL loading
		 * interrupt via the following write to the status register. If
		 * this is done after then one gets multiple PL done interrupts.
		 */
		lcd_disable_raster();

		lcdc_write(stat, &da8xx_fb_reg_base->stat);

		/* Disable PL completion inerrupt */
		reg_ras  = lcdc_read(&da8xx_fb_reg_base->raster_ctrl);
		reg_ras &= ~LCD_PL_ENABLE;
		lcdc_write(reg_ras, &da8xx_fb_reg_base->raster_ctrl);

		/* Setup and start data loading mode */
		lcd_blit(LOAD_DATA, par);
		return LCD_PL_LOAD_DONE;
	} else {
		lcdc_write(stat, &da8xx_fb_reg_base->stat);

		if (stat & LCD_END_OF_FRAME0)
			debug("LCD_END_OF_FRAME0\n");

		lcdc_write(par->dma_start,
			&da8xx_fb_reg_base->dma_frm_buf_base_addr_0);
		lcdc_write(par->dma_end,
			&da8xx_fb_reg_base->dma_frm_buf_ceiling_addr_0);
		par->vsync_flag = 1;
		return LCD_END_OF_FRAME0;
	}
	return stat;
}

static u32 wait_for_event(u32 event)
{
	u32 timeout = 50000;
	u32 ret;

	do {
		ret = lcdc_irq_handler();
		udelay(1000);
	} while (!(ret & event));

	if (timeout <= 0) {
		printf("%s: event %d not hit\n", __func__, event);
		return -1;
	}

	return 0;

}

void *video_hw_init(void)
{
	struct da8xx_fb_par *par;
	u32 size;
	char *p;

	if (!lcd_panel) {
		printf("Display not initialized\n");
		return NULL;
	}
	gpanel.winSizeX = lcd_panel->width;
	gpanel.winSizeY = lcd_panel->height;
	gpanel.plnSizeX = lcd_panel->width;
	gpanel.plnSizeY = lcd_panel->height;

	switch (bits_x_pixel) {
	case 24:
		gpanel.gdfBytesPP = 4;
		gpanel.gdfIndex = GDF_32BIT_X888RGB;
		break;
	case 16:
		gpanel.gdfBytesPP = 2;
		gpanel.gdfIndex = GDF_16BIT_565RGB;
		break;
	default:
		gpanel.gdfBytesPP = 1;
		gpanel.gdfIndex = GDF__8BIT_INDEX;
		break;
	}

	da8xx_fb_reg_base = (struct da8xx_lcd_regs *)DAVINCI_LCD_CNTL_BASE;

	debug("Resolution: %dx%d %x\n",
		gpanel.winSizeX,
		gpanel.winSizeY,
		lcd_cfg.bpp);

	size = sizeof(struct fb_info) + sizeof(struct da8xx_fb_par);
	da8xx_fb_info = malloc(size);
	debug("da8xx_fb_info at %x\n", (unsigned int)da8xx_fb_info);

	if (!da8xx_fb_info) {
		printf("Memory allocation failed for fb_info\n");
		return NULL;
	}
	memset(da8xx_fb_info, 0, size);
	p = (char *)da8xx_fb_info;
	da8xx_fb_info->par = p +  sizeof(struct fb_info);
	debug("da8xx_par at %x\n", (unsigned int)da8xx_fb_info->par);

	par = da8xx_fb_info->par;
	par->pxl_clk = lcd_panel->pxl_clk;

	if (lcd_init(par, &lcd_cfg, lcd_panel) < 0) {
		printf("lcd_init failed\n");
		goto err_release_fb;
	}

	/* allocate frame buffer */
	par->vram_size = lcd_panel->width * lcd_panel->height * lcd_cfg.bpp;
	par->vram_size = par->vram_size * LCD_NUM_BUFFERS / 8;

	par->vram_virt = malloc(par->vram_size);

	par->vram_phys = (dma_addr_t) par->vram_virt;
	debug("Requesting 0x%x bytes for framebuffer at 0x%x\n",
		(unsigned int)par->vram_size,
		(unsigned int)par->vram_virt);
	if (!par->vram_virt) {
		printf("GLCD: malloc for frame buffer failed\n");
		goto err_release_fb;
	}

	gpanel.frameAdrs = (unsigned int)par->vram_virt;
	da8xx_fb_info->screen_base = (char *) par->vram_virt;
	da8xx_fb_fix.smem_start	= gpanel.frameAdrs;
	da8xx_fb_fix.smem_len = par->vram_size;
	da8xx_fb_fix.line_length = (lcd_panel->width * lcd_cfg.bpp) / 8;

	par->dma_start = par->vram_phys;
	par->dma_end   = par->dma_start + lcd_panel->height *
		da8xx_fb_fix.line_length - 1;

	/* allocate palette buffer */
	par->v_palette_base = malloc(PALETTE_SIZE);
	if (!par->v_palette_base) {
		printf("GLCD: malloc for palette buffer failed\n");
		goto err_release_fb_mem;
	}
	memset(par->v_palette_base, 0, PALETTE_SIZE);
	par->p_palette_base = (unsigned int)par->v_palette_base;

	/* Initialize par */
	da8xx_fb_info->var.bits_per_pixel = lcd_cfg.bpp;

	da8xx_fb_var.xres = lcd_panel->width;
	da8xx_fb_var.xres_virtual = lcd_panel->width;

	da8xx_fb_var.yres         = lcd_panel->height;
	da8xx_fb_var.yres_virtual = lcd_panel->height * LCD_NUM_BUFFERS;

	da8xx_fb_var.grayscale =
	    lcd_cfg.p_disp_panel->panel_shade == MONOCHROME ? 1 : 0;
	da8xx_fb_var.bits_per_pixel = lcd_cfg.bpp;

	da8xx_fb_var.hsync_len = lcd_panel->hsw;
	da8xx_fb_var.vsync_len = lcd_panel->vsw;

	/* Initialize fbinfo */
	da8xx_fb_info->flags = FBINFO_FLAG_DEFAULT;
	da8xx_fb_info->fix = da8xx_fb_fix;
	da8xx_fb_info->var = da8xx_fb_var;
	da8xx_fb_info->pseudo_palette = par->pseudo_palette;
	da8xx_fb_info->fix.visual = (da8xx_fb_info->var.bits_per_pixel <= 8) ?
				FB_VISUAL_PSEUDOCOLOR : FB_VISUAL_TRUECOLOR;

	/* Clear interrupt */
	memset((void *)par->vram_virt, 0, par->vram_size);
	lcd_disable_raster();
	lcdc_write(0xFFFF, &da8xx_fb_reg_base->stat);
	debug("Palette at 0x%x size %d\n", par->p_palette_base,
		par->palette_sz);
	lcdc_dma_start();

	/* Load a default palette */
	fb_setcolreg(0, 0, 0, 0, 0xffff, da8xx_fb_info);

	/* Check that the palette is loaded */
	wait_for_event(LCD_PL_LOAD_DONE);

	/* Wait until DMA is working */
	wait_for_event(LCD_END_OF_FRAME0);

	return (void *)&gpanel;

err_release_fb_mem:
	free(par->vram_virt);

err_release_fb:
	free(da8xx_fb_info);

	return NULL;
}

void video_set_lut(unsigned int index,	/* color number */
		    unsigned char r,	/* red */
		    unsigned char g,	/* green */
		    unsigned char b	/* blue */
		    )
{

	return;
}

void da8xx_video_init(const struct da8xx_panel *panel, int bits_pixel)
{
	lcd_panel = panel;
	bits_x_pixel = bits_pixel;
}
