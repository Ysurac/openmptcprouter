/*
 * Copyright (c) 2015, The Linux Foundation. All rights reserved.

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef ___QCA_COMMON_H_
#define ___QCA_COMMON_H_


typedef struct {
	uint base;
	uint clk_mode;
	struct mmc *mmc;
	int dev_num;
} qca_mmc;

int qca_mmc_init(bd_t *, qca_mmc *);
void board_mmc_deinit(void);
int conf_mdtb_select(void *addr, char *rcmd, int rcmd_size, const char **conf_name_out);

#endif  /*  __QCA_COMMON_H_ */
