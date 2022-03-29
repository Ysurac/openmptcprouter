/*
 * Driver for the TWSI (i2c) controller found on the Marvell
 * orion5x and kirkwood SoC families.
 *
 * Author: Albert Aribaud <albert.u.boot@aribaud.net>
 * Copyright (c) 2010 Albert Aribaud.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <common.h>
#include <i2c.h>
#include <asm/errno.h>
#include <asm/io.h>

/*
 * include a file that will provide CONFIG_I2C_MVTWSI_BASE
 * and possibly other settings
 */

#if defined(CONFIG_ORION5X)
#include <asm/arch/orion5x.h>
#elif defined(CONFIG_KIRKWOOD)
#include <asm/arch/kirkwood.h>
#else
#error Driver mvtwsi not supported by SoC or board
#endif

/*
 * TWSI register structure
 */

struct  mvtwsi_registers {
	u32 slave_address;
	u32 data;
	u32 control;
	union {
		u32 status;	/* when reading */
		u32 baudrate;	/* when writing */
	};
	u32 xtnd_slave_addr;
	u32 reserved[2];
	u32 soft_reset;
};

/*
 * Control register fields
 */

#define	MVTWSI_CONTROL_ACK	0x00000004
#define	MVTWSI_CONTROL_IFLG	0x00000008
#define	MVTWSI_CONTROL_STOP	0x00000010
#define	MVTWSI_CONTROL_START	0x00000020
#define	MVTWSI_CONTROL_TWSIEN	0x00000040
#define	MVTWSI_CONTROL_INTEN	0x00000080

/*
 * Status register values -- only those expected in normal master
 * operation on non-10-bit-address devices; whatever status we don't
 * expect in nominal conditions (bus errors, arbitration losses,
 * missing ACKs...) we just pass back to the caller as an error
 * code.
 */

#define	MVTWSI_STATUS_START		0x08
#define	MVTWSI_STATUS_REPEATED_START	0x10
#define	MVTWSI_STATUS_ADDR_W_ACK	0x18
#define	MVTWSI_STATUS_DATA_W_ACK	0x28
#define	MVTWSI_STATUS_ADDR_R_ACK	0x40
#define	MVTWSI_STATUS_ADDR_R_NAK	0x48
#define	MVTWSI_STATUS_DATA_R_ACK	0x50
#define	MVTWSI_STATUS_DATA_R_NAK	0x58
#define	MVTWSI_STATUS_IDLE		0xF8

/*
 * The single instance of the controller we'll be dealing with
 */

static struct  mvtwsi_registers *twsi =
	(struct  mvtwsi_registers *) CONFIG_I2C_MVTWSI_BASE;

/*
 * Returned statuses are 0 for success and nonzero otherwise.
 * Currently, cmd_i2c and cmd_eeprom do not interpret an error status.
 * Thus to ease debugging, the return status contains some debug info:
 * - bits 31..24 are error class: 1 is timeout, 2 is 'status mismatch'.
 * - bits 23..16 are the last value of the control register.
 * - bits 15..8 are the last value of the status register.
 * - bits 7..0 are the expected value of the status register.
 */

#define MVTWSI_ERROR_WRONG_STATUS	0x01
#define MVTWSI_ERROR_TIMEOUT		0x02

#define MVTWSI_ERROR(ec, lc, ls, es) (((ec << 24) & 0xFF000000) | \
	((lc << 16) & 0x00FF0000) | ((ls<<8) & 0x0000FF00) | (es & 0xFF))

/*
 * Wait for IFLG to raise, or return 'timeout'; then if status is as expected,
 * return 0 (ok) or return 'wrong status'.
 */
static int twsi_wait(int expected_status)
{
	int control, status;
	int timeout = 1000;

	do {
		control = readl(&twsi->control);
		if (control & MVTWSI_CONTROL_IFLG) {
			status = readl(&twsi->status);
			if (status == expected_status)
				return 0;
			else
				return MVTWSI_ERROR(
					MVTWSI_ERROR_WRONG_STATUS,
					control, status, expected_status);
		}
		udelay(10); /* one clock cycle at 100 kHz */
	} while (timeout--);
	status = readl(&twsi->status);
	return MVTWSI_ERROR(
		MVTWSI_ERROR_TIMEOUT, control, status, expected_status);
}

/*
 * These flags are ORed to any write to the control register
 * They allow global setting of TWSIEN and ACK.
 * By default none are set.
 * twsi_start() sets TWSIEN (in case the controller was disabled)
 * twsi_recv() sets ACK or resets it depending on expected status.
 */
static u8 twsi_control_flags = MVTWSI_CONTROL_TWSIEN;

/*
 * Assert the START condition, either in a single I2C transaction
 * or inside back-to-back ones (repeated starts).
 */
static int twsi_start(int expected_status)
{
	/* globally set TWSIEN in case it was not */
	twsi_control_flags |= MVTWSI_CONTROL_TWSIEN;
	/* assert START */
	writel(twsi_control_flags | MVTWSI_CONTROL_START, &twsi->control);
	/* wait for controller to process START */
	return twsi_wait(expected_status);
}

/*
 * Send a byte (i2c address or data).
 */
static int twsi_send(u8 byte, int expected_status)
{
	/* put byte in data register for sending */
	writel(byte, &twsi->data);
	/* clear any pending interrupt -- that'll cause sending */
	writel(twsi_control_flags, &twsi->control);
	/* wait for controller to receive byte and check ACK */
	return twsi_wait(expected_status);
}

/*
 * Receive a byte.
 * Global mvtwsi_control_flags variable says if we should ack or nak.
 */
static int twsi_recv(u8 *byte)
{
	int expected_status, status;

	/* compute expected status based on ACK bit in global control flags */
	if (twsi_control_flags & MVTWSI_CONTROL_ACK)
		expected_status = MVTWSI_STATUS_DATA_R_ACK;
	else
		expected_status = MVTWSI_STATUS_DATA_R_NAK;
	/* acknowledge *previous state* and launch receive */
	writel(twsi_control_flags, &twsi->control);
	/* wait for controller to receive byte and assert ACK or NAK */
	status = twsi_wait(expected_status);
	/* if we did receive expected byte then store it */
	if (status == 0)
		*byte = readl(&twsi->data);
	/* return status */
	return status;
}

/*
 * Assert the STOP condition.
 * This is also used to force the bus back in idle (SDA=SCL=1).
 */
static int twsi_stop(int status)
{
	int control, stop_status;
	int timeout = 1000;

	/* assert STOP */
	control = MVTWSI_CONTROL_TWSIEN | MVTWSI_CONTROL_STOP;
	writel(control, &twsi->control);
	/* wait for IDLE; IFLG won't rise so twsi_wait() is no use. */
	do {
		stop_status = readl(&twsi->status);
		if (stop_status == MVTWSI_STATUS_IDLE)
			break;
		udelay(10); /* one clock cycle at 100 kHz */
	} while (timeout--);
	control = readl(&twsi->control);
	if (stop_status != MVTWSI_STATUS_IDLE)
		if (status == 0)
			status = MVTWSI_ERROR(
				MVTWSI_ERROR_TIMEOUT,
				control, status, MVTWSI_STATUS_IDLE);
	return status;
}

/*
 * Ugly formula to convert m and n values to a frequency comes from
 * TWSI specifications
 */

#define TWSI_FREQUENCY(m, n) \
	((u8) (CONFIG_SYS_TCLK / (10 * (m + 1) * 2 * (1 << n))))

/*
 * These are required to be reprogrammed before enabling the controller
 * because a reset loses them.
 * Default values come from the spec, but a twsi_reset will change them.
 * twsi_slave_address left uninitialized lest checkpatch.pl complains.
 */

/* Baudrate generator: m (bits 7..4) =4, n (bits 3..0) =4 */
static u8 twsi_baud_rate = 0x44; /* baudrate at controller reset */
/* Default frequency corresponding to default m=4, n=4 */
static u8 twsi_actual_speed = TWSI_FREQUENCY(4, 4);
/* Default slave address is 0 (so is an uninitialized static) */
static u8 twsi_slave_address;

/*
 * Reset controller.
 * Called at end of i2c_init unsuccessful i2c transactions.
 * Controller reset also resets the baud rate and slave address, so
 * re-establish them.
 */
static void twsi_reset(void)
{
	/* ensure controller will be enabled by any twsi*() function */
	twsi_control_flags = MVTWSI_CONTROL_TWSIEN;
	/* reset controller */
	writel(0, &twsi->soft_reset);
	/* wait 2 ms -- this is what the Marvell LSP does */
	udelay(20000);
	/* set baud rate */
	writel(twsi_baud_rate, &twsi->baudrate);
	/* set slave address even though we don't use it */
	writel(twsi_slave_address, &twsi->slave_address);
	writel(0, &twsi->xtnd_slave_addr);
	/* assert STOP but don't care for the result */
	(void) twsi_stop(0);
}

/*
 * I2C init called by cmd_i2c when doing 'i2c reset'.
 * Sets baud to the highest possible value not exceeding requested one.
 */
void i2c_init(int requested_speed, int slaveadd)
{
	int	tmp_speed, highest_speed, n, m;
	int	baud = 0x44; /* baudrate at controller reset */

	/* use actual speed to collect progressively higher values */
	highest_speed = 0;
	/* compute m, n setting for highest speed not above requested speed */
	for (n = 0; n < 8; n++) {
		for (m = 0; m < 16; m++) {
			tmp_speed = TWSI_FREQUENCY(m, n);
			if ((tmp_speed <= requested_speed)
			 && (tmp_speed > highest_speed)) {
				highest_speed = tmp_speed;
				baud = (m << 3) | n;
			}
		}
	}
	/* save baud rate and slave for later calls to twsi_reset */
	twsi_baud_rate = baud;
	twsi_actual_speed = highest_speed;
	twsi_slave_address = slaveadd;
	/* reset controller */
	twsi_reset();
}

/*
 * Begin I2C transaction with expected start status, at given address.
 * Common to i2c_probe, i2c_read and i2c_write.
 * Expected address status will derive from direction bit (bit 0) in addr.
 */
static int i2c_begin(int expected_start_status, u8 addr)
{
	int status, expected_addr_status;

	/* compute expected address status from direction bit in addr */
	if (addr & 1) /* reading */
		expected_addr_status = MVTWSI_STATUS_ADDR_R_ACK;
	else /* writing */
		expected_addr_status = MVTWSI_STATUS_ADDR_W_ACK;
	/* assert START */
	status = twsi_start(expected_start_status);
	/* send out the address if the start went well */
	if (status == 0)
		status = twsi_send(addr, expected_addr_status);
	/* return ok or status of first failure to caller */
	return status;
}

/*
 * I2C probe called by cmd_i2c when doing 'i2c probe'.
 * Begin read, nak data byte, end.
 */
int i2c_probe(uchar chip)
{
	u8 dummy_byte;
	int status;

	/* begin i2c read */
	status = i2c_begin(MVTWSI_STATUS_START, (chip << 1) | 1);
	/* dummy read was accepted: receive byte but NAK it. */
	if (status == 0)
		status = twsi_recv(&dummy_byte);
	/* Stop transaction */
	twsi_stop(0);
	/* return 0 or status of first failure */
	return status;
}

/*
 * I2C read called by cmd_i2c when doing 'i2c read' and by cmd_eeprom.c
 * Begin write, send address byte(s), begin read, receive data bytes, end.
 *
 * NOTE: some EEPROMS want a stop right before the second start, while
 * some will choke if it is there. Deciding which we should do is eeprom
 * stuff, not i2c, but at the moment the APIs won't let us put it in
 * cmd_eeprom, so we have to choose here, and for the moment that'll be
 * a repeated start without a preceding stop.
 */
int i2c_read(u8 dev, uint addr, int alen, u8 *data, int length)
{
	int status;

	/* begin i2c write to send the address bytes */
	status = i2c_begin(MVTWSI_STATUS_START, (dev << 1));
	/* send addr bytes */
	while ((status == 0) && alen--)
		status = twsi_send(addr >> (8*alen),
			MVTWSI_STATUS_DATA_W_ACK);
	/* begin i2c read to receive eeprom data bytes */
	if (status == 0)
		status = i2c_begin(
			MVTWSI_STATUS_REPEATED_START, (dev << 1) | 1);
	/* prepare ACK if at least one byte must be received */
	if (length > 0)
		twsi_control_flags |= MVTWSI_CONTROL_ACK;
	/* now receive actual bytes */
	while ((status == 0) && length--) {
		/* reset NAK if we if no more to read now */
		if (length == 0)
			twsi_control_flags &= ~MVTWSI_CONTROL_ACK;
		/* read current byte */
		status = twsi_recv(data++);
	}
	/* Stop transaction */
	status = twsi_stop(status);
	/* return 0 or status of first failure */
	return status;
}

/*
 * I2C write called by cmd_i2c when doing 'i2c write' and by cmd_eeprom.c
 * Begin write, send address byte(s), send data bytes, end.
 */
int i2c_write(u8 dev, uint addr, int alen, u8 *data, int length)
{
	int status;

	/* begin i2c write to send the eeprom adress bytes then data bytes */
	status = i2c_begin(MVTWSI_STATUS_START, (dev << 1));
	/* send addr bytes */
	while ((status == 0) && alen--)
		status = twsi_send(addr >> (8*alen),
			MVTWSI_STATUS_DATA_W_ACK);
	/* send data bytes */
	while ((status == 0) && (length-- > 0))
		status = twsi_send(*(data++), MVTWSI_STATUS_DATA_W_ACK);
	/* Stop transaction */
	status = twsi_stop(status);
	/* return 0 or status of first failure */
	return status;
}

/*
 * Bus set routine: we only support bus 0.
 */
int i2c_set_bus_num(unsigned int bus)
{
	if (bus > 0) {
		return -1;
	}
	return 0;
}

/*
 * Bus get routine: hard-return bus 0.
 */
unsigned int i2c_get_bus_num(void)
{
	return 0;
}
