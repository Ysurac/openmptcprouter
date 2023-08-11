/*
 *
 * Copyright (c) 2015, 2016 The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _IPQ40XX_BOARD_PARAM_H_
#define _IPQ40XX_BOARD_PARAM_H_

#include <asm/arch-ipq40xx/iomap.h>
#include <asm/arch-qcom-common/gpio.h>
#include <asm/sizes.h>
#include "ipq40xx_cdp.h"

gpio_func_data_t mmc_ap_dk07[] = {
	{
		.gpio = 23,
		.func = 1,	/* sdio0 */
		.pull = GPIO_PULL_UP,
		.drvstr = DRV_TYPE_C,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 24,
		.func = 1,	 /* sdio1 */
		.pull = GPIO_PULL_UP,
		.drvstr = DRV_TYPE_C,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 25,
		.func = 1,	 /* sdio2 */
		.pull = GPIO_PULL_UP,
		.drvstr = DRV_TYPE_C,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 26,
		.func = 1,	 /* sdio3 */
		.pull = GPIO_PULL_UP,
		.drvstr = DRV_TYPE_C,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 27,
		.func = 1,	 /* sdio_clk */
		.pull = GPIO_PULL_UP,
		.drvstr = DRV_TYPE_A,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 28,
		.func = 1,	 /* sdio_cmd */
		.pull = GPIO_PULL_UP,
		.drvstr = DRV_TYPE_C,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
};

gpio_func_data_t mmc_ap_dk04[] = {
	{
		.gpio = 23,
		.func = 1,
		.pull = GPIO_PULL_UP,
		.drvstr = DRV_TYPE_C,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 24,
		.func = 1,
		.pull = GPIO_PULL_UP,
		.drvstr = DRV_TYPE_C,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 25,
		.func = 1,
		.pull = GPIO_PULL_UP,
		.drvstr = DRV_TYPE_C,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 26,
		.func = 1,
		.pull = GPIO_PULL_UP,
		.drvstr = DRV_TYPE_C,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 27,
		.func = 1,
		.pull = GPIO_PULL_UP,
		.drvstr = DRV_TYPE_A,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 28,
		.func = 1,
		.pull = GPIO_PULL_UP,
		.drvstr = DRV_TYPE_C,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 29,
		.func = 1,
		.pull = GPIO_PULL_UP,
		.drvstr = DRV_TYPE_C,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 30,
		.func = 1,
		.pull = GPIO_PULL_UP,
		.drvstr = DRV_TYPE_C,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 31,
		.func = 1,
		.pull = GPIO_PULL_UP,
		.drvstr = DRV_TYPE_C,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 32,
		.func = 1,
		.pull = GPIO_PULL_UP,
		.drvstr = DRV_TYPE_C,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
};


gpio_func_data_t spi_nor_bga[] = {
	{
		.gpio = 12,
		.func = 1,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 13,
		.func = 1,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 14,
		.func = 1,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 15,
		.func = 1,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
};

gpio_func_data_t nand_gpio_bga[] = {
	{
		.gpio = 52,
		.func = 1,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 53,
		.func = 1,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 54,
		.func = 1,
		.pull = GPIO_PULL_DOWN,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 55,
		.func = 1,
		.pull = GPIO_PULL_DOWN,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 56,
		.func = 1,
		.pull = GPIO_PULL_DOWN,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 57,
		.func = 1,
		.pull = GPIO_PULL_DOWN,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 58,
		.func = 1,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 59,
		.func = 1,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 60,
		.func = 1,
		.pull = GPIO_PULL_DOWN,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 61,
		.func = 1,
		.pull = GPIO_PULL_DOWN,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 62,
		.func = 1,
		.pull = GPIO_PULL_DOWN,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 63,
		.func = 1,
		.pull = GPIO_PULL_DOWN,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 64,
		.func = 1,
		.pull = GPIO_PULL_DOWN,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 65,
		.func = 1,
		.pull = GPIO_PULL_DOWN,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 66,
		.func = 1,
		.pull = GPIO_PULL_DOWN,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 67,
		.func = 1,
		.pull = GPIO_PULL_DOWN,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 68,
		.func = 1,
		.pull = GPIO_PULL_DOWN,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 69,
		.func = 1,
		.pull = GPIO_PULL_DOWN,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},

};

gpio_func_data_t nand_gpio_dk07[] = {
	{
		.gpio = 52,
		.func = 1,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 53,
		.func = 1,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 54,
		.func = 1,
		.pull = GPIO_PULL_DOWN,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 55,
		.func = 1,
		.pull = GPIO_PULL_DOWN,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 56,
		.func = 1,
		.pull = GPIO_PULL_DOWN,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 57,
		.func = 1,
		.pull = GPIO_PULL_DOWN,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 58,
		.func = 1,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 59,
		.func = 1,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 60,
		.func = 1,
		.pull = GPIO_PULL_DOWN,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 62,
		.func = 1,
		.pull = GPIO_PULL_DOWN,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 63,
		.func = 1,
		.pull = GPIO_PULL_DOWN,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 64,
		.func = 1,
		.pull = GPIO_PULL_DOWN,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 65,
		.func = 1,
		.pull = GPIO_PULL_DOWN,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 66,
		.func = 1,
		.pull = GPIO_PULL_DOWN,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 67,
		.func = 1,
		.pull = GPIO_PULL_DOWN,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 68,
		.func = 1,
		.pull = GPIO_PULL_DOWN,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 69,
		.func = 1,
		.pull = GPIO_PULL_DOWN,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},

};

gpio_func_data_t rgmii_gpio_cfg[] = {
	{
		.gpio = 22,
		.func = 1,	/* RGMMI0 */
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_16MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_DISABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES0
	},
	{
		.gpio = 23,
		.func = 2,	/* RGMII1 */
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_16MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_DISABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES0
	},
	{
		.gpio = 24,
		.func = 2,	/* RGMII2 */
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_16MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_DISABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES0
	},
	{
		.gpio = 25,
		.func = 2,	/* RGMII3 */
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_16MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_DISABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES0
	},
	{
		.gpio = 26,
		.func = 2,	/* RGMII RX */
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_16MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_DISABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES0
	},
	{
		.gpio = 27,
		.func = 2,	/* RGMII_TXC */
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_16MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_DISABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES0
	},
	{
		.gpio = 28,
		.func = 2,	/* RGMII0 */
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_16MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_DISABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES0
	},
	{
		.gpio = 29,
		.func = 2,	/* RGMII1 */
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_16MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_DISABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES0
	},
	{
		.gpio = 30,
		.func = 2,	/* RGMII2 */
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_16MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_DISABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES0
	},
	{
		.gpio = 31,
		.func = 2,	/* RGMII3 */
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_16MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_DISABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES0
	},
	{
		.gpio = 32,
		.func = 2,	/* RGMII RX_C */
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_16MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_DISABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES0
	},
	{
		.gpio = 33,
		.func = 1,	/* RGMII TX */
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_16MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_DISABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES0
	},
};

gpio_func_data_t sw_gpio_bga[] = {
	{
		.gpio = 6,
		.func = 1,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 7,
		.func = 1,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 47,
		.func = 0,
		.pull = GPIO_PULL_DOWN,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_ENABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
};

gpio_func_data_t ap_dk04_1_c2_sw_gpio_bga[] = {
	{
		.gpio = 6,
		.func = 1,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 7,
		.func = 1,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 67,
		.func = 0,
		.pull = GPIO_PULL_DOWN,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_ENABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
};

gpio_func_data_t ap_dk06_1_c1_sw_gpio_bga[] = {
	{
		.gpio = 6,
		.func = 1,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 7,
		.func = 1,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 19,
		.func = 0,
		.pull = GPIO_PULL_DOWN,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_ENABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		/* overriding the default configuration of gpio52 */
		.gpio = 52,
		.func = 0,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
};

gpio_func_data_t ap_dk07_1_c1_sw_gpio_bga[] = {
	{
		.gpio = 6,
		.func = 1,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 7,
		.func = 1,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 41,
		.func = 0,
		.pull = GPIO_PULL_DOWN,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_ENABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		/* overriding the default configuration of gpio52 */
		.gpio = 52,
		.func = 0,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
};

gpio_func_data_t db_dk_2_1_sw_gpio_bga[] = {
	{
		.gpio = 6,
		.func = 1,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 7,
		.func = 1,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
};

gpio_func_data_t sw_gpio_qfn[] = {
	{
		.gpio = 52,
		.func = 2,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 53,
		.func = 2,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 59,
		.func = 0,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_ENABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
};

gpio_func_data_t ap_dk01_1_c2_sw_gpio_qfn[] = {
	{
		.gpio = 52,
		.func = 2,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 53,
		.func = 2,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
	{
		.gpio = 62,
		.func = 0,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_ENABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2
	},
};

gpio_func_data_t uart1_gpio_dk01[] = {
	{
		.gpio = 60,
		.func = 2,
		.pull = GPIO_PULL_DOWN,
		.oe = GPIO_OE_ENABLE
	},
	{
		.gpio = 61,
		.func = 2,
		.pull = GPIO_NO_PULL,
		.oe = GPIO_OE_ENABLE
	},
};

gpio_func_data_t uart1_gpio_dk04[] = {
	{
		.gpio = 16,
		.func = 1,
		.pull = GPIO_PULL_DOWN,
		.oe = GPIO_OE_ENABLE
	},
	{
		.gpio = 17,
		.func = 1,
		.pull = GPIO_NO_PULL,
		.oe = GPIO_OE_ENABLE
	},
};

gpio_func_data_t uart2_gpio[] = {
	{
		.gpio = 8,
		.func = 1,
		.pull = GPIO_NO_PULL,
		.oe = GPIO_OE_ENABLE
	},
	{
		.gpio = 9,
		.func = 1,
		.pull = GPIO_NO_PULL,
		.oe = GPIO_OE_ENABLE
	},
};

#ifdef CONFIG_IPQ40XX_I2C
gpio_func_data_t i2c0_gpio[] = {
	{
		.gpio = 20,
		.func = 1,
		.pull = GPIO_NO_PULL,
		.oe = GPIO_OE_ENABLE
	},
	{
		.gpio = 21,
		.func = 1,
		.pull = GPIO_NO_PULL,
		.oe = GPIO_OE_ENABLE
	},
};
#endif

uart_cfg_t uart1_console_uart_dk01 = {
	.uart_dm_base = UART1_DM_BASE,
	.dbg_uart_gpio = uart1_gpio_dk01,
};

uart_cfg_t uart1_console_uart_dk04 = {
	.uart_dm_base = UART1_DM_BASE,
	.dbg_uart_gpio = uart1_gpio_dk04,
};

uart_cfg_t uart2 = {
	.uart_dm_base = UART2_DM_BASE,
	.dbg_uart_gpio = uart2_gpio,
};

#ifdef CONFIG_IPQ40XX_I2C
i2c_cfg_t i2c0 = {
	.i2c_base = I2C0_BASE,
	.i2c_gpio = i2c0_gpio,
};
#endif

#define ipq40xx_edma_cfg(_b, _pn, _p, ...)		\
{							\
	.base		= IPQ40XX_EDMA_CFG_BASE,	\
	.unit		= _b,				\
	.phy		= PHY_INTERFACE_MODE_##_p,	\
	.phy_addr	= {.count = _pn, {__VA_ARGS__}},\
	.phy_name	= "IPQ MDIO"#_b			\
}

#ifdef CONFIG_IPQ40XX_PCI

#define PCIE_RST_GPIO		38
#define PCIE_WAKE_GPIO		40
#define PCIE_CLKREQ_GPIO	39

#define PCIE20_0_PARF_PHYS	0x80000
#define PCIE20_0_ELBI_PHYS	0x40000f20
#define PCIE20_0_PHYS		0x40000000
#define PCIE20_SIZE             0xf1d
#define PCIE20_0_AXI_BAR_PHYS	0x40300000
#define PCIE20_0_AXI_BAR_SIZE	0xd00000
#define PCIE20_0_AXI_CONF	0x40100000
#define PCIE_AXI_CONF_SIZE	SZ_1M
#define PCIE20_0_RESET		0x0181D010

#define PCIE_RST_CTRL_PIPE_ARES			0x4
#define PCIE_RST_CTRL_PIPE_STICKY_ARES		0x100
#define PCIE_RST_CTRL_PIPE_PHY_AHB_ARES		0x800
#define PCIE_RST_CTRL_AXI_M_ARES		0x1
#define PCIE_RST_CTRL_AXI_M_STICKY_ARES		0x80
#define PCIE_RST_CTRL_AXI_S_ARES		0x2
#define PCIE_RST_CTRL_AHB_ARES			0x400

#define MSM_PCIE_DEV_CFG_ADDR			0x01000000

#define PCIE20_PLR_IATU_VIEWPORT		0x900
#define PCIE20_PLR_IATU_CTRL1			0x904
#define PCIE20_PLR_IATU_CTRL2			0x908
#define PCIE20_PLR_IATU_LBAR			0x90C
#define PCIE20_PLR_IATU_UBAR			0x910
#define PCIE20_PLR_IATU_LAR			0x914
#define PCIE20_PLR_IATU_LTAR			0x918
#define PCIE20_PLR_IATU_UTAR			0x91c

/* PCIE20_PARF_PHYS Registers */
#define PARF_SLV_ADDR_SPACE_SIZE		0x16C
#define SLV_ADDR_SPACE_SZ			0x40000000
#define PCIE_0_PCIE20_PARF_LTSSM		0x1B0
#define LTSSM_EN				(1 << 8)
/* PCIE20_PHYS Registers */
#define PCIE_0_PORT_FORCE_REG			0x708
#define PCIE_0_ACK_F_ASPM_CTRL_REG		0x70C
#define L1_ENTRANCE_LATENCY(x)			(x << 27)
#define L0_ENTRANCE_LATENCY(x)			(x << 24)
#define COMMON_CLK_N_FTS(x)			(x << 16)
#define ACK_N_FTS(x)				(x << 8)

#define PCIE_0_GEN2_CTRL_REG			0x80C
#define FAST_TRAINING_SEQ(x)			(x << 0)
#define NUM_OF_LANES(x)				(x << 8)
#define DIRECT_SPEED_CHANGE			(1 << 17)

#define PCIE_0_TYPE0_STATUS_COMMAND_REG_1	0x004
#define PCI_TYPE0_BUS_MASTER_EN			(1 << 2)

#define PCIE_0_MISC_CONTROL_1_REG		0x8BC
#define DBI_RO_WR_EN				(1 << 0)

#define PCIE_0_LINK_CAPABILITIES_REG		0x07C
#define PCIE_CAP_ASPM_OPT_COMPLIANCE		(1 << 22)
#define PCIE_CAP_LINK_BW_NOT_CAP		(1 << 21)
#define PCIE_CAP_DLL_ACTIVE_REP_CAP		(1 << 20)
#define PCIE_CAP_L1_EXIT_LATENCY(x)		(x << 15)
#define PCIE_CAP_L0S_EXIT_LATENCY(x)		(x << 12)
#define PCIE_CAP_MAX_LINK_WIDTH(x)		(x << 4)
#define PCIE_CAP_MAX_LINK_SPEED(x)		(x << 0)

#define PCIE_0_DEVICE_CONTROL2_DEVICE_STATUS2_REG	0x098
#define PCIE_CAP_CPL_TIMEOUT_DISABLE			(1 << 4)
#define PCIE_0_TYPE0_LINK_CONTROL_LINK_STATUS_REG_1	0x080

gpio_func_data_t pci_0_gpio_data[] = {
	{
		.gpio = PCIE_RST_GPIO,
		.func = 0,
		.out = GPIO_OUT_HIGH,
		.pull = GPIO_PULL_DOWN,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_ENABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2,
	},
	{
		.gpio = PCIE_WAKE_GPIO,
		.func = 0,
		.out = GPIO_OUT_LOW,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2,
	},
	{
		.gpio = PCIE_CLKREQ_GPIO,
		.func = 0,
		.out = GPIO_OUT_HIGH,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_ENABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2,
	},
};

gpio_func_data_t pci_0_dk07_gpio_data[] = {
	{
		.gpio = PCIE_RST_GPIO,
		.func = 0,
		.out = GPIO_OUT_HIGH,
		.pull = GPIO_PULL_DOWN,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_ENABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2,
	},
	{
		.gpio = PCIE_WAKE_GPIO,
		.func = 0,
		.out = GPIO_OUT_LOW,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_2MA,
		.oe = GPIO_OE_DISABLE,
		.gpio_vm = GPIO_VM_ENABLE,
		.gpio_od_en = GPIO_OD_DISABLE,
		.gpio_pu_res = GPIO_PULL_RES2,
	},
};

#define TLMM_GPIO_OUT_1		0x01200004
#define TLMM_GPIO_OE_1		0x01200084

#define pcie_board_cfg(_id)						\
{									\
	.pci_gpio		= pci_##_id##_gpio_data,		\
	.pci_gpio_count		= ARRAY_SIZE(pci_##_id##_gpio_data),	\
	.parf			= PCIE20_##_id##_PARF_PHYS,		\
	.elbi			= PCIE20_##_id##_ELBI_PHYS,		\
	.pcie20			= PCIE20_##_id##_PHYS,			\
	.axi_bar_start		= PCIE20_##_id##_AXI_BAR_PHYS,		\
	.axi_bar_size		= PCIE20_##_id##_AXI_BAR_SIZE,		\
	.axi_conf		= PCIE20_##_id##_AXI_CONF,		\
	.pcie_rst		= PCIE20_##_id##_RESET,			\
}

#define pcie_board_cfg_dk07(_id)                                             \
{                                                                       \
	.pci_gpio		= pci_##_id##_dk07_gpio_data,                \
	.pci_gpio_count		= ARRAY_SIZE(pci_##_id##_dk07_gpio_data),    \
	.parf			= PCIE20_##_id##_PARF_PHYS,             \
	.elbi			= PCIE20_##_id##_ELBI_PHYS,             \
	.pcie20			= PCIE20_##_id##_PHYS,                  \
	.axi_bar_start		= PCIE20_##_id##_AXI_BAR_PHYS,          \
	.axi_bar_size		= PCIE20_##_id##_AXI_BAR_SIZE,          \
	.axi_conf		= PCIE20_##_id##_AXI_CONF,              \
	.pcie_rst		= PCIE20_##_id##_RESET,                 \
}

#define PCIE20_PARF_PHY_CTRL				0x40
#define __mask(a, b)	(((1 << ((a) + 1)) - 1) & ~((1 << (b)) - 1))
#define __set(v, a, b)	(((v) << (b)) & __mask(a, b))
#define PCIE20_PARF_PHY_CTRL_PHY_TX0_TERM_OFFST(x)	_set(x, 20, 16)
#define PCIE20_PARF_PCS_DEEMPH				0x34
#define PCIE20_PARF_PCS_DEEMPH_TX_DEEMPH_GEN1(x)	__set(x, 21, 16)
#define PCIE20_PARF_PCS_DEEMPH_TX_DEEMPH_GEN2_3_5DB(x)	__set(x, 13, 8)
#define PCIE20_PARF_PCS_DEEMPH_TX_DEEMPH_GEN2_6DB(x)	__set(x, 5, 0)
#define PCIE20_PARF_PCS_SWING				0x38
#define PCIE20_PARF_PCS_SWING_TX_SWING_FULL(x)		__set(x, 14, 8)
#define PCIE20_PARF_PCS_SWING_TX_SWING_LOW(x)		__set(x, 6, 0)
#define PCIE20_PARF_PHY_REFCLK 				0x4C
#define PCIE_SFAB_AXI_S5_FCLK_CTL			(MSM_CLK_CTL_BASE + 0x2154)
#define PCIE20_AXI_MSTR_RESP_COMP_CTRL0			0x818
#define PCIE20_AXI_MSTR_RESP_COMP_CTRL1			0x81c

#endif

#define ipq40xx_edma_cfg_invalid()	{ .unit = -1, }
/* Board specific parameter Array */
board_ipq40xx_params_t board_params[] = {
	{
		.machid = MACH_TYPE_IPQ40XX_AP_DK01_1_S1,
		.ddr_size = (128 << 20),
		.mtdids = "nand2=spi0.0",
		.console_uart_cfg = &uart1_console_uart_dk01,
		.sw_gpio = ap_dk01_1_c2_sw_gpio_qfn,
		.sw_gpio_count = ARRAY_SIZE(ap_dk01_1_c2_sw_gpio_qfn),
		.edma_cfg = {
			ipq40xx_edma_cfg(0, 5, PSGMII,
					0, 1, 2, 3, 4)
		},
		.spi_nand_available = 0,
		.nor_nand_available = 0,
		.nor_emmc_available = 0,
		.dtb_config_name = { "config@ap.dk01.1-c3", "config@4",
					"config@ap.dk01.1-c1" },
	},
	{
		.machid = MACH_TYPE_IPQ40XX_AP_DK01_1_C1,
		.ddr_size = (256 << 20),
		.mtdids = "nand2=spi0.0",
		.console_uart_cfg = &uart1_console_uart_dk01,
		.sw_gpio = sw_gpio_qfn,
		.sw_gpio_count = ARRAY_SIZE(sw_gpio_qfn),
		.edma_cfg = {
			ipq40xx_edma_cfg(0, 5, PSGMII,
					0, 1, 2, 3, 4)
		},
		.spi_nand_available = 0,
		.nor_nand_available = 0,
		.nor_emmc_available = 0,
		.dtb_config_name = { "config@4", "config@ap.dk01.1-c1" },
	},
	{
		.machid = MACH_TYPE_IPQ40XX_AP_DK01_1_C2,
		.ddr_size = (256 << 20),
		.mtdids = "nand1=nand1,nand2=spi0.0",
		.console_uart_cfg = &uart1_console_uart_dk01,
		.sw_gpio = ap_dk01_1_c2_sw_gpio_qfn,
		.sw_gpio_count = ARRAY_SIZE(ap_dk01_1_c2_sw_gpio_qfn),
		.edma_cfg = {
			ipq40xx_edma_cfg(0, 5, PSGMII,
					0, 1, 2, 3, 4)
		},
		.spi_nand_available = 1,
		.nor_nand_available = 0,
		.nor_emmc_available = 0,
		.dtb_config_name = { "config@5", "config@ap.dk01.1-c2" },
	},
	{
		.machid = MACH_TYPE_IPQ40XX_AP_DK04_1_C1,
		.ddr_size = (256 << 20),
		.mtdids = "nand0=nand0,nand2=spi0.0",
		.spi_nor_gpio = spi_nor_bga,
		.spi_nor_gpio_count = ARRAY_SIZE(spi_nor_bga),
		.nand_gpio = nand_gpio_bga,
		.nand_gpio_count = ARRAY_SIZE(nand_gpio_bga),
		.sw_gpio = sw_gpio_bga,
		.sw_gpio_count = ARRAY_SIZE(sw_gpio_bga),
		.edma_cfg = {
			ipq40xx_edma_cfg(0, 5, PSGMII,
					0, 1, 2, 3, 4)
		},
		.uart_cfg = &uart2,
		.console_uart_cfg = &uart1_console_uart_dk04,
#ifdef CONFIG_IPQ40XX_I2C
		.i2c_cfg = &i2c0,
#endif
		.mmc_gpio = mmc_ap_dk04,
		.mmc_gpio_count = ARRAY_SIZE(mmc_ap_dk04),
		.spi_nand_available = 0,
		.nor_nand_available = 0,
		.nor_emmc_available = 0,
#ifdef CONFIG_IPQ40XX_PCI
		.pcie_cfg = {
			pcie_board_cfg(0),
		},
#endif
		.dtb_config_name = { "config@1", "config@ap.dk04.1-c1" },
	},
	{
		.machid = MACH_TYPE_IPQ40XX_AP_DK04_1_C4,
		.ddr_size = (256 << 20),
		.mtdids = "nand0=nand0,nand2=spi0.0",
		.spi_nor_gpio = spi_nor_bga,
		.spi_nor_gpio_count = ARRAY_SIZE(spi_nor_bga),
		.nand_gpio = nand_gpio_bga,
		.nand_gpio_count = ARRAY_SIZE(nand_gpio_bga),
		.sw_gpio = sw_gpio_bga,
		.sw_gpio_count = ARRAY_SIZE(sw_gpio_bga),
		.edma_cfg = {
			ipq40xx_edma_cfg(0, 5, PSGMII,
					0, 1, 2, 3, 4)
		},
		.uart_cfg = &uart2,
		.console_uart_cfg = &uart1_console_uart_dk04,
#ifdef CONFIG_IPQ40XX_I2C
		.i2c_cfg = &i2c0,
#endif
		.mmc_gpio = mmc_ap_dk04,
		.mmc_gpio_count = ARRAY_SIZE(mmc_ap_dk04),
		.spi_nand_available = 0,
		.nor_nand_available = 0,
		.nor_emmc_available = 0,
#ifdef CONFIG_IPQ40XX_PCI
		.pcie_cfg = {
			pcie_board_cfg(0),
		},
#endif
		.dtb_config_name = { "config@8", "config@ap.dk04.1-c4" },
	},
	{
		.machid = MACH_TYPE_IPQ40XX_AP_DK04_1_C2,
		.ddr_size = (256 << 20),
		.mtdids = "nand2=spi0.0",
		.uart_cfg = &uart2,
		.console_uart_cfg = &uart1_console_uart_dk04,
#ifdef CONFIG_IPQ40XX_I2C
		.i2c_cfg = &i2c0,
#endif
		.spi_nor_gpio = spi_nor_bga,
		.spi_nor_gpio_count = ARRAY_SIZE(spi_nor_bga),
		.sw_gpio = ap_dk04_1_c2_sw_gpio_bga,
		.sw_gpio_count = ARRAY_SIZE(ap_dk04_1_c2_sw_gpio_bga),
		.edma_cfg = {
			ipq40xx_edma_cfg(0, 5, PSGMII,
					0, 1, 2, 3, 4)
		},
		.mmc_gpio = mmc_ap_dk04,
		.mmc_gpio_count = ARRAY_SIZE(mmc_ap_dk04),
		.spi_nand_available = 0,
		.nor_nand_available = 0,
		.nor_emmc_available = 0,
		.dtb_config_name = { "config@2", "config@ap.dk04.1-c2" },
	},
	{
		.machid = MACH_TYPE_IPQ40XX_AP_DK04_1_C3,
		.ddr_size = (256 << 20),
		.console_uart_cfg = &uart1_console_uart_dk04,
		.mtdids = "nand0=nand0,nand2=spi0.0",
		.spi_nor_gpio = spi_nor_bga,
		.spi_nor_gpio_count = ARRAY_SIZE(spi_nor_bga),
		.nand_gpio = nand_gpio_bga,
		.nand_gpio_count = ARRAY_SIZE(nand_gpio_bga),
		.sw_gpio = sw_gpio_bga,
		.sw_gpio_count = ARRAY_SIZE(sw_gpio_bga),
		.edma_cfg = {
			ipq40xx_edma_cfg(0, 5, PSGMII,
					0, 1, 2, 3, 4)
		},
		.mmc_gpio = mmc_ap_dk04,
		.mmc_gpio_count = ARRAY_SIZE(mmc_ap_dk04),
		.spi_nand_available = 0,
		.nor_nand_available = 0,
		.nor_emmc_available = 1,
		.dtb_config_name = { "config@3", "config@ap.dk04.1-c3" },
	},
	{
		.machid = MACH_TYPE_IPQ40XX_AP_DK04_1_C5,
		.ddr_size = (256 << 20),
		.mtdids = "nand1=nand1,nand2=spi0.0",
		.spi_nor_gpio = spi_nor_bga,
		.spi_nor_gpio_count = ARRAY_SIZE(spi_nor_bga),
		.sw_gpio = sw_gpio_bga,
		.sw_gpio_count = ARRAY_SIZE(sw_gpio_bga),
		.edma_cfg = {
			ipq40xx_edma_cfg(0, 5, PSGMII,
					0, 1, 2, 3, 4)
		},
		.uart_cfg = &uart2,
		.console_uart_cfg = &uart1_console_uart_dk04,
#ifdef CONFIG_IPQ40XX_I2C
		.i2c_cfg = &i2c0,
#endif
		.spi_nand_available = 1,
		.nor_nand_available = 0,
		.nor_emmc_available = 0,
#ifdef CONFIG_IPQ40XX_PCI
		.pcie_cfg = {
			pcie_board_cfg(0),
		},
#endif
		.dtb_config_name = { "config@12", "config@ap.dk04.1-c5" },
	},
	{
		.machid = MACH_TYPE_IPQ40XX_AP_DK05_1_C1,
		.ddr_size = (256 << 20),
		.mtdids = "nand1=nand1,nand2=spi0.0",
		.console_uart_cfg = &uart1_console_uart_dk01,
		.sw_gpio = ap_dk01_1_c2_sw_gpio_qfn,
		.sw_gpio_count = ARRAY_SIZE(ap_dk01_1_c2_sw_gpio_qfn),
		.edma_cfg = {
			ipq40xx_edma_cfg(0, 5, PSGMII,
					0, 1, 2, 3, 4)
		},
		.spi_nand_available = 1,
		.nor_nand_available = 0,
		.nor_emmc_available = 0,
		.dtb_config_name = { "config@11", "config@ap.dk05.1-c1" },
	},
	{
		.machid = MACH_TYPE_IPQ40XX_AP_DK06_1_C1,
		.ddr_size = (256 << 20),
		.console_uart_cfg = &uart1_console_uart_dk04,
		.mtdids = "nand0=nand0,nand2=spi0.0",
		.spi_nor_gpio = spi_nor_bga,
		.spi_nor_gpio_count = ARRAY_SIZE(spi_nor_bga),
		.nand_gpio = nand_gpio_bga,
		.nand_gpio_count = ARRAY_SIZE(nand_gpio_bga),
		.sw_gpio = ap_dk06_1_c1_sw_gpio_bga,
		.sw_gpio_count = ARRAY_SIZE(ap_dk06_1_c1_sw_gpio_bga),
		.edma_cfg = {
			ipq40xx_edma_cfg(0, 5, PSGMII,
			0, 1, 2, 3, 4)
			},
		.mmc_gpio = mmc_ap_dk04,
		.mmc_gpio_count = ARRAY_SIZE(mmc_ap_dk04),
		.spi_nand_available = 0,
		.nor_nand_available = 0,
		.nor_emmc_available = 0,
		.dtb_config_name = { "config@9", "config@ap.dk06.1-c1" },
	},
	{
		.machid = MACH_TYPE_IPQ40XX_AP_DK07_1_C1,
		.ddr_size = (512 << 20),
		.console_uart_cfg = &uart1_console_uart_dk04,
		.mtdids = "nand0=nand0,nand2=spi0.0",
		.spi_nor_gpio = spi_nor_bga,
		.spi_nor_gpio_count = ARRAY_SIZE(spi_nor_bga),
		.nand_gpio = nand_gpio_dk07,
		.nand_gpio_count = ARRAY_SIZE(nand_gpio_dk07),
		.sw_gpio = ap_dk07_1_c1_sw_gpio_bga,
		.sw_gpio_count = ARRAY_SIZE(ap_dk07_1_c1_sw_gpio_bga),
		.edma_cfg = {
			ipq40xx_edma_cfg(0, 5, PSGMII,
			0, 1, 2, 3, 4)
			},
#ifdef CONFIG_IPQ40XX_I2C
		.i2c_cfg = &i2c0,
#endif
		.mmc_gpio = mmc_ap_dk07,
		.mmc_gpio_count = ARRAY_SIZE(mmc_ap_dk07),
		.spi_nand_available = 0,
		.nor_nand_available = 1,
		.nor_emmc_available = 0,
#ifdef CONFIG_IPQ40XX_PCI
		.pcie_cfg = {
			pcie_board_cfg_dk07(0),
		},
#endif
		.dtb_config_name = { "config@10", "config@ap.dk07.1-c1" },
	},
	{
		.machid = MACH_TYPE_IPQ40XX_AP_DK07_1_C2,
		.ddr_size = (512 << 20),
		.console_uart_cfg = &uart1_console_uart_dk04,
		.mtdids = "nand0=nand0",
		.nand_gpio = nand_gpio_dk07,
		.nand_gpio_count = ARRAY_SIZE(nand_gpio_dk07),
		.sw_gpio = ap_dk07_1_c1_sw_gpio_bga,
		.sw_gpio_count = ARRAY_SIZE(ap_dk07_1_c1_sw_gpio_bga),
		.edma_cfg = {
			ipq40xx_edma_cfg(0, 5, PSGMII,
			0, 1, 2, 3, 4)
			},
#ifdef CONFIG_IPQ40XX_I2C
		.i2c_cfg = &i2c0,
#endif
		.mmc_gpio = mmc_ap_dk07,
		.mmc_gpio_count = ARRAY_SIZE(mmc_ap_dk07),
		.spi_nand_available = 0,
		.nor_nand_available = 0,
		.nor_emmc_available = 0,
		.dtb_config_name = { "config@13", "config@ap.dk07.1-c2" },
	},
	{
		.machid = MACH_TYPE_IPQ40XX_AP_DK07_1_C3,
		.ddr_size = (512 << 20),
		.console_uart_cfg = &uart1_console_uart_dk04,
		.mtdids = "nand0=nand0",
		.nand_gpio = nand_gpio_dk07,
		.nand_gpio_count = ARRAY_SIZE(nand_gpio_dk07),
		.sw_gpio = ap_dk07_1_c1_sw_gpio_bga,
		.sw_gpio_count = ARRAY_SIZE(ap_dk07_1_c1_sw_gpio_bga),
		.edma_cfg = {
			ipq40xx_edma_cfg(0, 5, PSGMII,
			0, 1, 2, 3, 4)
			},
#ifdef CONFIG_IPQ40XX_I2C
		.i2c_cfg = &i2c0,
#endif
		.spi_nand_available = 0,
		.nor_nand_available = 0,
		.nor_emmc_available = 0,
#ifdef CONFIG_IPQ40XX_PCI
		.pcie_cfg = {
			pcie_board_cfg_dk07(0),
		},
#endif
		.dtb_config_name = { "config@14", "config@ap.dk07.1-c3" },
	},
	{
		.machid = MACH_TYPE_IPQ40XX_DB_DK01_1_C1,
		.ddr_size = (256 << 20),
		.mtdids = "nand2=spi0.0",
		.console_uart_cfg = &uart1_console_uart_dk01,
		.sw_gpio = sw_gpio_qfn,
		.sw_gpio_count = ARRAY_SIZE(sw_gpio_qfn),
		.edma_cfg = {
			ipq40xx_edma_cfg(0, 5, RGMII,
					0, 1, 2, 3, 4)
		},
		.spi_nand_available = 0,
		.nor_nand_available = 0,
		.nor_emmc_available = 0,
		.dtb_config_name = { "config@6", "config@db.dk01.1-c1" },
	},
	{
		.machid = MACH_TYPE_IPQ40XX_DB_DK02_1_C1,
		.ddr_size = (256 << 20),
		.mtdids = "nand0=nand0,nand2=spi0.0",
		.console_uart_cfg = &uart1_console_uart_dk04,
		.spi_nor_gpio = spi_nor_bga,
		.spi_nor_gpio_count = ARRAY_SIZE(spi_nor_bga),
		.nand_gpio = nand_gpio_bga,
		.nand_gpio_count = ARRAY_SIZE(nand_gpio_bga),
		.sw_gpio = db_dk_2_1_sw_gpio_bga,
		.sw_gpio_count = ARRAY_SIZE(db_dk_2_1_sw_gpio_bga),
		.rgmii_gpio = rgmii_gpio_cfg,
		.rgmii_gpio_count = ARRAY_SIZE(rgmii_gpio_cfg),
		.edma_cfg = {
			ipq40xx_edma_cfg(0, 5, RGMII,
					0, 1, 2, 3, 4)
		},
		.mmc_gpio = mmc_ap_dk04,
		.mmc_gpio_count = ARRAY_SIZE(mmc_ap_dk04),
		.spi_nand_available = 0,
		.nor_nand_available = 0,
		.nor_emmc_available = 0,
		.dtb_config_name = { "config@7", "config@db.dk02.1-c1" },
	},
	{
		.machid = MACH_TYPE_IPQ40XX_TB832,
		.ddr_size = (256 << 20),
		.console_uart_cfg = &uart1_console_uart_dk04,
		.sw_gpio = sw_gpio_bga,
		.sw_gpio_count = ARRAY_SIZE(sw_gpio_bga),
		.edma_cfg = {
			ipq40xx_edma_cfg(0, 5, PSGMII,
					0, 1, 2, 3, 4)
		},
		.spi_nand_available = 0,
		.nor_nand_available = 0,
		.nor_emmc_available = 0,
		.dtb_config_name = { "", "" },
	},
};

#define NUM_IPQ40XX_BOARDS	ARRAY_SIZE(board_params)
#endif /* _IPQ40XX_BOARD_PARAM_H_ */
