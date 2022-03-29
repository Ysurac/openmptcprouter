/*
 * (C) Copyright 2003
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
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
 * Boot support
 */
#include <common.h>
#include <command.h>
#include <linux/compiler.h>

DECLARE_GLOBAL_DATA_PTR;

__maybe_unused
static void print_num(const char *name, ulong value)
{
	printf("%-12s= 0x%08lX\n", name, value);
}

__maybe_unused
static void print_eth(int idx)
{
	char name[10], *val;
	if (idx)
		sprintf(name, "eth%iaddr", idx);
	else
		strcpy(name, "ethaddr");
	val = getenv(name);
	if (!val)
		val = "(not set)";
	printf("%-12s= %s\n", name, val);
}

__maybe_unused
static void print_lnum(const char *name, u64 value)
{
	printf("%-12s= 0x%.8llX\n", name, value);
}

__maybe_unused
static void print_mhz(const char *name, unsigned long hz)
{
	char buf[32];

	printf("%-12s= %6s MHz\n", name, strmhz(buf, hz));
}

#if defined(CONFIG_PPC)

int do_bdinfo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	bd_t *bd = gd->bd;

#ifdef DEBUG
	print_num("bd address",		(ulong)bd);
#endif
	print_num("memstart",		bd->bi_memstart);
	print_lnum("memsize",		bd->bi_memsize);
	print_num("flashstart",		bd->bi_flashstart);
	print_num("flashsize",		bd->bi_flashsize);
	print_num("flashoffset",	bd->bi_flashoffset);
	print_num("sramstart",		bd->bi_sramstart);
	print_num("sramsize",		bd->bi_sramsize);
#if	defined(CONFIG_5xx)  || defined(CONFIG_8xx) || \
	defined(CONFIG_8260) || defined(CONFIG_E500)
	print_num("immr_base",		bd->bi_immr_base);
#endif
	print_num("bootflags",		bd->bi_bootflags);
#if	defined(CONFIG_405CR) || defined(CONFIG_405EP) || \
	defined(CONFIG_405GP) || \
	defined(CONFIG_440EP) || defined(CONFIG_440EPX) || \
	defined(CONFIG_440GR) || defined(CONFIG_440GRX) || \
	defined(CONFIG_440SP) || defined(CONFIG_440SPE) || \
	defined(CONFIG_XILINX_405)
	print_mhz("procfreq",		bd->bi_procfreq);
	print_mhz("plb_busfreq",	bd->bi_plb_busfreq);
#if	defined(CONFIG_405EP) || defined(CONFIG_405GP) || \
	defined(CONFIG_440EP) || defined(CONFIG_440EPX) || \
	defined(CONFIG_440GR) || defined(CONFIG_440GRX) || \
	defined(CONFIG_440SPE) || defined(CONFIG_XILINX_405)
	print_mhz("pci_busfreq",	bd->bi_pci_busfreq);
#endif
#else	/* ! CONFIG_405GP, CONFIG_405CR, CONFIG_405EP, CONFIG_XILINX_405, CONFIG_440EP CONFIG_440GR */
#if defined(CONFIG_CPM2)
	print_mhz("vco",		bd->bi_vco);
	print_mhz("sccfreq",		bd->bi_sccfreq);
	print_mhz("brgfreq",		bd->bi_brgfreq);
#endif
	print_mhz("intfreq",		bd->bi_intfreq);
#if defined(CONFIG_CPM2)
	print_mhz("cpmfreq",		bd->bi_cpmfreq);
#endif
	print_mhz("busfreq",		bd->bi_busfreq);
#endif /* CONFIG_405GP, CONFIG_405CR, CONFIG_405EP, CONFIG_XILINX_405, CONFIG_440EP CONFIG_440GR */
#if defined(CONFIG_MPC8220)
	print_mhz("inpfreq",		bd->bi_inpfreq);
	print_mhz("flbfreq",		bd->bi_flbfreq);
	print_mhz("pcifreq",		bd->bi_pcifreq);
	print_mhz("vcofreq",		bd->bi_vcofreq);
	print_mhz("pevfreq",		bd->bi_pevfreq);
#endif

#ifdef CONFIG_ENABLE_36BIT_PHYS
#ifdef CONFIG_PHYS_64BIT
	puts("addressing  = 36-bit\n");
#else
	puts("addressing  = 32-bit\n");
#endif
#endif

	print_eth(0);
#if defined(CONFIG_HAS_ETH1)
	print_eth(1);
#endif
#if defined(CONFIG_HAS_ETH2)
	print_eth(2);
#endif
#if defined(CONFIG_HAS_ETH3)
	print_eth(3);
#endif
#if defined(CONFIG_HAS_ETH4)
	print_eth(4);
#endif
#if defined(CONFIG_HAS_ETH5)
	print_eth(5);
#endif

#ifdef CONFIG_HERMES
	print_mhz("ethspeed",		bd->bi_ethspeed);
#endif
	printf("IP addr     = %s\n", getenv("ipaddr"));
	printf("baudrate    = %6ld bps\n", bd->bi_baudrate);
	print_num("relocaddr", gd->relocaddr);
	return 0;
}

#elif defined(CONFIG_NIOS2)

int do_bdinfo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	bd_t *bd = gd->bd;

	print_num("mem start",		(ulong)bd->bi_memstart);
	print_lnum("mem size",		(u64)bd->bi_memsize);
	print_num("flash start",	(ulong)bd->bi_flashstart);
	print_num("flash size",		(ulong)bd->bi_flashsize);
	print_num("flash offset",	(ulong)bd->bi_flashoffset);

#if defined(CONFIG_SYS_SRAM_BASE)
	print_num ("sram start",	(ulong)bd->bi_sramstart);
	print_num ("sram size",		(ulong)bd->bi_sramsize);
#endif

#if defined(CONFIG_CMD_NET)
	print_eth(0);
	printf("ip_addr     = %s\n", getenv("ipaddr"));
#endif

	printf("baudrate    = %ld bps\n", bd->bi_baudrate);

	return 0;
}

#elif defined(CONFIG_MICROBLAZE)

int do_bdinfo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	bd_t *bd = gd->bd;
	print_num("mem start      ",	(ulong)bd->bi_memstart);
	print_lnum("mem size       ",	(u64)bd->bi_memsize);
	print_num("flash start    ",	(ulong)bd->bi_flashstart);
	print_num("flash size     ",	(ulong)bd->bi_flashsize);
	print_num("flash offset   ",	(ulong)bd->bi_flashoffset);
#if defined(CONFIG_SYS_SRAM_BASE)
	print_num("sram start     ",	(ulong)bd->bi_sramstart);
	print_num("sram size      ",	(ulong)bd->bi_sramsize);
#endif
#if defined(CONFIG_CMD_NET)
	print_eth(0);
	printf("ip_addr     = %s\n", getenv("ipaddr"));
#endif
	printf("baudrate    = %ld bps\n", (ulong)bd->bi_baudrate);
	return 0;
}

#elif defined(CONFIG_SPARC)

int do_bdinfo(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	bd_t *bd = gd->bd;

#ifdef DEBUG
	print_num("bd address             ", (ulong) bd);
#endif
	print_num("memstart               ", bd->bi_memstart);
	print_lnum("memsize                ", bd->bi_memsize);
	print_num("flashstart             ", bd->bi_flashstart);
	print_num("CONFIG_SYS_MONITOR_BASE       ", CONFIG_SYS_MONITOR_BASE);
	print_num("CONFIG_ENV_ADDR           ", CONFIG_ENV_ADDR);
	printf("CONFIG_SYS_RELOC_MONITOR_BASE = 0x%lx (%d)\n", CONFIG_SYS_RELOC_MONITOR_BASE,
	       CONFIG_SYS_MONITOR_LEN);
	printf("CONFIG_SYS_MALLOC_BASE        = 0x%lx (%d)\n", CONFIG_SYS_MALLOC_BASE,
	       CONFIG_SYS_MALLOC_LEN);
	printf("CONFIG_SYS_INIT_SP_OFFSET     = 0x%lx (%d)\n", CONFIG_SYS_INIT_SP_OFFSET,
	       CONFIG_SYS_STACK_SIZE);
	printf("CONFIG_SYS_PROM_OFFSET        = 0x%lx (%d)\n", CONFIG_SYS_PROM_OFFSET,
	       CONFIG_SYS_PROM_SIZE);
	printf("CONFIG_SYS_GBL_DATA_OFFSET    = 0x%lx (%d)\n", CONFIG_SYS_GBL_DATA_OFFSET,
	       GENERATED_GBL_DATA_SIZE);

#if defined(CONFIG_CMD_NET)
	print_eth(0);
	printf("ip_addr     = %s\n", getenv("ipaddr"));
#endif
	printf("baudrate               = %6ld bps\n", bd->bi_baudrate);
	return 0;
}

#elif defined(CONFIG_M68K)

int do_bdinfo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	bd_t *bd = gd->bd;

	print_num("memstart",		(ulong)bd->bi_memstart);
	print_lnum("memsize",		(u64)bd->bi_memsize);
	print_num("flashstart",		(ulong)bd->bi_flashstart);
	print_num("flashsize",		(ulong)bd->bi_flashsize);
	print_num("flashoffset",	(ulong)bd->bi_flashoffset);
#if defined(CONFIG_SYS_INIT_RAM_ADDR)
	print_num("sramstart",		(ulong)bd->bi_sramstart);
	print_num("sramsize",		(ulong)bd->bi_sramsize);
#endif
#if defined(CONFIG_SYS_MBAR)
	print_num("mbar",		bd->bi_mbar_base);
#endif
	print_mhz("cpufreq",		bd->bi_intfreq);
	print_mhz("busfreq",		bd->bi_busfreq);
#ifdef CONFIG_PCI
	print_mhz("pcifreq",		bd->bi_pcifreq);
#endif
#ifdef CONFIG_EXTRA_CLOCK
	print_mhz("flbfreq",		bd->bi_flbfreq);
	print_mhz("inpfreq",		bd->bi_inpfreq);
	print_mhz("vcofreq",		bd->bi_vcofreq);
#endif
#if defined(CONFIG_CMD_NET)
	print_eth(0);
#if defined(CONFIG_HAS_ETH1)
	print_eth(1);
#endif
#if defined(CONFIG_HAS_ETH2)
	print_eth(2);
#endif
#if defined(CONFIG_HAS_ETH3)
	print_eth(3);
#endif

	printf("ip_addr     = %s\n", getenv("ipaddr"));
#endif
	printf("baudrate    = %ld bps\n", bd->bi_baudrate);

	return 0;
}

#elif defined(CONFIG_BLACKFIN)

int do_bdinfo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	bd_t *bd = gd->bd;

	printf("U-Boot      = %s\n", bd->bi_r_version);
	printf("CPU         = %s\n", bd->bi_cpu);
	printf("Board       = %s\n", bd->bi_board_name);
	print_mhz("VCO",	bd->bi_vco);
	print_mhz("CCLK",	bd->bi_cclk);
	print_mhz("SCLK",	bd->bi_sclk);

	print_num("boot_params",	(ulong)bd->bi_boot_params);
	print_num("memstart",		(ulong)bd->bi_memstart);
	print_lnum("memsize",		(u64)bd->bi_memsize);
	print_num("flashstart",		(ulong)bd->bi_flashstart);
	print_num("flashsize",		(ulong)bd->bi_flashsize);
	print_num("flashoffset",	(ulong)bd->bi_flashoffset);

	print_eth(0);
	printf("ip_addr     = %s\n", getenv("ipaddr"));
	printf("baudrate    = %d bps\n", bd->bi_baudrate);

	return 0;
}

#elif defined(CONFIG_MIPS)

int do_bdinfo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	bd_t *bd = gd->bd;

	print_num("boot_params",	(ulong)bd->bi_boot_params);
	print_num("memstart",		(ulong)bd->bi_memstart);
	print_lnum("memsize",		(u64)bd->bi_memsize);
	print_num("flashstart",		(ulong)bd->bi_flashstart);
	print_num("flashsize",		(ulong)bd->bi_flashsize);
	print_num("flashoffset",	(ulong)bd->bi_flashoffset);

	print_eth(0);
	printf("ip_addr     = %s\n", getenv("ipaddr"));
	printf("baudrate    = %d bps\n", bd->bi_baudrate);

	return 0;
}

#elif defined(CONFIG_AVR32)

int do_bdinfo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	bd_t *bd = gd->bd;

	print_num("boot_params",	(ulong)bd->bi_boot_params);
	print_num("memstart",		(ulong)bd->bi_memstart);
	print_lnum("memsize",		(u64)bd->bi_memsize);
	print_num("flashstart",		(ulong)bd->bi_flashstart);
	print_num("flashsize",		(ulong)bd->bi_flashsize);
	print_num("flashoffset",	(ulong)bd->bi_flashoffset);

	print_eth(0);
	printf("ip_addr     = %s\n", getenv("ipaddr"));
	printf("baudrate    = %lu bps\n", bd->bi_baudrate);

	return 0;
}

#elif defined(CONFIG_ARM)

int do_bdinfo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int i;
	bd_t *bd = gd->bd;

	print_num("arch_number",	bd->bi_arch_number);
	print_num("boot_params",	(ulong)bd->bi_boot_params);

	for (i = 0; i < CONFIG_NR_DRAM_BANKS; ++i) {
		print_num("DRAM bank",	i);
		print_num("-> start",	bd->bi_dram[i].start);
		print_num("-> size",	bd->bi_dram[i].size);
	}

#if defined(CONFIG_CMD_NET)
	print_eth(0);
	printf("ip_addr     = %s\n", getenv("ipaddr"));
#endif
	printf("baudrate    = %d bps\n", bd->bi_baudrate);
#if !(defined(CONFIG_SYS_ICACHE_OFF) && defined(CONFIG_SYS_DCACHE_OFF))
	print_num("TLB addr", gd->tlb_addr);
#endif
	print_num("relocaddr", gd->relocaddr);
	print_num("reloc off", gd->reloc_off);
	print_num("irq_sp", gd->irq_sp);	/* irq stack pointer */
	print_num("sp start ", gd->start_addr_sp);
	print_num("FB base  ", gd->fb_base);
	/*
	 * TODO: Currently only support for davinci SOC's is added.
	 * Remove this check once all the board implement this.
	 */
#ifdef CONFIG_CLOCKS
	printf("ARM frequency = %ld MHz\n", gd->bd->bi_arm_freq);
	printf("DSP frequency = %ld MHz\n", gd->bd->bi_dsp_freq);
	printf("DDR frequency = %ld MHz\n", gd->bd->bi_ddr_freq);
#endif
	return 0;
}

#elif defined(CONFIG_SH)

int do_bdinfo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	bd_t *bd = gd->bd;
	print_num("mem start      ",	(ulong)bd->bi_memstart);
	print_lnum("mem size       ",	(u64)bd->bi_memsize);
	print_num("flash start    ",	(ulong)bd->bi_flashstart);
	print_num("flash size     ",	(ulong)bd->bi_flashsize);
	print_num("flash offset   ",	(ulong)bd->bi_flashoffset);

#if defined(CONFIG_CMD_NET)
	print_eth(0);
	printf("ip_addr     = %s\n", getenv("ipaddr"));
#endif
	printf("baudrate    = %ld bps\n", (ulong)bd->bi_baudrate);
	return 0;
}

#elif defined(CONFIG_X86)

int do_bdinfo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int i;
	bd_t *bd = gd->bd;

	print_num("boot_params",	(ulong)bd->bi_boot_params);
	print_num("bi_memstart",	bd->bi_memstart);
	print_num("bi_memsize",		bd->bi_memsize);
	print_num("bi_flashstart",	bd->bi_flashstart);
	print_num("bi_flashsize",	bd->bi_flashsize);
	print_num("bi_flashoffset",	bd->bi_flashoffset);
	print_num("bi_sramstart",	bd->bi_sramstart);
	print_num("bi_sramsize",	bd->bi_sramsize);
	print_num("bi_bootflags",	bd->bi_bootflags);
	print_mhz("cpufreq",		bd->bi_intfreq);
	print_mhz("busfreq",		bd->bi_busfreq);

	for (i = 0; i < CONFIG_NR_DRAM_BANKS; ++i) {
		print_num("DRAM bank",	i);
		print_num("-> start",	bd->bi_dram[i].start);
		print_num("-> size",	bd->bi_dram[i].size);
	}

#if defined(CONFIG_CMD_NET)
	print_eth(0);
	printf("ip_addr     = %s\n", getenv("ipaddr"));
	print_mhz("ethspeed",	    bd->bi_ethspeed);
#endif
	printf("baudrate    = %d bps\n", bd->bi_baudrate);

	return 0;
}

#elif defined(CONFIG_SANDBOX)

int do_bdinfo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int i;
	bd_t *bd = gd->bd;

	print_num("boot_params", (ulong)bd->bi_boot_params);

	for (i = 0; i < CONFIG_NR_DRAM_BANKS; ++i) {
		print_num("DRAM bank", i);
		print_num("-> start", bd->bi_dram[i].start);
		print_num("-> size", bd->bi_dram[i].size);
	}

#if defined(CONFIG_CMD_NET)
	print_eth(0);
	printf("ip_addr     = %s\n", getenv("ipaddr"));
#endif
	print_num("FB base  ", gd->fb_base);
	return 0;
}

#elif defined(CONFIG_NDS32)

int do_bdinfo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int i;
	bd_t *bd = gd->bd;

	print_num("arch_number",	bd->bi_arch_number);
	print_num("boot_params",	(ulong)bd->bi_boot_params);

	for (i = 0; i < CONFIG_NR_DRAM_BANKS; ++i) {
		print_num("DRAM bank",	i);
		print_num("-> start",	bd->bi_dram[i].start);
		print_num("-> size",	bd->bi_dram[i].size);
	}

#if defined(CONFIG_CMD_NET)
	print_eth(0);
	printf("ip_addr     = %s\n", getenv("ipaddr"));
#endif
	printf("baudrate    = %d bps\n", bd->bi_baudrate);

	return 0;
}

#elif defined(CONFIG_OPENRISC)

int do_bdinfo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	bd_t *bd = gd->bd;

	print_num("mem start",		(ulong)bd->bi_memstart);
	print_lnum("mem size",		(u64)bd->bi_memsize);
	print_num("flash start",	(ulong)bd->bi_flashstart);
	print_num("flash size",		(ulong)bd->bi_flashsize);
	print_num("flash offset",	(ulong)bd->bi_flashoffset);

#if defined(CONFIG_CMD_NET)
	print_eth(0);
	printf("ip_addr     = %s\n", getenv("ipaddr"));
#endif

	printf("baudrate    = %ld bps\n", bd->bi_baudrate);

	return 0;
}

#else
 #error "a case for this architecture does not exist!"
#endif

/* -------------------------------------------------------------------- */

U_BOOT_CMD(
	bdinfo,	1,	1,	do_bdinfo,
	"print Board Info structure",
	""
);
