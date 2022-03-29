#ifndef __ASM_SH_CACHE_H
#define __ASM_SH_CACHE_H

#if defined(CONFIG_SH4) || defined(CONFIG_SH4A)

int cache_control(unsigned int cmd);

#define L1_CACHE_BYTES 32

struct __large_struct { unsigned long buf[100]; };
#define __m(x) (*(struct __large_struct *)(x))

void dcache_wback_range(u32 start, u32 end);
void dcache_invalid_range(u32 start, u32 end);

#else

/*
 * 32-bytes is the largest L1 data cache line size for SH the architecture.  So
 * it is a safe default for DMA alignment.
 */
#define ARCH_DMA_MINALIGN	32

#endif /* CONFIG_SH4 || CONFIG_SH4A */

/*
 * Use the L1 data cache line size value for the minimum DMA buffer alignment
 * on SH.
 */
#ifndef ARCH_DMA_MINALIGN
#define ARCH_DMA_MINALIGN	L1_CACHE_BYTES
#endif

#endif	/* __ASM_SH_CACHE_H */
