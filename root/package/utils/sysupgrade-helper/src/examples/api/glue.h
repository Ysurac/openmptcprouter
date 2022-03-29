/*
 * (C) Copyright 2007 Semihalf
 *
 * Written by: Rafal Jaworowski <raj@semihalf.com>
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
 *
 */

/*
 * This is the header file for conveniency wrapper routines (API glue)
 */

#ifndef _API_GLUE_H_
#define _API_GLUE_H_

#define API_SEARCH_LEN		(3 * 1024 * 1024)	/* 3MB search range */

#define UB_MAX_MR	5	/* max mem regions number */
#define UB_MAX_DEV	6	/* max devices number */

extern void *syscall_ptr;
extern uint32_t search_hint;

int	syscall(int, int *, ...);
int	api_search_sig(struct api_signature **sig);

/*
 * The ub_ library calls are part of the application, not U-Boot code!  They
 * are front-end wrappers that are used by the consumer application: they
 * prepare arguments for particular syscall and jump to the low level
 * syscall()
 */

/* console */
int	ub_getc(void);
int	ub_tstc(void);
void	ub_putc(char c);
void	ub_puts(const char *s);

/* system */
void			ub_reset(void);
struct sys_info *	ub_get_sys_info(void);

/* time */
void		ub_udelay(unsigned long);
unsigned long	ub_get_timer(unsigned long);

/* env vars */
char *		ub_env_get(const char *name);
void		ub_env_set(const char *name, char *value);
const char *	ub_env_enum(const char *last);

/* devices */
int			ub_dev_enum(void);
int			ub_dev_open(int handle);
int			ub_dev_close(int handle);
int			ub_dev_read(int handle, void *buf, lbasize_t len,
				lbastart_t start, lbasize_t *rlen);
int			ub_dev_send(int handle, void *buf, int len);
int			ub_dev_recv(int handle, void *buf, int len, int *rlen);
struct device_info *	ub_dev_get(int);

/* display */
int ub_display_get_info(int type, struct display_info *di);
int ub_display_draw_bitmap(ulong bitmap, int x, int y);
void ub_display_clear(void);

#endif /* _API_GLUE_H_ */
