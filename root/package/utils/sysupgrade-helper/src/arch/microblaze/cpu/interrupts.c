/*
 * (C) Copyright 2007 Michal Simek
 * (C) Copyright 2004 Atmark Techno, Inc.
 *
 * Michal  SIMEK <monstr@monstr.eu>
 * Yasushi SHOJI <yashi@atmark-techno.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <command.h>
#include <asm/microblaze_intc.h>
#include <asm/asm.h>

#undef DEBUG_INT

extern void microblaze_disable_interrupts (void);
extern void microblaze_enable_interrupts (void);

void enable_interrupts (void)
{
	MSRSET(0x2);
}

int disable_interrupts (void)
{
	unsigned int msr;

	MFS(msr, rmsr);
	MSRCLR(0x2);
	return (msr & 0x2) != 0;
}

#ifdef CONFIG_SYS_INTC_0

static struct irq_action vecs[CONFIG_SYS_INTC_0_NUM];

/* mapping structure to interrupt controller */
microblaze_intc_t *intc = (microblaze_intc_t *) (CONFIG_SYS_INTC_0_ADDR);

/* default handler */
void def_hdlr (void)
{
	puts ("def_hdlr\n");
}

void enable_one_interrupt (int irq)
{
	int mask;
	int offset = 1;
	offset <<= irq;
	mask = intc->ier;
	intc->ier = (mask | offset);
#ifdef DEBUG_INT
	printf ("Enable one interrupt irq %x - mask %x,ier %x\n", offset, mask,
		intc->ier);
	printf ("INTC isr %x, ier %x, iar %x, mer %x\n", intc->isr, intc->ier,
		intc->iar, intc->mer);
#endif
}

void disable_one_interrupt (int irq)
{
	int mask;
	int offset = 1;
	offset <<= irq;
	mask = intc->ier;
	intc->ier = (mask & ~offset);
#ifdef DEBUG_INT
	printf ("Disable one interrupt irq %x - mask %x,ier %x\n", irq, mask,
		intc->ier);
	printf ("INTC isr %x, ier %x, iar %x, mer %x\n", intc->isr, intc->ier,
		intc->iar, intc->mer);
#endif
}

/* adding new handler for interrupt */
void install_interrupt_handler (int irq, interrupt_handler_t * hdlr, void *arg)
{
	struct irq_action *act;
	/* irq out of range */
	if ((irq < 0) || (irq > CONFIG_SYS_INTC_0_NUM)) {
		puts ("IRQ out of range\n");
		return;
	}
	act = &vecs[irq];
	if (hdlr) {		/* enable */
		act->handler = hdlr;
		act->arg = arg;
		act->count = 0;
		enable_one_interrupt (irq);
	} else {		/* disable */
		act->handler = (interrupt_handler_t *) def_hdlr;
		act->arg = (void *)irq;
		disable_one_interrupt (irq);
	}
}

/* initialization interrupt controller - hardware */
void intc_init (void)
{
	intc->mer = 0;
	intc->ier = 0;
	intc->iar = 0xFFFFFFFF;
	/* XIntc_Start - hw_interrupt enable and all interrupt enable */
	intc->mer = 0x3;
#ifdef DEBUG_INT
	printf ("INTC isr %x, ier %x, iar %x, mer %x\n", intc->isr, intc->ier,
		intc->iar, intc->mer);
#endif
}

int interrupts_init (void)
{
	int i;
	/* initialize irq list */
	for (i = 0; i < CONFIG_SYS_INTC_0_NUM; i++) {
		vecs[i].handler = (interrupt_handler_t *) def_hdlr;
		vecs[i].arg = (void *)i;
		vecs[i].count = 0;
	}
	/* initialize intc controller */
	intc_init ();
	enable_interrupts ();
	return 0;
}

void interrupt_handler (void)
{
	int irqs = intc->ivr;	/* find active interrupt */
	int mask = 1;
#ifdef DEBUG_INT
	int value;
	printf ("INTC isr %x, ier %x, iar %x, mer %x\n", intc->isr, intc->ier,
		intc->iar, intc->mer);
	R14(value);
	printf ("Interrupt handler on %x line, r14 %x\n", irqs, value);
#endif
	struct irq_action *act = vecs + irqs;

#ifdef DEBUG_INT
	printf
	    ("Jumping to interrupt handler rutine addr %x,count %x,arg %x\n",
	     act->handler, act->count, act->arg);
#endif
	act->handler (act->arg);
	act->count++;

	intc->iar = mask << irqs;

#ifdef DEBUG_INT
	printf ("Dump INTC reg, isr %x, ier %x, iar %x, mer %x\n", intc->isr,
		intc->ier, intc->iar, intc->mer);
	R14(value);
	printf ("Interrupt handler on %x line, r14 %x\n", irqs, value);
#endif
}
#endif

#if defined(CONFIG_CMD_IRQ)
#ifdef CONFIG_SYS_INTC_0
int do_irqinfo (cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	int i;
	struct irq_action *act = vecs;

	puts ("\nInterrupt-Information:\n\n"
	      "Nr  Routine   Arg       Count\n"
	      "-----------------------------\n");

	for (i = 0; i < CONFIG_SYS_INTC_0_NUM; i++) {
		if (act->handler != (interrupt_handler_t*) def_hdlr) {
			printf ("%02d  %08x  %08x  %d\n", i,
				(int)act->handler, (int)act->arg, act->count);
		}
		act++;
	}
	puts ("\n");
	return (0);
}
#else
int do_irqinfo (cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	puts ("Undefined interrupt controller\n");
}
#endif
#endif
