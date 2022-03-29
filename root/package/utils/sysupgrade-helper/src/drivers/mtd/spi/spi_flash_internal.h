/*
 * SPI flash internal definitions
 *
 * Copyright (C) 2008 Atmel Corporation
 */

/* Common parameters -- kind of high, but they should only occur when there
 * is a problem (and well your system already is broken), so err on the side
 * of caution in case we're dealing with slower SPI buses and/or processors.
 */
#define SPI_FLASH_PROG_TIMEOUT		(2 * CONFIG_SYS_HZ)
#define SPI_FLASH_PAGE_ERASE_TIMEOUT	(5 * CONFIG_SYS_HZ)
#define SPI_FLASH_SECTOR_ERASE_TIMEOUT	(10 * CONFIG_SYS_HZ)
#define SPI_FLASH_BERASE_TIMEOUT(f)	((f)->berase_timeout * CONFIG_SYS_HZ)
#define FAST_READ_DUMMY_BYTE		1

/* Common commands */
#define CMD_READ_ID			0x9f

#define CMD_READ_ARRAY_SLOW		0x03
#define CMD_READ_ARRAY_FAST		0x0b

/* Spansion new commands for 4-byte address access */
#define CMD_4READ_ARRAY_FAST		0x0c
#define CMD_4PAGE_PROGRAM		0x12

#define CMD_WRITE_STATUS		0x01
#define CMD_PAGE_PROGRAM		0x02
#define CMD_WRITE_DISABLE		0x04
#define CMD_READ_STATUS			0x05
#define CMD_WRITE_ENABLE		0x06

/* Common status */
#define STATUS_WIP			0x01

/* Send a single-byte command to the device and read the response */
int spi_flash_cmd(struct spi_slave *spi, u8 cmd, void *response, size_t len);

/*
 * Send a multi-byte command to the device and read the response. Used
 * for flash array reads, etc.
 */
int spi_flash_cmd_read(struct spi_slave *spi, const u8 *cmd,
		size_t cmd_len, void *data, size_t data_len);

int spi_flash_cmd_read_fast(struct spi_flash *flash, u32 offset,
		size_t len, void *data);

/*
 * Send a multi-byte command to the device followed by (optional)
 * data. Used for programming the flash array, etc.
 */
int spi_flash_cmd_write(struct spi_slave *spi, const u8 *cmd, size_t cmd_len,
		const void *data, size_t data_len);

/*
 * Write the requested data out breaking it up into multiple write
 * commands as needed per the write size.
 */
int spi_flash_cmd_write_multi(struct spi_flash *flash, u32 offset,
		size_t len, const void *buf);

/*
 * Enable writing on the SPI flash.
 */
static inline int spi_flash_cmd_write_enable(struct spi_flash *flash)
{
	return spi_flash_cmd(flash->spi, CMD_WRITE_ENABLE, NULL, 0);
}

/*
 * Disable writing on the SPI flash.
 */
static inline int spi_flash_cmd_write_disable(struct spi_flash *flash)
{
	return spi_flash_cmd(flash->spi, CMD_WRITE_DISABLE, NULL, 0);
}

/*
 * Same as spi_flash_cmd_read() except it also claims/releases the SPI
 * bus. Used as common part of the ->read() operation.
 */
int spi_flash_read_common(struct spi_flash *flash, const u8 *cmd,
		size_t cmd_len, void *data, size_t data_len);

/* Send a command to the device and wait for some bit to clear itself. */
int spi_flash_cmd_poll_bit(struct spi_flash *flash, unsigned long timeout,
			   u8 cmd, u8 poll_bit);

/*
 * Send the read status command to the device and wait for the wip
 * (write-in-progress) bit to clear itself.
 */
int spi_flash_cmd_wait_ready(struct spi_flash *flash, unsigned long timeout);

/* Erase sectors. */
int spi_flash_cmd_erase(struct spi_flash *flash, u8 erase_cmd,
			u32 offset, size_t len);

/* Erase block */
int spi_flash_cmd_erase_block(struct spi_flash *flash, u8 erase_cmd,
			u32 offset, size_t len);

/* Manufacturer-specific probe functions */
struct spi_flash *spi_flash_probe_spansion(struct spi_slave *spi, u8 *idcode);
struct spi_flash *spi_flash_probe_atmel(struct spi_slave *spi, u8 *idcode);
struct spi_flash *spi_flash_probe_eon(struct spi_slave *spi, u8 *idcode);
struct spi_flash *spi_flash_probe_macronix(struct spi_slave *spi, u8 *idcode);
struct spi_flash *spi_flash_probe_sst(struct spi_slave *spi, u8 *idcode);
struct spi_flash *spi_flash_probe_stmicro(struct spi_slave *spi, u8 *idcode);
struct spi_flash *spi_flash_probe_winbond(struct spi_slave *spi, u8 *idcode);
struct spi_flash *spi_fram_probe_ramtron(struct spi_slave *spi, u8 *idcode);
