/*
 * Copyright (C) 2004-2007 Freescale Semiconductor, Inc.
 * TsiChung Liew (Tsi-Chung.Liew@freescale.com)
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * PCI Configuration space access support
 */
#include <common.h>
#include <pci.h>
#include <asm/io.h>
#include <asm/immap.h>

#if defined(CONFIG_PCI)
/* System RAM mapped over PCI */
#define CONFIG_SYS_PCI_SYS_MEM_BUS	CONFIG_SYS_SDRAM_BASE
#define CONFIG_SYS_PCI_SYS_MEM_PHYS	CONFIG_SYS_SDRAM_BASE
#define CONFIG_SYS_PCI_SYS_MEM_SIZE	(1024 * 1024 * 1024)

#define cfg_read(val, addr, type, op)		*val = op((type)(addr));
#define cfg_write(val, addr, type, op)		op((type *)(addr), (val));

#define PCI_OP(rw, size, type, op, mask)				\
int pci_##rw##_cfg_##size(struct pci_controller *hose,			\
	pci_dev_t dev, int offset, type val)				\
{									\
	u32 addr = 0;							\
	u16 cfg_type = 0;						\
	addr = ((offset & 0xfc) | cfg_type | (dev)  | 0x80000000);	\
	out_be32(hose->cfg_addr, addr);					\
	cfg_##rw(val, hose->cfg_data + (offset & mask), type, op);	\
	__asm__ __volatile__("nop");					\
	__asm__ __volatile__("nop");					\
	out_be32(hose->cfg_addr, addr & 0x7fffffff);			\
	return 0;							\
}

PCI_OP(read, byte, u8 *, in_8, 3)
PCI_OP(read, word, u16 *, in_le16, 2)
PCI_OP(write, byte, u8, out_8, 3)
PCI_OP(write, word, u16, out_le16, 2)
PCI_OP(write, dword, u32, out_le32, 0)

int pci_read_cfg_dword(struct pci_controller *hose, pci_dev_t dev,
		       int offset, u32 * val)
{
	u32 addr;
	u32 tmpv;
	u32 mask = 2;		/* word access */
	/* Read lower 16 bits */
	addr = ((offset & 0xfc) | (dev) | 0x80000000);
	out_be32(hose->cfg_addr, addr);
	*val = (u32) in_le16((u16 *) (hose->cfg_data + (offset & mask)));
	__asm__ __volatile__("nop");
	out_be32(hose->cfg_addr, addr & 0x7fffffff);

	/* Read upper 16 bits */
	offset += 2;
	addr = ((offset & 0xfc) | 1 | (dev) | 0x80000000);
	out_be32(hose->cfg_addr, addr);
	tmpv = (u32) in_le16((u16 *) (hose->cfg_data + (offset & mask)));
	__asm__ __volatile__("nop");
	out_be32(hose->cfg_addr, addr & 0x7fffffff);

	/* combine results into dword value */
	*val = (tmpv << 16) | *val;

	return 0;
}

void pci_mcf547x_8x_init(struct pci_controller *hose)
{
	volatile pci_t *pci = (volatile pci_t *) MMAP_PCI;
	volatile gpio_t *gpio = (gpio_t *) MMAP_GPIO;

	/* Port configuration */
	gpio->par_pcibg =
	    GPIO_PAR_PCIBG_PCIBG0(3) | GPIO_PAR_PCIBG_PCIBG1(3) |
	    GPIO_PAR_PCIBG_PCIBG2(3) | GPIO_PAR_PCIBG_PCIBG3(3) |
	    GPIO_PAR_PCIBG_PCIBG4(3);
	gpio->par_pcibr =
	    GPIO_PAR_PCIBR_PCIBR0(3) | GPIO_PAR_PCIBR_PCIBR1(3) |
	    GPIO_PAR_PCIBR_PCIBR2(3) | GPIO_PAR_PCIBR_PCIBR3(3) |
	    GPIO_PAR_PCIBR_PCIBR4(3);

	/* Assert reset bit */
	pci->gscr |= PCI_GSCR_PR;

	pci->tcr1 = PCI_TCR1_P;

	/* Initiator windows */
	pci->iw0btar = CONFIG_SYS_PCI_MEM_PHYS | (CONFIG_SYS_PCI_MEM_PHYS >> 16);
	pci->iw1btar = CONFIG_SYS_PCI_IO_PHYS | (CONFIG_SYS_PCI_IO_PHYS >> 16);
	pci->iw2btar = CONFIG_SYS_PCI_CFG_PHYS | (CONFIG_SYS_PCI_CFG_PHYS >> 16);

	pci->iwcr =
	    PCI_IWCR_W0C_EN | PCI_IWCR_W1C_EN | PCI_IWCR_W1C_IO |
	    PCI_IWCR_W2C_EN | PCI_IWCR_W2C_IO;

	pci->icr = 0;

	/* Enable bus master and mem access */
	pci->scr = PCI_SCR_B | PCI_SCR_M;

	/* Cache line size and master latency */
	pci->cr1 = PCI_CR1_CLS(8) | PCI_CR1_LTMR(0xF8);
	pci->cr2 = 0;

#ifdef CONFIG_SYS_PCI_BAR0
	pci->bar0 = PCI_BAR_BAR0(CONFIG_SYS_PCI_BAR0);
	pci->tbatr0a = CONFIG_SYS_PCI_TBATR0 | PCI_TBATR_EN;
#endif
#ifdef CONFIG_SYS_PCI_BAR1
	pci->bar1 = PCI_BAR_BAR1(CONFIG_SYS_PCI_BAR1);
	pci->tbatr1a = CONFIG_SYS_PCI_TBATR1 | PCI_TBATR_EN;
#endif

	/* Deassert reset bit */
	pci->gscr &= ~PCI_GSCR_PR;
	udelay(1000);

	/* Enable PCI bus master support */
	hose->first_busno = 0;
	hose->last_busno = 0xff;

	pci_set_region(hose->regions + 0, CONFIG_SYS_PCI_MEM_BUS, CONFIG_SYS_PCI_MEM_PHYS,
		       CONFIG_SYS_PCI_MEM_SIZE, PCI_REGION_MEM);

	pci_set_region(hose->regions + 1, CONFIG_SYS_PCI_IO_BUS, CONFIG_SYS_PCI_IO_PHYS,
		       CONFIG_SYS_PCI_IO_SIZE, PCI_REGION_IO);

	pci_set_region(hose->regions + 2, CONFIG_SYS_PCI_SYS_MEM_BUS,
		       CONFIG_SYS_PCI_SYS_MEM_PHYS, CONFIG_SYS_PCI_SYS_MEM_SIZE,
		       PCI_REGION_MEM | PCI_REGION_SYS_MEMORY);

	hose->region_count = 3;

	hose->cfg_addr = &(pci->car);
	hose->cfg_data = (volatile unsigned char *)CONFIG_SYS_PCI_CFG_BUS;

	pci_set_ops(hose, pci_read_cfg_byte, pci_read_cfg_word,
		    pci_read_cfg_dword, pci_write_cfg_byte, pci_write_cfg_word,
		    pci_write_cfg_dword);

	/* Hose scan */
	pci_register_hose(hose);
	hose->last_busno = pci_hose_scan(hose);
}
#endif				/* CONFIG_PCI */
