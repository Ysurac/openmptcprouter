/*
 * (C) Copyright 2004
 * Pantelis Antoniou, Intracom S.A. , panto@intracom.gr
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

/*
 * DSP test
 *
 * This test verifies the connection and performs a memory test
 * on any connected DSP(s). The meat of the work is done
 * in the board specific function.
 */

#include <post.h>

#if CONFIG_POST & CONFIG_SYS_POST_DSP

extern int board_post_dsp(int flags);

int dsp_post_test (int flags)
{
	return board_post_dsp(flags);
}

#endif /* CONFIG_POST & CONFIG_SYS_POST_DSP */
