/*
 * Copyright (c) 2014, 2015 The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <common.h>
#include <pci.h>

#include <asm/sizes.h>
#include <asm/io.h>
#include <asm/arch-ipq40xx/iomap.h>
#include <asm/arch-qcom-common/gpio.h>
#include "ipq40xx_cdp.h"

#define PCI_CFG0_RDWR			0x4
#define PCI_CFG1_RDWR			0x5
#define RD				0
#define WR 				1
#define PCIE20_PLR_IATU_VIEWPORT        0x900
#define PCIE20_PLR_IATU_CTRL1           0x904
#define PCIE20_PLR_IATU_LTAR            0x918
#define MSM_PCIE_DEV_CFG_ADDR   	0x01000000
#define PCIE20_SIZE   			SZ_4K
#define PCIE_AXI_CONF_SIZE   		SZ_1M

static unsigned int local_buses[] = { 0, 0 };
extern board_ipq40xx_params_t *gboard_param;
struct pci_controller pci_hose[PCI_MAX_DEVICES];

void ipq_pcie_config_cfgtype(uint32_t phyaddr)
{
	uint32_t bdf, cfgtype;

	cfgtype = PCI_CFG0_RDWR;
	bdf = MSM_PCIE_DEV_CFG_ADDR;

	writel(0, phyaddr + PCIE20_PLR_IATU_VIEWPORT);

	/* Program Bdf Address */
	writel(bdf, phyaddr + PCIE20_PLR_IATU_LTAR);

	/* Write Config Request Type */
	writel(cfgtype, phyaddr + PCIE20_PLR_IATU_CTRL1);
}

int ipq_pcie_rd_conf_byte(struct  pci_controller *hose, pci_dev_t dev,
				     int offset, u8 *val)
{
	int bus = PCI_BUS (dev);
	uint32_t addr;
	uint32_t word_offset, byte_offset, mask;
	uint32_t rd_val;

	word_offset = offset & ~0x3;
	byte_offset = offset & 0x3;
	mask = (~0 >> (8 * (4 - 1))) << (8 * byte_offset);

	ipq_pcie_config_cfgtype(hose->regions[0].phys_start);
	if ((bus == local_buses[0]) || (bus == local_buses[1])) {
		addr = hose->regions[0].phys_start;
	} else {
		addr = hose->regions[1].phys_start;
	}
	rd_val = readl(addr + word_offset);
	*val = ((rd_val & mask) >> (8 * byte_offset));

	return 0;
}

int ipq_pcie_rd_conf_word(struct  pci_controller *hose, pci_dev_t dev,
				     int offset, u16 *val)
{
	int bus = PCI_BUS (dev);
	uint32_t addr;
	uint32_t word_offset, byte_offset, mask;
	uint32_t rd_val;

	word_offset = offset & ~0x3;
	byte_offset = offset & 0x3;
	mask = (~0 >> (8 * (4 - 2))) << (8 * byte_offset);

	ipq_pcie_config_cfgtype(hose->regions[0].phys_start);
	if ((bus == local_buses[0]) || (bus == local_buses[1])) {
		addr = hose->regions[0].phys_start;
	} else {
		addr = hose->regions[1].phys_start;
	}
	rd_val = readl(addr + word_offset);
	*val = ((rd_val & mask) >> (8 * byte_offset));

	return 0;
}
int ipq_pcie_rd_conf_dword(struct  pci_controller *hose, pci_dev_t dev,
				     int offset, u32 *val)
{
	int bus = PCI_BUS (dev);
	uint32_t addr;
	uint32_t word_offset, byte_offset, mask;
	uint32_t rd_val;

	word_offset = offset & ~0x3;
	byte_offset = offset & 0x3;
	mask = (~0 >> (8 * (4 - 4))) << (8 * byte_offset);

	ipq_pcie_config_cfgtype(hose->regions[0].phys_start);
	if ((bus == local_buses[0]) || (bus == local_buses[1])) {
		addr = hose->regions[0].phys_start;
	} else {
		addr = hose->regions[1].phys_start;
	}
	rd_val = readl(addr + word_offset);
	*val = ((rd_val & mask) >> (8 * byte_offset));

	return 0;
}

int ipq_pcie_wr_conf_byte(struct  pci_controller *hose, pci_dev_t dev,
				     int offset, u8 val)
{
	int bus = PCI_BUS (dev);
	uint32_t addr;
	uint32_t word_offset, byte_offset, mask;
	uint32_t rd_val, wr_val;

	word_offset = offset & ~0x3;
	byte_offset = offset & 0x3;
	mask = (~0 >> (8 * (4 - 1))) << (8 * byte_offset);

	ipq_pcie_config_cfgtype(hose->regions[0].phys_start);
	if ((bus == local_buses[0]) || (bus == local_buses[1])) {
		addr = hose->regions[0].phys_start;
	} else {
		addr = hose->regions[1].phys_start;
	}
	rd_val = readl(addr + word_offset);
	wr_val = (rd_val & ~mask) |((val << (8 * byte_offset)) & mask);
	writel(wr_val, addr + word_offset);

	return 0;
}

int ipq_pcie_wr_conf_word(struct  pci_controller *hose, pci_dev_t dev,
				     int offset, u16 val)
{
	int bus = PCI_BUS (dev);
	uint32_t addr;
	uint32_t word_offset, byte_offset, mask;
	uint32_t rd_val, wr_val;

	word_offset = offset & ~0x3;
	byte_offset = offset & 0x3;
	mask = (~0 >> (8 * (4 - 2))) << (8 * byte_offset);

	ipq_pcie_config_cfgtype(hose->regions[0].phys_start);
	if ((bus == local_buses[0]) || (bus == local_buses[1])) {
		addr = hose->regions[0].phys_start;
	} else {
		addr = hose->regions[1].phys_start;
	}
	rd_val = readl(addr + word_offset);
	wr_val = (rd_val & ~mask) |((val << (8 * byte_offset)) & mask);
	writel(wr_val, addr + word_offset);

	return 0;
}

int ipq_pcie_wr_conf_dword(struct  pci_controller *hose, pci_dev_t dev,
				     int offset, u32 val)
{
	int bus = PCI_BUS (dev);
	uint32_t addr;
	uint32_t word_offset, byte_offset, mask;
	uint32_t rd_val, wr_val;

	word_offset = offset & ~0x3;
	byte_offset = offset & 0x3;
	mask = (~0 >> (8 * (4 - 4))) << (8 * byte_offset);

	ipq_pcie_config_cfgtype(hose->regions[0].phys_start);
	if ((bus == local_buses[0]) || (bus == local_buses[1])) {
		addr = hose->regions[0].phys_start;
	} else {
		addr = hose->regions[1].phys_start;
	}
	rd_val = readl(addr + word_offset);
	wr_val = (rd_val & ~mask) |((val << (8 * byte_offset)) & mask);
	writel(wr_val, addr + word_offset);

	return 0;
}

void pci_init_board (void)
{
	pcie_params_t	*cfg;
	int i, bus = 0;

	board_pci_init();

	for (i = 0; i < PCI_MAX_DEVICES; i++) {
		cfg = &gboard_param->pcie_cfg[i];
		if (cfg->linkup) {
			pci_hose[i].first_busno = bus;
			pci_hose[i].last_busno = 0xff;
			local_buses[0] = pci_hose[i].first_busno;

			/* PCI memory space */
			pci_set_region (pci_hose[i].regions + 0,
					cfg->pcie20,
					cfg->pcie20,
					PCIE20_SIZE, PCI_REGION_MEM);

			/* PCI device confgiuration  space */
			pci_set_region (pci_hose[i].regions + 1,
					cfg->axi_conf,
					cfg->axi_conf,
				(PCIE_AXI_CONF_SIZE - 1), PCI_REGION_MEM);

			pci_hose[i].region_count = 2;
			pci_register_hose (&pci_hose[i]);
			pci_set_ops (&pci_hose[i],
				ipq_pcie_rd_conf_byte,
				ipq_pcie_rd_conf_word,
				ipq_pcie_rd_conf_dword,
				ipq_pcie_wr_conf_byte,
				ipq_pcie_wr_conf_word,
				ipq_pcie_wr_conf_dword);

			pci_hose[i].last_busno = pci_hose[i].first_busno + 1;
			bus = pci_hose[i].last_busno + 1;
		}
	}
}
