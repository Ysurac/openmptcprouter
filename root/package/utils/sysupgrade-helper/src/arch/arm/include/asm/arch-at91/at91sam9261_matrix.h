/*
 * [origin: Linux kernel include/asm-arm/arch-at91/at91sam9261_matrix.h]
 *
 * Copyright (C) 2007 Atmel Corporation.
 *
 * Memory Controllers (MATRIX, EBI) - System peripherals registers.
 * Based on AT91SAM9261 datasheet revision D.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef AT91SAM9261_MATRIX_H
#define AT91SAM9261_MATRIX_H

#ifndef __ASSEMBLY__

struct at91_matrix {
	u32	mcfg;	/* Master Configuration Registers */
	u32	scfg[5];	/* Slave Configuration Registers */
	u32	filler[6];
	u32	ebicsa;		/* EBI Chip Select Assignment Register */
};
#endif /* __ASSEMBLY__ */

#define AT91_MATRIX_ULBT_INFINITE       (0 << 0)
#define AT91_MATRIX_ULBT_SINGLE         (1 << 0)
#define AT91_MATRIX_ULBT_FOUR           (2 << 0)
#define AT91_MATRIX_ULBT_EIGHT          (3 << 0)
#define AT91_MATRIX_ULBT_SIXTEEN        (4 << 0)

#define AT91_MATRIX_DEFMSTR_TYPE_NONE   (0 << 16)
#define AT91_MATRIX_DEFMSTR_TYPE_LAST   (1 << 16)
#define AT91_MATRIX_DEFMSTR_TYPE_FIXED  (2 << 16)
#define AT91_MATRIX_FIXED_DEFMSTR_SHIFT 18
#define AT91_MATRIX_ARBT_ROUND_ROBIN    (0 << 24)
#define AT91_MATRIX_ARBT_FIXED_PRIORITY (1 << 24)

#define AT91_MATRIX_M0PR_SHIFT          0
#define AT91_MATRIX_M1PR_SHIFT          4
#define AT91_MATRIX_M2PR_SHIFT          8
#define AT91_MATRIX_M3PR_SHIFT          12
#define AT91_MATRIX_M4PR_SHIFT          16
#define AT91_MATRIX_M5PR_SHIFT          20

#define AT91_MATRIX_RCB0                (1 << 0)
#define AT91_MATRIX_RCB1                (1 << 1)

#define AT91_MATRIX_CS1A_SDRAMC         (1 << 1)
#define AT91_MATRIX_CS3A_SMC_SMARTMEDIA (1 << 3)
#define AT91_MATRIX_CS4A_SMC_CF1        (1 << 4)
#define AT91_MATRIX_CS5A_SMC_CF2        (1 << 5)
#define AT91_MATRIX_DBPUC               (1 << 8)
#define AT91_MATRIX_VDDIOMSEL_1_8V      (0 << 16)
#define AT91_MATRIX_VDDIOMSEL_3_3V      (1 << 16)

#endif
