/*
 * MPC8xx Communication Processor Module.
 * Copyright (c) 1997 Dan Malek (dmalek@jlc.net)
 *
 * (C) Copyright 2000-2006
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * This file contains structures and information for the communication
 * processor channels.  Some CPM control and status is available
 * throught the MPC8xx internal memory map.  See immap.h for details.
 * This file only contains what I need for the moment, not the total
 * CPM capabilities.  I (or someone else) will add definitions as they
 * are needed.  -- Dan
 *
 * On the MBX board, EPPC-Bug loads CPM microcode into the first 512
 * bytes of the DP RAM and relocates the I2C parameter area to the
 * IDMA1 space.  The remaining DP RAM is available for buffer descriptors
 * or other use.
 */
#ifndef __CPM_8XX__
#define __CPM_8XX__

#include <linux/config.h>
#include <asm/8xx_immap.h>

/* CPM Command register.
*/
#define CPM_CR_RST		((ushort)0x8000)
#define CPM_CR_OPCODE		((ushort)0x0f00)
#define CPM_CR_CHAN		((ushort)0x00f0)
#define CPM_CR_FLG		((ushort)0x0001)

/* Some commands (there are more...later)
*/
#define CPM_CR_INIT_TRX		((ushort)0x0000)
#define CPM_CR_INIT_RX		((ushort)0x0001)
#define CPM_CR_INIT_TX		((ushort)0x0002)
#define CPM_CR_HUNT_MODE	((ushort)0x0003)
#define CPM_CR_STOP_TX		((ushort)0x0004)
#define CPM_CR_RESTART_TX	((ushort)0x0006)
#define CPM_CR_SET_GADDR	((ushort)0x0008)

/* Channel numbers.
*/
#define CPM_CR_CH_SCC1		((ushort)0x0000)
#define CPM_CR_CH_I2C		((ushort)0x0001)    /* I2C and IDMA1 */
#define CPM_CR_CH_SCC2		((ushort)0x0004)
#define CPM_CR_CH_SPI		((ushort)0x0005)    /* SPI/IDMA2/Timers */
#define CPM_CR_CH_SCC3		((ushort)0x0008)
#define CPM_CR_CH_SMC1		((ushort)0x0009)    /* SMC1 / DSP1 */
#define CPM_CR_CH_SCC4		((ushort)0x000c)
#define CPM_CR_CH_SMC2		((ushort)0x000d)    /* SMC2 / DSP2 */

#define mk_cr_cmd(CH, CMD)	((CMD << 8) | (CH << 4))

/*
 * DPRAM defines and allocation functions
 */

/* The dual ported RAM is multi-functional.  Some areas can be (and are
 * being) used for microcode.  There is an area that can only be used
 * as data ram for buffer descriptors, which is all we use right now.
 * Currently the first 512 and last 256 bytes are used for microcode.
 */
#ifdef  CONFIG_SYS_ALLOC_DPRAM

#define CPM_DATAONLY_BASE	((uint)0x0800)
#define CPM_DATAONLY_SIZE	((uint)0x0700)
#define CPM_DP_NOSPACE		((uint)0x7fffffff)

#else

#define CPM_SERIAL_BASE		0x0800
#define CPM_I2C_BASE		0x0820
#define CPM_SPI_BASE		0x0840
#define CPM_FEC_BASE		0x0860
#define CPM_SERIAL2_BASE	0x08E0
#define CPM_SCC_BASE		0x0900
#define CPM_POST_BASE		0x0980
#define CPM_WLKBD_BASE		0x0a00

#endif

#ifndef CONFIG_SYS_CPM_POST_WORD_ADDR
#define CPM_POST_WORD_ADDR	0x07FC
#else
#define CPM_POST_WORD_ADDR	CONFIG_SYS_CPM_POST_WORD_ADDR
#endif

#ifndef CONFIG_SYS_CPM_BOOTCOUNT_ADDR
#define CPM_BOOTCOUNT_ADDR	(CPM_POST_WORD_ADDR - 2*sizeof(ulong))
#else
#define CPM_BOOTCOUNT_ADDR	CONFIG_SYS_CPM_BOOTCOUNT_ADDR
#endif

#define BD_IIC_START	((uint) 0x0400) /* <- please use CPM_I2C_BASE !! */

/* Export the base address of the communication processor registers
 * and dual port ram.
 */
extern	cpm8xx_t	*cpmp;		/* Pointer to comm processor */

/* Buffer descriptors used by many of the CPM protocols.
*/
typedef struct cpm_buf_desc {
	ushort	cbd_sc;		/* Status and Control */
	ushort	cbd_datlen;	/* Data length in buffer */
	uint	cbd_bufaddr;	/* Buffer address in host memory */
} cbd_t;

#define BD_SC_EMPTY	((ushort)0x8000)	/* Receive is empty */
#define BD_SC_READY	((ushort)0x8000)	/* Transmit is ready */
#define BD_SC_WRAP	((ushort)0x2000)	/* Last buffer descriptor */
#define BD_SC_INTRPT	((ushort)0x1000)	/* Interrupt on change */
#define BD_SC_LAST	((ushort)0x0800)	/* Last buffer in frame */
#define BD_SC_TC	((ushort)0x0400)	/* Transmit CRC */
#define BD_SC_CM	((ushort)0x0200)	/* Continous mode */
#define BD_SC_ID	((ushort)0x0100)	/* Rec'd too many idles */
#define BD_SC_P		((ushort)0x0100)	/* xmt preamble */
#define BD_SC_BR	((ushort)0x0020)	/* Break received */
#define BD_SC_FR	((ushort)0x0010)	/* Framing error */
#define BD_SC_PR	((ushort)0x0008)	/* Parity error */
#define BD_SC_OV	((ushort)0x0002)	/* Overrun */
#define BD_SC_CD	((ushort)0x0001)	/* Carrier Detect lost */

/* Parameter RAM offsets.
*/
#define PROFF_SCC1	((uint)0x0000)
#define PROFF_IIC	((uint)0x0080)
#define PROFF_SCC2	((uint)0x0100)
#define PROFF_SPI	((uint)0x0180)
#define PROFF_SCC3	((uint)0x0200)
#define PROFF_SMC1	((uint)0x0280)
#define PROFF_SCC4	((uint)0x0300)
#define PROFF_SMC2	((uint)0x0380)

/* Define enough so I can at least use the serial port as a UART.
 * The MBX uses SMC1 as the host serial port.
 */
typedef struct smc_uart {
	ushort	smc_rbase;	/* Rx Buffer descriptor base address */
	ushort	smc_tbase;	/* Tx Buffer descriptor base address */
	u_char	smc_rfcr;	/* Rx function code */
	u_char	smc_tfcr;	/* Tx function code */
	ushort	smc_mrblr;	/* Max receive buffer length */
	uint	smc_rstate;	/* Internal */
	uint	smc_idp;	/* Internal */
	ushort	smc_rbptr;	/* Internal */
	ushort	smc_ibc;	/* Internal */
	uint	smc_rxtmp;	/* Internal */
	uint	smc_tstate;	/* Internal */
	uint	smc_tdp;	/* Internal */
	ushort	smc_tbptr;	/* Internal */
	ushort	smc_tbc;	/* Internal */
	uint	smc_txtmp;	/* Internal */
	ushort	smc_maxidl;	/* Maximum idle characters */
	ushort	smc_tmpidl;	/* Temporary idle counter */
	ushort	smc_brklen;	/* Last received break length */
	ushort	smc_brkec;	/* rcv'd break condition counter */
	ushort	smc_brkcr;	/* xmt break count register */
	ushort	smc_rmask;	/* Temporary bit mask */
	u_char	res1[8];
	ushort	smc_rpbase;	/* Relocation pointer */
} smc_uart_t;

/* Function code bits.
*/
#define SMC_EB	((u_char)0x10)	/* Set big endian byte order */

/* SMC uart mode register.
*/
#define	SMCMR_REN	((ushort)0x0001)
#define SMCMR_TEN	((ushort)0x0002)
#define SMCMR_DM	((ushort)0x000c)
#define SMCMR_SM_GCI	((ushort)0x0000)
#define SMCMR_SM_UART	((ushort)0x0020)
#define SMCMR_SM_TRANS	((ushort)0x0030)
#define SMCMR_SM_MASK	((ushort)0x0030)
#define SMCMR_PM_EVEN	((ushort)0x0100)	/* Even parity, else odd */
#define SMCMR_REVD	SMCMR_PM_EVEN
#define SMCMR_PEN	((ushort)0x0200)	/* Parity enable */
#define SMCMR_BS	SMCMR_PEN
#define SMCMR_SL	((ushort)0x0400)	/* Two stops, else one */
#define SMCR_CLEN_MASK	((ushort)0x7800)	/* Character length */
#define smcr_mk_clen(C)	(((C) << 11) & SMCR_CLEN_MASK)

/* SMC2 as Centronics parallel printer.  It is half duplex, in that
 * it can only receive or transmit.  The parameter ram values for
 * each direction are either unique or properly overlap, so we can
 * include them in one structure.
 */
typedef struct smc_centronics {
	ushort	scent_rbase;
	ushort	scent_tbase;
	u_char	scent_cfcr;
	u_char	scent_smask;
	ushort	scent_mrblr;
	uint	scent_rstate;
	uint	scent_r_ptr;
	ushort	scent_rbptr;
	ushort	scent_r_cnt;
	uint	scent_rtemp;
	uint	scent_tstate;
	uint	scent_t_ptr;
	ushort	scent_tbptr;
	ushort	scent_t_cnt;
	uint	scent_ttemp;
	ushort	scent_max_sl;
	ushort	scent_sl_cnt;
	ushort	scent_character1;
	ushort	scent_character2;
	ushort	scent_character3;
	ushort	scent_character4;
	ushort	scent_character5;
	ushort	scent_character6;
	ushort	scent_character7;
	ushort	scent_character8;
	ushort	scent_rccm;
	ushort	scent_rccr;
} smc_cent_t;

/* Centronics Status Mask Register.
*/
#define SMC_CENT_F	((u_char)0x08)
#define SMC_CENT_PE	((u_char)0x04)
#define SMC_CENT_S	((u_char)0x02)

/* SMC Event and Mask register.
*/
#define	SMCM_BRKE	((unsigned char)0x40)	/* When in UART Mode */
#define	SMCM_BRK	((unsigned char)0x10)	/* When in UART Mode */
#define	SMCM_TXE	((unsigned char)0x10)	/* When in Transparent Mode */
#define	SMCM_BSY	((unsigned char)0x04)
#define	SMCM_TX		((unsigned char)0x02)
#define	SMCM_RX		((unsigned char)0x01)

/* Baud rate generators.
*/
#define CPM_BRG_RST		((uint)0x00020000)
#define CPM_BRG_EN		((uint)0x00010000)
#define CPM_BRG_EXTC_INT	((uint)0x00000000)
#define CPM_BRG_EXTC_CLK2	((uint)0x00004000)
#define CPM_BRG_EXTC_CLK6	((uint)0x00008000)
#define CPM_BRG_ATB		((uint)0x00002000)
#define CPM_BRG_CD_MASK		((uint)0x00001ffe)
#define CPM_BRG_DIV16		((uint)0x00000001)

/* SI Clock Route Register
*/
#define SICR_RCLK_SCC1_BRG1	((uint)0x00000000)
#define SICR_TCLK_SCC1_BRG1	((uint)0x00000000)
#define SICR_RCLK_SCC2_BRG2	((uint)0x00000800)
#define SICR_TCLK_SCC2_BRG2	((uint)0x00000100)
#define SICR_RCLK_SCC3_BRG3	((uint)0x00100000)
#define SICR_TCLK_SCC3_BRG3	((uint)0x00020000)
#define SICR_RCLK_SCC4_BRG4	((uint)0x18000000)
#define SICR_TCLK_SCC4_BRG4	((uint)0x03000000)

/* SCCs.
*/
#define SCC_GSMRH_IRP		((uint)0x00040000)
#define SCC_GSMRH_GDE		((uint)0x00010000)
#define SCC_GSMRH_TCRC_CCITT	((uint)0x00008000)
#define SCC_GSMRH_TCRC_BISYNC	((uint)0x00004000)
#define SCC_GSMRH_TCRC_HDLC	((uint)0x00000000)
#define SCC_GSMRH_REVD		((uint)0x00002000)
#define SCC_GSMRH_TRX		((uint)0x00001000)
#define SCC_GSMRH_TTX		((uint)0x00000800)
#define SCC_GSMRH_CDP		((uint)0x00000400)
#define SCC_GSMRH_CTSP		((uint)0x00000200)
#define SCC_GSMRH_CDS		((uint)0x00000100)
#define SCC_GSMRH_CTSS		((uint)0x00000080)
#define SCC_GSMRH_TFL		((uint)0x00000040)
#define SCC_GSMRH_RFW		((uint)0x00000020)
#define SCC_GSMRH_TXSY		((uint)0x00000010)
#define SCC_GSMRH_SYNL16	((uint)0x0000000c)
#define SCC_GSMRH_SYNL8		((uint)0x00000008)
#define SCC_GSMRH_SYNL4		((uint)0x00000004)
#define SCC_GSMRH_RTSM		((uint)0x00000002)
#define SCC_GSMRH_RSYN		((uint)0x00000001)

#define SCC_GSMRL_SIR		((uint)0x80000000)	/* SCC2 only */
#define SCC_GSMRL_EDGE_NONE	((uint)0x60000000)
#define SCC_GSMRL_EDGE_NEG	((uint)0x40000000)
#define SCC_GSMRL_EDGE_POS	((uint)0x20000000)
#define SCC_GSMRL_EDGE_BOTH	((uint)0x00000000)
#define SCC_GSMRL_TCI		((uint)0x10000000)
#define SCC_GSMRL_TSNC_3	((uint)0x0c000000)
#define SCC_GSMRL_TSNC_4	((uint)0x08000000)
#define SCC_GSMRL_TSNC_14	((uint)0x04000000)
#define SCC_GSMRL_TSNC_INF	((uint)0x00000000)
#define SCC_GSMRL_RINV		((uint)0x02000000)
#define SCC_GSMRL_TINV		((uint)0x01000000)
#define SCC_GSMRL_TPL_128	((uint)0x00c00000)
#define SCC_GSMRL_TPL_64	((uint)0x00a00000)
#define SCC_GSMRL_TPL_48	((uint)0x00800000)
#define SCC_GSMRL_TPL_32	((uint)0x00600000)
#define SCC_GSMRL_TPL_16	((uint)0x00400000)
#define SCC_GSMRL_TPL_8		((uint)0x00200000)
#define SCC_GSMRL_TPL_NONE	((uint)0x00000000)
#define SCC_GSMRL_TPP_ALL1	((uint)0x00180000)
#define SCC_GSMRL_TPP_01	((uint)0x00100000)
#define SCC_GSMRL_TPP_10	((uint)0x00080000)
#define SCC_GSMRL_TPP_ZEROS	((uint)0x00000000)
#define SCC_GSMRL_TEND		((uint)0x00040000)
#define SCC_GSMRL_TDCR_32	((uint)0x00030000)
#define SCC_GSMRL_TDCR_16	((uint)0x00020000)
#define SCC_GSMRL_TDCR_8	((uint)0x00010000)
#define SCC_GSMRL_TDCR_1	((uint)0x00000000)
#define SCC_GSMRL_RDCR_32	((uint)0x0000c000)
#define SCC_GSMRL_RDCR_16	((uint)0x00008000)
#define SCC_GSMRL_RDCR_8	((uint)0x00004000)
#define SCC_GSMRL_RDCR_1	((uint)0x00000000)
#define SCC_GSMRL_RENC_DFMAN	((uint)0x00003000)
#define SCC_GSMRL_RENC_MANCH	((uint)0x00002000)
#define SCC_GSMRL_RENC_FM0	((uint)0x00001000)
#define SCC_GSMRL_RENC_NRZI	((uint)0x00000800)
#define SCC_GSMRL_RENC_NRZ	((uint)0x00000000)
#define SCC_GSMRL_TENC_DFMAN	((uint)0x00000600)
#define SCC_GSMRL_TENC_MANCH	((uint)0x00000400)
#define SCC_GSMRL_TENC_FM0	((uint)0x00000200)
#define SCC_GSMRL_TENC_NRZI	((uint)0x00000100)
#define SCC_GSMRL_TENC_NRZ	((uint)0x00000000)
#define SCC_GSMRL_DIAG_LE	((uint)0x000000c0)	/* Loop and echo */
#define SCC_GSMRL_DIAG_ECHO	((uint)0x00000080)
#define SCC_GSMRL_DIAG_LOOP	((uint)0x00000040)
#define SCC_GSMRL_DIAG_NORM	((uint)0x00000000)
#define SCC_GSMRL_ENR		((uint)0x00000020)
#define SCC_GSMRL_ENT		((uint)0x00000010)
#define SCC_GSMRL_MODE_ENET	((uint)0x0000000c)
#define SCC_GSMRL_MODE_DDCMP	((uint)0x00000009)
#define SCC_GSMRL_MODE_BISYNC	((uint)0x00000008)
#define SCC_GSMRL_MODE_V14	((uint)0x00000007)
#define SCC_GSMRL_MODE_AHDLC	((uint)0x00000006)
#define SCC_GSMRL_MODE_PROFIBUS	((uint)0x00000005)
#define SCC_GSMRL_MODE_UART	((uint)0x00000004)
#define SCC_GSMRL_MODE_SS7	((uint)0x00000003)
#define SCC_GSMRL_MODE_ATALK	((uint)0x00000002)
#define SCC_GSMRL_MODE_HDLC	((uint)0x00000000)

#define SCC_TODR_TOD		((ushort)0x8000)

/* SCC Event and Mask register.
*/
#define	SCCM_TXE	((unsigned char)0x10)
#define	SCCM_BSY	((unsigned char)0x04)
#define	SCCM_TX		((unsigned char)0x02)
#define	SCCM_RX		((unsigned char)0x01)

typedef struct scc_param {
	ushort	scc_rbase;	/* Rx Buffer descriptor base address */
	ushort	scc_tbase;	/* Tx Buffer descriptor base address */
	u_char	scc_rfcr;	/* Rx function code */
	u_char	scc_tfcr;	/* Tx function code */
	ushort	scc_mrblr;	/* Max receive buffer length */
	uint	scc_rstate;	/* Internal */
	uint	scc_idp;	/* Internal */
	ushort	scc_rbptr;	/* Internal */
	ushort	scc_ibc;	/* Internal */
	uint	scc_rxtmp;	/* Internal */
	uint	scc_tstate;	/* Internal */
	uint	scc_tdp;	/* Internal */
	ushort	scc_tbptr;	/* Internal */
	ushort	scc_tbc;	/* Internal */
	uint	scc_txtmp;	/* Internal */
	uint	scc_rcrc;	/* Internal */
	uint	scc_tcrc;	/* Internal */
} sccp_t;

/* Function code bits.
*/
#define SCC_EB	((u_char)0x10)	/* Set big endian byte order */

/* CPM Ethernet through SCCx.
 */
typedef struct scc_enet {
	sccp_t	sen_genscc;
	uint	sen_cpres;	/* Preset CRC */
	uint	sen_cmask;	/* Constant mask for CRC */
	uint	sen_crcec;	/* CRC Error counter */
	uint	sen_alec;	/* alignment error counter */
	uint	sen_disfc;	/* discard frame counter */
	ushort	sen_pads;	/* Tx short frame pad character */
	ushort	sen_retlim;	/* Retry limit threshold */
	ushort	sen_retcnt;	/* Retry limit counter */
	ushort	sen_maxflr;	/* maximum frame length register */
	ushort	sen_minflr;	/* minimum frame length register */
	ushort	sen_maxd1;	/* maximum DMA1 length */
	ushort	sen_maxd2;	/* maximum DMA2 length */
	ushort	sen_maxd;	/* Rx max DMA */
	ushort	sen_dmacnt;	/* Rx DMA counter */
	ushort	sen_maxb;	/* Max BD byte count */
	ushort	sen_gaddr1;	/* Group address filter */
	ushort	sen_gaddr2;
	ushort	sen_gaddr3;
	ushort	sen_gaddr4;
	uint	sen_tbuf0data0;	/* Save area 0 - current frame */
	uint	sen_tbuf0data1;	/* Save area 1 - current frame */
	uint	sen_tbuf0rba;	/* Internal */
	uint	sen_tbuf0crc;	/* Internal */
	ushort	sen_tbuf0bcnt;	/* Internal */
	ushort	sen_paddrh;	/* physical address (MSB) */
	ushort	sen_paddrm;
	ushort	sen_paddrl;	/* physical address (LSB) */
	ushort	sen_pper;	/* persistence */
	ushort	sen_rfbdptr;	/* Rx first BD pointer */
	ushort	sen_tfbdptr;	/* Tx first BD pointer */
	ushort	sen_tlbdptr;	/* Tx last BD pointer */
	uint	sen_tbuf1data0;	/* Save area 0 - current frame */
	uint	sen_tbuf1data1;	/* Save area 1 - current frame */
	uint	sen_tbuf1rba;	/* Internal */
	uint	sen_tbuf1crc;	/* Internal */
	ushort	sen_tbuf1bcnt;	/* Internal */
	ushort	sen_txlen;	/* Tx Frame length counter */
	ushort	sen_iaddr1;	/* Individual address filter */
	ushort	sen_iaddr2;
	ushort	sen_iaddr3;
	ushort	sen_iaddr4;
	ushort	sen_boffcnt;	/* Backoff counter */

	/* NOTE: Some versions of the manual have the following items
	 * incorrectly documented.  Below is the proper order.
	 */
	ushort	sen_taddrh;	/* temp address (MSB) */
	ushort	sen_taddrm;
	ushort	sen_taddrl;	/* temp address (LSB) */
} scc_enet_t;

/**********************************************************************
 *
 * Board specific configuration settings.
 *
 * Please note that we use the presence of a #define SCC_ENET and/or
 * #define FEC_ENET to enable the SCC resp. FEC ethernet drivers.
 **********************************************************************/


/***  ADS  *************************************************************/

#if defined(CONFIG_MPC860) && defined(CONFIG_ADS)
/* This ENET stuff is for the MPC860ADS with ethernet on SCC1.
 */

#define	PROFF_ENET	PROFF_SCC1
#define	CPM_CR_ENET	CPM_CR_CH_SCC1
#define	SCC_ENET	0

#define PA_ENET_RXD	((ushort)0x0001)
#define PA_ENET_TXD	((ushort)0x0002)
#define PA_ENET_TCLK	((ushort)0x0100)
#define PA_ENET_RCLK	((ushort)0x0200)

#define PB_ENET_TENA	((uint)0x00001000)

#define PC_ENET_CLSN	((ushort)0x0010)
#define PC_ENET_RENA	((ushort)0x0020)

#define SICR_ENET_MASK	((uint)0x000000ff)
#define SICR_ENET_CLKRT	((uint)0x0000002c)

/* 68160 PHY control */

#define PC_ENET_ETHLOOP ((ushort)0x0800)
#define PC_ENET_TPFLDL	((ushort)0x0400)
#define PC_ENET_TPSQEL  ((ushort)0x0200)

#endif	/* MPC860ADS */

/***  AMX860  **********************************************/

#if defined(CONFIG_AMX860)

/* This ENET stuff is for the AMX860 with ethernet on SCC1.
 */

#define PROFF_ENET	PROFF_SCC1
#define CPM_CR_ENET	CPM_CR_CH_SCC1
#define SCC_ENET	0

#define PA_ENET_RXD	((ushort)0x0001)
#define PA_ENET_TXD	((ushort)0x0002)
#define PA_ENET_TCLK	((ushort)0x0400)
#define PA_ENET_RCLK	((ushort)0x0800)

#define PB_ENET_TENA	((uint)0x00001000)

#define PC_ENET_CLSN	((ushort)0x0010)
#define PC_ENET_RENA	((ushort)0x0020)

#define SICR_ENET_MASK	((uint)0x000000ff)
#define SICR_ENET_CLKRT	((uint)0x0000003e)

/* 68160 PHY control */

#define PB_ENET_ETHLOOP	((uint)0x00020000)
#define PB_ENET_TPFLDL	((uint)0x00010000)
#define PB_ENET_TPSQEL	((uint)0x00008000)
#define PD_ENET_ETH_EN	((ushort)0x0004)

#endif	/* CONFIG_AMX860 */

/***  BSEIP  **********************************************************/

#ifdef CONFIG_BSEIP
/* This ENET stuff is for the MPC823 with ethernet on SCC2.
 * This is unique to the BSE ip-Engine board.
 */
#define	PROFF_ENET	PROFF_SCC2
#define	CPM_CR_ENET	CPM_CR_CH_SCC2
#define	SCC_ENET	1
#define PA_ENET_RXD	((ushort)0x0004)
#define PA_ENET_TXD	((ushort)0x0008)
#define PA_ENET_TCLK	((ushort)0x0100)
#define PA_ENET_RCLK	((ushort)0x0200)
#define PB_ENET_TENA	((uint)0x00002000)
#define PC_ENET_CLSN	((ushort)0x0040)
#define PC_ENET_RENA	((ushort)0x0080)

/* BSE uses port B and C bits for PHY control also.
*/
#define PB_BSE_POWERUP	((uint)0x00000004)
#define PB_BSE_FDXDIS	((uint)0x00008000)
#define PC_BSE_LOOPBACK	((ushort)0x0800)

#define SICR_ENET_MASK	((uint)0x0000ff00)
#define SICR_ENET_CLKRT	((uint)0x00002c00)
#endif	/* CONFIG_BSEIP */

/***  BSEIP  **********************************************************/

#ifdef CONFIG_FLAGADM
/* Enet configuration for the FLAGADM */
/* Enet on SCC2 */

#define	PROFF_ENET	PROFF_SCC2
#define	CPM_CR_ENET	CPM_CR_CH_SCC2
#define	SCC_ENET	1
#define PA_ENET_RXD	((ushort)0x0004)
#define PA_ENET_TXD	((ushort)0x0008)
#define PA_ENET_TCLK	((ushort)0x0100)
#define PA_ENET_RCLK	((ushort)0x0400)
#define PB_ENET_TENA	((uint)0x00002000)
#define PC_ENET_CLSN	((ushort)0x0040)
#define PC_ENET_RENA	((ushort)0x0080)

#define SICR_ENET_MASK	((uint)0x0000ff00)
#define SICR_ENET_CLKRT	((uint)0x00003400)
#endif	/* CONFIG_FLAGADM */

/***  C2MON  **********************************************************/

#ifdef CONFIG_C2MON

# ifndef CONFIG_FEC_ENET	/* use SCC for 10Mbps Ethernet	*/
#  error "Ethernet on SCC not supported on C2MON Board!"
# else				/* Use FEC for Fast Ethernet */

#undef	SCC_ENET
#define FEC_ENET

#define PD_MII_TXD1	((ushort)0x1000)	/* PD  3 */
#define PD_MII_TXD2	((ushort)0x0800)	/* PD  4 */
#define PD_MII_TXD3	((ushort)0x0400)	/* PD  5 */
#define PD_MII_RX_DV	((ushort)0x0200)	/* PD  6 */
#define PD_MII_RX_ERR	((ushort)0x0100)	/* PD  7 */
#define PD_MII_RX_CLK	((ushort)0x0080)	/* PD  8 */
#define PD_MII_TXD0	((ushort)0x0040)	/* PD  9 */
#define PD_MII_RXD0	((ushort)0x0020)	/* PD 10 */
#define PD_MII_TX_ERR	((ushort)0x0010)	/* PD 11 */
#define PD_MII_MDC	((ushort)0x0008)	/* PD 12 */
#define PD_MII_RXD1	((ushort)0x0004)	/* PD 13 */
#define PD_MII_RXD2	((ushort)0x0002)	/* PD 14 */
#define PD_MII_RXD3	((ushort)0x0001)	/* PD 15 */

#define PD_MII_MASK	((ushort)0x1FFF)	/* PD 3...15 */

# endif	/* CONFIG_FEC_ENET */
#endif	/* CONFIG_C2MON */

/*********************************************************************/

/***  ELPT860 *********************************************************/

#ifdef CONFIG_ELPT860
/* Bits in parallel I/O port registers that have to be set/cleared
 * to configure the pins for SCC1 use.
 */
#  define PROFF_ENET        PROFF_SCC1
#  define CPM_CR_ENET       CPM_CR_CH_SCC1
#  define SCC_ENET          0

#  define PA_ENET_RXD       ((ushort)0x0001)	/* PA 15 */
#  define PA_ENET_TXD       ((ushort)0x0002)	/* PA 14 */
#  define PA_ENET_RCLK      ((ushort)0x0100)	/* PA  7 */
#  define PA_ENET_TCLK      ((ushort)0x0200)	/* PA  6 */

#  define PC_ENET_TENA      ((ushort)0x0001)	/* PC 15 */
#  define PC_ENET_CLSN      ((ushort)0x0010)	/* PC 11 */
#  define PC_ENET_RENA      ((ushort)0x0020)	/* PC 10 */

/* Control bits in the SICR to route TCLK (CLK2) and RCLK (CLK1) to
 * SCC1.  Also, make sure GR1 (bit 24) and SC1 (bit 25) are zero.
 */
#  define SICR_ENET_MASK    ((uint)0x000000FF)
#  define SICR_ENET_CLKRT   ((uint)0x00000025)
#endif	/* CONFIG_ELPT860 */

/***  ESTEEM 192E  **************************************************/
#ifdef CONFIG_ESTEEM192E
/* ESTEEM192E
 * This ENET stuff is for the MPC850 with ethernet on SCC2. This
 * is very similar to the RPX-Lite configuration.
 * Note TENA , LOOPBACK , FDPLEX_DIS on Port B.
 */

#define	PROFF_ENET	PROFF_SCC2
#define	CPM_CR_ENET	CPM_CR_CH_SCC2
#define	SCC_ENET	1

#define PA_ENET_RXD	((ushort)0x0004)
#define PA_ENET_TXD	((ushort)0x0008)
#define PA_ENET_TCLK	((ushort)0x0200)
#define PA_ENET_RCLK	((ushort)0x0800)
#define PB_ENET_TENA	((uint)0x00002000)
#define PC_ENET_CLSN	((ushort)0x0040)
#define PC_ENET_RENA	((ushort)0x0080)

#define SICR_ENET_MASK	((uint)0x0000ff00)
#define SICR_ENET_CLKRT	((uint)0x00003d00)

#define PB_ENET_LOOPBACK ((uint)0x00004000)
#define PB_ENET_FDPLEX_DIS ((uint)0x00008000)

#endif

/***  FADS823  ********************************************************/

#if defined(CONFIG_MPC823FADS) && defined(CONFIG_FADS)
/* This ENET stuff is for the MPC823FADS with ethernet on SCC2.
 */
#ifdef CONFIG_SCC2_ENET
#define	PROFF_ENET	PROFF_SCC2
#define	CPM_CR_ENET	CPM_CR_CH_SCC2
#define	SCC_ENET	1
#define CPMVEC_ENET	CPMVEC_SCC2
#endif

#ifdef CONFIG_SCC1_ENET
#define	PROFF_ENET	PROFF_SCC1
#define	CPM_CR_ENET	CPM_CR_CH_SCC1
#define	SCC_ENET	0
#define CPMVEC_ENET	CPMVEC_SCC1
#endif

#define PA_ENET_RXD	((ushort)0x0004)
#define PA_ENET_TXD	((ushort)0x0008)
#define PA_ENET_TCLK	((ushort)0x0400)
#define PA_ENET_RCLK	((ushort)0x0200)

#define PB_ENET_TENA	((uint)0x00002000)

#define PC_ENET_CLSN	((ushort)0x0040)
#define PC_ENET_RENA	((ushort)0x0080)

#define SICR_ENET_MASK	((uint)0x0000ff00)
#define SICR_ENET_CLKRT	((uint)0x00002e00)

#endif	/* CONFIG_FADS823FADS */

/***  FADS850SAR  ********************************************************/

#if defined(CONFIG_MPC850SAR) && defined(CONFIG_FADS)
/* This ENET stuff is for the MPC850SAR with ethernet on SCC2.  Some of
 * this may be unique to the FADS850SAR configuration.
 * Note TENA is on Port B.
 */
#define	PROFF_ENET	PROFF_SCC2
#define	CPM_CR_ENET	CPM_CR_CH_SCC2
#define	SCC_ENET	1
#define PA_ENET_RXD	((ushort)0x0004)	/* PA 13 */
#define PA_ENET_TXD	((ushort)0x0008)	/* PA 12 */
#define PA_ENET_RCLK	((ushort)0x0200)	/* PA 6 */
#define PA_ENET_TCLK	((ushort)0x0800)	/* PA 4 */
#define PB_ENET_TENA	((uint)0x00002000)	/* PB 18 */
#define PC_ENET_CLSN	((ushort)0x0040)	/* PC 9 */
#define PC_ENET_RENA	((ushort)0x0080)	/* PC 8 */

#define SICR_ENET_MASK	((uint)0x0000ff00)
#define SICR_ENET_CLKRT	((uint)0x00002f00)	/* RCLK-CLK2, TCLK-CLK4 */
#endif	/* CONFIG_FADS850SAR */

/***  FADS860T********************************************************/

#if defined(CONFIG_FADS) && defined(CONFIG_MPC86x)
/*
 * This ENET stuff is for the MPC86xFADS/MPC8xxADS with ethernet on SCC1.
 */
#ifdef CONFIG_SCC1_ENET

#define	SCC_ENET	0

#define	PROFF_ENET	PROFF_SCC1
#define	CPM_CR_ENET	CPM_CR_CH_SCC1

#define PA_ENET_RXD	((ushort)0x0001)
#define PA_ENET_TXD	((ushort)0x0002)
#define PA_ENET_TCLK	((ushort)0x0100)
#define PA_ENET_RCLK	((ushort)0x0200)

#define PB_ENET_TENA	((uint)0x00001000)

#define PC_ENET_CLSN	((ushort)0x0010)
#define PC_ENET_RENA	((ushort)0x0020)

#define SICR_ENET_MASK	((uint)0x000000ff)
#define SICR_ENET_CLKRT	((uint)0x0000002c)

#endif	/* CONFIG_SCC1_ETHERNET */

/*
 * This ENET stuff is for the MPC860TFADS/MPC86xADS/MPC885ADS
 * with ethernet on FEC.
 */

#ifdef CONFIG_FEC_ENET
#define	FEC_ENET	/* Use FEC for Ethernet */
#endif	/* CONFIG_FEC_ENET */

#endif	/* CONFIG_FADS && CONFIG_MPC86x */

/***  FPS850L, FPS860L  ************************************************/

#if defined(CONFIG_FPS850L) || defined(CONFIG_FPS860L)
/* Bits in parallel I/O port registers that have to be set/cleared
 * to configure the pins for SCC2 use.
 */
#define	PROFF_ENET	PROFF_SCC2
#define	CPM_CR_ENET	CPM_CR_CH_SCC2
#define	SCC_ENET	1
#define PA_ENET_RXD	((ushort)0x0004)	/* PA 13 */
#define PA_ENET_TXD	((ushort)0x0008)	/* PA 12 */
#define PA_ENET_RCLK	((ushort)0x0100)	/* PA  7 */
#define PA_ENET_TCLK	((ushort)0x0400)	/* PA  5 */

#define PC_ENET_TENA	((ushort)0x0002)	/* PC 14 */
#define PC_ENET_CLSN	((ushort)0x0040)	/* PC  9 */
#define PC_ENET_RENA	((ushort)0x0080)	/* PC  8 */

/* Control bits in the SICR to route TCLK (CLK2) and RCLK (CLK4) to
 * SCC2.  Also, make sure GR2 (bit 16) and SC2 (bit 17) are zero.
 */
#define SICR_ENET_MASK	((uint)0x0000ff00)
#define SICR_ENET_CLKRT	((uint)0x00002600)
#endif	/* CONFIG_FPS850L, CONFIG_FPS860L */

/*** GEN860T **********************************************************/
#if defined(CONFIG_GEN860T)
#undef	SCC_ENET
#define	FEC_ENET

#define PD_MII_TXD1	((ushort)0x1000)	/* PD  3	*/
#define PD_MII_TXD2	((ushort)0x0800)	/* PD  4	*/
#define PD_MII_TXD3	((ushort)0x0400)	/* PD  5	*/
#define PD_MII_RX_DV	((ushort)0x0200)	/* PD  6	*/
#define PD_MII_RX_ERR	((ushort)0x0100)	/* PD  7	*/
#define PD_MII_RX_CLK	((ushort)0x0080)	/* PD  8	*/
#define PD_MII_TXD0	((ushort)0x0040)	/* PD  9	*/
#define PD_MII_RXD0	((ushort)0x0020)	/* PD 10	*/
#define PD_MII_TX_ERR	((ushort)0x0010)	/* PD 11	*/
#define PD_MII_MDC	((ushort)0x0008)	/* PD 12	*/
#define PD_MII_RXD1	((ushort)0x0004)	/* PD 13	*/
#define PD_MII_RXD2	((ushort)0x0002)	/* PD 14	*/
#define PD_MII_RXD3	((ushort)0x0001)	/* PD 15	*/
#define PD_MII_MASK	((ushort)0x1FFF)	/* PD 3-15	*/
#endif	/* CONFIG_GEN860T */

/***  GENIETV  ********************************************************/

#if defined(CONFIG_GENIETV)
/* Ethernet is only on SCC2 */

#define CONFIG_SCC2_ENET
#define	PROFF_ENET	PROFF_SCC2
#define	CPM_CR_ENET	CPM_CR_CH_SCC2
#define	SCC_ENET	1
#define CPMVEC_ENET	CPMVEC_SCC2

#define PA_ENET_RXD	((ushort)0x0004)	/* PA 13 */
#define PA_ENET_TXD	((ushort)0x0008)	/* PA 12 */
#define PA_ENET_TCLK	((ushort)0x0400)	/* PA  5 */
#define PA_ENET_RCLK	((ushort)0x0200)	/* PA  6 */

#define PB_ENET_TENA	((uint)0x00002000)	/* PB 18 */

#define PC_ENET_CLSN	((ushort)0x0040)	/* PC  9 */
#define PC_ENET_RENA	((ushort)0x0080)	/* PC  8 */

#define SICR_ENET_MASK	((uint)0x0000ff00)
#define SICR_ENET_CLKRT	((uint)0x00002e00)

#endif	/* CONFIG_GENIETV */

/*** HERMES-PRO ******************************************************/

/* The HERMES-PRO uses the FEC on a MPC860T for Ethernet */

#ifdef CONFIG_HERMES

#define	FEC_ENET	/* use FEC for EThernet */
#undef	SCC_ENET


#define PD_MII_TXD1	((ushort)0x1000)	/* PD  3 */
#define PD_MII_TXD2	((ushort)0x0800)	/* PD  4 */
#define PD_MII_TXD3	((ushort)0x0400)	/* PD  5 */
#define PD_MII_RX_DV	((ushort)0x0200)	/* PD  6 */
#define PD_MII_RX_ERR	((ushort)0x0100)	/* PD  7 */
#define PD_MII_RX_CLK	((ushort)0x0080)	/* PD  8 */
#define PD_MII_TXD0	((ushort)0x0040)	/* PD  9 */
#define PD_MII_RXD0	((ushort)0x0020)	/* PD 10 */
#define PD_MII_TX_ERR	((ushort)0x0010)	/* PD 11 */
#define PD_MII_MDC	((ushort)0x0008)	/* PD 12 */
#define PD_MII_RXD1	((ushort)0x0004)	/* PD 13 */
#define PD_MII_RXD2	((ushort)0x0002)	/* PD 14 */
#define PD_MII_RXD3	((ushort)0x0001)	/* PD 15 */

#define PD_MII_MASK	((ushort)0x1FFF)	/* PD 3...15 */

#endif	/* CONFIG_HERMES */

/***  IAD210  **********************************************************/

/* The IAD210 uses the FEC on a MPC860P for Ethernet */

#if defined(CONFIG_IAD210)

# define  FEC_ENET    /* use FEC for Ethernet */
# undef   SCC_ENET

# define PD_MII_TXD1    ((ushort) 0x1000 )	/* PD  3 */
# define PD_MII_TXD2    ((ushort) 0x0800 )	/* PD  4 */
# define PD_MII_TXD3    ((ushort) 0x0400 )	/* PD  5 */
# define PD_MII_RX_DV   ((ushort) 0x0200 )	/* PD  6 */
# define PD_MII_RX_ERR  ((ushort) 0x0100 )	/* PD  7 */
# define PD_MII_RX_CLK  ((ushort) 0x0080 )	/* PD  8 */
# define PD_MII_TXD0    ((ushort) 0x0040 )	/* PD  9 */
# define PD_MII_RXD0    ((ushort) 0x0020 )	/* PD 10 */
# define PD_MII_TX_ERR  ((ushort) 0x0010 )	/* PD 11 */
# define PD_MII_MDC     ((ushort) 0x0008 )	/* PD 12 */
# define PD_MII_RXD1    ((ushort) 0x0004 )	/* PD 13 */
# define PD_MII_RXD2    ((ushort) 0x0002 )	/* PD 14 */
# define PD_MII_RXD3    ((ushort) 0x0001 )	/* PD 15 */

# define PD_MII_MASK    ((ushort) 0x1FFF )   /* PD 3...15 */

#endif	/* CONFIG_IAD210 */

/*** ICU862  **********************************************************/

#if defined(CONFIG_ICU862)

#ifdef CONFIG_FEC_ENET
#define FEC_ENET	/* use FEC for EThernet */
#endif  /* CONFIG_FEC_ETHERNET */

#endif /* CONFIG_ICU862 */

/***  IP860  **********************************************************/

#if defined(CONFIG_IP860)
/* Bits in parallel I/O port registers that have to be set/cleared
 * to configure the pins for SCC1 use.
 */
#define	PROFF_ENET	PROFF_SCC1
#define	CPM_CR_ENET	CPM_CR_CH_SCC1
#define	SCC_ENET	0
#define PA_ENET_RXD	((ushort)0x0001)	/* PA 15 */
#define PA_ENET_TXD	((ushort)0x0002)	/* PA 14 */
#define PA_ENET_RCLK	((ushort)0x0200)	/* PA  6 */
#define PA_ENET_TCLK	((ushort)0x0100)	/* PA  7 */

#define PC_ENET_TENA	((ushort)0x0001)	/* PC 15 */
#define PC_ENET_CLSN	((ushort)0x0010)	/* PC 11 */
#define PC_ENET_RENA	((ushort)0x0020)	/* PC 10 */

#define PB_ENET_RESET	(uint)0x00000008	/* PB 28 */
#define PB_ENET_JABD	(uint)0x00000004	/* PB 29 */

/* Control bits in the SICR to route TCLK (CLK1) and RCLK (CLK2) to
 * SCC1.  Also, make sure GR1 (bit 24) and SC1 (bit 25) are zero.
 */
#define SICR_ENET_MASK	((uint)0x000000ff)
#define SICR_ENET_CLKRT	((uint)0x0000002C)
#endif	/* CONFIG_IP860 */

/*** IVMS8  **********************************************************/

/* The IVMS8 uses the FEC on a MPC860T for Ethernet */

#if defined(CONFIG_IVMS8) || defined(CONFIG_IVML24)

#define	FEC_ENET	/* use FEC for EThernet */
#undef	SCC_ENET

#define	PB_ENET_POWER	((uint)0x00010000)	/* PB 15 */

#define PC_ENET_RESET	((ushort)0x0010)	/* PC 11 */

#define PD_MII_TXD1	((ushort)0x1000)	/* PD  3 */
#define PD_MII_TXD2	((ushort)0x0800)	/* PD  4 */
#define PD_MII_TXD3	((ushort)0x0400)	/* PD  5 */
#define PD_MII_RX_DV	((ushort)0x0200)	/* PD  6 */
#define PD_MII_RX_ERR	((ushort)0x0100)	/* PD  7 */
#define PD_MII_RX_CLK	((ushort)0x0080)	/* PD  8 */
#define PD_MII_TXD0	((ushort)0x0040)	/* PD  9 */
#define PD_MII_RXD0	((ushort)0x0020)	/* PD 10 */
#define PD_MII_TX_ERR	((ushort)0x0010)	/* PD 11 */
#define PD_MII_MDC	((ushort)0x0008)	/* PD 12 */
#define PD_MII_RXD1	((ushort)0x0004)	/* PD 13 */
#define PD_MII_RXD2	((ushort)0x0002)	/* PD 14 */
#define PD_MII_RXD3	((ushort)0x0001)	/* PD 15 */

#define PD_MII_MASK	((ushort)0x1FFF)	/* PD 3...15 */

#endif	/* CONFIG_IVMS8, CONFIG_IVML24 */

/***  KUP4K, KUP4X ****************************************************/
/* The KUP4 boards uses the FEC on a MPC8xx for Ethernet */

#if defined(CONFIG_KUP4K) || defined(CONFIG_KUP4X)

#define	FEC_ENET	/* use FEC for EThernet */
#undef	SCC_ENET

#define	PB_ENET_POWER	((uint)0x00010000)	/* PB 15 */

#define PC_ENET_RESET	((ushort)0x0010)	/* PC 11 */

#define PD_MII_TXD1	((ushort)0x1000)	/* PD  3 */
#define PD_MII_TXD2	((ushort)0x0800)	/* PD  4 */
#define PD_MII_TXD3	((ushort)0x0400)	/* PD  5 */
#define PD_MII_RX_DV	((ushort)0x0200)	/* PD  6 */
#define PD_MII_RX_ERR	((ushort)0x0100)	/* PD  7 */
#define PD_MII_RX_CLK	((ushort)0x0080)	/* PD  8 */
#define PD_MII_TXD0	((ushort)0x0040)	/* PD  9 */
#define PD_MII_RXD0	((ushort)0x0020)	/* PD 10 */
#define PD_MII_TX_ERR	((ushort)0x0010)	/* PD 11 */
#define PD_MII_MDC	((ushort)0x0008)	/* PD 12 */
#define PD_MII_RXD1	((ushort)0x0004)	/* PD 13 */
#define PD_MII_RXD2	((ushort)0x0002)	/* PD 14 */
#define PD_MII_RXD3	((ushort)0x0001)	/* PD 15 */

#define PD_MII_MASK	((ushort)0x1FFF)	/* PD 3...15 */

#endif	/* CONFIG_KUP4K */


/***  LANTEC  *********************************************************/

#if defined(CONFIG_LANTEC) && CONFIG_LANTEC >= 2
/* Bits in parallel I/O port registers that have to be set/cleared
 * to configure the pins for SCC2 use.
 */
#define	PROFF_ENET	PROFF_SCC2
#define	CPM_CR_ENET	CPM_CR_CH_SCC2
#define	SCC_ENET	1
#define PA_ENET_RXD	((ushort)0x0004)	/* PA 13 */
#define PA_ENET_TXD	((ushort)0x0008)	/* PA 12 */
#define PA_ENET_RCLK	((ushort)0x0200)	/* PA  6 */
#define PA_ENET_TCLK	((ushort)0x0400)	/* PA  5 */

#define PB_ENET_TENA	((uint)0x00002000)	/* PB 18 */

#define PC_ENET_LBK	((ushort)0x0010)	/* PC 11 */
#define PC_ENET_CLSN	((ushort)0x0040)	/* PC  9 */
#define PC_ENET_RENA	((ushort)0x0080)	/* PC  8 */

/* Control bits in the SICR to route TCLK (CLK3) and RCLK (CLK2) to
 * SCC2.  Also, make sure GR2 (bit 16) and SC2 (bit 17) are zero.
 */
#define SICR_ENET_MASK	((uint)0x0000FF00)
#define SICR_ENET_CLKRT	((uint)0x00002E00)
#endif	/* CONFIG_LANTEC v2 */

/***  LWMON  **********************************************************/

#if defined(CONFIG_LWMON)
/* Bits in parallel I/O port registers that have to be set/cleared
 * to configure the pins for SCC2 use.
 */
#define	PROFF_ENET	PROFF_SCC2
#define	CPM_CR_ENET	CPM_CR_CH_SCC2
#define	SCC_ENET	1
#define PA_ENET_RXD	((ushort)0x0004)	/* PA 13 */
#define PA_ENET_TXD	((ushort)0x0008)	/* PA 12 */
#define PA_ENET_RCLK	((ushort)0x0800)	/* PA  4 */
#define PA_ENET_TCLK	((ushort)0x0400)	/* PA  5 */

#define PB_ENET_TENA	((uint)0x00002000)	/* PB 18 */

#define PC_ENET_CLSN	((ushort)0x0040)	/* PC  9 */
#define PC_ENET_RENA	((ushort)0x0080)	/* PC  8 */

/* Control bits in the SICR to route TCLK (CLK3) and RCLK (CLK4) to
 * SCC2.  Also, make sure GR2 (bit 16) and SC2 (bit 17) are zero.
 */
#define SICR_ENET_MASK	((uint)0x0000ff00)
#define SICR_ENET_CLKRT	((uint)0x00003E00)
#endif	/* CONFIG_LWMON */

/***  NX823  ***********************************************/

#if defined(CONFIG_NX823)
/* Bits in parallel I/O port registers that have to be set/cleared
 * to configure the pins for SCC1 use.
 */
#define PROFF_ENET	PROFF_SCC2
#define CPM_CR_ENET	CPM_CR_CH_SCC2
#define SCC_ENET	1
#define PA_ENET_RXD	((ushort)0x0004)  /* PA 13 */
#define PA_ENET_TXD	((ushort)0x0008)  /* PA 12 */
#define PA_ENET_RCLK	((ushort)0x0200)  /* PA  6 */
#define PA_ENET_TCLK	((ushort)0x0800)  /* PA  4 */

#define PB_ENET_TENA	((uint)0x00002000)   /* PB 18 */

#define PC_ENET_CLSN	((ushort)0x0040)  /* PC  9 */
#define PC_ENET_RENA	((ushort)0x0080)  /* PC  8 */

/* Control bits in the SICR to route TCLK (CLK3) and RCLK (CLK1) to
 * SCC2.  Also, make sure GR2 (bit 16) and SC2 (bit 17) are zero.
 */
#define SICR_ENET_MASK	((uint)0x0000ff00)
#define SICR_ENET_CLKRT	((uint)0x00002f00)

#endif   /* CONFIG_NX823 */

/***  MBX  ************************************************************/

#ifdef CONFIG_MBX
/* Bits in parallel I/O port registers that have to be set/cleared
 * to configure the pins for SCC1 use.  The TCLK and RCLK seem unique
 * to the MBX860 board.  Any two of the four available clocks could be
 * used, and the MPC860 cookbook manual has an example using different
 * clock pins.
 */
#define	PROFF_ENET	PROFF_SCC1
#define	CPM_CR_ENET	CPM_CR_CH_SCC1
#define	SCC_ENET	0
#define PA_ENET_RXD	((ushort)0x0001)
#define PA_ENET_TXD	((ushort)0x0002)
#define PA_ENET_TCLK	((ushort)0x0200)
#define PA_ENET_RCLK	((ushort)0x0800)
#define PC_ENET_TENA	((ushort)0x0001)
#define PC_ENET_CLSN	((ushort)0x0010)
#define PC_ENET_RENA	((ushort)0x0020)

/* Control bits in the SICR to route TCLK (CLK2) and RCLK (CLK4) to
 * SCC1.  Also, make sure GR1 (bit 24) and SC1 (bit 25) are zero.
 */
#define SICR_ENET_MASK	((uint)0x000000ff)
#define SICR_ENET_CLKRT	((uint)0x0000003d)
#endif	/* CONFIG_MBX */

/***  KM8XX  *********************************************************/

/* The KM8XX Service Module uses SCC3 for Ethernet */

#ifdef CONFIG_KM8XX
#define PROFF_ENET	PROFF_SCC3		/* Ethernet on SCC3 */
#define CPM_CR_ENET	CPM_CR_CH_SCC3
#define SCC_ENET	2
#define PA_ENET_RXD	((ushort)0x0010)	/* PA 11 */
#define PA_ENET_TXD	((ushort)0x0020)	/* PA 10 */
#define PA_ENET_RCLK	((ushort)0x1000)	/* PA  3 CLK 5 */
#define PA_ENET_TCLK	((ushort)0x2000)	/* PA  2 CLK 6 */

#define PC_ENET_TENA	((ushort)0x0004)	/* PC 13 */

#define PC_ENET_RENA	((ushort)0x0200)	/* PC  6 */
#define PC_ENET_CLSN	((ushort)0x0100)	/* PC  7 */

/* Control bits in the SICR to route TCLK (CLK6) and RCLK (CLK5) to
 * SCC3.  Also, make sure GR3 (bit 8) and SC3 (bit 9) are zero.
 */
#define SICR_ENET_MASK	((uint)0x00FF0000)
#define SICR_ENET_CLKRT	((uint)0x00250000)
#endif	/* CONFIG_KM8XX */


/***  MHPC  ********************************************************/

#if defined(CONFIG_MHPC)
/* This ENET stuff is for the MHPC with ethernet on SCC2.
 * Note TENA is on Port B.
 */
#define	PROFF_ENET	PROFF_SCC2
#define	CPM_CR_ENET	CPM_CR_CH_SCC2
#define	SCC_ENET	1
#define PA_ENET_RXD	((ushort)0x0004)	/* PA 13 */
#define PA_ENET_TXD	((ushort)0x0008)	/* PA 12 */
#define PA_ENET_RCLK	((ushort)0x0200)	/* PA 6 */
#define PA_ENET_TCLK	((ushort)0x0400)	/* PA 5 */
#define PB_ENET_TENA	((uint)0x00002000)	/* PB 18 */
#define PC_ENET_CLSN	((ushort)0x0040)	/* PC 9 */
#define PC_ENET_RENA	((ushort)0x0080)	/* PC 8 */

#define SICR_ENET_MASK	((uint)0x0000ff00)
#define SICR_ENET_CLKRT	((uint)0x00002e00)	/* RCLK-CLK2, TCLK-CLK3 */
#endif	/* CONFIG_MHPC */

/***  NETVIA  *******************************************************/

/* SinoVee Microsystems SC8xx series FEL8xx-AT,SC823,SC850,SC855T,SC860T */
#if ( defined CONFIG_SVM_SC8xx )
# ifndef CONFIG_FEC_ENET

#define PROFF_ENET      PROFF_SCC2
#define CPM_CR_ENET     CPM_CR_CH_SCC2
#define SCC_ENET        1

	/* Bits in parallel I/O port registers that have to be set/cleared
	 *  *  *  * to configure the pins for SCC2 use.
	 *   *   *   */
#define PA_ENET_RXD     ((ushort)0x0004)        /* PA 13 */
#define PA_ENET_TXD     ((ushort)0x0008)        /* PA 12 */
#define PA_ENET_RCLK    ((ushort)0x0400)        /* PA  5 */
#define PA_ENET_TCLK    ((ushort)0x0800)        /* PA  4 */

#define PB_ENET_TENA    ((uint)0x00002000)      /* PB 18 */

#define PC_ENET_CLSN    ((ushort)0x0040)        /* PC  9 */
#define PC_ENET_RENA    ((ushort)0x0080)        /* PC  8 */
/* Control bits in the SICR to route TCLK (CLK3) and RCLK (CLK1) to
 *  *  *  * SCC2.  Also, make sure GR2 (bit 16) and SC2 (bit 17) are zero.
 *   *   *   */
#define SICR_ENET_MASK  ((uint)0x0000ff00)
#define SICR_ENET_CLKRT ((uint)0x00003700)

# else                          /* Use FEC for Fast Ethernet */

#undef  SCC_ENET
#define FEC_ENET

#define PD_MII_TXD1     ((ushort)0x1000)        /* PD  3 */
#define PD_MII_TXD2     ((ushort)0x0800)        /* PD  4 */
#define PD_MII_TXD3     ((ushort)0x0400)        /* PD  5 */
#define PD_MII_RX_DV    ((ushort)0x0200)        /* PD  6 */
#define PD_MII_RX_ERR   ((ushort)0x0100)        /* PD  7 */
#define PD_MII_RX_CLK   ((ushort)0x0080)        /* PD  8 */
#define PD_MII_TXD0     ((ushort)0x0040)        /* PD  9 */
#define PD_MII_RXD0     ((ushort)0x0020)        /* PD 10 */
#define PD_MII_TX_ERR   ((ushort)0x0010)        /* PD 11 */
#define PD_MII_MDC      ((ushort)0x0008)        /* PD 12 */
#define PD_MII_RXD1     ((ushort)0x0004)        /* PD 13 */
#define PD_MII_RXD2     ((ushort)0x0002)        /* PD 14 */
#define PD_MII_RXD3     ((ushort)0x0001)        /* PD 15 */

#define PD_MII_MASK     ((ushort)0x1FFF)        /* PD 3...15 */

# endif /* CONFIG_FEC_ENET */
#endif  /* CONFIG_SVM_SC8xx */


#if defined(CONFIG_NETVIA)
/* Bits in parallel I/O port registers that have to be set/cleared
 * to configure the pins for SCC2 use.
 */
#define	PROFF_ENET	PROFF_SCC2
#define	CPM_CR_ENET	CPM_CR_CH_SCC2
#define	SCC_ENET	1
#define PA_ENET_RXD	((ushort)0x0004)	/* PA 13 */
#define PA_ENET_TXD	((ushort)0x0008)	/* PA 12 */
#define PA_ENET_RCLK	((ushort)0x0200)	/* PA  6 */
#define PA_ENET_TCLK	((ushort)0x0800)	/* PA  4 */

#if !defined(CONFIG_NETVIA_VERSION) || CONFIG_NETVIA_VERSION == 1
# define PB_ENET_PDN	((ushort)0x4000)	/* PB 17 */
#elif CONFIG_NETVIA_VERSION >= 2
# define PC_ENET_PDN	((ushort)0x0008)	/* PC 12 */
#endif

#define PB_ENET_TENA	((ushort)0x2000)	/* PB 18 */

#define PC_ENET_CLSN	((ushort)0x0040)	/* PC  9 */
#define PC_ENET_RENA	((ushort)0x0080)	/* PC  8 */

/* Control bits in the SICR to route TCLK (CLK3) and RCLK (CLK1) to
 * SCC2.  Also, make sure GR2 (bit 16) and SC2 (bit 17) are zero.
 */
#define SICR_ENET_MASK	((uint)0x0000ff00)
#define SICR_ENET_CLKRT	((uint)0x00002f00)

#endif	/* CONFIG_NETVIA */

/***  QS850/QS823  ***************************************************/

#if defined(CONFIG_QS850) || defined(CONFIG_QS823)
#undef FEC_ENET /* Don't use FEC for EThernet */

#define PROFF_ENET		PROFF_SCC2
#define CPM_CR_ENET		CPM_CR_CH_SCC2
#define SCC_ENET		1

#define PA_ENET_RXD		((ushort)0x0004)  /* RXD on PA13 (Pin D9) */
#define PA_ENET_TXD		((ushort)0x0008)  /* TXD on PA12 (Pin D7) */
#define PC_ENET_RENA		((ushort)0x0080)  /* RENA on PC8 (Pin D12) */
#define PC_ENET_CLSN		((ushort)0x0040)  /* CLSN on PC9 (Pin C12) */
#define PA_ENET_TCLK		((ushort)0x0200)  /* TCLK on PA6 (Pin D8) */
#define PA_ENET_RCLK		((ushort)0x0800)  /* RCLK on PA4 (Pin D10) */
#define PB_ENET_TENA		((uint)0x00002000)  /* TENA on PB18 (Pin D11) */
#define PC_ENET_LBK		((ushort)0x0010)  /* Loopback control on PC11 (Pin B14) */
#define PC_ENET_LI		((ushort)0x0020)  /* Link Integrity control PC10 (A15) */
#define PC_ENET_SQE		((ushort)0x0100)  /* SQE Disable control PC7 (B15) */

/* SCC2 TXCLK from CLK2
 * SCC2 RXCLK from CLK4
 * SCC2 Connected to NMSI */
#define SICR_ENET_MASK		((uint)0x00007F00)
#define SICR_ENET_CLKRT		((uint)0x00003D00)

#endif /* CONFIG_QS850/QS823 */

/***  QS860T  ***************************************************/

#ifdef CONFIG_QS860T
#ifdef CONFIG_FEC_ENET
#define FEC_ENET /* use FEC for EThernet */
#endif /* CONFIG_FEC_ETHERNET */

/* This ENET stuff is for GTH 10 Mbit ( SCC ) */
#define PROFF_ENET		PROFF_SCC1
#define CPM_CR_ENET		CPM_CR_CH_SCC1
#define SCC_ENET		0

#define PA_ENET_RXD		((ushort)0x0001) /* PA15 */
#define PA_ENET_TXD		((ushort)0x0002) /* PA14 */
#define PA_ENET_TCLK		((ushort)0x0800) /* PA4 */
#define PA_ENET_RCLK		((ushort)0x0200) /* PA6 */
#define PB_ENET_TENA		((uint)0x00001000) /* PB19 */
#define PC_ENET_CLSN		((ushort)0x0010) /* PC11 */
#define PC_ENET_RENA		((ushort)0x0020) /* PC10 */

#define SICR_ENET_MASK		((uint)0x000000ff)
/* RCLK PA4 -->CLK4, TCLK PA6 -->CLK2 */
#define SICR_ENET_CLKRT		((uint)0x0000003D)

#endif /* CONFIG_QS860T */

/***  RPXCLASSIC  *****************************************************/

#ifdef CONFIG_RPXCLASSIC

#ifdef CONFIG_FEC_ENET

# define FEC_ENET				/* use FEC for EThernet */
# undef SCC_ENET

#else	/* ! CONFIG_FEC_ENET */

/* Bits in parallel I/O port registers that have to be set/cleared
 * to configure the pins for SCC1 use.
 */
#define	PROFF_ENET	PROFF_SCC1
#define	CPM_CR_ENET	CPM_CR_CH_SCC1
#define	SCC_ENET	0
#define PA_ENET_RXD	((ushort)0x0001)
#define PA_ENET_TXD	((ushort)0x0002)
#define PA_ENET_TCLK	((ushort)0x0200)
#define PA_ENET_RCLK	((ushort)0x0800)
#define PB_ENET_TENA	((uint)0x00001000)
#define PC_ENET_CLSN	((ushort)0x0010)
#define PC_ENET_RENA	((ushort)0x0020)

/* Control bits in the SICR to route TCLK (CLK2) and RCLK (CLK4) to
 * SCC1.  Also, make sure GR1 (bit 24) and SC1 (bit 25) are zero.
 */
#define SICR_ENET_MASK	((uint)0x000000ff)
#define SICR_ENET_CLKRT	((uint)0x0000003d)

#endif	/* CONFIG_FEC_ENET */

#endif	/* CONFIG_RPXCLASSIC */

/***  RPXLITE  ********************************************************/

#ifdef CONFIG_RPXLITE
/* This ENET stuff is for the MPC850 with ethernet on SCC2.  Some of
 * this may be unique to the RPX-Lite configuration.
 * Note TENA is on Port B.
 */
#define	PROFF_ENET	PROFF_SCC2
#define	CPM_CR_ENET	CPM_CR_CH_SCC2
#define	SCC_ENET	1
#define PA_ENET_RXD	((ushort)0x0004)
#define PA_ENET_TXD	((ushort)0x0008)
#define PA_ENET_TCLK	((ushort)0x0200)
#define PA_ENET_RCLK	((ushort)0x0800)
#if defined(CONFIG_RMU)
#define PC_ENET_TENA	((uint)0x00000002)	/* PC14 */
#else
#define PB_ENET_TENA	((uint)0x00002000)
#endif
#define PC_ENET_CLSN	((ushort)0x0040)
#define PC_ENET_RENA	((ushort)0x0080)

#define SICR_ENET_MASK	((uint)0x0000ff00)
#define SICR_ENET_CLKRT	((uint)0x00003d00)
#endif	/* CONFIG_RPXLITE */

/***  SM850  *********************************************************/

/* The SM850 Service Module uses SCC2 for IrDA and SCC3 for Ethernet */

#ifdef CONFIG_SM850
#define PROFF_ENET	PROFF_SCC3		/* Ethernet on SCC3 */
#define CPM_CR_ENET	CPM_CR_CH_SCC3
#define SCC_ENET	2
#define PB_ENET_RXD	((uint)0x00000004)	/* PB 29 */
#define PB_ENET_TXD	((uint)0x00000002)	/* PB 30 */
#define PA_ENET_RCLK	((ushort)0x0100)	/* PA  7 */
#define PA_ENET_TCLK	((ushort)0x0400)	/* PA  5 */

#define PC_ENET_LBK	((ushort)0x0008)	/* PC 12 */
#define PC_ENET_TENA	((ushort)0x0004)	/* PC 13 */

#define PC_ENET_RENA	((ushort)0x0800)	/* PC  4 */
#define PC_ENET_CLSN	((ushort)0x0400)	/* PC  5 */

/* Control bits in the SICR to route TCLK (CLK3) and RCLK (CLK1) to
 * SCC3.  Also, make sure GR3 (bit 8) and SC3 (bit 9) are zero.
 */
#define SICR_ENET_MASK	((uint)0x00FF0000)
#define SICR_ENET_CLKRT	((uint)0x00260000)
#endif	/* CONFIG_SM850 */

/***  SPD823TS  ******************************************************/

#ifdef CONFIG_SPD823TS
/* Bits in parallel I/O port registers that have to be set/cleared
 * to configure the pins for SCC2 use.
 */
#define	PROFF_ENET	PROFF_SCC2		/* Ethernet on SCC2 */
#define CPM_CR_ENET     CPM_CR_CH_SCC2
#define	SCC_ENET	1
#define PA_ENET_MDC	((ushort)0x0001)	/* PA 15 !!! */
#define PA_ENET_MDIO	((ushort)0x0002)	/* PA 14 !!! */
#define PA_ENET_RXD	((ushort)0x0004)	/* PA 13 */
#define PA_ENET_TXD	((ushort)0x0008)	/* PA 12 */
#define PA_ENET_RCLK	((ushort)0x0200)	/* PA  6 */
#define PA_ENET_TCLK	((ushort)0x0400)	/* PA  5 */

#define PB_ENET_TENA	((uint)0x00002000)	/* PB 18 */

#define PC_ENET_CLSN	((ushort)0x0040)	/* PC  9 */
#define PC_ENET_RENA	((ushort)0x0080)	/* PC  8 */
#define	PC_ENET_RESET	((ushort)0x0100)	/* PC  7 !!! */

/* Control bits in the SICR to route TCLK (CLK3) and RCLK (CLK2) to
 * SCC2.  Also, make sure GR2 (bit 16) and SC2 (bit 17) are zero.
 */
#define SICR_ENET_MASK	((uint)0x0000ff00)
#define SICR_ENET_CLKRT	((uint)0x00002E00)
#endif	/* CONFIG_SPD823TS */

/***  SXNI855T  ******************************************************/

#if defined(CONFIG_SXNI855T)

#ifdef CONFIG_FEC_ENET
#define	FEC_ENET	/* use FEC for Ethernet */
#endif	/* CONFIG_FEC_ETHERNET */

#endif	/* CONFIG_SXNI855T */

/***  MVS1, TQM823L/M, TQM850L/M, TQM885D, ETX094, R360MPI  **********/

#if (defined(CONFIG_MVS) && CONFIG_MVS < 2) || \
    defined(CONFIG_R360MPI) || defined(CONFIG_RBC823)  || \
    defined(CONFIG_TQM823L) || defined(CONFIG_TQM823M) || \
    defined(CONFIG_TQM850L) || defined(CONFIG_TQM850M) || \
    defined(CONFIG_TQM885D) || defined(CONFIG_ETX094)  || \
    defined(CONFIG_RRVISION)|| defined(CONFIG_VIRTLAB2)|| \
   (defined(CONFIG_LANTEC) && CONFIG_LANTEC < 2)

/* Bits in parallel I/O port registers that have to be set/cleared
 * to configure the pins for SCC2 use.
 */
#define	PROFF_ENET	PROFF_SCC2
#define	CPM_CR_ENET	CPM_CR_CH_SCC2
#if (!defined(CONFIG_TK885D))	/* TK885D does not use SCC Ethernet */
#define	SCC_ENET	1
#endif
#define PA_ENET_RXD	((ushort)0x0004)	/* PA 13 */
#define PA_ENET_TXD	((ushort)0x0008)	/* PA 12 */
#define PA_ENET_RCLK	((ushort)0x0100)	/* PA  7 */
#define PA_ENET_TCLK	((ushort)0x0400)	/* PA  5 */

#define PB_ENET_TENA	((uint)0x00002000)	/* PB 18 */

#define PC_ENET_CLSN	((ushort)0x0040)	/* PC  9 */
#define PC_ENET_RENA	((ushort)0x0080)	/* PC  8 */
#if defined(CONFIG_R360MPI)
#define PC_ENET_LBK	((ushort)0x0008)	/* PC 12 */
#endif   /* CONFIG_R360MPI */

/* Control bits in the SICR to route TCLK (CLK3) and RCLK (CLK1) to
 * SCC2.  Also, make sure GR2 (bit 16) and SC2 (bit 17) are zero.
 */
#define SICR_ENET_MASK	((uint)0x0000ff00)
#define SICR_ENET_CLKRT	((uint)0x00002600)

# ifdef CONFIG_FEC_ENET		/* Use FEC for Fast Ethernet */
#define FEC_ENET
# endif	/* CONFIG_FEC_ENET */

#endif	/* CONFIG_MVS v1, CONFIG_TQM823L/M, CONFIG_TQM850L/M, etc. */

/***  TQM855L/M, TQM860L/M, TQM862L/M, TQM866L/M  *********************/

#if defined(CONFIG_TQM855L) || defined(CONFIG_TQM855M) || \
    defined(CONFIG_TQM860L) || defined(CONFIG_TQM860M) || \
    defined(CONFIG_TQM862L) || defined(CONFIG_TQM862M) || \
    defined(CONFIG_TQM866L) || defined(CONFIG_TQM866M)

# ifdef CONFIG_SCC1_ENET	/* use SCC for 10Mbps Ethernet	*/

/* Bits in parallel I/O port registers that have to be set/cleared
 * to configure the pins for SCC1 use.
 */
#define	PROFF_ENET	PROFF_SCC1
#define	CPM_CR_ENET	CPM_CR_CH_SCC1
#define	SCC_ENET	0
#define PA_ENET_RXD	((ushort)0x0001)	/* PA 15 */
#define PA_ENET_TXD	((ushort)0x0002)	/* PA 14 */
#define PA_ENET_RCLK	((ushort)0x0100)	/* PA  7 */
#define PA_ENET_TCLK	((ushort)0x0400)	/* PA  5 */

#define PC_ENET_TENA	((ushort)0x0001)	/* PC 15 */
#define PC_ENET_CLSN	((ushort)0x0010)	/* PC 11 */
#define PC_ENET_RENA	((ushort)0x0020)	/* PC 10 */

/* Control bits in the SICR to route TCLK (CLK3) and RCLK (CLK1) to
 * SCC1.  Also, make sure GR1 (bit 24) and SC1 (bit 25) are zero.
 */
#define SICR_ENET_MASK	((uint)0x000000ff)
#define SICR_ENET_CLKRT	((uint)0x00000026)

# endif	/* CONFIG_SCC1_ENET */

# ifdef CONFIG_FEC_ENET		/* Use FEC for Fast Ethernet */

#define FEC_ENET

#define PD_MII_TXD1	((ushort)0x1000)	/* PD  3 */
#define PD_MII_TXD2	((ushort)0x0800)	/* PD  4 */
#define PD_MII_TXD3	((ushort)0x0400)	/* PD  5 */
#define PD_MII_RX_DV	((ushort)0x0200)	/* PD  6 */
#define PD_MII_RX_ERR	((ushort)0x0100)	/* PD  7 */
#define PD_MII_RX_CLK	((ushort)0x0080)	/* PD  8 */
#define PD_MII_TXD0	((ushort)0x0040)	/* PD  9 */
#define PD_MII_RXD0	((ushort)0x0020)	/* PD 10 */
#define PD_MII_TX_ERR	((ushort)0x0010)	/* PD 11 */
#define PD_MII_MDC	((ushort)0x0008)	/* PD 12 */
#define PD_MII_RXD1	((ushort)0x0004)	/* PD 13 */
#define PD_MII_RXD2	((ushort)0x0002)	/* PD 14 */
#define PD_MII_RXD3	((ushort)0x0001)	/* PD 15 */

#define PD_MII_MASK	((ushort)0x1FFF)	/* PD 3...15 */

# endif	/* CONFIG_FEC_ENET */
#endif	/* CONFIG_TQM855L/M, TQM860L/M, TQM862L/M */

/***  V37  **********************************************************/

#ifdef CONFIG_V37
/* This ENET stuff is for the MPC823 with ethernet on SCC2.  Some of
 * this may be unique to the Marel V37 configuration.
 * Note TENA is on Port B.
 */
#define	PROFF_ENET	PROFF_SCC2
#define	CPM_CR_ENET	CPM_CR_CH_SCC2
#define	SCC_ENET	1
#define PA_ENET_RXD	((ushort)0x0004)
#define PA_ENET_TXD	((ushort)0x0008)
#define PA_ENET_TCLK	((ushort)0x0400)
#define PA_ENET_RCLK	((ushort)0x0200)
#define PB_ENET_TENA	((uint)0x00002000)
#define PC_ENET_CLSN	((ushort)0x0040)
#define PC_ENET_RENA	((ushort)0x0080)

#define SICR_ENET_MASK	((uint)0x0000ff00)
#define SICR_ENET_CLKRT	((uint)0x00002e00)
#endif	/* CONFIG_V37 */


/*********************************************************************/

/* SCC Event register as used by Ethernet.
*/
#define SCCE_ENET_GRA	((ushort)0x0080)	/* Graceful stop complete */
#define SCCE_ENET_TXE	((ushort)0x0010)	/* Transmit Error */
#define SCCE_ENET_RXF	((ushort)0x0008)	/* Full frame received */
#define SCCE_ENET_BSY	((ushort)0x0004)	/* All incoming buffers full */
#define SCCE_ENET_TXB	((ushort)0x0002)	/* A buffer was transmitted */
#define SCCE_ENET_RXB	((ushort)0x0001)	/* A buffer was received */

/* SCC Mode Register (PSMR) as used by Ethernet.
*/
#define SCC_PSMR_HBC	((ushort)0x8000)	/* Enable heartbeat */
#define SCC_PSMR_FC	((ushort)0x4000)	/* Force collision */
#define SCC_PSMR_RSH	((ushort)0x2000)	/* Receive short frames */
#define SCC_PSMR_IAM	((ushort)0x1000)	/* Check individual hash */
#define SCC_PSMR_ENCRC	((ushort)0x0800)	/* Ethernet CRC mode */
#define SCC_PSMR_PRO	((ushort)0x0200)	/* Promiscuous mode */
#define SCC_PSMR_BRO	((ushort)0x0100)	/* Catch broadcast pkts */
#define SCC_PSMR_SBT	((ushort)0x0080)	/* Special backoff timer */
#define SCC_PSMR_LPB	((ushort)0x0040)	/* Set Loopback mode */
#define SCC_PSMR_SIP	((ushort)0x0020)	/* Sample Input Pins */
#define SCC_PSMR_LCW	((ushort)0x0010)	/* Late collision window */
#define SCC_PSMR_NIB22	((ushort)0x000a)	/* Start frame search */
#define SCC_PSMR_FDE	((ushort)0x0001)	/* Full duplex enable */

/* Buffer descriptor control/status used by Ethernet receive.
*/
#define BD_ENET_RX_EMPTY	((ushort)0x8000)
#define BD_ENET_RX_WRAP		((ushort)0x2000)
#define BD_ENET_RX_INTR		((ushort)0x1000)
#define BD_ENET_RX_LAST		((ushort)0x0800)
#define BD_ENET_RX_FIRST	((ushort)0x0400)
#define BD_ENET_RX_MISS		((ushort)0x0100)
#define BD_ENET_RX_LG		((ushort)0x0020)
#define BD_ENET_RX_NO		((ushort)0x0010)
#define BD_ENET_RX_SH		((ushort)0x0008)
#define BD_ENET_RX_CR		((ushort)0x0004)
#define BD_ENET_RX_OV		((ushort)0x0002)
#define BD_ENET_RX_CL		((ushort)0x0001)
#define BD_ENET_RX_STATS	((ushort)0x013f)	/* All status bits */

/* Buffer descriptor control/status used by Ethernet transmit.
*/
#define BD_ENET_TX_READY	((ushort)0x8000)
#define BD_ENET_TX_PAD		((ushort)0x4000)
#define BD_ENET_TX_WRAP		((ushort)0x2000)
#define BD_ENET_TX_INTR		((ushort)0x1000)
#define BD_ENET_TX_LAST		((ushort)0x0800)
#define BD_ENET_TX_TC		((ushort)0x0400)
#define BD_ENET_TX_DEF		((ushort)0x0200)
#define BD_ENET_TX_HB		((ushort)0x0100)
#define BD_ENET_TX_LC		((ushort)0x0080)
#define BD_ENET_TX_RL		((ushort)0x0040)
#define BD_ENET_TX_RCMASK	((ushort)0x003c)
#define BD_ENET_TX_UN		((ushort)0x0002)
#define BD_ENET_TX_CSL		((ushort)0x0001)
#define BD_ENET_TX_STATS	((ushort)0x03ff)	/* All status bits */

/* SCC as UART
*/
typedef struct scc_uart {
	sccp_t	scc_genscc;
	uint	scc_res1;	/* Reserved */
	uint	scc_res2;	/* Reserved */
	ushort	scc_maxidl;	/* Maximum idle chars */
	ushort	scc_idlc;	/* temp idle counter */
	ushort	scc_brkcr;	/* Break count register */
	ushort	scc_parec;	/* receive parity error counter */
	ushort	scc_frmec;	/* receive framing error counter */
	ushort	scc_nosec;	/* receive noise counter */
	ushort	scc_brkec;	/* receive break condition counter */
	ushort	scc_brkln;	/* last received break length */
	ushort	scc_uaddr1;	/* UART address character 1 */
	ushort	scc_uaddr2;	/* UART address character 2 */
	ushort	scc_rtemp;	/* Temp storage */
	ushort	scc_toseq;	/* Transmit out of sequence char */
	ushort	scc_char1;	/* control character 1 */
	ushort	scc_char2;	/* control character 2 */
	ushort	scc_char3;	/* control character 3 */
	ushort	scc_char4;	/* control character 4 */
	ushort	scc_char5;	/* control character 5 */
	ushort	scc_char6;	/* control character 6 */
	ushort	scc_char7;	/* control character 7 */
	ushort	scc_char8;	/* control character 8 */
	ushort	scc_rccm;	/* receive control character mask */
	ushort	scc_rccr;	/* receive control character register */
	ushort	scc_rlbc;	/* receive last break character */
} scc_uart_t;

/* SCC Event and Mask registers when it is used as a UART.
*/
#define UART_SCCM_GLR		((ushort)0x1000)
#define UART_SCCM_GLT		((ushort)0x0800)
#define UART_SCCM_AB		((ushort)0x0200)
#define UART_SCCM_IDL		((ushort)0x0100)
#define UART_SCCM_GRA		((ushort)0x0080)
#define UART_SCCM_BRKE		((ushort)0x0040)
#define UART_SCCM_BRKS		((ushort)0x0020)
#define UART_SCCM_CCR		((ushort)0x0008)
#define UART_SCCM_BSY		((ushort)0x0004)
#define UART_SCCM_TX		((ushort)0x0002)
#define UART_SCCM_RX		((ushort)0x0001)

/* The SCC PSMR when used as a UART.
*/
#define SCU_PSMR_FLC		((ushort)0x8000)
#define SCU_PSMR_SL		((ushort)0x4000)
#define SCU_PSMR_CL		((ushort)0x3000)
#define SCU_PSMR_UM		((ushort)0x0c00)
#define SCU_PSMR_FRZ		((ushort)0x0200)
#define SCU_PSMR_RZS		((ushort)0x0100)
#define SCU_PSMR_SYN		((ushort)0x0080)
#define SCU_PSMR_DRT		((ushort)0x0040)
#define SCU_PSMR_PEN		((ushort)0x0010)
#define SCU_PSMR_RPM		((ushort)0x000c)
#define SCU_PSMR_REVP		((ushort)0x0008)
#define SCU_PSMR_TPM		((ushort)0x0003)
#define SCU_PSMR_TEVP		((ushort)0x0003)

/* CPM Transparent mode SCC.
 */
typedef struct scc_trans {
	sccp_t	st_genscc;
	uint	st_cpres;	/* Preset CRC */
	uint	st_cmask;	/* Constant mask for CRC */
} scc_trans_t;

#define BD_SCC_TX_LAST		((ushort)0x0800)

/* IIC parameter RAM.
*/
typedef struct iic {
	ushort	iic_rbase;	/* Rx Buffer descriptor base address */
	ushort	iic_tbase;	/* Tx Buffer descriptor base address */
	u_char	iic_rfcr;	/* Rx function code */
	u_char	iic_tfcr;	/* Tx function code */
	ushort	iic_mrblr;	/* Max receive buffer length */
	uint	iic_rstate;	/* Internal */
	uint	iic_rdp;	/* Internal */
	ushort	iic_rbptr;	/* Internal */
	ushort	iic_rbc;	/* Internal */
	uint	iic_rxtmp;	/* Internal */
	uint	iic_tstate;	/* Internal */
	uint	iic_tdp;	/* Internal */
	ushort	iic_tbptr;	/* Internal */
	ushort	iic_tbc;	/* Internal */
	uint	iic_txtmp;	/* Internal */
	uint	iic_res;	/* reserved */
	ushort	iic_rpbase;	/* Relocation pointer */
	ushort	iic_res2;	/* reserved */
} iic_t;

/* SPI parameter RAM.
*/
typedef struct spi {
	ushort	spi_rbase;	/* Rx Buffer descriptor base address */
	ushort	spi_tbase;	/* Tx Buffer descriptor base address */
	u_char	spi_rfcr;	/* Rx function code */
	u_char	spi_tfcr;	/* Tx function code */
	ushort	spi_mrblr;	/* Max receive buffer length */
	uint	spi_rstate;	/* Internal */
	uint	spi_rdp;	/* Internal */
	ushort	spi_rbptr;	/* Internal */
	ushort	spi_rbc;	/* Internal */
	uint	spi_rxtmp;	/* Internal */
	uint	spi_tstate;	/* Internal */
	uint	spi_tdp;	/* Internal */
	ushort	spi_tbptr;	/* Internal */
	ushort	spi_tbc;	/* Internal */
	uint	spi_txtmp;	/* Internal */
	uint	spi_res;
	ushort	spi_rpbase;	/* Relocation pointer */
	ushort	spi_res2;
} spi_t;

/* SPI Mode register.
*/
#define SPMODE_LOOP	((ushort)0x4000)	/* Loopback */
#define SPMODE_CI	((ushort)0x2000)	/* Clock Invert */
#define SPMODE_CP	((ushort)0x1000)	/* Clock Phase */
#define SPMODE_DIV16	((ushort)0x0800)	/* BRG/16 mode */
#define SPMODE_REV	((ushort)0x0400)	/* Reversed Data */
#define SPMODE_MSTR	((ushort)0x0200)	/* SPI Master */
#define SPMODE_EN	((ushort)0x0100)	/* Enable */
#define SPMODE_LENMSK	((ushort)0x00f0)	/* character length */
#define SPMODE_PMMSK	((ushort)0x000f)	/* prescale modulus */

#define SPMODE_LEN(x)	((((x)-1)&0xF)<<4)
#define SPMODE_PM(x)	((x) &0xF)

/* HDLC parameter RAM.
*/

typedef struct hdlc_pram_s {
	/*
	 * SCC parameter RAM
	 */
	ushort	rbase;		/* Rx Buffer descriptor base address */
	ushort	tbase;		/* Tx Buffer descriptor base address */
	uchar	rfcr;		/* Rx function code */
	uchar	tfcr;		/* Tx function code */
	ushort	mrblr;		/* Rx buffer length */
	ulong	rstate;		/* Rx internal state */
	ulong	rptr;		/* Rx internal data pointer */
	ushort	rbptr;		/* rb BD Pointer */
	ushort	rcount;		/* Rx internal byte count */
	ulong	rtemp;		/* Rx temp */
	ulong	tstate;		/* Tx internal state */
	ulong	tptr;		/* Tx internal data pointer */
	ushort	tbptr;		/* Tx BD pointer */
	ushort	tcount;		/* Tx byte count */
	ulong	ttemp;		/* Tx temp */
	ulong	rcrc;		/* temp receive CRC */
	ulong	tcrc;		/* temp transmit CRC */
	/*
	 * HDLC specific parameter RAM
	 */
	uchar	res[4];		/* reserved */
	ulong	c_mask;		/* CRC constant */
	ulong	c_pres;		/* CRC preset */
	ushort	disfc;		/* discarded frame counter */
	ushort	crcec;		/* CRC error counter */
	ushort	abtsc;		/* abort sequence counter */
	ushort	nmarc;		/* nonmatching address rx cnt */
	ushort	retrc;		/* frame retransmission cnt */
	ushort	mflr;		/* maximum frame length reg */
	ushort	max_cnt;	/* maximum length counter */
	ushort	rfthr;		/* received frames threshold */
	ushort	rfcnt;		/* received frames count */
	ushort	hmask;		/* user defined frm addr mask */
	ushort	haddr1;		/* user defined frm address 1 */
	ushort	haddr2;		/* user defined frm address 2 */
	ushort	haddr3;		/* user defined frm address 3 */
	ushort	haddr4;		/* user defined frm address 4 */
	ushort	tmp;		/* temp */
	ushort	tmp_mb;		/* temp */
} hdlc_pram_t;

/* CPM interrupts.  There are nearly 32 interrupts generated by CPM
 * channels or devices.  All of these are presented to the PPC core
 * as a single interrupt.  The CPM interrupt handler dispatches its
 * own handlers, in a similar fashion to the PPC core handler.  We
 * use the table as defined in the manuals (i.e. no special high
 * priority and SCC1 == SCCa, etc...).
 */
#define CPMVEC_NR		32
#define CPMVEC_OFFSET           0x00010000
#define CPMVEC_PIO_PC15		((ushort)0x1f | CPMVEC_OFFSET)
#define CPMVEC_SCC1		((ushort)0x1e | CPMVEC_OFFSET)
#define CPMVEC_SCC2		((ushort)0x1d | CPMVEC_OFFSET)
#define CPMVEC_SCC3		((ushort)0x1c | CPMVEC_OFFSET)
#define CPMVEC_SCC4		((ushort)0x1b | CPMVEC_OFFSET)
#define CPMVEC_PIO_PC14		((ushort)0x1a | CPMVEC_OFFSET)
#define CPMVEC_TIMER1		((ushort)0x19 | CPMVEC_OFFSET)
#define CPMVEC_PIO_PC13		((ushort)0x18 | CPMVEC_OFFSET)
#define CPMVEC_PIO_PC12		((ushort)0x17 | CPMVEC_OFFSET)
#define CPMVEC_SDMA_CB_ERR	((ushort)0x16 | CPMVEC_OFFSET)
#define CPMVEC_IDMA1		((ushort)0x15 | CPMVEC_OFFSET)
#define CPMVEC_IDMA2		((ushort)0x14 | CPMVEC_OFFSET)
#define CPMVEC_TIMER2		((ushort)0x12 | CPMVEC_OFFSET)
#define CPMVEC_RISCTIMER	((ushort)0x11 | CPMVEC_OFFSET)
#define CPMVEC_I2C		((ushort)0x10 | CPMVEC_OFFSET)
#define CPMVEC_PIO_PC11		((ushort)0x0f | CPMVEC_OFFSET)
#define CPMVEC_PIO_PC10		((ushort)0x0e | CPMVEC_OFFSET)
#define CPMVEC_TIMER3		((ushort)0x0c | CPMVEC_OFFSET)
#define CPMVEC_PIO_PC9		((ushort)0x0b | CPMVEC_OFFSET)
#define CPMVEC_PIO_PC8		((ushort)0x0a | CPMVEC_OFFSET)
#define CPMVEC_PIO_PC7		((ushort)0x09 | CPMVEC_OFFSET)
#define CPMVEC_TIMER4		((ushort)0x07 | CPMVEC_OFFSET)
#define CPMVEC_PIO_PC6		((ushort)0x06 | CPMVEC_OFFSET)
#define CPMVEC_SPI		((ushort)0x05 | CPMVEC_OFFSET)
#define CPMVEC_SMC1		((ushort)0x04 | CPMVEC_OFFSET)
#define CPMVEC_SMC2		((ushort)0x03 | CPMVEC_OFFSET)
#define CPMVEC_PIO_PC5		((ushort)0x02 | CPMVEC_OFFSET)
#define CPMVEC_PIO_PC4		((ushort)0x01 | CPMVEC_OFFSET)
#define CPMVEC_ERROR		((ushort)0x00 | CPMVEC_OFFSET)

extern void irq_install_handler(int vec, void (*handler)(void *), void *dev_id);

/* CPM interrupt configuration vector.
*/
#define	CICR_SCD_SCC4		((uint)0x00c00000)	/* SCC4 @ SCCd */
#define	CICR_SCC_SCC3		((uint)0x00200000)	/* SCC3 @ SCCc */
#define	CICR_SCB_SCC2		((uint)0x00040000)	/* SCC2 @ SCCb */
#define	CICR_SCA_SCC1		((uint)0x00000000)	/* SCC1 @ SCCa */
#define CICR_IRL_MASK		((uint)0x0000e000)	/* Core interrrupt */
#define CICR_HP_MASK		((uint)0x00001f00)	/* Hi-pri int. */
#define CICR_IEN		((uint)0x00000080)	/* Int. enable */
#define CICR_SPS		((uint)0x00000001)	/* SCC Spread */
#endif /* __CPM_8XX__ */
