/*
 *  Copyright (C) 2005 Sandburst Corporation
 *  Travis B. Sawyer
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

#include <config.h>
#include <common.h>
#include <command.h>
#include "karef.h"
#include "karef_version.h"
#include <timestamp.h>
#include <asm/processor.h>
#include <asm/io.h>
#include <spd_sdram.h>
#include <i2c.h>
#include "../common/sb_common.h"
#include "../common/ppc440gx_i2c.h"
#if defined(CONFIG_HAS_ETH0) || defined(CONFIG_HAS_ETH1) || \
    defined(CONFIG_HAS_ETH2) || defined(CONFIG_HAS_ETH3)
#include <net.h>
#endif

void fpga_init (void);

KAREF_BOARD_ID_ST board_id_as[] =
{
	{"Undefined"},			     /* Not specified */
	{"Kamino Reference Design"},
	{"Reserved"},			     /* Reserved for future use */
	{"Reserved"},			     /* Reserved for future use */
};

KAREF_BOARD_ID_ST ofem_board_id_as[] =
{
	{"Undefined"},
	{"1x10 + 10x2"},
	{"Reserved"},
	{"Reserved"},
};

/*************************************************************************
 *  board_early_init_f
 *
 *  Setup chip selects, initialize the Opto-FPGA, initialize
 *  interrupt polarity and triggers.
 ************************************************************************/
int board_early_init_f (void)
{
	ppc440_gpio_regs_t *gpio_regs;

	/* Enable GPIO interrupts */
	mtsdr(SDR0_PFC0, 0x00103E00);

	/* Setup access for LEDs, and system topology info */
	gpio_regs = (ppc440_gpio_regs_t *)CONFIG_SYS_GPIO_BASE;
	gpio_regs->open_drain = SBCOMMON_GPIO_SYS_LEDS;
	gpio_regs->tri_state  = SBCOMMON_GPIO_DBGLEDS;

	/* Turn on all the leds for now */
	gpio_regs->out = SBCOMMON_GPIO_LEDS;

	/*--------------------------------------------------------------------+
	  | Initialize EBC CONFIG
	  +-------------------------------------------------------------------*/
	mtebc(EBC0_CFG,
	      EBC_CFG_LE_UNLOCK	   | EBC_CFG_PTD_ENABLE |
	      EBC_CFG_RTC_64PERCLK | EBC_CFG_ATC_PREVIOUS |
	      EBC_CFG_DTC_PREVIOUS | EBC_CFG_CTC_PREVIOUS |
	      EBC_CFG_EMC_DEFAULT  | EBC_CFG_PME_DISABLE |
	      EBC_CFG_PR_32);

	/*--------------------------------------------------------------------+
	  | 1/2 MB FLASH. Initialize bank 0 with default values.
	  +-------------------------------------------------------------------*/
	mtebc(PB0AP,
	      EBC_BXAP_BME_DISABLED | EBC_BXAP_TWT_ENCODE(8) |
	      EBC_BXAP_BCE_DISABLE  | EBC_BXAP_CSN_ENCODE(1) |
	      EBC_BXAP_OEN_ENCODE(1)| EBC_BXAP_WBN_ENCODE(1) |
	      EBC_BXAP_WBF_ENCODE(1)| EBC_BXAP_TH_ENCODE(1) |
	      EBC_BXAP_RE_DISABLED  | EBC_BXAP_BEM_WRITEONLY |
	      EBC_BXAP_PEN_DISABLED);

	mtebc(PB0CR, EBC_BXCR_BAS_ENCODE(CONFIG_SYS_FLASH_BASE) |
	      EBC_BXCR_BS_1MB | EBC_BXCR_BU_RW | EBC_BXCR_BW_8BIT);
	/*--------------------------------------------------------------------+
	  | 8KB NVRAM/RTC. Initialize bank 1 with default values.
	  +-------------------------------------------------------------------*/
	mtebc(PB1AP,
	      EBC_BXAP_BME_DISABLED | EBC_BXAP_TWT_ENCODE(10) |
	      EBC_BXAP_BCE_DISABLE  | EBC_BXAP_CSN_ENCODE(1) |
	      EBC_BXAP_OEN_ENCODE(1)| EBC_BXAP_WBN_ENCODE(1) |
	      EBC_BXAP_WBF_ENCODE(1)| EBC_BXAP_TH_ENCODE(1) |
	      EBC_BXAP_RE_DISABLED  | EBC_BXAP_BEM_WRITEONLY |
	      EBC_BXAP_PEN_DISABLED);

	mtebc(PB1CR, EBC_BXCR_BAS_ENCODE(0x48000000) |
	      EBC_BXCR_BS_1MB | EBC_BXCR_BU_RW | EBC_BXCR_BW_8BIT);

	/*--------------------------------------------------------------------+
	  | Compact Flash, uses 2 Chip Selects (2 & 6)
	  +-------------------------------------------------------------------*/
	mtebc(PB2AP,
	      EBC_BXAP_BME_DISABLED | EBC_BXAP_TWT_ENCODE(8) |
	      EBC_BXAP_BCE_DISABLE  | EBC_BXAP_CSN_ENCODE(1) |
	      EBC_BXAP_OEN_ENCODE(1)| EBC_BXAP_WBN_ENCODE(1) |
	      EBC_BXAP_WBF_ENCODE(0)| EBC_BXAP_TH_ENCODE(1) |
	      EBC_BXAP_RE_DISABLED  | EBC_BXAP_BEM_WRITEONLY |
	      EBC_BXAP_PEN_DISABLED);

	mtebc(PB2CR, EBC_BXCR_BAS_ENCODE(0xF0000000) |
	      EBC_BXCR_BS_1MB | EBC_BXCR_BU_RW | EBC_BXCR_BW_16BIT);

	/*--------------------------------------------------------------------+
	  | KaRef Scan FPGA. Initialize bank 3 with default values.
	  +-------------------------------------------------------------------*/
	mtebc(PB5AP,
	      EBC_BXAP_RE_ENABLED    | EBC_BXAP_SOR_NONDELAYED |
	      EBC_BXAP_BME_DISABLED  | EBC_BXAP_TWT_ENCODE(3) |
	      EBC_BXAP_TH_ENCODE(1)  | EBC_BXAP_WBF_ENCODE(0) |
	      EBC_BXAP_CSN_ENCODE(1) | EBC_BXAP_PEN_DISABLED |
	      EBC_BXAP_OEN_ENCODE(1) | EBC_BXAP_BEM_RW);

	mtebc(PB5CR, EBC_BXCR_BAS_ENCODE(0x48200000) |
	      EBC_BXCR_BS_1MB | EBC_BXCR_BU_RW | EBC_BXCR_BW_32BIT);

	/*--------------------------------------------------------------------+
	  | MAC A & B for Kamino.  OFEM FPGA decodes the addresses
	  | Initialize bank 4 with default values.
	  +-------------------------------------------------------------------*/
	mtebc(PB4AP,
	      EBC_BXAP_RE_ENABLED    | EBC_BXAP_SOR_NONDELAYED |
	      EBC_BXAP_BME_DISABLED  | EBC_BXAP_TWT_ENCODE(3) |
	      EBC_BXAP_TH_ENCODE(1)  | EBC_BXAP_WBF_ENCODE(0) |
	      EBC_BXAP_CSN_ENCODE(1) | EBC_BXAP_PEN_DISABLED |
	      EBC_BXAP_OEN_ENCODE(1) | EBC_BXAP_BEM_RW);

	mtebc(PB4CR, EBC_BXCR_BAS_ENCODE(0x48600000) |
	      EBC_BXCR_BS_2MB | EBC_BXCR_BU_RW | EBC_BXCR_BW_32BIT);

	/*--------------------------------------------------------------------+
	  | OFEM FPGA  Initialize bank 5 with default values.
	  +-------------------------------------------------------------------*/
	mtebc(PB3AP,
	      EBC_BXAP_RE_ENABLED    | EBC_BXAP_SOR_NONDELAYED |
	      EBC_BXAP_BME_DISABLED  | EBC_BXAP_TWT_ENCODE(3) |
	      EBC_BXAP_TH_ENCODE(1)  | EBC_BXAP_WBF_ENCODE(0) |
	      EBC_BXAP_CSN_ENCODE(1) | EBC_BXAP_PEN_DISABLED |
	      EBC_BXAP_OEN_ENCODE(1) | EBC_BXAP_BEM_RW);


	mtebc(PB3CR, EBC_BXCR_BAS_ENCODE(0x48400000) |
	      EBC_BXCR_BS_1MB | EBC_BXCR_BU_RW | EBC_BXCR_BW_32BIT);


	/*--------------------------------------------------------------------+
	  | Compact Flash, uses 2 Chip Selects (2 & 6)
	  +-------------------------------------------------------------------*/
	mtebc(PB6AP,
	      EBC_BXAP_BME_DISABLED | EBC_BXAP_TWT_ENCODE(8) |
	      EBC_BXAP_BCE_DISABLE  | EBC_BXAP_CSN_ENCODE(1) |
	      EBC_BXAP_OEN_ENCODE(1)| EBC_BXAP_WBN_ENCODE(1) |
	      EBC_BXAP_WBF_ENCODE(0)| EBC_BXAP_TH_ENCODE(1) |
	      EBC_BXAP_RE_DISABLED  | EBC_BXAP_BEM_WRITEONLY |
	      EBC_BXAP_PEN_DISABLED);

	mtebc(PB6CR, EBC_BXCR_BAS_ENCODE(0xF0100000) |
	      EBC_BXCR_BS_1MB | EBC_BXCR_BU_RW | EBC_BXCR_BW_16BIT);

	/*--------------------------------------------------------------------+
	  | BME-32. Initialize bank 7 with default values.
	  +-------------------------------------------------------------------*/
	mtebc(PB7AP,
	      EBC_BXAP_RE_ENABLED    | EBC_BXAP_SOR_NONDELAYED |
	      EBC_BXAP_BME_DISABLED  | EBC_BXAP_TWT_ENCODE(3) |
	      EBC_BXAP_TH_ENCODE(1)  | EBC_BXAP_WBF_ENCODE(0) |
	      EBC_BXAP_CSN_ENCODE(1) | EBC_BXAP_PEN_DISABLED |
	      EBC_BXAP_OEN_ENCODE(1) | EBC_BXAP_BEM_RW);

	mtebc(PB7CR, EBC_BXCR_BAS_ENCODE(0x48500000) |
	      EBC_BXCR_BS_1MB | EBC_BXCR_BU_RW | EBC_BXCR_BW_32BIT);

	/*--------------------------------------------------------------------+
	 * Setup the interrupt controller polarities, triggers, etc.
	 +-------------------------------------------------------------------*/
	/*
	 * Because of the interrupt handling rework to handle 440GX interrupts
	 * with the common code, we needed to change names of the UIC registers.
	 * Here the new relationship:
	 *
	 * U-Boot name	440GX name
	 * -----------------------
	 * UIC0		UICB0
	 * UIC1		UIC0
	 * UIC2		UIC1
	 * UIC3		UIC2
	 */
	mtdcr (UIC1SR, 0xffffffff);	/* clear all */
	mtdcr (UIC1ER, 0x00000000);	/* disable all */
	mtdcr (UIC1CR, 0x00000000);	/* all non- critical */
	mtdcr (UIC1PR, 0xfffffe03);	/* polarity */
	mtdcr (UIC1TR, 0x01c00000);	/* trigger edge vs level */
	mtdcr (UIC1VR, 0x00000001);	/* int31 highest, base=0x000 */
	mtdcr (UIC1SR, 0xffffffff);	/* clear all */

	mtdcr (UIC2SR, 0xffffffff);	/* clear all */
	mtdcr (UIC2ER, 0x00000000);	/* disable all */
	mtdcr (UIC2CR, 0x00000000);	/* all non-critical */
	mtdcr (UIC2PR, 0xffffc8ff);	/* polarity */
	mtdcr (UIC2TR, 0x00ff0000);	/* trigger edge vs level */
	mtdcr (UIC2VR, 0x00000001);	/* int31 highest, base=0x000 */
	mtdcr (UIC2SR, 0xffffffff);	/* clear all */

	mtdcr (UIC3SR, 0xffffffff);	/* clear all */
	mtdcr (UIC3ER, 0x00000000);	/* disable all */
	mtdcr (UIC3CR, 0x00000000);	/* all non-critical */
	mtdcr (UIC3PR, 0xffff83ff);	/* polarity */
	mtdcr (UIC3TR, 0x00ff8c0f);	/* trigger edge vs level */
	mtdcr (UIC3VR, 0x00000001);	/* int31 highest, base=0x000 */
	mtdcr (UIC3SR, 0xffffffff);	/* clear all */

	mtdcr (UIC0SR, 0xfc000000);	/* clear all */
	mtdcr (UIC0ER, 0x00000000);	/* disable all */
	mtdcr (UIC0CR, 0x00000000);	/* all non-critical */
	mtdcr (UIC0PR, 0xfc000000);
	mtdcr (UIC0TR, 0x00000000);
	mtdcr (UIC0VR, 0x00000001);

	fpga_init();

	return 0;
}


/*************************************************************************
 *  checkboard
 *
 *  Dump pertinent info to the console
 ************************************************************************/
int checkboard (void)
{
	sys_info_t sysinfo;
	unsigned char brd_rev, brd_id;
	unsigned short sernum;
	unsigned char scan_rev, scan_id, ofem_rev=0, ofem_id=0;
	unsigned char ofem_brd_rev, ofem_brd_id;
	KAREF_FPGA_REGS_ST *karef_ps;
	OFEM_FPGA_REGS_ST *ofem_ps;

	karef_ps = (KAREF_FPGA_REGS_ST *)CONFIG_SYS_KAREF_FPGA_BASE;
	ofem_ps = (OFEM_FPGA_REGS_ST *)CONFIG_SYS_OFEM_FPGA_BASE;

	scan_id = (unsigned char)((karef_ps->revision_ul &
				   SAND_HAL_KA_SC_SCAN_REVISION_IDENTIFICATION_MASK)
				  >> SAND_HAL_KA_SC_SCAN_REVISION_IDENTIFICATION_SHIFT);

	scan_rev = (unsigned char)((karef_ps->revision_ul & SAND_HAL_KA_SC_SCAN_REVISION_REVISION_MASK)
				   >> SAND_HAL_KA_SC_SCAN_REVISION_REVISION_SHIFT);

	brd_rev = (unsigned char)((karef_ps->boardinfo_ul & SAND_HAL_KA_SC_SCAN_BRD_INFO_BRD_REV_MASK)
				  >> SAND_HAL_KA_SC_SCAN_BRD_INFO_BRD_REV_SHIFT);

	brd_id = (unsigned char)((karef_ps->boardinfo_ul & SAND_HAL_KA_SC_SCAN_BRD_INFO_BRD_ID_MASK)
				 >> SAND_HAL_KA_SC_SCAN_BRD_INFO_BRD_ID_SHIFT);

	ofem_brd_id = (unsigned char)((karef_ps->boardinfo_ul & SAND_HAL_KA_SC_SCAN_BRD_INFO_FEM_ID_MASK)
				      >> SAND_HAL_KA_SC_SCAN_BRD_INFO_FEM_ID_SHIFT);

	ofem_brd_rev = (unsigned char)((karef_ps->boardinfo_ul & SAND_HAL_KA_SC_SCAN_BRD_INFO_FEM_REV_MASK)
				       >> SAND_HAL_KA_SC_SCAN_BRD_INFO_FEM_REV_SHIFT);

	if (0xF != ofem_brd_id) {
		ofem_id = (unsigned char)((ofem_ps->revision_ul &
					   SAND_HAL_KA_OF_OFEM_REVISION_IDENTIFICATION_MASK)
					  >> SAND_HAL_KA_OF_OFEM_REVISION_IDENTIFICATION_SHIFT);

		ofem_rev = (unsigned char)((ofem_ps->revision_ul &
					    SAND_HAL_KA_OF_OFEM_REVISION_REVISION_MASK)
					   >> SAND_HAL_KA_OF_OFEM_REVISION_REVISION_SHIFT);
	}

	get_sys_info (&sysinfo);

	sernum = sbcommon_get_serial_number();

	printf ("Board: Sandburst Corporation Kamino Reference Design "
		"Serial Number: %d\n", sernum);
	printf ("%s\n", KAREF_U_BOOT_REL_STR);

	printf ("Built %s %s by %s\n", U_BOOT_DATE, U_BOOT_TIME, BUILDUSER);
	if (sbcommon_get_master()) {
		printf("Slot 0 - Master\nSlave board");
		if (sbcommon_secondary_present())
			printf(" present\n");
		else
			printf(" not detected\n");
	} else {
		printf("Slot 1 - Slave\n\n");
	}

	printf ("ScanFPGA ID:\t0x%02X\tRev:  0x%02X\n", scan_id, scan_rev);
	printf ("Board Rev:\t0x%02X\tID:   0x%02X\n", brd_rev, brd_id);
	if(0xF != ofem_brd_id) {
		printf("OFemFPGA ID:\t0x%02X\tRev:  0x%02X\n", ofem_id, ofem_rev);
		printf("OFEM Board Rev:\t0x%02X\tID:   0x%02X\n", ofem_brd_id, ofem_brd_rev);
	}

	/* Fix the ack in the bme 32 */
	udelay(5000);
	out32(CONFIG_SYS_BME32_BASE + 0x0000000C, 0x00000001);
	asm("eieio");


	return (0);
}

/*************************************************************************
 *  misc_init_f
 *
 *  Initialize I2C bus one to gain access to the fans
 ************************************************************************/
int misc_init_f (void)
{
	/* Turn on i2c bus 1 */
	puts ("I2C1:  ");
	i2c1_init (CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
	puts ("ready\n");

	/* Turn on fans 3 & 4 */
	sbcommon_fans();

	return (0);
}

/*************************************************************************
 *  misc_init_r
 *
 *  Do nothing.
 ************************************************************************/
int misc_init_r (void)
{
	unsigned short sernum;
	char envstr[255];
	uchar enetaddr[6];
	KAREF_FPGA_REGS_ST *karef_ps;
	OFEM_FPGA_REGS_ST *ofem_ps;

	if(NULL != getenv("secondserial")) {
		puts("secondserial is set, switching to second serial port\n");
		setenv("stderr", "serial1");
		setenv("stdout", "serial1");
		setenv("stdin", "serial1");
	}

	setenv("ubrelver", KAREF_U_BOOT_REL_STR);

	memset(envstr, 0, 255);
	sprintf (envstr, "Built %s %s by %s",
		 U_BOOT_DATE, U_BOOT_TIME, BUILDUSER);
	setenv("bldstr", envstr);
	saveenv();

	if( getenv("autorecover")) {
		setenv("autorecover", NULL);
		saveenv();
		sernum = sbcommon_get_serial_number();

		printf("\nSetting up environment for automatic filesystem recovery\n");
		/*
		 * Setup default bootargs
		 */
		memset(envstr, 0, 255);

		sprintf(envstr, "console=ttyS0,9600 root=/dev/ram0 "
			"rw ip=10.100.70.%d:::255.255.0.0:karef%d:eth0:none idebus=33",
			sernum, sernum);
		setenv("bootargs", envstr);

		/*
		 * Setup Default boot command
		 */
		setenv("bootcmd", "fatload ide 0 8000000 uimage.karef;"
		       "fatload ide 0 8100000 pramdisk;"
		       "bootm 8000000 8100000");

		printf("Done.  Please type allow the system to continue to boot\n");
	}

	if( getenv("fakeled")) {
		karef_ps = (KAREF_FPGA_REGS_ST *)CONFIG_SYS_KAREF_FPGA_BASE;
		ofem_ps = (OFEM_FPGA_REGS_ST *)CONFIG_SYS_OFEM_FPGA_BASE;
		ofem_ps->control_ul &= ~SAND_HAL_KA_SC_SCAN_CNTL_FAULT_LED_MASK;
		karef_ps->control_ul &= ~SAND_HAL_KA_OF_OFEM_CNTL_FAULT_LED_MASK;
		setenv("bootdelay", "-1");
		saveenv();
		printf("fakeled is set. use 'setenv fakeled ; setenv bootdelay 5 ; saveenv' to recover\n");
	}

#ifdef CONFIG_HAS_ETH0
	if (!eth_getenv_enetaddr("ethaddr", enetaddr)) {
		board_get_enetaddr(0, enetaddr);
		eth_setenv_enetaddr("ethaddr", enetaddr);
	}
#endif

#ifdef CONFIG_HAS_ETH1
	if (!eth_getenv_enetaddr("eth1addr", enetaddr)) {
		board_get_enetaddr(1, enetaddr);
		eth_setenv_enetaddr("eth1addr", enetaddr);
	}
#endif

#ifdef CONFIG_HAS_ETH2
	if (!eth_getenv_enetaddr("eth2addr", enetaddr)) {
		board_get_enetaddr(2, enetaddr);
		eth_setenv_enetaddr("eth2addr", enetaddr);
	}
#endif

#ifdef CONFIG_HAS_ETH3
	if (!eth_getenv_enetaddr("eth3addr", enetaddr)) {
		board_get_enetaddr(3, enetaddr);
		eth_setenv_enetaddr("eth3addr", enetaddr);
	}
#endif

	return (0);
}

/*************************************************************************
 *  ide_set_reset
 ************************************************************************/
#ifdef CONFIG_IDE_RESET
void ide_set_reset(int on)
{
	KAREF_FPGA_REGS_ST *karef_ps;
	/* TODO: ide reset */
	karef_ps = (KAREF_FPGA_REGS_ST *)CONFIG_SYS_KAREF_FPGA_BASE;

	if (on) {
		karef_ps->reset_ul &= ~SAND_HAL_KA_SC_SCAN_RESET_CF_RESET_N_MASK;
	} else {
		karef_ps->reset_ul |= SAND_HAL_KA_SC_SCAN_RESET_CF_RESET_N_MASK;
	}
}
#endif /* CONFIG_IDE_RESET */

/*************************************************************************
 *  fpga_init
 ************************************************************************/
void fpga_init(void)
{
	KAREF_FPGA_REGS_ST *karef_ps;
	OFEM_FPGA_REGS_ST *ofem_ps;
	unsigned char ofem_id;
	unsigned long tmp;

	/* Ensure we have power all around */
	udelay(500);

	karef_ps = (KAREF_FPGA_REGS_ST *)CONFIG_SYS_KAREF_FPGA_BASE;
	tmp =
		SAND_HAL_KA_SC_SCAN_RESET_CF_RESET_N_MASK |
		SAND_HAL_KA_SC_SCAN_RESET_BME_RESET_N_MASK |
		SAND_HAL_KA_SC_SCAN_RESET_KA_RESET_N_MASK |
		SAND_HAL_KA_SC_SCAN_RESET_SLAVE_RESET_N_MASK |
		SAND_HAL_KA_SC_SCAN_RESET_OFEM_RESET_N_MASK |
		SAND_HAL_KA_SC_SCAN_RESET_IFE_A_RESET_N_MASK |
		SAND_HAL_KA_SC_SCAN_RESET_I2C_MUX1_RESET_N_MASK |
		SAND_HAL_KA_SC_SCAN_RESET_PHY0_RESET_N_MASK |
		SAND_HAL_KA_SC_SCAN_RESET_PHY1_RESET_N_MASK;

	karef_ps->reset_ul = tmp;

	/*
	 * Wait a bit to allow the ofem fpga to get its brains
	 */
	udelay(5000);

	/*
	 * Check to see if the ofem is there
	 */
	ofem_id = (unsigned char)((karef_ps->boardinfo_ul & SAND_HAL_KA_SC_SCAN_BRD_INFO_FEM_ID_MASK)
				  >> SAND_HAL_KA_SC_SCAN_BRD_INFO_FEM_ID_SHIFT);
	if(0xF != ofem_id) {
		tmp =
			SAND_HAL_KA_OF_OFEM_RESET_I2C_MUX0_RESET_N_MASK |
			SAND_HAL_KA_OF_OFEM_RESET_LOCH0_RESET_N_MASK |
			SAND_HAL_KA_OF_OFEM_RESET_MAC0_RESET_N_MASK;

		ofem_ps = (OFEM_FPGA_REGS_ST *)CONFIG_SYS_OFEM_FPGA_BASE;
		ofem_ps->reset_ul = tmp;

		ofem_ps->control_ul |= 1 < SAND_HAL_KA_OF_OFEM_CNTL_FAULT_LED_SHIFT;
	}

	karef_ps->control_ul |= 1 << SAND_HAL_KA_SC_SCAN_CNTL_FAULT_LED_SHIFT;

	asm("eieio");

	return;
}

int karefSetupVars(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	unsigned short sernum;
	char envstr[255];

	sernum = sbcommon_get_serial_number();

	memset(envstr, 0, 255);
	/*
	 * Setup our ip address
	 */
	sprintf(envstr, "10.100.70.%d", sernum);

	setenv("ipaddr", envstr);
	/*
	 * Setup the host ip address
	 */
	setenv("serverip", "10.100.17.10");

	/*
	 * Setup default bootargs
	 */
	memset(envstr, 0, 255);

	sprintf(envstr, "console=ttyS0,9600 root=/dev/nfs "
		"rw nfsroot=10.100.17.10:/home/metrobox/mbc70.%d "
		"nfsaddrs=10.100.70.%d:10.100.17.10:10.100.1.1:"
		"255.255.0.0:karef%d.sandburst.com:eth0:none idebus=33",
		sernum, sernum, sernum);

	setenv("bootargs_nfs", envstr);
	setenv("bootargs", envstr);

	/*
	 * Setup CF bootargs
	 */
	memset(envstr, 0, 255);

	sprintf(envstr, "console=ttyS0,9600 root=/dev/hda2 "
		"rw ip=10.100.70.%d:::255.255.0.0:karef%d:eth0:none idebus=33",
		sernum, sernum);

	setenv("bootargs_cf", envstr);

	/*
	 * Setup Default boot command
	 */
	setenv("bootcmd_tftp", "tftp 8000000 uImage.karef;bootm 8000000");
	setenv("bootcmd", "tftp 8000000 uImage.karef;bootm 8000000");

	/*
	 * Setup compact flash boot command
	 */
	setenv("bootcmd_cf", "fatload ide 0 8000000 uimage.karef;bootm 8000000");

	saveenv();

	return(1);
}

int karefRecover(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	unsigned short sernum;
	char envstr[255];

	sernum = sbcommon_get_serial_number();

	printf("\nSetting up environment for filesystem recovery\n");
	/*
	 * Setup default bootargs
	 */
	memset(envstr, 0, 255);

	sprintf(envstr, "console=ttyS0,9600 root=/dev/ram0 "
		"rw ip=10.100.70.%d:::255.255.0.0:karef%d:eth0:none",
		sernum, sernum);
	setenv("bootargs", envstr);

	/*
	 * Setup Default boot command
	 */

	setenv("bootcmd", "fatload ide 0 8000000 uimage.karef;"
	       "fatload ide 0 8100000 pramdisk;"
	       "bootm 8000000 8100000");

	printf("Done.  Please type boot<cr>.\nWhen the kernel has booted"
	       " please type fsrecover.sh<cr>\n");

	return(1);
}

U_BOOT_CMD(kasetup, 1, 1, karefSetupVars,
	   "Set environment to factory defaults", "");

U_BOOT_CMD(karecover, 1, 1, karefRecover,
	   "Set environment to allow for fs recovery", "");
