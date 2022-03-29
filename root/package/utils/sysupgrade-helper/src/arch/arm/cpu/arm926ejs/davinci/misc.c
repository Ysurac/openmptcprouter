/*
 * Miscelaneous DaVinci functions.
 *
 * Copyright (C) 2009 Nick Thompson, GE Fanuc Ltd, <nick.thompson@gefanuc.com>
 * Copyright (C) 2007 Sergey Kubushyn <ksi@koi8.net>
 * Copyright (C) 2008 Lyrtech <www.lyrtech.com>
 * Copyright (C) 2004 Texas Instruments.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <common.h>
#include <i2c.h>
#include <net.h>
#include <asm/arch/hardware.h>
#include <asm/io.h>
#include <asm/arch/davinci_misc.h>

DECLARE_GLOBAL_DATA_PTR;

#ifndef CONFIG_SPL_BUILD
int dram_init(void)
{
	/* dram_init must store complete ramsize in gd->ram_size */
	gd->ram_size = get_ram_size(
			(void *)CONFIG_SYS_SDRAM_BASE,
			CONFIG_MAX_RAM_BANK_SIZE);
	return 0;
}

void dram_init_banksize(void)
{
	gd->bd->bi_dram[0].start = CONFIG_SYS_SDRAM_BASE;
	gd->bd->bi_dram[0].size = gd->ram_size;
}
#endif

#ifdef CONFIG_DRIVER_TI_EMAC
/*
 * Read ethernet MAC address from EEPROM for DVEVM compatible boards.
 * Returns 1 if found, 0 otherwise.
 */
int dvevm_read_mac_address(uint8_t *buf)
{
#ifdef CONFIG_SYS_I2C_EEPROM_ADDR
	/* Read MAC address. */
	if (i2c_read(CONFIG_SYS_I2C_EEPROM_ADDR, 0x7F00,
		CONFIG_SYS_I2C_EEPROM_ADDR_LEN, (uint8_t *) &buf[0], 6))
		goto i2cerr;

	/* Check that MAC address is valid. */
	if (!is_valid_ether_addr(buf))
		goto err;

	return 1; /* Found */

i2cerr:
	printf("Read from EEPROM @ 0x%02x failed\n",
		CONFIG_SYS_I2C_EEPROM_ADDR);
err:
#endif /* CONFIG_SYS_I2C_EEPROM_ADDR */

	return 0;
}

/*
 * Set the mii mode as MII or RMII
 */
#if defined(CONFIG_SOC_DA8XX)
void davinci_emac_mii_mode_sel(int mode_sel)
{
	int val;

	val = readl(&davinci_syscfg_regs->cfgchip3);
	if (mode_sel == 0)
		val &= ~(1 << 8);
	else
		val |= (1 << 8);
	writel(val, &davinci_syscfg_regs->cfgchip3);
}
#endif
/*
 * If there is no MAC address in the environment, then it will be initialized
 * (silently) from the value in the EEPROM.
 */
void davinci_sync_env_enetaddr(uint8_t *rom_enetaddr)
{
	uint8_t env_enetaddr[6];
	int ret;

	ret = eth_getenv_enetaddr_by_index("eth", 0, env_enetaddr);
	if (ret) {
		/*
		 * There is no MAC address in the environment, so we
		 * initialize it from the value in the EEPROM.
		 */
		debug("### Setting environment from EEPROM MAC address = "
			"\"%pM\"\n",
			env_enetaddr);
		ret = !eth_setenv_enetaddr("ethaddr", rom_enetaddr);
	}
	if (!ret)
		printf("Failed to set mac address from EEPROM\n");
}
#endif	/* CONFIG_DRIVER_TI_EMAC */

#if defined(CONFIG_SOC_DA8XX)
#ifndef CONFIG_USE_IRQ
void irq_init(void)
{
	/*
	 * Mask all IRQs by clearing the global enable and setting
	 * the enable clear for all the 90 interrupts.
	 */
	writel(0, &davinci_aintc_regs->ger);

	writel(0, &davinci_aintc_regs->hier);

	writel(0xffffffff, &davinci_aintc_regs->ecr1);
	writel(0xffffffff, &davinci_aintc_regs->ecr2);
	writel(0xffffffff, &davinci_aintc_regs->ecr3);
}
#endif

/*
 * Enable PSC for various peripherals.
 */
int da8xx_configure_lpsc_items(const struct lpsc_resource *item,
				    const int n_items)
{
	int i;

	for (i = 0; i < n_items; i++)
		lpsc_on(item[i].lpsc_no);

	return 0;
}
#endif
