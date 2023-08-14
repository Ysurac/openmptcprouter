/*
 * Copyright (c) 2012 The Linux Foundation. All rights reserved.
 */
#ifndef QCOM_NAND_H
#define QCOM_NAND_H

#define IPQ806x_EBI2ND_BASE				(0x1ac00000)
#define IPQ40xx_EBI2ND_BASE				(0x079b0000)
#define IPQ40xx_QPIC_BAM_CTRL				(0x07984000)

struct ebi2nd_regs {
	uint32_t flash_cmd;                               /* 0x00000000 */
	uint32_t addr0;                                   /* 0x00000004 */
	uint32_t addr1;                                   /* 0x00000008 */
	uint32_t flash_chip_select;                       /* 0x0000000C */
	uint32_t exec_cmd;                                /* 0x00000010 */
	uint32_t flash_status;                            /* 0x00000014 */
	uint32_t buffer_status;                           /* 0x00000018 */
	uint32_t sflashc_status;                          /* 0x0000001C */
	uint32_t dev0_cfg0;                               /* 0x00000020 */
	uint32_t dev0_cfg1;                               /* 0x00000024 */
	uint32_t dev0_ecc_cfg;                            /* 0x00000028 */
	uint32_t dev1_ecc_cfg;                            /* 0x0000002C */
	uint32_t dev1_cfg0;                               /* 0x00000030 */
	uint32_t dev1_cfg1;                               /* 0x00000034 */
	uint32_t sflashc_cmd;                             /* 0x00000038 */
	uint32_t sflashc_exec_cmd;                        /* 0x0000003C */
	uint32_t flash_read_id;                           /* 0x00000040 */
	uint32_t flash_read_status;                       /* 0x00000044 */
	uint32_t flash_read_id2;                          /* 0x00000048 */
	uint8_t reserved0[4];
	uint32_t flash_config_data;                       /* 0x00000050 */
	uint32_t flash_config;                            /* 0x00000054 */
	uint32_t flash_config_mode;                       /* 0x00000058 */
	uint8_t reserved1[4];
	uint32_t flash_config_status;                     /* 0x00000060 */
	uint32_t macro1_reg;                              /* 0x00000064 */
	uint32_t hsddr_nand_cfg;                          /* 0x00000068 */
	uint8_t reserved2[4];
	uint32_t xfr_step1;                               /* 0x00000070 */
	uint32_t xfr_step2;                               /* 0x00000074 */
	uint32_t xfr_step3;                               /* 0x00000078 */
	uint32_t xfr_step4;                               /* 0x0000007C */
	uint32_t xfr_step5;                               /* 0x00000080 */
	uint32_t xfr_step6;                               /* 0x00000084 */
	uint32_t xfr_step7;                               /* 0x00000088 */
	uint8_t reserved3[4];
	uint32_t genp_reg0;                               /* 0x00000090 */
	uint32_t genp_reg1;                               /* 0x00000094 */
	uint32_t genp_reg2;                               /* 0x00000098 */
	uint32_t genp_reg3;                               /* 0x0000009C */
	uint32_t dev_cmd0;                                /* 0x000000A0 */
	uint32_t dev_cmd1;                                /* 0x000000A4 */
	uint32_t dev_cmd2;                                /* 0x000000A8 */
	uint32_t dev_cmd_vld;                             /* 0x000000AC */
	uint8_t reserved4[16];
	uint32_t addr2;                                   /* 0x000000C0 */
	uint32_t addr3;                                   /* 0x000000C4 */
	uint32_t addr4;                                   /* 0x000000C8 */
	uint32_t addr5;                                   /* 0x000000CC */
	uint32_t dev_cmd3;                                /* 0x000000D0 */
	uint32_t dev_cmd4;                                /* 0x000000D4 */
	uint32_t dev_cmd5;                                /* 0x000000D8 */
	uint32_t dev_cmd6;                                /* 0x000000DC */
	uint32_t sflashc_burst_cfg;                       /* 0x000000E0 */
	uint32_t addr6;                                   /* 0x000000E4 */
	uint32_t erased_cw_detect_cfg;                    /* 0x000000E8 */
	uint32_t erased_cw_detect_status;                 /* 0x000000EC */
	uint32_t ebi2_ecc_buf_cfg;                        /* 0x000000F0 */
	uint32_t dbg_cfg;                                 /* 0x000000F4 */
	uint32_t hw_profile_cfg;                          /* 0x000000F8 */
	uint32_t hw_info;                                 /* 0x000000FC */
	uint32_t buffn_acc[144];                          /* 0x00000100 */
	uint8_t reserved5[3008];			  /* 0x00000340 */
	uint32_t qpic_nand_ctrl;			  /* 0x00000f00 */
	uint32_t qpic_nand_status;			  /* 0x00000f04 */
	uint32_t qpic_nand_version;			  /* 0x00000f08 */
	uint32_t qpic_nand_debug;			  /* 0x00000f0c */
	uint32_t qpic_irq_stts;				  /* 0x00000f10 */
	uint32_t qpic_irq_clr;				  /* 0x00000f14 */
	uint32_t qpic_irq_en;				  /* 0x00000f18 */
	uint32_t qpic_nand_mutex;			  /* 0x00000f1c */
	uint32_t qpic_nand_read_location_0;		  /* 0x00000f20 */
	uint32_t qpic_nand_read_location_1;		  /* 0x00000f24 */
	uint32_t qpic_nand_read_location_2;		  /* 0x00000f28 */
	uint32_t qpic_nand_read_location_3;		  /* 0x00000f2c */
	uint32_t qpic_nand_read_location_4;		  /* 0x00000f30 */
	uint8_t reserved6[12];				  /* 0x00000f34 */
	uint32_t qpic_nand_config_bits;			  /* 0x00000f40 */
	uint32_t qpic_nand_mpu_bypass;			  /* 0x00000f44 */
};

#define QCOM_NAND_IPQ				0
#define QCOM_NAND_QPIC				1

/* Register: NAND_DEVn_CFG0 */
#define SET_RD_MODE_AFTER_STATUS_MASK            0x80000000
#define SET_RD_MODE_AFTER_STATUS_SEND_READ_CMD   0x80000000
#define SET_RD_MODE_AFTER_STATUS_DO_NOT_SEND     0x00000000
#define SET_RD_MODE_AFTER_STATUS(i)              ((i) << 31)

#define STATUS_BFR_READ_MASK                     0x40000000
#define STATUS_BFR_READ_DO_NOT_READ_STATUS       0x00000000
#define STATUS_BFR_READ_READ_STATUS_BEFORE_DATA  0x40000000
#define STATUS_BFR_READ(i)                       ((i) << 30)

#define NUM_ADDR_CYCLES_MASK                     0x38000000
#define NUM_ADDR_CYCLES_NO_ADDRESS_CYCLES        0x00000000
#define NUM_ADDR_CYCLES(i)                       ((i) << 27)

#define SPARE_SIZE_BYTES_MASK                    0x07800000
#define SPARE_SIZE_BYTES(i)                      ((i) << 23)

#define RS_ECC_PARITY_SIZE_BYTES_MASK            0x00780000
#define RS_ECC_PARITY_SIZE_BYTES(i)              ((i) << 19)

#define UD_SIZE_BYTES_MASK                       0x0007fe00
#define UD_SIZE_BYTES(i)                         ((i) << 9)

#define CW_PER_PAGE_MASK                         0x000001c0
#define CW_PER_PAGE_1_CODEWORD_PER_PAGE          0x00000000
#define CW_PER_PAGE_2_CODEWORDS_PER_PAGE         0x00000040
#define CW_PER_PAGE_3_CODEWORDS_PER_PAGE         0x00000080
#define CW_PER_PAGE_4_CODEWORDS_PER_PAGE         0x000000C0
#define CW_PER_PAGE_5_CODEWORDS_PER_PAGE         0x00000100
#define CW_PER_PAGE_6_CODEWORDS_PER_PAGE         0x00000140
#define CW_PER_PAGE_7_CODEWORDS_PER_PAGE         0x00000180
#define CW_PER_PAGE_8_CODEWORDS_PER_PAGE         0x000001C0
#define CW_PER_PAGE(i)                           ((i) << 6)

#define MSB_CW_PER_PAGE_MASK                     0x00000020
#define MSB_CW_PER_PAGE(i)                       ((i) << 5)

#define DISABLE_STATUS_AFTER_WRITE_MASK          0x00000010
#define DISABLE_STATUS_AFTER_WRITE(i)            ((i) << 4)

#define BUSY_TIMEOUT_ERROR_SELECT_MASK           0x0000000f
#define BUSY_TIMEOUT_ERROR_SELECT_16_MS          0x00000000
#define BUSY_TIMEOUT_ERROR_SELECT_32_MS          0x00000001
#define BUSY_TIMEOUT_ERROR_SELECT_64_MS          0x00000002
#define BUSY_TIMEOUT_ERROR_SELECT_128_MS         0x00000003
#define BUSY_TIMEOUT_ERROR_SELECT_256_MS         0x00000004
#define BUSY_TIMEOUT_ERROR_SELECT_512_MS         0x00000005
#define BUSY_TIMEOUT_ERROR_SELECT_1_SEC          0x00000006
#define BUSY_TIMEOUT_ERROR_SELECT_2_SEC          0x00000007
#define BUSY_TIMEOUT_ERROR_SELECT_1_MS           0x00000008
#define BUSY_TIMEOUT_ERROR_SELECT(i)             ((i) << 0)


/* Register: NAND_DEVn_CFG1 */
#define RS_ECC_MODE_MASK                         0x30000000
#define RS_ECC_MODE(i)                           ((i) << 28)

#define ENABLE_BCH_ECC_MASK                      0x08000000
#define ENABLE_BCH_ECC_ENABLES_TAVOR_ECC_CORE_WITH_BCH_ENCODING_DECODING 0x08000000
#define ENABLE_BCH_ECC(i)                        ((i) << 27)

#define DISABLE_ECC_RESET_AFTER_OPDONE_MASK      0x02000000
#define DISABLE_ECC_RESET_AFTER_OPDONE(i)        ((i) << 25)

#define ECC_DECODER_CGC_EN_MASK                  0x01000000
#define ECC_DECODER_CGC_EN_FREE_RUNNING_CLOCK    0x01000000
#define ECC_DECODER_CGC_EN(i)                    ((i) << 24)

#define ECC_ENCODER_CGC_EN_MASK                  0x00800000
#define ECC_ENCODER_CGC_EN_FREE_RUNNING_CLOCK    0x00800000
#define ECC_ENCODER_CGC_EN(i)                    ((i) << 23)

#define WR_RD_BSY_GAP_MASK                       0x007e0000
#define WR_RD_BSY_GAP_2_CLOCK_CYCLE_GAP          0x00000000
#define WR_RD_BSY_GAP_4_CLOCK_CYCLES_GAP         0x00020000
#define WR_RD_BSY_GAP_6_CLOCK_CYCLES_GAP         0x00040000
#define WR_RD_BSY_GAP_8_CLOCK_CYCLES_GAP         0x00060000
#define WR_RD_BSY_GAP_10_CLOCK_CYCLES_GAP        0x00080000
#define WR_RD_BSY_GAP_128_CLOCK_CYCLES_GAP       0x007E0000
#define WR_RD_BSY_GAP(i)                         ((i) << 17)

#define BAD_BLOCK_IN_SPARE_AREA_MASK             0x00010000
#define BAD_BLOCK_IN_SPARE_AREA_IN_USER_DATA_AREA 0x00000000
#define BAD_BLOCK_IN_SPARE_AREA_IN_SPARE_AREA    0x00010000
#define BAD_BLOCK_IN_SPARE_AREA(i)               ((i) << 16)

#define BAD_BLOCK_BYTE_NUM_MASK                  0x0000ffc0
#define BAD_BLOCK_BYTE_NUM(i)                    ((i) << 6)

#define CS_ACTIVE_BSY_MASK                       0x00000020
#define CS_ACTIVE_BSY_ASSERT_CS_DURING_BUSY      0x00000020
#define CS_ACTIVE_BSY_ALLOW_CS_DE_ASSERTION      0x00000000
#define CS_ACTIVE_BSY(i)                         ((i) << 5)

#define NAND_RECOVERY_CYCLES_MASK                0x0000001c
#define NAND_RECOVERY_CYCLES_1_RECOVERY_CYCLE    0x00000000
#define NAND_RECOVERY_CYCLES_2_RECOVERY_CYCLES   0x00000004
#define NAND_RECOVERY_CYCLES_3_RECOVERY_CYCLES   0x00000008
#define NAND_RECOVERY_CYCLES_8_RECOVERY_CYCLES   0x0000001C
#define NAND_RECOVERY_CYCLES(i)                  ((i) << 2)

#define WIDE_FLASH_MASK                          0x00000002
#define WIDE_FLASH_8_BIT_DATA_BUS                0x00000000
#define WIDE_FLASH_16_BIT_DATA_BUS               0x00000002
#define WIDE_FLASH(i)                            ((i) << 1)

#define ECC_DISABLE_MASK                         0x00000001
#define ECC_DISABLE_ECC_ENABLED                  0x00000000
#define ECC_DISABLE_ECC_DISABLED                 0x00000001
#define ECC_DISABLE(i)                           ((i) << 0)


/* Register: NAND_DEVn_ECC_CFG */
#define ECC_FORCE_CLK_OPEN_MASK                  0x40000000
#define ECC_FORCE_CLK_OPEN(i)                    ((i) << 30)

#define ECC_DEC_CLK_SHUTDOWN_MASK                0x20000000
#define ECC_DEC_CLK_SHUTDOWN(i)                  ((i) << 29)

#define ECC_ENC_CLK_SHUTDOWN_MASK                0x10000000
#define ECC_ENC_CLK_SHUTDOWN(i)                  ((i) << 28)

#define ECC_NUM_DATA_BYTES_MASK                  0x03ff0000
#define ECC_NUM_DATA_BYTES_516_BYTES             0x02040000
#define ECC_NUM_DATA_BYTES_517_BYTES             0x02050000
#define ECC_NUM_DATA_BYTES(i)                    ((i) << 16)

#define BCH_ECC_PARITY_SIZE_BYTES_MASK           0x00001f00
#define BCH_ECC_PARITY_SIZE_BYTES(i)             ((i) << 8)

#define BCH_ECC_MODE_MASK                        0x00000030
#define BCH_ECC_MODE_8_BIT_ECC_ERROR_DETECTION_CORRECTION 0x00000010
#define BCH_ECC_MODE_12_BIT_ECC_ERROR_DETECTION_CORRECTION 0x00000020
#define BCH_ECC_MODE_16_BIT_ECC_ERROR_DETECTION_CORRECTION 0x00000030
#define BCH_ECC_MODE(i)                          ((i) << 4)

#define ECC_SW_RESET_MASK                        0x00000002
#define ECC_SW_RESET_SOFTWARE_RESET_TO_ECC       0x00000002
#define ECC_SW_RESET(i)                          ((i) << 1)

#define BCH_ECC_DISABLE_MASK                     0x00000001
#define BCH_ECC_DISABLE_ECC_ENABLED              0x00000000
#define BCH_ECC_DISABLE(i)                       ((i) << 0)


/* Register: EBI2_ECC_BUF_CFG */
#define NUM_STEPS_MASK                           0x000003ff
#define NUM_STEPS(i)                             ((i) << 0)


/* Register: FLASH_DEV_CMD_VLD */
#define SEQ_READ_START_VLD_MASK                  0x00000010
#define SEQ_READ_START_VLD(i)                    ((i) << 4)

#define ERASE_START_VLD_MASK                     0x00000008
#define ERASE_START_VLD(i)                       ((i) << 3)

#define WRITE_START_VLD_MASK                     0x00000004
#define WRITE_START_VLD(i)                       ((i) << 2)

#define READ_STOP_VLD_MASK                       0x00000002
#define READ_STOP_VLD(i)                         ((i) << 1)

#define READ_START_VLD_MASK                      0x00000001
#define READ_START_VLD(i)                        ((i) << 0)


/* Register: NAND_ADDR0 */
#define DEV_ADDR0_MASK                           0xffffffff
#define DEV_ADDR0(i)                             ((i) << 0)


/* Register: NAND_ADDR1 */
#define DEV_ADDR1_MASK                           0xffffffff
#define DEV_ADDR1(i)                             ((i) << 0)


/* Register: NANDC_EXEC_CMD */
#define EXEC_CMD_MASK                            0x00000001
#define EXEC_CMD_EXECUTE_THE_COMMAND             0x00000001
#define EXEC_CMD(i)                              ((i) << 0)


/* Register: NAND_ERASED_CW_DETECT_CFG */
#define ERASED_CW_ECC_MASK_MASK                  0x00000002
#define ERASED_CW_ECC_MASK(i)                    ((i) << 1)

#define AUTO_DETECT_RES_MASK                     0x00000001
#define AUTO_DETECT_RES(i)                       ((i) << 0)


/* Register: NAND_ERASED_CW_DETECT_STATUS */
#define PAGE_ERASED_MASK                         0x00000020
#define PAGE_ERASED(i)                           ((i) << 5)

#define CODEWORD_ERASED_MASK                     0x00000010
#define CODEWORD_ERASED(i)                       ((i) << 4)

#define ERASED_CW_ECC_MASK_MASK                  0x00000002
#define ERASED_CW_ECC_MASK(i)                    ((i) << 1)

#define AUTO_DETECT_RES_MASK                     0x00000001
#define AUTO_DETECT_RES(i)                       ((i) << 0)


/* Register: NAND_FLASH_CMD */
#define ONE_NAND_INTR_STATUS_MASK                0x00040000
#define ONE_NAND_INTR_STATUS(i)                  ((i) << 18)

#define ONE_NAND_HOST_CFG_MASK                   0x00020000
#define ONE_NAND_HOST_CFG(i)                     ((i) << 17)

#define AUTO_DETECT_DATA_XFR_SIZE_MASK           0x0001ff80
#define AUTO_DETECT_DATA_XFR_SIZE(i)             ((i) << 7)

#define AUTO_DETECT_MASK                         0x00000040
#define AUTO_DETECT(i)                           ((i) << 6)

#define LAST_PAGE_MASK                           0x00000020
#define LAST_PAGE(i)                             ((i) << 5)

#define PAGE_ACC_MASK                            0x00000010
#define PAGE_ACC_PAGE_ACCESS_COMMAND             0x00000010
#define PAGE_ACC_NON_PAGE_ACCESS_COMMAND         0x00000000
#define PAGE_ACC(i)                              ((i) << 4)

#define OP_CMD_MASK                              0x0000000f
#define OP_CMD_RESERVED_0                        0x00000000
#define OP_CMD_ABORT_TRANSACTION                 0x00000001
#define OP_CMD_PAGE_READ                         0x00000002
#define OP_CMD_PAGE_READ_WITH_ECC                0x00000003
#define OP_CMD_PAGE_READ_WITH_ECC_SPARE          0x00000004
#define OP_CMD_RESERVED_5                        0x00000005
#define OP_CMD_PROGRAM_PAGE                      0x00000006
#define OP_CMD_PAGE_PROGRAM_WITH_ECC             0x00000007
#define OP_CMD_RESERVED_8                        0x00000008
#define OP_CMD_PROGRAM_PAGE_WITH_SPARE           0x00000009
#define OP_CMD_BLOCK_ERASE                       0x0000000A
#define OP_CMD_FETCH_ID                          0x0000000B
#define OP_CMD_CHECK_STATUS                      0x0000000C
#define OP_CMD_RESET_NAND_FLASH_DEVICE           0x0000000D
#define OP_CMD_RESERVED_E                        0x0000000E
#define OP_CMD_RESERVED_F                        0x0000000F
#define OP_CMD(i)                                ((i) << 0)


/* Register: NAND_FLASH_STATUS */
#define DEV_STATUS_MASK                          0xffff0000
#define DEV_STATUS(i)                            ((i) << 16)

#define CODEWORD_CNTR_MASK                       0x0000f000
#define CODEWORD_CNTR(i)                         ((i) << 12)

#define DEVICE_2KBYTE_MASK                       0x00000800
#define DEVICE_2KBYTE_2K_BYTE_PAGE_DEVICE        0x00000800
#define DEVICE_2KBYTE_NOT_A_2K_BYTE_PAGE_DEVICE  0x00000000
#define DEVICE_2KBYTE(i)                         ((i) << 11)

#define DEVICE_512BYTE_MASK                      0x00000400
#define DEVICE_512BYTE_512_BYTE_PAGE_DEVICE      0x00000400
#define DEVICE_512BYTE_NOT_A_512_BYTE_PAGE_DEVICE 0x00000000
#define DEVICE_512BYTE(i)                        ((i) << 10)

#define AUTO_DETECT_DONE_MASK                    0x00000200
#define AUTO_DETECT_DONE(i)                      ((i) << 9)

#define MPU_ERROR_MASK                           0x00000100
#define MPU_ERROR_MPU_ERROR_FOR_THE_ACCESS       0x00000100
#define MPU_ERROR_NO_ERROR                       0x00000000
#define MPU_ERROR(i)                             ((i) << 8)

#define PROG_ERASE_OP_RESULT_MASK                0x00000080
#define PROG_ERASE_OP_RESULT_SUCCESSFUL          0x00000000
#define PROG_ERASE_OP_RESULT_NOT_SUCCESSFUL      0x00000080
#define PROG_ERASE_OP_RESULT(i)                  ((i) << 7)

#define NANDC_TIMEOUT_ERR_MASK                   0x00000040
#define NANDC_TIMEOUT_ERR_NO_ERROR               0x00000000
#define NANDC_TIMEOUT_ERR_ERROR                  0x00000040
#define NANDC_TIMEOUT_ERR(i)                     ((i) << 6)

#define READY_BSY_N_MASK                         0x00000020
#define READY_BSY_N_EXTERNAL_FLASH_IS_BUSY       0x00000000
#define READY_BSY_N_EXTERNAL_FLASH_IS_READY      0x00000020
#define READY_BSY_N(i)                           ((i) << 5)

#define OP_ERR_MASK                              0x00000010
#define OP_ERR(i)                                ((i) << 4)

#define OPER_STATUS_MASK                         0x0000000f
#define OPER_STATUS_IDLE_STATE                   0x00000000
#define OPER_STATUS_ABORT_TRANSACTION            0x00000001
#define OPER_STATUS_PAGE_READ                    0x00000002
#define OPER_STATUS_PAGE_READ_WITH_ECC           0x00000003
#define OPER_STATUS_PAGE_READ_WITH_ECC_AND_SPARE_DATA 0x00000004
#define OPER_STATUS_SEQUENTIAL_PAGE_READ         0x00000005
#define OPER_STATUS_PROGRAM_PAGE                 0x00000006
#define OPER_STATUS_PROGRAM_PAGE_WITH_ECC        0x00000007
#define OPER_STATUS_RESERVED_PROGRAMMING         0x00000008
#define OPER_STATUS_PROGRAM_PAGE_WITH_SPARE      0x00000009
#define OPER_STATUS_BLOCK_ERASE                  0x0000000A
#define OPER_STATUS_FETCH_ID                     0x0000000B
#define OPER_STATUS_CHECK_STATUS                 0x0000000C
#define OPER_STATUS_RESET_FLASH_DEVICE           0x0000000D
#define OPER_STATUS(i)                           ((i) << 0)


/* Register: NANDC_BUFFER_STATUS */
#define BAD_BLOCK_STATUS_MASK                    0xffff0000
#define BAD_BLOCK_STATUS(i)                      ((i) << 16)

#define XFR_STEP2_REG_UPDATE_DONE_MASK           0x00000200
#define XFR_STEP2_REG_UPDATE_DONE(i)             ((i) << 9)

#define UNCORRECTABLE_MASK                       0x00000100
#define UNCORRECTABLE(i)                         ((i) << 8)

#define NUM_ERRORS_MASK                          0x0000001f
#define NUM_ERRORS(i)                            ((i) << 0)


/* Register: FLASH_DEV_CMD1 */
#define SEQ_READ_MODE_START_MASK                 0xff000000
#define SEQ_READ_MODE_START(i)                   ((i) << 24)

#define SEQ_READ_MODE_ADDR_MASK                  0x00ff0000
#define SEQ_READ_MODE_ADDR(i)                    ((i) << 16)

#define READ_START_MASK                          0x0000ff00
#define READ_START(i)                            ((i) << 8)

#define READ_ADDR_MASK                           0x000000ff
#define READ_ADDR(i)                             ((i) << 0)

/* Register: NAND_DEBUG */
#define BAM_MODE_BIT_RESET			(1 << 31)

/* Register: NAND_CTRL */
#define BAM_MODE_EN				0x1

/* Register: BAM_CTRL */
#define BAM_CTRL_CGC				0x00020000

#endif
