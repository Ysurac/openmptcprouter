#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <post.h>

struct serial_device {
	/* enough bytes to match alignment of following func pointer */
	char name[16];

	int  (*init) (void);
	int  (*uninit) (void);
	void (*setbrg) (void);
	int (*getc) (void);
	int (*tstc) (void);
	void (*putc) (const char c);
	void (*puts) (const char *s);
#if CONFIG_POST & CONFIG_SYS_POST_UART
	void (*loop) (int);
#endif

	struct serial_device *next;
};

extern struct serial_device serial_smc_device;
extern struct serial_device serial_scc_device;
extern struct serial_device *default_serial_console(void);

#if	defined(CONFIG_405GP) || defined(CONFIG_405CR) || \
	defined(CONFIG_405EP) || defined(CONFIG_405EZ) || \
	defined(CONFIG_405EX) || defined(CONFIG_440) || \
	defined(CONFIG_MB86R0x) || defined(CONFIG_MPC5xxx) || \
	defined(CONFIG_MPC83xx) || defined(CONFIG_MPC85xx) || \
	defined(CONFIG_MPC86xx) || defined(CONFIG_SYS_SC520) || \
	defined(CONFIG_TEGRA2) || defined(CONFIG_SYS_COREBOOT)
extern struct serial_device serial0_device;
extern struct serial_device serial1_device;
#if defined(CONFIG_SYS_NS16550_SERIAL)
extern struct serial_device eserial1_device;
extern struct serial_device eserial2_device;
extern struct serial_device eserial3_device;
extern struct serial_device eserial4_device;
#endif /* CONFIG_SYS_NS16550_SERIAL */

#endif

#if defined(CONFIG_MPC512X)
extern struct serial_device serial1_device;
extern struct serial_device serial3_device;
extern struct serial_device serial4_device;
extern struct serial_device serial6_device;
#endif

#if defined(CONFIG_XILINX_UARTLITE)
extern struct serial_device uartlite_serial0_device;
extern struct serial_device uartlite_serial1_device;
extern struct serial_device uartlite_serial2_device;
extern struct serial_device uartlite_serial3_device;
#endif

#if defined(CONFIG_S3C2410)
extern struct serial_device s3c24xx_serial0_device;
extern struct serial_device s3c24xx_serial1_device;
extern struct serial_device s3c24xx_serial2_device;
#endif

#if defined(CONFIG_S5P)
extern struct serial_device s5p_serial0_device;
extern struct serial_device s5p_serial1_device;
extern struct serial_device s5p_serial2_device;
extern struct serial_device s5p_serial3_device;
#endif

#if defined(CONFIG_OMAP3_ZOOM2)
extern struct serial_device zoom2_serial_device0;
extern struct serial_device zoom2_serial_device1;
extern struct serial_device zoom2_serial_device2;
extern struct serial_device zoom2_serial_device3;
#endif

extern struct serial_device serial_ffuart_device;
extern struct serial_device serial_btuart_device;
extern struct serial_device serial_stuart_device;

#if defined(CONFIG_SYS_BFIN_UART)
extern void serial_register_bfin_uart(void);
extern struct serial_device bfin_serial0_device;
extern struct serial_device bfin_serial1_device;
extern struct serial_device bfin_serial2_device;
extern struct serial_device bfin_serial3_device;
#endif

extern void serial_register(struct serial_device *);
extern void serial_initialize(void);
extern void serial_stdio_init(void);
extern int serial_assign(const char *name);
extern void serial_reinit_all(void);

/* For usbtty */
#ifdef CONFIG_USB_TTY

extern int usbtty_getc(void);
extern void usbtty_putc(const char c);
extern void usbtty_puts(const char *str);
extern int usbtty_tstc(void);

#else

/* stubs */
#define usbtty_getc() 0
#define usbtty_putc(a)
#define usbtty_puts(a)
#define usbtty_tstc() 0

#endif /* CONFIG_USB_TTY */

#if defined(CONFIG_MPC512X) &&  defined(CONFIG_SERIAL_MULTI)
extern struct stdio_dev *open_port(int num, int baudrate);
extern int close_port(int num);
extern int write_port(struct stdio_dev *port, char *buf);
extern int read_port(struct stdio_dev *port, char *buf, int size);
#endif

#endif
