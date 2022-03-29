/*
 * (C) Copyright 2002
 * Daniel Engström, Omicron Ceti AB, <daniel@omicron.se>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <pci.h>
#include <asm/io.h>
#include <asm/pci.h>

#undef PCI_ROM_SCAN_VERBOSE

int pci_shadow_rom(pci_dev_t dev, unsigned char *dest)
{
	struct pci_controller *hose;
	int res = -1;
	int i;

	u32 rom_addr;
	u32 addr_reg;
	u32 size;

	u16 vendor;
	u16 device;
	u32 class_code;

	u32 pci_data;

	hose = pci_bus_to_hose(PCI_BUS(dev));

	debug("pci_shadow_rom() asked to shadow device %x to %x\n",
	       dev, (u32)dest);

	pci_read_config_word(dev, PCI_VENDOR_ID, &vendor);
	pci_read_config_word(dev, PCI_DEVICE_ID, &device);
	pci_read_config_dword(dev, PCI_CLASS_REVISION, &class_code);

	class_code &= 0xffffff00;
	class_code >>= 8;

	debug("PCI Header Vendor %04x device %04x class %06x\n",
	       vendor, device, class_code);

	/* Enable the rom addess decoder */
	pci_write_config_dword(dev, PCI_ROM_ADDRESS, (u32)PCI_ROM_ADDRESS_MASK);
	pci_read_config_dword(dev, PCI_ROM_ADDRESS, &addr_reg);

	if (!addr_reg) {
		/* register unimplemented */
		printf("pci_chadow_rom: device do not seem to have a rom\n");
		return -1;
	}

	size = (~(addr_reg&PCI_ROM_ADDRESS_MASK)) + 1;

	debug("ROM is %d bytes\n", size);

	rom_addr = pci_get_rom_window(hose, size);

	debug("ROM mapped at %x\n", rom_addr);

	pci_write_config_dword(dev, PCI_ROM_ADDRESS,
			       pci_phys_to_mem(dev, rom_addr)
			       |PCI_ROM_ADDRESS_ENABLE);


	for (i = rom_addr; i < rom_addr + size; i += 512) {
		if (readw(i) == 0xaa55) {
#ifdef PCI_ROM_SCAN_VERBOSE
			printf("ROM signature found\n");
#endif
			pci_data = readw(0x18 + i);
			pci_data += i;

			if (0 == memcmp((void *)pci_data, "PCIR", 4)) {
#ifdef PCI_ROM_SCAN_VERBOSE
				printf("Fount PCI rom image at offset %d\n",
				       i - rom_addr);
				printf("Vendor %04x device %04x class %06x\n",
				       readw(pci_data + 4), readw(pci_data + 6),
				       readl(pci_data + 0x0d) & 0xffffff);
				printf("%s\n",
				       (readw(pci_data + 0x15) & 0x80) ?
				       "Last image" : "More images follow");
				switch	(readb(pci_data + 0x14)) {
				case 0:
					printf("X86 code\n");
					break;
				case 1:
					printf("Openfirmware code\n");
					break;
				case 2:
					printf("PARISC code\n");
					break;
				}
				printf("Image size %d\n",
				       readw(pci_data + 0x10) * 512);
#endif
				/*
				 * FixMe: I think we should compare the class
				 * code bytes as well but I have no reference
				 * on the exact order of these bytes in the PCI
				 * ROM header
				 */
				if (readw(pci_data + 4) == vendor &&
				    readw(pci_data + 6) == device &&
				    readb(pci_data + 0x14) == 0) {
#ifdef PCI_ROM_SCAN_VERBOSE
					printf("Suitable ROM image found\n");
#endif
					memmove(dest, (void *)rom_addr,
						readw(pci_data + 0x10) * 512);
					res = 0;
					break;

				}

				if (readw(pci_data + 0x15) & 0x80)
					break;
			}
		}

	}

#ifdef PCI_ROM_SCAN_VERBOSE
	if (res)
		printf("No suitable image found\n");
#endif
	/* disable PAR register and PCI device ROM address devocer */
	pci_remove_rom_window(hose, rom_addr);

	pci_write_config_dword(dev, PCI_ROM_ADDRESS, 0);

	return res;
}

#ifdef PCI_BIOS_DEBUG

void print_bios_bios_stat(void)
{
	printf("16 bit functions:\n");
	printf("pci_bios_present:                %d\n",
			RELOC_16_LONG(0xf000, num_pci_bios_present));
	printf("pci_bios_find_device:            %d\n",
			RELOC_16_LONG(0xf000, num_pci_bios_find_device));
	printf("pci_bios_find_class:             %d\n",
			RELOC_16_LONG(0xf000, num_pci_bios_find_class));
	printf("pci_bios_generate_special_cycle: %d\n",
			RELOC_16_LONG(0xf000,
				      num_pci_bios_generate_special_cycle));
	printf("pci_bios_read_cfg_byte:          %d\n",
			RELOC_16_LONG(0xf000, num_pci_bios_read_cfg_byte));
	printf("pci_bios_read_cfg_word:          %d\n",
			RELOC_16_LONG(0xf000, num_pci_bios_read_cfg_word));
	printf("pci_bios_read_cfg_dword:         %d\n",
			RELOC_16_LONG(0xf000, num_pci_bios_read_cfg_dword));
	printf("pci_bios_write_cfg_byte:         %d\n",
			RELOC_16_LONG(0xf000, num_pci_bios_write_cfg_byte));
	printf("pci_bios_write_cfg_word:         %d\n",
			RELOC_16_LONG(0xf000, num_pci_bios_write_cfg_word));
	printf("pci_bios_write_cfg_dword:        %d\n",
			RELOC_16_LONG(0xf000, num_pci_bios_write_cfg_dword));
	printf("pci_bios_get_irq_routing:        %d\n",
			RELOC_16_LONG(0xf000, num_pci_bios_get_irq_routing));
	printf("pci_bios_set_irq:                %d\n",
			RELOC_16_LONG(0xf000, num_pci_bios_set_irq));
	printf("pci_bios_unknown_function:       %d\n",
			RELOC_16_LONG(0xf000, num_pci_bios_unknown_function));
}
#endif
