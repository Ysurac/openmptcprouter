/*
 * Copyright 2006 Freescale Semiconductor.
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

#include <common.h>
#include <pci.h>

/* Config the VIA chip */
void mpc85xx_config_via(struct pci_controller *hose,
			pci_dev_t dev, struct pci_config_table *tab)
{
	pci_dev_t bridge;
	unsigned int cmdstat;

	/* Enable USB and IDE functions */
	pci_hose_write_config_byte(hose, dev, 0x48, 0x08);

	pci_hose_read_config_dword(hose, dev, PCI_COMMAND, &cmdstat);
	cmdstat |= PCI_COMMAND_IO | PCI_COMMAND_MEMORY| PCI_COMMAND_MASTER;
	pci_hose_write_config_dword(hose, dev, PCI_COMMAND, cmdstat);
	pci_hose_write_config_byte(hose, dev, PCI_CACHE_LINE_SIZE, 0x08);
	pci_hose_write_config_byte(hose, dev, PCI_LATENCY_TIMER, 0x80);

	/*
	 * Force the backplane P2P bridge to have a window
	 * open from 0x00000000-0x00001fff in PCI I/O space.
	 * This allows legacy I/O (i8259, etc) on the VIA
	 * southbridge to be accessed.
	 */
	bridge = PCI_BDF(0,BRIDGE_ID,0);
	pci_hose_write_config_byte(hose, bridge, PCI_IO_BASE, 0);
	pci_hose_write_config_word(hose, bridge, PCI_IO_BASE_UPPER16, 0);
	pci_hose_write_config_byte(hose, bridge, PCI_IO_LIMIT, 0x10);
	pci_hose_write_config_word(hose, bridge, PCI_IO_LIMIT_UPPER16, 0);
}

/* Function 1, IDE */
void mpc85xx_config_via_usbide(struct pci_controller *hose,
			       pci_dev_t dev, struct pci_config_table *tab)
{
	pciauto_config_device(hose, dev);
	/*
	 * Since the P2P window was forced to cover the fixed
	 * legacy I/O addresses, it is necessary to manually
	 * place the base addresses for the IDE and USB functions
	 * within this window.
	 */
	pci_hose_write_config_dword(hose, dev, PCI_BASE_ADDRESS_0, 0x1ff8);
	pci_hose_write_config_dword(hose, dev, PCI_BASE_ADDRESS_1, 0x1ff4);
	pci_hose_write_config_dword(hose, dev, PCI_BASE_ADDRESS_2, 0x1fe8);
	pci_hose_write_config_dword(hose, dev, PCI_BASE_ADDRESS_3, 0x1fe4);
	pci_hose_write_config_dword(hose, dev, PCI_BASE_ADDRESS_4, 0x1fd0);
}

/* Function 2, USB ports 0-1 */
void mpc85xx_config_via_usb(struct pci_controller *hose,
			    pci_dev_t dev, struct pci_config_table *tab)
{
	pciauto_config_device(hose, dev);

	pci_hose_write_config_dword(hose, dev, PCI_BASE_ADDRESS_4, 0x1fa0);
}

/* Function 3, USB ports 2-3 */
void mpc85xx_config_via_usb2(struct pci_controller *hose,
			     pci_dev_t dev, struct pci_config_table *tab)
{
	pciauto_config_device(hose, dev);

	pci_hose_write_config_dword(hose, dev, PCI_BASE_ADDRESS_4, 0x1f80);
}

/* Function 5, Power Management */
void mpc85xx_config_via_power(struct pci_controller *hose,
			      pci_dev_t dev, struct pci_config_table *tab)
{
	pciauto_config_device(hose, dev);

	pci_hose_write_config_dword(hose, dev, PCI_BASE_ADDRESS_0, 0x1e00);
	pci_hose_write_config_dword(hose, dev, PCI_BASE_ADDRESS_1, 0x1dfc);
	pci_hose_write_config_dword(hose, dev, PCI_BASE_ADDRESS_2, 0x1df8);
}

/* Function 6, AC97 Interface */
void mpc85xx_config_via_ac97(struct pci_controller *hose,
			     pci_dev_t dev, struct pci_config_table *tab)
{
	pciauto_config_device(hose, dev);

	pci_hose_write_config_dword(hose, dev, PCI_BASE_ADDRESS_0, 0x1c00);
}
