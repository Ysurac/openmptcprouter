#ifndef _PPC_PTRACE_H
#define _PPC_PTRACE_H

/*
 * This struct defines the way the registers are stored on the
 * kernel stack during a system call or other kernel entry.
 *
 * this should only contain volatile regs
 * since we can keep non-volatile in the thread_struct
 * should set this up when only volatiles are saved
 * by intr code.
 *
 * Since this is going on the stack, *CARE MUST BE TAKEN* to insure
 * that the overall structure is a multiple of 16 bytes in length.
 *
 * Note that the offsets of the fields in this struct correspond with
 * the PT_* values below.  This simplifies arch/powerpc/kernel/ptrace.c.
 */

#include <linux/config.h>

#ifndef __ASSEMBLY__
#ifdef CONFIG_PPC64BRIDGE
#define PPC_REG unsigned long /*long*/
#else
#define PPC_REG unsigned long
#endif
struct pt_regs {
	PPC_REG gpr[32];
	PPC_REG nip;
	PPC_REG msr;
	PPC_REG orig_gpr3;	/* Used for restarting system calls */
	PPC_REG ctr;
	PPC_REG link;
	PPC_REG xer;
	PPC_REG ccr;
	PPC_REG mq;		/* 601 only (not used at present) */
				/* Used on APUS to hold IPL value. */
	PPC_REG trap;		/* Reason for being here */
	PPC_REG dar;		/* Fault registers */
	PPC_REG dsisr;
	PPC_REG result;		/* Result of a system call */
};
#endif

#define STACK_FRAME_OVERHEAD	16	/* size of minimum stack frame */

/* Size of stack frame allocated when calling signal handler. */
#define __SIGNAL_FRAMESIZE	64

#define instruction_pointer(regs) ((regs)->nip)
#define user_mode(regs) (((regs)->msr & MSR_PR) != 0)

/*
 * Offsets used by 'ptrace' system call interface.
 * These can't be changed without breaking binary compatibility
 * with MkLinux, etc.
 */
#define PT_R0	0
#define PT_R1	1
#define PT_R2	2
#define PT_R3	3
#define PT_R4	4
#define PT_R5	5
#define PT_R6	6
#define PT_R7	7
#define PT_R8	8
#define PT_R9	9
#define PT_R10	10
#define PT_R11	11
#define PT_R12	12
#define PT_R13	13
#define PT_R14	14
#define PT_R15	15
#define PT_R16	16
#define PT_R17	17
#define PT_R18	18
#define PT_R19	19
#define PT_R20	20
#define PT_R21	21
#define PT_R22	22
#define PT_R23	23
#define PT_R24	24
#define PT_R25	25
#define PT_R26	26
#define PT_R27	27
#define PT_R28	28
#define PT_R29	29
#define PT_R30	30
#define PT_R31	31

#define PT_NIP	32
#define PT_MSR	33
#ifdef __KERNEL__
#define PT_ORIG_R3 34
#endif
#define PT_CTR	35
#define PT_LNK	36
#define PT_XER	37
#define PT_CCR	38
#define PT_MQ	39

#define PT_FPR0	48	/* each FP reg occupies 2 slots in this space */
#define PT_FPR31 (PT_FPR0 + 2*31)
#define PT_FPSCR (PT_FPR0 + 2*32 + 1)

#endif
