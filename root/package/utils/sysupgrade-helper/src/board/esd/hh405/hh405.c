/*
 * (C) Copyright 2001-2004
 * Stefan Roese, esd gmbh germany, stefan.roese@esd-electronics.com
 *
 * (C) Copyright 2005
 * Stefan Roese, DENX Software Engineering, sr@denx.de.
 *
 * (C) Copyright 2006-2007
 * Matthias Fuchs, esd GmbH, matthias.fuchs@esd-electronics.com
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/processor.h>
#include <asm/io.h>
#include <command.h>
#include <malloc.h>
#include <pci.h>
#include <sm501.h>

DECLARE_GLOBAL_DATA_PTR;

/* FPGA internal regs */
#define FPGA_CTRL	((u16 *)(CONFIG_SYS_FPGA_BASE_ADDR + 0x000))
#define FPGA_STATUS	((u16 *)(CONFIG_SYS_FPGA_BASE_ADDR + 0x002))
#define FPGA_CTR	((u16 *)(CONFIG_SYS_FPGA_BASE_ADDR + 0x004))
#define FPGA_BL		((u16 *)(CONFIG_SYS_FPGA_BASE_ADDR + 0x006))

/* FPGA Control Reg */
#define FPGA_CTRL_REV0      0x0001
#define FPGA_CTRL_REV1      0x0002
#define FPGA_CTRL_VGA0_BL   0x0004
#define FPGA_CTRL_VGA0_BL_MODE 0x0008
#define FPGA_CTRL_CF_RESET  0x0040
#define FPGA_CTRL_PS2_PWR   0x0080
#define FPGA_CTRL_CF_PWRN   0x0100      /* low active */
#define FPGA_CTRL_CF_BUS_EN 0x0200
#define FPGA_CTRL_LCD_CLK   0x7000      /* mask for lcd clock */
#define FPGA_CTRL_OW_ENABLE 0x8000

#define FPGA_STATUS_CF_DETECT 0x8000

#ifdef CONFIG_VIDEO_SM501

#define SWAP32(x)	 ((((x) & 0x000000ff) << 24) | (((x) & 0x0000ff00) << 8)|\
			  (((x) & 0x00ff0000) >>  8) | (((x) & 0xff000000) >> 24) )

#ifdef CONFIG_VIDEO_SM501_8BPP
#error CONFIG_VIDEO_SM501_8BPP not supported.
#endif /* CONFIG_VIDEO_SM501_8BPP */

#ifdef CONFIG_VIDEO_SM501_16BPP
#define BPP	16

/*
 * 800x600 display B084SN03: PCLK = 40MHz
 * => 2*PCLK = 80MHz
 * 336/4 = 84MHz
 * => PCLK = 84MHz
 */
static const SMI_REGS init_regs_800x600 [] =
{
#if 1 /* test-only */
	{0x0005c, SWAP32(0xffffffff)}, /* set endianess to big endian */
#else
	{0x0005c, SWAP32(0x00000000)}, /* set endianess to little endian */
#endif
	{0x00004, SWAP32(0x00000000)},
	/* clocks for pm1... */
	{0x00048, SWAP32(0x00021807)},
	{0x0004C, SWAP32(0x221a0a01)},
	{0x00054, SWAP32(0x00000001)},
	/* clocks for pm0... */
	{0x00040, SWAP32(0x00021807)},
	{0x00044, SWAP32(0x221a0a01)},
	{0x00054, SWAP32(0x00000000)},
	/* GPIO */
	{0x1000c, SWAP32(0xfffffff0)}, /* GPIO32..63 direction */
	/* panel control regs... */
	{0x80000, SWAP32(0x0f013105)}, /* panel display control: 16-bit RGB 5:6:5 mode */
	{0x80004, SWAP32(0xc428bb17)}, /* panel panning control ??? */
	{0x8000C, SWAP32(0x00010000)}, /* panel fb address */
	{0x80010, SWAP32(0x06400640)}, /* panel fb offset/window width */
	{0x80014, SWAP32(0x03200000)}, /* panel fb width (0x320=800) */
	{0x80018, SWAP32(0x02580000)}, /* panel fb height (0x258=600) */
	{0x8001C, SWAP32(0x00000000)}, /* panel plane tl location */
	{0x80020, SWAP32(0x02580320)}, /* panel plane br location */
	{0x80024, SWAP32(0x041f031f)}, /* panel horizontal total */
	{0x80028, SWAP32(0x00800347)}, /* panel horizontal sync */
	{0x8002C, SWAP32(0x02730257)}, /* panel vertical total */
	{0x80030, SWAP32(0x00040258)}, /* panel vertical sync */
	{0x80200, SWAP32(0x00010000)}, /* crt display control */
	{0, 0}
};

/*
 * 1024x768 display G150XG02: PCLK = 65MHz
 * => 2*PCLK = 130MHz
 * 288/2 = 144MHz
 * => PCLK = 72MHz
 */
static const SMI_REGS init_regs_1024x768 [] =
{
	{0x00004, SWAP32(0x00000000)},
	/* clocks for pm1... */
	{0x00048, SWAP32(0x00021807)},
	{0x0004C, SWAP32(0x011a0a01)},
	{0x00054, SWAP32(0x00000001)},
	/* clocks for pm0... */
	{0x00040, SWAP32(0x00021807)},
	{0x00044, SWAP32(0x011a0a01)},
	{0x00054, SWAP32(0x00000000)},
	/* GPIO */
	{0x1000c, SWAP32(0xfffffff0)}, /* GPIO32..63 direction */
	/* panel control regs... */
	{0x80000, SWAP32(0x0f013105)}, /* panel display control: 16-bit RGB 5:6:5 mode */
	{0x80004, SWAP32(0xc428bb17)}, /* panel panning control ??? */
	{0x8000C, SWAP32(0x00010000)}, /* panel fb address */
	{0x80010, SWAP32(0x08000800)}, /* panel fb offset/window width */
	{0x80014, SWAP32(0x04000000)}, /* panel fb width (0x400=1024) */
	{0x80018, SWAP32(0x03000000)}, /* panel fb height (0x300=768) */
	{0x8001C, SWAP32(0x00000000)}, /* panel plane tl location */
	{0x80020, SWAP32(0x03000400)}, /* panel plane br location */
	{0x80024, SWAP32(0x053f03ff)}, /* panel horizontal total */
	{0x80028, SWAP32(0x0140040f)}, /* panel horizontal sync */
	{0x8002C, SWAP32(0x032502ff)}, /* panel vertical total */
	{0x80030, SWAP32(0x00260301)}, /* panel vertical sync */
	{0x80200, SWAP32(0x00010000)}, /* crt display control */
	{0, 0}
};

#endif /* CONFIG_VIDEO_SM501_16BPP */

#ifdef CONFIG_VIDEO_SM501_32BPP
#define BPP	32

/*
 * 800x600 display B084SN03: PCLK = 40MHz
 * => 2*PCLK = 80MHz
 * 336/4 = 84MHz
 * => PCLK = 84MHz
 */
static const SMI_REGS init_regs_800x600 [] =
{
#if 0 /* test-only */
	{0x0005c, SWAP32(0xffffffff)}, /* set endianess to big endian */
#else
	{0x0005c, SWAP32(0x00000000)}, /* set endianess to little endian */
#endif
	{0x00004, SWAP32(0x00000000)},
	/* clocks for pm1... */
	{0x00048, SWAP32(0x00021807)},
	{0x0004C, SWAP32(0x221a0a01)},
	{0x00054, SWAP32(0x00000001)},
	/* clocks for pm0... */
	{0x00040, SWAP32(0x00021807)},
	{0x00044, SWAP32(0x221a0a01)},
	{0x00054, SWAP32(0x00000000)},
	/* GPIO */
	{0x1000c, SWAP32(0xfffffff0)}, /* GPIO32..63 direction */
	/* panel control regs... */
	{0x80000, SWAP32(0x0f013106)}, /* panel display control: 32-bit RGB 8:8:8 mode */
	{0x80004, SWAP32(0xc428bb17)}, /* panel panning control ??? */
	{0x8000C, SWAP32(0x00010000)}, /* panel fb address */
	{0x80010, SWAP32(0x0c800c80)}, /* panel fb offset/window width */
	{0x80014, SWAP32(0x03200000)}, /* panel fb width (0x320=800) */
	{0x80018, SWAP32(0x02580000)}, /* panel fb height (0x258=600) */
	{0x8001C, SWAP32(0x00000000)}, /* panel plane tl location */
	{0x80020, SWAP32(0x02580320)}, /* panel plane br location */
	{0x80024, SWAP32(0x041f031f)}, /* panel horizontal total */
	{0x80028, SWAP32(0x00800347)}, /* panel horizontal sync */
	{0x8002C, SWAP32(0x02730257)}, /* panel vertical total */
	{0x80030, SWAP32(0x00040258)}, /* panel vertical sync */
	{0x80200, SWAP32(0x00010000)}, /* crt display control */
	{0, 0}
};

/*
 * 1024x768 display G150XG02: PCLK = 65MHz
 * => 2*PCLK = 130MHz
 * 288/2 = 144MHz
 * => PCLK = 72MHz
 */
static const SMI_REGS init_regs_1024x768 [] =
{
	{0x00004, SWAP32(0x00000000)},
	/* clocks for pm1... */
	{0x00048, SWAP32(0x00021807)},
	{0x0004C, SWAP32(0x011a0a01)},
	{0x00054, SWAP32(0x00000001)},
	/* clocks for pm0... */
	{0x00040, SWAP32(0x00021807)},
	{0x00044, SWAP32(0x011a0a01)},
	{0x00054, SWAP32(0x00000000)},
	/* GPIO */
	{0x1000c, SWAP32(0xfffffff0)}, /* GPIO32..63 direction */
	/* panel control regs... */
	{0x80000, SWAP32(0x0f013106)}, /* panel display control: 32-bit RGB 8:8:8 mode */
	{0x80004, SWAP32(0xc428bb17)}, /* panel panning control ??? */
	{0x8000C, SWAP32(0x00010000)}, /* panel fb address */
	{0x80010, SWAP32(0x10001000)}, /* panel fb offset/window width */
	{0x80014, SWAP32(0x04000000)}, /* panel fb width (0x400=1024) */
	{0x80018, SWAP32(0x03000000)}, /* panel fb height (0x300=768) */
	{0x8001C, SWAP32(0x00000000)}, /* panel plane tl location */
	{0x80020, SWAP32(0x03000400)}, /* panel plane br location */
	{0x80024, SWAP32(0x053f03ff)}, /* panel horizontal total */
	{0x80028, SWAP32(0x0140040f)}, /* panel horizontal sync */
	{0x8002C, SWAP32(0x032502ff)}, /* panel vertical total */
	{0x80030, SWAP32(0x00260301)}, /* panel vertical sync */
	{0x80200, SWAP32(0x00010000)}, /* crt display control */
	{0, 0}
};

#endif /* CONFIG_VIDEO_SM501_32BPP */

#endif /* CONFIG_VIDEO_SM501 */

#if 0
#define FPGA_DEBUG
#endif

extern void lxt971_no_sleep(void);

/* fpga configuration data - gzip compressed and generated by bin2c */
const unsigned char fpgadata[] =
{
#include "fpgadata.c"
};

/*
 * include common fpga code (for esd boards)
 */
#include "../common/fpga.c"


/* logo bitmap data - gzip compressed and generated by bin2c */
unsigned char logo_bmp_320[] =
{
#include "logo_320_240_4bpp.c"
};

unsigned char logo_bmp_320_8bpp[] =
{
#include "logo_320_240_8bpp.c"
};

unsigned char logo_bmp_640[] =
{
#include "logo_640_480_24bpp.c"
};

unsigned char logo_bmp_1024[] =
{
#include "logo_1024_768_8bpp.c"
};


/*
 * include common lcd code (for esd boards)
 */
#include "../common/lcd.c"

#include "../common/s1d13704_320_240_4bpp.h"
#include "../common/s1d13705_320_240_8bpp.h"
#include "../common/s1d13806_640_480_16bpp.h"
#include "../common/s1d13806_1024_768_8bpp.h"


/*
 * include common auto-update code (for esd boards)
 */
#include "../common/auto_update.h"

au_image_t au_image[] = {
	{"hh405/preinst.img", 0, -1, AU_SCRIPT},
	{"hh405/u-boot.img", 0xfff80000, 0x00080000, AU_FIRMWARE},
	{"hh405/pImage_${bd_type}", 0x00000000, 0x00100000, AU_NAND},
	{"hh405/pImage.initrd", 0x00100000, 0x00200000, AU_NAND},
	{"hh405/yaffsmt2.img", 0x00300000, 0x01c00000, AU_NAND},
	{"hh405/postinst.img", 0, 0, AU_SCRIPT},
};

int N_AU_IMAGES = (sizeof(au_image) / sizeof(au_image[0]));


/*
 * Get version of HH405 board from GPIO's
 */
int board_revision(void)
{
	unsigned long osrh_reg;
	unsigned long isr1h_reg;
	unsigned long tcr_reg;
	unsigned long value;

	/*
	 * Setup GPIO pins (BLAST/GPIO0 and GPIO9 as GPIO)
	 */
	osrh_reg = in_be32((void *)GPIO0_OSRH);
	isr1h_reg = in_be32((void *)GPIO0_ISR1H);
	tcr_reg = in_be32((void *)GPIO0_TCR);
	out_be32((void *)GPIO0_OSRH, osrh_reg & ~0xC0003000);     /* output select */
	out_be32((void *)GPIO0_ISR1H, isr1h_reg | 0xC0003000);    /* input select  */
	out_be32((void *)GPIO0_TCR, tcr_reg & ~0x80400000);       /* select input  */

	udelay(1000);            /* wait some time before reading input */
	value = in_be32((void *)GPIO0_IR) & 0x80400000;         /* get config bits */

	/*
	 * Restore GPIO settings
	 */
	out_be32((void *)GPIO0_OSRH, osrh_reg);                   /* output select */
	out_be32((void *)GPIO0_ISR1H, isr1h_reg);                 /* input select  */
	out_be32((void *)GPIO0_TCR, tcr_reg);  /* enable output driver for outputs */

	if (value & 0x80000000) {
		/* Revision 1.0 or 1.1 detected */
		return 1;
	} else {
		if (value & 0x00400000) {
			/* unused */
			return 3;
		} else {
			return 2;
		}
	}
}


int board_early_init_f (void)
{
	/*
	 * IRQ 0-15  405GP internally generated; active high; level sensitive
	 * IRQ 16    405GP internally generated; active low; level sensitive
	 * IRQ 17-24 RESERVED
	 * IRQ 25 (EXT IRQ 0) CAN0; active low; level sensitive
	 * IRQ 26 (EXT IRQ 1) SER0 ; active low; level sensitive
	 * IRQ 27 (EXT IRQ 2) SER1; active low; level sensitive
	 * IRQ 28 (EXT IRQ 3) FPGA 0; active low; level sensitive
	 * IRQ 29 (EXT IRQ 4) FPGA 1; active low; level sensitive
	 * IRQ 30 (EXT IRQ 5) PCI INTA; active low; level sensitive
	 * IRQ 31 (EXT IRQ 6) COMPACT FLASH; active high; level sensitive
	 */
	mtdcr(UIC0SR, 0xFFFFFFFF);       /* clear all ints */
	mtdcr(UIC0ER, 0x00000000);       /* disable all ints */
	mtdcr(UIC0CR, 0x00000000);       /* set all to be non-critical*/
	mtdcr(UIC0PR, CONFIG_SYS_UIC0_POLARITY);/* set int polarities */
	mtdcr(UIC0TR, 0x10000000);       /* set int trigger levels */
	mtdcr(UIC0VCR, 0x00000001);      /* set vect base=0,INT0 highest priority*/
	mtdcr(UIC0SR, 0xFFFFFFFF);       /* clear all ints */

	/*
	 * EBC Configuration Register: set ready timeout to 512 ebc-clks -> ca. 15 us
	 */
	mtebc(EBC0_CFG, 0xa8400000); /* ebc always driven */

	return 0;
}

int cf_enable(void)
{
	int i;

	if (gd->board_type >= 2) {
		if (in_be16(FPGA_STATUS) & FPGA_STATUS_CF_DETECT) {
			if (!(in_be16(FPGA_CTRL) & FPGA_CTRL_CF_BUS_EN)) {
				out_be16(FPGA_CTRL,
					 in_be16(FPGA_CTRL) & ~FPGA_CTRL_CF_PWRN);

				for (i=0; i<300; i++)
					udelay(1000);

				out_be16(FPGA_CTRL,
					 in_be16(FPGA_CTRL) | FPGA_CTRL_CF_BUS_EN);

				for (i=0; i<20; i++)
					udelay(1000);
			}
		} else {
			out_be16(FPGA_CTRL,
				 in_be16(FPGA_CTRL) & ~FPGA_CTRL_CF_BUS_EN);
			out_be16(FPGA_CTRL,
				 in_be16(FPGA_CTRL) | FPGA_CTRL_CF_PWRN);
		}
	}

	return 0;
}

int misc_init_r (void)
{
	unsigned char *dst;
	ulong len = sizeof(fpgadata);
	int status;
	int index;
	int i;
	char *str;
	unsigned long contrast0 = 0xffffffff;

	dst = malloc(CONFIG_SYS_FPGA_MAX_SIZE);
	if (gunzip (dst, CONFIG_SYS_FPGA_MAX_SIZE, (uchar *)fpgadata, &len) != 0) {
		printf ("GUNZIP ERROR - must RESET board to recover\n");
		do_reset (NULL, 0, 0, NULL);
	}

	status = fpga_boot(dst, len);
	if (status != 0) {
		printf("\nFPGA: Booting failed ");
		switch (status) {
		case ERROR_FPGA_PRG_INIT_LOW:
			printf("(Timeout: INIT not low after asserting PROGRAM*)\n ");
			break;
		case ERROR_FPGA_PRG_INIT_HIGH:
			printf("(Timeout: INIT not high after deasserting PROGRAM*)\n ");
			break;
		case ERROR_FPGA_PRG_DONE:
			printf("(Timeout: DONE not high after programming FPGA)\n ");
			break;
		}

		/* display infos on fpgaimage */
		index = 15;
		for (i=0; i<4; i++) {
			len = dst[index];
			printf("FPGA: %s\n", &(dst[index+1]));
			index += len+3;
		}
		putc ('\n');
		/* delayed reboot */
		for (i=20; i>0; i--) {
			printf("Rebooting in %2d seconds \r",i);
			for (index=0;index<1000;index++)
				udelay(1000);
		}
		putc ('\n');
		do_reset(NULL, 0, 0, NULL);
	}

	puts("FPGA:  ");

	/* display infos on fpgaimage */
	index = 15;
	for (i=0; i<4; i++) {
		len = dst[index];
		printf("%s ", &(dst[index+1]));
		index += len+3;
	}
	putc ('\n');

	free(dst);

	/*
	 * Reset FPGA via FPGA_INIT pin
	 */
	/* setup FPGA_INIT as output */
	out_be32((void *)GPIO0_TCR,
		 in_be32((void *)GPIO0_TCR) | FPGA_INIT);
	out_be32((void *)GPIO0_OR,
		 in_be32((void *)GPIO0_OR) & ~FPGA_INIT);  /* reset low */
	udelay(1000); /* wait 1ms */
	out_be32((void *)GPIO0_OR,
		 in_be32((void *)GPIO0_OR) | FPGA_INIT);   /* reset high */
	udelay(1000); /* wait 1ms */

	/*
	 * Write Board revision into FPGA
	 */
	out_be16(FPGA_CTRL, in_be16(FPGA_CTRL) | (gd->board_type & 0x0003));

	/*
	 * Setup and enable EEPROM write protection
	 */
	out_be32((void *)GPIO0_OR,
		 in_be32((void *)GPIO0_OR) | CONFIG_SYS_EEPROM_WP);

	/*
	 * Reset touch-screen controller
	 */
	out_be32((void *)GPIO0_OR,
		 in_be32((void *)GPIO0_OR) & ~CONFIG_SYS_TOUCH_RST);
	udelay(1000);
	out_be32((void *)GPIO0_OR,
		 in_be32((void *)GPIO0_OR) | CONFIG_SYS_TOUCH_RST);

	/*
	 * Enable power on PS/2 interface (with reset)
	 */
	out_be16(FPGA_CTRL, in_be16(FPGA_CTRL) & ~FPGA_CTRL_PS2_PWR);
	for (i=0;i<500;i++)
		udelay(1000);
	out_be16(FPGA_CTRL, in_be16(FPGA_CTRL) | FPGA_CTRL_PS2_PWR);

	/*
	 * Get contrast value from environment variable
	 */
	str = getenv("contrast0");
	if (str) {
		contrast0 = simple_strtol(str, NULL, 16);
		if (contrast0 > 255) {
			printf("ERROR: contrast0 value too high (0x%lx)!\n",
			       contrast0);
			contrast0 = 0xffffffff;
		}
	}

	/*
	 * Init lcd interface and display logo
	 */

	str = getenv("bd_type");
	if (strcmp(str, "ppc230") == 0) {
		/*
		 * Switch backlight on
		 */
		out_be16(FPGA_CTRL,
			 in_be16(FPGA_CTRL) | FPGA_CTRL_VGA0_BL);
		out_be16(FPGA_BL, 0x0000);

		lcd_setup(1, 0);
		lcd_init((uchar *)CONFIG_SYS_LCD_BIG_REG, (uchar *)CONFIG_SYS_LCD_BIG_MEM,
			 regs_13806_1024_768_8bpp,
			 sizeof(regs_13806_1024_768_8bpp)/sizeof(regs_13806_1024_768_8bpp[0]),
			 logo_bmp_1024, sizeof(logo_bmp_1024));
	} else if (strcmp(str, "ppc220") == 0) {
		/*
		 * Switch backlight on
		 */
		out_be16(FPGA_CTRL,
			 in_be16(FPGA_CTRL) & ~FPGA_CTRL_VGA0_BL);
		out_be16(FPGA_BL, 0x0000);

		lcd_setup(1, 0);
		lcd_init((uchar *)CONFIG_SYS_LCD_BIG_REG, (uchar *)CONFIG_SYS_LCD_BIG_MEM,
			 regs_13806_640_480_16bpp,
			 sizeof(regs_13806_640_480_16bpp)/sizeof(regs_13806_640_480_16bpp[0]),
			 logo_bmp_640, sizeof(logo_bmp_640));
	} else if (strcmp(str, "ppc215") == 0) {
		/*
		 * Set default display contrast voltage
		 */
		if (contrast0 == 0xffffffff) {
			out_be16(FPGA_CTR, 0x0082);
		} else {
			out_be16(FPGA_CTR, contrast0);
		}
		out_be16(FPGA_BL, 0xffff);
		/*
		 * Switch backlight on
		 */
		out_be16(FPGA_CTRL,
			 in_be16(FPGA_CTRL) |
			 FPGA_CTRL_VGA0_BL |
			 FPGA_CTRL_VGA0_BL_MODE);
		/*
		 * Set lcd clock (small epson)
		 */
		out_be16(FPGA_CTRL, in_be16(FPGA_CTRL) | LCD_CLK_06250);
		udelay(100);               /* wait for 100 us */

		lcd_setup(0, 1);
		lcd_init((uchar *)CONFIG_SYS_LCD_SMALL_REG, (uchar *)CONFIG_SYS_LCD_SMALL_MEM,
			 regs_13705_320_240_8bpp,
			 sizeof(regs_13705_320_240_8bpp)/sizeof(regs_13705_320_240_8bpp[0]),
			 logo_bmp_320_8bpp, sizeof(logo_bmp_320_8bpp));
	} else if (strcmp(str, "ppc210") == 0) {
		/*
		 * Set default display contrast voltage
		 */
		if (contrast0 == 0xffffffff) {
			out_be16(FPGA_CTR, 0x0060);
		} else {
			out_be16(FPGA_CTR, contrast0);
		}
		out_be16(FPGA_BL, 0xffff);
		/*
		 * Switch backlight on
		 */
		out_be16(FPGA_CTRL,
			 in_be16(FPGA_CTRL) |
			 FPGA_CTRL_VGA0_BL |
			 FPGA_CTRL_VGA0_BL_MODE);
		/*
		 * Set lcd clock (small epson), enable 1-wire interface
		 */
		out_be16(FPGA_CTRL,
			 in_be16(FPGA_CTRL) |
			 LCD_CLK_08330 |
			 FPGA_CTRL_OW_ENABLE);

		lcd_setup(0, 1);
		lcd_init((uchar *)CONFIG_SYS_LCD_SMALL_REG, (uchar *)CONFIG_SYS_LCD_SMALL_MEM,
			 regs_13704_320_240_4bpp,
			 sizeof(regs_13704_320_240_4bpp)/sizeof(regs_13704_320_240_4bpp[0]),
			 logo_bmp_320, sizeof(logo_bmp_320));
#ifdef CONFIG_VIDEO_SM501
	} else {
		pci_dev_t devbusfn;

		/*
		 * Is SM501 connected (ppc221/ppc231)?
		 */
		devbusfn = pci_find_device(PCI_VENDOR_SM, PCI_DEVICE_SM501, 0);
		if (devbusfn != -1) {
			puts("VGA:   SM501 with 8 MB ");
			if (strcmp(str, "ppc221") == 0) {
				printf("(800*600, %dbpp)\n", BPP);
				out_be16(FPGA_BL, 0x002d); /* max. allowed brightness */
			} else if (strcmp(str, "ppc231") == 0) {
				printf("(1024*768, %dbpp)\n", BPP);
				out_be16(FPGA_BL, 0x0000);
			} else {
				printf("Unsupported bd_type defined (%s) -> No display configured!\n", str);
				return 0;
			}
		} else {
			printf("Unsupported bd_type defined (%s) -> No display configured!\n", str);
			return 0;
		}
#endif /* CONFIG_VIDEO_SM501 */
	}

	cf_enable();

	return (0);
}


/*
 * Check Board Identity:
 */

int checkboard (void)
{
	char str[64];
	int i = getenv_f("serial#", str, sizeof(str));

	puts ("Board: ");

	if (i == -1) {
		puts ("### No HW ID - assuming HH405");
	} else {
		puts(str);
	}

	if (getenv_f("bd_type", str, sizeof(str)) != -1) {
		printf(" (%s", str);
	} else {
		puts(" (Missing bd_type!");
	}

	gd->board_type = board_revision();
	printf(", Rev %ld.x)\n", gd->board_type);

	return 0;
}

#ifdef CONFIG_IDE_RESET
void ide_set_reset(int on)
{
	if (((gd->board_type >= 2) &&
	     (in_be16(FPGA_STATUS) & FPGA_STATUS_CF_DETECT)) ||
	    (gd->board_type < 2)) {
		/*
		 * Assert or deassert CompactFlash Reset Pin
		 */
		if (on) {		/* assert RESET */
			cf_enable();
			out_be16(FPGA_CTRL,
				 in_be16(FPGA_CTRL) &
				 ~FPGA_CTRL_CF_RESET);
		} else {		/* release RESET */
			out_be16(FPGA_CTRL,
				 in_be16(FPGA_CTRL) |
				 FPGA_CTRL_CF_RESET);
		}
	}
}
#endif /* CONFIG_IDE_RESET */


#if defined(CONFIG_SYS_EEPROM_WREN)
/* Input: <dev_addr>  I2C address of EEPROM device to enable.
 *         <state>     -1: deliver current state
 *	               0: disable write
 *		       1: enable write
 *  Returns:           -1: wrong device address
 *                      0: dis-/en- able done
 *		     0/1: current state if <state> was -1.
 */
int eeprom_write_enable (unsigned dev_addr, int state)
{
	if (CONFIG_SYS_I2C_EEPROM_ADDR != dev_addr) {
		return -1;
	} else {
		switch (state) {
		case 1:
			/* Enable write access, clear bit GPIO_SINT2. */
			out_be32((void *)GPIO0_OR,
				 in_be32((void *)GPIO0_OR) & ~CONFIG_SYS_EEPROM_WP);
			state = 0;
			break;
		case 0:
			/* Disable write access, set bit GPIO_SINT2. */
			out_be32((void *)GPIO0_OR,
				 in_be32((void *)GPIO0_OR) | CONFIG_SYS_EEPROM_WP);
			state = 0;
			break;
		default:
			/* Read current status back. */
			state = (0 == (in_be32((void *)GPIO0_OR) &
				       CONFIG_SYS_EEPROM_WP));
			break;
		}
	}
	return state;
}

int do_eep_wren (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int query = argc == 1;
	int state = 0;

	if (query) {
		/* Query write access state. */
		state = eeprom_write_enable (CONFIG_SYS_I2C_EEPROM_ADDR, -1);
		if (state < 0) {
			puts ("Query of write access state failed.\n");
		} else {
			printf ("Write access for device 0x%0x is %sabled.\n",
				CONFIG_SYS_I2C_EEPROM_ADDR, state ? "en" : "dis");
			state = 0;
		}
	} else {
		if ('0' == argv[1][0]) {
			/* Disable write access. */
			state = eeprom_write_enable (CONFIG_SYS_I2C_EEPROM_ADDR, 0);
		} else {
			/* Enable write access. */
			state = eeprom_write_enable (CONFIG_SYS_I2C_EEPROM_ADDR, 1);
		}
		if (state < 0) {
			puts ("Setup of write access state failed.\n");
		}
	}

	return state;
}

U_BOOT_CMD(eepwren,	2,	0,	do_eep_wren,
	"Enable / disable / query EEPROM write access",
	""
);
#endif /* #if defined(CONFIG_SYS_EEPROM_WREN) */


#ifdef CONFIG_VIDEO_SM501
#ifdef CONFIG_CONSOLE_EXTRA_INFO
/*
 * Return text to be printed besides the logo.
 */
void video_get_info_str (int line_number, char *info)
{
	char str[64];
	char str2[64];
	int i = getenv_f("serial#", str2, sizeof(str));

	if (line_number == 1) {
		sprintf(str, " Board: ");

		if (i == -1) {
			strcat(str, "### No HW ID - assuming HH405");
		} else {
			strcat(str, str2);
		}

		if (getenv_f("bd_type", str2, sizeof(str2)) != -1) {
			strcat(str, " (");
			strcat(str, str2);
		} else {
			strcat(str, " (Missing bd_type!");
		}

		sprintf(str2, ", Rev %ld.x)", gd->board_type);
		strcat(str, str2);
		strcpy(info, str);
	} else {
		info [0] = '\0';
	}
}
#endif /* CONFIG_CONSOLE_EXTRA_INFO */

/*
 * Returns SM501 register base address. First thing called in the driver.
 */
unsigned int board_video_init (void)
{
	pci_dev_t devbusfn;
	u32 addr;

	/*
	 * Is SM501 connected (ppc221/ppc231)?
	 */
	devbusfn = pci_find_device(PCI_VENDOR_SM, PCI_DEVICE_SM501, 0);
	if (devbusfn != -1) {
		pci_read_config_dword(devbusfn, PCI_BASE_ADDRESS_1, (u32 *)&addr);
		return (addr & 0xfffffffe);
	}

	return 0;
}

/*
 * Returns SM501 framebuffer address
 */
unsigned int board_video_get_fb (void)
{
	pci_dev_t devbusfn;
	u32 addr;

	/*
	 * Is SM501 connected (ppc221/ppc231)?
	 */
	devbusfn = pci_find_device(PCI_VENDOR_SM, PCI_DEVICE_SM501, 0);
	if (devbusfn != -1) {
		pci_read_config_dword(devbusfn, PCI_BASE_ADDRESS_0, (u32 *)&addr);
		addr &= 0xfffffffe;
#ifdef CONFIG_VIDEO_SM501_FBMEM_OFFSET
		addr += CONFIG_VIDEO_SM501_FBMEM_OFFSET;
#endif
		return addr;
	}

	return 0;
}

/*
 * Called after initializing the SM501 and before clearing the screen.
 */
void board_validate_screen (unsigned int base)
{
}

/*
 * Return a pointer to the initialization sequence.
 */
const SMI_REGS *board_get_regs (void)
{
	char *str;

	str = getenv("bd_type");
	if (strcmp(str, "ppc221") == 0) {
		return init_regs_800x600;
	} else {
		return init_regs_1024x768;
	}
}

int board_get_width (void)
{
	char *str;

	str = getenv("bd_type");
	if (strcmp(str, "ppc221") == 0) {
		return 800;
	} else {
		return 1024;
	}
}

int board_get_height (void)
{
	char *str;

	str = getenv("bd_type");
	if (strcmp(str, "ppc221") == 0) {
		return 600;
	} else {
		return 768;
	}
}

#endif /* CONFIG_VIDEO_SM501 */


void reset_phy(void)
{
#ifdef CONFIG_LXT971_NO_SLEEP

	/*
	 * Disable sleep mode in LXT971
	 */
	lxt971_no_sleep();
#endif
}
