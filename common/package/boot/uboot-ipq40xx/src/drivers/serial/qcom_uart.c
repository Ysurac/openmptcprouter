/*
 * Copyright (c) 2012-2014 The Linux Foundation. All rights reserved.
 * Source : APQ8064 LK boot
 *
 * Copyright (c) 2011-2012, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of Google, Inc. nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <common.h>
#include <watchdog.h>
#include <asm/arch-qcom-common/clk.h>
#include <asm/arch-qcom-common/uart.h>
#include <asm/arch-qcom-common/gsbi.h>
#include "ipq40xx_cdp.h"

#define FIFO_DATA_SIZE	4

#if defined CONFIG_IPQ806X
extern board_ipq806x_params_t *gboard_param;
#elif defined CONFIG_IPQ40XX
extern board_ipq40xx_params_t *gboard_param;
#endif

static unsigned int msm_boot_uart_dm_init(unsigned int  uart_dm_base);

#define UART2_M  0x18
#define UART2_N  625
#define UART2_2D 313
/* Received data is valid or not */
static int valid_data = 0;
static int uart_valid_data = 0;

/* Received data */
static unsigned int word = 0;
static unsigned int uart_word = 0;

/**
 * msm_boot_uart_dm_init_rx_transfer - Init Rx transfer
 * @uart_dm_base: UART controller base address
 */
static unsigned int msm_boot_uart_dm_init_rx_transfer(unsigned int uart_dm_base)
{
	/* Reset receiver */
	writel(MSM_BOOT_UART_DM_CMD_RESET_RX,
		MSM_BOOT_UART_DM_CR(uart_dm_base));

	/* Enable receiver */
	writel(MSM_BOOT_UART_DM_CR_RX_ENABLE,
		MSM_BOOT_UART_DM_CR(uart_dm_base));
	writel(MSM_BOOT_UART_DM_DMRX_DEF_VALUE,
		MSM_BOOT_UART_DM_DMRX(uart_dm_base));

	/* Clear stale event */
	writel(MSM_BOOT_UART_DM_CMD_RES_STALE_INT,
		MSM_BOOT_UART_DM_CR(uart_dm_base));

	/* Enable stale event */
	writel(MSM_BOOT_UART_DM_GCMD_ENA_STALE_EVT,
		MSM_BOOT_UART_DM_CR(uart_dm_base));

	return MSM_BOOT_UART_DM_E_SUCCESS;
}

/**
 * msm_boot_uart_dm_read - reads a word from the RX FIFO.
 * @data: location where the read data is stored
 * @count: no of valid data in the FIFO
 * @wait: indicates blocking call or not blocking call
 *
 * Reads a word from the RX FIFO. If no data is available blocks if
 * @wait is true, else returns %MSM_BOOT_UART_DM_E_RX_NOT_READY.
 */
static unsigned int
msm_boot_uart_dm_read(unsigned int *data, int *count, int wait,
			uart_cfg_t *uart_cfg)
{
	static int total_rx_data = 0;
	static int rx_data_read = 0;
	unsigned int  base = 0;
	uint32_t status_reg;

	base = uart_cfg->uart_dm_base;

	if (data == NULL)
		return MSM_BOOT_UART_DM_E_INVAL;

	status_reg = readl(MSM_BOOT_UART_DM_MISR(base));

	/* Check for DM_RXSTALE for RX transfer to finish */
	while (!(status_reg & MSM_BOOT_UART_DM_RXSTALE)) {
		status_reg = readl(MSM_BOOT_UART_DM_MISR(base));
		if (!wait)
			return MSM_BOOT_UART_DM_E_RX_NOT_READY;
	}

	/* Check for Overrun error. We'll just reset Error Status */
	if (readl(MSM_BOOT_UART_DM_SR(base)) &
			MSM_BOOT_UART_DM_SR_UART_OVERRUN) {
		writel(MSM_BOOT_UART_DM_CMD_RESET_ERR_STAT,
			MSM_BOOT_UART_DM_CR(base));
		total_rx_data = rx_data_read = 0;
		msm_boot_uart_dm_init(base);
		return MSM_BOOT_UART_DM_E_RX_NOT_READY;
	}

	/* Read UART_DM_RX_TOTAL_SNAP for actual number of bytes received */
	if (total_rx_data == 0)
		total_rx_data =  readl(MSM_BOOT_UART_DM_RX_TOTAL_SNAP(base));

	/* Data available in FIFO; read a word. */
	*data = readl(MSM_BOOT_UART_DM_RF(base, 0));

	/* WAR for http://prism/CR/548280 */
	if (*data == 0) {
		return MSM_BOOT_UART_DM_E_RX_NOT_READY;
	}

	/* increment the total count of chars we've read so far */
	rx_data_read += FIFO_DATA_SIZE;

	/* actual count of valid data in word */
	*count = ((total_rx_data < rx_data_read) ?
			(FIFO_DATA_SIZE - (rx_data_read - total_rx_data)) :
			FIFO_DATA_SIZE);

	/* If there are still data left in FIFO we'll read them before
	 * initializing RX Transfer again
	 */
	if (rx_data_read < total_rx_data)
		return MSM_BOOT_UART_DM_E_SUCCESS;

	msm_boot_uart_dm_init_rx_transfer(base);
	total_rx_data = rx_data_read = 0;

	return MSM_BOOT_UART_DM_E_SUCCESS;
}

/**
 * msm_boot_uart_replace_lr_with_cr - replaces "\n" with "\r\n"
 * @data_in:      characters to be converted
 * @num_of_chars: no. of characters
 * @data_out:     location where converted chars are stored
 *
 * Replace linefeed char "\n" with carriage return + linefeed
 * "\r\n". Currently keeping it simple than efficient.
 */
static unsigned int
msm_boot_uart_replace_lr_with_cr(char *data_in,
                                 int num_of_chars,
                                 char *data_out, int *num_of_chars_out)
{
        int i = 0, j = 0;

        if ((data_in == NULL) || (data_out == NULL) || (num_of_chars < 0))
                return MSM_BOOT_UART_DM_E_INVAL;

        for (i = 0, j = 0; i < num_of_chars; i++, j++) {
                if (data_in[i] == '\n')
                        data_out[j++] = '\r';

                data_out[j] = data_in[i];
        }

        *num_of_chars_out = j;

        return MSM_BOOT_UART_DM_E_SUCCESS;
}

/**
 * msm_boot_uart_dm_write - transmit data
 * @data:          data to transmit
 * @num_of_chars:  no. of bytes to transmit
 *
 * Writes the data to the TX FIFO. If no space is available blocks
 * till space becomes available.
 */
static unsigned int
msm_boot_uart_dm_write(char *data, unsigned int num_of_chars,
			uart_cfg_t *uart_cfg)
{
	unsigned int tx_word_count = 0;
	unsigned int tx_char_left = 0, tx_char = 0;
	unsigned int tx_word = 0;
	int i = 0;
	char *tx_data = NULL;
	char new_data[1024];
	unsigned int base = uart_cfg->uart_dm_base;

        if ((data == NULL) || (num_of_chars <= 0))
                return MSM_BOOT_UART_DM_E_INVAL;

        /* Replace line-feed (/n) with carriage-return + line-feed (/r/n) */
        msm_boot_uart_replace_lr_with_cr(data, num_of_chars, new_data, &i);

        tx_data = new_data;
        num_of_chars = i;

        /* Write to NO_CHARS_FOR_TX register number of characters
        * to be transmitted. However, before writing TX_FIFO must
        * be empty as indicated by TX_READY interrupt in IMR register
        */
        /* Check if transmit FIFO is empty.
        * If not we'll wait for TX_READY interrupt. */

        if (!(readl(MSM_BOOT_UART_DM_SR(base)) & MSM_BOOT_UART_DM_SR_TXEMT)) {
                while (!(readl(MSM_BOOT_UART_DM_ISR(base)) & MSM_BOOT_UART_DM_TX_READY))
                        __udelay(1);
        }

        /* We are here. FIFO is ready to be written. */
        /* Write number of characters to be written */
        writel(num_of_chars, MSM_BOOT_UART_DM_NO_CHARS_FOR_TX(base));

        /* Clear TX_READY interrupt */
        writel(MSM_BOOT_UART_DM_GCMD_RES_TX_RDY_INT, MSM_BOOT_UART_DM_CR(base));

        /* We use four-character word FIFO. So we need to divide data into
        * four characters and write in UART_DM_TF register */
        tx_word_count = (num_of_chars % 4) ? ((num_of_chars / 4) + 1) :
                        (num_of_chars / 4);
        tx_char_left = num_of_chars;

        for (i = 0; i < (int)tx_word_count; i++) {
                tx_char = (tx_char_left < 4) ? tx_char_left : 4;
                PACK_CHARS_INTO_WORDS(tx_data, tx_char, tx_word);

                /* Wait till TX FIFO has space */
                while (!(readl(MSM_BOOT_UART_DM_SR(base)) & MSM_BOOT_UART_DM_SR_TXRDY))
                        __udelay(1);

                /* TX FIFO has space. Write the chars */
                writel(tx_word, MSM_BOOT_UART_DM_TF(base, 0));
                tx_char_left = num_of_chars - (i + 1) * 4;
                tx_data = tx_data + 4;
        }

        return MSM_BOOT_UART_DM_E_SUCCESS;
}

/*
 * msm_boot_uart_dm_reset - resets UART controller
 * @base: UART controller base address
 */
static unsigned int msm_boot_uart_dm_reset(unsigned int base)
{
	writel(MSM_BOOT_UART_DM_CMD_RESET_RX, MSM_BOOT_UART_DM_CR(base));
	writel(MSM_BOOT_UART_DM_CMD_RESET_TX, MSM_BOOT_UART_DM_CR(base));
	writel(MSM_BOOT_UART_DM_CMD_RESET_ERR_STAT, MSM_BOOT_UART_DM_CR(base));
	writel(MSM_BOOT_UART_DM_CMD_RES_TX_ERR, MSM_BOOT_UART_DM_CR(base));
	writel(MSM_BOOT_UART_DM_CMD_RES_STALE_INT, MSM_BOOT_UART_DM_CR(base));

	return MSM_BOOT_UART_DM_E_SUCCESS;
}

/*
 * msm_boot_uart_dm_init - initilaizes UART controller
 * @uart_dm_base: UART controller base address
 */
static unsigned int msm_boot_uart_dm_init(unsigned int  uart_dm_base)
{
	/* Configure UART mode registers MR1 and MR2 */
	/* Hardware flow control isn't supported */
	writel(0x0, MSM_BOOT_UART_DM_MR1(uart_dm_base));

	/* 8-N-1 configuration: 8 data bits - No parity - 1 stop bit */
	writel(MSM_BOOT_UART_DM_8_N_1_MODE, MSM_BOOT_UART_DM_MR2(uart_dm_base));

	/* Configure Interrupt Mask register IMR */
	writel(MSM_BOOT_UART_DM_IMR_ENABLED, MSM_BOOT_UART_DM_IMR(uart_dm_base));

	/*
	 * Configure Tx and Rx watermarks configuration registers
	 * TX watermark value is set to 0 - interrupt is generated when
	 * FIFO level is less than or equal to 0
	 */
	writel(MSM_BOOT_UART_DM_TFW_VALUE, MSM_BOOT_UART_DM_TFWR(uart_dm_base));

	/* RX watermark value */
	writel(MSM_BOOT_UART_DM_RFW_VALUE, MSM_BOOT_UART_DM_RFWR(uart_dm_base));

	/* Configure Interrupt Programming Register */
	/* Set initial Stale timeout value */
	writel(MSM_BOOT_UART_DM_STALE_TIMEOUT_LSB,
		MSM_BOOT_UART_DM_IPR(uart_dm_base));

	/* Configure IRDA if required */
	/* Disabling IRDA mode */
	writel(0x0, MSM_BOOT_UART_DM_IRDA(uart_dm_base));

	/* Configure hunt character value in HCR register */
	/* Keep it in reset state */
	writel(0x0, MSM_BOOT_UART_DM_HCR(uart_dm_base));

	/*
	 * Configure Rx FIFO base address
	 * Both TX/RX shares same SRAM and default is half-n-half.
	 * Sticking with default value now.
	 * As such RAM size is (2^RAM_ADDR_WIDTH, 32-bit entries).
	 * We have found RAM_ADDR_WIDTH = 0x7f
	 */

	/* Issue soft reset command */
	msm_boot_uart_dm_reset(uart_dm_base);

	/* Enable/Disable Rx/Tx DM interfaces */
	/* Data Mover not currently utilized. */
	writel(0x0, MSM_BOOT_UART_DM_DMEN(uart_dm_base));

	/* Enable transmitter */
	writel(MSM_BOOT_UART_DM_CR_TX_ENABLE,
		MSM_BOOT_UART_DM_CR(uart_dm_base));

	/* Initialize Receive Path */
	msm_boot_uart_dm_init_rx_transfer(uart_dm_base);

	return 0;
}

static void configure_uart_dm(unsigned int uart_index,
				uart_cfg_t *uart_cfg)
{
#ifdef CONFIG_IPQ806X
	ipq_configure_gpio(uart_cfg->dbg_uart_gpio, NO_OF_DBG_UART_GPIOS);

	uart_clock_config(uart_cfg->base,
			uart_cfg->uart_mnd_value.m_value,
			uart_cfg->uart_mnd_value.n_value,
			uart_cfg->uart_mnd_value.d_value,
			gboard_param->clk_dummy);
	writel(GSBI_PROTOCOL_CODE_I2C_UART <<
			GSBI_CTRL_REG_PROTOCOL_CODE_S,
			GSBI_CTRL_REG(uart_cfg->gsbi_base));
#elif defined CONFIG_IPQ40XX
	qca_configure_gpio(uart_cfg->dbg_uart_gpio, NO_OF_DBG_UART_GPIOS);
	/* Enable Clocks for UART2. UART1 clocks are initialized
	   in Boot */
	if (uart_index == 1) {
		writel(1, GCC_BLSP1_UART1_APPS_CBCR);
	} else if (uart_index == 2) {
		uart2_clock_config(UART2_M, UART2_N, UART2_2D);
		writel(1, GCC_BLSP1_UART2_APPS_CBCR);
	}
#endif
	writel(UART_DM_CLK_RX_TX_BIT_RATE,
			MSM_BOOT_UART_DM_CSR(uart_cfg->uart_dm_base));
	/* Intialize UART_DM */
	msm_boot_uart_dm_init(uart_cfg->uart_dm_base);
}

/**
 * uart_dm_init - initializes UART
 *
 * Initializes clocks, GPIO and UART controller.
 */
static void uart_dm_init(void)
{
	uart_cfg_t *console_uart_cfg = gboard_param->console_uart_cfg;
	uart_cfg_t *uart_cfg = gboard_param->uart_cfg;

	/* Configure console  UART */
	configure_uart_dm(1, console_uart_cfg);

	/* Configure Additional  UARTs */
	if (uart_cfg)
		configure_uart_dm(2, uart_cfg);
}

/**
 * serial_putc - transmits a character
 * @c: character to transmit
 */
void serial_putc(char c)
{
	uart_cfg_t *uart_tmp_cfg = gboard_param->console_uart_cfg;

        msm_boot_uart_dm_write(&c, 1, uart_tmp_cfg);
}

/**
 * serial_puts - transmits a string of data
 * @s: string to transmit
 */
void serial_puts(const char *s)
{
        while (*s != '\0')
                serial_putc(*s++);
}

/**
 * serial_tstc - checks if data available for reading
 *
 * Returns 1 if data available, 0 otherwise
 */
int serial_tstc(void)
{
	uart_cfg_t *uart_tmp_cfg = gboard_param->console_uart_cfg;
	/* Return if data is already read */
	if (valid_data)
		return 1;

	/* Read data from the FIFO */
	if (msm_boot_uart_dm_read(&word, &valid_data, 0,
		uart_tmp_cfg) != MSM_BOOT_UART_DM_E_SUCCESS)
		return 0;

	return 1;
}

/**
 * serial_getc - reads a character
 *
 * Returns the character read from serial port.
 */
int serial_getc(void)
{
	int byte;

	while (!serial_tstc()) {
		WATCHDOG_RESET();
		/* wait for incoming data */
	}

	byte = (int)word & 0xff;
	word = word >> 8;
	valid_data--;

	return byte;
}

/*
 * serial_setbrg - sets serial baudarate
 */
void serial_setbrg(void)
{
        return;
}

/**
 * serial_init - initializes serial controller
 */
int  serial_init(void)
{
        uart_dm_init();
        return 0;
}

/**
 * do_uartwr - transmits a string of data
 * @s: string to transmit
 */
static int do_uartwr(cmd_tbl_t *cmdtp, int flag,
			int argc, char * const argv[])
{
	char *s = argv[1];
	uart_cfg_t *uart_tmp_cfg = gboard_param->uart_cfg;

	if (argc < 2)
		goto usage;

	if (uart_tmp_cfg) {
		while (*s != '\0')
			msm_boot_uart_dm_write(s++, 1, uart_tmp_cfg);
	} else {
		printf("Second UART not present \n");
	}
	return 0;
usage:
	return CMD_RET_USAGE;
}

static int uart_serial_tstc(uart_cfg_t *uart_tmp_cfg)
{
	/* Return if data is already read */
	if (uart_valid_data)
		return 1;

	/* Read data from the FIFO */
	if (msm_boot_uart_dm_read(&uart_word, &uart_valid_data, 0,
		uart_tmp_cfg) != MSM_BOOT_UART_DM_E_SUCCESS)
		return 0;

	return 1;
}

static int do_uartrd(cmd_tbl_t *cmdtp, int flag,
			int argc, char * const argv[])
{
	int byte;
	uart_cfg_t *uart_tmp_cfg = gboard_param->uart_cfg;

	if (argc > 1)
		goto usage;

	if (uart_tmp_cfg) {
		for (;;) {
			while (!uart_serial_tstc(uart_tmp_cfg)) {
				/* wait for incoming data */
			}
			byte = (int)uart_word & 0xff;
			switch (byte) {
			case 0x03:
				uart_word = uart_word >> 8;
				uart_valid_data--;
				return (-1);
			default:
				serial_putc(byte);
			}
			uart_word = uart_word >> 8;
			uart_valid_data--;
		}
	} else {
		printf("Second UART not present \n");
	}
	return 0;
usage:
	return CMD_RET_USAGE;
}

U_BOOT_CMD(uartwr, 2, 1, do_uartwr,
	   "uartwr to second UART",
	   "uartwr - write strings to uart\n");

U_BOOT_CMD(uartrd, 1, 1, do_uartrd,
	   "uartrd read from second UART",
	   "uartrd - read strings from uart\n");
