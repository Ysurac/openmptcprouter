/*
 * (C) Copyright 2000-2002
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
#include <mpc8xx.h>
#include <mpc8xx_irq.h>
#include <asm/processor.h>
#include <commproc.h>

/************************************************************************/

/*
 * CPM interrupt vector functions.
 */
struct interrupt_action {
	interrupt_handler_t *handler;
	void *arg;
};

static struct interrupt_action cpm_vecs[CPMVEC_NR];
static struct interrupt_action irq_vecs[NR_IRQS];

static void cpm_interrupt_init (void);
static void cpm_interrupt (void *regs);

/************************************************************************/

int interrupt_init_cpu (unsigned *decrementer_count)
{
	volatile immap_t *immr = (immap_t *) CONFIG_SYS_IMMR;

	*decrementer_count = get_tbclk () / CONFIG_SYS_HZ;

	/* disable all interrupts */
	immr->im_siu_conf.sc_simask = 0;

	/* Configure CPM interrupts */
	cpm_interrupt_init ();

	return (0);
}

/************************************************************************/

/*
 * Handle external interrupts
 */
void external_interrupt (struct pt_regs *regs)
{
	volatile immap_t *immr = (immap_t *) CONFIG_SYS_IMMR;
	int irq;
	ulong simask, newmask;
	ulong vec, v_bit;

	/*
	 * read the SIVEC register and shift the bits down
	 * to get the irq number
	 */
	vec = immr->im_siu_conf.sc_sivec;
	irq = vec >> 26;
	v_bit = 0x80000000UL >> irq;

	/*
	 * Read Interrupt Mask Register and Mask Interrupts
	 */
	simask = immr->im_siu_conf.sc_simask;
	newmask = simask & (~(0xFFFF0000 >> irq));
	immr->im_siu_conf.sc_simask = newmask;

	if (!(irq & 0x1)) {		/* External Interrupt ?     */
		ulong siel;

		/*
		 * Read Interrupt Edge/Level Register
		 */
		siel = immr->im_siu_conf.sc_siel;

		if (siel & v_bit) {	/* edge triggered interrupt ?   */
			/*
			 * Rewrite SIPEND Register to clear interrupt
			 */
			immr->im_siu_conf.sc_sipend = v_bit;
		}
	}

	if (irq_vecs[irq].handler != NULL) {
		irq_vecs[irq].handler (irq_vecs[irq].arg);
	} else {
		printf ("\nBogus External Interrupt IRQ %d Vector %ld\n",
				irq, vec);
		/* turn off the bogus interrupt to avoid it from now */
		simask &= ~v_bit;
	}
	/*
	 * Re-Enable old Interrupt Mask
	 */
	immr->im_siu_conf.sc_simask = simask;
}

/************************************************************************/

/*
 * CPM interrupt handler
 */
static void cpm_interrupt (void *regs)
{
	volatile immap_t *immr = (immap_t *) CONFIG_SYS_IMMR;
	uint vec;

	/*
	 * Get the vector by setting the ACK bit
	 * and then reading the register.
	 */
	immr->im_cpic.cpic_civr = 1;
	vec = immr->im_cpic.cpic_civr;
	vec >>= 11;

	if (cpm_vecs[vec].handler != NULL) {
		(*cpm_vecs[vec].handler) (cpm_vecs[vec].arg);
	} else {
		immr->im_cpic.cpic_cimr &= ~(1 << vec);
		printf ("Masking bogus CPM interrupt vector 0x%x\n", vec);
	}
	/*
	 * After servicing the interrupt,
	 * we have to remove the status indicator.
	 */
	immr->im_cpic.cpic_cisr |= (1 << vec);
}

/*
 * The CPM can generate the error interrupt when there is a race
 * condition between generating and masking interrupts. All we have
 * to do is ACK it and return. This is a no-op function so we don't
 * need any special tests in the interrupt handler.
 */
static void cpm_error_interrupt (void *dummy)
{
}

/************************************************************************/
/*
 * Install and free an interrupt handler
 */
void irq_install_handler (int vec, interrupt_handler_t * handler,
						  void *arg)
{
	volatile immap_t *immr = (immap_t *) CONFIG_SYS_IMMR;

	if ((vec & CPMVEC_OFFSET) != 0) {
		/* CPM interrupt */
		vec &= 0xffff;
		if (cpm_vecs[vec].handler != NULL) {
			printf ("CPM interrupt 0x%x replacing 0x%x\n",
				(uint) handler,
				(uint) cpm_vecs[vec].handler);
		}
		cpm_vecs[vec].handler = handler;
		cpm_vecs[vec].arg = arg;
		immr->im_cpic.cpic_cimr |= (1 << vec);
#if 0
		printf ("Install CPM interrupt for vector %d ==> %p\n",
			vec, handler);
#endif
	} else {
		/* SIU interrupt */
		if (irq_vecs[vec].handler != NULL) {
			printf ("SIU interrupt %d 0x%x replacing 0x%x\n",
				vec,
				(uint) handler,
				(uint) cpm_vecs[vec].handler);
		}
		irq_vecs[vec].handler = handler;
		irq_vecs[vec].arg = arg;
		immr->im_siu_conf.sc_simask |= 1 << (31 - vec);
#if 0
		printf ("Install SIU interrupt for vector %d ==> %p\n",
			vec, handler);
#endif
	}
}

void irq_free_handler (int vec)
{
	volatile immap_t *immr = (immap_t *) CONFIG_SYS_IMMR;

	if ((vec & CPMVEC_OFFSET) != 0) {
		/* CPM interrupt */
		vec &= 0xffff;
#if 0
		printf ("Free CPM interrupt for vector %d ==> %p\n",
			vec, cpm_vecs[vec].handler);
#endif
		immr->im_cpic.cpic_cimr &= ~(1 << vec);
		cpm_vecs[vec].handler = NULL;
		cpm_vecs[vec].arg = NULL;
	} else {
		/* SIU interrupt */
#if 0
		printf ("Free CPM interrupt for vector %d ==> %p\n",
			vec, cpm_vecs[vec].handler);
#endif
		immr->im_siu_conf.sc_simask &= ~(1 << (31 - vec));
		irq_vecs[vec].handler = NULL;
		irq_vecs[vec].arg = NULL;
	}
}

/************************************************************************/

static void cpm_interrupt_init (void)
{
	volatile immap_t *immr = (immap_t *) CONFIG_SYS_IMMR;

	/*
	 * Initialize the CPM interrupt controller.
	 */

	immr->im_cpic.cpic_cicr =
		(CICR_SCD_SCC4 |
		 CICR_SCC_SCC3 |
		 CICR_SCB_SCC2 |
		 CICR_SCA_SCC1) | ((CPM_INTERRUPT / 2) << 13) | CICR_HP_MASK;

	immr->im_cpic.cpic_cimr = 0;

	/*
	 * Install the error handler.
	 */
	irq_install_handler (CPMVEC_ERROR, cpm_error_interrupt, NULL);

	immr->im_cpic.cpic_cicr |= CICR_IEN;

	/*
	 * Install the cpm interrupt handler
	 */
	irq_install_handler (CPM_INTERRUPT, cpm_interrupt, NULL);
}

/************************************************************************/

/*
 * timer_interrupt - gets called when the decrementer overflows,
 * with interrupts disabled.
 * Trivial implementation - no need to be really accurate.
 */
void timer_interrupt_cpu (struct pt_regs *regs)
{
	volatile immap_t *immr = (immap_t *) CONFIG_SYS_IMMR;

#if 0
	printf ("*** Timer Interrupt *** ");
#endif
	/* Reset Timer Expired and Timers Interrupt Status */
	immr->im_clkrstk.cark_plprcrk = KAPWR_KEY;
	__asm__ ("nop");
	/*
	  Clear TEXPS (and TMIST on older chips). SPLSS (on older
	  chips) is cleared too.

	  Bitwise OR is a read-modify-write operation so ALL bits
	  which are cleared by writing `1' would be cleared by
	  operations like

	  immr->im_clkrst.car_plprcr |= PLPRCR_TEXPS;

	  The same can be achieved by simple writing of the PLPRCR
	  to itself. If a bit value should be preserved, read the
	  register, ZERO the bit and write, not OR, the result back.
	*/
	immr->im_clkrst.car_plprcr = immr->im_clkrst.car_plprcr;
}

/************************************************************************/
