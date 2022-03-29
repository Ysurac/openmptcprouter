/*
 * MPC8260 I/O port pin manipulation functions
 */

#ifndef _ASM_IOPIN_8260_H_
#define _ASM_IOPIN_8260_H_

#include <linux/types.h>
#include <asm/immap_8260.h>

#ifdef __KERNEL__

typedef
    struct {
	u_char port:2;	/* port number (A=0, B=1, C=2, D=3) */
	u_char pin:5;	/* port pin (0-31) */
	u_char flag:1;	/* for whatever */
    }
iopin_t;

#define IOPIN_PORTA	0
#define IOPIN_PORTB	1
#define IOPIN_PORTC	2
#define IOPIN_PORTD	3

extern __inline__ void
iopin_set_high(iopin_t *iopin)
{
    volatile uint *datp = &((immap_t *)CONFIG_SYS_IMMR)->im_ioport.iop_pdata;
    datp[iopin->port * 8] |= (1 << (31 - iopin->pin));
}

extern __inline__ void
iopin_set_low(iopin_t *iopin)
{
    volatile uint *datp = &((immap_t *)CONFIG_SYS_IMMR)->im_ioport.iop_pdata;
    datp[iopin->port * 8] &= ~(1 << (31 - iopin->pin));
}

extern __inline__ uint
iopin_is_high(iopin_t *iopin)
{
    volatile uint *datp = &((immap_t *)CONFIG_SYS_IMMR)->im_ioport.iop_pdata;
    return (datp[iopin->port * 8] >> (31 - iopin->pin)) & 1;
}

extern __inline__ uint
iopin_is_low(iopin_t *iopin)
{
    volatile uint *datp = &((immap_t *)CONFIG_SYS_IMMR)->im_ioport.iop_pdata;
    return ((datp[iopin->port * 8] >> (31 - iopin->pin)) & 1) ^ 1;
}

extern __inline__ void
iopin_set_out(iopin_t *iopin)
{
    volatile uint *dirp = &((immap_t *)CONFIG_SYS_IMMR)->im_ioport.iop_pdira;
    dirp[iopin->port * 8] |= (1 << (31 - iopin->pin));
}

extern __inline__ void
iopin_set_in(iopin_t *iopin)
{
    volatile uint *dirp = &((immap_t *)CONFIG_SYS_IMMR)->im_ioport.iop_pdira;
    dirp[iopin->port * 8] &= ~(1 << (31 - iopin->pin));
}

extern __inline__ uint
iopin_is_out(iopin_t *iopin)
{
    volatile uint *dirp = &((immap_t *)CONFIG_SYS_IMMR)->im_ioport.iop_pdira;
    return (dirp[iopin->port * 8] >> (31 - iopin->pin)) & 1;
}

extern __inline__ uint
iopin_is_in(iopin_t *iopin)
{
    volatile uint *dirp = &((immap_t *)CONFIG_SYS_IMMR)->im_ioport.iop_pdira;
    return ((dirp[iopin->port * 8] >> (31 - iopin->pin)) & 1) ^ 1;
}

extern __inline__ void
iopin_set_odr(iopin_t *iopin)
{
    volatile uint *odrp = &((immap_t *)CONFIG_SYS_IMMR)->im_ioport.iop_podra;
    odrp[iopin->port * 8] |= (1 << (31 - iopin->pin));
}

extern __inline__ void
iopin_set_act(iopin_t *iopin)
{
    volatile uint *odrp = &((immap_t *)CONFIG_SYS_IMMR)->im_ioport.iop_podra;
    odrp[iopin->port * 8] &= ~(1 << (31 - iopin->pin));
}

extern __inline__ uint
iopin_is_odr(iopin_t *iopin)
{
    volatile uint *odrp = &((immap_t *)CONFIG_SYS_IMMR)->im_ioport.iop_podra;
    return (odrp[iopin->port * 8] >> (31 - iopin->pin)) & 1;
}

extern __inline__ uint
iopin_is_act(iopin_t *iopin)
{
    volatile uint *odrp = &((immap_t *)CONFIG_SYS_IMMR)->im_ioport.iop_podra;
    return ((odrp[iopin->port * 8] >> (31 - iopin->pin)) & 1) ^ 1;
}

extern __inline__ void
iopin_set_ded(iopin_t *iopin)
{
    volatile uint *parp = &((immap_t *)CONFIG_SYS_IMMR)->im_ioport.iop_ppara;
    parp[iopin->port * 8] |= (1 << (31 - iopin->pin));
}

extern __inline__ void
iopin_set_gen(iopin_t *iopin)
{
    volatile uint *parp = &((immap_t *)CONFIG_SYS_IMMR)->im_ioport.iop_ppara;
    parp[iopin->port * 8] &= ~(1 << (31 - iopin->pin));
}

extern __inline__ uint
iopin_is_ded(iopin_t *iopin)
{
    volatile uint *parp = &((immap_t *)CONFIG_SYS_IMMR)->im_ioport.iop_ppara;
    return (parp[iopin->port * 8] >> (31 - iopin->pin)) & 1;
}

extern __inline__ uint
iopin_is_gen(iopin_t *iopin)
{
    volatile uint *parp = &((immap_t *)CONFIG_SYS_IMMR)->im_ioport.iop_ppara;
    return ((parp[iopin->port * 8] >> (31 - iopin->pin)) & 1) ^ 1;
}

extern __inline__ void
iopin_set_opt2(iopin_t *iopin)
{
    volatile uint *sorp = &((immap_t *)CONFIG_SYS_IMMR)->im_ioport.iop_psora;
    sorp[iopin->port * 8] |= (1 << (31 - iopin->pin));
}

extern __inline__ void
iopin_set_opt1(iopin_t *iopin)
{
    volatile uint *sorp = &((immap_t *)CONFIG_SYS_IMMR)->im_ioport.iop_psora;
    sorp[iopin->port * 8] &= ~(1 << (31 - iopin->pin));
}

extern __inline__ uint
iopin_is_opt2(iopin_t *iopin)
{
    volatile uint *sorp = &((immap_t *)CONFIG_SYS_IMMR)->im_ioport.iop_psora;
    return (sorp[iopin->port * 8] >> (31 - iopin->pin)) & 1;
}

extern __inline__ uint
iopin_is_opt1(iopin_t *iopin)
{
    volatile uint *sorp = &((immap_t *)CONFIG_SYS_IMMR)->im_ioport.iop_psora;
    return ((sorp[iopin->port * 8] >> (31 - iopin->pin)) & 1) ^ 1;
}

#endif /* __KERNEL__ */

#endif /* _ASM_IOPIN_8260_H_ */
