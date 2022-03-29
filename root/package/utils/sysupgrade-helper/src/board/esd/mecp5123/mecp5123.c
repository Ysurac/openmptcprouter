/*
 * (C) Copyright 2009 Wolfgang Denk <wd@denx.de>
 * (C) Copyright 2009 Dave Srl www.dave.eu
 * (C) Copyright 2009 Stefan Roese <sr@denx.de>
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
 *
 */

#include <common.h>
#include <asm/bitops.h>
#include <command.h>
#include <asm/io.h>
#include <asm/processor.h>
#include <asm/mpc512x.h>
#include <fdt_support.h>

DECLARE_GLOBAL_DATA_PTR;

/* Clocks in use */
#define SCCR1_CLOCKS_EN	(CLOCK_SCCR1_CFG_EN |				\
			 CLOCK_SCCR1_LPC_EN |				\
			 CLOCK_SCCR1_PSC_EN(CONFIG_PSC_CONSOLE) |	\
			 CLOCK_SCCR1_PSCFIFO_EN |			\
			 CLOCK_SCCR1_DDR_EN |				\
			 CLOCK_SCCR1_FEC_EN |				\
			 CLOCK_SCCR1_NFC_EN |				\
			 CLOCK_SCCR1_PCI_EN |				\
			 CLOCK_SCCR1_TPR_EN)

#define SCCR2_CLOCKS_EN	(CLOCK_SCCR2_MEM_EN |	\
			 CLOCK_SCCR2_I2C_EN)

int eeprom_write_enable(unsigned dev_addr, int state)
{
	volatile immap_t *im = (immap_t *)CONFIG_SYS_IMMR;

	if (dev_addr != CONFIG_SYS_I2C_EEPROM_ADDR)
		return -1;

	if (state == 0)
		setbits_be32(&im->gpio.gpdat, 0x00100000);
	else
		clrbits_be32(&im->gpio.gpdat, 0x00100000);

	return 0;
}

int board_early_init_f(void)
{
	volatile immap_t *im = (immap_t *)CONFIG_SYS_IMMR;
	u32 spridr;
	int i;

	/*
	 * Initialize Local Window for NOR FLASH access
	 */
	out_be32(&im->sysconf.lpcs0aw,
		 CSAW_START(CONFIG_SYS_FLASH_BASE) |
		 CSAW_STOP(CONFIG_SYS_FLASH_BASE, CONFIG_SYS_FLASH_SIZE));
	sync_law(&im->sysconf.lpcs0aw);

	/*
	 * Initialize Local Window for boot access
	 */
	out_be32(&im->sysconf.lpbaw,
		 CSAW_START(0xffb00000) | CSAW_STOP(0xffb00000, 0x00010000));
	sync_law(&im->sysconf.lpbaw);

	/*
	 * Initialize Local Window for VPC3 access
	 */
	out_be32(&im->sysconf.lpcs1aw,
		 CSAW_START(CONFIG_SYS_VPC3_BASE) |
		 CSAW_STOP(CONFIG_SYS_VPC3_BASE, CONFIG_SYS_VPC3_SIZE));
	sync_law(&im->sysconf.lpcs1aw);

	/*
	 * Configure Flash Speed
	 */
	out_be32(&im->lpc.cs_cfg[0], CONFIG_SYS_CS0_CFG);

	/*
	 * Configure VPC3 Speed
	 */
	out_be32(&im->lpc.cs_cfg[1], CONFIG_SYS_CS1_CFG);

	spridr = in_be32(&im->sysconf.spridr);
	if (SVR_MJREV(spridr) >= 2)
		out_be32(&im->lpc.altr, CONFIG_SYS_CS_ALETIMING);

	/*
	 * Enable clocks
	 */
	out_be32(&im->clk.sccr[0], SCCR1_CLOCKS_EN);
	out_be32(&im->clk.sccr[1], SCCR2_CLOCKS_EN);
#if defined(CONFIG_IIM) || defined(CONFIG_CMD_FUSE)
	setbits_be32(&im->clk.sccr[1], CLOCK_SCCR2_IIM_EN);
#endif

	/*
	 * Configure MSCAN clocks
	 */
	for (i=0; i<4; ++i) {
		out_be32(&im->clk.msccr[i], 0x00300000);
		out_be32(&im->clk.msccr[i], 0x00310000);
	}

	/*
	 * Configure GPIO's
	 */
	clrbits_be32(&im->gpio.gpodr, 0x000000e0);
	clrbits_be32(&im->gpio.gpdir, 0x00ef0000);
	setbits_be32(&im->gpio.gpdir, 0x001000e0);
	setbits_be32(&im->gpio.gpdat, 0x00100000);

	return 0;
}

phys_size_t initdram(int board_type)
{
	return get_ram_size(0, fixed_sdram(NULL, NULL, 0));
}

int misc_init_r(void)
{
	volatile immap_t *im = (immap_t *) CONFIG_SYS_IMMR;
	u32 val;

	/*
	 * Optimize access to profibus chip (VPC3) on the local bus
	 */

	/*
	 * Select 1:1 for LPC_DIV
	 */
	val = in_be32(&im->clk.scfr[0]) & ~SCFR1_LPC_DIV_MASK;
	out_be32(&im->clk.scfr[0], val | (0x1 << SCFR1_LPC_DIV_SHIFT));

	/*
	 * Configure LPC Chips Select Deadcycle Control Register
	 * CS0 - device can drive data 2 clock cycle(s) after CS deassertion
	 * CS1 - device can drive data 1 clock cycle(s) after CS deassertion
	 */
	clrbits_be32(&im->lpc.cs_dccr, 0x000000ff);
	setbits_be32(&im->lpc.cs_dccr, (0x00 << 4) | (0x01 << 0));

	/*
	 * Configure LPC Chips Select Holdcycle Control Register
	 * CS0 - data is valid 2 clock cycle(s) after CS deassertion
	 * CS1 - data is valid 1 clock cycle(s) after CS deassertion
	 */
	clrbits_be32(&im->lpc.cs_hccr, 0x000000ff);
	setbits_be32(&im->lpc.cs_hccr, (0x00 << 4) | (0x01 << 0));

	return 0;
}

static iopin_t ioregs_init[] = {
	/* FUNC1=FEC_RX_DV Sets Next 3 to FEC pads */
	{
		offsetof(struct ioctrl512x, io_control_spdif_txclk), 3, 0,
		IO_PIN_FMUX(1) | IO_PIN_HOLD(0) | IO_PIN_PUD(0) |
		IO_PIN_PUE(0) | IO_PIN_ST(0) | IO_PIN_DS(3)
	},
	/* FUNC1=FEC_COL Sets Next 15 to FEC pads */
	{
		offsetof(struct ioctrl512x, io_control_psc0_0), 15, 0,
		IO_PIN_FMUX(1) | IO_PIN_HOLD(0) | IO_PIN_PUD(0) |
		IO_PIN_PUE(0) | IO_PIN_ST(0) | IO_PIN_DS(3)
	},
	/* FUNC1=SELECT LPC_CS1 */
	{
		offsetof(struct ioctrl512x, io_control_lpc_cs1), 1, 0,
		IO_PIN_FMUX(0) | IO_PIN_HOLD(0) | IO_PIN_PUD(0) |
		IO_PIN_PUE(0) | IO_PIN_ST(0) | IO_PIN_DS(3)
	},
	/* FUNC3=SELECT PSC5_2 */
	{
		offsetof(struct ioctrl512x, io_control_psc5_2), 1, 0,
		IO_PIN_FMUX(2) | IO_PIN_HOLD(0) | IO_PIN_PUD(0) |
		IO_PIN_PUE(0) | IO_PIN_ST(0) | IO_PIN_DS(3)
	},
	/* FUNC3=SELECT PSC5_3 */
	{
		offsetof(struct ioctrl512x, io_control_psc5_3), 1, 0,
		IO_PIN_FMUX(3) | IO_PIN_HOLD(0) | IO_PIN_PUD(0) |
		IO_PIN_PUE(0) | IO_PIN_ST(0) | IO_PIN_DS(3)
	},
	/* FUNC3=SELECT PSC7_3 */
	{
		offsetof(struct ioctrl512x, io_control_psc7_3), 1, 0,
		IO_PIN_FMUX(3) | IO_PIN_HOLD(0) | IO_PIN_PUD(0) |
		IO_PIN_PUE(0) | IO_PIN_ST(0) | IO_PIN_DS(3)
	},
	/* FUNC3=SELECT PSC9_0 */
	{
		offsetof(struct ioctrl512x, io_control_psc9_0), 3, 0,
		IO_PIN_FMUX(3) | IO_PIN_HOLD(0) | IO_PIN_PUD(0) |
		IO_PIN_PUE(0) | IO_PIN_ST(0) | IO_PIN_DS(3)
	},
	/* FUNC3=SELECT PSC10_0 */
	{
		offsetof(struct ioctrl512x, io_control_psc10_0), 3, 0,
		IO_PIN_FMUX(3) | IO_PIN_HOLD(0) | IO_PIN_PUD(0) |
		IO_PIN_PUE(0) | IO_PIN_ST(0) | IO_PIN_DS(3)
	},
	/* FUNC3=SELECT PSC10_3 */
	{
		offsetof(struct ioctrl512x, io_control_psc10_3), 1, 0,
		IO_PIN_FMUX(0) | IO_PIN_HOLD(0) | IO_PIN_PUD(0) |
		IO_PIN_PUE(0) | IO_PIN_ST(0) | IO_PIN_DS(3)
	},
	/* FUNC3=SELECT PSC11_0 */
	{
		offsetof(struct ioctrl512x, io_control_psc11_0), 4, 0,
		IO_PIN_FMUX(3) | IO_PIN_HOLD(0) | IO_PIN_PUD(0) |
		IO_PIN_PUE(0) | IO_PIN_ST(0) | IO_PIN_DS(3)
	},
	/* FUNC0=SELECT IRQ0 */
	{
		offsetof(struct ioctrl512x, io_control_irq0), 4, 0,
		IO_PIN_FMUX(0) | IO_PIN_HOLD(0) | IO_PIN_PUD(0) |
		IO_PIN_PUE(0) | IO_PIN_ST(0) | IO_PIN_DS(3)
	}
};

static iopin_t rev2_silicon_pci_ioregs_init[] = {
	/* FUNC0=PCI Sets next 54 to PCI pads */
	{
		offsetof(struct ioctrl512x, io_control_pci_ad31), 54, 0,
		IO_PIN_FMUX(0) | IO_PIN_HOLD(0) | IO_PIN_DS(0)
	}
};

int checkboard(void)
{
	volatile immap_t *im = (immap_t *)CONFIG_SYS_IMMR;
	u32 spridr;

	puts("Board: MECP_5123\n");

	/*
	 * Initialize function mux & slew rate IO inter alia on IO
	 * Pins
	 */
	iopin_initialize(ioregs_init, ARRAY_SIZE(ioregs_init));

	spridr = in_be32(&im->sysconf.spridr);
	if (SVR_MJREV(spridr) >= 2)
		iopin_initialize(rev2_silicon_pci_ioregs_init, 1);

	return 0;
}

#if defined(CONFIG_OF_LIBFDT) && defined(CONFIG_OF_BOARD_SETUP)
void ft_board_setup(void *blob, bd_t *bd)
{
	ft_cpu_setup(blob, bd);
}
#endif /* defined(CONFIG_OF_LIBFDT) && defined(CONFIG_OF_BOARD_SETUP) */
