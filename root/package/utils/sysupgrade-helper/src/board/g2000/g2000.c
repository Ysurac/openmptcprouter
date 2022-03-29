/*
 * (C) Copyright 2004
 * Stefan Roese, esd gmbh germany, stefan.roese@esd-electronics.com
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
#include <asm/processor.h>
#include <command.h>

#define MEM_MCOPT1_INIT_VAL     0x00800000
#define MEM_RTR_INIT_VAL        0x04070000
#define MEM_PMIT_INIT_VAL       0x07c00000
#define MEM_MB0CF_INIT_VAL      0x00082001
#define MEM_MB1CF_INIT_VAL      0x04082000
#define MEM_SDTR1_INIT_VAL      0x00854005
#define SDRAM0_CFG_ENABLE       0x80000000

#define CONFIG_SYS_SDRAM_SIZE          0x04000000      /* 64 MB */

int board_early_init_f (void)
{
#if 0 /* test-only */
	mtdcr (UIC0SR, 0xFFFFFFFF);      /* clear all ints */
	mtdcr (UIC0ER, 0x00000000);      /* disable all ints */
	mtdcr (UIC0CR, 0x00000010);
	mtdcr (UIC0PR, 0xFFFF7FF0);      /* set int polarities */
	mtdcr (UIC0TR, 0x00000010);      /* set int trigger levels */
	mtdcr (UIC0SR, 0xFFFFFFFF);      /* clear all ints */
#else
	mtdcr(UIC0SR, 0xFFFFFFFF);       /* clear all ints */
	mtdcr(UIC0ER, 0x00000000);       /* disable all ints */
	mtdcr(UIC0CR, 0x00000000);       /* set all to be non-critical*/
	mtdcr(UIC0PR, 0xFFFFFFF0);       /* set int polarities */
	mtdcr(UIC0TR, 0x10000000);       /* set int trigger levels */
	mtdcr(UIC0VCR, 0x00000001);      /* set vect base=0,INT0 highest priority*/
	mtdcr(UIC0SR, 0xFFFFFFFF);       /* clear all ints */
#endif

#if 1 /* test-only */
	/*
	 * EBC Configuration Register: set ready timeout to 512 ebc-clks -> ca. 15 us
	 */
	mtebc (EBC0_CFG, 0xa8400000); /* ebc always driven */
#endif

	return 0;
}


int misc_init_f (void)
{
	return 0;  /* dummy implementation */
}


int misc_init_r (void)
{
#if defined(CONFIG_CMD_NAND)
	/*
	 * Set NAND-FLASH GPIO signals to default
	 */
	out32(GPIO0_OR, in32(GPIO0_OR) & ~(CONFIG_SYS_NAND_CLE | CONFIG_SYS_NAND_ALE));
	out32(GPIO0_OR, in32(GPIO0_OR) | CONFIG_SYS_NAND_CE);
#endif

	return (0);
}


/*
 * Check Board Identity:
 */
int checkboard (void)
{
	char str[64];
	int i = getenv_f("serial#", str, sizeof(str));

	puts ("Board: ");

	if (i == -1) {
		puts ("### No HW ID - assuming G2000");
	} else {
		puts(str);
	}

	putc ('\n');

	return 0;
}


/* -------------------------------------------------------------------------
  G2000 rev B is an embeded design. we don't read for spd of this version.
  Doing static SDRAM controller configuration in the following section.
   ------------------------------------------------------------------------- */

long int init_sdram_static_settings(void)
{
	/* disable memcontroller so updates work */
	mtsdram(SDRAM0_CFG, MEM_MCOPT1_INIT_VAL);
	mtsdram(SDRAM0_RTR, MEM_RTR_INIT_VAL);
	mtsdram(SDRAM0_PMIT, MEM_PMIT_INIT_VAL);
	mtsdram(SDRAM0_B0CR, MEM_MB0CF_INIT_VAL);
	mtsdram(SDRAM0_B1CR, MEM_MB1CF_INIT_VAL);
	mtsdram(SDRAM0_TR, MEM_SDTR1_INIT_VAL);

	/* SDRAM have a power on delay,  500 micro should do */
	udelay(500);
	mtsdram(SDRAM0_CFG, MEM_MCOPT1_INIT_VAL|SDRAM0_CFG_ENABLE);

	return (CONFIG_SYS_SDRAM_SIZE); /* CONFIG_SYS_SDRAM_SIZE is in G2000.h */
 }


phys_size_t initdram (int board_type)
{
	long int ret;

/* flzt, we can still turn this on in the future */
/* #ifdef CONFIG_SPD_EEPROM
	ret = spd_sdram ();
#else
	ret = init_sdram_static_settings();
#endif
*/

	ret = init_sdram_static_settings();

	return ret;
}

#if 0 /* test-only !!! */
int do_dumpebc(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	ulong ap, cr;

	printf("\nEBC registers for PPC405GP:\n");
	mfebc(PB0AP, ap); mfebc(PB0CR, cr);
	printf("0: AP=%08lx CP=%08lx\n", ap, cr);
	mfebc(PB1AP, ap); mfebc(PB1CR, cr);
	printf("1: AP=%08lx CP=%08lx\n", ap, cr);
	mfebc(PB2AP, ap); mfebc(PB2CR, cr);
	printf("2: AP=%08lx CP=%08lx\n", ap, cr);
	mfebc(PB3AP, ap); mfebc(PB3CR, cr);
	printf("3: AP=%08lx CP=%08lx\n", ap, cr);
	mfebc(PB4AP, ap); mfebc(PB4CR, cr);
	printf("4: AP=%08lx CP=%08lx\n", ap, cr);
	printf("\n");

	return 0;
}
U_BOOT_CMD(
	dumpebc,	1,	1,	do_dumpebc,
	"Dump all EBC registers",
	""
);


int do_dumpdcr(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int i;

	printf("\nDevice Configuration Registers (DCR's) for PPC405GP:");
	for (i=0; i<=0x1e0; i++) {
		if (!(i % 0x8)) {
			printf("\n%04x ", i);
		}
		printf("%08lx ", get_dcr(i));
	}
	printf("\n");

	return 0;
}
U_BOOT_CMD(
	dumpdcr,	1,	1,	do_dumpdcr,
	"Dump all DCR registers",
	""
);


int do_dumpspr(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	printf("\nSpecial Purpose Registers (SPR's) for PPC405GP:");
	printf("\n%04x %08x ", 947, mfspr(947));
	printf("\n%04x %08x ", 9, mfspr(9));
	printf("\n%04x %08x ", 1014, mfspr(1014));
	printf("\n%04x %08x ", 1015, mfspr(1015));
	printf("\n%04x %08x ", 1010, mfspr(1010));
	printf("\n%04x %08x ", 957, mfspr(957));
	printf("\n%04x %08x ", 1008, mfspr(1008));
	printf("\n%04x %08x ", 1018, mfspr(1018));
	printf("\n%04x %08x ", 954, mfspr(954));
	printf("\n%04x %08x ", 950, mfspr(950));
	printf("\n%04x %08x ", 951, mfspr(951));
	printf("\n%04x %08x ", 981, mfspr(981));
	printf("\n%04x %08x ", 980, mfspr(980));
	printf("\n%04x %08x ", 982, mfspr(982));
	printf("\n%04x %08x ", 1012, mfspr(1012));
	printf("\n%04x %08x ", 1013, mfspr(1013));
	printf("\n%04x %08x ", 948, mfspr(948));
	printf("\n%04x %08x ", 949, mfspr(949));
	printf("\n%04x %08x ", 1019, mfspr(1019));
	printf("\n%04x %08x ", 979, mfspr(979));
	printf("\n%04x %08x ", 8, mfspr(8));
	printf("\n%04x %08x ", 945, mfspr(945));
	printf("\n%04x %08x ", 987, mfspr(987));
	printf("\n%04x %08x ", 287, mfspr(287));
	printf("\n%04x %08x ", 953, mfspr(953));
	printf("\n%04x %08x ", 955, mfspr(955));
	printf("\n%04x %08x ", 272, mfspr(272));
	printf("\n%04x %08x ", 273, mfspr(273));
	printf("\n%04x %08x ", 274, mfspr(274));
	printf("\n%04x %08x ", 275, mfspr(275));
	printf("\n%04x %08x ", 260, mfspr(260));
	printf("\n%04x %08x ", 276, mfspr(276));
	printf("\n%04x %08x ", 261, mfspr(261));
	printf("\n%04x %08x ", 277, mfspr(277));
	printf("\n%04x %08x ", 262, mfspr(262));
	printf("\n%04x %08x ", 278, mfspr(278));
	printf("\n%04x %08x ", 263, mfspr(263));
	printf("\n%04x %08x ", 279, mfspr(279));
	printf("\n%04x %08x ", 26, mfspr(26));
	printf("\n%04x %08x ", 27, mfspr(27));
	printf("\n%04x %08x ", 990, mfspr(990));
	printf("\n%04x %08x ", 991, mfspr(991));
	printf("\n%04x %08x ", 956, mfspr(956));
	printf("\n%04x %08x ", 284, mfspr(284));
	printf("\n%04x %08x ", 285, mfspr(285));
	printf("\n%04x %08x ", 986, mfspr(986));
	printf("\n%04x %08x ", 984, mfspr(984));
	printf("\n%04x %08x ", 256, mfspr(256));
	printf("\n%04x %08x ", 1, mfspr(1));
	printf("\n%04x %08x ", 944, mfspr(944));
	printf("\n");

	return 0;
}
U_BOOT_CMD(
	dumpspr,	1,	1,	do_dumpspr,
	"Dump all SPR registers",
	""
);
#endif
