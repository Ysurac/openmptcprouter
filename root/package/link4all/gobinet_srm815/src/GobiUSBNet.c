/*===========================================================================
FILE:
   GobiUSBNet.c

DESCRIPTION:
   Qualcomm USB Network device for Gobi 3000

FUNCTIONS:
   GobiNetSuspend
   GobiNetResume
   GobiNetDriverBind
   GobiNetDriverUnbind
   GobiUSBNetURBCallback
   GobiUSBNetTXTimeout
   GobiUSBNetAutoPMThread
   GobiUSBNetStartXmit
   GobiUSBNetOpen
   GobiUSBNetStop
   GobiUSBNetProbe
   GobiUSBNetModInit
   GobiUSBNetModExit

Copyright (c) 2011, Code Aurora Forum. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of Code Aurora Forum nor
      the names of its contributors may be used to endorse or promote
      products derived from this software without specific prior written
      permission.


THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
===========================================================================*/

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------

#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/kernel.h>
#include <linux/ethtool.h>

#include <net/arp.h>
#include <net/ip.h>
#include <net/ipv6.h>

#include "Structs.h"
#include "QMIDevice.h"
#include "QMI.h"

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

// Version Information
//add new module or new feature, increase major version. fix bug, increase minor version
#define DRIVER_VERSION "Meig_GobiNet_Driver_V1.4.1"
#define DRIVER_AUTHOR "Qualcomm Innovation Center"
#define DRIVER_DESC "GobiNet"

// Debug flag
int meig_debug = 0;

// Allow user interrupts
//int interruptible = 1;

// Number of IP packets which may be queued up for transmit
static int txQueueLength = 100;

// Class should be created during module init, so needs to be global
static struct class * gpClass;

static const unsigned char meig_mac[ETH_ALEN] = {0x02, 0x50, 0xf3, 0x00, 0x00, 0x00};
//static const u8 broadcast_addr[ETH_ALEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

//setup data call by "AT$QCRMCALL=1,1"
static uint __read_mostly qcrmcall_mode = 0;
module_param( qcrmcall_mode, uint, S_IRUGO | S_IWUSR );

static struct sk_buff * ether_to_ip_fixup(struct net_device *dev, struct sk_buff *skb)
{
    const struct ethhdr *ehdr;

    skb_reset_mac_header(skb);
    ehdr = eth_hdr(skb);

    if (ehdr->h_proto == htons(ETH_P_IP)) {
        if (unlikely(skb->len <= (sizeof(struct ethhdr) + sizeof(struct iphdr)))) {
            goto drop_skb;
        }
    } else if (ehdr->h_proto == htons(ETH_P_IPV6)) {
        if (unlikely(skb->len <= (sizeof(struct ethhdr) + sizeof(struct ipv6hdr)))) {
            goto drop_skb;
        }
    } else {
        DBG("%s skb h_proto is %04x\n", dev->name, ntohs(ehdr->h_proto));
        goto drop_skb;
    }

    if (unlikely(skb_pull(skb, ETH_HLEN)))
        return skb;

drop_skb:
    return NULL;
}

//#define MEIG_REMOVE_TX_ZLP
#define USB_CDC_SET_REMOVE_TX_ZLP_COMMAND 0x5D

//#define MEIG_WWAN_MULTI_PACKAGES

#ifdef MEIG_WWAN_MULTI_PACKAGES
static uint __read_mostly rx_packets = 10;
module_param( rx_packets, uint, S_IRUGO | S_IWUSR );

#define USB_CDC_SET_MULTI_PACKAGE_COMMAND (0x5C)
#define MEIG_NET_MSG_SPEC		(0x80)
#define MEIG_NET_MSG_ID_IP_DATA		(0x00)

struct multi_package_config {
    __le32 enable;
    __le32 package_max_len;
    __le32 package_max_count_in_queue;
    __le32 timeout;
} __packed;

struct meig_net_package_header {
    unsigned char msg_spec;
    unsigned char msg_id;
    unsigned short payload_len;
    unsigned char reserve[16];
} __packed;
#endif

#ifdef CONFIG_BRIDGE
static int __read_mostly bridge_mode = 0;
module_param( bridge_mode, int, S_IRUGO | S_IWUSR );

static int bridge_arp_reply(sGobiUSBNet * pGobiDev, struct sk_buff *skb)
{
    struct net_device *dev = pGobiDev->mpNetDev->net;
    struct arphdr *parp;
    u8 *arpptr, *sha;
    u8  sip[4], tip[4], ipv4[4];
    struct sk_buff *reply = NULL;

    ipv4[0]  = (pGobiDev->m_bridge_ipv4 >> 24) & 0xFF;
    ipv4[1]  = (pGobiDev->m_bridge_ipv4 >> 16) & 0xFF;
    ipv4[2]  = (pGobiDev->m_bridge_ipv4 >> 8) & 0xFF;
    ipv4[3]  = (pGobiDev->m_bridge_ipv4 >> 0) & 0xFF;

    parp = arp_hdr(skb);

    if (parp->ar_hrd == htons(ARPHRD_ETHER)  && parp->ar_pro == htons(ETH_P_IP)
        && parp->ar_op == htons(ARPOP_REQUEST) && parp->ar_hln == 6 && parp->ar_pln == 4) {
        arpptr = (u8 *)parp + sizeof(struct arphdr);
        sha = arpptr;
        arpptr += dev->addr_len;	/* sha */
        memcpy(sip, arpptr, sizeof(sip));
        arpptr += sizeof(sip);
        arpptr += dev->addr_len;	/* tha */
        memcpy(tip, arpptr, sizeof(tip));

        DBG("sip = %d.%d.%d.%d, tip=%d.%d.%d.%d, ipv4=%d.%d.%d.%d\n",
            sip[0], sip[1], sip[2], sip[3], tip[0], tip[1], tip[2], tip[3], ipv4[0], ipv4[1], ipv4[2], ipv4[3]);
        if (tip[0] == ipv4[0] && tip[1] == ipv4[1] && tip[2] == ipv4[2] && tip[3] != ipv4[3])
            reply = arp_create(ARPOP_REPLY, ETH_P_ARP, *((__be32 *)sip), dev, *((__be32 *)tip), sha, meig_mac, sha);

        if (reply) {
            skb_reset_mac_header(reply);
            __skb_pull(reply, skb_network_offset(reply));
            reply->ip_summed = CHECKSUM_UNNECESSARY;
            reply->pkt_type = PACKET_HOST;

            netif_rx_ni(reply);
        }
        return 1;
    }

    return 0;
}

static ssize_t bridge_mode_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    struct net_device *pNet = to_net_dev(dev);
    struct usbnet * pDev = netdev_priv( pNet );
    sGobiUSBNet * pGobiDev = (sGobiUSBNet *)pDev->data[0];

    return snprintf(buf, PAGE_SIZE, "%d\n", pGobiDev->m_bridge_mode);
}

static ssize_t bridge_mode_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    struct net_device *pNet = to_net_dev(dev);
    struct usbnet * pDev = netdev_priv( pNet );
    sGobiUSBNet * pGobiDev = (sGobiUSBNet *)pDev->data[0];

    if (!GobiTestDownReason( pGobiDev, NET_IFACE_STOPPED )) {
        INFO("please ifconfig %s down\n", pNet->name);
        return -EPERM;
    }

    pGobiDev->m_bridge_mode = !!simple_strtoul(buf, NULL, 10);

    return count;
}

static ssize_t bridge_ipv4_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    struct net_device *pNet = to_net_dev(dev);
    struct usbnet * pDev = netdev_priv( pNet );
    sGobiUSBNet * pGobiDev = (sGobiUSBNet *)pDev->data[0];
    unsigned char ipv4[4];

    ipv4[0]  = (pGobiDev->m_bridge_ipv4 >> 24) & 0xFF;
    ipv4[1]  = (pGobiDev->m_bridge_ipv4 >> 16) & 0xFF;
    ipv4[2]  = (pGobiDev->m_bridge_ipv4 >> 8) & 0xFF;
    ipv4[3]  = (pGobiDev->m_bridge_ipv4 >> 0) & 0xFF;

    return snprintf(buf, PAGE_SIZE, "%d.%d.%d.%d\n",  ipv4[0], ipv4[1], ipv4[2], ipv4[3]);
}

static ssize_t bridge_ipv4_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    struct net_device *pNet = to_net_dev(dev);
    struct usbnet * pDev = netdev_priv( pNet );
    sGobiUSBNet * pGobiDev = (sGobiUSBNet *)pDev->data[0];

    pGobiDev->m_bridge_ipv4 = simple_strtoul(buf, NULL, 16);

    return count;
}


static DEVICE_ATTR(bridge_mode, S_IWUSR | S_IRUGO, bridge_mode_show, bridge_mode_store);
static DEVICE_ATTR(bridge_ipv4, S_IWUSR | S_IRUGO, bridge_ipv4_show, bridge_ipv4_store);
#endif

#ifdef MEIG_WWAN_QMAP
/*
    Meig_WCDMA&LTE_Linux_USB_Driver_User_Guide_V1.9.pdf
    5.6.	Test QMAP on GobiNet or QMI WWAN
    0 - no QMAP
    1 - QMAP (Aggregation protocol)
    X - QMAP (Multiplexing and Aggregation protocol)
*/
static uint __read_mostly qmap_mode = 0;
module_param( qmap_mode, uint, S_IRUGO | S_IWUSR );

struct qmap_hdr {
    u8 cd_rsvd_pad;
    u8 mux_id;
    u16 pkt_len;
} __packed;

struct qmap_priv {
    struct net_device *real_dev;
    u8 offset_id;
};

static int qmap_open(struct net_device *dev)
{
    struct qmap_priv *priv = netdev_priv(dev);
    struct net_device *real_dev = priv->real_dev;

    if (!(priv->real_dev->flags & IFF_UP))
        return -ENETDOWN;

    if (netif_carrier_ok(real_dev))
        netif_carrier_on(dev);
    return 0;
}

static int qmap_stop(struct net_device *pNet)
{
    netif_carrier_off(pNet);
    return 0;
}

static int qmap_start_xmit(struct sk_buff *skb, struct net_device *pNet)
{
    int err;
    struct qmap_priv *priv = netdev_priv(pNet);
    unsigned int len;
    struct qmap_hdr *hdr;

    if (ether_to_ip_fixup(pNet, skb) == NULL) {
        dev_kfree_skb_any (skb);
        return NETDEV_TX_OK;
    }

    len = skb->len;
    hdr = (struct qmap_hdr *)skb_push(skb, sizeof(struct qmap_hdr));
    hdr->cd_rsvd_pad = 0;
    hdr->mux_id = MEIG_QMAP_MUX_ID + priv->offset_id;
    hdr->pkt_len = cpu_to_be16(len);

    skb->dev = priv->real_dev;
    err = dev_queue_xmit(skb);
#if (LINUX_VERSION_CODE > KERNEL_VERSION( 2,6,14 ))
    if (err == NET_XMIT_SUCCESS) {
        pNet->stats.tx_packets++;
        pNet->stats.tx_bytes += skb->len;
    } else {
        pNet->stats.tx_errors++;
    }
#endif

    return err;
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION( 2,6,29 ))
#else
static const struct net_device_ops qmap_netdev_ops = {
    .ndo_open       = qmap_open,
    .ndo_stop       = qmap_stop,
    .ndo_start_xmit = qmap_start_xmit,
};
#endif

static int qmap_register_device(sGobiUSBNet * pDev, u8 offset_id)
{
    struct net_device *real_dev = pDev->mpNetDev->net;
    struct net_device *qmap_net;
    struct qmap_priv *priv;
    int err;

    qmap_net = alloc_etherdev(sizeof(*priv));
    if (!qmap_net)
        return -ENOBUFS;

    SET_NETDEV_DEV(qmap_net, &real_dev->dev);
    priv = netdev_priv(qmap_net);
    priv->offset_id = offset_id;
    priv->real_dev = real_dev;
    sprintf(qmap_net->name, "%s.%d", real_dev->name, offset_id + 1);
#if (LINUX_VERSION_CODE < KERNEL_VERSION( 2,6,29 ))
    qmap_net->open = qmap_open;
    qmap_net->stop = qmap_stop;
    qmap_net->hard_start_xmit = qmap_start_xmit;
#else
    qmap_net->netdev_ops = &qmap_netdev_ops;
#endif
    memcpy (qmap_net->dev_addr, real_dev->dev_addr, ETH_ALEN);

    err = register_netdev(qmap_net);
    if (err < 0)
        goto out_free_newdev;
    netif_device_attach (qmap_net);

    pDev->mpQmapNetDev[offset_id] = qmap_net;
    qmap_net->flags |= IFF_NOARP;

    INFO("%s\n", qmap_net->name);

    return 0;

out_free_newdev:
    free_netdev(qmap_net);
    return err;
}

static void qmap_unregister_device(sGobiUSBNet * pDev, u8 offset_id)
{
    struct net_device *net = pDev->mpQmapNetDev[offset_id];
    if (net != NULL) {
        netif_carrier_off( net );
        unregister_netdev (net);
        free_netdev(net);
    }
}

static ssize_t qmap_mode_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    struct net_device *pNet = to_net_dev(dev);
    struct usbnet * pDev = netdev_priv( pNet );
    sGobiUSBNet * pGobiDev = (sGobiUSBNet *)pDev->data[0];

    return snprintf(buf, PAGE_SIZE, "%d\n", pGobiDev->m_qmap_mode);
}

static ssize_t qmap_mode_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    struct net_device *pNet = to_net_dev(dev);
    struct usbnet * pDev = netdev_priv( pNet );
    sGobiUSBNet * pGobiDev = (sGobiUSBNet *)pDev->data[0];
    int err;
    unsigned int qmap_mode;
    int rx_urb_size = 4096;

    if (!GobiTestDownReason( pGobiDev, NET_IFACE_STOPPED )) {
        INFO("please ifconfig %s down\n", pNet->name);
        return -EPERM;
    }

    if (!pGobiDev->mbQMIReady) {
        INFO("please wait qmi ready\n");
        return -EBUSY;
    }

    qmap_mode = simple_strtoul(buf, NULL, 10);

    if (pGobiDev->m_qcrmcall_mode) {
        INFO("AT$QCRMCALL MODE had enabled\n");
        return -EINVAL;
    }

    if (qmap_mode <= 0 || qmap_mode > MEIG_WWAN_QMAP) {
        INFO("qmap_mode = %d is Invalid argument, shoule be 1 ~ %d\n", qmap_mode, MEIG_WWAN_QMAP);
        return -EINVAL;
    }

    if (pGobiDev->m_qmap_mode) {
        INFO("qmap_mode aleary set to %d, do not allow re-set again\n", pGobiDev->m_qmap_mode);
        return -EPERM;
    }

    // Setup Data Format
    err = MeigQMIWDASetDataFormat (pGobiDev,
                                   qmap_mode,
                                   &rx_urb_size);
    if (err != 0) {
        return err;
    }

    pDev->rx_urb_size = rx_urb_size;
    pGobiDev->m_qmap_mode = qmap_mode;

    if (pGobiDev->m_qmap_mode > 1) {
        unsigned i;
        for (i = 0; i < pGobiDev->m_qmap_mode; i++) {
            qmap_register_device(pGobiDev, i);
        }
    }
#if (LINUX_VERSION_CODE < KERNEL_VERSION( 5,0,0 ))
#ifdef FLAG_RX_ASSEMBLE
    if (pGobiDev->m_qmap_mode)
        pDev->driver_info->flags |= FLAG_RX_ASSEMBLE;
#endif
#endif

    return count;
}

static DEVICE_ATTR(qmap_mode, S_IWUSR | S_IRUGO, qmap_mode_show, qmap_mode_store);
#endif

static struct attribute *gobinet_sysfs_attrs[] = {
#ifdef CONFIG_BRIDGE
    &dev_attr_bridge_mode.attr,
    &dev_attr_bridge_ipv4.attr,
#endif
#ifdef MEIG_WWAN_QMAP
    &dev_attr_qmap_mode.attr,
#endif
    NULL,
};

static struct attribute_group gobinet_sysfs_attr_group = {
    .attrs = gobinet_sysfs_attrs,
};

#ifdef CONFIG_PM
/*===========================================================================
METHOD:
   GobiNetSuspend (Public Method)

DESCRIPTION:
   Stops QMI traffic while device is suspended

PARAMETERS
   pIntf          [ I ] - Pointer to interface
   powerEvent     [ I ] - Power management event

RETURN VALUE:
   int - 0 for success
         negative errno for failure
===========================================================================*/
static int GobiNetSuspend(
    struct usb_interface *     pIntf,
    pm_message_t               powerEvent )
{
    struct usbnet * pDev;
    sGobiUSBNet * pGobiDev;

    if (pIntf == 0) {
        return -ENOMEM;
    }

#if (LINUX_VERSION_CODE > KERNEL_VERSION( 2,6,23 ))
    pDev = usb_get_intfdata( pIntf );
#else
    pDev = (struct usbnet *)pIntf->dev.platform_data;
#endif

    if (pDev == NULL || pDev->net == NULL) {
        DBG( "failed to get netdevice\n" );
        return -ENXIO;
    }

    pGobiDev = (sGobiUSBNet *)pDev->data[0];
    if (pGobiDev == NULL) {
        DBG( "failed to get QMIDevice\n" );
        return -ENXIO;
    }

    // Is this autosuspend or system suspend?
    //    do we allow remote wakeup?
#if (LINUX_VERSION_CODE < KERNEL_VERSION( 2,6,33 ))
#if (LINUX_VERSION_CODE > KERNEL_VERSION( 2,6,18 ))
    if (pDev->udev->auto_pm == 0)
#else
    if (1)
#endif
#else
    if ((powerEvent.event & PM_EVENT_AUTO) == 0)
#endif
    {
        DBG( "device suspended to power level %d\n",
             powerEvent.event );
        GobiSetDownReason( pGobiDev, DRIVER_SUSPENDED );
    } else {
        DBG( "device autosuspend\n" );
    }

    if (powerEvent.event & PM_EVENT_SUSPEND) {
        // Stop QMI read callbacks
        if (pGobiDev->m_qcrmcall_mode) {
        } else {
            KillRead( pGobiDev );
        }
#if (LINUX_VERSION_CODE > KERNEL_VERSION( 2,6,22 ))
        pDev->udev->reset_resume = 0;
#endif

        // Store power state to avoid duplicate resumes
        pIntf->dev.power.power_state.event = powerEvent.event;
    } else {
        // Other power modes cause QMI connection to be lost
#if (LINUX_VERSION_CODE > KERNEL_VERSION( 2,6,22 ))
        pDev->udev->reset_resume = 1;
#endif
    }

    // Run usbnet's suspend function
    return usbnet_suspend( pIntf, powerEvent );
}
int MeigGobiNetSuspend(struct usb_interface *pIntf, pm_message_t powerEvent )
{
    return GobiNetSuspend(pIntf, powerEvent);
}

/*===========================================================================
METHOD:
   GobiNetResume (Public Method)

DESCRIPTION:
   Resume QMI traffic or recreate QMI device

PARAMETERS
   pIntf          [ I ] - Pointer to interface

RETURN VALUE:
   int - 0 for success
         negative errno for failure
===========================================================================*/
static int GobiNetResume( struct usb_interface * pIntf )
{
    struct usbnet * pDev;
    sGobiUSBNet * pGobiDev;
    int nRet;
    int oldPowerState;

    if (pIntf == 0) {
        return -ENOMEM;
    }

#if (LINUX_VERSION_CODE > KERNEL_VERSION( 2,6,23 ))
    pDev = usb_get_intfdata( pIntf );
#else
    pDev = (struct usbnet *)pIntf->dev.platform_data;
#endif

    if (pDev == NULL || pDev->net == NULL) {
        DBG( "failed to get netdevice\n" );
        return -ENXIO;
    }

    pGobiDev = (sGobiUSBNet *)pDev->data[0];
    if (pGobiDev == NULL) {
        DBG( "failed to get QMIDevice\n" );
        return -ENXIO;
    }

    oldPowerState = pIntf->dev.power.power_state.event;
    pIntf->dev.power.power_state.event = PM_EVENT_ON;
    DBG( "resuming from power mode %d\n", oldPowerState );

    if (oldPowerState & PM_EVENT_SUSPEND) {
        // It doesn't matter if this is autoresume or system resume
        GobiClearDownReason( pGobiDev, DRIVER_SUSPENDED );

        nRet = usbnet_resume( pIntf );
        if (nRet != 0) {
            DBG( "usbnet_resume error %d\n", nRet );
            return nRet;
        }

        // Restart QMI read callbacks
        if (pGobiDev->m_qcrmcall_mode) {
            nRet = 0;
        } else {
            nRet = StartRead( pGobiDev );
        }
        if (nRet != 0) {
            DBG( "StartRead error %d\n", nRet );
            return nRet;
        }

#ifdef CONFIG_PM
#if (LINUX_VERSION_CODE < KERNEL_VERSION( 2,6,29 ))
        // Kick Auto PM thread to process any queued URBs
        complete( &pGobiDev->mAutoPM.mThreadDoWork );
#endif
#endif /* CONFIG_PM */
    } else {
        DBG( "nothing to resume\n" );
        return 0;
    }

    return nRet;
}
#endif /* CONFIG_PM */

/*===========================================================================
METHOD:
   GobiNetDriverBind (Public Method)

DESCRIPTION:
   Setup in and out pipes

PARAMETERS
   pDev           [ I ] - Pointer to usbnet device
   pIntf          [ I ] - Pointer to interface

RETURN VALUE:
   int - 0 for success
         Negative errno for error
===========================================================================*/
static int GobiNetDriverBind(
    struct usbnet *         pDev,
    struct usb_interface *  pIntf )
{
    int numEndpoints;
    int endpointIndex;
    struct usb_host_endpoint * pEndpoint = NULL;
    struct usb_host_endpoint * pIn = NULL;
    struct usb_host_endpoint * pOut = NULL;

    // Verify one altsetting
    if (pIntf->num_altsetting != 1) {
        DBG( "invalid num_altsetting %u\n", pIntf->num_altsetting );
        return -ENODEV;
    }

    // Verify correct interface
    if ( !test_bit(pIntf->cur_altsetting->desc.bInterfaceNumber, &pDev->driver_info->data)) {
        DBG( "invalid interface %d\n",
             pIntf->cur_altsetting->desc.bInterfaceNumber );
        return -ENODEV;
    }


    // Collect In and Out endpoints
    numEndpoints = pIntf->cur_altsetting->desc.bNumEndpoints;
    for (endpointIndex = 0; endpointIndex < numEndpoints; endpointIndex++) {
        pEndpoint = pIntf->cur_altsetting->endpoint + endpointIndex;
        if (pEndpoint == NULL) {
            DBG( "invalid endpoint %u\n", endpointIndex );
            return -ENODEV;
        }

        if (usb_endpoint_dir_in( &pEndpoint->desc ) == true
            &&  usb_endpoint_xfer_int( &pEndpoint->desc ) == false) {
            pIn = pEndpoint;
        } else if (usb_endpoint_dir_out( &pEndpoint->desc ) == true) {
            pOut = pEndpoint;
        }
    }

    if (pIn == NULL || pOut == NULL) {
        DBG( "invalid endpoints\n" );
        return -ENODEV;
    }

    if (usb_set_interface( pDev->udev,
                           pIntf->cur_altsetting->desc.bInterfaceNumber,
                           0 ) != 0) {
        DBG( "unable to set interface\n" );
        return -ENODEV;
    }

    pDev->in = usb_rcvbulkpipe( pDev->udev,
                                pIn->desc.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK );
    pDev->out = usb_sndbulkpipe( pDev->udev,
                                 pOut->desc.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK );

#if defined(MEIG_WWAN_MULTI_PACKAGES)
    if (rx_packets && pDev->udev->descriptor.idVendor == cpu_to_le16(0x05c6)) {
        struct multi_package_config rx_config = {
            .enable = cpu_to_le32(1),
            .package_max_len = cpu_to_le32((1500 + sizeof(struct meig_net_package_header)) * rx_packets),
            .package_max_count_in_queue = cpu_to_le32(rx_packets),
            .timeout = cpu_to_le32(10*1000), //10ms
        };
        int ret = 0;

        ret = usb_control_msg(
                  interface_to_usbdev(pIntf),
                  usb_sndctrlpipe(interface_to_usbdev(pIntf), 0),
                  USB_CDC_SET_MULTI_PACKAGE_COMMAND,
                  0x21, //USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE
                  1,
                  pIntf->cur_altsetting->desc.bInterfaceNumber,
                  &rx_config, sizeof(rx_config), 100);

        DBG( "rx_packets=%d, ret=%d\n", rx_packets, ret);
        if (ret == sizeof(rx_config)) {
            pDev->rx_urb_size = le32_to_cpu(rx_config.package_max_len);
        } else {
            rx_packets = 0;
        }
    }
#endif

#if 1 //def DATA_MODE_RP
    /* make MAC addr easily distinguishable from an IP header */
    if ((pDev->net->dev_addr[0] & 0xd0) == 0x40) {
        /*clear this bit wil make usbnet apdater named as usbX(instead if ethX)*/
        pDev->net->dev_addr[0] |= 0x02;	/* set local assignment bit */
        pDev->net->dev_addr[0] &= 0xbf;	/* clear "IP" bit */
    }
#endif

    DBG( "in %x, out %x\n",
         pIn->desc.bEndpointAddress,
         pOut->desc.bEndpointAddress );

    // In later versions of the kernel, usbnet helps with this
#if (LINUX_VERSION_CODE <= KERNEL_VERSION( 2,6,23 ))
    pIntf->dev.platform_data = (void *)pDev;
#endif

    if (qcrmcall_mode == 0 && pDev->net->sysfs_groups[0] == NULL && gobinet_sysfs_attr_group.attrs[0] != NULL) {
#if (LINUX_VERSION_CODE <= KERNEL_VERSION( 2,6,32)) //see commit 0c509a6c9393b27a8c5a01acd4a72616206cfc24
        pDev->net->sysfs_groups[1] = &gobinet_sysfs_attr_group; //see netdev_register_sysfs()
#else
        pDev->net->sysfs_groups[0] = &gobinet_sysfs_attr_group;
#endif
    }

    if (!pDev->rx_urb_size) {
//to advoid module report mtu 1460, but rx 1500 bytes IP packets, and cause the customer's system crash
//next setting can make usbnet.c:usbnet_change_mtu() do not modify rx_urb_size according to mtu
        pDev->rx_urb_size = ETH_DATA_LEN + ETH_HLEN + 6;
    }

    return 0;
}

/*===========================================================================
METHOD:
   GobiNetDriverUnbind (Public Method)

DESCRIPTION:
   Deregisters QMI device (Registration happened in the probe function)

PARAMETERS
   pDev           [ I ] - Pointer to usbnet device
   pIntfUnused    [ I ] - Pointer to interface

RETURN VALUE:
   None
===========================================================================*/
static void GobiNetDriverUnbind(
    struct usbnet *         pDev,
    struct usb_interface *  pIntf)
{
    sGobiUSBNet * pGobiDev = (sGobiUSBNet *)pDev->data[0];

    // Should already be down, but just in case...
    netif_carrier_off( pDev->net );

    if (pGobiDev->m_qcrmcall_mode) {
    } else {
        DeregisterQMIDevice( pGobiDev );
    }

#if (LINUX_VERSION_CODE >= KERNEL_VERSION( 2,6,29 ))
    kfree( pDev->net->netdev_ops );
    pDev->net->netdev_ops = NULL;
#endif

#if (LINUX_VERSION_CODE <= KERNEL_VERSION( 2,6,23 ))
    pIntf->dev.platform_data = NULL;
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION( 2,6,19 ))
    pIntf->needs_remote_wakeup = 0;
#endif

    if (atomic_dec_and_test(&pGobiDev->refcount))
        kfree( pGobiDev );
    else
        INFO("memory leak!\n");
}

#if 1 //def DATA_MODE_RP
/*===========================================================================
METHOD:
   GobiNetDriverTxFixup (Public Method)

DESCRIPTION:
   Handling data format mode on transmit path

PARAMETERS
   pDev           [ I ] - Pointer to usbnet device
   pSKB           [ I ] - Pointer to transmit packet buffer
   flags          [ I ] - os flags

RETURN VALUE:
   None
===========================================================================*/
static struct sk_buff *GobiNetDriverTxFixup(struct usbnet *dev, struct sk_buff *skb, gfp_t flags)
{
    sGobiUSBNet * pGobiDev = (sGobiUSBNet *)dev->data[0];

    if (!pGobiDev) {
        DBG( "failed to get QMIDevice\n" );
        dev_kfree_skb_any(skb);
        return NULL;
    }

    if (!pGobiDev->mbRawIPMode)
        return skb;

#ifdef MEIG_WWAN_QMAP
    if (pGobiDev->m_qmap_mode) {
        struct qmap_hdr *qhdr;

        if (pGobiDev->m_qmap_mode > 1) {
            qhdr = (struct qmap_hdr *)skb->data;
            if (qhdr->cd_rsvd_pad != 0) {
                goto drop_skb;
            }
            if ((qhdr->mux_id&0xF0) != 0x80) {
                goto drop_skb;
            }
        } else {
            if (ether_to_ip_fixup(dev->net, skb) == NULL)
                goto drop_skb;
            qhdr = (struct qmap_hdr *)skb_push(skb, sizeof(struct qmap_hdr));
            qhdr->cd_rsvd_pad = 0;
            qhdr->mux_id = MEIG_QMAP_MUX_ID;
            qhdr->pkt_len = cpu_to_be16(skb->len - sizeof(struct qmap_hdr));
        }

        return skb;
    }
#endif

#ifdef CONFIG_BRIDGE
    if (pGobiDev->m_bridge_mode) {
        struct ethhdr *ehdr;
        const struct iphdr *iph;

        if (unlikely(skb->len <= ETH_ALEN))
            goto drop_skb;
        skb_reset_mac_header(skb);
        ehdr = eth_hdr(skb);
//meig_debug = 1;
//        DBG("ethhdr: ");
//        PrintHex(ehdr, sizeof(struct ethhdr));

        if (ehdr->h_proto == htons(ETH_P_ARP)) {
            bridge_arp_reply(pGobiDev, skb);
            goto drop_skb;
        }

        iph = ip_hdr(skb);
        //DBG("iphdr: ");
        //PrintHex((void *)iph, sizeof(struct iphdr));

// 1	0.000000000	0.0.0.0	255.255.255.255	DHCP	362	DHCP Request  - Transaction ID 0xe7643ad7
        if (ehdr->h_proto == htons(ETH_P_IP) && iph->protocol == IPPROTO_UDP && iph->saddr == 0x00000000 && iph->daddr == 0xFFFFFFFF) {
            //DBG("udphdr: ");
            //PrintHex(udp_hdr(skb), sizeof(struct udphdr));

            //if (udp_hdr(skb)->dest == htons(67)) //DHCP Request
            {
                int save_debug = meig_debug;
                memcpy(pGobiDev->mHostMAC, ehdr->h_source, ETH_ALEN);
                INFO("PC Mac Address: ");
                meig_debug=1;
                PrintHex(pGobiDev->mHostMAC, ETH_ALEN);
                meig_debug=save_debug;
            }
        }

#if 0
//Ethernet II, Src: DellInc_de:14:09 (f8:b1:56:de:14:09), Dst: IPv4mcast_7f:ff:fa (01:00:5e:7f:ff:fa)
//126	85.213727000	10.184.164.175	239.255.255.250	SSDP	175	M-SEARCH * HTTP/1.1
//Ethernet II, Src: DellInc_de:14:09 (f8:b1:56:de:14:09), Dst: IPv6mcast_16 (33:33:00:00:00:16)
//160	110.305488000	fe80::6819:38ad:fcdc:2444	ff02::16	ICMPv6	90	Multicast Listener Report Message v2
        if (memcmp(ehdr->h_dest, meig_mac, ETH_ALEN) && memcmp(ehdr->h_dest, broadcast_addr, ETH_ALEN)) {
            DBG("Drop h_dest: ");
            PrintHex(ehdr, sizeof(struct ethhdr));
            dev_kfree_skb_any(skb);
            return NULL;
        }
#endif

        if (memcmp(ehdr->h_source, pGobiDev->mHostMAC, ETH_ALEN)) {
            DBG("Drop h_source: ");
            PrintHex(ehdr, sizeof(struct ethhdr));
            goto drop_skb;
        }

//meig_debug = 0;
    }
#endif

    // Skip Ethernet header from message
    if (likely(ether_to_ip_fixup(dev->net, skb))) {
        return skb;
    } else {
#if (LINUX_VERSION_CODE > KERNEL_VERSION( 2,6,22 ))
        dev_err(&dev->intf->dev,  "Packet Dropped ");
#elif (LINUX_VERSION_CODE > KERNEL_VERSION( 2,6,18 ))
        dev_err(dev->net->dev.parent,  "Packet Dropped ");
#else
        INFO("Packet Dropped ");
#endif
    }

drop_skb:
#if (LINUX_VERSION_CODE <= KERNEL_VERSION( 2,6,24 )) && defined(CONFIG_X86_32)
    INFO("dev_kfree_skb_any() will make kernel panic on CentOS!\n");
    meig_debug=1;
    PrintHex(skb->data, 32);
    meig_debug=0;
#else
    // Filter the packet out, release it
    dev_kfree_skb_any(skb);
#endif
    return NULL;
}

#if defined(MEIG_WWAN_MULTI_PACKAGES)
static int GobiNetDriverRxPktsFixup(struct usbnet *dev, struct sk_buff *skb)
{
    sGobiUSBNet * pGobiDev = (sGobiUSBNet *)dev->data[0];

    if (!pGobiDev->mbRawIPMode)
        return 1;

    /* This check is no longer done by usbnet */
    if (skb->len < dev->net->hard_header_len)
        return 0;

    if (!rx_packets) {
        return GobiNetDriverRxFixup(dev, skb);
    }

    while (likely(skb->len)) {
        struct sk_buff* new_skb;
        struct meig_net_package_header package_header;

        if (skb->len < sizeof(package_header))
            return 0;

        memcpy(&package_header, skb->data, sizeof(package_header));
        package_header.payload_len = be16_to_cpu(package_header.payload_len);

        if (package_header.msg_spec != MEIG_NET_MSG_SPEC || package_header.msg_id != MEIG_NET_MSG_ID_IP_DATA)
            return 0;

        if (skb->len < (package_header.payload_len + sizeof(package_header)))
            return 0;

        skb_pull(skb, sizeof(package_header));

        if (skb->len == package_header.payload_len)
            return GobiNetDriverRxFixup(dev, skb);

        new_skb = skb_clone(skb, GFP_ATOMIC);
        if (new_skb) {
            skb_trim(new_skb, package_header.payload_len);
            if (GobiNetDriverRxFixup(dev, new_skb))
                usbnet_skb_return(dev, new_skb);
            else
                return 0;
        }

        skb_pull(skb, package_header.payload_len);
    }

    return 0;
}
#else
#ifdef MEIG_WWAN_QMAP
static int GobiNetDriverRxQmapFixup(struct usbnet *dev, struct sk_buff *skb)
{
    sGobiUSBNet * pGobiDev = (sGobiUSBNet *)dev->data[0];
    static int debug_len = 0;
    int debug_pkts = 0;
    int update_len = skb->len;

    while (skb->len > sizeof(struct qmap_hdr)) {
        struct qmap_hdr *qhdr = (struct qmap_hdr *)skb->data;
        struct net_device *qmap_net;
        struct sk_buff *qmap_skb;
        __be16 proto;
        int pkt_len;
        u8 offset_id = 0;
        int err;
        unsigned len = (be16_to_cpu(qhdr->pkt_len) + sizeof(struct qmap_hdr));

#if 0
        meig_debug = 1;
        DBG("rx: %d\n", skb->len);
        PrintHex(skb->data, 16);
        meig_debug = 0;
#endif

        if (skb->len < (be16_to_cpu(qhdr->pkt_len) + sizeof(struct qmap_hdr))) {
            INFO("drop qmap unknow pkt, len=%d, pkt_len=%d\n", skb->len, be16_to_cpu(qhdr->pkt_len));
            meig_debug = 1;
            PrintHex(skb->data, 16);
            meig_debug = 0;
            goto out;
        }

        debug_pkts++;

        if (qhdr->cd_rsvd_pad & 0x80) {
            INFO("drop qmap command packet %x\n", qhdr->cd_rsvd_pad);
            goto skip_pkt;;
        }

        offset_id = qhdr->mux_id - MEIG_QMAP_MUX_ID;
        if (offset_id >= pGobiDev->m_qmap_mode) {
            INFO("drop qmap unknow mux_id %x\n", qhdr->mux_id);
            goto skip_pkt;
        }

        if (pGobiDev->m_qmap_mode > 1) {
            qmap_net = pGobiDev->mpQmapNetDev[offset_id];
        } else {
            qmap_net = dev->net;
        }

        if (qmap_net == NULL) {
            INFO("drop qmap unknow mux_id %x\n", qhdr->mux_id);
            goto skip_pkt;
        }

        switch (skb->data[sizeof(struct qmap_hdr)] & 0xf0) {
        case 0x40:
            proto = htons(ETH_P_IP);
            break;
        case 0x60:
            proto = htons(ETH_P_IPV6);
            break;
        default:
            goto skip_pkt;
        }

        pkt_len = be16_to_cpu(qhdr->pkt_len) - (qhdr->cd_rsvd_pad&0x3F);
        qmap_skb = netdev_alloc_skb(qmap_net, ETH_HLEN + pkt_len);

        skb_reset_mac_header(qmap_skb);
        memcpy(eth_hdr(qmap_skb)->h_source, meig_mac, ETH_ALEN);
        memcpy(eth_hdr(qmap_skb)->h_dest, qmap_net->dev_addr, ETH_ALEN);
        eth_hdr(qmap_skb)->h_proto = proto;
        memcpy(skb_put(qmap_skb, ETH_HLEN + pkt_len) + ETH_HLEN, skb->data + sizeof(struct qmap_hdr), pkt_len);

        if (pGobiDev->m_qmap_mode > 1) {
            qmap_skb->protocol = eth_type_trans (qmap_skb, qmap_net);
            memset(qmap_skb->cb, 0, sizeof(struct skb_data));
            err = netif_rx(qmap_skb);
#if (LINUX_VERSION_CODE > KERNEL_VERSION( 2,6,14 ))
            if (err == NET_RX_SUCCESS) {
                qmap_net->stats.rx_packets++;
                qmap_net->stats.rx_bytes += qmap_skb->len;
            } else {
                qmap_net->stats.rx_errors++;
            }
#endif
        } else {
            usbnet_skb_return(dev, qmap_skb);
        }

skip_pkt:
        skb_pull(skb, len);
    }

out:
    if (update_len > debug_len) {
        debug_len = update_len;
        INFO("rx_pkts=%d, rx_len=%d\n", debug_pkts, debug_len);
    }
    return 0;
}
#endif
/*===========================================================================
METHOD:
   GobiNetDriverRxFixup (Public Method)

DESCRIPTION:
   Handling data format mode on receive path

PARAMETERS
   pDev           [ I ] - Pointer to usbnet device
   pSKB           [ I ] - Pointer to received packet buffer

RETURN VALUE:
   None
===========================================================================*/
static int GobiNetDriverRxFixup(struct usbnet *dev, struct sk_buff *skb)
{
    __be16 proto;
    sGobiUSBNet * pGobiDev = (sGobiUSBNet *)dev->data[0];

    if (!pGobiDev->mbRawIPMode)
        return 1;

    /* This check is no longer done by usbnet */
    if (skb->len < dev->net->hard_header_len)
        return 0;

#ifdef MEIG_WWAN_QMAP
    if (pGobiDev->m_qmap_mode) {
        return GobiNetDriverRxQmapFixup(dev, skb);
    }
#endif

    switch (skb->data[0] & 0xf0) {
    case 0x40:
        proto = htons(ETH_P_IP);
        break;
    case 0x60:
        proto = htons(ETH_P_IPV6);
        break;
    case 0x00:
        if (is_multicast_ether_addr(skb->data))
            return 1;
        /* possibly bogus destination - rewrite just in case */
        skb_reset_mac_header(skb);
        goto fix_dest;
    default:
        /* pass along other packets without modifications */
        return 1;
    }
    if (skb_headroom(skb) < ETH_HLEN && pskb_expand_head(skb, ETH_HLEN, 0, GFP_ATOMIC)) {
        DBG("%s: couldn't pskb_expand_head\n", __func__);
        return 0;
    }
    skb_push(skb, ETH_HLEN);
    skb_reset_mac_header(skb);
    eth_hdr(skb)->h_proto = proto;
    memcpy(eth_hdr(skb)->h_source, meig_mac, ETH_ALEN);
fix_dest:
#ifdef CONFIG_BRIDGE
    if (pGobiDev->m_bridge_mode) {
        memcpy(eth_hdr(skb)->h_dest, pGobiDev->mHostMAC, ETH_ALEN);
        //memcpy(eth_hdr(skb)->h_dest, broadcast_addr, ETH_ALEN);
    } else
#endif
        memcpy(eth_hdr(skb)->h_dest, dev->net->dev_addr, ETH_ALEN);

#ifdef CONFIG_BRIDGE
#if 0
    if (pGobiDev->m_bridge_mode) {
        struct ethhdr *ehdr = eth_hdr(skb);
        meig_debug = 1;
        DBG(": ");
        PrintHex(ehdr, sizeof(struct ethhdr));
        meig_debug = 0;
    }
#endif
#endif

    return 1;
}
#endif
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION( 2,6,29 ))
#ifdef CONFIG_PM
/*===========================================================================
METHOD:
   GobiUSBNetURBCallback (Public Method)

DESCRIPTION:
   Write is complete, cleanup and signal that we're ready for next packet

PARAMETERS
   pURB     [ I ] - Pointer to sAutoPM struct

RETURN VALUE:
   None
===========================================================================*/
#if (LINUX_VERSION_CODE > KERNEL_VERSION( 2,6,18 ))
void GobiUSBNetURBCallback( struct urb * pURB )
#else
void GobiUSBNetURBCallback(struct urb *pURB, struct pt_regs *regs)
#endif
{
    unsigned long activeURBflags;
    sAutoPM * pAutoPM = (sAutoPM *)pURB->context;
    if (pAutoPM == NULL) {
        // Should never happen
        DBG( "bad context\n" );
        return;
    }

    if (pURB->status != 0) {
        // Note that in case of an error, the behaviour is no different
        DBG( "urb finished with error %d\n", pURB->status );
    }

    // Remove activeURB (memory to be freed later)
    spin_lock_irqsave( &pAutoPM->mActiveURBLock, activeURBflags );

    // EAGAIN used to signify callback is done
    pAutoPM->mpActiveURB = ERR_PTR( -EAGAIN );

    spin_unlock_irqrestore( &pAutoPM->mActiveURBLock, activeURBflags );

    complete( &pAutoPM->mThreadDoWork );

#ifdef URB_FREE_BUFFER_BY_SELF
    if (pURB->transfer_flags & URB_FREE_BUFFER)
        kfree(pURB->transfer_buffer);
#endif
    usb_free_urb( pURB );
}

/*===========================================================================
METHOD:
   GobiUSBNetTXTimeout (Public Method)

DESCRIPTION:
   Timeout declared by the net driver.  Stop all transfers

PARAMETERS
   pNet     [ I ] - Pointer to net device

RETURN VALUE:
   None
===========================================================================*/
void GobiUSBNetTXTimeout( struct net_device * pNet )
{
    struct sGobiUSBNet * pGobiDev;
    sAutoPM * pAutoPM;
    sURBList * pURBListEntry;
    unsigned long activeURBflags, URBListFlags;
    struct usbnet * pDev = netdev_priv( pNet );
    struct urb * pURB;

    if (pDev == NULL || pDev->net == NULL) {
        DBG( "failed to get usbnet device\n" );
        return;
    }

    pGobiDev = (sGobiUSBNet *)pDev->data[0];
    if (pGobiDev == NULL) {
        DBG( "failed to get QMIDevice\n" );
        return;
    }
    pAutoPM = &pGobiDev->mAutoPM;

    DBG( "\n" );

    // Grab a pointer to active URB
    spin_lock_irqsave( &pAutoPM->mActiveURBLock, activeURBflags );
    pURB = pAutoPM->mpActiveURB;
    spin_unlock_irqrestore( &pAutoPM->mActiveURBLock, activeURBflags );
    // Stop active URB
    if (pURB != NULL) {
        usb_kill_urb( pURB );
    }

    // Cleanup URB List
    spin_lock_irqsave( &pAutoPM->mURBListLock, URBListFlags );

    pURBListEntry = pAutoPM->mpURBList;
    while (pURBListEntry != NULL) {
        pAutoPM->mpURBList = pAutoPM->mpURBList->mpNext;
        atomic_dec( &pAutoPM->mURBListLen );
        usb_free_urb( pURBListEntry->mpURB );
        kfree( pURBListEntry );
        pURBListEntry = pAutoPM->mpURBList;
    }

    spin_unlock_irqrestore( &pAutoPM->mURBListLock, URBListFlags );

    complete( &pAutoPM->mThreadDoWork );

    return;
}

/*===========================================================================
METHOD:
   GobiUSBNetAutoPMThread (Public Method)

DESCRIPTION:
   Handle device Auto PM state asynchronously
   Handle network packet transmission asynchronously

PARAMETERS
   pData     [ I ] - Pointer to sAutoPM struct

RETURN VALUE:
   int - 0 for success
         Negative errno for error
===========================================================================*/
static int GobiUSBNetAutoPMThread( void * pData )
{
    unsigned long activeURBflags, URBListFlags;
    sURBList * pURBListEntry;
    int status;
    struct usb_device * pUdev;
    sAutoPM * pAutoPM = (sAutoPM *)pData;
    struct urb * pURB;

    if (pAutoPM == NULL) {
        DBG( "passed null pointer\n" );
        return -EINVAL;
    }

    pUdev = interface_to_usbdev( pAutoPM->mpIntf );

    DBG( "traffic thread started\n" );

    while (pAutoPM->mbExit == false) {
        // Wait for someone to poke us
        wait_for_completion_interruptible( &pAutoPM->mThreadDoWork );

        // Time to exit?
        if (pAutoPM->mbExit == true) {
            // Stop activeURB
            spin_lock_irqsave( &pAutoPM->mActiveURBLock, activeURBflags );
            pURB = pAutoPM->mpActiveURB;
            spin_unlock_irqrestore( &pAutoPM->mActiveURBLock, activeURBflags );

            // EAGAIN used to signify callback is done
            if (IS_ERR( pAutoPM->mpActiveURB )
                &&  PTR_ERR( pAutoPM->mpActiveURB ) == -EAGAIN ) {
                pURB = NULL;
            }

            if (pURB != NULL) {
                usb_kill_urb( pURB );
            }
            // Will be freed in callback function

            // Cleanup URB List
            spin_lock_irqsave( &pAutoPM->mURBListLock, URBListFlags );

            pURBListEntry = pAutoPM->mpURBList;
            while (pURBListEntry != NULL) {
                pAutoPM->mpURBList = pAutoPM->mpURBList->mpNext;
                atomic_dec( &pAutoPM->mURBListLen );
                usb_free_urb( pURBListEntry->mpURB );
                kfree( pURBListEntry );
                pURBListEntry = pAutoPM->mpURBList;
            }

            spin_unlock_irqrestore( &pAutoPM->mURBListLock, URBListFlags );

            break;
        }

        // Is our URB active?
        spin_lock_irqsave( &pAutoPM->mActiveURBLock, activeURBflags );

        // EAGAIN used to signify callback is done
        if (IS_ERR( pAutoPM->mpActiveURB )
            &&  PTR_ERR( pAutoPM->mpActiveURB ) == -EAGAIN ) {
            pAutoPM->mpActiveURB = NULL;

            // Restore IRQs so task can sleep
            spin_unlock_irqrestore( &pAutoPM->mActiveURBLock, activeURBflags );

            // URB is done, decrement the Auto PM usage count
            usb_autopm_put_interface( pAutoPM->mpIntf );

            // Lock ActiveURB again
            spin_lock_irqsave( &pAutoPM->mActiveURBLock, activeURBflags );
        }

        if (pAutoPM->mpActiveURB != NULL) {
            // There is already a URB active, go back to sleep
            spin_unlock_irqrestore( &pAutoPM->mActiveURBLock, activeURBflags );
            continue;
        }

        // Is there a URB waiting to be submitted?
        spin_lock_irqsave( &pAutoPM->mURBListLock, URBListFlags );
        if (pAutoPM->mpURBList == NULL) {
            // No more URBs to submit, go back to sleep
            spin_unlock_irqrestore( &pAutoPM->mURBListLock, URBListFlags );
            spin_unlock_irqrestore( &pAutoPM->mActiveURBLock, activeURBflags );
            continue;
        }

        // Pop an element
        pURBListEntry = pAutoPM->mpURBList;
        pAutoPM->mpURBList = pAutoPM->mpURBList->mpNext;
        atomic_dec( &pAutoPM->mURBListLen );
        spin_unlock_irqrestore( &pAutoPM->mURBListLock, URBListFlags );

        // Set ActiveURB
        pAutoPM->mpActiveURB = pURBListEntry->mpURB;
        spin_unlock_irqrestore( &pAutoPM->mActiveURBLock, activeURBflags );

        // Tell autopm core we need device woken up
        status = usb_autopm_get_interface( pAutoPM->mpIntf );
        if (status < 0) {
            DBG( "unable to autoresume interface: %d\n", status );

            // likely caused by device going from autosuspend -> full suspend
            if (status == -EPERM) {
#if (LINUX_VERSION_CODE < KERNEL_VERSION( 2,6,33 ))
#if (LINUX_VERSION_CODE > KERNEL_VERSION( 2,6,18 ))
                pUdev->auto_pm = 0;
#else
                pUdev = pUdev;
#endif
#endif
                GobiNetSuspend( pAutoPM->mpIntf, PMSG_SUSPEND );
            }

            // Add pURBListEntry back onto pAutoPM->mpURBList
            spin_lock_irqsave( &pAutoPM->mURBListLock, URBListFlags );
            pURBListEntry->mpNext = pAutoPM->mpURBList;
            pAutoPM->mpURBList = pURBListEntry;
            atomic_inc( &pAutoPM->mURBListLen );
            spin_unlock_irqrestore( &pAutoPM->mURBListLock, URBListFlags );

            spin_lock_irqsave( &pAutoPM->mActiveURBLock, activeURBflags );
            pAutoPM->mpActiveURB = NULL;
            spin_unlock_irqrestore( &pAutoPM->mActiveURBLock, activeURBflags );

            // Go back to sleep
            continue;
        }

        // Submit URB
        status = usb_submit_urb( pAutoPM->mpActiveURB, GFP_KERNEL );
        if (status < 0) {
            // Could happen for a number of reasons
            DBG( "Failed to submit URB: %d.  Packet dropped\n", status );
            spin_lock_irqsave( &pAutoPM->mActiveURBLock, activeURBflags );
            usb_free_urb( pAutoPM->mpActiveURB );
            pAutoPM->mpActiveURB = NULL;
            spin_unlock_irqrestore( &pAutoPM->mActiveURBLock, activeURBflags );
            usb_autopm_put_interface( pAutoPM->mpIntf );

            // Loop again
            complete( &pAutoPM->mThreadDoWork );
        }

        kfree( pURBListEntry );
    }

    DBG( "traffic thread exiting\n" );
    pAutoPM->mpThread = NULL;
    return 0;
}

/*===========================================================================
METHOD:
   GobiUSBNetStartXmit (Public Method)

DESCRIPTION:
   Convert sk_buff to usb URB and queue for transmit

PARAMETERS
   pNet     [ I ] - Pointer to net device

RETURN VALUE:
   NETDEV_TX_OK on success
   NETDEV_TX_BUSY on error
===========================================================================*/
int GobiUSBNetStartXmit(
    struct sk_buff *     pSKB,
    struct net_device *  pNet )
{
    unsigned long URBListFlags;
    struct sGobiUSBNet * pGobiDev;
    sAutoPM * pAutoPM;
    sURBList * pURBListEntry, ** ppURBListEnd;
    void * pURBData;
    struct usbnet * pDev = netdev_priv( pNet );

    //DBG( "\n" );

    if (pDev == NULL || pDev->net == NULL) {
        DBG( "failed to get usbnet device\n" );
        return NETDEV_TX_BUSY;
    }

    pGobiDev = (sGobiUSBNet *)pDev->data[0];
    if (pGobiDev == NULL) {
        DBG( "failed to get QMIDevice\n" );
        return NETDEV_TX_BUSY;
    }
    pAutoPM = &pGobiDev->mAutoPM;

    if( NULL == pSKB ) {
        DBG( "Buffer is NULL \n" );
        return NETDEV_TX_BUSY;
    }

    if (GobiTestDownReason( pGobiDev, DRIVER_SUSPENDED )) {
        // Should not happen
        DBG( "device is suspended\n" );
        dump_stack();
        return NETDEV_TX_BUSY;
    }

    if (GobiTestDownReason( pGobiDev, NO_NDIS_CONNECTION )) {
        //netif_carrier_off( pGobiDev->mpNetDev->net );
        //DBG( "device is disconnected\n" );
        //dump_stack();
        return NETDEV_TX_BUSY;
    }

    // Convert the sk_buff into a URB

    // Check if buffer is full
    if ( atomic_read( &pAutoPM->mURBListLen ) >= txQueueLength) {
        DBG( "not scheduling request, buffer is full\n" );
        return NETDEV_TX_BUSY;
    }

    // Allocate URBListEntry
    pURBListEntry = kmalloc( sizeof( sURBList ), GFP_ATOMIC );
    if (pURBListEntry == NULL) {
        DBG( "unable to allocate URBList memory\n" );
        return NETDEV_TX_BUSY;
    }
    pURBListEntry->mpNext = NULL;

    // Allocate URB
    pURBListEntry->mpURB = usb_alloc_urb( 0, GFP_ATOMIC );
    if (pURBListEntry->mpURB == NULL) {
        DBG( "unable to allocate URB\n" );
        // release all memory allocated by now
        if (pURBListEntry)
            kfree( pURBListEntry );
        return NETDEV_TX_BUSY;
    }

#if 1 //def DATA_MODE_RP
    GobiNetDriverTxFixup(pDev, pSKB, GFP_ATOMIC);
#endif

    // Allocate URB transfer_buffer
    pURBData = kmalloc( pSKB->len, GFP_ATOMIC );
    if (pURBData == NULL) {
        DBG( "unable to allocate URB data\n" );
        // release all memory allocated by now
        if (pURBListEntry) {
            usb_free_urb( pURBListEntry->mpURB );
            kfree( pURBListEntry );
        }
        return NETDEV_TX_BUSY;
    }
    // Fill with SKB's data
    memcpy( pURBData, pSKB->data, pSKB->len );

    usb_fill_bulk_urb( pURBListEntry->mpURB,
                       pGobiDev->mpNetDev->udev,
                       pGobiDev->mpNetDev->out,
                       pURBData,
                       pSKB->len,
                       GobiUSBNetURBCallback,
                       pAutoPM );

    /* Handle the need to send a zero length packet and release the
     * transfer buffer
     */
    pURBListEntry->mpURB->transfer_flags |= (URB_ZERO_PACKET | URB_FREE_BUFFER);

    // Aquire lock on URBList
    spin_lock_irqsave( &pAutoPM->mURBListLock, URBListFlags );

    // Add URB to end of list
    ppURBListEnd = &pAutoPM->mpURBList;
    while ((*ppURBListEnd) != NULL) {
        ppURBListEnd = &(*ppURBListEnd)->mpNext;
    }
    *ppURBListEnd = pURBListEntry;
    atomic_inc( &pAutoPM->mURBListLen );

    spin_unlock_irqrestore( &pAutoPM->mURBListLock, URBListFlags );

    complete( &pAutoPM->mThreadDoWork );

    // Start transfer timer
    pNet->trans_start = jiffies;
    // Free SKB
    if (pSKB)
        dev_kfree_skb_any( pSKB );

    return NETDEV_TX_OK;
}
#endif
static int (*local_usbnet_start_xmit) (struct sk_buff *skb, struct net_device *net);
#endif

static int GobiUSBNetStartXmit2( struct sk_buff *pSKB, struct net_device *pNet )
{
    struct sGobiUSBNet * pGobiDev;
    struct usbnet * pDev = netdev_priv( pNet );

    //DBG( "\n" );

    if (pDev == NULL || pDev->net == NULL) {
        DBG( "failed to get usbnet device\n" );
        return NETDEV_TX_BUSY;
    }

    pGobiDev = (sGobiUSBNet *)pDev->data[0];
    if (pGobiDev == NULL) {
        DBG( "failed to get QMIDevice\n" );
        return NETDEV_TX_BUSY;
    }

    if( NULL == pSKB ) {
        DBG( "Buffer is NULL \n" );
        return NETDEV_TX_BUSY;
    }

    if (GobiTestDownReason( pGobiDev, DRIVER_SUSPENDED )) {
        // Should not happen
        DBG( "device is suspended\n" );
        dump_stack();
        return NETDEV_TX_BUSY;
    }

    if (GobiTestDownReason( pGobiDev, NO_NDIS_CONNECTION )) {
        //netif_carrier_off( pGobiDev->mpNetDev->net );
        //DBG( "device is disconnected\n" );
        //dump_stack();
        return NETDEV_TX_BUSY;
    }

#if (LINUX_VERSION_CODE < KERNEL_VERSION( 2,6,29 ))
    return local_usbnet_start_xmit(pSKB, pNet);
#else
    return usbnet_start_xmit(pSKB, pNet);
#endif
}

/*===========================================================================
METHOD:
   GobiUSBNetOpen (Public Method)

DESCRIPTION:
   Wrapper to usbnet_open, correctly handling autosuspend
   Start AutoPM thread (if CONFIG_PM is defined)

PARAMETERS
   pNet     [ I ] - Pointer to net device

RETURN VALUE:
   int - 0 for success
         Negative errno for error
===========================================================================*/
static int GobiUSBNetOpen( struct net_device * pNet )
{
    int status = 0;
    struct sGobiUSBNet * pGobiDev;
    struct usbnet * pDev = netdev_priv( pNet );

    if (pDev == NULL) {
        DBG( "failed to get usbnet device\n" );
        return -ENXIO;
    }

    pGobiDev = (sGobiUSBNet *)pDev->data[0];
    if (pGobiDev == NULL) {
        DBG( "failed to get QMIDevice\n" );
        return -ENXIO;
    }

    DBG( "\n" );

#ifdef CONFIG_PM
#if (LINUX_VERSION_CODE < KERNEL_VERSION( 2,6,29 ))
    // Start the AutoPM thread
    pGobiDev->mAutoPM.mpIntf = pGobiDev->mpIntf;
    pGobiDev->mAutoPM.mbExit = false;
    pGobiDev->mAutoPM.mpURBList = NULL;
    pGobiDev->mAutoPM.mpActiveURB = NULL;
    spin_lock_init( &pGobiDev->mAutoPM.mURBListLock );
    spin_lock_init( &pGobiDev->mAutoPM.mActiveURBLock );
    atomic_set( &pGobiDev->mAutoPM.mURBListLen, 0 );
    init_completion( &pGobiDev->mAutoPM.mThreadDoWork );

    pGobiDev->mAutoPM.mpThread = kthread_run( GobiUSBNetAutoPMThread,
                                 &pGobiDev->mAutoPM,
                                 "GobiUSBNetAutoPMThread" );
    if (IS_ERR( pGobiDev->mAutoPM.mpThread )) {
        DBG( "AutoPM thread creation error\n" );
        return PTR_ERR( pGobiDev->mAutoPM.mpThread );
    }
#endif
#endif /* CONFIG_PM */

    // Allow traffic
    GobiClearDownReason( pGobiDev, NET_IFACE_STOPPED );

    // Pass to usbnet_open if defined
    if (pGobiDev->mpUSBNetOpen != NULL) {
        status = pGobiDev->mpUSBNetOpen( pNet );
#ifdef CONFIG_PM
        // If usbnet_open was successful enable Auto PM
        if (status == 0) {
#if (LINUX_VERSION_CODE < KERNEL_VERSION( 2,6,33 ))
            usb_autopm_enable( pGobiDev->mpIntf );
#else
            usb_autopm_put_interface( pGobiDev->mpIntf );
#endif
        }
#endif /* CONFIG_PM */
    } else {
        DBG( "no USBNetOpen defined\n" );
    }

    return status;
}

/*===========================================================================
METHOD:
   GobiUSBNetStop (Public Method)

DESCRIPTION:
   Wrapper to usbnet_stop, correctly handling autosuspend
   Stop AutoPM thread (if CONFIG_PM is defined)

PARAMETERS
   pNet     [ I ] - Pointer to net device

RETURN VALUE:
   int - 0 for success
         Negative errno for error
===========================================================================*/
static int GobiUSBNetStop( struct net_device * pNet )
{
    struct sGobiUSBNet * pGobiDev;
    struct usbnet * pDev = netdev_priv( pNet );

    if (pDev == NULL || pDev->net == NULL) {
        DBG( "failed to get netdevice\n" );
        return -ENXIO;
    }

    pGobiDev = (sGobiUSBNet *)pDev->data[0];
    if (pGobiDev == NULL) {
        DBG( "failed to get QMIDevice\n" );
        return -ENXIO;
    }

    // Stop traffic
    GobiSetDownReason( pGobiDev, NET_IFACE_STOPPED );

#ifdef CONFIG_PM
#if (LINUX_VERSION_CODE < KERNEL_VERSION( 2,6,29 ))
    // Tell traffic thread to exit
    pGobiDev->mAutoPM.mbExit = true;
    complete( &pGobiDev->mAutoPM.mThreadDoWork );

    // Wait for it to exit
    while( pGobiDev->mAutoPM.mpThread != NULL ) {
        msleep( 100 );
    }
    DBG( "thread stopped\n" );
#endif
#endif /* CONFIG_PM */

    // Pass to usbnet_stop, if defined
    if (pGobiDev->mpUSBNetStop != NULL) {
        return pGobiDev->mpUSBNetStop( pNet );
    } else {
        return 0;
    }
}

/*=========================================================================*/
// Struct driver_info
/*=========================================================================*/
static struct driver_info GobiNetInfo = {
    .description   = "Meig GobiNet Ethernet Device",
#ifdef CONFIG_ANDROID
    .flags         = FLAG_ETHER | FLAG_POINTTOPOINT, //usb0
#else
    .flags         = FLAG_ETHER,
#endif
    .bind          = GobiNetDriverBind,
    .unbind        = GobiNetDriverUnbind,
#if 1 //def DATA_MODE_RP
#if defined(MEIG_WWAN_MULTI_PACKAGES)
    .rx_fixup      = GobiNetDriverRxPktsFixup,
#else
    .rx_fixup      = GobiNetDriverRxFixup,
#endif
    .tx_fixup      = GobiNetDriverTxFixup,
#endif
    .data          = (1 << 5),
};

/*=========================================================================*/
// Qualcomm Gobi 3000 VID/PIDs
/*=========================================================================*/
#define GOBI_FIXED_INTF(vend, prod) \
    { \
          USB_DEVICE( vend, prod ), \
          .driver_info = (unsigned long)&GobiNetInfo, \
    }
static const struct usb_device_id MeigGobiVIDPIDTable [] = {
    GOBI_FIXED_INTF( 0x05c6, 0xf601 ), // SLM750V
    GOBI_FIXED_INTF( 0x2dee, 0x4d22 ), // SRM815
    //Terminating entry
    { }
};

MODULE_DEVICE_TABLE( usb, MeigGobiVIDPIDTable );

/*===========================================================================
METHOD:
   GobiUSBNetProbe (Public Method)

DESCRIPTION:
   Run usbnet_probe
   Setup QMI device

PARAMETERS
   pIntf        [ I ] - Pointer to interface
   pVIDPIDs     [ I ] - Pointer to VID/PID table

RETURN VALUE:
   int - 0 for success
         Negative errno for error
===========================================================================*/
static int GobiUSBNetProbe(
    struct usb_interface *        pIntf,
    const struct usb_device_id *  pVIDPIDs )
{
    int status;
    struct usbnet * pDev;
    sGobiUSBNet * pGobiDev;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION( 2,6,29 ))
    struct net_device_ops * pNetDevOps;
#endif

    status = usbnet_probe( pIntf, pVIDPIDs );
    if (status < 0) {
        DBG( "usbnet_probe failed %d\n", status );
        return status;
    }

#if (LINUX_VERSION_CODE >= KERNEL_VERSION( 2,6,19 ))
    pIntf->needs_remote_wakeup = 1;
#endif

#if (LINUX_VERSION_CODE > KERNEL_VERSION( 2,6,23 ))
    pDev = usb_get_intfdata( pIntf );
#else
    pDev = (struct usbnet *)pIntf->dev.platform_data;
#endif

    if (pDev == NULL || pDev->net == NULL) {
        DBG( "failed to get netdevice\n" );
        usbnet_disconnect( pIntf );
        return -ENXIO;
    }

    pGobiDev = kzalloc( sizeof( sGobiUSBNet ), GFP_KERNEL );
    if (pGobiDev == NULL) {
        DBG( "falied to allocate device buffers" );
        usbnet_disconnect( pIntf );
        return -ENOMEM;
    }

    atomic_set(&pGobiDev->refcount, 1);

    pDev->data[0] = (unsigned long)pGobiDev;

    pGobiDev->mpNetDev = pDev;

    // Clearing endpoint halt is a magic handshake that brings
    // the device out of low power (airplane) mode
    usb_clear_halt( pGobiDev->mpNetDev->udev, pDev->out );

    // Overload PM related network functions
#if (LINUX_VERSION_CODE < KERNEL_VERSION( 2,6,29 ))
    pGobiDev->mpUSBNetOpen = pDev->net->open;
    pDev->net->open = GobiUSBNetOpen;
    pGobiDev->mpUSBNetStop = pDev->net->stop;
    pDev->net->stop = GobiUSBNetStop;
#if defined(CONFIG_PM) && (LINUX_VERSION_CODE > KERNEL_VERSION( 2,6,14 ))
    pDev->net->hard_start_xmit = GobiUSBNetStartXmit;
    pDev->net->tx_timeout = GobiUSBNetTXTimeout;
#else
    local_usbnet_start_xmit = pDev->net->hard_start_xmit;
    pDev->net->hard_start_xmit = GobiUSBNetStartXmit2;
#endif
#else
    pNetDevOps = kmalloc( sizeof( struct net_device_ops ), GFP_KERNEL );
    if (pNetDevOps == NULL) {
        DBG( "falied to allocate net device ops" );
        usbnet_disconnect( pIntf );
        return -ENOMEM;
    }
    memcpy( pNetDevOps, pDev->net->netdev_ops, sizeof( struct net_device_ops ) );

    pGobiDev->mpUSBNetOpen = pNetDevOps->ndo_open;
    pNetDevOps->ndo_open = GobiUSBNetOpen;
    pGobiDev->mpUSBNetStop = pNetDevOps->ndo_stop;
    pNetDevOps->ndo_stop = GobiUSBNetStop;
#if 1
    pNetDevOps->ndo_start_xmit = GobiUSBNetStartXmit2;
#else
    pNetDevOps->ndo_start_xmit = usbnet_start_xmit;
#endif
    pNetDevOps->ndo_tx_timeout = usbnet_tx_timeout;

    pDev->net->netdev_ops = pNetDevOps;
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION( 2,6,31 ))
    memset( &(pGobiDev->mpNetDev->stats), 0, sizeof( struct net_device_stats ) );
#else
    memset( &(pGobiDev->mpNetDev->net->stats), 0, sizeof( struct net_device_stats ) );
#endif

    pGobiDev->mpIntf = pIntf;
    memset( &(pGobiDev->mMEID), '0', 14 );

    DBG( "Mac Address:\n" );
    PrintHex( &pGobiDev->mpNetDev->net->dev_addr[0], 6 );

    pGobiDev->mbQMIValid = false;
    memset( &pGobiDev->mQMIDev, 0, sizeof( sQMIDev ) );
    pGobiDev->mQMIDev.mbCdevIsInitialized = false;

    pGobiDev->mQMIDev.mpDevClass = gpClass;

#ifdef CONFIG_PM
#if (LINUX_VERSION_CODE < KERNEL_VERSION( 2,6,29 ))
    init_completion( &pGobiDev->mAutoPM.mThreadDoWork );
#endif
#endif /* CONFIG_PM */
    spin_lock_init( &pGobiDev->mQMIDev.mClientMemLock );

    // Default to device down
    pGobiDev->mDownReason = 0;

//#if (LINUX_VERSION_CODE < KERNEL_VERSION( 3,11,0 ))
    GobiSetDownReason( pGobiDev, NO_NDIS_CONNECTION );
    GobiSetDownReason( pGobiDev, NET_IFACE_STOPPED );
//#endif

    // Register QMI
    // pGobiDev->mbMdm9x07 |= (pDev->udev->descriptor.idVendor == cpu_to_le16(0x05c6));
    pGobiDev->mbMdm9x07 = true; //only support 9x07 chipset
    pGobiDev->mbRawIPMode = pGobiDev->mbMdm9x07;
    pGobiDev->mbRawIPMode = true;
    if ( pGobiDev->mbRawIPMode)
        pGobiDev->mpNetDev->net->flags |= IFF_NOARP;
#ifdef CONFIG_BRIDGE
    memcpy(pGobiDev->mHostMAC, pDev->net->dev_addr, 6);
    pGobiDev->m_bridge_mode = bridge_mode;
#endif

#ifdef MEIG_REMOVE_TX_ZLP
    {
        struct remove_tx_zlp_config {
            __le32 enable;
        } __packed;

        struct remove_tx_zlp_config cfg;
        cfg.enable = cpu_to_le32(1);  //1-enable  0-disable

        usb_control_msg(
            interface_to_usbdev(pIntf),
            usb_sndctrlpipe(interface_to_usbdev(pIntf), 0),
            USB_CDC_SET_REMOVE_TX_ZLP_COMMAND,
            0x21, //USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE
            0,
            pIntf->cur_altsetting->desc.bInterfaceNumber,
            &cfg, sizeof(cfg), 100);
    }
#endif

    pGobiDev->m_qcrmcall_mode = qcrmcall_mode;

    if (pGobiDev->m_qcrmcall_mode) {
        INFO("AT$QCRMCALL MODE!");

        GobiClearDownReason( pGobiDev, NO_NDIS_CONNECTION );
        usb_control_msg(
            interface_to_usbdev(pIntf),
            usb_sndctrlpipe(interface_to_usbdev(pIntf), 0),
            0x22, //USB_CDC_REQ_SET_CONTROL_LINE_STATE
            0x21, //USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE
            1, //active CDC DTR
            pIntf->cur_altsetting->desc.bInterfaceNumber,
            NULL, 0, 100);
        status = 0;
    } else {
        if (pGobiDev->mbRawIPMode) {
            pGobiDev->m_qmap_mode = qmap_mode;
        }
        status = RegisterQMIDevice( pGobiDev );
    }

    if (status != 0) {
        // usbnet_disconnect() will call GobiNetDriverUnbind() which will call
        // DeregisterQMIDevice() to clean up any partially created QMI device
        usbnet_disconnect( pIntf );
        return status;
    }

#if defined(MEIG_WWAN_QMAP)
    if (pGobiDev->m_qmap_mode > 1) {
        unsigned i;
        for (i = 0; i < pGobiDev->m_qmap_mode; i++) {
            qmap_register_device(pGobiDev, i);
        }
    }
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION( 5,0,0 ))
#ifdef FLAG_RX_ASSEMBLE
    if (pGobiDev->m_qmap_mode)
        pDev->driver_info->flags |= FLAG_RX_ASSEMBLE;
#endif
#endif
    // Success
    return 0;
}

static void GobiUSBNetDisconnect (struct usb_interface *intf)
{
#if defined(MEIG_WWAN_QMAP)
    struct usbnet *pDev = usb_get_intfdata(intf);
    sGobiUSBNet * pGobiDev = (sGobiUSBNet *)pDev->data[0];
    unsigned i;

    for (i = 0; i < pGobiDev->m_qmap_mode; i++) {
        qmap_unregister_device(pGobiDev, i);
    }
#endif

    usbnet_disconnect(intf);
}

static struct usb_driver GobiNet = {
    .name       = "GobiNet",
    .id_table   = MeigGobiVIDPIDTable,
    .probe      = GobiUSBNetProbe,
    .disconnect = GobiUSBNetDisconnect,
#ifdef CONFIG_PM
    .suspend    = GobiNetSuspend,
    .resume     = GobiNetResume,
#if (LINUX_VERSION_CODE > KERNEL_VERSION( 2,6,18 ))
    .supports_autosuspend = true,
#endif
#endif /* CONFIG_PM */
};

/*===========================================================================
METHOD:
   GobiUSBNetModInit (Public Method)

DESCRIPTION:
   Initialize module
   Create device class
   Register out usb_driver struct

RETURN VALUE:
   int - 0 for success
         Negative errno for error
===========================================================================*/
static int __init GobiUSBNetModInit( void )
{
    gpClass = class_create( THIS_MODULE, "GobiQMI" );
    if (IS_ERR( gpClass ) == true) {
        DBG( "error at class_create %ld\n", PTR_ERR( gpClass ) );
        return -ENOMEM;
    }

    // This will be shown whenever driver is loaded
    printk( KERN_INFO "%s: %s\n", DRIVER_DESC, DRIVER_VERSION );

    return usb_register( &GobiNet );
}
module_init( GobiUSBNetModInit );

/*===========================================================================
METHOD:
   GobiUSBNetModExit (Public Method)

DESCRIPTION:
   Deregister module
   Destroy device class

RETURN VALUE:
   void
===========================================================================*/
static void __exit GobiUSBNetModExit( void )
{
    usb_deregister( &GobiNet );

    class_destroy( gpClass );
}
module_exit( GobiUSBNetModExit );

MODULE_VERSION( DRIVER_VERSION );
MODULE_AUTHOR( DRIVER_AUTHOR );
MODULE_DESCRIPTION( DRIVER_DESC );
MODULE_LICENSE("Dual BSD/GPL");

#ifdef bool
#undef bool
#endif

module_param_named( debug, meig_debug, int, S_IRUGO | S_IWUSR );
MODULE_PARM_DESC( debug, "Debuging enabled or not" );

//module_param_named( interruptible, Meiginterruptible, int, S_IRUGO | S_IWUSR );
//MODULE_PARM_DESC( interruptible, "Listen for and return on user interrupt" );
module_param( txQueueLength, int, S_IRUGO | S_IWUSR );
MODULE_PARM_DESC( txQueueLength,
                  "Number of IP packets which may be queued up for transmit" );

