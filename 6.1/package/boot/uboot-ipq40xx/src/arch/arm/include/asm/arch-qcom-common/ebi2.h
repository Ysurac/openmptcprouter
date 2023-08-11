/*
 * Copyright (c) 2012 The Linux Foundation. All rights reserved.
 */
#ifndef QCOM_EBI2_H
#define QCOM_EBI2_H

#define IPQ806x_EBI2CR_BASE				(0x1A600000)
#define IPQ40xx_EBI2CR_BASE				(0x079a0000)

struct ebi2cr_regs {
	uint32_t chip_select_cfg0;                        /* 0x00000000 */
	uint32_t cfg;                                     /* 0x00000004 */
	uint32_t hw_info;                                 /* 0x00000008 */
	uint8_t reserved0[20];
	uint32_t lcd_cfg0;                                /* 0x00000020 */
	uint32_t lcd_cfg1;                                /* 0x00000024 */
	uint8_t reserved1[8];
	uint32_t arbiter_cfg;                             /* 0x00000030 */
	uint8_t reserved2[28];
	uint32_t debug_sel;                               /* 0x00000050 */
	uint32_t crc_cfg;                                 /* 0x00000054 */
	uint32_t crc_reminder_cfg;                        /* 0x00000058 */
	uint32_t nand_adm_mux;                            /* 0x0000005C */
	uint32_t mutex_addr_offset;                       /* 0x00000060 */
	uint32_t misr_value;                              /* 0x00000064 */
	uint32_t clkon_cfg;                               /* 0x00000068 */
	uint32_t core_clkon_cfg;                          /* 0x0000006C */
	uint8_t reserved3[144];
	/* QPIC */
	uint32_t qpic_qpic_version;			  /* 0x00000100 */
	uint32_t qpic_qpic_ctrl;			  /* 0x00000104 */
	uint32_t qpic_qpic_hwinfo;			  /* 0x00000108 */
	uint32_t qpic_qpic_reg_permission;		  /* 0x0000010C */
	uint32_t qpic_qpic_debug;			  /* 0x00000110 */
};

#define QPIC_EBI2CR_QPIC_LCDC_STTS		 0x079A2014
#define SW_RESET_DONE_SYNC			 (1 << 8)

/* Register: EBI2_CHIP_SELECT_CFG0 */
#define CS7_CFG_MASK                             0x00001000
#define CS7_CFG_DISABLE                          0x00000000
#define CS7_CFG_GENERAL_SRAM_MEMORY_INTERFACE    0x00001000
#define CS7_CFG(i)                               ((i) << 12)

#define CS6_CFG_MASK                             0x00000800
#define CS6_CFG_DISABLE                          0x00000000
#define CS6_CFG_GENERAL_SRAM_MEMORY_INTERFACE    0x00000800
#define CS6_CFG(i)                               ((i) << 11)

#define ETM_CS_CFG_MASK                          0x00000400
#define ETM_CS_CFG_DISABLE                       0x00000000
#define ETM_CS_CFG_GENERAL_SRAM_MEMORY_INTERFACE 0x00000400
#define ETM_CS_CFG(i)                            ((i) << 10)

#define CS5_CFG_MASK                             0x00000300
#define CS5_CFG_DISABLE                          0x00000000
#define CS5_CFG_LCD_DEVICE_CONNECTED             0x00000100
#define CS5_CFG_LCD_DEVICE_CHIP_ENABLE           0x00000200
#define CS5_CFG_GENERAL_SRAM_MEMORY_INTERFACE    0x00000300
#define CS5_CFG(i)                               ((i) << 8)

#define CS4_CFG_MASK                             0x000000c0
#define CS4_CFG_DISABLE                          0x00000000
#define CS4_CFG_LCD_DEVICE_CONNECTED             0x00000040
#define CS4_CFG_GENERAL_SRAM_MEMORY_INTERFACE    0x000000C0
#define CS4_CFG(i)                               ((i) << 6)

#define CS3_CFG_MASK                             0x00000020
#define CS3_CFG_DISABLE                          0x00000000
#define CS3_CFG_GENERAL_SRAM_MEMORY_INTERFACE    0x00000020
#define CS3_CFG(i)                               ((i) << 5)

#define CS2_CFG_MASK                             0x00000010
#define CS2_CFG_DISABLE                          0x00000000
#define CS2_CFG_GENERAL_SRAM_MEMORY_INTERFACE    0x00000010
#define CS2_CFG(i)                               ((i) << 4)

#define CS1_CFG_MASK                             0x0000000c
#define CS1_CFG_DISABLE                          0x00000000
#define CS1_CFG_SERIAL_FLASH_DEVICE              0x00000004
#define CS1_CFG_GENERAL_SRAM_MEMORY_INTERFACE    0x00000008
#define CS1_CFG(i)                               ((i) << 2)

#define CS0_CFG_MASK                             0x00000003
#define CS0_CFG_DISABLE                          0x00000000
#define CS0_CFG_SERIAL_FLASH_DEVICE              0x00000001
#define CS0_CFG_GENERAL_SRAM_MEMORY_INTERFACE    0x00000002
#define CS0_CFG(i)                               ((i) << 0)

/* Register: EBI2_CORE_CLKON_CFG */
#define GATE_NAND_ENA				(1 << 31)
#define GATE_LCD_ENA				(1 << 29)
#endif
