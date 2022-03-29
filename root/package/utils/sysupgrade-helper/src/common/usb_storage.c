/*
 * Most of this source has been derived from the Linux USB
 * project:
 *   (c) 1999-2002 Matthew Dharm (mdharm-usb@one-eyed-alien.net)
 *   (c) 2000 David L. Brown, Jr. (usb-storage@davidb.org)
 *   (c) 1999 Michael Gee (michael@linuxspecific.com)
 *   (c) 2000 Yggdrasil Computing, Inc.
 *
 *
 * Adapted for U-Boot:
 *   (C) Copyright 2001 Denis Peter, MPL AG Switzerland
 *
 * For BBB support (C) Copyright 2003
 * Gary Jennejohn, DENX Software Engineering <garyj@denx.de>
 *
 * BBB support based on /sys/dev/usb/umass.c from
 * FreeBSD.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 */

/* Note:
 * Currently only the CBI transport protocoll has been implemented, and it
 * is only tested with a TEAC USB Floppy. Other Massstorages with CBI or CB
 * transport protocoll may work as well.
 */
/*
 * New Note:
 * Support for USB Mass Storage Devices (BBB) has been added. It has
 * only been tested with USB memory sticks.
 */


#include <common.h>
#include <command.h>
#include <asm/byteorder.h>
#include <asm/processor.h>

#include <part.h>
#include <usb.h>

#undef BBB_COMDAT_TRACE
#undef BBB_XPORT_TRACE

#ifdef	USB_STOR_DEBUG
#define USB_BLK_DEBUG	1
#else
#define USB_BLK_DEBUG	0
#endif

#define USB_STOR_PRINTF(fmt, args...)	debug_cond(USB_BLK_DEBUG, fmt, ##args)

#include <scsi.h>
/* direction table -- this indicates the direction of the data
 * transfer for each command code -- a 1 indicates input
 */
static const unsigned char us_direction[256/8] = {
	0x28, 0x81, 0x14, 0x14, 0x20, 0x01, 0x90, 0x77,
	0x0C, 0x20, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
#define US_DIRECTION(x) ((us_direction[x>>3] >> (x & 7)) & 1)

static ccb usb_ccb __attribute__((aligned(ARCH_DMA_MINALIGN)));

/*
 * CBI style
 */

#define US_CBI_ADSC		0

/*
 * BULK only
 */
#define US_BBB_RESET		0xff
#define US_BBB_GET_MAX_LUN	0xfe

/* Command Block Wrapper */
typedef struct {
	__u32		dCBWSignature;
#	define CBWSIGNATURE	0x43425355
	__u32		dCBWTag;
	__u32		dCBWDataTransferLength;
	__u8		bCBWFlags;
#	define CBWFLAGS_OUT	0x00
#	define CBWFLAGS_IN	0x80
	__u8		bCBWLUN;
	__u8		bCDBLength;
#	define CBWCDBLENGTH	16
	__u8		CBWCDB[CBWCDBLENGTH];
} umass_bbb_cbw_t;
#define UMASS_BBB_CBW_SIZE	31
static __u32 CBWTag;

/* Command Status Wrapper */
typedef struct {
	__u32		dCSWSignature;
#	define CSWSIGNATURE	0x53425355
	__u32		dCSWTag;
	__u32		dCSWDataResidue;
	__u8		bCSWStatus;
#	define CSWSTATUS_GOOD	0x0
#	define CSWSTATUS_FAILED 0x1
#	define CSWSTATUS_PHASE	0x2
} umass_bbb_csw_t;
#define UMASS_BBB_CSW_SIZE	13

#define USB_MAX_STOR_DEV 5
static int usb_max_devs; /* number of highest available usb device */

static block_dev_desc_t usb_dev_desc[USB_MAX_STOR_DEV];

struct us_data;
typedef int (*trans_cmnd)(ccb *cb, struct us_data *data);
typedef int (*trans_reset)(struct us_data *data);

struct us_data {
	struct usb_device *pusb_dev;	 /* this usb_device */

	unsigned int	flags;			/* from filter initially */
	unsigned char	ifnum;			/* interface number */
	unsigned char	ep_in;			/* in endpoint */
	unsigned char	ep_out;			/* out ....... */
	unsigned char	ep_int;			/* interrupt . */
	unsigned char	subclass;		/* as in overview */
	unsigned char	protocol;		/* .............. */
	unsigned char	attention_done;		/* force attn on first cmd */
	unsigned short	ip_data;		/* interrupt data */
	int		action;			/* what to do */
	int		ip_wanted;		/* needed */
	int		*irq_handle;		/* for USB int requests */
	unsigned int	irqpipe;	 	/* pipe for release_irq */
	unsigned char	irqmaxp;		/* max packed for irq Pipe */
	unsigned char	irqinterval;		/* Intervall for IRQ Pipe */
	ccb		*srb;			/* current srb */
	trans_reset	transport_reset;	/* reset routine */
	trans_cmnd	transport;		/* transport routine */
};

/*
 * The U-Boot EHCI driver cannot handle more than 5 page aligned buffers
 * of 4096 bytes in a transfer without running itself out of qt_buffers
 */
#define USB_MAX_XFER_BLK(start, blksz)	(((4096 * 5) - (start % 4096)) / blksz)

static struct us_data usb_stor[USB_MAX_STOR_DEV];


#define USB_STOR_TRANSPORT_GOOD	   0
#define USB_STOR_TRANSPORT_FAILED -1
#define USB_STOR_TRANSPORT_ERROR  -2

int usb_stor_get_info(struct usb_device *dev, struct us_data *us,
		      block_dev_desc_t *dev_desc);
int usb_storage_probe(struct usb_device *dev, unsigned int ifnum,
		      struct us_data *ss);
unsigned long usb_stor_read(int device, unsigned long blknr,
			    unsigned long blkcnt, void *buffer);
unsigned long usb_stor_write(int device, unsigned long blknr,
			     unsigned long blkcnt, const void *buffer);
struct usb_device * usb_get_dev_index(int index);
void uhci_show_temp_int_td(void);

#ifdef CONFIG_PARTITIONS
block_dev_desc_t *usb_stor_get_dev(int index)
{
	return (index < usb_max_devs) ? &usb_dev_desc[index] : NULL;
}
#endif

void usb_show_progress(void)
{
	debug(".");
}

/*******************************************************************************
 * show info on storage devices; 'usb start/init' must be invoked earlier
 * as we only retrieve structures populated during devices initialization
 */
int usb_stor_info(void)
{
	int i;

	if (usb_max_devs > 0) {
		for (i = 0; i < usb_max_devs; i++) {
			printf("  Device %d: ", i);
			dev_print(&usb_dev_desc[i]);
		}
		return 0;
	}

	printf("No storage devices, perhaps not 'usb start'ed..?\n");
	return 1;
}

static unsigned int usb_get_max_lun(struct us_data *us)
{
	int len;
	ALLOC_CACHE_ALIGN_BUFFER(unsigned char, result, 1);
	len = usb_control_msg(us->pusb_dev,
			      usb_rcvctrlpipe(us->pusb_dev, 0),
			      US_BBB_GET_MAX_LUN,
			      USB_TYPE_CLASS | USB_RECIP_INTERFACE | USB_DIR_IN,
			      0, us->ifnum,
			      result, sizeof(char),
			      USB_CNTL_TIMEOUT * 5);
	USB_STOR_PRINTF("Get Max LUN -> len = %i, result = %i\n",
			len, (int) *result);
	return (len > 0) ? *result : 0;
}

/*******************************************************************************
 * scan the usb and reports device info
 * to the user if mode = 1
 * returns current device or -1 if no
 */
int usb_stor_scan(int mode)
{
	unsigned char i;
	struct usb_device *dev;

	if (mode == 1)
		printf("       scanning usb for storage devices... ");

	usb_disable_asynch(1); /* asynch transfer not allowed */

	for (i = 0; i < USB_MAX_STOR_DEV; i++) {
		memset(&usb_dev_desc[i], 0, sizeof(block_dev_desc_t));
		usb_dev_desc[i].if_type = IF_TYPE_USB;
		usb_dev_desc[i].dev = i;
		usb_dev_desc[i].part_type = PART_TYPE_UNKNOWN;
		usb_dev_desc[i].target = 0xff;
		usb_dev_desc[i].type = DEV_TYPE_UNKNOWN;
		usb_dev_desc[i].block_read = usb_stor_read;
		usb_dev_desc[i].block_write = usb_stor_write;
	}

	usb_max_devs = 0;
	for (i = 0; i < USB_MAX_DEVICE; i++) {
		dev = usb_get_dev_index(i); /* get device */
		USB_STOR_PRINTF("i=%d\n", i);
		if (dev == NULL)
			break; /* no more devices available */

		if (usb_storage_probe(dev, 0, &usb_stor[usb_max_devs])) {
			/* OK, it's a storage device.  Iterate over its LUNs
			 * and populate `usb_dev_desc'.
			 */
			int lun, max_lun, start = usb_max_devs;

			max_lun = usb_get_max_lun(&usb_stor[usb_max_devs]);
			for (lun = 0;
			     lun <= max_lun && usb_max_devs < USB_MAX_STOR_DEV;
			     lun++) {
				usb_dev_desc[usb_max_devs].lun = lun;
				if (usb_stor_get_info(dev, &usb_stor[start],
				    &usb_dev_desc[usb_max_devs]) == 1) {
					usb_max_devs++;
				}
			}
		}
		/* if storage device */
		if (usb_max_devs == USB_MAX_STOR_DEV) {
			printf("max USB Storage Device reached: %d stopping\n",
				usb_max_devs);
			break;
		}
	} /* for */

	usb_disable_asynch(0); /* asynch transfer allowed */
	printf("%d Storage Device(s) found\n", usb_max_devs);
	if (usb_max_devs > 0)
		return 0;
	return -1;
}

static int usb_stor_irq(struct usb_device *dev)
{
	struct us_data *us;
	us = (struct us_data *)dev->privptr;

	if (us->ip_wanted)
		us->ip_wanted = 0;
	return 0;
}


#ifdef	USB_STOR_DEBUG

static void usb_show_srb(ccb *pccb)
{
	int i;
	printf("SRB: len %d datalen 0x%lX\n ", pccb->cmdlen, pccb->datalen);
	for (i = 0; i < 12; i++)
		printf("%02X ", pccb->cmd[i]);
	printf("\n");
}

static void display_int_status(unsigned long tmp)
{
	printf("Status: %s %s %s %s %s %s %s\n",
		(tmp & USB_ST_ACTIVE) ? "Active" : "",
		(tmp & USB_ST_STALLED) ? "Stalled" : "",
		(tmp & USB_ST_BUF_ERR) ? "Buffer Error" : "",
		(tmp & USB_ST_BABBLE_DET) ? "Babble Det" : "",
		(tmp & USB_ST_NAK_REC) ? "NAKed" : "",
		(tmp & USB_ST_CRC_ERR) ? "CRC Error" : "",
		(tmp & USB_ST_BIT_ERR) ? "Bitstuff Error" : "");
}
#endif
/***********************************************************************
 * Data transfer routines
 ***********************************************************************/

static int us_one_transfer(struct us_data *us, int pipe, char *buf, int length)
{
	int max_size;
	int this_xfer;
	int result;
	int partial;
	int maxtry;
	int stat;

	/* determine the maximum packet size for these transfers */
	max_size = usb_maxpacket(us->pusb_dev, pipe) * 16;

	/* while we have data left to transfer */
	while (length) {

		/* calculate how long this will be -- maximum or a remainder */
		this_xfer = length > max_size ? max_size : length;
		length -= this_xfer;

		/* setup the retry counter */
		maxtry = 10;

		/* set up the transfer loop */
		do {
			/* transfer the data */
			USB_STOR_PRINTF("Bulk xfer 0x%x(%d) try #%d\n",
				  (unsigned int)buf, this_xfer, 11 - maxtry);
			result = usb_bulk_msg(us->pusb_dev, pipe, buf,
					      this_xfer, &partial,
					      USB_CNTL_TIMEOUT * 5);
			USB_STOR_PRINTF("bulk_msg returned %d xferred %d/%d\n",
				  result, partial, this_xfer);
			if (us->pusb_dev->status != 0) {
				/* if we stall, we need to clear it before
				 * we go on
				 */
#ifdef USB_STOR_DEBUG
				display_int_status(us->pusb_dev->status);
#endif
				if (us->pusb_dev->status & USB_ST_STALLED) {
					USB_STOR_PRINTF("stalled ->clearing endpoint halt for pipe 0x%x\n", pipe);
					stat = us->pusb_dev->status;
					usb_clear_halt(us->pusb_dev, pipe);
					us->pusb_dev->status = stat;
					if (this_xfer == partial) {
						USB_STOR_PRINTF("bulk transferred with error %lX, but data ok\n", us->pusb_dev->status);
						return 0;
					}
					else
						return result;
				}
				if (us->pusb_dev->status & USB_ST_NAK_REC) {
					USB_STOR_PRINTF("Device NAKed bulk_msg\n");
					return result;
				}
				USB_STOR_PRINTF("bulk transferred with error");
				if (this_xfer == partial) {
					USB_STOR_PRINTF(" %ld, but data ok\n",
							us->pusb_dev->status);
					return 0;
				}
				/* if our try counter reaches 0, bail out */
					USB_STOR_PRINTF(" %ld, data %d\n",
						us->pusb_dev->status, partial);
				if (!maxtry--)
						return result;
			}
			/* update to show what data was transferred */
			this_xfer -= partial;
			buf += partial;
			/* continue until this transfer is done */
		} while (this_xfer);
	}

	/* if we get here, we're done and successful */
	return 0;
}

static int usb_stor_BBB_reset(struct us_data *us)
{
	int result;
	unsigned int pipe;

	/*
	 * Reset recovery (5.3.4 in Universal Serial Bus Mass Storage Class)
	 *
	 * For Reset Recovery the host shall issue in the following order:
	 * a) a Bulk-Only Mass Storage Reset
	 * b) a Clear Feature HALT to the Bulk-In endpoint
	 * c) a Clear Feature HALT to the Bulk-Out endpoint
	 *
	 * This is done in 3 steps.
	 *
	 * If the reset doesn't succeed, the device should be port reset.
	 *
	 * This comment stolen from FreeBSD's /sys/dev/usb/umass.c.
	 */
	USB_STOR_PRINTF("BBB_reset\n");
	result = usb_control_msg(us->pusb_dev, usb_sndctrlpipe(us->pusb_dev, 0),
				 US_BBB_RESET,
				 USB_TYPE_CLASS | USB_RECIP_INTERFACE,
				 0, us->ifnum, 0, 0, USB_CNTL_TIMEOUT * 5);

	if ((result < 0) && (us->pusb_dev->status & USB_ST_STALLED)) {
		USB_STOR_PRINTF("RESET:stall\n");
		return -1;
	}

	/* long wait for reset */
	mdelay(150);
	USB_STOR_PRINTF("BBB_reset result %d: status %lX reset\n", result,
			us->pusb_dev->status);
	pipe = usb_rcvbulkpipe(us->pusb_dev, us->ep_in);
	result = usb_clear_halt(us->pusb_dev, pipe);
	/* long wait for reset */
	mdelay(150);
	USB_STOR_PRINTF("BBB_reset result %d: status %lX clearing IN endpoint\n",
			result, us->pusb_dev->status);
	/* long wait for reset */
	pipe = usb_sndbulkpipe(us->pusb_dev, us->ep_out);
	result = usb_clear_halt(us->pusb_dev, pipe);
	mdelay(150);
	USB_STOR_PRINTF("BBB_reset result %d: status %lX"
			" clearing OUT endpoint\n", result,
			us->pusb_dev->status);
	USB_STOR_PRINTF("BBB_reset done\n");
	return 0;
}

/* FIXME: this reset function doesn't really reset the port, and it
 * should. Actually it should probably do what it's doing here, and
 * reset the port physically
 */
static int usb_stor_CB_reset(struct us_data *us)
{
	unsigned char cmd[12];
	int result;

	USB_STOR_PRINTF("CB_reset\n");
	memset(cmd, 0xff, sizeof(cmd));
	cmd[0] = SCSI_SEND_DIAG;
	cmd[1] = 4;
	result = usb_control_msg(us->pusb_dev, usb_sndctrlpipe(us->pusb_dev, 0),
				 US_CBI_ADSC,
				 USB_TYPE_CLASS | USB_RECIP_INTERFACE,
				 0, us->ifnum, cmd, sizeof(cmd),
				 USB_CNTL_TIMEOUT * 5);

	/* long wait for reset */
	mdelay(1500);
	USB_STOR_PRINTF("CB_reset result %d: status %lX"
			" clearing endpoint halt\n", result,
			us->pusb_dev->status);
	usb_clear_halt(us->pusb_dev, usb_rcvbulkpipe(us->pusb_dev, us->ep_in));
	usb_clear_halt(us->pusb_dev, usb_rcvbulkpipe(us->pusb_dev, us->ep_out));

	USB_STOR_PRINTF("CB_reset done\n");
	return 0;
}

/*
 * Set up the command for a BBB device. Note that the actual SCSI
 * command is copied into cbw.CBWCDB.
 */
int usb_stor_BBB_comdat(ccb *srb, struct us_data *us)
{
	int result;
	int actlen;
	int dir_in;
	unsigned int pipe;
	ALLOC_CACHE_ALIGN_BUFFER(umass_bbb_cbw_t, cbw, 1);

	dir_in = US_DIRECTION(srb->cmd[0]);

#ifdef BBB_COMDAT_TRACE
	printf("dir %d lun %d cmdlen %d cmd %p datalen %lu pdata %p\n",
		dir_in, srb->lun, srb->cmdlen, srb->cmd, srb->datalen,
		srb->pdata);
	if (srb->cmdlen) {
		for (result = 0; result < srb->cmdlen; result++)
			printf("cmd[%d] %#x ", result, srb->cmd[result]);
		printf("\n");
	}
#endif
	/* sanity checks */
	if (!(srb->cmdlen <= CBWCDBLENGTH)) {
		USB_STOR_PRINTF("usb_stor_BBB_comdat:cmdlen too large\n");
		return -1;
	}

	/* always OUT to the ep */
	pipe = usb_sndbulkpipe(us->pusb_dev, us->ep_out);

	cbw->dCBWSignature = cpu_to_le32(CBWSIGNATURE);
	cbw->dCBWTag = cpu_to_le32(CBWTag++);
	cbw->dCBWDataTransferLength = cpu_to_le32(srb->datalen);
	cbw->bCBWFlags = (dir_in ? CBWFLAGS_IN : CBWFLAGS_OUT);
	cbw->bCBWLUN = srb->lun;
	cbw->bCDBLength = srb->cmdlen;
	/* copy the command data into the CBW command data buffer */
	/* DST SRC LEN!!! */
	memcpy(cbw->CBWCDB, srb->cmd, srb->cmdlen);
	result = usb_bulk_msg(us->pusb_dev, pipe, cbw, UMASS_BBB_CBW_SIZE,
			      &actlen, USB_CNTL_TIMEOUT * 5);
	if (result < 0)
		USB_STOR_PRINTF("usb_stor_BBB_comdat:usb_bulk_msg error\n");
	return result;
}

/* FIXME: we also need a CBI_command which sets up the completion
 * interrupt, and waits for it
 */
int usb_stor_CB_comdat(ccb *srb, struct us_data *us)
{
	int result = 0;
	int dir_in, retry;
	unsigned int pipe;
	unsigned long status;

	retry = 5;
	dir_in = US_DIRECTION(srb->cmd[0]);

	if (dir_in)
		pipe = usb_rcvbulkpipe(us->pusb_dev, us->ep_in);
	else
		pipe = usb_sndbulkpipe(us->pusb_dev, us->ep_out);

	while (retry--) {
		USB_STOR_PRINTF("CBI gets a command: Try %d\n", 5 - retry);
#ifdef USB_STOR_DEBUG
		usb_show_srb(srb);
#endif
		/* let's send the command via the control pipe */
		result = usb_control_msg(us->pusb_dev,
					 usb_sndctrlpipe(us->pusb_dev , 0),
					 US_CBI_ADSC,
					 USB_TYPE_CLASS | USB_RECIP_INTERFACE,
					 0, us->ifnum,
					 srb->cmd, srb->cmdlen,
					 USB_CNTL_TIMEOUT * 5);
		USB_STOR_PRINTF("CB_transport: control msg returned %d,"
				" status %lX\n", result, us->pusb_dev->status);
		/* check the return code for the command */
		if (result < 0) {
			if (us->pusb_dev->status & USB_ST_STALLED) {
				status = us->pusb_dev->status;
				USB_STOR_PRINTF(" stall during command found,"
						" clear pipe\n");
				usb_clear_halt(us->pusb_dev,
					      usb_sndctrlpipe(us->pusb_dev, 0));
				us->pusb_dev->status = status;
			}
			USB_STOR_PRINTF(" error during command %02X"
					" Stat = %lX\n", srb->cmd[0],
					us->pusb_dev->status);
			return result;
		}
		/* transfer the data payload for this command, if one exists*/

		USB_STOR_PRINTF("CB_transport: control msg returned %d,"
				" direction is %s to go 0x%lx\n", result,
				dir_in ? "IN" : "OUT", srb->datalen);
		if (srb->datalen) {
			result = us_one_transfer(us, pipe, (char *)srb->pdata,
						 srb->datalen);
			USB_STOR_PRINTF("CBI attempted to transfer data,"
					" result is %d status %lX, len %d\n",
					result, us->pusb_dev->status,
					us->pusb_dev->act_len);
			if (!(us->pusb_dev->status & USB_ST_NAK_REC))
				break;
		} /* if (srb->datalen) */
		else
			break;
	}
	/* return result */

	return result;
}


int usb_stor_CBI_get_status(ccb *srb, struct us_data *us)
{
	int timeout;

	us->ip_wanted = 1;
	submit_int_msg(us->pusb_dev, us->irqpipe,
			(void *) &us->ip_data, us->irqmaxp, us->irqinterval);
	timeout = 1000;
	while (timeout--) {
		if ((volatile int *) us->ip_wanted == 0)
			break;
		mdelay(10);
	}
	if (us->ip_wanted) {
		printf("	Did not get interrupt on CBI\n");
		us->ip_wanted = 0;
		return USB_STOR_TRANSPORT_ERROR;
	}
	USB_STOR_PRINTF
		("Got interrupt data 0x%x, transfered %d status 0x%lX\n",
		 us->ip_data, us->pusb_dev->irq_act_len,
		 us->pusb_dev->irq_status);
	/* UFI gives us ASC and ASCQ, like a request sense */
	if (us->subclass == US_SC_UFI) {
		if (srb->cmd[0] == SCSI_REQ_SENSE ||
		    srb->cmd[0] == SCSI_INQUIRY)
			return USB_STOR_TRANSPORT_GOOD; /* Good */
		else if (us->ip_data)
			return USB_STOR_TRANSPORT_FAILED;
		else
			return USB_STOR_TRANSPORT_GOOD;
	}
	/* otherwise, we interpret the data normally */
	switch (us->ip_data) {
	case 0x0001:
		return USB_STOR_TRANSPORT_GOOD;
	case 0x0002:
		return USB_STOR_TRANSPORT_FAILED;
	default:
		return USB_STOR_TRANSPORT_ERROR;
	}			/* switch */
	return USB_STOR_TRANSPORT_ERROR;
}

#define USB_TRANSPORT_UNKNOWN_RETRY 5
#define USB_TRANSPORT_NOT_READY_RETRY 10

/* clear a stall on an endpoint - special for BBB devices */
int usb_stor_BBB_clear_endpt_stall(struct us_data *us, __u8 endpt)
{
	int result;

	/* ENDPOINT_HALT = 0, so set value to 0 */
	result = usb_control_msg(us->pusb_dev, usb_sndctrlpipe(us->pusb_dev, 0),
				USB_REQ_CLEAR_FEATURE, USB_RECIP_ENDPOINT,
				0, endpt, 0, 0, USB_CNTL_TIMEOUT * 5);
	return result;
}

int usb_stor_BBB_transport(ccb *srb, struct us_data *us)
{
	int result, retry;
	int dir_in;
	int actlen, data_actlen;
	unsigned int pipe, pipein, pipeout;
	ALLOC_CACHE_ALIGN_BUFFER(umass_bbb_csw_t, csw, 1);
#ifdef BBB_XPORT_TRACE
	unsigned char *ptr;
	int index;
#endif

	dir_in = US_DIRECTION(srb->cmd[0]);

	/* COMMAND phase */
	USB_STOR_PRINTF("COMMAND phase\n");
	result = usb_stor_BBB_comdat(srb, us);
	if (result < 0) {
		USB_STOR_PRINTF("failed to send CBW status %ld\n",
			us->pusb_dev->status);
		usb_stor_BBB_reset(us);
		return USB_STOR_TRANSPORT_FAILED;
	}
	mdelay(5);
	pipein = usb_rcvbulkpipe(us->pusb_dev, us->ep_in);
	pipeout = usb_sndbulkpipe(us->pusb_dev, us->ep_out);
	/* DATA phase + error handling */
	data_actlen = 0;
	/* no data, go immediately to the STATUS phase */
	if (srb->datalen == 0)
		goto st;
	USB_STOR_PRINTF("DATA phase\n");
	if (dir_in)
		pipe = pipein;
	else
		pipe = pipeout;
	result = usb_bulk_msg(us->pusb_dev, pipe, srb->pdata, srb->datalen,
			      &data_actlen, USB_CNTL_TIMEOUT * 5);
	/* special handling of STALL in DATA phase */
	if ((result < 0) && (us->pusb_dev->status & USB_ST_STALLED)) {
		USB_STOR_PRINTF("DATA:stall\n");
		/* clear the STALL on the endpoint */
		result = usb_stor_BBB_clear_endpt_stall(us,
					dir_in ? us->ep_in : us->ep_out);
		if (result >= 0)
			/* continue on to STATUS phase */
			goto st;
	}
	if (result < 0) {
		USB_STOR_PRINTF("usb_bulk_msg error status %ld\n",
			us->pusb_dev->status);
		usb_stor_BBB_reset(us);
		return USB_STOR_TRANSPORT_FAILED;
	}
#ifdef BBB_XPORT_TRACE
	for (index = 0; index < data_actlen; index++)
		printf("pdata[%d] %#x ", index, srb->pdata[index]);
	printf("\n");
#endif
	/* STATUS phase + error handling */
st:
	retry = 0;
again:
	USB_STOR_PRINTF("STATUS phase\n");
	result = usb_bulk_msg(us->pusb_dev, pipein, csw, UMASS_BBB_CSW_SIZE,
				&actlen, USB_CNTL_TIMEOUT*5);

	/* special handling of STALL in STATUS phase */
	if ((result < 0) && (retry < 1) &&
	    (us->pusb_dev->status & USB_ST_STALLED)) {
		USB_STOR_PRINTF("STATUS:stall\n");
		/* clear the STALL on the endpoint */
		result = usb_stor_BBB_clear_endpt_stall(us, us->ep_in);
		if (result >= 0 && (retry++ < 1))
			/* do a retry */
			goto again;
	}
	if (result < 0) {
		USB_STOR_PRINTF("usb_bulk_msg error status %ld\n",
			us->pusb_dev->status);
		usb_stor_BBB_reset(us);
		return USB_STOR_TRANSPORT_FAILED;
	}
#ifdef BBB_XPORT_TRACE
	ptr = (unsigned char *)csw;
	for (index = 0; index < UMASS_BBB_CSW_SIZE; index++)
		printf("ptr[%d] %#x ", index, ptr[index]);
	printf("\n");
#endif
	/* misuse pipe to get the residue */
	pipe = le32_to_cpu(csw->dCSWDataResidue);
	if (pipe == 0 && srb->datalen != 0 && srb->datalen - data_actlen != 0)
		pipe = srb->datalen - data_actlen;
	if (CSWSIGNATURE != le32_to_cpu(csw->dCSWSignature)) {
		USB_STOR_PRINTF("!CSWSIGNATURE\n");
		usb_stor_BBB_reset(us);
		return USB_STOR_TRANSPORT_FAILED;
	} else if ((CBWTag - 1) != le32_to_cpu(csw->dCSWTag)) {
		USB_STOR_PRINTF("!Tag\n");
		usb_stor_BBB_reset(us);
		return USB_STOR_TRANSPORT_FAILED;
	} else if (csw->bCSWStatus > CSWSTATUS_PHASE) {
		USB_STOR_PRINTF(">PHASE\n");
		usb_stor_BBB_reset(us);
		return USB_STOR_TRANSPORT_FAILED;
	} else if (csw->bCSWStatus == CSWSTATUS_PHASE) {
		USB_STOR_PRINTF("=PHASE\n");
		usb_stor_BBB_reset(us);
		return USB_STOR_TRANSPORT_FAILED;
	} else if (data_actlen > srb->datalen) {
		USB_STOR_PRINTF("transferred %dB instead of %ldB\n",
			data_actlen, srb->datalen);
		return USB_STOR_TRANSPORT_FAILED;
	} else if (csw->bCSWStatus == CSWSTATUS_FAILED) {
		USB_STOR_PRINTF("FAILED\n");
		return USB_STOR_TRANSPORT_FAILED;
	}

	return result;
}

int usb_stor_CB_transport(ccb *srb, struct us_data *us)
{
	int result, status;
	ccb *psrb;
	ccb reqsrb;
	int retry, notready;

	psrb = &reqsrb;
	status = USB_STOR_TRANSPORT_GOOD;
	retry = 0;
	notready = 0;
	/* issue the command */
do_retry:
	result = usb_stor_CB_comdat(srb, us);
	USB_STOR_PRINTF("command / Data returned %d, status %lX\n",
			result, us->pusb_dev->status);
	/* if this is an CBI Protocol, get IRQ */
	if (us->protocol == US_PR_CBI) {
		status = usb_stor_CBI_get_status(srb, us);
		/* if the status is error, report it */
		if (status == USB_STOR_TRANSPORT_ERROR) {
			USB_STOR_PRINTF(" USB CBI Command Error\n");
			return status;
		}
		srb->sense_buf[12] = (unsigned char)(us->ip_data >> 8);
		srb->sense_buf[13] = (unsigned char)(us->ip_data & 0xff);
		if (!us->ip_data) {
			/* if the status is good, report it */
			if (status == USB_STOR_TRANSPORT_GOOD) {
				USB_STOR_PRINTF(" USB CBI Command Good\n");
				return status;
			}
		}
	}
	/* do we have to issue an auto request? */
	/* HERE we have to check the result */
	if ((result < 0) && !(us->pusb_dev->status & USB_ST_STALLED)) {
		USB_STOR_PRINTF("ERROR %lX\n", us->pusb_dev->status);
		us->transport_reset(us);
		return USB_STOR_TRANSPORT_ERROR;
	}
	if ((us->protocol == US_PR_CBI) &&
	    ((srb->cmd[0] == SCSI_REQ_SENSE) ||
	    (srb->cmd[0] == SCSI_INQUIRY))) {
		/* do not issue an autorequest after request sense */
		USB_STOR_PRINTF("No auto request and good\n");
		return USB_STOR_TRANSPORT_GOOD;
	}
	/* issue an request_sense */
	memset(&psrb->cmd[0], 0, 12);
	psrb->cmd[0] = SCSI_REQ_SENSE;
	psrb->cmd[1] = srb->lun << 5;
	psrb->cmd[4] = 18;
	psrb->datalen = 18;
	psrb->pdata = &srb->sense_buf[0];
	psrb->cmdlen = 12;
	/* issue the command */
	result = usb_stor_CB_comdat(psrb, us);
	USB_STOR_PRINTF("auto request returned %d\n", result);
	/* if this is an CBI Protocol, get IRQ */
	if (us->protocol == US_PR_CBI)
		status = usb_stor_CBI_get_status(psrb, us);

	if ((result < 0) && !(us->pusb_dev->status & USB_ST_STALLED)) {
		USB_STOR_PRINTF(" AUTO REQUEST ERROR %ld\n",
				us->pusb_dev->status);
		return USB_STOR_TRANSPORT_ERROR;
	}
	USB_STOR_PRINTF("autorequest returned 0x%02X 0x%02X 0x%02X 0x%02X\n",
			srb->sense_buf[0], srb->sense_buf[2],
			srb->sense_buf[12], srb->sense_buf[13]);
	/* Check the auto request result */
	if ((srb->sense_buf[2] == 0) &&
	    (srb->sense_buf[12] == 0) &&
	    (srb->sense_buf[13] == 0)) {
		/* ok, no sense */
		return USB_STOR_TRANSPORT_GOOD;
	}

	/* Check the auto request result */
	switch (srb->sense_buf[2]) {
	case 0x01:
		/* Recovered Error */
		return USB_STOR_TRANSPORT_GOOD;
		break;
	case 0x02:
		/* Not Ready */
		if (notready++ > USB_TRANSPORT_NOT_READY_RETRY) {
			printf("cmd 0x%02X returned 0x%02X 0x%02X 0x%02X"
			       " 0x%02X (NOT READY)\n", srb->cmd[0],
				srb->sense_buf[0], srb->sense_buf[2],
				srb->sense_buf[12], srb->sense_buf[13]);
			return USB_STOR_TRANSPORT_FAILED;
		} else {
			mdelay(100);
			goto do_retry;
		}
		break;
	default:
		if (retry++ > USB_TRANSPORT_UNKNOWN_RETRY) {
			printf("cmd 0x%02X returned 0x%02X 0x%02X 0x%02X"
			       " 0x%02X\n", srb->cmd[0], srb->sense_buf[0],
				srb->sense_buf[2], srb->sense_buf[12],
				srb->sense_buf[13]);
			return USB_STOR_TRANSPORT_FAILED;
		} else
			goto do_retry;
		break;
	}
	return USB_STOR_TRANSPORT_FAILED;
}


static int usb_inquiry(ccb *srb, struct us_data *ss)
{
	int retry, i;
	retry = 5;
	do {
		memset(&srb->cmd[0], 0, 12);
		srb->cmd[0] = SCSI_INQUIRY;
		srb->cmd[1] = srb->lun << 5;
		srb->cmd[4] = 36;
		srb->datalen = 36;
		srb->cmdlen = 12;
		i = ss->transport(srb, ss);
		USB_STOR_PRINTF("inquiry returns %d\n", i);
		if (i == 0)
			break;
	} while (--retry);

	if (!retry) {
		printf("error in inquiry\n");
		return -1;
	}
	return 0;
}

static int usb_request_sense(ccb *srb, struct us_data *ss)
{
	char *ptr;

	ptr = (char *)srb->pdata;
	memset(&srb->cmd[0], 0, 12);
	srb->cmd[0] = SCSI_REQ_SENSE;
	srb->cmd[1] = srb->lun << 5;
	srb->cmd[4] = 18;
	srb->datalen = 18;
	srb->pdata = &srb->sense_buf[0];
	srb->cmdlen = 12;
	ss->transport(srb, ss);
	USB_STOR_PRINTF("Request Sense returned %02X %02X %02X\n",
			srb->sense_buf[2], srb->sense_buf[12],
			srb->sense_buf[13]);
	srb->pdata = (uchar *)ptr;
	return 0;
}

static int usb_test_unit_ready(ccb *srb, struct us_data *ss)
{
	int retries = 10;

	do {
		memset(&srb->cmd[0], 0, 12);
		srb->cmd[0] = SCSI_TST_U_RDY;
		srb->cmd[1] = srb->lun << 5;
		srb->datalen = 0;
		srb->cmdlen = 12;
		if (ss->transport(srb, ss) == USB_STOR_TRANSPORT_GOOD)
			return 0;
		usb_request_sense(srb, ss);
		mdelay(100);
	} while (retries--);

	return -1;
}

static int usb_read_capacity(ccb *srb, struct us_data *ss)
{
	int retry;
	/* XXX retries */
	retry = 3;
	do {
		memset(&srb->cmd[0], 0, 12);
		srb->cmd[0] = SCSI_RD_CAPAC;
		srb->cmd[1] = srb->lun << 5;
		srb->datalen = 8;
		srb->cmdlen = 12;
		if (ss->transport(srb, ss) == USB_STOR_TRANSPORT_GOOD)
			return 0;
	} while (retry--);

	return -1;
}

static int usb_read_10(ccb *srb, struct us_data *ss, unsigned long start,
		       unsigned short blocks)
{
	memset(&srb->cmd[0], 0, 12);
	srb->cmd[0] = SCSI_READ10;
	srb->cmd[1] = srb->lun << 5;
	srb->cmd[2] = ((unsigned char) (start >> 24)) & 0xff;
	srb->cmd[3] = ((unsigned char) (start >> 16)) & 0xff;
	srb->cmd[4] = ((unsigned char) (start >> 8)) & 0xff;
	srb->cmd[5] = ((unsigned char) (start)) & 0xff;
	srb->cmd[7] = ((unsigned char) (blocks >> 8)) & 0xff;
	srb->cmd[8] = (unsigned char) blocks & 0xff;
	srb->cmdlen = 12;
	USB_STOR_PRINTF("read10: start %lx blocks %x\n", start, blocks);
	return ss->transport(srb, ss);
}

static int usb_write_10(ccb *srb, struct us_data *ss, unsigned long start,
			unsigned short blocks)
{
	memset(&srb->cmd[0], 0, 12);
	srb->cmd[0] = SCSI_WRITE10;
	srb->cmd[1] = srb->lun << 5;
	srb->cmd[2] = ((unsigned char) (start >> 24)) & 0xff;
	srb->cmd[3] = ((unsigned char) (start >> 16)) & 0xff;
	srb->cmd[4] = ((unsigned char) (start >> 8)) & 0xff;
	srb->cmd[5] = ((unsigned char) (start)) & 0xff;
	srb->cmd[7] = ((unsigned char) (blocks >> 8)) & 0xff;
	srb->cmd[8] = (unsigned char) blocks & 0xff;
	srb->cmdlen = 12;
	USB_STOR_PRINTF("write10: start %lx blocks %x\n", start, blocks);
	return ss->transport(srb, ss);
}


#ifdef CONFIG_USB_BIN_FIXUP
/*
 * Some USB storage devices queried for SCSI identification data respond with
 * binary strings, which if output to the console freeze the terminal. The
 * workaround is to modify the vendor and product strings read from such
 * device with proper values (as reported by 'usb info').
 *
 * Vendor and product length limits are taken from the definition of
 * block_dev_desc_t in include/part.h.
 */
static void usb_bin_fixup(struct usb_device_descriptor descriptor,
				unsigned char vendor[],
				unsigned char product[]) {
	const unsigned char max_vendor_len = 40;
	const unsigned char max_product_len = 20;
	if (descriptor.idVendor == 0x0424 && descriptor.idProduct == 0x223a) {
		strncpy((char *)vendor, "SMSC", max_vendor_len);
		strncpy((char *)product, "Flash Media Cntrller",
			max_product_len);
	}
}
#endif /* CONFIG_USB_BIN_FIXUP */

unsigned long usb_stor_read(int device, unsigned long blknr,
			    unsigned long blkcnt, void *buffer)
{
	unsigned long start, blks, buf_addr, max_xfer_blk;
	unsigned short smallblks;
	struct usb_device *dev;
	struct us_data *ss;
	int retry, i;
	ccb *srb = &usb_ccb;

	if (blkcnt == 0)
		return 0;

	device &= 0xff;
	/* Setup  device */
	USB_STOR_PRINTF("\nusb_read: dev %d \n", device);
	dev = NULL;
	for (i = 0; i < USB_MAX_DEVICE; i++) {
		dev = usb_get_dev_index(i);
		if (dev == NULL)
			return 0;
		if (dev->devnum == usb_dev_desc[device].target)
			break;
	}
	ss = (struct us_data *)dev->privptr;

	usb_disable_asynch(1); /* asynch transfer not allowed */
	srb->lun = usb_dev_desc[device].lun;
	buf_addr = (unsigned long)buffer;
	start = blknr;
	blks = blkcnt;
	if (usb_test_unit_ready(srb, ss)) {
		printf("Device NOT ready\n   Request Sense returned %02X %02X"
		       " %02X\n", srb->sense_buf[2], srb->sense_buf[12],
		       srb->sense_buf[13]);
		return 0;
	}

	USB_STOR_PRINTF("\nusb_read: dev %d startblk %lx, blccnt %lx"
			" buffer %lx\n", device, start, blks, buf_addr);

	do {
		/* XXX need some comment here */
		retry = 2;
		srb->pdata = (unsigned char *)buf_addr;
		max_xfer_blk = USB_MAX_XFER_BLK(buf_addr,
						usb_dev_desc[device].blksz);
		if (blks > max_xfer_blk)
			smallblks = (unsigned short) max_xfer_blk;
		else
			smallblks = (unsigned short) blks;
retry_it:
		if (smallblks == max_xfer_blk)
			usb_show_progress();
		srb->datalen = usb_dev_desc[device].blksz * smallblks;
		srb->pdata = (unsigned char *)buf_addr;
		if (usb_read_10(srb, ss, start, smallblks)) {
			USB_STOR_PRINTF("Read ERROR\n");
			usb_request_sense(srb, ss);
			if (retry--)
				goto retry_it;
			blkcnt -= blks;
			break;
		}
		start += smallblks;
		blks -= smallblks;
		buf_addr += srb->datalen;
	} while (blks != 0);

	USB_STOR_PRINTF("usb_read: end startblk %lx, blccnt %x buffer %lx\n",
			start, smallblks, buf_addr);

	usb_disable_asynch(0); /* asynch transfer allowed */
	if (blkcnt >= max_xfer_blk)
		debug("\n");
	return blkcnt;
}

unsigned long usb_stor_write(int device, unsigned long blknr,
				unsigned long blkcnt, const void *buffer)
{
	unsigned long start, blks, buf_addr, max_xfer_blk;
	unsigned short smallblks;
	struct usb_device *dev;
	struct us_data *ss;
	int retry, i;
	ccb *srb = &usb_ccb;

	if (blkcnt == 0)
		return 0;

	device &= 0xff;
	/* Setup  device */
	USB_STOR_PRINTF("\nusb_write: dev %d \n", device);
	dev = NULL;
	for (i = 0; i < USB_MAX_DEVICE; i++) {
		dev = usb_get_dev_index(i);
		if (dev == NULL)
			return 0;
		if (dev->devnum == usb_dev_desc[device].target)
			break;
	}
	ss = (struct us_data *)dev->privptr;

	usb_disable_asynch(1); /* asynch transfer not allowed */

	srb->lun = usb_dev_desc[device].lun;
	buf_addr = (unsigned long)buffer;
	start = blknr;
	blks = blkcnt;
	if (usb_test_unit_ready(srb, ss)) {
		printf("Device NOT ready\n   Request Sense returned %02X %02X"
		       " %02X\n", srb->sense_buf[2], srb->sense_buf[12],
			srb->sense_buf[13]);
		return 0;
	}

	USB_STOR_PRINTF("\nusb_write: dev %d startblk %lx, blccnt %lx"
			" buffer %lx\n", device, start, blks, buf_addr);

	do {
		/* If write fails retry for max retry count else
		 * return with number of blocks written successfully.
		 */
		retry = 2;
		srb->pdata = (unsigned char *)buf_addr;
		max_xfer_blk = USB_MAX_XFER_BLK(buf_addr,
						usb_dev_desc[device].blksz);
		if (blks > max_xfer_blk)
			smallblks = (unsigned short) max_xfer_blk;
		else
			smallblks = (unsigned short) blks;
retry_it:
		if (smallblks == max_xfer_blk)
			usb_show_progress();
		srb->datalen = usb_dev_desc[device].blksz * smallblks;
		srb->pdata = (unsigned char *)buf_addr;
		if (usb_write_10(srb, ss, start, smallblks)) {
			USB_STOR_PRINTF("Write ERROR\n");
			usb_request_sense(srb, ss);
			if (retry--)
				goto retry_it;
			blkcnt -= blks;
			break;
		}
		start += smallblks;
		blks -= smallblks;
		buf_addr += srb->datalen;
	} while (blks != 0);

	USB_STOR_PRINTF("usb_write: end startblk %lx, blccnt %x buffer %lx\n",
			start, smallblks, buf_addr);

	usb_disable_asynch(0); /* asynch transfer allowed */
	if (blkcnt >= max_xfer_blk)
		debug("\n");
	return blkcnt;

}

/* Probe to see if a new device is actually a Storage device */
int usb_storage_probe(struct usb_device *dev, unsigned int ifnum,
		      struct us_data *ss)
{
	struct usb_interface *iface;
	int i;
	struct usb_endpoint_descriptor *ep_desc;
	unsigned int flags = 0;

	int protocol = 0;
	int subclass = 0;

	/* let's examine the device now */
	iface = &dev->config.if_desc[ifnum];

#if 0
	/* this is the place to patch some storage devices */
	USB_STOR_PRINTF("iVendor %X iProduct %X\n", dev->descriptor.idVendor,
			dev->descriptor.idProduct);

	if ((dev->descriptor.idVendor) == 0x066b &&
	    (dev->descriptor.idProduct) == 0x0103) {
		USB_STOR_PRINTF("patched for E-USB\n");
		protocol = US_PR_CB;
		subclass = US_SC_UFI;	    /* an assumption */
	}
#endif

	if (dev->descriptor.bDeviceClass != 0 ||
			iface->desc.bInterfaceClass != USB_CLASS_MASS_STORAGE ||
			iface->desc.bInterfaceSubClass < US_SC_MIN ||
			iface->desc.bInterfaceSubClass > US_SC_MAX) {
		/* if it's not a mass storage, we go no further */
		return 0;
	}

	memset(ss, 0, sizeof(struct us_data));

	/* At this point, we know we've got a live one */
	USB_STOR_PRINTF("\n\nUSB Mass Storage device detected\n");

	/* Initialize the us_data structure with some useful info */
	ss->flags = flags;
	ss->ifnum = ifnum;
	ss->pusb_dev = dev;
	ss->attention_done = 0;

	/* If the device has subclass and protocol, then use that.  Otherwise,
	 * take data from the specific interface.
	 */
	if (subclass) {
		ss->subclass = subclass;
		ss->protocol = protocol;
	} else {
		ss->subclass = iface->desc.bInterfaceSubClass;
		ss->protocol = iface->desc.bInterfaceProtocol;
	}

	/* set the handler pointers based on the protocol */
	USB_STOR_PRINTF("Transport: ");
	switch (ss->protocol) {
	case US_PR_CB:
		USB_STOR_PRINTF("Control/Bulk\n");
		ss->transport = usb_stor_CB_transport;
		ss->transport_reset = usb_stor_CB_reset;
		break;

	case US_PR_CBI:
		USB_STOR_PRINTF("Control/Bulk/Interrupt\n");
		ss->transport = usb_stor_CB_transport;
		ss->transport_reset = usb_stor_CB_reset;
		break;
	case US_PR_BULK:
		USB_STOR_PRINTF("Bulk/Bulk/Bulk\n");
		ss->transport = usb_stor_BBB_transport;
		ss->transport_reset = usb_stor_BBB_reset;
		break;
	default:
		printf("USB Storage Transport unknown / not yet implemented\n");
		return 0;
		break;
	}

	/*
	 * We are expecting a minimum of 2 endpoints - in and out (bulk).
	 * An optional interrupt is OK (necessary for CBI protocol).
	 * We will ignore any others.
	 */
	for (i = 0; i < iface->desc.bNumEndpoints; i++) {
		ep_desc = &iface->ep_desc[i];
		/* is it an BULK endpoint? */
		if ((ep_desc->bmAttributes &
		     USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_BULK) {
			if (ep_desc->bEndpointAddress & USB_DIR_IN)
				ss->ep_in = ep_desc->bEndpointAddress &
						USB_ENDPOINT_NUMBER_MASK;
			else
				ss->ep_out =
					ep_desc->bEndpointAddress &
					USB_ENDPOINT_NUMBER_MASK;
		}

		/* is it an interrupt endpoint? */
		if ((ep_desc->bmAttributes &
		     USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_INT) {
			ss->ep_int = ep_desc->bEndpointAddress &
						USB_ENDPOINT_NUMBER_MASK;
			ss->irqinterval = ep_desc->bInterval;
		}
	}
	USB_STOR_PRINTF("Endpoints In %d Out %d Int %d\n",
		  ss->ep_in, ss->ep_out, ss->ep_int);

	/* Do some basic sanity checks, and bail if we find a problem */
	if (usb_set_interface(dev, iface->desc.bInterfaceNumber, 0) ||
	    !ss->ep_in || !ss->ep_out ||
	    (ss->protocol == US_PR_CBI && ss->ep_int == 0)) {
		USB_STOR_PRINTF("Problems with device\n");
		return 0;
	}
	/* set class specific stuff */
	/* We only handle certain protocols.  Currently, these are
	 * the only ones.
	 * The SFF8070 accepts the requests used in u-boot
	 */
	if (ss->subclass != US_SC_UFI && ss->subclass != US_SC_SCSI &&
	    ss->subclass != US_SC_8070) {
		printf("Sorry, protocol %d not yet supported.\n", ss->subclass);
		return 0;
	}
	if (ss->ep_int) {
		/* we had found an interrupt endpoint, prepare irq pipe
		 * set up the IRQ pipe and handler
		 */
		ss->irqinterval = (ss->irqinterval > 0) ? ss->irqinterval : 255;
		ss->irqpipe = usb_rcvintpipe(ss->pusb_dev, ss->ep_int);
		ss->irqmaxp = usb_maxpacket(dev, ss->irqpipe);
		dev->irq_handle = usb_stor_irq;
	}
	dev->privptr = (void *)ss;
	return 1;
}

int usb_stor_get_info(struct usb_device *dev, struct us_data *ss,
		      block_dev_desc_t *dev_desc)
{
	unsigned char perq, modi;
	ALLOC_CACHE_ALIGN_BUFFER(unsigned long, cap, 2);
	ALLOC_CACHE_ALIGN_BUFFER(unsigned char, usb_stor_buf, 36);
	unsigned long *capacity, *blksz;
	ccb *pccb = &usb_ccb;

	pccb->pdata = usb_stor_buf;

	dev_desc->target = dev->devnum;
	pccb->lun = dev_desc->lun;
	USB_STOR_PRINTF(" address %d\n", dev_desc->target);

	if (usb_inquiry(pccb, ss))
		return -1;

	perq = usb_stor_buf[0];
	modi = usb_stor_buf[1];

	if ((perq & 0x1f) == 0x1f) {
		/* skip unknown devices */
		return 0;
	}
	if ((modi&0x80) == 0x80) {
		/* drive is removable */
		dev_desc->removable = 1;
	}
	memcpy(&dev_desc->vendor[0], (const void *) &usb_stor_buf[8], 8);
	memcpy(&dev_desc->product[0], (const void *) &usb_stor_buf[16], 16);
	memcpy(&dev_desc->revision[0], (const void *) &usb_stor_buf[32], 4);
	dev_desc->vendor[8] = 0;
	dev_desc->product[16] = 0;
	dev_desc->revision[4] = 0;
#ifdef CONFIG_USB_BIN_FIXUP
	usb_bin_fixup(dev->descriptor, (uchar *)dev_desc->vendor,
		      (uchar *)dev_desc->product);
#endif /* CONFIG_USB_BIN_FIXUP */
	USB_STOR_PRINTF("ISO Vers %X, Response Data %X\n", usb_stor_buf[2],
			usb_stor_buf[3]);
	if (usb_test_unit_ready(pccb, ss)) {
		printf("Device NOT ready\n"
		       "   Request Sense returned %02X %02X %02X\n",
		       pccb->sense_buf[2], pccb->sense_buf[12],
		       pccb->sense_buf[13]);
		if (dev_desc->removable == 1) {
			dev_desc->type = perq;
			return 1;
		}
		return 0;
	}
	pccb->pdata = (unsigned char *)&cap[0];
	memset(pccb->pdata, 0, 8);
	if (usb_read_capacity(pccb, ss) != 0) {
		printf("READ_CAP ERROR\n");
		cap[0] = 2880;
		cap[1] = 0x200;
	}
	USB_STOR_PRINTF("Read Capacity returns: 0x%lx, 0x%lx\n", cap[0],
			cap[1]);
#if 0
	if (cap[0] > (0x200000 * 10)) /* greater than 10 GByte */
		cap[0] >>= 16;
#endif
	cap[0] = cpu_to_be32(cap[0]);
	cap[1] = cpu_to_be32(cap[1]);

	/* this assumes bigendian! */
	cap[0] += 1;
	capacity = &cap[0];
	blksz = &cap[1];
	USB_STOR_PRINTF("Capacity = 0x%lx, blocksz = 0x%lx\n",
			*capacity, *blksz);
	dev_desc->lba = *capacity;
	dev_desc->blksz = *blksz;
	dev_desc->type = perq;
	USB_STOR_PRINTF(" address %d\n", dev_desc->target);
	USB_STOR_PRINTF("partype: %d\n", dev_desc->part_type);

	init_part(dev_desc);

	USB_STOR_PRINTF("partype: %d\n", dev_desc->part_type);
	return 1;
}
