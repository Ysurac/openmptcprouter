/*
 * (C) Copyright 2002
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

#include <common.h>

/*
 * CPU test
 * Shift instructions:		rlwnm
 *
 * The test contains a pre-built table of instructions, operands and
 * expected results. For each table entry, the test will cyclically use
 * different sets of operand registers and result registers.
 */

#include <post.h>
#include "cpu_asm.h"

#if CONFIG_POST & CONFIG_SYS_POST_CPU

extern void cpu_post_exec_22 (ulong *code, ulong *cr, ulong *res, ulong op1,
    ulong op2);
extern ulong cpu_post_makecr (long v);

static struct cpu_post_rlwnm_s
{
    ulong cmd;
    ulong op1;
    ulong op2;
    uchar mb;
    uchar me;
    ulong res;
} cpu_post_rlwnm_table[] =
{
   {
	OP_RLWNM,
	0xffff0000,
	24,
	16,
	23,
	0x0000ff00
   },
};
static unsigned int cpu_post_rlwnm_size = ARRAY_SIZE(cpu_post_rlwnm_table);

int cpu_post_test_rlwnm (void)
{
    int ret = 0;
    unsigned int i, reg;
    int flag = disable_interrupts();

    for (i = 0; i < cpu_post_rlwnm_size && ret == 0; i++)
    {
	struct cpu_post_rlwnm_s *test = cpu_post_rlwnm_table + i;

	for (reg = 0; reg < 32 && ret == 0; reg++)
	{
	    unsigned int reg0 = (reg + 0) % 32;
	    unsigned int reg1 = (reg + 1) % 32;
	    unsigned int reg2 = (reg + 2) % 32;
	    unsigned int stk = reg < 16 ? 31 : 15;
	    unsigned long code[] =
	    {
		ASM_STW(stk, 1, -4),
		ASM_ADDI(stk, 1, -24),
		ASM_STW(3, stk, 12),
		ASM_STW(4, stk, 16),
		ASM_STW(reg0, stk, 8),
		ASM_STW(reg1, stk, 4),
		ASM_STW(reg2, stk, 0),
		ASM_LWZ(reg1, stk, 12),
		ASM_LWZ(reg0, stk, 16),
		ASM_122(test->cmd, reg2, reg1, reg0, test->mb, test->me),
		ASM_STW(reg2, stk, 12),
		ASM_LWZ(reg2, stk, 0),
		ASM_LWZ(reg1, stk, 4),
		ASM_LWZ(reg0, stk, 8),
		ASM_LWZ(3, stk, 12),
		ASM_ADDI(1, stk, 24),
		ASM_LWZ(stk, 1, -4),
		ASM_BLR,
	    };
	    unsigned long codecr[] =
	    {
		ASM_STW(stk, 1, -4),
		ASM_ADDI(stk, 1, -24),
		ASM_STW(3, stk, 12),
		ASM_STW(4, stk, 16),
		ASM_STW(reg0, stk, 8),
		ASM_STW(reg1, stk, 4),
		ASM_STW(reg2, stk, 0),
		ASM_LWZ(reg1, stk, 12),
		ASM_LWZ(reg0, stk, 16),
		ASM_122(test->cmd, reg2, reg1, reg0, test->mb, test->me) |
		    BIT_C,
		ASM_STW(reg2, stk, 12),
		ASM_LWZ(reg2, stk, 0),
		ASM_LWZ(reg1, stk, 4),
		ASM_LWZ(reg0, stk, 8),
		ASM_LWZ(3, stk, 12),
		ASM_ADDI(1, stk, 24),
		ASM_LWZ(stk, 1, -4),
		ASM_BLR,
	    };
	    ulong res;
	    ulong cr;

	    if (ret == 0)
	    {
		cr = 0;
		cpu_post_exec_22 (code, & cr, & res, test->op1, test->op2);

		ret = res == test->res && cr == 0 ? 0 : -1;

		if (ret != 0)
		{
	            post_log ("Error at rlwnm test %d !\n", i);
		}
	    }

	    if (ret == 0)
	    {
		cpu_post_exec_22 (codecr, & cr, & res, test->op1, test->op2);

		ret = res == test->res &&
		      (cr & 0xe0000000) == cpu_post_makecr (res) ? 0 : -1;

		if (ret != 0)
		{
	            post_log ("Error at rlwnm test %d !\n", i);
	        }
	    }
	}
    }

    if (flag)
	enable_interrupts();

    return ret;
}

#endif
