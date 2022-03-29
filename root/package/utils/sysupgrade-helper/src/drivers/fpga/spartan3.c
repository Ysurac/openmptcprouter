/*
 * (C) Copyright 2002
 * Rich Ireland, Enterasys Networks, rireland@enterasys.com.
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
 *
 */

/*
 * Configuration support for Xilinx Spartan3 devices.  Based
 * on spartan2.c (Rich Ireland, rireland@enterasys.com).
 */

#include <common.h>		/* core U-Boot definitions */
#include <spartan3.h>		/* Spartan-II device family */

/* Define FPGA_DEBUG to get debug printf's */
#ifdef	FPGA_DEBUG
#define PRINTF(fmt,args...)	printf (fmt ,##args)
#else
#define PRINTF(fmt,args...)
#endif

#undef CONFIG_SYS_FPGA_CHECK_BUSY

/* Note: The assumption is that we cannot possibly run fast enough to
 * overrun the device (the Slave Parallel mode can free run at 50MHz).
 * If there is a need to operate slower, define CONFIG_FPGA_DELAY in
 * the board config file to slow things down.
 */
#ifndef CONFIG_FPGA_DELAY
#define CONFIG_FPGA_DELAY()
#endif

#ifndef CONFIG_SYS_FPGA_WAIT
#define CONFIG_SYS_FPGA_WAIT CONFIG_SYS_HZ/100	/* 10 ms */
#endif

static int Spartan3_sp_load(Xilinx_desc *desc, const void *buf, size_t bsize);
static int Spartan3_sp_dump(Xilinx_desc *desc, const void *buf, size_t bsize);
/* static int Spartan3_sp_info(Xilinx_desc *desc ); */

static int Spartan3_ss_load(Xilinx_desc *desc, const void *buf, size_t bsize);
static int Spartan3_ss_dump(Xilinx_desc *desc, const void *buf, size_t bsize);
/* static int Spartan3_ss_info(Xilinx_desc *desc); */

/* ------------------------------------------------------------------------- */
/* Spartan-II Generic Implementation */
int Spartan3_load(Xilinx_desc *desc, const void *buf, size_t bsize)
{
	int ret_val = FPGA_FAIL;

	switch (desc->iface) {
	case slave_serial:
		PRINTF ("%s: Launching Slave Serial Load\n", __FUNCTION__);
		ret_val = Spartan3_ss_load (desc, buf, bsize);
		break;

	case slave_parallel:
		PRINTF ("%s: Launching Slave Parallel Load\n", __FUNCTION__);
		ret_val = Spartan3_sp_load (desc, buf, bsize);
		break;

	default:
		printf ("%s: Unsupported interface type, %d\n",
				__FUNCTION__, desc->iface);
	}

	return ret_val;
}

int Spartan3_dump(Xilinx_desc *desc, const void *buf, size_t bsize)
{
	int ret_val = FPGA_FAIL;

	switch (desc->iface) {
	case slave_serial:
		PRINTF ("%s: Launching Slave Serial Dump\n", __FUNCTION__);
		ret_val = Spartan3_ss_dump (desc, buf, bsize);
		break;

	case slave_parallel:
		PRINTF ("%s: Launching Slave Parallel Dump\n", __FUNCTION__);
		ret_val = Spartan3_sp_dump (desc, buf, bsize);
		break;

	default:
		printf ("%s: Unsupported interface type, %d\n",
				__FUNCTION__, desc->iface);
	}

	return ret_val;
}

int Spartan3_info( Xilinx_desc *desc )
{
	return FPGA_SUCCESS;
}


/* ------------------------------------------------------------------------- */
/* Spartan-II Slave Parallel Generic Implementation */

static int Spartan3_sp_load(Xilinx_desc *desc, const void *buf, size_t bsize)
{
	int ret_val = FPGA_FAIL;	/* assume the worst */
	Xilinx_Spartan3_Slave_Parallel_fns *fn = desc->iface_fns;

	PRINTF ("%s: start with interface functions @ 0x%p\n",
			__FUNCTION__, fn);

	if (fn) {
		size_t bytecount = 0;
		unsigned char *data = (unsigned char *) buf;
		int cookie = desc->cookie;	/* make a local copy */
		unsigned long ts;		/* timestamp */

		PRINTF ("%s: Function Table:\n"
				"ptr:\t0x%p\n"
				"struct: 0x%p\n"
				"pre: 0x%p\n"
				"pgm:\t0x%p\n"
				"init:\t0x%p\n"
				"err:\t0x%p\n"
				"clk:\t0x%p\n"
				"cs:\t0x%p\n"
				"wr:\t0x%p\n"
				"read data:\t0x%p\n"
				"write data:\t0x%p\n"
				"busy:\t0x%p\n"
				"abort:\t0x%p\n",
				"post:\t0x%p\n\n",
				__FUNCTION__, &fn, fn, fn->pre, fn->pgm, fn->init, fn->err,
				fn->clk, fn->cs, fn->wr, fn->rdata, fn->wdata, fn->busy,
				fn->abort, fn->post);

		/*
		 * This code is designed to emulate the "Express Style"
		 * Continuous Data Loading in Slave Parallel Mode for
		 * the Spartan-II Family.
		 */
#ifdef CONFIG_SYS_FPGA_PROG_FEEDBACK
		printf ("Loading FPGA Device %d...\n", cookie);
#endif
		/*
		 * Run the pre configuration function if there is one.
		 */
		if (*fn->pre) {
			(*fn->pre) (cookie);
		}

		/* Establish the initial state */
		(*fn->pgm) (TRUE, TRUE, cookie);	/* Assert the program, commit */

		/* Get ready for the burn */
		CONFIG_FPGA_DELAY ();
		(*fn->pgm) (FALSE, TRUE, cookie);	/* Deassert the program, commit */

		ts = get_timer (0);		/* get current time */
		/* Now wait for INIT and BUSY to go high */
		do {
			CONFIG_FPGA_DELAY ();
			if (get_timer (ts) > CONFIG_SYS_FPGA_WAIT) {	/* check the time */
				puts ("** Timeout waiting for INIT to clear.\n");
				(*fn->abort) (cookie);	/* abort the burn */
				return FPGA_FAIL;
			}
		} while ((*fn->init) (cookie) && (*fn->busy) (cookie));

		(*fn->wr) (TRUE, TRUE, cookie); /* Assert write, commit */
		(*fn->cs) (TRUE, TRUE, cookie); /* Assert chip select, commit */
		(*fn->clk) (TRUE, TRUE, cookie);	/* Assert the clock pin */

		/* Load the data */
		while (bytecount < bsize) {
			/* XXX - do we check for an Ctrl-C press in here ??? */
			/* XXX - Check the error bit? */

			(*fn->wdata) (data[bytecount++], TRUE, cookie); /* write the data */
			CONFIG_FPGA_DELAY ();
			(*fn->clk) (FALSE, TRUE, cookie);	/* Deassert the clock pin */
			CONFIG_FPGA_DELAY ();
			(*fn->clk) (TRUE, TRUE, cookie);	/* Assert the clock pin */

#ifdef CONFIG_SYS_FPGA_CHECK_BUSY
			ts = get_timer (0);	/* get current time */
			while ((*fn->busy) (cookie)) {
				/* XXX - we should have a check in here somewhere to
				 * make sure we aren't busy forever... */

				CONFIG_FPGA_DELAY ();
				(*fn->clk) (FALSE, TRUE, cookie);	/* Deassert the clock pin */
				CONFIG_FPGA_DELAY ();
				(*fn->clk) (TRUE, TRUE, cookie);	/* Assert the clock pin */

				if (get_timer (ts) > CONFIG_SYS_FPGA_WAIT) {	/* check the time */
					puts ("** Timeout waiting for BUSY to clear.\n");
					(*fn->abort) (cookie);	/* abort the burn */
					return FPGA_FAIL;
				}
			}
#endif

#ifdef CONFIG_SYS_FPGA_PROG_FEEDBACK
			if (bytecount % (bsize / 40) == 0)
				putc ('.');		/* let them know we are alive */
#endif
		}

		CONFIG_FPGA_DELAY ();
		(*fn->cs) (FALSE, TRUE, cookie);	/* Deassert the chip select */
		(*fn->wr) (FALSE, TRUE, cookie);	/* Deassert the write pin */

#ifdef CONFIG_SYS_FPGA_PROG_FEEDBACK
		putc ('\n');			/* terminate the dotted line */
#endif

		/* now check for done signal */
		ts = get_timer (0);		/* get current time */
		ret_val = FPGA_SUCCESS;
		while ((*fn->done) (cookie) == FPGA_FAIL) {
			/* XXX - we should have a check in here somewhere to
			 * make sure we aren't busy forever... */

			CONFIG_FPGA_DELAY ();
			(*fn->clk) (FALSE, TRUE, cookie);	/* Deassert the clock pin */
			CONFIG_FPGA_DELAY ();
			(*fn->clk) (TRUE, TRUE, cookie);	/* Assert the clock pin */

			if (get_timer (ts) > CONFIG_SYS_FPGA_WAIT) {	/* check the time */
				puts ("** Timeout waiting for DONE to clear.\n");
				(*fn->abort) (cookie);	/* abort the burn */
				ret_val = FPGA_FAIL;
				break;
			}
		}

		/*
		 * Run the post configuration function if there is one.
		 */
		if (*fn->post)
			(*fn->post) (cookie);

#ifdef CONFIG_SYS_FPGA_PROG_FEEDBACK
		if (ret_val == FPGA_SUCCESS)
			puts ("Done.\n");
		else
			puts ("Fail.\n");
#endif

	} else {
		printf ("%s: NULL Interface function table!\n", __FUNCTION__);
	}

	return ret_val;
}

static int Spartan3_sp_dump(Xilinx_desc *desc, const void *buf, size_t bsize)
{
	int ret_val = FPGA_FAIL;	/* assume the worst */
	Xilinx_Spartan3_Slave_Parallel_fns *fn = desc->iface_fns;

	if (fn) {
		unsigned char *data = (unsigned char *) buf;
		size_t bytecount = 0;
		int cookie = desc->cookie;	/* make a local copy */

		printf ("Starting Dump of FPGA Device %d...\n", cookie);

		(*fn->cs) (TRUE, TRUE, cookie); /* Assert chip select, commit */
		(*fn->clk) (TRUE, TRUE, cookie);	/* Assert the clock pin */

		/* dump the data */
		while (bytecount < bsize) {
			/* XXX - do we check for an Ctrl-C press in here ??? */

			(*fn->clk) (FALSE, TRUE, cookie);	/* Deassert the clock pin */
			(*fn->clk) (TRUE, TRUE, cookie);	/* Assert the clock pin */
			(*fn->rdata) (&(data[bytecount++]), cookie);	/* read the data */
#ifdef CONFIG_SYS_FPGA_PROG_FEEDBACK
			if (bytecount % (bsize / 40) == 0)
				putc ('.');		/* let them know we are alive */
#endif
		}

		(*fn->cs) (FALSE, FALSE, cookie);	/* Deassert the chip select */
		(*fn->clk) (FALSE, TRUE, cookie);	/* Deassert the clock pin */
		(*fn->clk) (TRUE, TRUE, cookie);	/* Assert the clock pin */

#ifdef CONFIG_SYS_FPGA_PROG_FEEDBACK
		putc ('\n');			/* terminate the dotted line */
#endif
		puts ("Done.\n");

		/* XXX - checksum the data? */
	} else {
		printf ("%s: NULL Interface function table!\n", __FUNCTION__);
	}

	return ret_val;
}


/* ------------------------------------------------------------------------- */

static int Spartan3_ss_load(Xilinx_desc *desc, const void *buf, size_t bsize)
{
	int ret_val = FPGA_FAIL;	/* assume the worst */
	Xilinx_Spartan3_Slave_Serial_fns *fn = desc->iface_fns;
	int i;
	unsigned char val;

	PRINTF ("%s: start with interface functions @ 0x%p\n",
			__FUNCTION__, fn);

	if (fn) {
		size_t bytecount = 0;
		unsigned char *data = (unsigned char *) buf;
		int cookie = desc->cookie;	/* make a local copy */
		unsigned long ts;		/* timestamp */

		PRINTF ("%s: Function Table:\n"
				"ptr:\t0x%p\n"
				"struct: 0x%p\n"
				"pgm:\t0x%p\n"
				"init:\t0x%p\n"
				"clk:\t0x%p\n"
				"wr:\t0x%p\n"
				"done:\t0x%p\n\n",
				__FUNCTION__, &fn, fn, fn->pgm, fn->init,
				fn->clk, fn->wr, fn->done);
#ifdef CONFIG_SYS_FPGA_PROG_FEEDBACK
		printf ("Loading FPGA Device %d...\n", cookie);
#endif

		/*
		 * Run the pre configuration function if there is one.
		 */
		if (*fn->pre) {
			(*fn->pre) (cookie);
		}

		/* Establish the initial state */
		(*fn->pgm) (TRUE, TRUE, cookie);	/* Assert the program, commit */

		/* Wait for INIT state (init low)                            */
		ts = get_timer (0);		/* get current time */
		do {
			CONFIG_FPGA_DELAY ();
			if (get_timer (ts) > CONFIG_SYS_FPGA_WAIT) {	/* check the time */
				puts ("** Timeout waiting for INIT to start.\n");
				if (*fn->abort)
					(*fn->abort) (cookie);
				return FPGA_FAIL;
			}
		} while (!(*fn->init) (cookie));

		/* Get ready for the burn */
		CONFIG_FPGA_DELAY ();
		(*fn->pgm) (FALSE, TRUE, cookie);	/* Deassert the program, commit */

		ts = get_timer (0);		/* get current time */
		/* Now wait for INIT to go high */
		do {
			CONFIG_FPGA_DELAY ();
			if (get_timer (ts) > CONFIG_SYS_FPGA_WAIT) {	/* check the time */
				puts ("** Timeout waiting for INIT to clear.\n");
				if (*fn->abort)
					(*fn->abort) (cookie);
				return FPGA_FAIL;
			}
		} while ((*fn->init) (cookie));

		/* Load the data */
		if(*fn->bwr)
			(*fn->bwr) (data, bsize, TRUE, cookie);
		else {
			while (bytecount < bsize) {

				/* Xilinx detects an error if INIT goes low (active)
				   while DONE is low (inactive) */
				if ((*fn->done) (cookie) == 0 && (*fn->init) (cookie)) {
					puts ("** CRC error during FPGA load.\n");
					if (*fn->abort)
						(*fn->abort) (cookie);
					return (FPGA_FAIL);
				}
				val = data [bytecount ++];
				i = 8;
				do {
					/* Deassert the clock */
					(*fn->clk) (FALSE, TRUE, cookie);
					CONFIG_FPGA_DELAY ();
					/* Write data */
					(*fn->wr) ((val & 0x80), TRUE, cookie);
					CONFIG_FPGA_DELAY ();
					/* Assert the clock */
					(*fn->clk) (TRUE, TRUE, cookie);
					CONFIG_FPGA_DELAY ();
					val <<= 1;
					i --;
				} while (i > 0);

#ifdef CONFIG_SYS_FPGA_PROG_FEEDBACK
				if (bytecount % (bsize / 40) == 0)
					putc ('.');		/* let them know we are alive */
#endif
			}
		}

		CONFIG_FPGA_DELAY ();

#ifdef CONFIG_SYS_FPGA_PROG_FEEDBACK
		putc ('\n');			/* terminate the dotted line */
#endif

		/* now check for done signal */
		ts = get_timer (0);		/* get current time */
		ret_val = FPGA_SUCCESS;
		(*fn->wr) (TRUE, TRUE, cookie);

		while (! (*fn->done) (cookie)) {
			/* XXX - we should have a check in here somewhere to
			 * make sure we aren't busy forever... */

			CONFIG_FPGA_DELAY ();
			(*fn->clk) (FALSE, TRUE, cookie);	/* Deassert the clock pin */
			CONFIG_FPGA_DELAY ();
			(*fn->clk) (TRUE, TRUE, cookie);	/* Assert the clock pin */

			putc ('*');

			if (get_timer (ts) > CONFIG_SYS_FPGA_WAIT) {	/* check the time */
				puts ("** Timeout waiting for DONE to clear.\n");
				ret_val = FPGA_FAIL;
				break;
			}
		}
		putc ('\n');			/* terminate the dotted line */

		/*
		 * Run the post configuration function if there is one.
		 */
		if (*fn->post)
			(*fn->post) (cookie);

#ifdef CONFIG_SYS_FPGA_PROG_FEEDBACK
		if (ret_val == FPGA_SUCCESS)
			puts ("Done.\n");
		else
			puts ("Fail.\n");
#endif

	} else {
		printf ("%s: NULL Interface function table!\n", __FUNCTION__);
	}

	return ret_val;
}

static int Spartan3_ss_dump(Xilinx_desc *desc, const void *buf, size_t bsize)
{
	/* Readback is only available through the Slave Parallel and         */
	/* boundary-scan interfaces.                                         */
	printf ("%s: Slave Serial Dumping is unavailable\n",
			__FUNCTION__);
	return FPGA_FAIL;
}
