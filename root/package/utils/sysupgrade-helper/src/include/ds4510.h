/*
 * Copyright 2008 Extreme Engineering Solutions, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * Version 2 as published by the Free Software Foundation.
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

#ifndef __DS4510_H_
#define __DS4510_H_

/* General defines */
#define DS4510_NUM_IO				0x04
#define DS4510_IO_MASK				((1 << DS4510_NUM_IO) - 1)
#define DS4510_EEPROM_PAGE_WRITE_DELAY_MS	20

/* EEPROM from 0x00 - 0x39 */
#define DS4510_EEPROM				0x00
#define DS4510_EEPROM_SIZE			0x40
#define DS4510_EEPROM_PAGE_SIZE			0x08
#define DS4510_EEPROM_PAGE_OFFSET(x)	((x) & (DS4510_EEPROM_PAGE_SIZE - 1))

/* SEEPROM from 0xf0 - 0xf7 */
#define DS4510_SEEPROM				0xf0
#define DS4510_SEEPROM_SIZE			0x08

/* Registers overlapping SEEPROM from 0xf0 - 0xf7 */
#define DS4510_PULLUP				0xF0
#define DS4510_PULLUP_DIS			0x00
#define DS4510_PULLUP_EN			0x01
#define DS4510_RSTDELAY				0xF1
#define DS4510_RSTDELAY_MASK			0x03
#define DS4510_RSTDELAY_125			0x00
#define DS4510_RSTDELAY_250			0x01
#define DS4510_RSTDELAY_500			0x02
#define DS4510_RSTDELAY_1000			0x03
#define DS4510_IO3				0xF4
#define DS4510_IO2				0xF5
#define DS4510_IO1				0xF6
#define DS4510_IO0				0xF7

/* Status configuration registers from 0xf8 - 0xf9*/
#define DS4510_IO_STATUS			0xF8
#define DS4510_CFG				0xF9
#define DS4510_CFG_READY			0x80
#define DS4510_CFG_TRIP_POINT			0x40
#define DS4510_CFG_RESET			0x20
#define DS4510_CFG_SEE				0x10
#define DS4510_CFG_SWRST			0x08

/* SRAM from 0xfa - 0xff */
#define DS4510_SRAM				0xfa
#define DS4510_SRAM_SIZE			0x06

int ds4510_mem_write(uint8_t chip, int offset, uint8_t *buf, int count);
int ds4510_mem_read(uint8_t chip, int offset, uint8_t *buf, int count);
int ds4510_see_write(uint8_t chip, uint8_t nv);
int ds4510_rstdelay_write(uint8_t chip, uint8_t delay);
int ds4510_pullup_write(uint8_t chip, uint8_t val);
int ds4510_pullup_read(uint8_t chip);
int ds4510_gpio_write(uint8_t chip, uint8_t val);
int ds4510_gpio_read(uint8_t chip);
int ds4510_gpio_read_val(uint8_t chip);

#endif /* __DS4510_H_ */
