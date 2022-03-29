
/*
 * MPC85xx Communication Processor Module
 * Copyright (c) 2003,Motorola Inc.
 * Xianghua Xiao (X.Xiao@motorola.com)
 *
 * MPC8260 Communication Processor Module.
 * Copyright (c) 1999 Dan Malek (dmalek@jlc.net)
 *
 * This file contains structures and information for the communication
 * processor channels found in the dual port RAM or parameter RAM.
 * All CPM control and status is available through the MPC8260 internal
 * memory map.  See immap.h for details.
 */
#ifndef __CPM_85XX__
#define __CPM_85XX__

#include <asm/immap_85xx.h>

/* CPM Command register.
*/
#define CPM_CR_RST	((uint)0x80000000)
#define CPM_CR_PAGE	((uint)0x7c000000)
#define CPM_CR_SBLOCK	((uint)0x03e00000)
#define CPM_CR_FLG	((uint)0x00010000)
#define CPM_CR_MCN	((uint)0x00003fc0)
#define CPM_CR_OPCODE	((uint)0x0000000f)

/* Device sub-block and page codes.
*/
#define CPM_CR_SCC1_SBLOCK	(0x04)
#define CPM_CR_SCC2_SBLOCK	(0x05)
#define CPM_CR_SCC3_SBLOCK	(0x06)
#define CPM_CR_SCC4_SBLOCK	(0x07)
#define CPM_CR_SMC1_SBLOCK	(0x08)
#define CPM_CR_SMC2_SBLOCK	(0x09)
#define CPM_CR_SPI_SBLOCK	(0x0a)
#define CPM_CR_I2C_SBLOCK	(0x0b)
#define CPM_CR_TIMER_SBLOCK	(0x0f)
#define CPM_CR_RAND_SBLOCK	(0x0e)
#define CPM_CR_FCC1_SBLOCK	(0x10)
#define CPM_CR_FCC2_SBLOCK	(0x11)
#define CPM_CR_FCC3_SBLOCK	(0x12)
#define CPM_CR_MCC1_SBLOCK	(0x1c)

#define CPM_CR_SCC1_PAGE	(0x00)
#define CPM_CR_SCC2_PAGE	(0x01)
#define CPM_CR_SCC3_PAGE	(0x02)
#define CPM_CR_SCC4_PAGE	(0x03)
#define CPM_CR_SPI_PAGE		(0x09)
#define CPM_CR_I2C_PAGE		(0x0a)
#define CPM_CR_TIMER_PAGE	(0x0a)
#define CPM_CR_RAND_PAGE	(0x0a)
#define CPM_CR_FCC1_PAGE	(0x04)
#define CPM_CR_FCC2_PAGE	(0x05)
#define CPM_CR_FCC3_PAGE	(0x06)
#define CPM_CR_MCC1_PAGE	(0x07)
#define CPM_CR_MCC2_PAGE	(0x08)

/* Some opcodes (there are more...later)
*/
#define CPM_CR_INIT_TRX		((ushort)0x0000)
#define CPM_CR_INIT_RX		((ushort)0x0001)
#define CPM_CR_INIT_TX		((ushort)0x0002)
#define CPM_CR_HUNT_MODE	((ushort)0x0003)
#define CPM_CR_STOP_TX		((ushort)0x0004)
#define CPM_CR_RESTART_TX	((ushort)0x0006)
#define CPM_CR_SET_GADDR	((ushort)0x0008)

#define mk_cr_cmd(PG, SBC, MCN, OP) \
	((PG << 26) | (SBC << 21) | (MCN << 6) | OP)

/* Dual Port RAM addresses.  The first 16K is available for almost
 * any CPM use, so we put the BDs there.  The first 128 bytes are
 * used for SMC1 and SMC2 parameter RAM, so we start allocating
 * BDs above that.  All of this must change when we start
 * downloading RAM microcode.
 */
#define CPM_DATAONLY_BASE	((uint)128)
#define CPM_DP_NOSPACE		((uint)0x7FFFFFFF)
#if defined(CONFIG_MPC8541) || defined(CONFIG_MPC8555)
#define CPM_FCC_SPECIAL_BASE	((uint)0x00009000)
#define CPM_DATAONLY_SIZE	((uint)(8 * 1024) - CPM_DATAONLY_BASE)
#else	/* MPC8540, MPC8560 */
#define CPM_FCC_SPECIAL_BASE	((uint)0x0000B000)
#define CPM_DATAONLY_SIZE	((uint)(16 * 1024) - CPM_DATAONLY_BASE)
#endif

/* The number of pages of host memory we allocate for CPM.  This is
 * done early in kernel initialization to get physically contiguous
 * pages.
 */
#define NUM_CPM_HOST_PAGES	2

/* Export the base address of the communication processor registers
 * and dual port ram.
 */
/*extern	cpm8560_t	*cpmp;	 Pointer to comm processor */
uint		m8560_cpm_dpalloc(uint size, uint align);
uint		m8560_cpm_hostalloc(uint size, uint align);
void		m8560_cpm_setbrg(uint brg, uint rate);
void		m8560_cpm_fastbrg(uint brg, uint rate, int div16);
void		m8560_cpm_extcbrg(uint brg, uint rate, uint extclk, int pinsel);

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
#define BD_SC_CM	((ushort)0x0200)	/* Continous mode */
#define BD_SC_ID	((ushort)0x0100)	/* Rec'd too many idles */
#define BD_SC_P		((ushort)0x0100)	/* xmt preamble */
#define BD_SC_BR	((ushort)0x0020)	/* Break received */
#define BD_SC_FR	((ushort)0x0010)	/* Framing error */
#define BD_SC_PR	((ushort)0x0008)	/* Parity error */
#define BD_SC_OV	((ushort)0x0002)	/* Overrun */
#define BD_SC_CD	((ushort)0x0001)	/* ?? */

/* Function code bits, usually generic to devices.
*/
#define CPMFCR_GBL	((u_char)0x20)	/* Set memory snooping */
#define CPMFCR_EB	((u_char)0x10)	/* Set big endian byte order */
#define CPMFCR_TC2	((u_char)0x04)	/* Transfer code 2 value */
#define CPMFCR_DTB	((u_char)0x02)	/* Use local bus for data when set */
#define CPMFCR_BDB	((u_char)0x01)	/* Use local bus for BD when set */

/* Parameter RAM offsets from the base.
*/
#define CPM_POST_WORD_ADDR      0x80FC	/* steal a long at the end of SCC1 */
#define PROFF_SCC1		((uint)0x8000)
#define PROFF_SCC2		((uint)0x8100)
#define PROFF_SCC3		((uint)0x8200)
#define PROFF_SCC4		((uint)0x8300)
#define PROFF_FCC1		((uint)0x8400)
#define PROFF_FCC2		((uint)0x8500)
#define PROFF_FCC3		((uint)0x8600)
#define PROFF_MCC1		((uint)0x8700)
#define PROFF_MCC2		((uint)0x8800)
#define PROFF_SPI_BASE		((uint)0x89fc)
#define PROFF_TIMERS		((uint)0x8ae0)
#define PROFF_REVNUM		((uint)0x8af0)
#define PROFF_RAND		((uint)0x8af8)
#define PROFF_I2C_BASE		((uint)0x8afc)

/* Baud rate generators.
*/
#define CPM_BRG_RST		((uint)0x00020000)
#define CPM_BRG_EN		((uint)0x00010000)
#define CPM_BRG_EXTC_INT	((uint)0x00000000)
#define CPM_BRG_EXTC_CLK3_9	((uint)0x00004000)
#define CPM_BRG_EXTC_CLK5_15	((uint)0x00008000)
#define CPM_BRG_ATB		((uint)0x00002000)
#define CPM_BRG_CD_MASK		((uint)0x00001ffe)
#define CPM_BRG_DIV16		((uint)0x00000001)

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

/* CPM Ethernet through SCC1.
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
 * Common to SCC and FCC.
 */
#define BD_ENET_RX_EMPTY	((ushort)0x8000)
#define BD_ENET_RX_WRAP		((ushort)0x2000)
#define BD_ENET_RX_INTR		((ushort)0x1000)
#define BD_ENET_RX_LAST		((ushort)0x0800)
#define BD_ENET_RX_FIRST	((ushort)0x0400)
#define BD_ENET_RX_MISS		((ushort)0x0100)
#define BD_ENET_RX_BC		((ushort)0x0080)	/* FCC Only */
#define BD_ENET_RX_MC		((ushort)0x0040)	/* FCC Only */
#define BD_ENET_RX_LG		((ushort)0x0020)
#define BD_ENET_RX_NO		((ushort)0x0010)
#define BD_ENET_RX_SH		((ushort)0x0008)
#define BD_ENET_RX_CR		((ushort)0x0004)
#define BD_ENET_RX_OV		((ushort)0x0002)
#define BD_ENET_RX_CL		((ushort)0x0001)
#define BD_ENET_RX_STATS	((ushort)0x01ff)	/* All status bits */

/* Buffer descriptor control/status used by Ethernet transmit.
 * Common to SCC and FCC.
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

/* How about some FCCs.....
*/
#define FCC_GFMR_DIAG_NORM	((uint)0x00000000)
#define FCC_GFMR_DIAG_LE	((uint)0x40000000)
#define FCC_GFMR_DIAG_AE	((uint)0x80000000)
#define FCC_GFMR_DIAG_ALE	((uint)0xc0000000)
#define FCC_GFMR_TCI		((uint)0x20000000)
#define FCC_GFMR_TRX		((uint)0x10000000)
#define FCC_GFMR_TTX		((uint)0x08000000)
#define FCC_GFMR_TTX		((uint)0x08000000)
#define FCC_GFMR_CDP		((uint)0x04000000)
#define FCC_GFMR_CTSP		((uint)0x02000000)
#define FCC_GFMR_CDS		((uint)0x01000000)
#define FCC_GFMR_CTSS		((uint)0x00800000)
#define FCC_GFMR_SYNL_NONE	((uint)0x00000000)
#define FCC_GFMR_SYNL_AUTO	((uint)0x00004000)
#define FCC_GFMR_SYNL_8		((uint)0x00008000)
#define FCC_GFMR_SYNL_16	((uint)0x0000c000)
#define FCC_GFMR_RTSM		((uint)0x00002000)
#define FCC_GFMR_RENC_NRZ	((uint)0x00000000)
#define FCC_GFMR_RENC_NRZI	((uint)0x00000800)
#define FCC_GFMR_REVD		((uint)0x00000400)
#define FCC_GFMR_TENC_NRZ	((uint)0x00000000)
#define FCC_GFMR_TENC_NRZI	((uint)0x00000100)
#define FCC_GFMR_TCRC_16	((uint)0x00000000)
#define FCC_GFMR_TCRC_32	((uint)0x00000080)
#define FCC_GFMR_ENR		((uint)0x00000020)
#define FCC_GFMR_ENT		((uint)0x00000010)
#define FCC_GFMR_MODE_ENET	((uint)0x0000000c)
#define FCC_GFMR_MODE_ATM	((uint)0x0000000a)
#define FCC_GFMR_MODE_HDLC	((uint)0x00000000)

/* Generic FCC parameter ram.
*/
typedef struct fcc_param {
	ushort	fcc_riptr;	/* Rx Internal temp pointer */
	ushort	fcc_tiptr;	/* Tx Internal temp pointer */
	ushort	fcc_res1;
	ushort	fcc_mrblr;	/* Max receive buffer length, mod 32 bytes */
	uint	fcc_rstate;	/* Upper byte is Func code, must be set */
	uint	fcc_rbase;	/* Receive BD base */
	ushort	fcc_rbdstat;	/* RxBD status */
	ushort	fcc_rbdlen;	/* RxBD down counter */
	uint	fcc_rdptr;	/* RxBD internal data pointer */
	uint	fcc_tstate;	/* Upper byte is Func code, must be set */
	uint	fcc_tbase;	/* Transmit BD base */
	ushort	fcc_tbdstat;	/* TxBD status */
	ushort	fcc_tbdlen;	/* TxBD down counter */
	uint	fcc_tdptr;	/* TxBD internal data pointer */
	uint	fcc_rbptr;	/* Rx BD Internal buf pointer */
	uint	fcc_tbptr;	/* Tx BD Internal buf pointer */
	uint	fcc_rcrc;	/* Rx temp CRC */
	uint	fcc_res2;
	uint	fcc_tcrc;	/* Tx temp CRC */
} fccp_t;


/* Ethernet controller through FCC.
*/
typedef struct fcc_enet {
	fccp_t	fen_genfcc;
	uint	fen_statbuf;	/* Internal status buffer */
	uint	fen_camptr;	/* CAM address */
	uint	fen_cmask;	/* Constant mask for CRC */
	uint	fen_cpres;	/* Preset CRC */
	uint	fen_crcec;	/* CRC Error counter */
	uint	fen_alec;	/* alignment error counter */
	uint	fen_disfc;	/* discard frame counter */
	ushort	fen_retlim;	/* Retry limit */
	ushort	fen_retcnt;	/* Retry counter */
	ushort	fen_pper;	/* Persistence */
	ushort	fen_boffcnt;	/* backoff counter */
	uint	fen_gaddrh;	/* Group address filter, high 32-bits */
	uint	fen_gaddrl;	/* Group address filter, low 32-bits */
	ushort	fen_tfcstat;	/* out of sequence TxBD */
	ushort	fen_tfclen;
	uint	fen_tfcptr;
	ushort	fen_mflr;	/* Maximum frame length (1518) */
	ushort	fen_paddrh;	/* MAC address */
	ushort	fen_paddrm;
	ushort	fen_paddrl;
	ushort	fen_ibdcount;	/* Internal BD counter */
	ushort	fen_ibdstart;	/* Internal BD start pointer */
	ushort	fen_ibdend;	/* Internal BD end pointer */
	ushort	fen_txlen;	/* Internal Tx frame length counter */
	uint	fen_ibdbase[8]; /* Internal use */
	uint	fen_iaddrh;	/* Individual address filter */
	uint	fen_iaddrl;
	ushort	fen_minflr;	/* Minimum frame length (64) */
	ushort	fen_taddrh;	/* Filter transfer MAC address */
	ushort	fen_taddrm;
	ushort	fen_taddrl;
	ushort	fen_padptr;	/* Pointer to pad byte buffer */
	ushort	fen_cftype;	/* control frame type */
	ushort	fen_cfrange;	/* control frame range */
	ushort	fen_maxb;	/* maximum BD count */
	ushort	fen_maxd1;	/* Max DMA1 length (1520) */
	ushort	fen_maxd2;	/* Max DMA2 length (1520) */
	ushort	fen_maxd;	/* internal max DMA count */
	ushort	fen_dmacnt;	/* internal DMA counter */
	uint	fen_octc;	/* Total octect counter */
	uint	fen_colc;	/* Total collision counter */
	uint	fen_broc;	/* Total broadcast packet counter */
	uint	fen_mulc;	/* Total multicast packet count */
	uint	fen_uspc;	/* Total packets < 64 bytes */
	uint	fen_frgc;	/* Total packets < 64 bytes with errors */
	uint	fen_ospc;	/* Total packets > 1518 */
	uint	fen_jbrc;	/* Total packets > 1518 with errors */
	uint	fen_p64c;	/* Total packets == 64 bytes */
	uint	fen_p65c;	/* Total packets 64 < bytes <= 127 */
	uint	fen_p128c;	/* Total packets 127 < bytes <= 255 */
	uint	fen_p256c;	/* Total packets 256 < bytes <= 511 */
	uint	fen_p512c;	/* Total packets 512 < bytes <= 1023 */
	uint	fen_p1024c;	/* Total packets 1024 < bytes <= 1518 */
	uint	fen_cambuf;	/* Internal CAM buffer poiner */
	ushort	fen_rfthr;	/* Received frames threshold */
	ushort	fen_rfcnt;	/* Received frames count */
} fcc_enet_t;

/* FCC Event/Mask register as used by Ethernet.
*/
#define FCC_ENET_GRA	((ushort)0x0080)	/* Graceful stop complete */
#define FCC_ENET_RXC	((ushort)0x0040)	/* Control Frame Received */
#define FCC_ENET_TXC	((ushort)0x0020)	/* Out of seq. Tx sent */
#define FCC_ENET_TXE	((ushort)0x0010)	/* Transmit Error */
#define FCC_ENET_RXF	((ushort)0x0008)	/* Full frame received */
#define FCC_ENET_BSY	((ushort)0x0004)	/* Busy.  Rx Frame dropped */
#define FCC_ENET_TXB	((ushort)0x0002)	/* A buffer was transmitted */
#define FCC_ENET_RXB	((ushort)0x0001)	/* A buffer was received */

/* FCC Mode Register (FPSMR) as used by Ethernet.
*/
#define FCC_PSMR_HBC	((uint)0x80000000)	/* Enable heartbeat */
#define FCC_PSMR_FC	((uint)0x40000000)	/* Force Collision */
#define FCC_PSMR_SBT	((uint)0x20000000)	/* Stop backoff timer */
#define FCC_PSMR_LPB	((uint)0x10000000)	/* Local protect. 1 = FDX */
#define FCC_PSMR_LCW	((uint)0x08000000)	/* Late collision select */
#define FCC_PSMR_FDE	((uint)0x04000000)	/* Full Duplex Enable */
#define FCC_PSMR_MON	((uint)0x02000000)	/* RMON Enable */
#define FCC_PSMR_PRO	((uint)0x00400000)	/* Promiscuous Enable */
#define FCC_PSMR_FCE	((uint)0x00200000)	/* Flow Control Enable */
#define FCC_PSMR_RSH	((uint)0x00100000)	/* Receive Short Frames */
#define FCC_PSMR_CAM	((uint)0x00000400)	/* CAM enable */
#define FCC_PSMR_BRO	((uint)0x00000200)	/* Broadcast pkt discard */
#define FCC_PSMR_ENCRC	((uint)0x00000080)	/* Use 32-bit CRC */

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
	uint	spi_res;	/* Tx temp. */
	uint	spi_res1[4];	/* SDMA temp. */
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

#define SPI_EB		((u_char)0x10)		/* big endian byte order */

#define BD_IIC_START	((ushort)0x0400)

/*-----------------------------------------------------------------------
 * CMXFCR - CMX FCC Clock Route Register                                15-12
 */
#define CMXFCR_FC1         0x40000000   /* FCC1 connection              */
#define CMXFCR_RF1CS_MSK   0x38000000   /* Receive FCC1 Clock Source Mask */
#define CMXFCR_TF1CS_MSK   0x07000000   /* Transmit FCC1 Clock Source Mask */
#define CMXFCR_FC2         0x00400000   /* FCC2 connection              */
#define CMXFCR_RF2CS_MSK   0x00380000   /* Receive FCC2 Clock Source Mask */
#define CMXFCR_TF2CS_MSK   0x00070000   /* Transmit FCC2 Clock Source Mask */
#define CMXFCR_FC3         0x00004000   /* FCC3 connection              */
#define CMXFCR_RF3CS_MSK   0x00003800   /* Receive FCC3 Clock Source Mask */
#define CMXFCR_TF3CS_MSK   0x00000700   /* Transmit FCC3 Clock Source Mask */

#define CMXFCR_RF1CS_BRG5  0x00000000   /* Receive FCC1 Clock Source is BRG5 */
#define CMXFCR_RF1CS_BRG6  0x08000000   /* Receive FCC1 Clock Source is BRG6 */
#define CMXFCR_RF1CS_BRG7  0x10000000   /* Receive FCC1 Clock Source is BRG7 */
#define CMXFCR_RF1CS_BRG8  0x18000000   /* Receive FCC1 Clock Source is BRG8 */
#define CMXFCR_RF1CS_CLK9  0x20000000   /* Receive FCC1 Clock Source is CLK9 */
#define CMXFCR_RF1CS_CLK10 0x28000000   /* Receive FCC1 Clock Source is CLK10 */
#define CMXFCR_RF1CS_CLK11 0x30000000   /* Receive FCC1 Clock Source is CLK11 */
#define CMXFCR_RF1CS_CLK12 0x38000000   /* Receive FCC1 Clock Source is CLK12 */

#define CMXFCR_TF1CS_BRG5  0x00000000   /* Transmit FCC1 Clock Source is BRG5 */
#define CMXFCR_TF1CS_BRG6  0x01000000   /* Transmit FCC1 Clock Source is BRG6 */
#define CMXFCR_TF1CS_BRG7  0x02000000   /* Transmit FCC1 Clock Source is BRG7 */
#define CMXFCR_TF1CS_BRG8  0x03000000   /* Transmit FCC1 Clock Source is BRG8 */
#define CMXFCR_TF1CS_CLK9  0x04000000   /* Transmit FCC1 Clock Source is CLK9 */
#define CMXFCR_TF1CS_CLK10 0x05000000   /* Transmit FCC1 Clock Source is CLK10 */
#define CMXFCR_TF1CS_CLK11 0x06000000   /* Transmit FCC1 Clock Source is CLK11 */
#define CMXFCR_TF1CS_CLK12 0x07000000   /* Transmit FCC1 Clock Source is CLK12 */

#define CMXFCR_RF2CS_BRG5  0x00000000   /* Receive FCC2 Clock Source is BRG5 */
#define CMXFCR_RF2CS_BRG6  0x00080000   /* Receive FCC2 Clock Source is BRG6 */
#define CMXFCR_RF2CS_BRG7  0x00100000   /* Receive FCC2 Clock Source is BRG7 */
#define CMXFCR_RF2CS_BRG8  0x00180000   /* Receive FCC2 Clock Source is BRG8 */
#define CMXFCR_RF2CS_CLK13 0x00200000   /* Receive FCC2 Clock Source is CLK13 */
#define CMXFCR_RF2CS_CLK14 0x00280000   /* Receive FCC2 Clock Source is CLK14 */
#define CMXFCR_RF2CS_CLK15 0x00300000   /* Receive FCC2 Clock Source is CLK15 */
#define CMXFCR_RF2CS_CLK16 0x00380000   /* Receive FCC2 Clock Source is CLK16 */

#define CMXFCR_TF2CS_BRG5  0x00000000   /* Transmit FCC2 Clock Source is BRG5 */
#define CMXFCR_TF2CS_BRG6  0x00010000   /* Transmit FCC2 Clock Source is BRG6 */
#define CMXFCR_TF2CS_BRG7  0x00020000   /* Transmit FCC2 Clock Source is BRG7 */
#define CMXFCR_TF2CS_BRG8  0x00030000   /* Transmit FCC2 Clock Source is BRG8 */
#define CMXFCR_TF2CS_CLK13 0x00040000   /* Transmit FCC2 Clock Source is CLK13 */
#define CMXFCR_TF2CS_CLK14 0x00050000   /* Transmit FCC2 Clock Source is CLK14 */
#define CMXFCR_TF2CS_CLK15 0x00060000   /* Transmit FCC2 Clock Source is CLK15 */
#define CMXFCR_TF2CS_CLK16 0x00070000   /* Transmit FCC2 Clock Source is CLK16 */

#define CMXFCR_RF3CS_BRG5  0x00000000   /* Receive FCC3 Clock Source is BRG5 */
#define CMXFCR_RF3CS_BRG6  0x00000800   /* Receive FCC3 Clock Source is BRG6 */
#define CMXFCR_RF3CS_BRG7  0x00001000   /* Receive FCC3 Clock Source is BRG7 */
#define CMXFCR_RF3CS_BRG8  0x00001800   /* Receive FCC3 Clock Source is BRG8 */
#define CMXFCR_RF3CS_CLK13 0x00002000   /* Receive FCC3 Clock Source is CLK13 */
#define CMXFCR_RF3CS_CLK14 0x00002800   /* Receive FCC3 Clock Source is CLK14 */
#define CMXFCR_RF3CS_CLK15 0x00003000   /* Receive FCC3 Clock Source is CLK15 */
#define CMXFCR_RF3CS_CLK16 0x00003800   /* Receive FCC3 Clock Source is CLK16 */

#define CMXFCR_TF3CS_BRG5  0x00000000   /* Transmit FCC3 Clock Source is BRG5 */
#define CMXFCR_TF3CS_BRG6  0x00000100   /* Transmit FCC3 Clock Source is BRG6 */
#define CMXFCR_TF3CS_BRG7  0x00000200   /* Transmit FCC3 Clock Source is BRG7 */
#define CMXFCR_TF3CS_BRG8  0x00000300   /* Transmit FCC3 Clock Source is BRG8 */
#define CMXFCR_TF3CS_CLK13 0x00000400   /* Transmit FCC3 Clock Source is CLK13 */
#define CMXFCR_TF3CS_CLK14 0x00000500   /* Transmit FCC3 Clock Source is CLK14 */
#define CMXFCR_TF3CS_CLK15 0x00000600   /* Transmit FCC3 Clock Source is CLK15 */
#define CMXFCR_TF3CS_CLK16 0x00000700   /* Transmit FCC3 Clock Source is CLK16 */

/*-----------------------------------------------------------------------
 * CMXSCR - CMX SCC Clock Route Register                                15-14
 */
#define CMXSCR_GR1         0x80000000   /* Grant Support of SCC1        */
#define CMXSCR_SC1         0x40000000   /* SCC1 connection              */
#define CMXSCR_RS1CS_MSK   0x38000000   /* Receive SCC1 Clock Source Mask */
#define CMXSCR_TS1CS_MSK   0x07000000   /* Transmit SCC1 Clock Source Mask */
#define CMXSCR_GR2         0x00800000   /* Grant Support of SCC2        */
#define CMXSCR_SC2         0x00400000   /* SCC2 connection              */
#define CMXSCR_RS2CS_MSK   0x00380000   /* Receive SCC2 Clock Source Mask */
#define CMXSCR_TS2CS_MSK   0x00070000   /* Transmit SCC2 Clock Source Mask */
#define CMXSCR_GR3         0x00008000   /* Grant Support of SCC3        */
#define CMXSCR_SC3         0x00004000   /* SCC3 connection              */
#define CMXSCR_RS3CS_MSK   0x00003800   /* Receive SCC3 Clock Source Mask */
#define CMXSCR_TS3CS_MSK   0x00000700   /* Transmit SCC3 Clock Source Mask */
#define CMXSCR_GR4         0x00000080   /* Grant Support of SCC4        */
#define CMXSCR_SC4         0x00000040   /* SCC4 connection              */
#define CMXSCR_RS4CS_MSK   0x00000038   /* Receive SCC4 Clock Source Mask */
#define CMXSCR_TS4CS_MSK   0x00000007   /* Transmit SCC4 Clock Source Mask */

#define CMXSCR_RS1CS_BRG1  0x00000000   /* SCC1 Rx Clock Source is BRG1 */
#define CMXSCR_RS1CS_BRG2  0x08000000   /* SCC1 Rx Clock Source is BRG2 */
#define CMXSCR_RS1CS_BRG3  0x10000000   /* SCC1 Rx Clock Source is BRG3 */
#define CMXSCR_RS1CS_BRG4  0x18000000   /* SCC1 Rx Clock Source is BRG4 */
#define CMXSCR_RS1CS_CLK11 0x20000000   /* SCC1 Rx Clock Source is CLK11 */
#define CMXSCR_RS1CS_CLK12 0x28000000   /* SCC1 Rx Clock Source is CLK12 */
#define CMXSCR_RS1CS_CLK3  0x30000000   /* SCC1 Rx Clock Source is CLK3 */
#define CMXSCR_RS1CS_CLK4  0x38000000   /* SCC1 Rx Clock Source is CLK4 */

#define CMXSCR_TS1CS_BRG1  0x00000000   /* SCC1 Tx Clock Source is BRG1 */
#define CMXSCR_TS1CS_BRG2  0x01000000   /* SCC1 Tx Clock Source is BRG2 */
#define CMXSCR_TS1CS_BRG3  0x02000000   /* SCC1 Tx Clock Source is BRG3 */
#define CMXSCR_TS1CS_BRG4  0x03000000   /* SCC1 Tx Clock Source is BRG4 */
#define CMXSCR_TS1CS_CLK11 0x04000000   /* SCC1 Tx Clock Source is CLK11 */
#define CMXSCR_TS1CS_CLK12 0x05000000   /* SCC1 Tx Clock Source is CLK12 */
#define CMXSCR_TS1CS_CLK3  0x06000000   /* SCC1 Tx Clock Source is CLK3 */
#define CMXSCR_TS1CS_CLK4  0x07000000   /* SCC1 Tx Clock Source is CLK4 */

#define CMXSCR_RS2CS_BRG1  0x00000000   /* SCC2 Rx Clock Source is BRG1 */
#define CMXSCR_RS2CS_BRG2  0x00080000   /* SCC2 Rx Clock Source is BRG2 */
#define CMXSCR_RS2CS_BRG3  0x00100000   /* SCC2 Rx Clock Source is BRG3 */
#define CMXSCR_RS2CS_BRG4  0x00180000   /* SCC2 Rx Clock Source is BRG4 */
#define CMXSCR_RS2CS_CLK11 0x00200000   /* SCC2 Rx Clock Source is CLK11 */
#define CMXSCR_RS2CS_CLK12 0x00280000   /* SCC2 Rx Clock Source is CLK12 */
#define CMXSCR_RS2CS_CLK3  0x00300000   /* SCC2 Rx Clock Source is CLK3 */
#define CMXSCR_RS2CS_CLK4  0x00380000   /* SCC2 Rx Clock Source is CLK4 */

#define CMXSCR_TS2CS_BRG1  0x00000000   /* SCC2 Tx Clock Source is BRG1 */
#define CMXSCR_TS2CS_BRG2  0x00010000   /* SCC2 Tx Clock Source is BRG2 */
#define CMXSCR_TS2CS_BRG3  0x00020000   /* SCC2 Tx Clock Source is BRG3 */
#define CMXSCR_TS2CS_BRG4  0x00030000   /* SCC2 Tx Clock Source is BRG4 */
#define CMXSCR_TS2CS_CLK11 0x00040000   /* SCC2 Tx Clock Source is CLK11 */
#define CMXSCR_TS2CS_CLK12 0x00050000   /* SCC2 Tx Clock Source is CLK12 */
#define CMXSCR_TS2CS_CLK3  0x00060000   /* SCC2 Tx Clock Source is CLK3 */
#define CMXSCR_TS2CS_CLK4  0x00070000   /* SCC2 Tx Clock Source is CLK4 */

#define CMXSCR_RS3CS_BRG1  0x00000000   /* SCC3 Rx Clock Source is BRG1 */
#define CMXSCR_RS3CS_BRG2  0x00000800   /* SCC3 Rx Clock Source is BRG2 */
#define CMXSCR_RS3CS_BRG3  0x00001000   /* SCC3 Rx Clock Source is BRG3 */
#define CMXSCR_RS3CS_BRG4  0x00001800   /* SCC3 Rx Clock Source is BRG4 */
#define CMXSCR_RS3CS_CLK5  0x00002000   /* SCC3 Rx Clock Source is CLK5 */
#define CMXSCR_RS3CS_CLK6  0x00002800   /* SCC3 Rx Clock Source is CLK6 */
#define CMXSCR_RS3CS_CLK7  0x00003000   /* SCC3 Rx Clock Source is CLK7 */
#define CMXSCR_RS3CS_CLK8  0x00003800   /* SCC3 Rx Clock Source is CLK8 */

#define CMXSCR_TS3CS_BRG1  0x00000000   /* SCC3 Tx Clock Source is BRG1 */
#define CMXSCR_TS3CS_BRG2  0x00000100   /* SCC3 Tx Clock Source is BRG2 */
#define CMXSCR_TS3CS_BRG3  0x00000200   /* SCC3 Tx Clock Source is BRG3 */
#define CMXSCR_TS3CS_BRG4  0x00000300   /* SCC3 Tx Clock Source is BRG4 */
#define CMXSCR_TS3CS_CLK5  0x00000400   /* SCC3 Tx Clock Source is CLK5 */
#define CMXSCR_TS3CS_CLK6  0x00000500   /* SCC3 Tx Clock Source is CLK6 */
#define CMXSCR_TS3CS_CLK7  0x00000600   /* SCC3 Tx Clock Source is CLK7 */
#define CMXSCR_TS3CS_CLK8  0x00000700   /* SCC3 Tx Clock Source is CLK8 */

#define CMXSCR_RS4CS_BRG1  0x00000000   /* SCC4 Rx Clock Source is BRG1 */
#define CMXSCR_RS4CS_BRG2  0x00000008   /* SCC4 Rx Clock Source is BRG2 */
#define CMXSCR_RS4CS_BRG3  0x00000010   /* SCC4 Rx Clock Source is BRG3 */
#define CMXSCR_RS4CS_BRG4  0x00000018   /* SCC4 Rx Clock Source is BRG4 */
#define CMXSCR_RS4CS_CLK5  0x00000020   /* SCC4 Rx Clock Source is CLK5 */
#define CMXSCR_RS4CS_CLK6  0x00000028   /* SCC4 Rx Clock Source is CLK6 */
#define CMXSCR_RS4CS_CLK7  0x00000030   /* SCC4 Rx Clock Source is CLK7 */
#define CMXSCR_RS4CS_CLK8  0x00000038   /* SCC4 Rx Clock Source is CLK8 */

#define CMXSCR_TS4CS_BRG1  0x00000000   /* SCC4 Tx Clock Source is BRG1 */
#define CMXSCR_TS4CS_BRG2  0x00000001   /* SCC4 Tx Clock Source is BRG2 */
#define CMXSCR_TS4CS_BRG3  0x00000002   /* SCC4 Tx Clock Source is BRG3 */
#define CMXSCR_TS4CS_BRG4  0x00000003   /* SCC4 Tx Clock Source is BRG4 */
#define CMXSCR_TS4CS_CLK5  0x00000004   /* SCC4 Tx Clock Source is CLK5 */
#define CMXSCR_TS4CS_CLK6  0x00000005   /* SCC4 Tx Clock Source is CLK6 */
#define CMXSCR_TS4CS_CLK7  0x00000006   /* SCC4 Tx Clock Source is CLK7 */
#define CMXSCR_TS4CS_CLK8  0x00000007   /* SCC4 Tx Clock Source is CLK8 */

#endif /* __CPM_85XX__ */
