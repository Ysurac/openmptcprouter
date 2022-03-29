/*
 * (C) Copyright 2003-2005
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
 * This file contains the configuration parameters for the INCA-IP board.
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#define CONFIG_MIPS32		1	/* MIPS 4Kc CPU core	*/
#define CONFIG_INCA_IP		1	/* on a INCA-IP Board	*/

#define CONFIG_XWAY_SWAP_BYTES

/*
 * Clock for the MIPS core (MHz)
 * allowed values: 100000000, 133000000, and 150000000 (default)
 */
#ifndef CONFIG_CPU_CLOCK_RATE
#define CONFIG_CPU_CLOCK_RATE	150000000
#endif

#define CONFIG_SYS_XWAY_EBU_BOOTCFG	0x40C4	/* CMULT = 8 */

#define CONFIG_BOOTDELAY	5	/* autoboot after 5 seconds	*/

#define CONFIG_BAUDRATE		115200

#define	CONFIG_TIMESTAMP		/* Print image info with timestamp */

#define CONFIG_PREBOOT	"echo;"	\
	"echo Type \\\"run flash_nfs\\\" to mount root filesystem over NFS;" \
	"echo"

#undef	CONFIG_BOOTARGS

#define	CONFIG_EXTRA_ENV_SETTINGS					\
	"nfsargs=setenv bootargs root=/dev/nfs rw "			\
		"nfsroot=${serverip}:${rootpath}\0"			\
	"ramargs=setenv bootargs root=/dev/ram rw\0"			\
	"addip=setenv bootargs ${bootargs} "				\
		"ip=${ipaddr}:${serverip}:${gatewayip}:${netmask}"	\
		":${hostname}:${netdev}:off\0"				\
	"addmisc=setenv bootargs ${bootargs} "				\
		"console=ttyS0,${baudrate} "				\
		"ethaddr=${ethaddr} "					\
		"panic=1\0"						\
	"flash_nfs=run nfsargs addip addmisc;"				\
		"bootm ${kernel_addr}\0"				\
	"flash_self=run ramargs addip addmisc;"				\
		"bootm ${kernel_addr} ${ramdisk_addr}\0"		\
	"net_nfs=tftp 80500000 ${bootfile};"				\
		"run nfsargs addip addmisc;bootm\0"			\
	"rootpath=/opt/eldk/mips_4KC\0"					\
	"bootfile=/tftpboot/INCA/uImage\0"				\
	"kernel_addr=B0040000\0"					\
	"ramdisk_addr=B0100000\0"					\
	"u-boot=/tftpboot/INCA/u-boot.bin\0"				\
	"load=tftp 80500000 ${u-boot}\0"				\
	"update=protect off 1:0-2;era 1:0-2;"				\
		"cp.b 80500000 B0000000 ${filesize}\0"			\
	""
#define CONFIG_BOOTCOMMAND	"run flash_self"


/*
 * BOOTP options
 */
#define CONFIG_BOOTP_BOOTFILESIZE
#define CONFIG_BOOTP_BOOTPATH
#define CONFIG_BOOTP_GATEWAY
#define CONFIG_BOOTP_HOSTNAME


/*
 * Command line configuration.
 */
#include <config_cmd_default.h>

#define CONFIG_CMD_ASKENV
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_ELF
#define CONFIG_CMD_JFFS2
#define CONFIG_CMD_NFS
#define CONFIG_CMD_PING
#define CONFIG_CMD_SNTP


/*
 * Miscellaneous configurable options
 */
#define	CONFIG_SYS_LONGHELP				/* undef to save memory      */
#define	CONFIG_SYS_PROMPT		"INCA-IP # "	/* Monitor Command Prompt    */
#define	CONFIG_SYS_CBSIZE		256		/* Console I/O Buffer Size   */
#define	CONFIG_SYS_PBSIZE (CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16)  /* Print Buffer Size */
#define	CONFIG_SYS_MAXARGS		16		/* max number of command args*/

#define CONFIG_SYS_MALLOC_LEN		128*1024

#define CONFIG_SYS_BOOTPARAMS_LEN	128*1024

#define CONFIG_SYS_MIPS_TIMER_FREQ	(incaip_get_cpuclk() / 2)

#define CONFIG_SYS_HZ			1000

#define CONFIG_SYS_SDRAM_BASE		0x80000000

#define	CONFIG_SYS_LOAD_ADDR		0x80100000	/* default load address	*/

#define CONFIG_SYS_MEMTEST_START	0x80100000
#define CONFIG_SYS_MEMTEST_END		0x80800000

/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */
#define CONFIG_SYS_MAX_FLASH_BANKS	2	/* max number of memory banks */
#define CONFIG_SYS_MAX_FLASH_SECT	(128)	/* max number of sectors on one chip */

#define PHYS_FLASH_1		0xb0000000 /* Flash Bank #1 */
#define PHYS_FLASH_2		0xb0800000 /* Flash Bank #2 */

/* The following #defines are needed to get flash environment right */
#define	CONFIG_SYS_MONITOR_BASE	CONFIG_SYS_TEXT_BASE
#define	CONFIG_SYS_MONITOR_LEN		(192 << 10)

#define CONFIG_SYS_INIT_SP_OFFSET	0x400000

#define CONFIG_SYS_FLASH_BASE		PHYS_FLASH_1

/* timeout values are in ticks */
#define CONFIG_SYS_FLASH_ERASE_TOUT	(2 * CONFIG_SYS_HZ) /* Timeout for Flash Erase */
#define CONFIG_SYS_FLASH_WRITE_TOUT	(2 * CONFIG_SYS_HZ) /* Timeout for Flash Write */

#define	CONFIG_ENV_IS_IN_FLASH	1

/* Address and size of Primary Environment Sector	*/
#define CONFIG_ENV_ADDR		0xB0030000
#define CONFIG_ENV_SIZE		0x10000

#define CONFIG_FLASH_16BIT

#define CONFIG_NR_DRAM_BANKS	1

#define CONFIG_INCA_IP_SWITCH
#define CONFIG_INCA_IP_SWITCH_AMDIX

/*
 * JFFS2 partitions
 */
/* No command line, one static partition, use all space on the device */
#undef CONFIG_CMD_MTDPARTS
#define CONFIG_JFFS2_DEV		"nor1"
#define CONFIG_JFFS2_PART_SIZE		0xFFFFFFFF
#define CONFIG_JFFS2_PART_OFFSET	0x00000000

/* mtdparts command line support */
/*
#define CONFIG_CMD_MTDPARTS
#define MTDIDS_DEFAULT		"nor0=INCA-IP Bank 0"
#define MTDPARTS_DEFAULT	"mtdparts=INCA-IP Bank 0:192k(uboot)," \
							"64k(env)," \
							"768k(linux)," \
							"1m@3m(rootfs)," \
							"768k(linux2)," \
							"3m@5m(rootfs2)"
*/

/*-----------------------------------------------------------------------
 * Cache Configuration
 */
#define CONFIG_SYS_DCACHE_SIZE		4096
#define CONFIG_SYS_ICACHE_SIZE		4096
#define CONFIG_SYS_CACHELINE_SIZE	16

#endif	/* __CONFIG_H */
