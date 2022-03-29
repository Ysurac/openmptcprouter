/* * Copyright (c) 2012 - 2013 The Linux Foundation. All rights reserved.* */

/*
 * This is the network dependent layer to handle network related functionality.
 * This file is tightly coupled to neworking frame work of linux 2.6.xx kernel.
 * The functionality carried out in this file should be treated as an example only
 * if the underlying operating system is not Linux.
 *
 * \note Many of the functions other than the device specific functions
 *  changes for operating system other than Linux 2.6.xx
 * \internal
 *-----------------------------REVISION HISTORY-----------------------------------
 * Ubicom		01/Mar/2010			Modified for Ubicom32
 * Synopsys		01/Aug/2007			Created
 */

#include "synopGMAC_plat.h"
#include "synopGMAC_Dev.h"
#include <asm/arch-ipq806x/nss/msm_ipq806x_gmac.h>
#include <asm/arch-ipq806x/nss/clock.h>
#include <asm/arch-ipq806x/nss/nss_reg.h>
#include <asm/arch-ipq806x/gpio.h>
#include <asm/arch-ipq806x/clock.h>
#include <common.h>
#include <linux/mii.h>


#define CONFIG_ARCH_IPQ806X
/*
 * u-boot API
 */
extern int eth_rx(void);
extern void eth_halt(void);
extern int eth_init(bd_t *bd);
extern void cleanup_skb(void);

extern int synop_phy_link_status(int speed);
void synop_phy_outofreset(void);
extern s32 synopGMAC_check_link(synopGMACdevice *gmacdev);
extern uint16_t mii_read_reg(synopGMACdevice *gmacdev,  uint32_t phy, uint32_t reg);
int eth_init_done;
u32 nss_base;
u32 gmac_base;
u32 qsgmii_base;
uint16_t ipq_mii_read_reg(uint32_t phy, uint32_t reg);
void ipq_mii_write_reg(uint32_t phy, uint32_t reg, uint16_t data);

#define GMAC_PORT    1 /* after bringup will be moved to board specific params */
#define TX_TOTAL_BUFSIZE  1700

u8 txbuffs[TX_TOTAL_BUFSIZE];
u8 TxSkbuf[sizeof(struct sk_buff)];

/*
 * These are the global pointers for their respecive structures
 */
static synopGMACdevice *synopGMACDev[1] = {NULL};

synopGMACdevice *netdev_priv(struct eth_device *dev) {
        return (synopGMACdevice *)(dev->priv);
}

static s32 gmac_plat_init(void)
{

	/*XXX: needs cleanup. need better way to share registers between different modules. */
        nss_base = NSS_REG_BASE ;
        gmac_base = gboard_param->gmac_base;
        qsgmii_base = QSGMII_REG_BASE;

	TR("%s: ipq806x: NSS base done. \n",__FUNCTION__);

	return 0;
}


static s32 gmac_dev_init(u32 id)
{
	u32 val;

	/* Set GMACn Ctl */
	val = GMAC_PHY_RGMII | GMAC_IFG_CTL(GMAC_IFG)
			| GMAC_IFG_LIMIT(GMAC_IFG);
	synopGMACSetBits((u32 *)nss_base, NSS_GMACn_CTL(id), val);

	val = synopGMACReadReg((u32 *)nss_base, NSS_GMACn_CTL(id));
	TR("%s: IPQ806X_GMAC%d_CTL(0x%x) - 0x%x\n", __FUNCTION__, id ,
		NSS_GMACn_CTL(id), val);

	/* Enable clk for GMACn */
	val = GMACn_RGMII_RX_CLK(id) | GMACn_RGMII_TX_CLK(id)
		| GMACn_PTP_CLK(id);
	synopGMACSetBits((u32 *)nss_base, NSS_ETH_CLK_GATE_CTL, val);

	val = synopGMACReadReg((u32 *)nss_base, NSS_ETH_CLK_GATE_CTL);
	TR("%s:NSS_ETHERNET_CLK_GATE_CTL(0x%x) - 0x%x  \n", __FUNCTION__ ,
		NSS_ETH_CLK_GATE_CTL, val);

	/* Select GMACn clk source.
	 * TODO: support for SGMII/QSGMII\
	*/

	val = synopGMACReadReg((u32 *)nss_base, NSS_ETH_CLK_SRC_CTL);
	TR("%s:NSS_ETHERNET_CLK_SRC_CTL(0x%x)- 0x%x\n", __FUNCTION__,
		NSS_ETH_CLK_SRC_CTL, val);

	/* Read status registers */
	val = synopGMACReadReg((u32 *)nss_base, NSS_ETH_CLK_ROOT_STAT );
	TR("%s:CLK_ROOT_STAT - 0x%x  \n", __FUNCTION__, val);

	val = synopGMACReadReg((u32 *)nss_base, NSS_QSGMII_CLK_CTL);
	TR("%s:QSGMII_CLK_CTL - 0x%x  \n", __FUNCTION__, val);

	synopGMACSetBits((u32 *)nss_base, NSS_ETH_CLK_SRC_CTL, (0x1 << id));

        return 0;
}


static s32 gmac_spare_ctl(synopGMACdevice *gmacdev)
{
	u32 val;
	u32 count;
	u32 id = 0;

	val = 1 << id;
	synopGMACSetBits((u32 *)nss_base, NSS_ETH_SPARE_CTL , val);

	val = synopGMACReadReg((u32 *)nss_base, NSS_ETH_SPARE_CTL );
	TR("NSS_ETHERNET_SPARE_CTL  - 0x%x\n",val);

	val = 1 << id;
	synopGMACClearBits((u32 *)nss_base, NSS_ETH_SPARE_CTL , val);

	val = synopGMACReadReg((u32 *)nss_base, NSS_ETH_SPARE_CTL);
	TR("NSS_ETHERNET_SPARE_CTL  - 0x%x after clear for gmac %d\n",val,id);

	val = synopGMACReadReg((u32 *)nss_base, NSS_ETH_SPARE_STAT) ;
	TR("NSS_ETHERNET_SPARE_STAT  - 0x%x; gmac %d spare ctl reset...\n",val,id);
	count =0;
	while((val & (1 << id)) != (1 << id)) {
		__udelay(1000);
		val = synopGMACReadReg((u32 *)nss_base, NSS_ETH_SPARE_STAT) ;
		if(count++ > 20) {
			TR0("!!!!!!!!!!! Timeout waiting for ETH_SPARE_STAT bit to set. Moving on...\n");
			break;
		}
	}

	return 0;
}

static void synopGMAC_linux_powerup_mac(synopGMACdevice *gmacdev)
{
	gmacdev->GMAC_Power_down = 0;	// Let ISR know that MAC is out of power down now
	if (synopGMAC_is_magic_packet_received(gmacdev))
		TR("GMAC wokeup due to Magic Pkt Received\n");
	if (synopGMAC_is_wakeup_frame_received(gmacdev))
		TR("GMAC wokeup due to Wakeup Frame Received\n");
	/*Disable the assertion of PMT interrupt*/
	synopGMAC_pmt_int_disable(gmacdev);
	/*Enable the mac and Dma rx and tx paths*/
	synopGMAC_rx_enable(gmacdev);
	synopGMAC_enable_dma_rx(gmacdev);

	synopGMAC_tx_enable(gmacdev);
	synopGMAC_enable_dma_tx(gmacdev);
}

/*
 * This sets up the transmit Descriptor queue in ring or chain mode.
 * This function is tightly coupled to the platform and operating system
 * Device is interested only after the descriptors are setup. Therefore this function
 * is not included in the device driver API. This function should be treated as an
 * example code to design the descriptor structures for ring mode or chain mode.
 * This function depends on the device structure for allocation consistent dma-able memory in case of linux.
 *	- Allocates the memory for the descriptors.
 *	- Initialize the Busy and Next descriptors indices to 0(Indicating first descriptor).
 *	- Initialize the Busy and Next descriptors to first descriptor address.
 * 	- Initialize the last descriptor with the endof ring in case of ring mode.
 *	- Initialize the descriptors in chain mode.
 * @param[in] pointer to synopGMACdevice.
 * @param[in] pointer to device structure.
 * @param[in] number of descriptor expected in tx descriptor queue.
 * @param[in] whether descriptors to be created in RING mode or CHAIN mode.
 * \return 0 upon success. Error code upon failure.
 * \note This function fails if allocation fails for required number of descriptors in Ring mode, but in chain mode
 *  function returns -ESYNOPGMACNOMEM in the process of descriptor chain creation. once returned from this function
 *  user should for gmacdev->TxDescCount to see how many descriptors are there in the chain. Should continue further
 *  only if the number of descriptors in the chain meets the requirements
 */
static s32 synopGMAC_setup_tx_desc_queue(synopGMACdevice * gmacdev, void * dev,u32 no_of_desc, u32 desc_mode)
{
	s32 i;
	DmaDesc *first_desc = NULL;
	dma_addr_t dma_addr;
	gmacdev->TxDescCount = 0;

	BUG_ON(desc_mode != RINGMODE);
	BUG_ON((no_of_desc & (no_of_desc - 1)) != 0);	/* Must be power-of-2 */

	TR("Total size of memory required for Tx Descriptors in Ring Mode = 0x%08x\n",
		(u32)((sizeof(DmaDesc) * no_of_desc)));
	first_desc = plat_alloc_consistent_dmaable_memory(dev, sizeof(DmaDesc) * no_of_desc,&dma_addr);
	if (first_desc == NULL) {
		TR("Error in Tx Descriptors memory allocation\n");
		return -ESYNOPGMACNOMEM;
	}

	BUG_ON((int)first_desc & (CACHE_LINE_SIZE - 1));
	gmacdev->TxDescCount = no_of_desc;
	gmacdev->TxDesc      = first_desc;
	gmacdev->TxDescDma   = dma_addr;
	TR("Tx Descriptors in Ring Mode: No. of descriptors = %d base = 0x%08x dma = 0x%08x\n",
		no_of_desc, (u32)first_desc, dma_addr);

	for (i =0; i < gmacdev -> TxDescCount; i++) {
		synopGMAC_tx_desc_init_ring(gmacdev->TxDesc + i, i == (gmacdev->TxDescCount - 1));
		TR("%02d %08x \n",i, (unsigned int)(gmacdev->TxDesc + i) );
	}

	gmacdev->TxNext = 0;
	gmacdev->TxBusy = 0;
	gmacdev->TxNextDesc = gmacdev->TxDesc;
	gmacdev->TxBusyDesc = gmacdev->TxDesc;
	gmacdev->BusyTxDesc  = 0;

	return -ESYNOPGMACNOERR;
}

/*
 * This sets up the receive Descriptor queue in ring or chain mode.
 * This function is tightly coupled to the platform and operating system
 * Device is interested only after the descriptors are setup. Therefore this function
 * is not included in the device driver API. This function should be treated as an
 * example code to design the descriptor structures in ring mode or chain mode.
 * This function depends on the device structure for allocation of consistent dma-able memory in case of linux.
 *	- Allocates the memory for the descriptors.
 *	- Initialize the Busy and Next descriptors indices to 0(Indicating first descriptor).
 *	- Initialize the Busy and Next descriptors to first descriptor address.
 * 	- Initialize the last descriptor with the endof ring in case of ring mode.
 *	- Initialize the descriptors in chain mode.
 * @param[in] pointer to synopGMACdevice.
 * @param[in] pointer to device structure.
 * @param[in] number of descriptor expected in rx descriptor queue.
 * @param[in] whether descriptors to be created in RING mode or CHAIN mode.
 * \return 0 upon success. Error code upon failure.
 * \note This function fails if allocation fails for required number of descriptors in Ring mode, but in chain mode
 *  function returns -ESYNOPGMACNOMEM in the process of descriptor chain creation. once returned from this function
 *  user should for gmacdev->RxDescCount to see how many descriptors are there in the chain. Should continue further
 *  only if the number of descriptors in the chain meets the requirements
 */
static s32 synopGMAC_setup_rx_desc_queue(synopGMACdevice * gmacdev,void * dev,u32 no_of_desc, u32 desc_mode)
{
	s32 i;
	DmaDesc *first_desc = NULL;
	dma_addr_t dma_addr;
	gmacdev->RxDescCount = 0;

	BUG_ON(desc_mode != RINGMODE);
	BUG_ON((no_of_desc & (no_of_desc - 1)) != 0);	/* Must be power-of-2 */

	TR("total size of memory required for Rx Descriptors in Ring Mode = 0x%08x\n",
		(u32)((sizeof(DmaDesc) * no_of_desc)));
	first_desc = plat_alloc_consistent_dmaable_memory (dev, sizeof(DmaDesc) * no_of_desc, &dma_addr);
	if (first_desc == NULL) {
		TR("Error in Rx Descriptor Memory allocation in Ring mode\n");
		return -ESYNOPGMACNOMEM;
	}
	BUG_ON((int)first_desc & (CACHE_LINE_SIZE - 1));
	gmacdev->RxDescCount = no_of_desc;
	gmacdev->RxDesc      = first_desc;
	gmacdev->RxDescDma   = dma_addr;
	TR("Rx Descriptors in Ring Mode: No. of descriptors = %d base = 0x%08x dma = 0x%08x\n",
		no_of_desc, (u32)first_desc, dma_addr);

	for (i =0; i < gmacdev -> RxDescCount; i++) {
		synopGMAC_rx_desc_init_ring(gmacdev->RxDesc + i, i == (gmacdev->RxDescCount - 1));
		TR("%02d %08x \n",i, (unsigned int)(gmacdev->RxDesc + i));
	}

	gmacdev->RxNext = 0;
	gmacdev->RxBusy = 0;
	gmacdev->RxNextDesc = gmacdev->RxDesc;
	gmacdev->RxBusyDesc = gmacdev->RxDesc;
	gmacdev->BusyRxDesc   = 0;

	return -ESYNOPGMACNOERR;
}

/*
 * This gives up the receive Descriptor queue in ring or chain mode.
 * This function is tightly coupled to the platform and operating system
 * Once device's Dma is stopped the memory descriptor memory and the buffer memory deallocation,
 * is completely handled by the operating system, this call is kept outside the device driver Api.
 * This function should be treated as an example code to de-allocate the descriptor structures in ring mode or chain mode
 * and network buffer deallocation.
 * This function depends on the device structure for dma-able memory deallocation for both descriptor memory and the
 * network buffer memory under linux.
 * The responsibility of this function is to
 *     - Free the network buffer memory if any.
 *	- Fee the memory allocated for the descriptors.
 * @param[in] pointer to synopGMACdevice.
 * @param[in] pointer to device structure.
 * @param[in] number of descriptor expected in rx descriptor queue.
 * @param[in] whether descriptors to be created in RING mode or CHAIN mode.
 * \return 0 upon success. Error code upon failure.
 * \note No referece should be made to descriptors once this function is called. This function is invoked when the device is closed.
 */
static void synopGMAC_giveup_rx_desc_queue(synopGMACdevice * gmacdev, void *dev, u32 desc_mode)
{
	s32 i;
	u32 status;
	dma_addr_t dma_addr1;
	u32 length1;
	u32 data1;

	for (i = 0; i < gmacdev->RxDescCount; i++) {
		synopGMAC_get_desc_data(gmacdev->RxDesc + i, &status, &dma_addr1, &length1, &data1);
		if ((length1 != 0) && (data1 != 0)) {
			dev_kfree_skb_any((struct sk_buff *) data1);
			TR("(Ring mode) rx buffer1 %08x of size %d from %d rx descriptor is given back\n", data1, length1, i);
		}
	}
	free(gmacdev->RxDesc);
	TR("Memory allocated %08x  for Rx Desriptors (ring) is given back\n", (u32)gmacdev->RxDesc);
	TR("rx-------------------------------------------------------------------rx\n");

	gmacdev->RxDesc    = NULL;
	gmacdev->RxDescDma = 0;
}

/*
 * This gives up the transmit Descriptor queue in ring or chain mode.
 * This function is tightly coupled to the platform and operating system
 * Once device's Dma is stopped the memory descriptor memory and the buffer memory deallocation,
 * is completely handled by the operating system, this call is kept outside the device driver Api.
 * This function should be treated as an example code to de-allocate the descriptor structures in ring mode or chain mode
 * and network buffer deallocation.
 * This function depends on the device structure for dma-able memory deallocation for both descriptor memory and the
 * network buffer memory under linux.
 * The responsibility of this function is to
 *     - Free the network buffer memory if any.
 *	- Fee the memory allocated for the descriptors.
 * @param[in] pointer to synopGMACdevice.
 * @param[in] pointer to device structure.
 * @param[in] number of descriptor expected in tx descriptor queue.
 * @param[in] whether descriptors to be created in RING mode or CHAIN mode.
 * \return 0 upon success. Error code upon failure.
 * \note No reference should be made to descriptors once this function is called. This function is invoked when the device is closed.
 */
static void synopGMAC_giveup_tx_desc_queue(synopGMACdevice * gmacdev,void * dev, u32 desc_mode)
{
	s32 i;
	u32 status;
	dma_addr_t dma_addr1;
	u32 length1;
	u32 data1;

	for (i = 0; i < gmacdev->TxDescCount; i++) {
		synopGMAC_get_desc_data(gmacdev->TxDesc + i, &status, &dma_addr1, &length1, &data1);
		if((length1 != 0) && (data1 != 0)){
			dev_kfree_skb_any((struct sk_buff *) data1);
			TR("(Ring mode) tx buffer1 %08x of size %d from %d rx descriptor is given back\n", data1, length1, i);
		}
	}
	free(gmacdev->TxDesc);
	TR("Memory allocated %08x for Tx Desriptors (ring) is given back\n", (u32)gmacdev->TxDesc);
	TR("tx-------------------------------------------------------------------tx\n");

	gmacdev->TxDesc    = NULL;
	gmacdev->TxDescDma = 0;
}

/*
 * Function to handle housekeeping after a packet is transmitted over the wire.
 * After the transmission of a packet DMA generates corresponding interrupt
 * (if it is enabled). It takes care of returning the sk_buff to the linux
 * kernel, updating the networking statistics and tracking the descriptors.
 * @param[in] pointer to net_device structure.
 * \return void.
 * \note This function runs in interrupt context
 */
static void synopGMAC_handle_transmit_over(struct eth_device *netdev)
{
	synopGMACdevice *gmacdev = netdev_priv(netdev);
	DmaDesc *txdesc;
	u32 status;

	/*
	 * Handle the transmit Descriptors
	 */
	while (1) {
		txdesc = synopGMAC_get_tx_qptr(gmacdev);
		if (unlikely(txdesc == NULL)) {
			break;
		}

		status = txdesc->status;
		synopGMAC_reset_tx_qptr(gmacdev);

		if (unlikely(synopGMAC_is_tx_ipv4header_checksum_error(gmacdev, status))) {
			TR0("Harware Failed to Insert IPV4 Header Checksum\n");
		}

		if (unlikely(synopGMAC_is_tx_payload_checksum_error(gmacdev, status))) {
			TR0("Harware Failed to Insert Payload Checksum\n");
		}

		if (likely(synopGMAC_is_desc_valid(status))) {
		}
		else {
			TR0("Error in Status %08x\n",status);
		}
	}
	netif_wake_queue(netdev);
}


/*
 * Function to Receive a packet from the interface.
 * After Receiving a packet, DMA transfers the received packet to the system memory
 * and generates corresponding interrupt (if it is enabled). This function prepares
 * the sk_buff for received packet after removing the ethernet CRC, and hands it over
 * to linux networking stack.
 * 	- Updataes the networking interface statistics
 *	- Keeps track of the rx descriptors
 * @param[in] pointer to net_device structure.
 * \return void.
 * \note This function runs in interrupt context.
 */
static u32 synopGMAC_handle_received_data(struct eth_device *netdev, u32 quota)
{
	synopGMACdevice *gmacdev = netdev_priv(netdev);
	DmaDesc *rxdesc;
	u32 status;
	u32 len;
	u32 data1;
	u32 dma_addr1;
	struct sk_buff *skb;
	u32 count = 0, refill_count = 0, reserve_len;

	/*
	 * Handle the Receive Descriptors
	 */
	while (count < quota) {
		rxdesc = synopGMAC_get_rx_qptr(gmacdev);

		if (unlikely(rxdesc == NULL)) {

			break;
		}

		status = rxdesc->status;
		dma_addr1 = rxdesc->buffer1;
		data1 = rxdesc->data1;
		synopGMAC_reset_rx_qptr(gmacdev);

		BUG_ON(data1 == 0);

		/*
		 * Perform prefetch on skbuff structure data
		 */
		skb = (struct sk_buff *)data1;

		/*
		 * Validate RX frame:
		 *	The max frame size check is implied by buffer's limited length
		 *	and the presence of both first and last descritpor flags.
		 */
		len = synopGMAC_get_rx_desc_frame_length(status);

		if (unlikely(!synopGMAC_is_rx_desc_valid(status)) ||
		    unlikely(len < (ETH_ZLEN + ETH_FCS_LEN))) {
			/*
			 * At first step unmapped the dma address
			 */
			//dma_unmap_single_buf(dma_addr1, 0);

			/*
			 * Now the present skb should be set free
			 */
			dev_kfree_skb_any(skb);
			TR0("%s: Invalid RX status = %08x\n", __FUNCTION__, status);

			count++;

			continue;
		}

		/*
		 * At first step unmapped the dma address
		 */
		len -= ETH_FCS_LEN; //Not interested in Ethernet CRC bytes
		skb_put(skb, len);
		count++;
		NetReceive(skb->data, skb->len);
		dev_kfree_skb_any(skb);
	}

	/* U-Boot could invoke eth_halt and halt our operations. */
	if (gmacdev->state == ETH_STATE_PASSIVE)
		return count;

	/*
	 * Now allocate more RX buffer and let GMAC DMA engine know about them.
	 */
	reserve_len = ETHERNET_EXTRA;
	while (gmacdev->BusyRxDesc < gmacdev->RxDescCount) {

		skb = alloc_skb(ETHERNET_EXTRA + ETH_MAX_FRAME_LEN +
				CACHE_LINE_SIZE, 0);
		if(skb == NULL){
			TR("SKB memory allocation failed \n");
			break;
		}

		skb_reserve(skb, reserve_len);
		dma_addr1 = virt_to_phys(skb->data);
		synopGMAC_set_rx_qptr(gmacdev, dma_addr1, ETH_MAX_FRAME_LEN,
					(u32)skb);
		refill_count++;
	}

	if (refill_count > 0) {
		synopGMAC_resume_dma_rx(gmacdev);
	}

	return count;
}


/*
 * Tasklet poll callback
 * This is the Tasklet callback function
 * @param[in] tasklet structure.
 *
 */
static void synopGMAC_task_poll(unsigned long arg)
{
	struct eth_device *netdev;
	synopGMACdevice *gmacdev;
	u32 interrupt;
        u32 budget = GMAC_POLL_BUDGET;

	gmacdev  = (synopGMACdevice *)arg;
	BUG_ON(gmacdev == NULL);

	netdev  = gmacdev->synopGMACnetdev;
	BUG_ON(netdev == NULL);

	/*
	 * Now lets handle the DMA interrupts
	 */
	interrupt = synopGMAC_get_interrupt_type(gmacdev);
	TR("%s:Interrupts to be handled: 0x%08x\n", __FUNCTION__, interrupt);

	if (unlikely(interrupt & GmacPmtIntr)) {
		TR("%s:: Interrupt due to PMT module\n",__FUNCTION__);
		synopGMAC_linux_powerup_mac(gmacdev);
	}

	if (unlikely(interrupt & GmacMmcIntr)) {
		TR("%s:: Interrupt due to MMC module\n",__FUNCTION__);
		TR("%s:: synopGMAC_rx_int_status = %08x\n",__FUNCTION__,synopGMAC_read_mmc_rx_int_status(gmacdev));
		TR("%s:: synopGMAC_tx_int_status = %08x\n",__FUNCTION__,synopGMAC_read_mmc_tx_int_status(gmacdev));
	}

	if (unlikely(interrupt & GmacLineIntfIntr)) {
		TR("%s:: Interrupt due to GMAC LINE module\n",__FUNCTION__);
		TR("GMAC status reg is %08x mask is %08x\n",
			synopGMACReadReg((u32 *)gmacdev->MacBase, GmacInterruptStatus),
			synopGMACReadReg((u32 *)gmacdev->MacBase, GmacInterruptMask));
		if (synopGMACReadReg((u32 *)gmacdev->MacBase, GmacInterruptStatus) & GmacRgmiiIntSts) {
			TR("GMAC RGMII status is %08x\n", synopGMACReadReg((u32 *)gmacdev->MacBase, 0x00D8));
			synopGMACReadReg((u32 *)gmacdev->MacBase, 0x00D8);
		}
	}
	if (interrupt && DmaIntRxCompleted) {
	     synopGMAC_handle_received_data(netdev, budget);
	}

	if (gmacdev->BusyTxDesc > 0) {
		synopGMAC_handle_transmit_over(netdev);
	}

	if (unlikely(interrupt & (DmaIntErrorMask
			| DmaIntRxAbnMask | DmaIntRxStoppedMask
			| DmaIntTxAbnMask | DmaIntTxStoppedMask))) {

		if (interrupt & DmaIntErrorMask) {
			TR0("%s::Fatal Bus Error Inetrrupt Seen (DMA status = 0x%08x)\n",__FUNCTION__,interrupt);
			BUG();

		}

		if (interrupt & DmaIntRxAbnMask) {
			TR0("%s::Abnormal Rx Interrupt Seen (DMA status = 0x%08x)\n", __FUNCTION__, interrupt);
			if (gmacdev->GMAC_Power_down == 0) {
				/*
				 * Just issue a poll demand to resume DMA operation
				 * (May happen when too few RX desciptors are in use)
				 */
				synopGMAC_handle_received_data(netdev, 0);
				synopGMAC_resume_dma_rx(gmacdev);
			}
		}

		if (interrupt & DmaIntRxStoppedMask) {
			if (gmacdev->GMAC_Power_down == 0) {
				TR0("%s::Receiver stopped (DMA status = 0x%08x)\n", __FUNCTION__, interrupt);
				BUG();
				synopGMAC_handle_received_data(netdev, 0);
				synopGMAC_enable_dma_rx(gmacdev);
			}
		}

		if (interrupt & DmaIntTxAbnMask) {
			TR0("%s::Abnormal Tx Interrupt Seen (DMA status = 0x%08x)\n", __FUNCTION__, interrupt);
			if (gmacdev->GMAC_Power_down == 0) {
			       synopGMAC_handle_transmit_over(netdev);
			}
		}

		if (interrupt & DmaIntTxStoppedMask) {
			if (gmacdev->GMAC_Power_down == 0) {
				TR0("%s::Transmitter stopped (DMA status = 0x%08x)\n", __FUNCTION__, interrupt);
				BUG();

				synopGMAC_disable_dma_tx(gmacdev);
				synopGMAC_take_desc_ownership_tx(gmacdev);
				synopGMAC_enable_dma_tx(gmacdev);
				netif_wake_queue(netdev);
			}
		}
	}
}

/*
 * Function used when the interface is opened for use.
 * We register synopGMAC_linux_open function to linux open(). Basically this
 * function prepares the the device for operation . This function is called whenever ifconfig (in Linux)
 * activates the device (for example "ifconfig eth0 up"). This function registers
 * system resources needed
 * 	- Attaches device to device specific structure
 * 	- Programs the MDC clock for PHY configuration
 * 	- Check and initialize the PHY interface
 *	- ISR registration
 * 	- Setup and initialize Tx and Rx descriptors
 *	- Initialize MAC and DMA
 *	- Allocate Memory for RX descriptors (The should be DMAable)
 * 	- Initialize one second timer to detect cable plug/unplug
 *	- Configure and Enable Interrupts
 *	- Enable Tx and Rx
 *	- start the Linux network queue interface
 * @param[in] pointer to net_device structure.
 * \return Returns 0 on success and error status upon failure.
 * 
 */
static s32 synopGMAC_linux_open(struct eth_device *netdev,
				struct eth_device *device)
{
	void *dev = NULL;;
	synopGMACdevice *gmacdev = (synopGMACdevice *) netdev_priv(netdev);

	TR("%s called \n",__FUNCTION__);
	TR("gmacdev = %08x netdev = %08x dev= %08x\n", (u32)gmacdev, (u32)netdev, (u32)dev);
	BUG_ON(gmacdev->synopGMACnetdev != netdev);

	/*
	 * Now platform dependent initialization.
	 */
	synopGMAC_disable_interrupt_all(gmacdev);

	/*
	 * Lets reset the IP
	 */
	synopGMAC_reset(gmacdev);

	/*
	 * Lets read the version of ip in to device structure
	 */
	synopGMAC_read_version(gmacdev);
	synopGMAC_set_mac_addr(gmacdev, GmacAddr0High, GmacAddr0Low, device->enetaddr);


	/*
	 * Set up the tx and rx descriptor queue/ring
	 */
	if (synopGMAC_setup_tx_desc_queue(gmacdev, dev, TRANSMIT_DESC_SIZE, RINGMODE) < 0) {
		TR0("Error in setup TX descriptor\n");
		return -ESYNOPGMACNOMEM;
	}
	synopGMAC_init_tx_desc_base(gmacdev);	//Program the transmit descriptor base address in to DmaTxBase addr

	if (synopGMAC_setup_rx_desc_queue(gmacdev, dev, RECEIVE_DESC_SIZE, RINGMODE) < 0) {
		TR0("Error in setup RX descriptor\n");
		synopGMAC_giveup_tx_desc_queue(gmacdev, dev, RINGMODE);
		return -ESYNOPGMACNOMEM;
	}
	synopGMAC_init_rx_desc_base(gmacdev);	/*Program the receive descriptor base address in to DmaTxBase addr*/

	synopGMAC_dma_bus_mode_init(gmacdev, DmaFixedBurstEnable | DmaBurstLength16 | DmaDescriptorSkip0 | DmaDescriptor8Words | DmaArbitPr); /*pbl32 incr with rxthreshold 128 and Desc is 8 Words */
	synopGMAC_dma_control_init(gmacdev, DmaStoreAndForward | DmaRxThreshCtrl128 | DmaTxSecondFrame);

	/*
	 * Initialize the mac interface
	 */
	synopGMAC_mac_init(gmacdev);
	synopGMAC_pause_control(gmacdev); // This enables the pause control in Full duplex mode of operation
	/*
	 * IPC Checksum offloading is enabled for this driver. Should only be used if Full Ip checksumm offload engine is configured in the hardware
	 */
	synopGMAC_enable_rx_chksum_offload(gmacdev);  	//Enable the offload engine in the receive path
	synopGMAC_rx_tcpip_chksum_drop_enable(gmacdev); // This is default configuration, DMA drops the packets if error in encapsulated ethernet payload
							// The FEF bit in DMA control register is configured to 0 indicating DMA to drop the errored frames.

	synopGMAC_handle_received_data(netdev, 0);

	synopGMAC_clear_interrupt(gmacdev);

	/*
	 * Disable the interrupts generated by MMC and IPC counters.
	 * If these are not disabled ISR should be modified accordingly to handle these interrupts.
	 */
	synopGMAC_disable_mmc_tx_interrupt(gmacdev, 0xFFFFFFFF);
	synopGMAC_disable_mmc_rx_interrupt(gmacdev, 0xFFFFFFFF);
	synopGMAC_disable_mmc_ipc_rx_interrupt(gmacdev, 0xFFFFFFFF);

	synopGMAC_enable_dma_rx(gmacdev);
	synopGMAC_enable_dma_tx(gmacdev);

	netif_start_queue(netdev);
	synopGMAC_enable_interrupt(gmacdev, DmaIntEnable);

	return 0;
}

/*
 * Function used when the interface is closed.
 *
 * This function is registered to linux stop() function. This function is
 * called whenever ifconfig (in Linux) closes the device (for example "ifconfig eth0 down").
 * This releases all the system resources allocated during open call.
 * system resources int needs
 * 	- Disable the device interrupts
 * 	- Stop the receiver and get back all the rx descriptors from the DMA
 * 	- Stop the transmitter and get back all the tx descriptors from the DMA
 * 	- Stop the Linux network queue interface
 *	- Free the irq (ISR registered is removed from the kernel)
 * 	- Release the TX and RX descripor memory
 *	- De-initialize one second timer rgistered for cable plug/unplug tracking
 * @param[in] pointer to net_device structure.
 * \return Returns 0 on success and error status upon failure.
 * \callgraph
 */
static s32 synopGMAC_linux_close(struct eth_device *netdev)
{
	synopGMACdevice *gmacdev = (synopGMACdevice *) netdev_priv(netdev);
	void *dev = NULL;

	TR("%s\n",__FUNCTION__);
	BUG_ON(gmacdev->synopGMACnetdev != netdev);

	/*
	 * Disable all the interrupts
	 */
	synopGMAC_disable_interrupt_all(gmacdev);
	TR("the synopGMAC interrupt has been disabled\n");

	/*
	 * Disable the reception
	 */
	synopGMAC_rx_disable(gmacdev);
	/*Allow any pending buffer to be read by host */
	__udelay(10000);
	synopGMAC_disable_dma_rx(gmacdev);
	synopGMAC_take_desc_ownership_rx(gmacdev);
	TR("the synopGMAC Reception has been disabled\n");

	/*
	 * Disable the transmission
	 */
	synopGMAC_disable_dma_tx(gmacdev);
	/* allow any pending transmission to complete */
	__udelay(10000);
	synopGMAC_tx_disable(gmacdev);
	synopGMAC_take_desc_ownership_tx(gmacdev);
	TR("the synopGMAC Transmission has been disabled\n");
	netif_stop_queue(netdev);

	/*
	 * Free the Rx Descriptor contents
	 */
	TR("Now calling synopGMAC_giveup_rx_desc_queue \n");
	synopGMAC_giveup_rx_desc_queue(gmacdev, dev, RINGMODE);
	TR("Now calling synopGMAC_giveup_tx_desc_queue \n");
	synopGMAC_giveup_tx_desc_queue(gmacdev, dev, RINGMODE);

	return -ESYNOPGMACNOERR;
}

/*
 * Function to transmit a given packet on the wire.
 * Whenever Linux Kernel has a packet ready to be transmitted, this function is called.
 * The function prepares a packet and prepares the descriptor and
 * enables/resumes the transmission.
 * @param[in] pointer to sk_buff structure.
 * @param[in] pointer to net_device structure.
 * \return Returns 0 on success and Error code on failure.
 * \note structure sk_buff is used to hold packet in Linux networking stacks.
 */
static s32 synopGMAC_linux_xmit_frames(struct sk_buff *skb, struct eth_device *netdev)
{
	u32 dma_addr;
	synopGMACdevice *gmacdev;

	TR("%s called \n",__FUNCTION__);
	BUG_ON(skb == NULL);
	BUG_ON((skb->len < ETH_HLEN) || (skb->len > ETH_MAX_FRAME_LEN));
	if ((skb->len < ETH_HLEN) || (skb->len > ETH_MAX_FRAME_LEN))
		goto drop;

	gmacdev = (synopGMACdevice *) netdev_priv(netdev);
	if (gmacdev == NULL)
		goto drop;
	BUG_ON(gmacdev->synopGMACnetdev != netdev);

	if (gmacdev->BusyTxDesc == gmacdev->TxDescCount) {
		/*
		 * Return busy here. The TX will be re-tried
		 */
		TR0("%s No More Free Tx Descriptors\n",__FUNCTION__);
		netif_stop_queue(netdev);
		return NETDEV_TX_BUSY;
	}

	/*
	 * Now we have skb ready and OS invoked this function. Lets make our DMA know about this
	 */
	dma_addr = virt_to_phys(skb->data);
	synopGMAC_set_tx_qptr(gmacdev, dma_addr, skb->len, (u32)skb, 0);

	/*
	 * Now force the DMA to start transmission
	 */
	synopGMAC_resume_dma_tx(gmacdev);
	TR("%s called \n",__FUNCTION__);
	BUG_ON(skb == NULL);
	return NETDEV_TX_OK;

drop:
	/*
	 * Now drop it
	 */
	dev_kfree_skb_any(skb);
	return NETDEV_TX_OK;
}

/*
 * Function to initialize the Linux network interface.
 *
 * Linux dependent Network interface is setup here. This provides
 * an example to handle the network dependent functionality.
 *
 * \return Returns 0 on success and Error code on failure.
 */
s32  synopGMAC_init_network_interface(void)
{
	u32 i;
	u32 id;
	s32 err = 0;
	struct eth_device *netdev;
	synopGMACdevice *gmacdev;
	const char *eth_name[2] = {"eth_lan", "eth_wan"};;

	cleanup_skb();

	for (i = 0; i < 1; i++) {

		/*
		 * Lets allocate and set up an ethernet device, it takes the sizeof the private structure.
		 *  This is mandatory as a 32 byte allignment is required for the private data structure.
		 */
		netdev = malloc(sizeof(struct eth_device));
		if(!netdev){
			TR0("Problem in alloc_etherdev()..Take Necessary action\n");
			err = -ESYNOPGMACNOMEM;
			break;
		}
		memcpy(netdev->name, eth_name[i], sizeof(netdev->name) - 1);

		/*
		 * Allocate Memory for the the GMACip structure
		 */
		gmacdev = malloc(sizeof(synopGMACdevice));
		if(!gmacdev){
			TR0("Error in Memory Allocataion \n");
			free(netdev);
			err = -ESYNOPGMACNOMEM;
			break;
		}
		netdev->priv = gmacdev;
		gmac_plat_init();
		gmacdev->phyid = gboard_param->phy_id; /* 6 or 4 */
		gmacdev->synopGMACMappedAddr = gmac_base;
	        TR("Initializing synopsys GMAC interface at port = 0x%x\n",
		gmacdev->synopGMACMappedAddr) ;

		id = ((gmac_base - NSS_GMAC0_BASE) / NSS_GMAC_REG_LEN);
		if (id > 3)
			return -EINVAL;

		gmac_dev_init(id);

		/*
		 * Attach the device to MAC struct This will configure all the required base addresses
		 * such as Mac base, configuration base, phy base address (out of 32 possible phys)
		 */
		synopGMAC_attach(gmacdev, gmacdev->synopGMACMappedAddr);
		gmacdev->synopGMACnetdev = netdev;

		/*
		 * This just fill in some default MAC address
		 */
		eth_getenv_enetaddr("ethaddr", netdev->dev_addr);
		synopGMACDev[i] = gmacdev;
	}

	if (synopGMACDev[0] == NULL) {
		TR0("no native Ethernet interface found\n");
		return err;
	}

	return 0;
}

/*
 * Function to initialize the Linux network interface.
 * Linux dependent Network interface is setup here. This provides
 * an example to handle the network dependent functionality.
 * \return Returns 0 on success and Error code on failure.
 */
void  synopGMAC_exit_network_interface(void)
{
	u32 i;
	synopGMACdevice *gmacdev;

	TR0("Now Calling network_unregister\n");
	for (i = 0; i < 1; i++) {
		gmacdev = synopGMACDev[i];
		if (gmacdev) {
			free(gmacdev->synopGMACnetdev);
			free(gmacdev);
			synopGMACDev[i] = NULL;
		}
	}

	cleanup_skb();
}

/***************************************************/

void ubi32_eth_cleanup(void)
{
	TR("ubi32_eth_cleanup\n");

	eth_init_done = 0;
	synopGMAC_exit_network_interface();
}

int  ipq_gmac_eth_send(struct eth_device *dev, void *packet, int len)
{
	synopGMACdevice *gmacdev = synopGMACDev[0];

	struct sk_buff *skb  = (struct sk_buff *)TxSkbuf;

	skb->data = (u8*)txbuffs;

	if (!gmacdev) {
		goto dropped;
	}
	if (!skb) {
		printf("ipq_gmac_eth_send: alloc_skb return NULL  \n");
		goto dropped;
	}

	skb->len=len;
	memcpy(skb->data, (void *)packet, len);
	synopGMAC_linux_xmit_frames(skb, gmacdev->synopGMACnetdev);
	return NETDEV_TX_OK;

dropped:
printf("dropped\n");
	return 1;
}

int  ipq_gmac_eth_rx(struct eth_device *dev)
{
	synopGMACdevice *gmacdev = synopGMACDev[0];

	if (!gmacdev) {
		return 0;
	}

	synopGMAC_task_poll((unsigned long)gmacdev);

	return 1;
}

void  ipq_gmac_eth_halt(struct eth_device *dev)
{
	TR("eth_halt\n");

	/*
	 * We are not initialized yet, so nothing to stop
	 */
	if (!eth_init_done) {
		return;
	}

	synopGMAC_linux_close(synopGMACDev[0]->synopGMACnetdev);
	synopGMACDev[0]->state = ETH_STATE_PASSIVE;
}

int  ipq_gmac_eth_init(struct eth_device *dev,bd_t *bd)
{
	TR("eth_init\n");

	if (!eth_init_done) {
		TR("eth_init: init GMAC\n");
		synopGMAC_init_network_interface();
		athrs17_reg_init();
		synopGMACDev[0]->state = ETH_STATE_INIT;
		eth_init_done = 1;
	}

	synopGMAC_linux_open(synopGMACDev[0]->synopGMACnetdev, dev);
	synopGMACDev[0]->state = ETH_STATE_ACTIVE;

	return 0;
}

uint16_t mii_read_reg(synopGMACdevice *gmacdev,  uint32_t phy, uint32_t reg)
{
	u16 data = 0;

	synopGMAC_read_phy_reg((u32 *)gmacdev->MacBase , phy, reg, &data);
	return (uint16_t)data;
}

/*
 * mii_write_reg()
 *	Write a register of an external PHY/Switch
 */
void mii_write_reg(synopGMACdevice *gmacdev, uint32_t phy, uint32_t reg, uint16_t data)
{
	synopGMAC_write_phy_reg((u32 *)gmacdev->MacBase, phy, reg, (uint16_t)data);
}


#define MII_BMCR   	 	0x00
#define BMCR_RESET      	0x8000
#define BMCR_ANENABLE		0x1000

void synopGMAC_reset_phy(synopGMACdevice *gmacdev, u32 phyid)
{
	mii_write_reg(gmacdev, phyid , MII_BMCR, BMCR_RESET);
	mii_write_reg(gmacdev, phyid , MII_BMCR, mii_read_reg(gmacdev, phyid, MII_BMCR) | BMCR_ANENABLE);

	TR("Reset Phy %u successful\n",phyid);
}


int ipq_gmac_eth_initialize(const char *ethaddr)
{
	struct eth_device *dev;
	unsigned int m = 5, not_n = 0xF4, not_2d = 0xF5;
	int i;
	gpio_func_data_t *gpio_func = gboard_param->gmac_gpio;

	gmac_clock_config(m, not_n, not_2d);

	TR("\r\n ***** ipqg_mac_eth_initialize ***** \r\n");
	dev = malloc(sizeof(*dev));
	if (dev == NULL)
		return -1;
	memset(dev, 0, sizeof(*dev));
	dev->iobase = gboard_param->gmac_base;
	dev->init = ipq_gmac_eth_init;
	dev->halt = ipq_gmac_eth_halt;
	dev->send = ipq_gmac_eth_send;
	dev->recv = ipq_gmac_eth_rx;
	memcpy(dev->enetaddr, ethaddr, 6);
	strcpy(dev->name, "ipq_gmac");
	eth_register(dev);

	for (i = 0; i < gboard_param->gmac_gpio_count; i++) {
		gpio_tlmm_config(gpio_func->gpio, gpio_func->func, gpio_func->dir,
			gpio_func->pull, gpio_func->drvstr, gpio_func->enable);
		gpio_func++;
	}

	return 0;
}

#include "nss_gmac_clocks.h"


/**
 * @brief Return clock divider value for RGMII PHY.
 * @param[in] nss_gmac_dev *
 * @return returns RGMII clock divider value.
 */
static uint32_t clk_div_rgmii(synopGMACdevice *gmacdev)
{
        uint32_t div;

        switch (gmacdev->Speed) {
        case SPEED1000:
                div = RGMII_CLK_DIV_1000;
                break;

        case SPEED100:
                div = RGMII_CLK_DIV_100;
                break;

        case SPEED10:
                div = RGMII_CLK_DIV_10;
                break;

        default:
                div = RGMII_CLK_DIV_1000;
                break;
        }

        return div;
}

/**
 * @brief Return clock divider value for SGMII PHY.
 * @param[in] nss_gmac_dev *
 * @return returns SGMII clock divider value.
 */
static uint32_t clk_div_sgmii(synopGMACdevice *gmacdev)
{
        uint32_t div;

        switch (gmacdev->Speed) {
        case SPEED1000:
                div = SGMII_CLK_DIV_1000;
                break;

        case SPEED100:
                div = SGMII_CLK_DIV_100;
                break;

        case SPEED10:
                div = SGMII_CLK_DIV_10;
                break;

        default:
                div = SGMII_CLK_DIV_1000;
                break;
        }

        return div;
}


/*
 * @brief Return clock divider value for QSGMII PHY.
 * @param[in] nss_gmac_dev *
 * @return returns QSGMII clock divider value.
 */
static uint32_t clk_div_qsgmii(synopGMACdevice *gmacdev)
{
        uint32_t div;

        switch (gmacdev->Speed) {
        case SPEED1000:
                div = QSGMII_CLK_DIV_1000;
                break;

        case SPEED100:
                div = QSGMII_CLK_DIV_100;
                break;

        case SPEED10:
                div = QSGMII_CLK_DIV_10;
                break;

        default:
                div = QSGMII_CLK_DIV_1000;
                break;
        }

        return div;
}


void nss_gmac_dev_init(synopGMACdevice *gmacdev)
{
        uint32_t val = 0;
        uint32_t id = 0, div;

	val = GMAC_IFG_CTL(GMAC_IFG) | GMAC_IFG_LIMIT(GMAC_IFG) | GMAC_CSYS_REQ;
	if (gmacdev->phy_mii_type == GMAC_INTF_RGMII) {
		val |= GMAC_PHY_RGMII;
	} else {
		val &= ~GMAC_PHY_RGMII;
	}

	synopGMACSetBits((u32 *)nss_base, NSS_GMACn_CTL(id), val);

	synopGMACClearBits((u32 *)MSM_CLK_CTL_BASE, GMAC_COREn_RESET(id), 0x1);

	/* Configure clock dividers for 1000Mbps default */
	gmacdev->Speed = SPEED1000;
	switch (gmacdev->phy_mii_type) {
	case GMAC_INTF_RGMII:
		div = clk_div_rgmii(gmacdev);
		break;

	case GMAC_INTF_SGMII:
		div = clk_div_sgmii(gmacdev);
		break;

	case GMAC_INTF_QSGMII:
		div = clk_div_qsgmii(gmacdev);
		break;
	}

	val = synopGMACReadReg((u32 *)nss_base, NSS_ETH_CLK_DIV0);

	val &= ~GMACn_CLK_DIV(id, GMACn_CLK_DIV_SIZE);
	val |= GMACn_CLK_DIV(id, div);
	synopGMACWriteReg((u32 *)nss_base, NSS_ETH_CLK_DIV0, val);

	/* Select Tx/Rx CLK source */
	val = 0;
	if (id == 0 || id == 1) {
		if (gmacdev->phy_mii_type == GMAC_INTF_RGMII) {
			val |= (1 << id);
		}
	} else {
		if (gmacdev->phy_mii_type == GMAC_INTF_SGMII) {
			val |= (1 << id);
		}
	}

        synopGMACSetBits((u32 *)nss_base, NSS_ETH_CLK_SRC_CTL, val);

	synopGMACSetBits((u32 *)nss_base, NSS_QSGMII_CLK_CTL, 0x1);

	/* Enable xGMII clk for GMACn */
	val = 0;
	if (gmacdev->phy_mii_type == GMAC_INTF_RGMII) {
		val |= GMACn_RGMII_RX_CLK(id) | GMACn_RGMII_TX_CLK(id);
	} else {
		val |= GMACn_GMII_RX_CLK(id) | GMACn_GMII_TX_CLK(id);
	}

	/* Optionally configure RGMII CDC delay */

	/* Enable PTP clock */
	val |= GMACn_PTP_CLK(id);

	synopGMACSetBits((u32 *)nss_base, NSS_ETH_CLK_GATE_CTL, val);
}

/**
 * @brief QSGMII dev init
 *
 * @param[in] nss_gmac_dev *
 * @return void
 */
void nss_gmac_qsgmii_dev_init(synopGMACdevice *gmacdev)
{
        uint32_t val;
        uint32_t id = gmacdev->macid;

#ifdef CONFIG_MACH_IPQ806X_RUMI3
        nss_gmac_rumi_qsgmii_init(gmacdev);
#endif
        /* Enable clk for GMACn */
        val = 0;
        if ((gmacdev->phy_mii_type == GMAC_INTF_SGMII) || (gmacdev->phy_mii_type == GMAC_INTF_QSGMII)) {
                val |= GMACn_QSGMII_RX_CLK(id) | GMACn_QSGMII_TX_CLK(id);
        }

        synopGMACSetBits((uint32_t *)nss_base, NSS_QSGMII_CLK_CTL, val);

        val = synopGMACReadReg((uint32_t *)nss_base, NSS_QSGMII_CLK_CTL);
        TR("%s: NSS_QSGMII_CLK_CTL(0x%x) - 0x%x",
                      __FUNCTION__, NSS_QSGMII_CLK_CTL, val);
}



/**
 * @brief Clear all NSS GMAC interface registers.
 * @return returns 0 on success.
 */
static void nss_gmac_clear_all_regs(void)
{
        synopGMACClearBits((uint32_t *)nss_base,
                                NSS_ETH_CLK_GATE_CTL, 0xFFFFFFFF);
        synopGMACClearBits((uint32_t *)nss_base,
                                NSS_ETH_CLK_DIV0, 0xFFFFFFFF);
        synopGMACClearBits((uint32_t *)nss_base,
                                NSS_ETH_CLK_DIV1, 0xFFFFFFFF);
        synopGMACClearBits((uint32_t *)nss_base,
                                NSS_ETH_CLK_SRC_CTL, 0xFFFFFFFF);
        synopGMACClearBits((uint32_t *)nss_base,
                                NSS_ETH_CLK_INV_CTL, 0xFFFFFFFF);
        synopGMACClearBits((uint32_t *)nss_base,
                                NSS_GMAC0_CTL, 0xFFFFFFFF);
        synopGMACClearBits((uint32_t *)nss_base,
                                NSS_GMAC1_CTL, 0xFFFFFFFF);
        synopGMACClearBits((uint32_t *)nss_base,
                                NSS_GMAC2_CTL, 0xFFFFFFFF);
        synopGMACClearBits((uint32_t *)nss_base,
                                NSS_GMAC3_CTL, 0xFFFFFFFF);
        synopGMACClearBits((uint32_t *)nss_base,
                                NSS_QSGMII_CLK_CTL, 0xFFFFFFFF);
}


static void nss_gmac_qsgmii_common_init(uint32_t *qsgmii_base)
{
#if defined(CONFIG_IPQ_GMAC_PHY_PROFILE_QS)
        /* Configure QSGMII Block for QSGMII mode */

        /* Put PHY in QSGMII Mode */
        synopGMACWriteReg(qsgmii_base, QSGMII_PHY_MODE_CTL, QSGMII_PHY_MODE_QSGMII);

        /* Put PCS in QSGMII Mode */
        synopGMACWriteReg(qsgmii_base, PCS_QSGMII_SGMII_MODE, PCS_QSGMII_MODE_QSGMII);
#else
        /* Configure QSGMII Block for 3xSGMII mode */

        /* Put PHY in SGMII Mode */
        synopGMACWriteReg(qsgmii_base, QSGMII_PHY_MODE_CTL, QSGMII_PHY_MODE_SGMII);

        /* Put PCS in SGMII Mode */
        synopGMACWriteReg(qsgmii_base, PCS_QSGMII_SGMII_MODE, PCS_QSGMII_MODE_SGMII);
#endif
}


/**
 * @brief Set GMAC speed.
 * @param[in] nss_gmac_dev *
 * @return returns 0 on success.
 */
int32_t nss_gmac_dev_set_speed(synopGMACdevice *gmacdev)
{
	uint32_t val;
	uint32_t id = gmacdev->macid;
	uint32_t div;
	uint32_t clk;

	gmacdev->phy_mii_type = GMAC_INTF_RGMII;
	switch (gmacdev->phy_mii_type) {
	case GMAC_INTF_RGMII:
		div = clk_div_rgmii(gmacdev);
		break;

	case GMAC_INTF_SGMII:
		div = clk_div_sgmii(gmacdev);
		break;

	case GMAC_INTF_QSGMII:
		div = clk_div_qsgmii(gmacdev);
		break;

	default:
		return -EINVAL;
		TR0("%s: Invalid MII type", __FUNCTION__);
		break;
	}

	clk = 0;
	/* Disable GMACn Tx/Rx clk */
	if (gmacdev->phy_mii_type == GMAC_INTF_RGMII) {
		clk |= GMACn_RGMII_RX_CLK(id) | GMACn_RGMII_TX_CLK(id);
	} else {
		clk |= GMACn_GMII_RX_CLK(id) | GMACn_GMII_TX_CLK(id);
	}
	synopGMACClearBits((u32*)nss_base, NSS_ETH_CLK_GATE_CTL, clk);

	/* set clock divider */
	val = synopGMACReadReg((u32*)nss_base, NSS_ETH_CLK_DIV0);
	val &= ~GMACn_CLK_DIV(id, GMACn_CLK_DIV_SIZE);
	val |= GMACn_CLK_DIV(id, div);
	synopGMACWriteReg((u32*)nss_base, NSS_ETH_CLK_DIV0, val);

	/* Enable GMACn Tx/Rx clk */
	synopGMACSetBits((u32*)nss_base, NSS_ETH_CLK_GATE_CTL, clk);

	val = synopGMACReadReg((u32*)nss_base, NSS_ETH_CLK_DIV0);
	TR0("%s:NSS_ETH_CLK_DIV0(0x%x) - 0x%x",
		      __FUNCTION__, NSS_ETH_CLK_DIV0, val);

	return 0;
}


void ipq806x_dev_board_init(synopGMACdevice *gmacdev)
{
	nss_gmac_common_init();
	switch (gmacdev->macid) {
	case 0:
		gmacdev->phy_mii_type = NSS_GMAC0_MII_MODE;
		break;

	case 1:
		gmacdev->phy_mii_type = NSS_GMAC1_MII_MODE;
		break;

	case 2:
		gmacdev->phy_mii_type = NSS_GMAC2_MII_MODE;
		break;

	case 3:
		gmacdev->phy_mii_type = NSS_GMAC3_MII_MODE;
		break;

	default:
		TR0("%s: Invalid MII type", __FUNCTION__);
		return ;
		break;
	}

	nss_gmac_dev_init(gmacdev);
}

/*
 * @brief Initialization commom to all GMACs.
 * @return returns 0 on success.
 */
int32_t nss_gmac_common_init(void)
{
	volatile uint32_t val;

	nss_gmac_clear_all_regs();

	synopGMACWriteReg(qsgmii_base, NSS_QSGMII_PHY_SERDES_CTL,
				PLL_PUMP_CURRENT_600uA | PLL_TANK_CURRENT_7mA |
				PCIE_MAX_POWER_MODE | PLL_LOOP_FILTER_RESISTOR_DEFAULT |
				PLL_ENABLE | SERDES_ENABLE_LCKDT);

	synopGMACWriteReg(qsgmii_base, NSS_PCS_CAL_LCKDT_CTL, LCKDT_RST_n);

	synopGMACWriteReg(qsgmii_base, NSS_QSGMII_PHY_QSGMII_CTL,
				QSGMII_TX_AMPLITUDE_600mV | QSGMII_TX_SLC_10 |
				QSGMII_THRESHOLD_DEFAULT | QSGMII_SLEW_RATE_DEFAULT |
				QSGMII_RX_EQUALIZER_DEFAULT | QSGMII_RX_DC_BIAS_DEFAULT |
				QSGMII_PHASE_LOOP_GAIN_DEFAULT | QSGMII_TX_DE_EMPHASIS_DEFAULT |
				QSGMII_ENABLE | QSGMII_ENABLE_TX |
				QSGMII_ENABLE_SD | QSGMII_ENABLE_RX |
				QSGMII_ENABLE_CDR);

	synopGMACWriteReg(qsgmii_base, NSS_PCS_QSGMII_BERT_THLD_CTL, 0x0);

	/*
	 * Deaassert GMAC AHB reset
	 */
	synopGMACClearBits((uint32_t *)(MSM_CLK_CTL_BASE), GMAC_AHB_RESET, 0x1);

	/* Bypass MACSEC */
	synopGMACSetBits((uint32_t *)nss_base, NSS_MACSEC_CTL, 0x7);

	val = synopGMACReadReg((uint32_t *)nss_base, NSS_MACSEC_CTL);
	TR("%s:NSS_MACSEC_CTL(0x%x) - 0x%x",
		     __FUNCTION__, NSS_MACSEC_CTL, val);

	/*
	 * Initialize ACC_GMAC_CUST field of NSS_ACC_REG register
	 * for GMAC and MACSEC memories.
	 */
	synopGMACClearBits((uint32_t *)(MSM_CLK_CTL_BASE), NSS_ACC_REG, GMAC_ACC_CUST_MASK);

	return 0;
}


void nss_gmac_reset_phy( uint32_t phyid)
{
	ipq_mii_write_reg( phyid, MII_BMCR, BMCR_RESET);
	ipq_mii_write_reg( phyid, MII_BMCR,
			    ipq_mii_read_reg( phyid, MII_BMCR)
			    | BMCR_ANENABLE);
}

int32_t ipq806x_get_link_speed(uint32_t phyid)
{
	int32_t lpa = 0, media, adv;
	lpa = ipq_mii_read_reg(phyid, MII_LPA);
	TR(" get_link_speed LPA %x \r\n ", lpa);
	adv = ipq_mii_read_reg(phyid, MII_ADVERTISE);
	TR(" get_link_speed LPA %x ADV %x  \r\n ", lpa, adv);
	media = mii_nway_result(lpa & adv);
	if((media & ADVERTISE_1000XFULL))
		return SPEED1000;
	else if (media & (ADVERTISE_100FULL | ADVERTISE_100HALF))
		return SPEED100;
	else if (media & (ADVERTISE_10FULL | ADVERTISE_10HALF))
		return SPEED10;
	else return SPEED1000; /* fix to 1000 for bringup */
}

int32_t ipq806x_get_duplex(uint32_t phyid)
{
	int32_t lpa = 0, media, duplex, adv;
	lpa = ipq_mii_read_reg(phyid, MII_LPA);
	TR(" get_link_speed LPA %x \r\n ", lpa);
	adv = ipq_mii_read_reg(phyid, MII_ADVERTISE);
	TR(" get_link_speed LPA %x ADV %x  \r\n ", lpa, adv);
	media = mii_nway_result(lpa & adv);
	duplex = (media & (ADVERTISE_FULL | ADVERTISE_1000XFULL)) ? FULLDUPLEX : HALFDUPLEX;

	return duplex;
}

