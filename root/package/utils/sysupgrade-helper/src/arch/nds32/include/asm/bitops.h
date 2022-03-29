/*
 * Copyright 1995, Russell King.
 * Various bits and pieces copyrights include:
 *  Linus Torvalds (test_bit).
 *
 * Copyright (C) 2011 Andes Technology Corporation
 * Shawn Lin, Andes Technology Corporation <nobuhiro@andestech.com>
 *
 * bit 0 is the LSB of addr; bit 32 is the LSB of (addr+1).
 *
 * Please note that the code in this file should never be included
 * from user space.  Many of these are not implemented in assembler
 * since they would be too costly.  Also, they require priviledged
 * instructions (which are not available from user mode) to ensure
 * that they are atomic.
 */

#ifndef __ASM_NDS_BITOPS_H
#define __ASM_NDS_BITOPS_H

#ifdef __KERNEL__

#include <asm/system.h>

#define smp_mb__before_clear_bit()	do { } while (0)
#define smp_mb__after_clear_bit()	do { } while (0)

/*
 * Function prototypes to keep gcc -Wall happy.
 */
extern void set_bit(int nr, void *addr);

static inline void __set_bit(int nr, void *addr)
{
	int *a = (int *)addr;
	int mask;

	a += nr >> 5;
	mask = 1 << (nr & 0x1f);
	*a |= mask;
}

extern void clear_bit(int nr, void *addr);

static inline void __clear_bit(int nr, void *addr)
{
	int *a = (int *)addr;
	int mask;
	unsigned long flags;

	a += nr >> 5;
	mask = 1 << (nr & 0x1f);
	local_irq_save(flags);
	*a &= ~mask;
	local_irq_restore(flags);
}

extern void change_bit(int nr, void *addr);

static inline void __change_bit(int nr, void *addr)
{
	int mask;
	unsigned long *ADDR = (unsigned long *)addr;

	ADDR += nr >> 5;
	mask = 1 << (nr & 31);
	*ADDR ^= mask;
}

extern int test_and_set_bit(int nr, void *addr);

static inline int __test_and_set_bit(int nr, void *addr)
{
	int mask, retval;
	unsigned int *a = (unsigned int *)addr;

	a += nr >> 5;
	mask = 1 << (nr & 0x1f);
	retval = (mask & *a) != 0;
	*a |= mask;
	return retval;
}

extern int test_and_clear_bit(int nr, void *addr);

static inline int __test_and_clear_bit(int nr, void *addr)
{
	int mask, retval;
	unsigned int *a = (unsigned int *)addr;

	a += nr >> 5;
	mask = 1 << (nr & 0x1f);
	retval = (mask & *a) != 0;
	*a &= ~mask;
	return retval;
}

extern int test_and_change_bit(int nr, void *addr);

static inline int __test_and_change_bit(int nr, void *addr)
{
	int mask, retval;
	unsigned int *a = (unsigned int *)addr;

	a += nr >> 5;
	mask = 1 << (nr & 0x1f);
	retval = (mask & *a) != 0;
	*a ^= mask;
	return retval;
}

extern int find_first_zero_bit(void *addr, unsigned size);
extern int find_next_zero_bit(void *addr, int size, int offset);

/*
 * This routine doesn't need to be atomic.
 */
static inline int test_bit(int nr, const void *addr)
{
	return ((unsigned char *) addr)[nr >> 3] & (1U << (nr & 7));
}

/*
 * ffz = Find First Zero in word. Undefined if no zero exists,
 * so code should check against ~0UL first..
 */
static inline unsigned long ffz(unsigned long word)
{
	int k;

	word = ~word;
	k = 31;
	if (word & 0x0000ffff) {
		k -= 16; word <<= 16;
	}
	if (word & 0x00ff0000) {
		k -= 8;  word <<= 8;
	}
	if (word & 0x0f000000) {
		k -= 4;  word <<= 4;
	}
	if (word & 0x30000000) {
		k -= 2;  word <<= 2;
	}
	if (word & 0x40000000)
		k -= 1;

	return k;
}

/*
 * ffs: find first bit set. This is defined the same way as
 * the libc and compiler builtin ffs routines, therefore
 * differs in spirit from the above ffz (man ffs).
 */

/*
 * redefined in include/linux/bitops.h
 * #define ffs(x) generic_ffs(x)
 */

/*
 * hweightN: returns the hamming weight (i.e. the number
 * of bits set) of a N-bit word
 */

#define hweight32(x) generic_hweight32(x)
#define hweight16(x) generic_hweight16(x)
#define hweight8(x) generic_hweight8(x)

#define ext2_set_bit			test_and_set_bit
#define ext2_clear_bit			test_and_clear_bit
#define ext2_test_bit			test_bit
#define ext2_find_first_zero_bit	find_first_zero_bit
#define ext2_find_next_zero_bit		find_next_zero_bit

/* Bitmap functions for the minix filesystem. */
#define minix_test_and_set_bit(nr, addr)	test_and_set_bit(nr, addr)
#define minix_set_bit(nr, addr)			set_bit(nr, addr)
#define minix_test_and_clear_bit(nr, addr)	test_and_clear_bit(nr, addr)
#define minix_test_bit(nr, addr)		test_bit(nr, addr)
#define minix_find_first_zero_bit(addr, size)	find_first_zero_bit(addr, size)

#endif /* __KERNEL__ */

#endif /* __ASM_NDS_BITOPS_H */
