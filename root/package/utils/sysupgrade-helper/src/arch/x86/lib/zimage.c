/*
 * Copyright (c) 2011 The Chromium OS Authors.
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

/*
 * Linux x86 zImage and bzImage loading
 *
 * based on the procdure described in
 * linux/Documentation/i386/boot.txt
 */

#include <common.h>
#include <asm/io.h>
#include <asm/ptrace.h>
#include <asm/zimage.h>
#include <asm/realmode.h>
#include <asm/byteorder.h>
#include <asm/bootparam.h>

/*
 * Memory lay-out:
 *
 * relative to setup_base (which is 0x90000 currently)
 *
 *	0x0000-0x7FFF	Real mode kernel
 *	0x8000-0x8FFF	Stack and heap
 *	0x9000-0x90FF	Kernel command line
 */
#define DEFAULT_SETUP_BASE	0x90000
#define COMMAND_LINE_OFFSET	0x9000
#define HEAP_END_OFFSET		0x8e00

#define COMMAND_LINE_SIZE	2048

unsigned generic_install_e820_map(unsigned max_entries,
				  struct e820entry *entries)
{
	return 0;
}

unsigned install_e820_map(unsigned max_entries,
			  struct e820entry *entries)
	__attribute__((weak, alias("generic_install_e820_map")));

static void build_command_line(char *command_line, int auto_boot)
{
	char *env_command_line;

	command_line[0] = '\0';

	env_command_line =  getenv("bootargs");

	/* set console= argument if we use a serial console */
	if (!strstr(env_command_line, "console=")) {
		if (!strcmp(getenv("stdout"), "serial")) {

			/* We seem to use serial console */
			sprintf(command_line, "console=ttyS0,%s ",
				getenv("baudrate"));
		}
	}

	if (auto_boot)
		strcat(command_line, "auto ");

	if (env_command_line)
		strcat(command_line, env_command_line);

	printf("Kernel command line: \"%s\"\n", command_line);
}

static int kernel_magic_ok(struct setup_header *hdr)
{
	if (KERNEL_MAGIC != hdr->boot_flag) {
		printf("Error: Invalid Boot Flag "
			"(found 0x%04x, expected 0x%04x)\n",
			hdr->boot_flag, KERNEL_MAGIC);
		return 0;
	} else {
		printf("Valid Boot Flag\n");
		return 1;
	}
}

static int get_boot_protocol(struct setup_header *hdr)
{
	if (hdr->header == KERNEL_V2_MAGIC) {
		printf("Magic signature found\n");
		return hdr->version;
	} else {
		/* Very old kernel */
		printf("Magic signature not found\n");
		return 0x0100;
	}
}

struct boot_params *load_zimage(char *image, unsigned long kernel_size,
				void **load_address)
{
	struct boot_params *setup_base;
	int setup_size;
	int bootproto;
	int big_image;

	struct boot_params *params = (struct boot_params *)image;
	struct setup_header *hdr = &params->hdr;

	/* base address for real-mode segment */
	setup_base = (struct boot_params *)DEFAULT_SETUP_BASE;

	if (!kernel_magic_ok(hdr))
		return 0;

	/* determine size of setup */
	if (0 == hdr->setup_sects) {
		printf("Setup Sectors = 0 (defaulting to 4)\n");
		setup_size = 5 * 512;
	} else {
		setup_size = (hdr->setup_sects + 1) * 512;
	}

	printf("Setup Size = 0x%8.8lx\n", (ulong)setup_size);

	if (setup_size > SETUP_MAX_SIZE)
		printf("Error: Setup is too large (%d bytes)\n", setup_size);

	/* determine boot protocol version */
	bootproto = get_boot_protocol(hdr);

	printf("Using boot protocol version %x.%02x\n",
	       (bootproto & 0xff00) >> 8, bootproto & 0xff);

	if (bootproto >= 0x0200) {
		if (hdr->setup_sects >= 15) {
			printf("Linux kernel version %s\n",
				(char *)params +
				hdr->kernel_version + 0x200);
		} else {
			printf("Setup Sectors < 15 - "
				"Cannot print kernel version.\n");
		}
	}

	/* Determine image type */
	big_image = (bootproto >= 0x0200) &&
		    (hdr->loadflags & BIG_KERNEL_FLAG);

	/* Determine load address */
	if (big_image)
		*load_address = (void *)BZIMAGE_LOAD_ADDR;
	else
		*load_address = (void *)ZIMAGE_LOAD_ADDR;

#if defined CONFIG_ZBOOT_32
	printf("Building boot_params at 0x%8.8lx\n", (ulong)setup_base);
	memset(setup_base, 0, sizeof(*setup_base));
	setup_base->hdr = params->hdr;
#else
	/* load setup */
	printf("Moving Real-Mode Code to 0x%8.8lx (%d bytes)\n",
	       (ulong)setup_base, setup_size);
	memmove(setup_base, image, setup_size);
#endif

	if (bootproto >= 0x0204)
		kernel_size = hdr->syssize * 16;
	else
		kernel_size -= setup_size;

	if (bootproto == 0x0100) {
		/*
		 * A very old kernel MUST have its real-mode code
		 * loaded at 0x90000
		 */
		if ((u32)setup_base != 0x90000) {
			/* Copy the real-mode kernel */
			memmove((void *)0x90000, setup_base, setup_size);

			/* Copy the command line */
			memmove((void *)0x99000,
				(u8 *)setup_base + COMMAND_LINE_OFFSET,
				COMMAND_LINE_SIZE);

			 /* Relocated */
			setup_base = (struct boot_params *)0x90000;
		}

		/* It is recommended to clear memory up to the 32K mark */
		memset((u8 *)0x90000 + setup_size, 0,
		       SETUP_MAX_SIZE - setup_size);
	}

	if (big_image) {
		if (kernel_size > BZIMAGE_MAX_SIZE) {
			printf("Error: bzImage kernel too big! "
				"(size: %ld, max: %d)\n",
				kernel_size, BZIMAGE_MAX_SIZE);
			return 0;
		}
	} else if ((kernel_size) > ZIMAGE_MAX_SIZE) {
		printf("Error: zImage kernel too big! (size: %ld, max: %d)\n",
		       kernel_size, ZIMAGE_MAX_SIZE);
		return 0;
	}

	printf("Loading %s at address %p (%ld bytes)\n",
		big_image ? "bzImage" : "zImage", *load_address, kernel_size);

	memmove(*load_address, image + setup_size, kernel_size);

	return setup_base;
}

int setup_zimage(struct boot_params *setup_base, char *cmd_line, int auto_boot,
		 unsigned long initrd_addr, unsigned long initrd_size)
{
	struct setup_header *hdr = &setup_base->hdr;
	int bootproto = get_boot_protocol(hdr);

#if defined CONFIG_ZBOOT_32
	setup_base->e820_entries = install_e820_map(
		ARRAY_SIZE(setup_base->e820_map), setup_base->e820_map);
#endif

	if (bootproto == 0x0100) {
		setup_base->screen_info.cl_magic = COMMAND_LINE_MAGIC;
		setup_base->screen_info.cl_offset = COMMAND_LINE_OFFSET;
	}
	if (bootproto >= 0x0200) {
		hdr->type_of_loader = 8;

		if (initrd_addr) {
			printf("Initial RAM disk at linear address "
			       "0x%08lx, size %ld bytes\n",
			       initrd_addr, initrd_size);

			hdr->ramdisk_image = initrd_addr;
			hdr->ramdisk_size = initrd_size;
		}
	}

	if (bootproto >= 0x0201) {
		hdr->heap_end_ptr = HEAP_END_OFFSET;
		hdr->loadflags |= HEAP_FLAG;
	}

	if (bootproto >= 0x0202) {
		hdr->cmd_line_ptr = (uintptr_t)cmd_line;
	} else if (bootproto >= 0x0200) {
		setup_base->screen_info.cl_magic = COMMAND_LINE_MAGIC;
		setup_base->screen_info.cl_offset =
			(uintptr_t)cmd_line - (uintptr_t)setup_base;

		hdr->setup_move_size = 0x9100;
	}

	/* build command line at COMMAND_LINE_OFFSET */
	build_command_line(cmd_line, auto_boot);
	return 0;
}

void boot_zimage(void *setup_base, void *load_address)
{
	printf("\nStarting kernel ...\n\n");

#if defined CONFIG_ZBOOT_32
	/*
	 * Set %ebx, %ebp, and %edi to 0, %esi to point to the boot_params
	 * structure, and then jump to the kernel. We assume that %cs is
	 * 0x10, 4GB flat, and read/execute, and the data segments are 0x18,
	 * 4GB flat, and read/write. U-boot is setting them up that way for
	 * itself in arch/i386/cpu/cpu.c.
	 */
	__asm__ __volatile__ (
	"movl $0, %%ebp		\n"
	"cli			\n"
	"jmp %[kernel_entry]	\n"
	:: [kernel_entry]"a"(load_address),
	   [boot_params] "S"(setup_base),
	   "b"(0), "D"(0)
	:  "%ebp"
	);
#else
	struct pt_regs regs;

	memset(&regs, 0, sizeof(struct pt_regs));
	regs.xds = (u32)setup_base >> 4;
	regs.xes = regs.xds;
	regs.xss = regs.xds;
	regs.esp = 0x9000;
	regs.eflags = 0;
	enter_realmode(((u32)setup_base + SETUP_START_OFFSET) >> 4, 0,
		       &regs, &regs);
#endif
}

void setup_pcat_compatibility(void)
	__attribute__((weak, alias("__setup_pcat_compatibility")));

void __setup_pcat_compatibility(void)
{
}

int do_zboot(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	struct boot_params *base_ptr;
	void *bzImage_addr = NULL;
	void *load_address;
	char *s;
	ulong bzImage_size = 0;
	ulong initrd_addr = 0;
	ulong initrd_size = 0;

	disable_interrupts();

	/* Setup board for maximum PC/AT Compatibility */
	setup_pcat_compatibility();

	if (argc >= 2) {
		/* argv[1] holds the address of the bzImage */
		s = argv[1];
	} else {
		s = getenv("fileaddr");
	}

	if (s)
		bzImage_addr = (void *)simple_strtoul(s, NULL, 16);

	if (argc >= 3) {
		/* argv[2] holds the size of the bzImage */
		bzImage_size = simple_strtoul(argv[2], NULL, 16);
	}

	if (argc >= 4)
		initrd_addr = simple_strtoul(argv[3], NULL, 16);
	if (argc >= 5)
		initrd_size = simple_strtoul(argv[4], NULL, 16);

	/* Lets look for */
	base_ptr = load_zimage(bzImage_addr, bzImage_size, &load_address);

	if (!base_ptr) {
		printf("## Kernel loading failed ...\n");
		return -1;
	}
	if (setup_zimage(base_ptr, (char *)base_ptr + COMMAND_LINE_OFFSET,
			0, initrd_addr, initrd_size)) {
		printf("Setting up boot parameters failed ...\n");
		return -1;
	}

	printf("## Transferring control to Linux "
	       "(at address %08x) ...\n",
	       (u32)base_ptr);

	/* we assume that the kernel is in place */
	boot_zimage(base_ptr, load_address);
	/* does not return */

	return -1;
}

U_BOOT_CMD(
	zboot, 5, 0,	do_zboot,
	"Boot bzImage",
	"[addr] [size] [initrd addr] [initrd size]\n"
	"      addr -        The optional starting address of the bzimage.\n"
	"                    If not set it defaults to the environment\n"
	"                    variable \"fileaddr\".\n"
	"      size -        The optional size of the bzimage. Defaults to\n"
	"                    zero.\n"
	"      initrd addr - The address of the initrd image to use, if any.\n"
	"      initrd size - The size of the initrd image to use, if any.\n"
);
