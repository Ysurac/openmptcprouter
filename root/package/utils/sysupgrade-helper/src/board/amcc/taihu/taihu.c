/*
 * (C) Copyright 2000-2005
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2005-2007
 * Beijing UD Technology Co., Ltd., taihusupport@amcc.com
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
#include <command.h>
#include <asm/processor.h>
#include <asm/io.h>
#include <spi.h>
#include <netdev.h>
#include <asm/ppc4xx-gpio.h>

extern int lcd_init(void);

/*
 * board_early_init_f
 */
int board_early_init_f(void)
{
	lcd_init();

	mtdcr(UIC0SR, 0xFFFFFFFF);	/* clear all ints */
	mtdcr(UIC0ER, 0x00000000);	/* disable all ints */
	mtdcr(UIC0CR, 0x00000000);
	mtdcr(UIC0PR, 0xFFFF7F00);	/* set int polarities */
	mtdcr(UIC0TR, 0x00000000);	/* set int trigger levels */
	mtdcr(UIC0SR, 0xFFFFFFFF);	/* clear all ints */
	mtdcr(UIC0VCR, 0x00000001);	/* set vect base=0,INT0 highest priority */

	mtebc(PB3AP, CONFIG_SYS_EBC_PB3AP);	/* memory bank 3 (CPLD_LCM) initialization */
	mtebc(PB3CR, CONFIG_SYS_EBC_PB3CR);

	/*
	 * Configure CPC0_PCI to enable PerWE as output
	 * and enable the internal PCI arbiter
	 */
	mtdcr(CPC0_PCI, CPC0_PCI_SPE | CPC0_PCI_HOST_CFG_EN | CPC0_PCI_ARBIT_EN);

	return 0;
}

/*
 * Check Board Identity:
 */
int checkboard(void)
{
	char buf[64];
	int i = getenv_f("serial#", buf, sizeof(buf));

	puts("Board: Taihu - AMCC PPC405EP Evaluation Board");

	if (i > 0) {
		puts(", serial# ");
		puts(buf);
	}
	putc('\n');

	return 0;
}

static int do_sw_stat(cmd_tbl_t* cmd_tp, int flags, int argc, char * const argv[])
{
	char stat;
	int i;

	stat = in_8((u8 *) CPLD_REG0_ADDR);
	printf("SW2 status: ");
	for (i=0; i<4; i++) /* 4-position */
		printf("%d:%s ", i, stat & (0x08 >> i)?"on":"off");
	printf("\n");
	return 0;
}

U_BOOT_CMD (
	sw2_stat, 1, 1, do_sw_stat,
	"show status of switch 2",
	""
);

static int do_led_ctl(cmd_tbl_t* cmd_tp, int flags, int argc, char * const argv[])
{
	int led_no;

	if (argc != 3)
		return cmd_usage(cmd_tp);

	led_no = simple_strtoul(argv[1], NULL, 16);
	if (led_no != 1 && led_no != 2)
		return cmd_usage(cmd_tp);

	if (strcmp(argv[2],"off") == 0x0) {
		if (led_no == 1)
			gpio_write_bit(30, 1);
		else
			gpio_write_bit(31, 1);
	} else if (strcmp(argv[2],"on") == 0x0) {
		if (led_no == 1)
			gpio_write_bit(30, 0);
		else
			gpio_write_bit(31, 0);
	} else {
		return cmd_usage(cmd_tp);
	}

	return 0;
}

U_BOOT_CMD (
	led_ctl, 3, 1, do_led_ctl,
	"make led 1 or 2  on or off",
	"<led_no> <on/off>	-  make led <led_no> on/off,\n"
	"\tled_no is 1 or 2"
);

#define SPI_CS_GPIO0	0
#define SPI_SCLK_GPIO14	14
#define SPI_DIN_GPIO15	15
#define SPI_DOUT_GPIO16	16

void spi_scl(int bit)
{
	gpio_write_bit(SPI_SCLK_GPIO14, bit);
}

void spi_sda(int bit)
{
	gpio_write_bit(SPI_DOUT_GPIO16, bit);
}

unsigned char spi_read(void)
{
	return (unsigned char)gpio_read_in_bit(SPI_DIN_GPIO15);
}

int spi_cs_is_valid(unsigned int bus, unsigned int cs)
{
	return bus == 0 && cs == 0;
}

void spi_cs_activate(struct spi_slave *slave)
{
	gpio_write_bit(SPI_CS_GPIO0, 1);
}

void spi_cs_deactivate(struct spi_slave *slave)
{
	gpio_write_bit(SPI_CS_GPIO0, 0);
}

#ifdef CONFIG_PCI
static unsigned char int_lines[32] = {
	29, 30, 27, 28, 29, 30, 25, 27,
	29, 30, 27, 28, 29, 30, 27, 28,
	29, 30, 27, 28, 29, 30, 27, 28,
	29, 30, 27, 28, 29, 30, 27, 28};

static void taihu_pci_fixup_irq(struct pci_controller *hose, pci_dev_t dev)
{
	unsigned char int_line = int_lines[PCI_DEV(dev) & 31];

	pci_hose_write_config_byte(hose, dev, PCI_INTERRUPT_LINE, int_line);
}

int pci_pre_init(struct pci_controller *hose)
{
	hose->fixup_irq = taihu_pci_fixup_irq;
	return 1;
}
#endif /* CONFIG_PCI */

int board_eth_init(bd_t *bis)
{
	cpu_eth_init(bis);
	return pci_eth_init(bis);
}
