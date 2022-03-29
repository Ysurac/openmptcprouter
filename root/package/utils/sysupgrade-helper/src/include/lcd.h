/*
 * MPC823 and PXA LCD Controller
 *
 * Modeled after video interface by Paolo Scaffardi
 *
 *
 * (C) Copyright 2001
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef _LCD_H_
#define _LCD_H_

extern char lcd_is_enabled;

extern int lcd_line_length;
extern int lcd_color_fg;
extern int lcd_color_bg;

/*
 * Frame buffer memory information
 */
extern void *lcd_base;		/* Start of framebuffer memory	*/
extern void *lcd_console_address;	/* Start of console buffer	*/

extern short console_col;
extern short console_row;
extern struct vidinfo panel_info;

extern void lcd_ctrl_init (void *lcdbase);
extern void lcd_enable (void);

/* setcolreg used in 8bpp/16bpp; initcolregs used in monochrome */
extern void lcd_setcolreg (ushort regno,
				ushort red, ushort green, ushort blue);
extern void lcd_initcolregs (void);

/* gunzip_bmp used if CONFIG_VIDEO_BMP_GZIP */
extern struct bmp_image *gunzip_bmp(unsigned long addr, unsigned long *lenp);
extern int bmp_display(ulong addr, int x, int y);

#if defined CONFIG_MPC823
/*
 * LCD controller stucture for MPC823 CPU
 */
typedef struct vidinfo {
	ushort	vl_col;		/* Number of columns (i.e. 640) */
	ushort	vl_row;		/* Number of rows (i.e. 480) */
	ushort	vl_width;	/* Width of display area in millimeters */
	ushort	vl_height;	/* Height of display area in millimeters */

	/* LCD configuration register */
	u_char	vl_clkp;	/* Clock polarity */
	u_char	vl_oep;		/* Output Enable polarity */
	u_char	vl_hsp;		/* Horizontal Sync polarity */
	u_char	vl_vsp;		/* Vertical Sync polarity */
	u_char	vl_dp;		/* Data polarity */
	u_char	vl_bpix;	/* Bits per pixel, 0 = 1, 1 = 2, 2 = 4, 3 = 8 */
	u_char	vl_lbw;		/* LCD Bus width, 0 = 4, 1 = 8 */
	u_char	vl_splt;	/* Split display, 0 = single-scan, 1 = dual-scan */
	u_char	vl_clor;	/* Color, 0 = mono, 1 = color */
	u_char	vl_tft;		/* 0 = passive, 1 = TFT */

	/* Horizontal control register. Timing from data sheet */
	ushort	vl_wbl;		/* Wait between lines */

	/* Vertical control register */
	u_char	vl_vpw;		/* Vertical sync pulse width */
	u_char	vl_lcdac;	/* LCD AC timing */
	u_char	vl_wbf;		/* Wait between frames */
} vidinfo_t;

#elif defined(CONFIG_CPU_PXA25X) || defined(CONFIG_CPU_PXA27X) || \
	defined CONFIG_CPU_MONAHANS
/*
 * PXA LCD DMA descriptor
 */
struct pxafb_dma_descriptor {
	u_long	fdadr;		/* Frame descriptor address register */
	u_long	fsadr;		/* Frame source address register */
	u_long	fidr;		/* Frame ID register */
	u_long	ldcmd;		/* Command register */
};

/*
 * PXA LCD info
 */
struct pxafb_info {

	/* Misc registers */
	u_long	reg_lccr3;
	u_long	reg_lccr2;
	u_long	reg_lccr1;
	u_long	reg_lccr0;
	u_long	fdadr0;
	u_long	fdadr1;

	/* DMA descriptors */
	struct	pxafb_dma_descriptor *	dmadesc_fblow;
	struct	pxafb_dma_descriptor *	dmadesc_fbhigh;
	struct	pxafb_dma_descriptor *	dmadesc_palette;

	u_long	screen;		/* physical address of frame buffer */
	u_long	palette;	/* physical address of palette memory */
	u_int	palette_size;
};

/*
 * LCD controller stucture for PXA CPU
 */
typedef struct vidinfo {
	ushort	vl_col;		/* Number of columns (i.e. 640) */
	ushort	vl_row;		/* Number of rows (i.e. 480) */
	ushort	vl_width;	/* Width of display area in millimeters */
	ushort	vl_height;	/* Height of display area in millimeters */

	/* LCD configuration register */
	u_char	vl_clkp;	/* Clock polarity */
	u_char	vl_oep;		/* Output Enable polarity */
	u_char	vl_hsp;		/* Horizontal Sync polarity */
	u_char	vl_vsp;		/* Vertical Sync polarity */
	u_char	vl_dp;		/* Data polarity */
	u_char	vl_bpix;	/* Bits per pixel, 0 = 1, 1 = 2, 2 = 4, 3 = 8, 4 = 16 */
	u_char	vl_lbw;		/* LCD Bus width, 0 = 4, 1 = 8 */
	u_char	vl_splt;	/* Split display, 0 = single-scan, 1 = dual-scan */
	u_char	vl_clor;	/* Color, 0 = mono, 1 = color */
	u_char	vl_tft;		/* 0 = passive, 1 = TFT */

	/* Horizontal control register. Timing from data sheet */
	ushort	vl_hpw;		/* Horz sync pulse width */
	u_char	vl_blw;		/* Wait before of line */
	u_char	vl_elw;		/* Wait end of line */

	/* Vertical control register. */
	u_char	vl_vpw;		/* Vertical sync pulse width */
	u_char	vl_bfw;		/* Wait before of frame */
	u_char	vl_efw;		/* Wait end of frame */

	/* PXA LCD controller params */
	struct	pxafb_info pxa;
} vidinfo_t;

#elif defined(CONFIG_ATMEL_LCD) || defined(CONFIG_ATMEL_HLCD)

typedef struct vidinfo {
	ushort vl_col;		/* Number of columns (i.e. 640) */
	ushort vl_row;		/* Number of rows (i.e. 480) */
	u_long vl_clk;	/* pixel clock in ps    */

	/* LCD configuration register */
	u_long vl_sync;		/* Horizontal / vertical sync */
	u_long vl_bpix;		/* Bits per pixel, 0 = 1, 1 = 2, 2 = 4, 3 = 8, 4 = 16 */
	u_long vl_tft;		/* 0 = passive, 1 = TFT */
	u_long vl_cont_pol_low;	/* contrast polarity is low */
	u_long vl_clk_pol;	/* clock polarity */

	/* Horizontal control register. */
	u_long vl_hsync_len;	/* Length of horizontal sync */
	u_long vl_left_margin;	/* Time from sync to picture */
	u_long vl_right_margin;	/* Time from picture to sync */

	/* Vertical control register. */
	u_long vl_vsync_len;	/* Length of vertical sync */
	u_long vl_upper_margin;	/* Time from sync to picture */
	u_long vl_lower_margin;	/* Time from picture to sync */

	u_long	mmio;		/* Memory mapped registers */
} vidinfo_t;

#elif defined(CONFIG_EXYNOS_FB)

enum {
	FIMD_RGB_INTERFACE = 1,
	FIMD_CPU_INTERFACE = 2,
};

enum exynos_fb_rgb_mode_t {
	MODE_RGB_P = 0,
	MODE_BGR_P = 1,
	MODE_RGB_S = 2,
	MODE_BGR_S = 3,
};

typedef struct vidinfo {
	ushort vl_col;		/* Number of columns (i.e. 640) */
	ushort vl_row;		/* Number of rows (i.e. 480) */
	ushort vl_width;	/* Width of display area in millimeters */
	ushort vl_height;	/* Height of display area in millimeters */

	/* LCD configuration register */
	u_char vl_freq;		/* Frequency */
	u_char vl_clkp;		/* Clock polarity */
	u_char vl_oep;		/* Output Enable polarity */
	u_char vl_hsp;		/* Horizontal Sync polarity */
	u_char vl_vsp;		/* Vertical Sync polarity */
	u_char vl_dp;		/* Data polarity */
	u_char vl_bpix;		/* Bits per pixel */

	/* Horizontal control register. Timing from data sheet */
	u_char vl_hspw;		/* Horz sync pulse width */
	u_char vl_hfpd;		/* Wait before of line */
	u_char vl_hbpd;		/* Wait end of line */

	/* Vertical control register. */
	u_char	vl_vspw;	/* Vertical sync pulse width */
	u_char	vl_vfpd;	/* Wait before of frame */
	u_char	vl_vbpd;	/* Wait end of frame */
	u_char  vl_cmd_allow_len; /* Wait end of frame */

	void (*cfg_gpio)(void);
	void (*backlight_on)(unsigned int onoff);
	void (*reset_lcd)(void);
	void (*lcd_power_on)(void);
	void (*cfg_ldo)(void);
	void (*enable_ldo)(unsigned int onoff);
	void (*mipi_power)(void);
	void (*backlight_reset)(void);

	unsigned int win_id;
	unsigned int init_delay;
	unsigned int power_on_delay;
	unsigned int reset_delay;
	unsigned int interface_mode;
	unsigned int mipi_enabled;
	unsigned int cs_setup;
	unsigned int wr_setup;
	unsigned int wr_act;
	unsigned int wr_hold;
	unsigned int logo_on;
	unsigned int logo_width;
	unsigned int logo_height;
	unsigned long logo_addr;
	unsigned int rgb_mode;
	unsigned int resolution;

	/* parent clock name(MPLL, EPLL or VPLL) */
	unsigned int pclk_name;
	/* ratio value for source clock from parent clock. */
	unsigned int sclk_div;

	unsigned int dual_lcd_enabled;

} vidinfo_t;

void init_panel_info(vidinfo_t *vid);

#else

typedef struct vidinfo {
	ushort	vl_col;		/* Number of columns (i.e. 160) */
	ushort	vl_row;		/* Number of rows (i.e. 100) */

	u_char	vl_bpix;	/* Bits per pixel, 0 = 1 */

	ushort	*cmap;		/* Pointer to the colormap */

	void	*priv;		/* Pointer to driver-specific data */
} vidinfo_t;

#endif /* CONFIG_MPC823, CONFIG_CPU_PXA25X, CONFIG_MCC200, CONFIG_ATMEL_LCD */

extern vidinfo_t panel_info;

/* Video functions */

#if defined(CONFIG_RBC823)
void	lcd_disable	(void);
#endif


/* int	lcd_init	(void *lcdbase); */
void	lcd_putc	(const char c);
void	lcd_puts	(const char *s);
void	lcd_printf	(const char *fmt, ...);
void	lcd_clear(void);
int	lcd_display_bitmap(ulong bmp_image, int x, int y);

/* Allow boards to customize the information displayed */
void lcd_show_board_info(void);

/************************************************************************/
/* ** BITMAP DISPLAY SUPPORT						*/
/************************************************************************/
#if defined(CONFIG_CMD_BMP) || defined(CONFIG_SPLASH_SCREEN)
# include <bmp_layout.h>
# include <asm/byteorder.h>
#endif

/*
 *  Information about displays we are using. This is for configuring
 *  the LCD controller and memory allocation. Someone has to know what
 *  is connected, as we can't autodetect anything.
 */
#define CONFIG_SYS_HIGH	0	/* Pins are active high			*/
#define CONFIG_SYS_LOW		1	/* Pins are active low			*/

#define LCD_MONOCHROME	0
#define LCD_COLOR2	1
#define LCD_COLOR4	2
#define LCD_COLOR8	3
#define LCD_COLOR16	4

/*----------------------------------------------------------------------*/
#if defined(CONFIG_LCD_INFO_BELOW_LOGO)
# define LCD_INFO_X		0
# define LCD_INFO_Y		(BMP_LOGO_HEIGHT + VIDEO_FONT_HEIGHT)
#elif defined(CONFIG_LCD_LOGO)
# define LCD_INFO_X		(BMP_LOGO_WIDTH + 4 * VIDEO_FONT_WIDTH)
# define LCD_INFO_Y		(VIDEO_FONT_HEIGHT)
#else
# define LCD_INFO_X		(VIDEO_FONT_WIDTH)
# define LCD_INFO_Y		(VIDEO_FONT_HEIGHT)
#endif

/* Default to 8bpp if bit depth not specified */
#ifndef LCD_BPP
# define LCD_BPP			LCD_COLOR8
#endif
#ifndef LCD_DF
# define LCD_DF			1
#endif

/* Calculate nr. of bits per pixel  and nr. of colors */
#define NBITS(bit_code)		(1 << (bit_code))
#define NCOLORS(bit_code)	(1 << NBITS(bit_code))

/************************************************************************/
/* ** CONSOLE CONSTANTS							*/
/************************************************************************/
#if LCD_BPP == LCD_MONOCHROME

/*
 * Simple black/white definitions
 */
# define CONSOLE_COLOR_BLACK	0
# define CONSOLE_COLOR_WHITE	1	/* Must remain last / highest	*/

#elif LCD_BPP == LCD_COLOR8

/*
 * 8bpp color definitions
 */
# define CONSOLE_COLOR_BLACK	0
# define CONSOLE_COLOR_RED	1
# define CONSOLE_COLOR_GREEN	2
# define CONSOLE_COLOR_YELLOW	3
# define CONSOLE_COLOR_BLUE	4
# define CONSOLE_COLOR_MAGENTA	5
# define CONSOLE_COLOR_CYAN	6
# define CONSOLE_COLOR_GREY	14
# define CONSOLE_COLOR_WHITE	15	/* Must remain last / highest	*/

#else

/*
 * 16bpp color definitions
 */
# define CONSOLE_COLOR_BLACK	0x0000
# define CONSOLE_COLOR_WHITE	0xffff	/* Must remain last / highest	*/

#endif /* color definitions */

/************************************************************************/
#ifndef PAGE_SIZE
# define PAGE_SIZE	4096
#endif

/************************************************************************/
/* ** CONSOLE DEFINITIONS & FUNCTIONS					*/
/************************************************************************/
#if defined(CONFIG_LCD_LOGO) && !defined(CONFIG_LCD_INFO_BELOW_LOGO)
# define CONSOLE_ROWS		((panel_info.vl_row-BMP_LOGO_HEIGHT) \
					/ VIDEO_FONT_HEIGHT)
#else
# define CONSOLE_ROWS		(panel_info.vl_row / VIDEO_FONT_HEIGHT)
#endif

#define CONSOLE_COLS		(panel_info.vl_col / VIDEO_FONT_WIDTH)
#define CONSOLE_ROW_SIZE	(VIDEO_FONT_HEIGHT * lcd_line_length)
#define CONSOLE_ROW_FIRST	(lcd_console_address)
#define CONSOLE_ROW_SECOND	(lcd_console_address + CONSOLE_ROW_SIZE)
#define CONSOLE_ROW_LAST	(lcd_console_address + CONSOLE_SIZE \
					- CONSOLE_ROW_SIZE)
#define CONSOLE_SIZE		(CONSOLE_ROW_SIZE * CONSOLE_ROWS)
#define CONSOLE_SCROLL_SIZE	(CONSOLE_SIZE - CONSOLE_ROW_SIZE)

#if LCD_BPP == LCD_MONOCHROME
# define COLOR_MASK(c)		((c)	  | (c) << 1 | (c) << 2 | (c) << 3 | \
				 (c) << 4 | (c) << 5 | (c) << 6 | (c) << 7)
#elif (LCD_BPP == LCD_COLOR8) || (LCD_BPP == LCD_COLOR16)
# define COLOR_MASK(c)		(c)
#else
# error Unsupported LCD BPP.
#endif

/************************************************************************/

#endif	/* _LCD_H_ */
