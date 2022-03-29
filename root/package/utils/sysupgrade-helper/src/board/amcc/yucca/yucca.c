/*
 * (C) Copyright 2006
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
 *
 * Port to AMCC-440SPE Evaluation Board SOP - April 2005
 *
 * PCIe supporting routines derived from Linux 440SPe PCIe driver.
 */

#include <common.h>
#include <asm/ppc4xx.h>
#include <i2c.h>
#include <netdev.h>
#include <asm/processor.h>
#include <asm/io.h>
#include <asm/4xx_pcie.h>
#include <asm/errno.h>

#include "yucca.h"

DECLARE_GLOBAL_DATA_PTR;

void fpga_init (void);

#define DEBUG_ENV
#ifdef DEBUG_ENV
#define DEBUGF(fmt,args...) printf(fmt ,##args)
#else
#define DEBUGF(fmt,args...)
#endif

#define FALSE	0
#define TRUE	1

int board_early_init_f (void)
{
/*----------------------------------------------------------------------------+
| Define Boot devices
+----------------------------------------------------------------------------*/
#define BOOT_FROM_SMALL_FLASH		0x00
#define BOOT_FROM_LARGE_FLASH_OR_SRAM	0x01
#define BOOT_FROM_PCI			0x02
#define BOOT_DEVICE_UNKNOWN		0x03

/*----------------------------------------------------------------------------+
| EBC Devices Characteristics
|   Peripheral Bank Access Parameters       -   EBC_BxAP
|   Peripheral Bank Configuration Register  -   EBC_BxCR
+----------------------------------------------------------------------------*/

/*
 * Small Flash and FRAM
 * BU Value
 * BxAP : 0x03800000  - 0 00000111 0 00 00 00 00 00 000 0 0 0 0 00000
 * B0CR : 0xff098000  - BAS = ff0 - 100 11 00 0000000000000
 * B2CR : 0xe7098000  - BAS = e70 - 100 11 00 0000000000000
 */
#define EBC_BXAP_SMALL_FLASH		EBC_BXAP_BME_DISABLED	| \
					EBC_BXAP_TWT_ENCODE(7)	| \
					EBC_BXAP_BCE_DISABLE	| \
					EBC_BXAP_BCT_2TRANS	| \
					EBC_BXAP_CSN_ENCODE(0)	| \
					EBC_BXAP_OEN_ENCODE(0)	| \
					EBC_BXAP_WBN_ENCODE(0)	| \
					EBC_BXAP_WBF_ENCODE(0)	| \
					EBC_BXAP_TH_ENCODE(0)	| \
					EBC_BXAP_RE_DISABLED	| \
					EBC_BXAP_SOR_DELAYED	| \
					EBC_BXAP_BEM_WRITEONLY	| \
					EBC_BXAP_PEN_DISABLED

#define EBC_BXCR_SMALL_FLASH_CS0	EBC_BXCR_BAS_ENCODE(0xFF000000)	| \
					EBC_BXCR_BS_16MB		| \
					EBC_BXCR_BU_RW			| \
					EBC_BXCR_BW_8BIT

#define EBC_BXCR_SMALL_FLASH_CS2	EBC_BXCR_BAS_ENCODE(0xe7000000)	| \
					EBC_BXCR_BS_16MB		| \
					EBC_BXCR_BU_RW			| \
					EBC_BXCR_BW_8BIT

/*
 * Large Flash and SRAM
 * BU Value
 * BxAP : 0x048ff240  - 0 00000111 0 00 00 00 00 00 000 0 0 0 0 00000
 * B0CR : 0xff09a000  - BAS = ff0 - 100 11 01 0000000000000
 * B2CR : 0xe709a000  - BAS = e70 - 100 11 01 0000000000000
*/
#define EBC_BXAP_LARGE_FLASH		EBC_BXAP_BME_DISABLED	| \
					EBC_BXAP_TWT_ENCODE(7)	| \
					EBC_BXAP_BCE_DISABLE	| \
					EBC_BXAP_BCT_2TRANS	| \
					EBC_BXAP_CSN_ENCODE(0)	| \
					EBC_BXAP_OEN_ENCODE(0)	| \
					EBC_BXAP_WBN_ENCODE(0)	| \
					EBC_BXAP_WBF_ENCODE(0)	| \
					EBC_BXAP_TH_ENCODE(0)	| \
					EBC_BXAP_RE_DISABLED	| \
					EBC_BXAP_SOR_DELAYED	| \
					EBC_BXAP_BEM_WRITEONLY	| \
					EBC_BXAP_PEN_DISABLED

#define EBC_BXCR_LARGE_FLASH_CS0	EBC_BXCR_BAS_ENCODE(0xFF000000)	| \
					EBC_BXCR_BS_16MB		| \
					EBC_BXCR_BU_RW			| \
					EBC_BXCR_BW_16BIT

#define EBC_BXCR_LARGE_FLASH_CS2	EBC_BXCR_BAS_ENCODE(0xE7000000)	| \
					EBC_BXCR_BS_16MB		| \
					EBC_BXCR_BU_RW			| \
					EBC_BXCR_BW_16BIT

/*
 * FPGA
 * BU value :
 * B1AP = 0x05895240  - 0 00001011 0 00 10 01 01 01 001 0 0 1 0 00000
 * B1CR = 0xe201a000  - BAS = e20 - 000 11 01 00000000000000
 */
#define EBC_BXAP_FPGA			EBC_BXAP_BME_DISABLED	| \
					EBC_BXAP_TWT_ENCODE(11)	| \
					EBC_BXAP_BCE_DISABLE	| \
					EBC_BXAP_BCT_2TRANS	| \
					EBC_BXAP_CSN_ENCODE(10)	| \
					EBC_BXAP_OEN_ENCODE(1)	| \
					EBC_BXAP_WBN_ENCODE(1)	| \
					EBC_BXAP_WBF_ENCODE(1)	| \
					EBC_BXAP_TH_ENCODE(1)	| \
					EBC_BXAP_RE_DISABLED	| \
					EBC_BXAP_SOR_DELAYED	| \
					EBC_BXAP_BEM_RW		| \
					EBC_BXAP_PEN_DISABLED

#define EBC_BXCR_FPGA_CS1		EBC_BXCR_BAS_ENCODE(0xe2000000)	| \
					EBC_BXCR_BS_1MB			| \
					EBC_BXCR_BU_RW			| \
					EBC_BXCR_BW_16BIT

	 unsigned long mfr;
	/*
	 * Define Variables for EBC initialization depending on BOOTSTRAP option
	 */
	unsigned long sdr0_pinstp, sdr0_sdstp1 ;
	unsigned long bootstrap_settings, ebc_data_width, boot_selection;
	int computed_boot_device = BOOT_DEVICE_UNKNOWN;

	/*-------------------------------------------------------------------+
	 | Initialize EBC CONFIG -
	 | Keep the Default value, but the bit PDT which has to be set to 1 ?TBC
	 | default value :
	 |	0x07C00000 - 0 0 000 1 1 1 1 1 0000 0 00000 000000000000
	 |
	 +-------------------------------------------------------------------*/
	mtebc(EBC0_CFG, EBC_CFG_LE_UNLOCK |
			EBC_CFG_PTD_ENABLE |
			EBC_CFG_RTC_16PERCLK |
			EBC_CFG_ATC_PREVIOUS |
			EBC_CFG_DTC_PREVIOUS |
			EBC_CFG_CTC_PREVIOUS |
			EBC_CFG_OEO_PREVIOUS |
			EBC_CFG_EMC_DEFAULT |
			EBC_CFG_PME_DISABLE |
			EBC_CFG_PR_16);

	/*-------------------------------------------------------------------+
	 |
	 |  PART 1 : Initialize EBC Bank 1
	 |  ==============================
	 | Bank1 is always associated to the EPLD.
	 | It has to be initialized prior to other banks settings computation
	 | since some board registers values may be needed to determine the
	 | boot type
	 |
	 +-------------------------------------------------------------------*/
	mtebc(PB1AP, EBC_BXAP_FPGA);
	mtebc(PB1CR, EBC_BXCR_FPGA_CS1);

	/*-------------------------------------------------------------------+
	 |
	 |  PART 2 : Determine which boot device was selected
	 |  =================================================
	 |
	 |  Read Pin Strap Register in PPC440SPe
	 |  Result can either be :
	 |   - Boot strap = boot from EBC 8bits     => Small Flash
	 |   - Boot strap = boot from PCI
	 |   - Boot strap = IIC
	 |  In case of boot from IIC, read Serial Device Strap Register1
	 |
	 |  Result can either be :
	 |   - Boot from EBC  - EBC Bus Width = 8bits    => Small Flash
	 |   - Boot from EBC  - EBC Bus Width = 16bits   => Large Flash or SRAM
	 |   - Boot from PCI
	 |
	 +-------------------------------------------------------------------*/
	/* Read Pin Strap Register in PPC440SP */
	mfsdr(SDR0_PINSTP, sdr0_pinstp);
	bootstrap_settings = sdr0_pinstp & SDR0_PINSTP_BOOTSTRAP_MASK;

	switch (bootstrap_settings) {
		case SDR0_PINSTP_BOOTSTRAP_SETTINGS0:
			/*
			 * Strapping Option A
			 * Boot from EBC - 8 bits , Small Flash
			 */
			computed_boot_device = BOOT_FROM_SMALL_FLASH;
			break;
		case SDR0_PINSTP_BOOTSTRAP_SETTINGS1:
			/*
			 * Strappping Option B
			 * Boot from PCI
			 */
			computed_boot_device = BOOT_FROM_PCI;
			break;
		case SDR0_PINSTP_BOOTSTRAP_IIC_50_EN:
		case SDR0_PINSTP_BOOTSTRAP_IIC_54_EN:
			/*
			 * Strapping Option C or D
			 * Boot Settings in IIC EEprom address 0x50 or 0x54
			 * Read Serial Device Strap Register1 in PPC440SPe
			 */
			mfsdr(SDR0_SDSTP1, sdr0_sdstp1);
			boot_selection = sdr0_sdstp1 & SDR0_SDSTP1_ERPN_MASK;
			ebc_data_width = sdr0_sdstp1 & SDR0_SDSTP1_EBCW_MASK;

			switch (boot_selection) {
				case SDR0_SDSTP1_ERPN_EBC:
					switch (ebc_data_width) {
						case SDR0_SDSTP1_EBCW_16_BITS:
							computed_boot_device =
								BOOT_FROM_LARGE_FLASH_OR_SRAM;
							break;
						case SDR0_SDSTP1_EBCW_8_BITS :
							computed_boot_device = BOOT_FROM_SMALL_FLASH;
							break;
					}
					break;

				case SDR0_SDSTP1_ERPN_PCI:
					computed_boot_device = BOOT_FROM_PCI;
					break;
				default:
					/* should not occure */
					computed_boot_device = BOOT_DEVICE_UNKNOWN;
			}
			break;
		default:
			/* should not be */
			computed_boot_device = BOOT_DEVICE_UNKNOWN;
			break;
	}

	/*-------------------------------------------------------------------+
	 |
	 |  PART 3 : Compute EBC settings depending on selected boot device
	 |  ======   ======================================================
	 |
	 | Resulting EBC init will be among following configurations :
	 |
	 |  - Boot from EBC 8bits => boot from Small Flash selected
	 |            EBC-CS0     = Small Flash
	 |            EBC-CS2     = Large Flash and SRAM
	 |
	 |  - Boot from EBC 16bits => boot from Large Flash or SRAM
	 |            EBC-CS0     = Large Flash or SRAM
	 |            EBC-CS2     = Small Flash
	 |
	 |  - Boot from PCI
	 |            EBC-CS0     = not initialized to avoid address contention
	 |            EBC-CS2     = same as boot from Small Flash selected
	 |
	 +-------------------------------------------------------------------*/
	unsigned long ebc0_cs0_bxap_value = 0, ebc0_cs0_bxcr_value = 0;
	unsigned long ebc0_cs2_bxap_value = 0, ebc0_cs2_bxcr_value = 0;

	switch (computed_boot_device) {
		/*-------------------------------------------------------------------*/
		case BOOT_FROM_PCI:
		/*-------------------------------------------------------------------*/
			/*
			 * By Default CS2 is affected to LARGE Flash
			 * do not initialize SMALL FLASH to avoid address contention
			 * Large Flash
			 */
			ebc0_cs2_bxap_value = EBC_BXAP_LARGE_FLASH;
			ebc0_cs2_bxcr_value = EBC_BXCR_LARGE_FLASH_CS2;
			break;

		/*-------------------------------------------------------------------*/
		case BOOT_FROM_SMALL_FLASH:
		/*-------------------------------------------------------------------*/
			ebc0_cs0_bxap_value = EBC_BXAP_SMALL_FLASH;
			ebc0_cs0_bxcr_value = EBC_BXCR_SMALL_FLASH_CS0;

			/*
			 * Large Flash or SRAM
			 */
			/* ebc0_cs2_bxap_value = EBC_BXAP_LARGE_FLASH; */
			ebc0_cs2_bxap_value = 0x048ff240;
			ebc0_cs2_bxcr_value = EBC_BXCR_LARGE_FLASH_CS2;
			break;

		/*-------------------------------------------------------------------*/
		case BOOT_FROM_LARGE_FLASH_OR_SRAM:
		/*-------------------------------------------------------------------*/
			ebc0_cs0_bxap_value = EBC_BXAP_LARGE_FLASH;
			ebc0_cs0_bxcr_value = EBC_BXCR_LARGE_FLASH_CS0;

			/* Small flash */
			ebc0_cs2_bxap_value = EBC_BXAP_SMALL_FLASH;
			ebc0_cs2_bxcr_value = EBC_BXCR_SMALL_FLASH_CS2;
			break;

		/*-------------------------------------------------------------------*/
		default:
		/*-------------------------------------------------------------------*/
			/* BOOT_DEVICE_UNKNOWN */
			break;
	}

	mtebc(PB0AP, ebc0_cs0_bxap_value);
	mtebc(PB0CR, ebc0_cs0_bxcr_value);
	mtebc(PB2AP, ebc0_cs2_bxap_value);
	mtebc(PB2CR, ebc0_cs2_bxcr_value);

	/*--------------------------------------------------------------------+
	 | Interrupt controller setup for the AMCC 440SPe Evaluation board.
	 +--------------------------------------------------------------------+
	+---------------------------------------------------------------------+
	|Interrupt| Source                            | Pol.  | Sensi.| Crit. |
	+---------+-----------------------------------+-------+-------+-------+
	| IRQ 00  | UART0                             | High  | Level | Non   |
	| IRQ 01  | UART1                             | High  | Level | Non   |
	| IRQ 02  | IIC0                              | High  | Level | Non   |
	| IRQ 03  | IIC1                              | High  | Level | Non   |
	| IRQ 04  | PCI0X0 MSG IN                     | High  | Level | Non   |
	| IRQ 05  | PCI0X0 CMD Write                  | High  | Level | Non   |
	| IRQ 06  | PCI0X0 Power Mgt                  | High  | Level | Non   |
	| IRQ 07  | PCI0X0 VPD Access                 | Rising| Edge  | Non   |
	| IRQ 08  | PCI0X0 MSI level 0                | High  | Lvl/ed| Non   |
	| IRQ 09  | External IRQ 15 - (PCI-Express)   | pgm H | Pgm   | Non   |
	| IRQ 10  | UIC2 Non-critical Int.            | NA    | NA    | Non   |
	| IRQ 11  | UIC2 Critical Interrupt           | NA    | NA    | Crit  |
	| IRQ 12  | PCI Express MSI Level 0           | Rising| Edge  | Non   |
	| IRQ 13  | PCI Express MSI Level 1           | Rising| Edge  | Non   |
	| IRQ 14  | PCI Express MSI Level 2           | Rising| Edge  | Non   |
	| IRQ 15  | PCI Express MSI Level 3           | Rising| Edge  | Non   |
	| IRQ 16  | UIC3 Non-critical Int.            | NA    | NA    | Non   |
	| IRQ 17  | UIC3 Critical Interrupt           | NA    | NA    | Crit  |
	| IRQ 18  | External IRQ 14 - (PCI-Express)   | Pgm   | Pgm   | Non   |
	| IRQ 19  | DMA Channel 0 FIFO Full           | High  | Level | Non   |
	| IRQ 20  | DMA Channel 0 Stat FIFO           | High  | Level | Non   |
	| IRQ 21  | DMA Channel 1 FIFO Full           | High  | Level | Non   |
	| IRQ 22  | DMA Channel 1 Stat FIFO           | High  | Level | Non   |
	| IRQ 23  | I2O Inbound Doorbell              | High  | Level | Non   |
	| IRQ 24  | Inbound Post List FIFO Not Empt   | High  | Level | Non   |
	| IRQ 25  | I2O Region 0 LL PLB Write         | High  | Level | Non   |
	| IRQ 26  | I2O Region 1 LL PLB Write         | High  | Level | Non   |
	| IRQ 27  | I2O Region 0 HB PLB Write         | High  | Level | Non   |
	| IRQ 28  | I2O Region 1 HB PLB Write         | High  | Level | Non   |
	| IRQ 29  | GPT Down Count Timer              | Rising| Edge  | Non   |
	| IRQ 30  | UIC1 Non-critical Int.            | NA    | NA    | Non   |
	| IRQ 31  | UIC1 Critical Interrupt           | NA    | NA    | Crit. |
	|----------------------------------------------------------------------
	| IRQ 32  | Ext. IRQ 13 - (PCI-Express)       |pgm (H)|pgm/Lvl| Non   |
	| IRQ 33  | MAL Serr                          | High  | Level | Non   |
	| IRQ 34  | MAL Txde                          | High  | Level | Non   |
	| IRQ 35  | MAL Rxde                          | High  | Level | Non   |
	| IRQ 36  | DMC CE or DMC UE                  | High  | Level | Non   |
	| IRQ 37  | EBC or UART2                      | High  |Lvl Edg| Non   |
	| IRQ 38  | MAL TX EOB                        | High  | Level | Non   |
	| IRQ 39  | MAL RX EOB                        | High  | Level | Non   |
	| IRQ 40  | PCIX0 MSI Level 1                 | High  |Lvl Edg| Non   |
	| IRQ 41  | PCIX0 MSI level 2                 | High  |Lvl Edg| Non   |
	| IRQ 42  | PCIX0 MSI level 3                 | High  |Lvl Edg| Non   |
	| IRQ 43  | L2 Cache                          | Risin | Edge  | Non   |
	| IRQ 44  | GPT Compare Timer 0               | Risin | Edge  | Non   |
	| IRQ 45  | GPT Compare Timer 1               | Risin | Edge  | Non   |
	| IRQ 46  | GPT Compare Timer 2               | Risin | Edge  | Non   |
	| IRQ 47  | GPT Compare Timer 3               | Risin | Edge  | Non   |
	| IRQ 48  | GPT Compare Timer 4               | Risin | Edge  | Non   |
	| IRQ 49  | Ext. IRQ 12 - PCI-X               |pgm/Fal|pgm/Lvl| Non   |
	| IRQ 50  | Ext. IRQ 11 -                     |pgm (H)|pgm/Lvl| Non   |
	| IRQ 51  | Ext. IRQ 10 -                     |pgm (H)|pgm/Lvl| Non   |
	| IRQ 52  | Ext. IRQ 9                        |pgm (H)|pgm/Lvl| Non   |
	| IRQ 53  | Ext. IRQ 8                        |pgm (H)|pgm/Lvl| Non   |
	| IRQ 54  | DMA Error                         | High  | Level | Non   |
	| IRQ 55  | DMA I2O Error                     | High  | Level | Non   |
	| IRQ 56  | Serial ROM                        | High  | Level | Non   |
	| IRQ 57  | PCIX0 Error                       | High  | Edge  | Non   |
	| IRQ 58  | Ext. IRQ 7-                       |pgm (H)|pgm/Lvl| Non   |
	| IRQ 59  | Ext. IRQ 6-                       |pgm (H)|pgm/Lvl| Non   |
	| IRQ 60  | EMAC0 Interrupt                   | High  | Level | Non   |
	| IRQ 61  | EMAC0 Wake-up                     | High  | Level | Non   |
	| IRQ 62  | Reserved                          | High  | Level | Non   |
	| IRQ 63  | XOR                               | High  | Level | Non   |
	|----------------------------------------------------------------------
	| IRQ 64  | PE0 AL                            | High  | Level | Non   |
	| IRQ 65  | PE0 VPD Access                    | Risin | Edge  | Non   |
	| IRQ 66  | PE0 Hot Reset Request             | Risin | Edge  | Non   |
	| IRQ 67  | PE0 Hot Reset Request             | Falli | Edge  | Non   |
	| IRQ 68  | PE0 TCR                           | High  | Level | Non   |
	| IRQ 69  | PE0 BusMaster VCO                 | Falli | Edge  | Non   |
	| IRQ 70  | PE0 DCR Error                     | High  | Level | Non   |
	| IRQ 71  | Reserved                          | N/A   | N/A   | Non   |
	| IRQ 72  | PE1 AL                            | High  | Level | Non   |
	| IRQ 73  | PE1 VPD Access                    | Risin | Edge  | Non   |
	| IRQ 74  | PE1 Hot Reset Request             | Risin | Edge  | Non   |
	| IRQ 75  | PE1 Hot Reset Request             | Falli | Edge  | Non   |
	| IRQ 76  | PE1 TCR                           | High  | Level | Non   |
	| IRQ 77  | PE1 BusMaster VCO                 | Falli | Edge  | Non   |
	| IRQ 78  | PE1 DCR Error                     | High  | Level | Non   |
	| IRQ 79  | Reserved                          | N/A   | N/A   | Non   |
	| IRQ 80  | PE2 AL                            | High  | Level | Non   |
	| IRQ 81  | PE2 VPD Access                    | Risin | Edge  | Non   |
	| IRQ 82  | PE2 Hot Reset Request             | Risin | Edge  | Non   |
	| IRQ 83  | PE2 Hot Reset Request             | Falli | Edge  | Non   |
	| IRQ 84  | PE2 TCR                           | High  | Level | Non   |
	| IRQ 85  | PE2 BusMaster VCO                 | Falli | Edge  | Non   |
	| IRQ 86  | PE2 DCR Error                     | High  | Level | Non   |
	| IRQ 87  | Reserved                          | N/A   | N/A   | Non   |
	| IRQ 88  | External IRQ(5)                   | Progr | Progr | Non   |
	| IRQ 89  | External IRQ 4 - Ethernet         | Progr | Progr | Non   |
	| IRQ 90  | External IRQ 3 - PCI-X            | Progr | Progr | Non   |
	| IRQ 91  | External IRQ 2 - PCI-X            | Progr | Progr | Non   |
	| IRQ 92  | External IRQ 1 - PCI-X            | Progr | Progr | Non   |
	| IRQ 93  | External IRQ 0 - PCI-X            | Progr | Progr | Non   |
	| IRQ 94  | Reserved                          | N/A   | N/A   | Non   |
	| IRQ 95  | Reserved                          | N/A   | N/A   | Non   |
	|---------------------------------------------------------------------
	| IRQ 96  | PE0 INTA                          | High  | Level | Non   |
	| IRQ 97  | PE0 INTB                          | High  | Level | Non   |
	| IRQ 98  | PE0 INTC                          | High  | Level | Non   |
	| IRQ 99  | PE0 INTD                          | High  | Level | Non   |
	| IRQ 100 | PE1 INTA                          | High  | Level | Non   |
	| IRQ 101 | PE1 INTB                          | High  | Level | Non   |
	| IRQ 102 | PE1 INTC                          | High  | Level | Non   |
	| IRQ 103 | PE1 INTD                          | High  | Level | Non   |
	| IRQ 104 | PE2 INTA                          | High  | Level | Non   |
	| IRQ 105 | PE2 INTB                          | High  | Level | Non   |
	| IRQ 106 | PE2 INTC                          | High  | Level | Non   |
	| IRQ 107 | PE2 INTD                          | Risin | Edge  | Non   |
	| IRQ 108 | PCI Express MSI Level 4           | Risin | Edge  | Non   |
	| IRQ 109 | PCI Express MSI Level 5           | Risin | Edge  | Non   |
	| IRQ 110 | PCI Express MSI Level 6           | Risin | Edge  | Non   |
	| IRQ 111 | PCI Express MSI Level 7           | Risin | Edge  | Non   |
	| IRQ 116 | PCI Express MSI Level 12          | Risin | Edge  | Non   |
	| IRQ 112 | PCI Express MSI Level 8           | Risin | Edge  | Non   |
	| IRQ 113 | PCI Express MSI Level 9           | Risin | Edge  | Non   |
	| IRQ 114 | PCI Express MSI Level 10          | Risin | Edge  | Non   |
	| IRQ 115 | PCI Express MSI Level 11          | Risin | Edge  | Non   |
	| IRQ 117 | PCI Express MSI Level 13          | Risin | Edge  | Non   |
	| IRQ 118 | PCI Express MSI Level 14          | Risin | Edge  | Non   |
	| IRQ 119 | PCI Express MSI Level 15          | Risin | Edge  | Non   |
	| IRQ 120 | PCI Express MSI Level 16          | Risin | Edge  | Non   |
	| IRQ 121 | PCI Express MSI Level 17          | Risin | Edge  | Non   |
	| IRQ 122 | PCI Express MSI Level 18          | Risin | Edge  | Non   |
	| IRQ 123 | PCI Express MSI Level 19          | Risin | Edge  | Non   |
	| IRQ 124 | PCI Express MSI Level 20          | Risin | Edge  | Non   |
	| IRQ 125 | PCI Express MSI Level 21          | Risin | Edge  | Non   |
	| IRQ 126 | PCI Express MSI Level 22          | Risin | Edge  | Non   |
	| IRQ 127 | PCI Express MSI Level 23          | Risin | Edge  | Non   |
	+---------+-----------------------------------+-------+-------+------*/
	/*--------------------------------------------------------------------+
	 | Put UICs in PowerPC440SPemode.
	 | Initialise UIC registers.  Clear all interrupts.  Disable all
	 | interrupts.
	 | Set critical interrupt values.  Set interrupt polarities.  Set
	 | interrupt trigger levels.  Make bit 0 High  priority.  Clear all
	 | interrupts again.
	 +-------------------------------------------------------------------*/
	mtdcr (UIC3SR, 0xffffffff);	/* Clear all interrupts */
	mtdcr (UIC3ER, 0x00000000);	/* disable all interrupts */
	mtdcr (UIC3CR, 0x00000000);	/* Set Critical / Non Critical
					 * interrupts */
	mtdcr (UIC3PR, 0xffffffff);	/* Set Interrupt Polarities */
	mtdcr (UIC3TR, 0x001fffff);	/* Set Interrupt Trigger Levels */
	mtdcr (UIC3VR, 0x00000001);	/* Set Vect base=0,INT31 Highest
					 * priority */
	mtdcr (UIC3SR, 0x00000000);	/* clear all  interrupts */
	mtdcr (UIC3SR, 0xffffffff);	/* clear all  interrupts */

	mtdcr (UIC2SR, 0xffffffff);	/* Clear all interrupts */
	mtdcr (UIC2ER, 0x00000000);	/* disable all interrupts */
	mtdcr (UIC2CR, 0x00000000);	/* Set Critical / Non Critical
					 * interrupts */
	mtdcr (UIC2PR, 0xebebebff);	/* Set Interrupt Polarities */
	mtdcr (UIC2TR, 0x74747400);	/* Set Interrupt Trigger Levels */
	mtdcr (UIC2VR, 0x00000001);	/* Set Vect base=0,INT31 Highest
					 * priority */
	mtdcr (UIC2SR, 0x00000000);	/* clear all interrupts */
	mtdcr (UIC2SR, 0xffffffff);	/* clear all interrupts */

	mtdcr (UIC1SR, 0xffffffff);	/* Clear all interrupts */
	mtdcr (UIC1ER, 0x00000000);	/* disable all interrupts */
	mtdcr (UIC1CR, 0x00000000);	/* Set Critical / Non Critical
					 * interrupts */
	mtdcr (UIC1PR, 0xffffffff);	/* Set Interrupt Polarities */
	mtdcr (UIC1TR, 0x001f8040);	/* Set Interrupt Trigger Levels */
	mtdcr (UIC1VR, 0x00000001);	/* Set Vect base=0,INT31 Highest
					 * priority */
	mtdcr (UIC1SR, 0x00000000);	/* clear all interrupts */
	mtdcr (UIC1SR, 0xffffffff);	/* clear all interrupts */

	mtdcr (UIC0SR, 0xffffffff);	/* Clear all interrupts */
	mtdcr (UIC0ER, 0x00000000);	/* disable all interrupts excepted
					 * cascade to be checked */
	mtdcr (UIC0CR, 0x00104001);	/* Set Critical / Non Critical
					 * interrupts */
	mtdcr (UIC0PR, 0xffffffff);	/* Set Interrupt Polarities */
	mtdcr (UIC0TR, 0x010f0004);	/* Set Interrupt Trigger Levels */
	mtdcr (UIC0VR, 0x00000001);	/* Set Vect base=0,INT31 Highest
					 * priority */
	mtdcr (UIC0SR, 0x00000000);	/* clear all interrupts */
	mtdcr (UIC0SR, 0xffffffff);	/* clear all interrupts */

	mfsdr(SDR0_MFR, mfr);
	mfr |= SDR0_MFR_FIXD;		/* Workaround for PCI/DMA */
	mtsdr(SDR0_MFR, mfr);

	fpga_init();

	return 0;
}

int checkboard (void)
{
	char buf[64];
	int i = getenv_f("serial#", buf, sizeof(buf));

	printf("Board: Yucca - AMCC 440SPe Evaluation Board");
	if (i > 0) {
		puts(", serial# ");
		puts(buf);
	}
	putc('\n');

	return 0;
}

/*
 * Override the default functions in arch/powerpc/cpu/ppc4xx/44x_spd_ddr2.c with
 * board specific values.
 */
static int ppc440spe_rev_a(void)
{
	if ((get_pvr() == PVR_440SPe_6_RA) || (get_pvr() == PVR_440SPe_RA))
		return 1;
	else
		return 0;
}

u32 ddr_wrdtr(u32 default_val) {
	/*
	 * Yucca boards with 440SPe rev. A need a slightly different setup
	 * for the MCIF0_WRDTR register.
	 */
	if (ppc440spe_rev_a())
		return (SDRAM_WRDTR_LLWP_1_CYC | SDRAM_WRDTR_WTR_270_DEG_ADV);

	return default_val;
}

u32 ddr_clktr(u32 default_val) {
	/*
	 * Yucca boards with 440SPe rev. A need a slightly different setup
	 * for the MCIF0_CLKTR register.
	 */
	if (ppc440spe_rev_a())
		return (SDRAM_CLKTR_CLKP_180_DEG_ADV);

	return default_val;
}

#if defined(CONFIG_PCI)
int board_pcie_card_present(int port)
{
	u16 reg;

	reg = in_be16((u16 *)FPGA_REG1C);
	switch(port) {
	case 0:
		return !(reg & FPGA_REG1C_PE0_PRSNT);
	case 1:
		return !(reg & FPGA_REG1C_PE1_PRSNT);
	case 2:
		return !(reg & FPGA_REG1C_PE2_PRSNT);
	default:
		return 0;
	}
}

/*
 * For the given slot, set endpoint mode, send power to the slot,
 * turn on the green LED and turn off the yellow LED, enable the
 * clock. In endpoint mode reset bit is read only.
 */
void board_pcie_setup_port(int port, int rootpoint)
{
	u16 power, clock, green_led, yellow_led,
		reset_off, rp, ep;

	switch (port) {
	case 0:
		rp = FPGA_REG1C_PE0_ROOTPOINT;
		ep = 0;
		break;
	case 1:
		rp = 0;
		ep = FPGA_REG1C_PE1_ENDPOINT;
		break;
	case 2:
		rp = 0;
		ep = FPGA_REG1C_PE2_ENDPOINT;
		break;

	default:
		return;
	}

	power = FPGA_REG1A_PWRON_ENCODE(port);
	green_led = FPGA_REG1A_GLED_ENCODE(port);
	clock = FPGA_REG1A_REFCLK_ENCODE(port);
	yellow_led = FPGA_REG1A_YLED_ENCODE(port);
	reset_off = FPGA_REG1C_PERST_ENCODE(port);

	out_be16((u16 *)FPGA_REG1A, ~(power | clock | green_led) &
		 (yellow_led | in_be16((u16 *)FPGA_REG1A)));

	out_be16((u16 *)FPGA_REG1C, ~(ep | reset_off) &
		 (rp | in_be16((u16 *)FPGA_REG1C)));

	if (rootpoint) {
		/*
		 * Leave device in reset for a while after powering on the
		 * slot to give it a chance to initialize.
		 */
		udelay(250 * 1000);

		out_be16((u16 *)FPGA_REG1C,
			 reset_off | in_be16((u16 *)FPGA_REG1C));
	}
}
#endif	/* defined(CONFIG_PCI) */

int misc_init_f (void)
{
	uint reg;

	out16(FPGA_REG10, (in16(FPGA_REG10) &
			~(FPGA_REG10_AUTO_NEG_DIS|FPGA_REG10_RESET_ETH)) |
				FPGA_REG10_10MHZ_ENABLE |
				FPGA_REG10_100MHZ_ENABLE |
				FPGA_REG10_GIGABIT_ENABLE |
				FPGA_REG10_FULL_DUPLEX );

	udelay(10000);	/* wait 10ms */

	out16(FPGA_REG10, (in16(FPGA_REG10) | FPGA_REG10_RESET_ETH));

	/* minimal init for PCIe */
	/* pci express 0 Endpoint Mode */
	mfsdr(SDRN_PESDR_DLPSET(0), reg);
	reg &= (~0x00400000);
	mtsdr(SDRN_PESDR_DLPSET(0), reg);
	/* pci express 1 Rootpoint  Mode */
	mfsdr(SDRN_PESDR_DLPSET(1), reg);
	reg |= 0x00400000;
	mtsdr(SDRN_PESDR_DLPSET(1), reg);
	/* pci express 2 Rootpoint  Mode */
	mfsdr(SDRN_PESDR_DLPSET(2), reg);
	reg |= 0x00400000;
	mtsdr(SDRN_PESDR_DLPSET(2), reg);

	out16(FPGA_REG1C,(in16 (FPGA_REG1C) &
				~FPGA_REG1C_PE0_ROOTPOINT &
				~FPGA_REG1C_PE1_ENDPOINT  &
				~FPGA_REG1C_PE2_ENDPOINT));

	return 0;
}

void fpga_init(void)
{
	/*
	 * by default sdram access is disabled by fpga
	 */
	out16(FPGA_REG10, (in16 (FPGA_REG10) |
				FPGA_REG10_SDRAM_ENABLE |
				FPGA_REG10_ENABLE_DISPLAY ));

	return;
}

/*---------------------------------------------------------------------------+
 | onboard_pci_arbiter_selected => from EPLD
 +---------------------------------------------------------------------------*/
int onboard_pci_arbiter_selected(int core_pci)
{
#if 0
	unsigned long onboard_pci_arbiter_sel;

	onboard_pci_arbiter_sel = in16(FPGA_REG0) & FPGA_REG0_EXT_ARB_SEL_MASK;

	if (onboard_pci_arbiter_sel == FPGA_REG0_EXT_ARB_SEL_EXTERNAL)
		return (BOARD_OPTION_SELECTED);
	else
#endif
	return (BOARD_OPTION_NOT_SELECTED);
}

int board_eth_init(bd_t *bis)
{
	cpu_eth_init(bis);
	return pci_eth_init(bis);
}
