/*
 * R8A66597 HCD (Host Controller Driver) for u-boot
 *
 * Copyright (C) 2008  Yoshihiro Shimoda <shimoda.yoshihiro@renesas.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <common.h>
#include <usb.h>
#include <asm/io.h>

#include "r8a66597.h"

#ifdef R8A66597_DEBUG
#define R8A66597_DPRINT		printf
#else
#define R8A66597_DPRINT(...)
#endif

static const char hcd_name[] = "r8a66597_hcd";
static unsigned short clock = CONFIG_R8A66597_XTAL;
static unsigned short vif = CONFIG_R8A66597_LDRV;
static unsigned short endian = CONFIG_R8A66597_ENDIAN;
static struct r8a66597 gr8a66597;

static void get_hub_data(struct usb_device *dev, u16 *hub_devnum, u16 *hubport)
{
	int i;

	*hub_devnum = 0;
	*hubport = 0;

	/* check a device connected to root_hub */
	if ((dev->parent && dev->parent->devnum == 1) ||
	    (dev->devnum == 1))
		return;

	for (i = 0; i < USB_MAXCHILDREN; i++) {
		if (dev->parent->children[i] == dev) {
			*hub_devnum = (u8)dev->parent->devnum;
			*hubport = i;
			return;
		}
	}

	printf("get_hub_data error.\n");
}

static void set_devadd(struct r8a66597 *r8a66597, u8 r8a66597_address,
			struct usb_device *dev, int port)
{
	u16 val, usbspd, upphub, hubport;
	unsigned long devadd_reg = get_devadd_addr(r8a66597_address);

	get_hub_data(dev, &upphub, &hubport);
	usbspd = r8a66597->speed;
	val = (upphub << 11) | (hubport << 8) | (usbspd << 6) | (port & 0x0001);
	r8a66597_write(r8a66597, val, devadd_reg);
}

static int r8a66597_clock_enable(struct r8a66597 *r8a66597)
{
	u16 tmp;
	int i = 0;

#if defined(CONFIG_SUPERH_ON_CHIP_R8A66597)
	do {
		r8a66597_write(r8a66597, SCKE, SYSCFG0);
		tmp = r8a66597_read(r8a66597, SYSCFG0);
		if (i++ > 1000) {
			printf("register access fail.\n");
			return -1;
		}
	} while ((tmp & SCKE) != SCKE);
	r8a66597_write(r8a66597, 0x04, 0x02);
#else
	do {
		r8a66597_write(r8a66597, USBE, SYSCFG0);
		tmp = r8a66597_read(r8a66597, SYSCFG0);
		if (i++ > 1000) {
			printf("register access fail.\n");
			return -1;
		}
	} while ((tmp & USBE) != USBE);
	r8a66597_bclr(r8a66597, USBE, SYSCFG0);
	r8a66597_mdfy(r8a66597, clock, XTAL, SYSCFG0);

	i = 0;
	r8a66597_bset(r8a66597, XCKE, SYSCFG0);
	do {
		udelay(1000);
		tmp = r8a66597_read(r8a66597, SYSCFG0);
		if (i++ > 500) {
			printf("register access fail.\n");
			return -1;
		}
	} while ((tmp & SCKE) != SCKE);
#endif	/* #if defined(CONFIG_SUPERH_ON_CHIP_R8A66597) */

	return 0;
}

static void r8a66597_clock_disable(struct r8a66597 *r8a66597)
{
	r8a66597_bclr(r8a66597, SCKE, SYSCFG0);
	udelay(1);
#if !defined(CONFIG_SUPERH_ON_CHIP_R8A66597)
	r8a66597_bclr(r8a66597, PLLC, SYSCFG0);
	r8a66597_bclr(r8a66597, XCKE, SYSCFG0);
	r8a66597_bclr(r8a66597, USBE, SYSCFG0);
#endif
}

static void r8a66597_enable_port(struct r8a66597 *r8a66597, int port)
{
	u16 val;

	val = port ? DRPD : DCFM | DRPD;
	r8a66597_bset(r8a66597, val, get_syscfg_reg(port));
	r8a66597_bset(r8a66597, HSE, get_syscfg_reg(port));

	r8a66597_write(r8a66597, BURST | CPU_ADR_RD_WR, get_dmacfg_reg(port));
}

static void r8a66597_disable_port(struct r8a66597 *r8a66597, int port)
{
	u16 val, tmp;

	r8a66597_write(r8a66597, 0, get_intenb_reg(port));
	r8a66597_write(r8a66597, 0, get_intsts_reg(port));

	r8a66597_port_power(r8a66597, port, 0);

	do {
		tmp = r8a66597_read(r8a66597, SOFCFG) & EDGESTS;
		udelay(640);
	} while (tmp == EDGESTS);

	val = port ? DRPD : DCFM | DRPD;
	r8a66597_bclr(r8a66597, val, get_syscfg_reg(port));
	r8a66597_bclr(r8a66597, HSE, get_syscfg_reg(port));
}

static int enable_controller(struct r8a66597 *r8a66597)
{
	int ret, port;

	ret = r8a66597_clock_enable(r8a66597);
	if (ret < 0)
		return ret;

	r8a66597_bset(r8a66597, vif & LDRV, PINCFG);
	r8a66597_bset(r8a66597, USBE, SYSCFG0);

	r8a66597_bset(r8a66597, INTL, SOFCFG);
	r8a66597_write(r8a66597, 0, INTENB0);
	r8a66597_write(r8a66597, 0, INTENB1);
	r8a66597_write(r8a66597, 0, INTENB2);

	r8a66597_bset(r8a66597, endian & BIGEND, CFIFOSEL);
	r8a66597_bset(r8a66597, endian & BIGEND, D0FIFOSEL);
	r8a66597_bset(r8a66597, endian & BIGEND, D1FIFOSEL);
	r8a66597_bset(r8a66597, TRNENSEL, SOFCFG);

	for (port = 0; port < R8A66597_MAX_ROOT_HUB; port++)
		r8a66597_enable_port(r8a66597, port);

	return 0;
}

static void disable_controller(struct r8a66597 *r8a66597)
{
	int i;

	if (!(r8a66597_read(r8a66597, SYSCFG0) & USBE))
		return;

	r8a66597_write(r8a66597, 0, INTENB0);
	r8a66597_write(r8a66597, 0, INTSTS0);

	r8a66597_write(r8a66597, 0, D0FIFOSEL);
	r8a66597_write(r8a66597, 0, D1FIFOSEL);
	r8a66597_write(r8a66597, 0, DCPCFG);
	r8a66597_write(r8a66597, 0x40, DCPMAXP);
	r8a66597_write(r8a66597, 0, DCPCTR);

	for (i = 0; i <= 10; i++)
		r8a66597_write(r8a66597, 0, get_devadd_addr(i));
	for (i = 1; i <= 5; i++) {
		r8a66597_write(r8a66597, 0, get_pipetre_addr(i));
		r8a66597_write(r8a66597, 0, get_pipetrn_addr(i));
	}
	for (i = 1; i < R8A66597_MAX_NUM_PIPE; i++) {
		r8a66597_write(r8a66597, 0, get_pipectr_addr(i));
		r8a66597_write(r8a66597, i, PIPESEL);
		r8a66597_write(r8a66597, 0, PIPECFG);
		r8a66597_write(r8a66597, 0, PIPEBUF);
		r8a66597_write(r8a66597, 0, PIPEMAXP);
		r8a66597_write(r8a66597, 0, PIPEPERI);
	}

	for (i = 0; i < R8A66597_MAX_ROOT_HUB; i++)
		r8a66597_disable_port(r8a66597, i);

	r8a66597_clock_disable(r8a66597);
}

static void r8a66597_reg_wait(struct r8a66597 *r8a66597, unsigned long reg,
			      u16 mask, u16 loop)
{
	u16 tmp;
	int i = 0;

	do {
		tmp = r8a66597_read(r8a66597, reg);
		if (i++ > 1000000) {
			printf("register%lx, loop %x is timeout\n", reg, loop);
			break;
		}
	} while ((tmp & mask) != loop);
}

static void pipe_buffer_setting(struct r8a66597 *r8a66597,
				struct usb_device *dev, unsigned long pipe)
{
	u16 val = 0;
	u16 pipenum, bufnum, maxpacket;

	if (usb_pipein(pipe)) {
		pipenum = BULK_IN_PIPENUM;
		bufnum = BULK_IN_BUFNUM;
		maxpacket = dev->epmaxpacketin[usb_pipeendpoint(pipe)];
	} else {
		pipenum = BULK_OUT_PIPENUM;
		bufnum = BULK_OUT_BUFNUM;
		maxpacket = dev->epmaxpacketout[usb_pipeendpoint(pipe)];
	}

	if (r8a66597->pipe_config & (1 << pipenum))
		return;
	r8a66597->pipe_config |= (1 << pipenum);

	r8a66597_bset(r8a66597, ACLRM, get_pipectr_addr(pipenum));
	r8a66597_bclr(r8a66597, ACLRM, get_pipectr_addr(pipenum));
	r8a66597_write(r8a66597, pipenum, PIPESEL);

	/* FIXME: This driver support bulk transfer only. */
	if (!usb_pipein(pipe))
		val |= R8A66597_DIR;
	else
		val |= R8A66597_SHTNAK;
	val |= R8A66597_BULK | R8A66597_DBLB | usb_pipeendpoint(pipe);
	r8a66597_write(r8a66597, val, PIPECFG);

	r8a66597_write(r8a66597, (8 << 10) | bufnum, PIPEBUF);
	r8a66597_write(r8a66597, make_devsel(usb_pipedevice(pipe)) |
				 maxpacket, PIPEMAXP);
	r8a66597_write(r8a66597, 0, PIPEPERI);
	r8a66597_write(r8a66597, SQCLR, get_pipectr_addr(pipenum));
}

static int send_setup_packet(struct r8a66597 *r8a66597, struct usb_device *dev,
			     struct devrequest *setup)
{
	int i;
	unsigned short *p = (unsigned short *)setup;
	unsigned long setup_addr = USBREQ;
	u16 intsts1;
	int timeout = 3000;
	u16 devsel = setup->request == USB_REQ_SET_ADDRESS ? 0 : dev->devnum;

	r8a66597_write(r8a66597, make_devsel(devsel) |
				 (8 << dev->maxpacketsize), DCPMAXP);
	r8a66597_write(r8a66597, ~(SIGN | SACK), INTSTS1);

	for (i = 0; i < 4; i++) {
		r8a66597_write(r8a66597, le16_to_cpu(p[i]), setup_addr);
		setup_addr += 2;
	}
	r8a66597_write(r8a66597, ~0x0001, BRDYSTS);
	r8a66597_write(r8a66597, SUREQ, DCPCTR);

	while (1) {
		intsts1 = r8a66597_read(r8a66597, INTSTS1);
		if (intsts1 & SACK)
			break;
		if (intsts1 & SIGN) {
			printf("setup packet send error\n");
			return -1;
		}
		if (timeout-- < 0) {
			printf("setup packet timeout\n");
			return -1;
		}
		udelay(500);
	}

	return 0;
}

static int send_bulk_packet(struct r8a66597 *r8a66597, struct usb_device *dev,
			    unsigned long pipe, void *buffer, int transfer_len)
{
	u16 tmp, bufsize;
	u16 *buf;
	size_t size;

	R8A66597_DPRINT("%s\n", __func__);

	r8a66597_mdfy(r8a66597, MBW | BULK_OUT_PIPENUM,
			MBW | CURPIPE, CFIFOSEL);
	r8a66597_reg_wait(r8a66597, CFIFOSEL, CURPIPE, BULK_OUT_PIPENUM);
	tmp = r8a66597_read(r8a66597, CFIFOCTR);
	if ((tmp & FRDY) == 0) {
		printf("%s FRDY is not set (%x)\n", __func__, tmp);
		return -1;
	}

	/* prepare parameters */
	bufsize = dev->epmaxpacketout[usb_pipeendpoint(pipe)];
	buf = (u16 *)(buffer + dev->act_len);
	size = min((int)bufsize, transfer_len - dev->act_len);

	/* write fifo */
	r8a66597_write(r8a66597, ~(1 << BULK_OUT_PIPENUM), BEMPSTS);
	if (buffer) {
		r8a66597_write_fifo(r8a66597, CFIFO, buf, size);
		r8a66597_write(r8a66597, BVAL, CFIFOCTR);
	}

	/* update parameters */
	dev->act_len += size;

	r8a66597_mdfy(r8a66597, PID_BUF, PID,
			get_pipectr_addr(BULK_OUT_PIPENUM));

	while (!(r8a66597_read(r8a66597, BEMPSTS) & (1 << BULK_OUT_PIPENUM)))
		if (ctrlc())
			return -1;
	r8a66597_write(r8a66597, ~(1 << BULK_OUT_PIPENUM), BEMPSTS);

	if (dev->act_len >= transfer_len)
		r8a66597_mdfy(r8a66597, PID_NAK, PID,
				get_pipectr_addr(BULK_OUT_PIPENUM));

	return 0;
}

static int receive_bulk_packet(struct r8a66597 *r8a66597,
			       struct usb_device *dev,
			       unsigned long pipe,
			       void *buffer, int transfer_len)
{
	u16 tmp;
	u16 *buf;
	const u16 pipenum = BULK_IN_PIPENUM;
	int rcv_len;
	int maxpacket = dev->epmaxpacketin[usb_pipeendpoint(pipe)];

	R8A66597_DPRINT("%s\n", __func__);

	/* prepare */
	if (dev->act_len == 0) {
		r8a66597_mdfy(r8a66597, PID_NAK, PID,
				get_pipectr_addr(pipenum));
		r8a66597_write(r8a66597, ~(1 << pipenum), BRDYSTS);

		r8a66597_write(r8a66597, TRCLR, get_pipetre_addr(pipenum));
		r8a66597_write(r8a66597,
				(transfer_len + maxpacket - 1) / maxpacket,
				get_pipetrn_addr(pipenum));
		r8a66597_bset(r8a66597, TRENB, get_pipetre_addr(pipenum));

		r8a66597_mdfy(r8a66597, PID_BUF, PID,
				get_pipectr_addr(pipenum));
	}

	r8a66597_mdfy(r8a66597, MBW | pipenum, MBW | CURPIPE, CFIFOSEL);
	r8a66597_reg_wait(r8a66597, CFIFOSEL, CURPIPE, pipenum);

	while (!(r8a66597_read(r8a66597, BRDYSTS) & (1 << pipenum)))
		if (ctrlc())
			return -1;
	r8a66597_write(r8a66597, ~(1 << pipenum), BRDYSTS);

	tmp = r8a66597_read(r8a66597, CFIFOCTR);
	if ((tmp & FRDY) == 0) {
		printf("%s FRDY is not set. (%x)\n", __func__, tmp);
		return -1;
	}

	buf = (u16 *)(buffer + dev->act_len);
	rcv_len = tmp & DTLN;
	dev->act_len += rcv_len;

	if (buffer) {
		if (rcv_len == 0)
			r8a66597_write(r8a66597, BCLR, CFIFOCTR);
		else
			r8a66597_read_fifo(r8a66597, CFIFO, buf, rcv_len);
	}

	return 0;
}

static int receive_control_packet(struct r8a66597 *r8a66597,
				  struct usb_device *dev,
				  void *buffer, int transfer_len)
{
	u16 tmp;
	int rcv_len;

	/* FIXME: limit transfer size : 64byte or less */

	r8a66597_bclr(r8a66597, R8A66597_DIR, DCPCFG);
	r8a66597_mdfy(r8a66597, 0, ISEL | CURPIPE, CFIFOSEL);
	r8a66597_reg_wait(r8a66597, CFIFOSEL, CURPIPE, 0);
	r8a66597_bset(r8a66597, SQSET, DCPCTR);
	r8a66597_write(r8a66597, BCLR, CFIFOCTR);
	r8a66597_mdfy(r8a66597, PID_BUF, PID, DCPCTR);

	while (!(r8a66597_read(r8a66597, BRDYSTS) & 0x0001))
		if (ctrlc())
			return -1;
	r8a66597_write(r8a66597, ~0x0001, BRDYSTS);

	r8a66597_mdfy(r8a66597, MBW, MBW | CURPIPE, CFIFOSEL);
	r8a66597_reg_wait(r8a66597, CFIFOSEL, CURPIPE, 0);

	tmp = r8a66597_read(r8a66597, CFIFOCTR);
	if ((tmp & FRDY) == 0) {
		printf("%s FRDY is not set. (%x)\n", __func__, tmp);
		return -1;
	}

	rcv_len = tmp & DTLN;
	dev->act_len += rcv_len;

	r8a66597_mdfy(r8a66597, PID_NAK, PID, DCPCTR);

	if (buffer) {
		if (rcv_len == 0)
			r8a66597_write(r8a66597, BCLR, DCPCTR);
		else
			r8a66597_read_fifo(r8a66597, CFIFO, buffer, rcv_len);
	}

	return 0;
}

static int send_status_packet(struct r8a66597 *r8a66597,
			       unsigned long pipe)
{
	r8a66597_bset(r8a66597, SQSET, DCPCTR);
	r8a66597_mdfy(r8a66597, PID_NAK, PID, DCPCTR);

	if (usb_pipein(pipe)) {
		r8a66597_bset(r8a66597, R8A66597_DIR, DCPCFG);
		r8a66597_mdfy(r8a66597, ISEL, ISEL | CURPIPE, CFIFOSEL);
		r8a66597_reg_wait(r8a66597, CFIFOSEL, CURPIPE, 0);
		r8a66597_write(r8a66597, ~BEMP0, BEMPSTS);
		r8a66597_write(r8a66597, BCLR | BVAL, CFIFOCTR);
	} else {
		r8a66597_bclr(r8a66597, R8A66597_DIR, DCPCFG);
		r8a66597_mdfy(r8a66597, 0, ISEL | CURPIPE, CFIFOSEL);
		r8a66597_reg_wait(r8a66597, CFIFOSEL, CURPIPE, 0);
		r8a66597_write(r8a66597, BCLR, CFIFOCTR);
	}
	r8a66597_mdfy(r8a66597, PID_BUF, PID, DCPCTR);

	while (!(r8a66597_read(r8a66597, BEMPSTS) & 0x0001))
		if (ctrlc())
			return -1;

	return 0;
}

static void r8a66597_check_syssts(struct r8a66597 *r8a66597, int port)
{
	int count = R8A66597_MAX_SAMPLING;
	unsigned short syssts, old_syssts;

	R8A66597_DPRINT("%s\n", __func__);

	old_syssts = r8a66597_read(r8a66597, get_syssts_reg(port) & LNST);
	while (count > 0) {
		mdelay(R8A66597_RH_POLL_TIME);

		syssts = r8a66597_read(r8a66597, get_syssts_reg(port) & LNST);
		if (syssts == old_syssts) {
			count--;
		} else {
			count = R8A66597_MAX_SAMPLING;
			old_syssts = syssts;
		}
	}
}

static void r8a66597_bus_reset(struct r8a66597 *r8a66597, int port)
{
	mdelay(10);
	r8a66597_mdfy(r8a66597, USBRST, USBRST | UACT, get_dvstctr_reg(port));
	mdelay(50);
	r8a66597_mdfy(r8a66597, UACT, USBRST | UACT, get_dvstctr_reg(port));
	mdelay(50);
}

static int check_usb_device_connecting(struct r8a66597 *r8a66597)
{
	int timeout = 10000;	/* 100usec * 10000 = 1sec */
	int i;

	for (i = 0; i < 5; i++) {
		/* check a usb cable connect */
		while (!(r8a66597_read(r8a66597, INTSTS1) & ATTCH)) {
			if (timeout-- < 0) {
				printf("%s timeout.\n", __func__);
				return -1;
			}
			udelay(100);
		}

		/* check a data line */
		r8a66597_check_syssts(r8a66597, 0);

		r8a66597_bus_reset(r8a66597, 0);
		r8a66597->speed = get_rh_usb_speed(r8a66597, 0);

		if (!(r8a66597_read(r8a66597, INTSTS1) & DTCH)) {
			r8a66597->port_change = USB_PORT_STAT_C_CONNECTION;
			r8a66597->port_status = USB_PORT_STAT_CONNECTION |
						USB_PORT_STAT_ENABLE;
			return 0;	/* success */
		}

		R8A66597_DPRINT("USB device has detached. retry = %d\n", i);
		r8a66597_write(r8a66597, ~DTCH, INTSTS1);
	}

	return -1;	/* fail */
}

/* based on usb_ohci.c */
#define min_t(type, x, y) \
		({ type __x = (x); type __y = (y); __x < __y ? __x : __y; })
/*-------------------------------------------------------------------------*
 * Virtual Root Hub
 *-------------------------------------------------------------------------*/

/* Device descriptor */
static __u8 root_hub_dev_des[] =
{
	0x12,	    /*	__u8  bLength; */
	0x01,	    /*	__u8  bDescriptorType; Device */
	0x10,	    /*	__u16 bcdUSB; v1.1 */
	0x01,
	0x09,	    /*	__u8  bDeviceClass; HUB_CLASSCODE */
	0x00,	    /*	__u8  bDeviceSubClass; */
	0x00,	    /*	__u8  bDeviceProtocol; */
	0x08,	    /*	__u8  bMaxPacketSize0; 8 Bytes */
	0x00,	    /*	__u16 idVendor; */
	0x00,
	0x00,	    /*	__u16 idProduct; */
	0x00,
	0x00,	    /*	__u16 bcdDevice; */
	0x00,
	0x00,	    /*	__u8  iManufacturer; */
	0x01,	    /*	__u8  iProduct; */
	0x00,	    /*	__u8  iSerialNumber; */
	0x01	    /*	__u8  bNumConfigurations; */
};

/* Configuration descriptor */
static __u8 root_hub_config_des[] =
{
	0x09,	    /*	__u8  bLength; */
	0x02,	    /*	__u8  bDescriptorType; Configuration */
	0x19,	    /*	__u16 wTotalLength; */
	0x00,
	0x01,	    /*	__u8  bNumInterfaces; */
	0x01,	    /*	__u8  bConfigurationValue; */
	0x00,	    /*	__u8  iConfiguration; */
	0x40,	    /*	__u8  bmAttributes; */

	0x00,	    /*	__u8  MaxPower; */

	/* interface */
	0x09,	    /*	__u8  if_bLength; */
	0x04,	    /*	__u8  if_bDescriptorType; Interface */
	0x00,	    /*	__u8  if_bInterfaceNumber; */
	0x00,	    /*	__u8  if_bAlternateSetting; */
	0x01,	    /*	__u8  if_bNumEndpoints; */
	0x09,	    /*	__u8  if_bInterfaceClass; HUB_CLASSCODE */
	0x00,	    /*	__u8  if_bInterfaceSubClass; */
	0x00,	    /*	__u8  if_bInterfaceProtocol; */
	0x00,	    /*	__u8  if_iInterface; */

	/* endpoint */
	0x07,	    /*	__u8  ep_bLength; */
	0x05,	    /*	__u8  ep_bDescriptorType; Endpoint */
	0x81,	    /*	__u8  ep_bEndpointAddress; IN Endpoint 1 */
	0x03,	    /*	__u8  ep_bmAttributes; Interrupt */
	0x02,	    /*	__u16 ep_wMaxPacketSize; ((MAX_ROOT_PORTS + 1) / 8 */
	0x00,
	0xff	    /*	__u8  ep_bInterval; 255 ms */
};

static unsigned char root_hub_str_index0[] =
{
	0x04,			/*  __u8  bLength; */
	0x03,			/*  __u8  bDescriptorType; String-descriptor */
	0x09,			/*  __u8  lang ID */
	0x04,			/*  __u8  lang ID */
};

static unsigned char root_hub_str_index1[] =
{
	34,			/*  __u8  bLength; */
	0x03,			/*  __u8  bDescriptorType; String-descriptor */
	'R',			/*  __u8  Unicode */
	0,				/*  __u8  Unicode */
	'8',			/*  __u8  Unicode */
	0,				/*  __u8  Unicode */
	'A',			/*  __u8  Unicode */
	0,				/*  __u8  Unicode */
	'6',			/*  __u8  Unicode */
	0,				/*  __u8  Unicode */
	'6',			/*  __u8  Unicode */
	0,				/*  __u8  Unicode */
	'5',			/*  __u8  Unicode */
	0,				/*  __u8  Unicode */
	'9',			/*  __u8  Unicode */
	0,				/*  __u8  Unicode */
	'7',			/*  __u8  Unicode */
	0,				/*  __u8  Unicode */
	' ',			/*  __u8  Unicode */
	0,				/*  __u8  Unicode */
	'R',			/*  __u8  Unicode */
	0,				/*  __u8  Unicode */
	'o',			/*  __u8  Unicode */
	0,				/*  __u8  Unicode */
	'o',			/*  __u8  Unicode */
	0,				/*  __u8  Unicode */
	't',			/*  __u8  Unicode */
	0,				/*  __u8  Unicode */
	'H',			/*  __u8  Unicode */
	0,				/*  __u8  Unicode */
	'u',			/*  __u8  Unicode */
	0,				/*  __u8  Unicode */
	'b',			/*  __u8  Unicode */
	0,				/*  __u8  Unicode */
};

static int r8a66597_submit_rh_msg(struct usb_device *dev, unsigned long pipe,
			void *buffer, int transfer_len, struct devrequest *cmd)
{
	struct r8a66597 *r8a66597 = &gr8a66597;
	int leni = transfer_len;
	int len = 0;
	int stat = 0;
	__u16 bmRType_bReq;
	__u16 wValue;
	__u16 wIndex;
	__u16 wLength;
	unsigned char data[32];

	R8A66597_DPRINT("%s\n", __func__);

	if (usb_pipeint(pipe)) {
		printf("Root-Hub submit IRQ: NOT implemented");
		return 0;
	}

	bmRType_bReq  = cmd->requesttype | (cmd->request << 8);
	wValue	      = cpu_to_le16 (cmd->value);
	wIndex	      = cpu_to_le16 (cmd->index);
	wLength	      = cpu_to_le16 (cmd->length);

	switch (bmRType_bReq) {
	case RH_GET_STATUS:
		*(__u16 *)buffer = cpu_to_le16(1);
		len = 2;
		break;
	case RH_GET_STATUS | RH_INTERFACE:
		*(__u16 *)buffer = cpu_to_le16(0);
		len = 2;
		break;
	case RH_GET_STATUS | RH_ENDPOINT:
		*(__u16 *)buffer = cpu_to_le16(0);
		len = 2;
		break;
	case RH_GET_STATUS | RH_CLASS:
		*(__u32 *)buffer = cpu_to_le32(0);
		len = 4;
		break;
	case RH_GET_STATUS | RH_OTHER | RH_CLASS:
		*(__u32 *)buffer = cpu_to_le32(r8a66597->port_status |
						(r8a66597->port_change << 16));
		len = 4;
		break;
	case RH_CLEAR_FEATURE | RH_ENDPOINT:
	case RH_CLEAR_FEATURE | RH_CLASS:
		break;

	case RH_CLEAR_FEATURE | RH_OTHER | RH_CLASS:
		switch (wValue) {
		case RH_C_PORT_CONNECTION:
			r8a66597->port_change &= ~USB_PORT_STAT_C_CONNECTION;
			break;
		}
		break;

	case RH_SET_FEATURE | RH_OTHER | RH_CLASS:
		switch (wValue) {
		case (RH_PORT_SUSPEND):
			break;
		case (RH_PORT_RESET):
			r8a66597_bus_reset(r8a66597, 0);
			break;
		case (RH_PORT_POWER):
			break;
		case (RH_PORT_ENABLE):
			break;
		}
		break;
	case RH_SET_ADDRESS:
		gr8a66597.rh_devnum = wValue;
		break;
	case RH_GET_DESCRIPTOR:
		switch ((wValue & 0xff00) >> 8) {
		case (0x01): /* device descriptor */
			len = min_t(unsigned int,
				  leni,
				  min_t(unsigned int,
				      sizeof(root_hub_dev_des),
				      wLength));
			memcpy(buffer, root_hub_dev_des, len);
			break;
		case (0x02): /* configuration descriptor */
			len = min_t(unsigned int,
				  leni,
				  min_t(unsigned int,
				      sizeof(root_hub_config_des),
				      wLength));
			memcpy(buffer, root_hub_config_des, len);
			break;
		case (0x03): /* string descriptors */
			if (wValue == 0x0300) {
				len = min_t(unsigned int,
					  leni,
					  min_t(unsigned int,
					      sizeof(root_hub_str_index0),
					      wLength));
				memcpy(buffer, root_hub_str_index0, len);
			}
			if (wValue == 0x0301) {
				len = min_t(unsigned int,
					  leni,
					  min_t(unsigned int,
					      sizeof(root_hub_str_index1),
					      wLength));
				memcpy(buffer, root_hub_str_index1, len);
			}
			break;
		default:
			stat = USB_ST_STALLED;
		}
		break;

	case RH_GET_DESCRIPTOR | RH_CLASS:
	{
		__u32 temp = 0x00000001;

		data[0] = 9;		/* min length; */
		data[1] = 0x29;
		data[2] = temp & RH_A_NDP;
		data[3] = 0;
		if (temp & RH_A_PSM)
			data[3] |= 0x1;
		if (temp & RH_A_NOCP)
			data[3] |= 0x10;
		else if (temp & RH_A_OCPM)
			data[3] |= 0x8;

		/* corresponds to data[4-7] */
		data[5] = (temp & RH_A_POTPGT) >> 24;
		data[7] = temp & RH_B_DR;
		if (data[2] < 7) {
			data[8] = 0xff;
		} else {
			data[0] += 2;
			data[8] = (temp & RH_B_DR) >> 8;
			data[10] = data[9] = 0xff;
		}

		len = min_t(unsigned int, leni,
			    min_t(unsigned int, data[0], wLength));
		memcpy(buffer, data, len);
		break;
	}

	case RH_GET_CONFIGURATION:
		*(__u8 *) buffer = 0x01;
		len = 1;
		break;
	case RH_SET_CONFIGURATION:
		break;
	default:
		R8A66597_DPRINT("unsupported root hub command");
		stat = USB_ST_STALLED;
	}

	mdelay(1);

	len = min_t(int, len, leni);

	dev->act_len = len;
	dev->status = stat;

	return stat;
}

int submit_bulk_msg(struct usb_device *dev, unsigned long pipe, void *buffer,
		    int transfer_len)
{
	struct r8a66597 *r8a66597 = &gr8a66597;
	int ret = 0;

	R8A66597_DPRINT("%s\n", __func__);
	R8A66597_DPRINT("pipe = %08x, buffer = %p, len = %d, devnum = %d\n",
			pipe, buffer, transfer_len, dev->devnum);

	set_devadd(r8a66597, dev->devnum, dev, 0);

	pipe_buffer_setting(r8a66597, dev, pipe);

	dev->act_len = 0;
	while (dev->act_len < transfer_len && ret == 0) {
		if (ctrlc())
			return -1;

		if (usb_pipein(pipe))
			ret = receive_bulk_packet(r8a66597, dev, pipe, buffer,
							transfer_len);
		else
			ret = send_bulk_packet(r8a66597, dev, pipe, buffer,
							transfer_len);
	}

	if (ret == 0)
		dev->status = 0;

	return ret;
}

int submit_control_msg(struct usb_device *dev, unsigned long pipe,
		       void *buffer, int transfer_len, struct devrequest *setup)
{
	struct r8a66597 *r8a66597 = &gr8a66597;
	u16 r8a66597_address = setup->request == USB_REQ_SET_ADDRESS ?
					0 : dev->devnum;

	R8A66597_DPRINT("%s\n", __func__);
	if (usb_pipedevice(pipe) == r8a66597->rh_devnum)
		return r8a66597_submit_rh_msg(dev, pipe, buffer, transfer_len,
						setup);

	R8A66597_DPRINT("%s: setup\n", __func__);
	set_devadd(r8a66597, r8a66597_address, dev, 0);

	if (send_setup_packet(r8a66597, dev, setup) < 0) {
		printf("setup packet send error\n");
		return -1;
	}

	dev->act_len = 0;
	if (usb_pipein(pipe))
		if (receive_control_packet(r8a66597, dev, buffer,
						transfer_len) < 0)
			return -1;

	if (send_status_packet(r8a66597, pipe) < 0)
		return -1;

	dev->status = 0;

	return 0;
}

int submit_int_msg(struct usb_device *dev, unsigned long pipe, void *buffer,
			int transfer_len, int interval)
{
	/* no implement */
	R8A66597_DPRINT("%s\n", __func__);
	return 0;
}

int usb_lowlevel_init(int index, void **controller))
{
	struct r8a66597 *r8a66597 = &gr8a66597;

	R8A66597_DPRINT("%s\n", __func__);

	memset(r8a66597, 0, sizeof(r8a66597));
	r8a66597->reg = CONFIG_R8A66597_BASE_ADDR;

	disable_controller(r8a66597);
	mdelay(100);

	enable_controller(r8a66597);
	r8a66597_port_power(r8a66597, 0 , 1);

	/* check usb device */
	check_usb_device_connecting(r8a66597);

	mdelay(50);

	return 0;
}

int usb_lowlevel_stop(int index)
{
	disable_controller(&gr8a66597);

	return 0;
}
