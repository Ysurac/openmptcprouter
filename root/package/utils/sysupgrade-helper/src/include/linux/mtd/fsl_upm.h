/*
 * FSL UPM NAND driver
 *
 * Copyright (C) 2007 MontaVista Software, Inc.
 *                    Anton Vorontsov <avorontsov@ru.mvista.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */

#ifndef __LINUX_MTD_NAND_FSL_UPM
#define __LINUX_MTD_NAND_FSL_UPM

#include <linux/mtd/nand.h>

#define FSL_UPM_WAIT_RUN_PATTERN  0x1
#define FSL_UPM_WAIT_WRITE_BYTE   0x2
#define FSL_UPM_WAIT_WRITE_BUFFER 0x4

struct fsl_upm {
	void __iomem *mdr;
	void __iomem *mxmr;
	void __iomem *mar;
	void __iomem *io_addr;
};

struct fsl_upm_nand {
	struct fsl_upm upm;

	int width;
	int upm_cmd_offset;
	int upm_addr_offset;
	int upm_mar_chip_offset;
	int wait_flags;
	int (*dev_ready)(int chip_nr);
	int chip_delay;
	int chip_offset;
	int chip_nr;

	/* no need to fill */
	int last_ctrl;
};

extern int fsl_upm_nand_init(struct nand_chip *chip, struct fsl_upm_nand *fun);

#endif
