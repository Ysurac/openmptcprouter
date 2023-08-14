
/* * Copyright (c) 2012 - 2013 The Linux Foundation. All rights reserved.* */

/* \file
 * This file defines the synopsys GMAC device dependent functions.
 * Most of the operations on the GMAC device are available in this file.
 * Functions for initiliasing and accessing MAC/DMA/PHY registers and the DMA descriptors
 * are encapsulated in this file. The functions are platform/host/OS independent.
 * These functions in turn use the low level device dependent (HAL) functions to
 * access the register space.
 * \internal
 * ------------------------REVISION HISTORY---------------------------------
 * Synopsys                 01/Aug/2007                              Created
 */
#include "synopGMAC_Dev.h"
#include <linux/mdio.h>
#include <asm/arch-ipq806x/nss/msm_ipq806x_gmac.h>

extern uint16_t mii_read_reg(synopGMACdevice *gmacdev,  uint32_t phy, uint32_t reg);
/*
 * Function to read the Phy register. The access to phy register
 * is a slow process as the data is moved accross MDI/MDO interface
 * @param[in] pointer to Register Base (It is the mac base in our case) .
 * @param[in] PhyBase register is the index of one of supported 32 PHY devices.
 * @param[in] Register offset is the index of one of the 32 phy register.
 * @param[out] u16 data read from the respective phy register (only valid iff return value is 0).
 * \return Returns 0 on success else return the error status.
 */
s32 synopGMAC_read_phy_reg(u32 *RegBase,u32 PhyBase, u32 RegOffset, u16 * data)
{
	u32 addr;
	u32 loop_variable;

	addr = ((PhyBase << GmiiDevShift) & GmiiDevMask) | ((RegOffset << GmiiRegShift) & GmiiRegMask);
	addr = addr | GmiiBusy ; //Gmii busy bit
	synopGMACWriteReg(RegBase,GmacGmiiAddr,addr); //write the address from where the data to be read in GmiiGmiiAddr register of synopGMAC ip

	//Wait till the busy bit gets cleared with in a certain amount of time
        for (loop_variable = 0; loop_variable < DEFAULT_LOOP_VARIABLE; loop_variable++) {
                if (!(synopGMACReadReg(RegBase,GmacGmiiAddr) & GmiiBusy)){
			break;
                }
		__udelay(0xFFFFF);
        }
        if(loop_variable < DEFAULT_LOOP_VARIABLE)
		*data = (u16)(synopGMACReadReg(RegBase, GmacGmiiData) & 0xFFFF);
        else{
		TR("Error::: PHY not responding Busy bit didnot get cleared !!!!!!\n");
		BUG();
		return -ESYNOPGMACPHYERR;
        }
	return -ESYNOPGMACNOERR;
}

/*
 * Function to write to the Phy register. The access to phy register
 * is a slow process as the data is moved accross MDI/MDO interface
 * @param[in] pointer to Register Base (It is the mac base in our case) .
 * @param[in] PhyBase register is the index of one of supported 32 PHY devices.
 * @param[in] Register offset is the index of one of the 32 phy register.
 * @param[in] data to be written to the respective phy register.
 * \return Returns 0 on success else return the error status.
 */
s32 synopGMAC_write_phy_reg(u32 *RegBase, u32 PhyBase, u32 RegOffset, u16 data)
{
	u32 addr;
	u32 loop_variable;

	synopGMACWriteReg(RegBase,GmacGmiiData,data); // write the data in to GmacGmiiData register of synopGMAC ip

	addr = ((PhyBase << GmiiDevShift) & GmiiDevMask) | ((RegOffset << GmiiRegShift) & GmiiRegMask) | GmiiWrite;

	addr = addr | GmiiBusy ; //set Gmii clk to 20-35 Mhz and Gmii busy bit

	synopGMACWriteReg(RegBase,GmacGmiiAddr,addr);
	for (loop_variable = 0; loop_variable < DEFAULT_LOOP_VARIABLE; loop_variable++) {
		if (!(synopGMACReadReg(RegBase,GmacGmiiAddr) & GmiiBusy)){
			break;
		}
		__udelay(DEFAULT_DELAY_VARIABLE);
	}

	if (loop_variable < DEFAULT_LOOP_VARIABLE) {
		return -ESYNOPGMACNOERR;
	} else {
		TR("Error::: PHY not responding Busy bit didnot get cleared !!!!!!\n");
		return -ESYNOPGMACPHYERR;
	}
}

/*
 * Function to read the GMAC IP Version and populates the same in device data structure.
 * @param[in] pointer to synopGMACdevice.
 * \return Always return 0.
 */

s32 synopGMAC_read_version (synopGMACdevice * gmacdev)
{
	u32 data = 0;
	data = synopGMACReadReg((u32 *)gmacdev->MacBase, GmacVersion );
	gmacdev->Version = data;
	TR("The synopGMAC version: %08x\n",data);
	return 0;
}

/*
 * Function to reset the GMAC core.
 * This reests the DMA and GMAC core. After reset all the registers holds their respective reset value
 * @param[in] pointer to synopGMACdevice.
 * \return 0 on success else return the error status.
 */
void synopGMAC_reset (synopGMACdevice * gmacdev )
{
	u32 data = 0;

	synopGMACWriteReg((u32 *)gmacdev->DmaBase, DmaBusMode ,DmaResetOn);
	do {
		__udelay(DEFAULT_LOOP_VARIABLE);
		data = synopGMACReadReg((u32 *)gmacdev->DmaBase, DmaBusMode);
	} while (data & DmaResetOn);

}

/*
 * Function to program DMA bus mode register.
 *
 * The Bus Mode register is programmed with the value given. The bits to be set are
 * bit wise or'ed and sent as the second argument to this function.
 * @param[in] pointer to synopGMACdevice.
 * @param[in] the data to be programmed.
 * \return 0 on success else return the error status.
 */
s32 synopGMAC_dma_bus_mode_init(synopGMACdevice * gmacdev, u32 init_value )
{
	synopGMACWriteReg((u32 *)gmacdev->DmaBase, DmaBusMode ,init_value);
	return 0;
}

/*
 * Function to program DMA Control register.
 *
 * The Dma Control register is programmed with the value given. The bits to be set are
 * bit wise or'ed and sent as the second argument to this function.
 * @param[in] pointer to synopGMACdevice.
 * @param[in] the data to be programmed.
 * \return 0 on success else return the error status.
 */
s32 synopGMAC_dma_control_init(synopGMACdevice * gmacdev, u32 init_value)
{
	synopGMACWriteReg((u32 *)gmacdev->DmaBase, DmaControl, init_value);
	return 0;
}


/*Gmac configuration functions*/

/*
 * Enable the watchdog timer on the receiver.
 * When enabled, Gmac enables Watchdog timer, and GMAC allows no more than
 * 2048 bytes of data (10,240 if Jumbo frame enabled).
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void synopGMAC_wd_enable(synopGMACdevice * gmacdev)
{
	synopGMACClearBits((u32 *)gmacdev->MacBase, GmacConfig, GmacWatchdog);
}

/*
 * Enables the Jabber frame support.
 * When enabled, GMAC disabled the jabber timer, and can transfer 16,384 byte frames.
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void synopGMAC_jab_enable(synopGMACdevice * gmacdev)
{
	synopGMACSetBits((u32 *)gmacdev->MacBase, GmacConfig, GmacJabber);
}

/*
 * Enables Frame bursting (Only in Half Duplex Mode).
 * When enabled, GMAC allows frame bursting in GMII Half Duplex mode.
 * Reserved in 10/100 and Full-Duplex configurations.
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void synopGMAC_frame_burst_enable(synopGMACdevice * gmacdev)
{
	synopGMACSetBits((u32 *)gmacdev->MacBase, GmacConfig, GmacFrameBurst);
}


/*
 * Disable Jumbo frame support.
 * When Disabled GMAC does not supports jumbo frames.
 * Giant frame error is reported in receive frame status.
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void synopGMAC_jumbo_frame_disable(synopGMACdevice * gmacdev)
{
	synopGMACClearBits((u32 *)gmacdev->MacBase, GmacConfig, GmacJumboFrame);
}

/*
 * Selects the GMII port.
 * When called GMII (1000Mbps) port is selected (programmable only in 10/100/1000 Mbps configuration).
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void synopGMAC_select_gmii(synopGMACdevice * gmacdev)
{
	synopGMACClearBits((u32 *)gmacdev->MacBase, GmacConfig, GmacSelectGmii);
}

/*
 * Selects the MII port.
 * When called MII (10/100Mbps) port is selected (programmable only in 10/100/1000 Mbps configuration).
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void synopGMAC_select_mii(synopGMACdevice * gmacdev)
{
	synopGMACSetBits((u32 *)gmacdev->MacBase, GmacConfig, GmacMiiGmii);
	if (gmacdev->Speed == SPEED100) {
		synopGMACSetBits((u32 *)gmacdev->MacBase, GmacConfig, GmacFESpeed100);
	} else {
		synopGMACClearBits((u32 *)gmacdev->MacBase, GmacConfig, GmacFESpeed100);
	}
}

/*
 * Enables Receive Own bit (Only in Half Duplex Mode).
 * When enaled GMAC receives all the packets given by phy while transmitting.
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void synopGMAC_rx_own_enable(synopGMACdevice * gmacdev)
{
	synopGMACClearBits((u32 *)gmacdev->MacBase, GmacConfig, GmacRxOwn);
}

/*
 * Disables Receive Own bit (Only in Half Duplex Mode).
 * When enaled GMAC disables the reception of frames when gmii_txen_o is asserted.
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void synopGMAC_rx_own_disable(synopGMACdevice * gmacdev)
{
	synopGMACSetBits((u32 *)gmacdev->MacBase, GmacConfig, GmacRxOwn);
}

/*
 * Sets the GMAC in Normal mode.
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void synopGMAC_loopback_off(synopGMACdevice * gmacdev)
{
	synopGMACClearBits((u32 *)gmacdev->MacBase, GmacConfig, GmacLoopback);
}

/*
 * Sets the GMAC core in Full-Duplex mode.
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void synopGMAC_set_full_duplex(synopGMACdevice * gmacdev)
{
	synopGMACSetBits((u32 *)gmacdev->MacBase, GmacConfig, GmacDuplex);
}

/*
 * Sets the GMAC core in Half-Duplex mode.
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void synopGMAC_set_half_duplex(synopGMACdevice * gmacdev)
{
	synopGMACClearBits((u32 *)gmacdev->MacBase, GmacConfig, GmacDuplex);
}

/*
 * GMAC tries retransmission (Only in Half Duplex mode).
 * If collision occurs on the GMII/MII, GMAC attempt retries based on the
 * back off limit configured.
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 * \note This function is tightly coupled with synopGMAC_back_off_limit(synopGMACdevice *, u32).
 */
void synopGMAC_retry_enable(synopGMACdevice * gmacdev)
{
	synopGMACClearBits((u32 *)gmacdev->MacBase, GmacConfig, GmacRetry);
}

/*
 * GMAC tries only one transmission (Only in Half Duplex mode).
 * If collision occurs on the GMII/MII, GMAC will ignore the current frami
 * transmission and report a frame abort with excessive collision in tranmit frame status.
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void synopGMAC_retry_disable(synopGMACdevice * gmacdev)
{
	synopGMACSetBits((u32 *)gmacdev->MacBase, GmacConfig, GmacRetry);
}

/*
 * GMAC doesnot strips the Pad/FCS field of incoming frames.
 * GMAC will pass all the incoming frames to Host unmodified.
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void synopGMAC_pad_crc_strip_disable(synopGMACdevice * gmacdev)
{
	synopGMACClearBits((u32 *)gmacdev->MacBase, GmacConfig, GmacPadCrcStrip);
}

/*
 * GMAC programmed with the back off limit value.
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 * \note This function is tightly coupled with synopGMAC_retry_enable(synopGMACdevice * gmacdev)
 */
void synopGMAC_back_off_limit(synopGMACdevice * gmacdev, u32 value)
{
	u32 data;
	data = synopGMACReadReg((u32 *)gmacdev->MacBase, GmacConfig);
	data &= (~GmacBackoffLimit);
	data |= value;
	synopGMACWriteReg((u32 *)gmacdev->MacBase, GmacConfig,data);
}

/*
 * Disables the Deferral check in GMAC (Only in Half Duplex mode).
 * GMAC defers until the CRS signal goes inactive.
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void synopGMAC_deferral_check_disable(synopGMACdevice * gmacdev)
{
	synopGMACClearBits((u32 *)gmacdev->MacBase, GmacConfig, GmacDeferralCheck);
}

/*
 * Enable the reception of frames on GMII/MII.
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void synopGMAC_rx_enable(synopGMACdevice * gmacdev)
{
	synopGMACSetBits((u32 *)gmacdev->MacBase, GmacConfig, GmacRx);
}

/*
 * Disable the reception of frames on GMII/MII.
 * GMAC receive state machine is disabled after completion of reception of current frame.
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void synopGMAC_rx_disable(synopGMACdevice * gmacdev)
{
	synopGMACClearBits((u32 *)gmacdev->MacBase, GmacConfig, GmacRx);

}

/*
 * Enable the transmission of frames on GMII/MII.
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void synopGMAC_tx_enable(synopGMACdevice * gmacdev)
{
	synopGMACSetBits((u32 *)gmacdev->MacBase, GmacConfig, GmacTx);
}

/*
 * Disable the transmission of frames on GMII/MII.
 * GMAC transmit state machine is disabled after completion of transmission of current frame.
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void synopGMAC_tx_disable(synopGMACdevice * gmacdev)
{
	synopGMACClearBits((u32 *)gmacdev->MacBase, GmacConfig, GmacTx);
}

/*Receive frame filter configuration functions*/

/*
 * Enables reception of all the frames to application.
 * GMAC passes all the frames received to application irrespective of whether they
 * pass SA/DA address filtering or not.
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void synopGMAC_frame_filter_enable(synopGMACdevice * gmacdev)
{
	synopGMACClearBits((u32 *)gmacdev->MacBase, GmacFrameFilter, GmacFilter);
}

/*
 * Disables Source address filtering.
 * When disabled GMAC forwards the received frames with updated SAMatch bit in RxStatus.
 * @param[in] pointer to synopGMACdevice.
 * \return void.
 */
void synopGMAC_src_addr_filter_disable(synopGMACdevice * gmacdev)
{
	synopGMACClearBits((u32 *)gmacdev->MacBase, GmacFrameFilter, GmacSrcAddrFilter);
}
/*
 * Enables the normal Destination address filtering.
 * @param[in] pointer to synopGMACdevice.
 * \return void.
 */
void synopGMAC_dst_addr_filter_normal(synopGMACdevice * gmacdev)
{
	synopGMACClearBits((u32 *)gmacdev->MacBase, GmacFrameFilter, GmacDestAddrFilterNor);
}

/*
 * Enables forwarding of control frames.
 * When set forwards all the control frames (incl. unicast and multicast PAUSE frames).
 * @param[in] pointer to synopGMACdevice.
 * \return void.
 * \note Depends on RFE of FlowControlRegister[2]
 */
void synopGMAC_set_pass_control(synopGMACdevice * gmacdev,u32 passcontrol)
{
	u32 data;
	data = synopGMACReadReg((u32 *)gmacdev->MacBase, GmacFrameFilter);
	data &= (~GmacPassControl);
	data |= passcontrol;
	synopGMACWriteReg((u32 *)gmacdev->MacBase,GmacFrameFilter,data);
}

/*
 * Enables Broadcast frames.
 * When enabled Address filtering module passes all incoming broadcast frames.
 * @param[in] pointer to synopGMACdevice.
 * \return void.
 */
void synopGMAC_broadcast_enable(synopGMACdevice * gmacdev)
{
	synopGMACClearBits((u32 *)gmacdev->MacBase, GmacFrameFilter, GmacBroadcast);
}

/*
 * Disable Multicast frames.
 * When disabled multicast frame filtering depends on HMC bit.
 * @param[in] pointer to synopGMACdevice.
 * \return void.
 */
void synopGMAC_multicast_disable(synopGMACdevice * gmacdev)
{
	synopGMACClearBits((u32 *)gmacdev->MacBase, GmacFrameFilter, GmacMulticastFilter);
}

/*
 * Disables multicast hash filtering.
 * When disabled GMAC performs perfect destination address filtering for multicast frames, it compares
 * DA field with the value programmed in DA register.
 * @param[in] pointer to synopGMACdevice.
 * \return void.
 */
void synopGMAC_multicast_hash_filter_disable(synopGMACdevice * gmacdev)
{
	synopGMACClearBits((u32 *)gmacdev->MacBase, GmacFrameFilter, GmacMcastHashFilter);
}

/*
 * Enables promiscous mode.
 * When enabled Address filter modules pass all incoming frames regardless of their Destination
 * and source addresses.
 * @param[in] pointer to synopGMACdevice.
 * \return void.
 */
void synopGMAC_promisc_enable(synopGMACdevice * gmacdev)
{
	synopGMACSetBits((u32 *)gmacdev->MacBase, GmacFrameFilter, GmacPromiscuousMode);
}

#if 0
/*
 * Enables unicast hash filtering.
 * When enabled GMAC performs the destination address filtering of unicast frames according to the hash table.
 * @param[in] pointer to synopGMACdevice.
 * \return void.
 */
void synopGMAC_unicast_hash_filter_enable(synopGMACdevice * gmacdev)
{
	synopGMACSetBits((u32 *)gmacdev->MacBase, GmacFrameFilter, GmacUcastHashFilter);
}
#endif

/*
 * Disables multicast hash filtering.
 * When disabled GMAC performs perfect destination address filtering for unicast frames, it compares
 * DA field with the value programmed in DA register.
 * @param[in] pointer to synopGMACdevice.
 * \return void.
 */
void synopGMAC_unicast_hash_filter_disable(synopGMACdevice * gmacdev)
{
	synopGMACClearBits((u32 *)gmacdev->MacBase, GmacFrameFilter, GmacUcastHashFilter);
}

#if 0

/*Flow control configuration functions*/

/*
 * Enables detection of pause frames with stations unicast address.
 * When enabled GMAC detects the pause frames with stations unicast address in addition to the
 * detection of pause frames with unique multicast address.
 * @param[in] pointer to synopGMACdevice.
 * \return void.
 */
void synopGMAC_unicast_pause_frame_detect_enable(synopGMACdevice * gmacdev)
{
	synopGMACSetBits((u32 *)gmacdev->MacBase, GmacFlowControl, GmacUnicastPauseFrame);
}

#endif

/*
 * Disables detection of pause frames with stations unicast address.
 * When disabled GMAC only detects with the unique multicast address (802.3x).
 * @param[in] pointer to synopGMACdevice.
 * \return void.
 */
void synopGMAC_unicast_pause_frame_detect_disable(synopGMACdevice * gmacdev)
{
	synopGMACClearBits((u32 *)gmacdev->MacBase, GmacFlowControl, GmacUnicastPauseFrame);
}

/*
 * Rx flow control enable.
 * When Enabled GMAC will decode the rx pause frame and disable the tx for a specified time.
 * @param[in] pointer to synopGMACdevice.
 * \return void.
 */
void synopGMAC_rx_flow_control_enable(synopGMACdevice * gmacdev)
{
	synopGMACSetBits((u32 *)gmacdev->MacBase, GmacFlowControl, GmacRxFlowControl);
}


/*
 * Rx flow control disable.
 * When disabled GMAC will not decode pause frame.
 * @param[in] pointer to synopGMACdevice.
 * \return void.
 */
void synopGMAC_rx_flow_control_disable(synopGMACdevice * gmacdev)
{
	synopGMACClearBits((u32 *)gmacdev->MacBase, GmacFlowControl, GmacRxFlowControl);
}

/*
 * Tx flow control enable.
 * When Enabled
 * 	- In full duplex GMAC enables flow control operation to transmit pause frames.
 *	- In Half duplex GMAC enables the back pressure operation
 * @param[in] pointer to synopGMACdevice.
 * \return void.
 */
void synopGMAC_tx_flow_control_enable(synopGMACdevice * gmacdev)
{
	synopGMACSetBits((u32 *)gmacdev->MacBase, GmacFlowControl, GmacTxFlowControl);
}

/*
 * Tx flow control disable.
 * When Disabled
 * 	- In full duplex GMAC will not transmit any pause frames.
 *	- In Half duplex GMAC disables the back pressure feature.
 * @param[in] pointer to synopGMACdevice.
 * \return void.
 */
void synopGMAC_tx_flow_control_disable(synopGMACdevice * gmacdev)
{
	synopGMACClearBits((u32 *)gmacdev->MacBase, GmacFlowControl, GmacTxFlowControl);
}

/*
 * Initiate Flowcontrol operation.
 * When Set
 * 	- In full duplex GMAC initiates pause control frame.
 *	- In Half duplex GMAC initiates back pressure function.
 * @param[in] pointer to synopGMACdevice.
 * \return void.
 */
void synopGMAC_tx_activate_flow_control(synopGMACdevice * gmacdev)
{
	//In case of full duplex check for this bit to b'0. if it is read as b'1 indicates that
        //control frame transmission is in progress.
	if(gmacdev->Speed == FULLDUPLEX){
		if(!synopGMACCheckBits((u32 *)gmacdev->MacBase, GmacFlowControl, GmacFlowControlBackPressure))
			synopGMACSetBits((u32 *)gmacdev->MacBase, GmacFlowControl, GmacFlowControlBackPressure);
	} else{ //if half duplex mode
		synopGMACSetBits((u32 *)gmacdev->MacBase, GmacFlowControl, GmacFlowControlBackPressure);
	}
}

/*
 * stops Flowcontrol operation.
 * @param[in] pointer to synopGMACdevice.
 * \return void.
 */
void synopGMAC_tx_deactivate_flow_control(synopGMACdevice * gmacdev)
{
	//In full duplex this bit is automatically cleared after transmitting a pause control frame.
	if(gmacdev->Speed == HALFDUPLEX){
		synopGMACSetBits((u32 *)gmacdev->MacBase, GmacFlowControl, GmacFlowControlBackPressure);
	}
}

/*
 * This enables the pause frame generation after programming the appropriate registers.
 * Teh Ubicom design has only 4K FIFO. The activation is set at (full -) 2k and deactivation
 * is set at (full -) 3k. These may have to tweaked if found any issues
 * @param[in] pointer to synopGMACdevice.
 * \return void.
 */
void synopGMAC_pause_control(synopGMACdevice *gmacdev)
{
	u32 omr_reg;
	u32 mac_flow_control_reg;
	omr_reg = synopGMACReadReg((u32 *)gmacdev->DmaBase, DmaControl);
	omr_reg |= DmaRxFlowCtrlAct3K | DmaRxFlowCtrlDeact4K |DmaEnHwFlowCtrl;
	synopGMACWriteReg((u32 *)gmacdev->DmaBase, DmaControl, omr_reg);

	mac_flow_control_reg = synopGMACReadReg((u32 *)gmacdev->MacBase, GmacFlowControl);
	mac_flow_control_reg |= GmacRxFlowControl | GmacTxFlowControl | 0xFFFF0000;
	synopGMACWriteReg((u32 *)gmacdev->MacBase, GmacFlowControl, mac_flow_control_reg);
}

/*
 * Example mac initialization sequence.
 * This function calls the initialization routines to initialize the GMAC register.
 * One can change the functions invoked here to have different configuration as per the requirement
 * @param[in] pointer to synopGMACdevice.
 * \return Returns 0 on success.
 */
s32 synopGMAC_mac_init(synopGMACdevice * gmacdev)
{
	u32 PHYreg;

	synopGMAC_wd_enable(gmacdev);
	synopGMAC_jab_enable(gmacdev);
	synopGMAC_frame_burst_enable(gmacdev);
	synopGMAC_jumbo_frame_disable(gmacdev);
	synopGMAC_loopback_off(gmacdev);
	gmacdev->Speed = ipq806x_get_link_speed(gmacdev->phyid);
	gmacdev->DuplexMode = ipq806x_get_duplex(gmacdev->phyid);

	if (gmacdev->Speed == SPEED1000) {
		synopGMAC_select_gmii(gmacdev);
	} else {
		synopGMAC_select_mii(gmacdev);
	}

	if (gmacdev->DuplexMode == FULLDUPLEX) {
		synopGMAC_set_full_duplex(gmacdev);
		synopGMAC_rx_own_enable(gmacdev);
		synopGMAC_retry_disable(gmacdev);
	} else {
		synopGMAC_set_half_duplex(gmacdev);
		synopGMAC_rx_own_disable(gmacdev);
		synopGMAC_retry_enable(gmacdev);
	}

	synopGMAC_pad_crc_strip_disable(gmacdev);
	synopGMAC_back_off_limit(gmacdev, GmacBackoffLimit0);
	synopGMAC_deferral_check_disable(gmacdev);
	synopGMAC_tx_enable(gmacdev);
	synopGMAC_rx_enable(gmacdev);

	/*Frame Filter Configuration*/
	synopGMAC_frame_filter_enable(gmacdev);
	synopGMAC_set_pass_control(gmacdev, GmacPassControl0);
	synopGMAC_broadcast_enable(gmacdev);
	synopGMAC_src_addr_filter_disable(gmacdev);
	synopGMAC_multicast_disable(gmacdev);
	synopGMAC_dst_addr_filter_normal(gmacdev);
	synopGMAC_multicast_hash_filter_disable(gmacdev);
	synopGMAC_promisc_enable(gmacdev);
	synopGMAC_unicast_hash_filter_disable(gmacdev);

	/*Flow Control Configuration*/
	synopGMAC_unicast_pause_frame_detect_disable(gmacdev);
	if (gmacdev->DuplexMode == FULLDUPLEX) {
		synopGMAC_rx_flow_control_disable(gmacdev);
		synopGMAC_tx_flow_control_disable(gmacdev);
	} else {
		synopGMAC_rx_flow_control_disable(gmacdev);
		synopGMAC_tx_flow_control_disable(gmacdev);

		/*To set PHY register to enable CRS on Transmit*/
		synopGMACWriteReg((u32 *)gmacdev->MacBase, GmacGmiiAddr, GmiiBusy | 0x00000408);
		PHYreg = synopGMACReadReg((u32 *)gmacdev->MacBase, GmacGmiiData);
		synopGMACWriteReg((u32 *)gmacdev->MacBase, GmacGmiiData, PHYreg   | 0x00000800);
		synopGMACWriteReg((u32 *)gmacdev->MacBase, GmacGmiiAddr, GmiiBusy | 0x0000040a);
	}

	synopGMACWriteReg((u32 *)gmacdev->MacBase, GmacConfig, \
			GmacFrameBurstEnable | GmacJumboFrameEnable | GmacSelectMii \
			| GmacFullDuplex | GmacTxEnable | GmacRxEnable);
	synopGMACWriteReg((u32 *)gmacdev->MacBase, GmacFrameFilter, \
			GmacFilterOff | GmacPromiscuousModeOn);

	return 0;
}

/*
 * Checks and initialze phy.
 * This function checks whether the phy initialization is complete.
 * @param[in] pointer to synopGMACdevice.
 * \return 0 if success else returns the error number.
 */
s32 synopGMAC_check_phy_init (synopGMACdevice * gmacdev)
{
	u16 data;
	s32 status = -ESYNOPGMACNOERR;
	s32 loop_count;

	loop_count = DEFAULT_LOOP_VARIABLE;
	while (loop_count-- > 0)
	{
		status = synopGMAC_read_phy_reg((u32 *)gmacdev->MacBase, gmacdev->PhyBase,
				PHY_STATUS_REG, &data);
		if(status)
			return status;

	        if ((data & Mii_AutoNegCmplt) != 0) {
			TR("Autonegotiation Complete\n");
			break;
		}
	}

	status = synopGMAC_read_phy_reg((u32 *)gmacdev->MacBase, gmacdev->PhyBase,
			PHY_SPECIFIC_STATUS_REG, &data);
	if(status)
		return status;

        if((data & Mii_phy_status_link_up) == 0){
		TR("No Link\n");
		gmacdev->LinkState = LINKDOWN;
		return -ESYNOPGMACPHYERR;
	}
	else{
		gmacdev->LinkState = LINKUP;
		TR("Link UP\n");
	}

	status = synopGMAC_read_phy_reg((u32 *)gmacdev->MacBase, gmacdev->PhyBase,
			PHY_SPECIFIC_STATUS_REG, &data);
	if(status)
		return status;

	gmacdev->DuplexMode = (data & Mii_phy_status_full_duplex)  ? FULLDUPLEX: HALFDUPLEX ;
	TR("Link is up in %s mode\n",(gmacdev->DuplexMode == FULLDUPLEX) ? "FULL DUPLEX": "HALF DUPLEX");

	/*if not set to Master configuration in case of Half duplex mode set it manually as Master*/
	if(gmacdev->DuplexMode == HALFDUPLEX){
		status = synopGMAC_read_phy_reg((u32 *)gmacdev->MacBase, gmacdev->PhyBase,
				PHY_CONTROL_REG, &data);
		if(status)
			return status;

		status = synopGMAC_write_phy_reg((u32 *)gmacdev->MacBase, gmacdev->PhyBase,
				PHY_CONTROL_REG, data | Mii_Manual_Master_Config );
		if(status)
			return status;
	}

	status = synopGMAC_read_phy_reg((u32 *)gmacdev->MacBase,gmacdev->PhyBase,PHY_SPECIFIC_STATUS_REG, &data);
	if(status)
		return status;
	if (data & Mii_phy_status_speed_1000)
		gmacdev->Speed = SPEED1000;
	else if(data & Mii_phy_status_speed_100)
		gmacdev->Speed = SPEED100;
	else
		gmacdev->Speed = SPEED10;

	if (gmacdev->Speed == SPEED1000)
		TR("Link is with 1000M Speed \n");
	if (gmacdev->Speed == SPEED100)
		TR("Link is with 100M Speed \n");
	if (gmacdev->Speed == SPEED10)
		TR("Link is with 10M Speed \n");

	return -ESYNOPGMACNOERR;
}

s32 synopGMAC_check_link(synopGMACdevice *gmacdev)
{
	uint16_t lnkstat ;

	lnkstat = mii_read_reg(gmacdev, gmacdev->PhyBase, MII_BMSR);

	if (lnkstat & 0x4)
	{
		printf("\r\n LINKUP LINKstat %x  ***********\r\n ", lnkstat);
		return LINKUP;
	}
	else
	{
		printf("\r\n**LINKDOWN** LINKstat %x  *********\r\n ", lnkstat);
		return LINKDOWN;
	}
}

/*
 * Sets the Mac address in to GMAC register.
 * This function sets the MAC address to the MAC register in question.
 * @param[in] pointer to synopGMACdevice to populate mac dma and phy addresses.
 * @param[in] Register offset for Mac address high
 * @param[in] Register offset for Mac address low
 * @param[in] buffer containing mac address to be programmed.
 * \return 0 upon success. Error code upon failure.
 */
s32 synopGMAC_set_mac_addr(synopGMACdevice *gmacdev, u32 MacHigh, u32 MacLow, u8 *MacAddr)
{
	u32 data;

	data = (MacAddr[5] << 8) | MacAddr[4];
	synopGMACWriteReg((u32 *)gmacdev->MacBase,MacHigh,data);
	data = (MacAddr[3] << 24) | (MacAddr[2] << 16) | (MacAddr[1] << 8) | MacAddr[0] ;
	synopGMACWriteReg((u32 *)gmacdev->MacBase, MacLow, data);
	return 0;
}

/*
 * Get the Mac address in to the address specified.
 * The mac register contents are read and written to buffer passed.
 * @param[in] pointer to synopGMACdevice to populate mac dma and phy addresses.
 * @param[in] Register offset for Mac address high
 * @param[in] Register offset for Mac address low
 * @param[out] buffer containing the device mac address.
 * \return 0 upon success. Error code upon failure.
 */
s32 synopGMAC_get_mac_addr(synopGMACdevice *gmacdev, u32 MacHigh, u32 MacLow, u8 *MacAddr)
{
	u32 data;

	data = synopGMACReadReg((u32 *)gmacdev->MacBase, MacHigh);
	MacAddr[5] = (data >> 8) & 0xff;
	MacAddr[4] = (data) & 0xff;

	data = synopGMACReadReg((u32 *)gmacdev->MacBase, MacLow);
	MacAddr[3] = (data >> 24) & 0xff;
	MacAddr[2] = (data >> 16) & 0xff;
	MacAddr[1] = (data >> 8 ) & 0xff;
	MacAddr[0] = (data )      & 0xff;

	return 0;
}

/*
 * Attaches the synopGMAC device structure to the hardware.
 * Device structure is populated with MAC/DMA and PHY base addresses.
 * @param[in] pointer to synopGMACdevice to populate mac dma and phy addresses.
 * @param[in] GMAC IP mac base address.
 * @param[in] GMAC IP dma base address.
 * @param[in] GMAC IP phy base address.
 * \return 0 upon success. Error code upon failure.
 * \note This is important function. No kernel api provided by Synopsys
 */

s32 synopGMAC_attach (synopGMACdevice * gmacdev, u32 regBase)
{
	/*Make sure the Device data strucure is cleared before we proceed further*/
	memset((void *)gmacdev, 0, sizeof(synopGMACdevice));
	/*Populate the mac and dma base addresses*/
	gmacdev->NbrBase = regBase;

	gmacdev->MacBase = regBase + MACBASE;
        gmacdev->DmaBase = regBase + DMABASE;

	if (gboard_param->machid == MACH_TYPE_IPQ806X_RUMI3)
		gmacdev->PhyBase = GMAC0_MDIO_ID;

	return 0;
}


s32 synopGMAC_init_tx_rx_desc_queue(synopGMACdevice *gmacdev)
{
	s32 i;
	for (i =0; i < gmacdev -> TxDescCount; i++) {
		synopGMAC_tx_desc_init_ring(gmacdev->TxDesc + i, i == gmacdev->TxDescCount-1);
	}

	for (i =0; i < gmacdev -> RxDescCount; i++) {
		synopGMAC_rx_desc_init_ring(gmacdev->RxDesc + i, i == gmacdev->RxDescCount-1);
	}

	gmacdev->TxNext = 0;
	gmacdev->TxBusy = 0;
	gmacdev->RxNext = 0;
	gmacdev->RxBusy = 0;

	return -ESYNOPGMACNOERR;
}

/*
 * Programs the DmaRxBaseAddress with the Rx descriptor base address.
 * Rx Descriptor's base address is available in the gmacdev structure. This function progrms the
 * Dma Rx Base address with the starting address of the descriptor ring or chain.
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void synopGMAC_init_rx_desc_base(synopGMACdevice *gmacdev)
{
	synopGMACWriteReg((u32 *)gmacdev->DmaBase, DmaRxBaseAddr, (u32)gmacdev->RxDescDma);
}

/*
 * Programs the DmaTxBaseAddress with the Tx descriptor base address.
 * Tx Descriptor's base address is available in the gmacdev structure. This function progrms the
 * Dma Tx Base address with the starting address of the descriptor ring or chain.
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void synopGMAC_init_tx_desc_base(synopGMACdevice *gmacdev)
{
	synopGMACWriteReg((u32 *)gmacdev->DmaBase, DmaTxBaseAddr, (u32)gmacdev->TxDescDma);
}

/*
 * Makes the Dma as owner for this descriptor.
 * This function sets the own bit of status field of the DMA descriptor,
 * indicating the DMA is the owner for this descriptor.
 * @param[in] pointer to DmaDesc structure.
 * \return returns void.
 */
void synopGMAC_set_owner_dma(DmaDesc *desc)
{
	desc->status |= DescOwnByDma;
}

/*
 * set tx descriptor to indicate SOF.
 * This Descriptor contains the start of ethernet frame.
 * @param[in] pointer to DmaDesc structure.
 * \return returns void.
 */
void synopGMAC_set_desc_sof(DmaDesc *desc)
{
	#ifdef ENH_DESC
	desc->status |= DescTxFirst;//ENH_DESC
	#else
	desc->length |= DescTxFirst;
	#endif
}

/*
 * set tx descriptor to indicate EOF.
 * This descriptor contains the End of ethernet frame.
 * @param[in] pointer to DmaDesc structure.
 * \return returns void.
 */
void synopGMAC_set_desc_eof(DmaDesc *desc)
{
	#ifdef ENH_DESC
	desc->status |= DescTxLast;//ENH_DESC
	#else
	desc->length |= DescTxLast;
	#endif
}

/*
 * checks whether this descriptor contains start of frame.
 * This function is to check whether the descriptor's data buffer
 * contains a fresh ethernet frame?
 * @param[in] pointer to DmaDesc structure.
 * \return returns true if SOF in current descriptor, else returns fail.
 */
bool synopGMAC_is_sof_in_rx_desc(DmaDesc *desc)
{
	return ((desc->status & DescRxFirst) == DescRxFirst);
}

/*
 * checks whether this descriptor contains end of frame.
 * This function is to check whether the descriptor's data buffer
 * contains end of ethernet frame?
 * @param[in] pointer to DmaDesc structure.
 * \return returns true if SOF in current descriptor, else returns fail.
 */
bool synopGMAC_is_eof_in_rx_desc(DmaDesc *desc)
{
	return ((desc->status & DescRxLast) == DescRxLast);
}

/*
 * checks whether destination address filter failed in the rx frame.
 * @param[in] pointer to DmaDesc structure.
 * \return returns true if Failed, false if not.
 */
bool synopGMAC_is_da_filter_failed(DmaDesc *desc)
{
	return ((desc->status & DescDAFilterFail) == DescDAFilterFail);
}

/*
 * checks whether source address filter failed in the rx frame.
 * @param[in] pointer to DmaDesc structure.
 * \return returns true if Failed, false if not.
 */
bool synopGMAC_is_sa_filter_failed(DmaDesc *desc)
{
	return ((desc->status & DescSAFilterFail) == DescSAFilterFail);
}

/*
 * Checks whether the tx is aborted due to collisions.
 * @param[in] pointer to DmaDesc structure.
 * \return returns true if collisions, else returns false.
 */
bool synopGMAC_is_tx_aborted(u32 status)
{
	return (((status & DescTxLateCollision) == DescTxLateCollision) | ((status & DescTxExcCollisions) == DescTxExcCollisions));

}

/*
 * Checks whether the tx carrier error.
 * @param[in] pointer to DmaDesc structure.
 * \return returns true if carrier error occured, else returns falser.
 */
bool synopGMAC_is_tx_carrier_error(u32 status)
{
	return (((status & DescTxLostCarrier) == DescTxLostCarrier)  | ((status & DescTxNoCarrier) == DescTxNoCarrier));
}

/*
 * Check for damaged frame due to overflow or collision.
 * Retruns true if rx frame was damaged due to buffer overflow in MTL or late collision in half duplex mode.
 * @param[in] pointer to DmaDesc structure.
 * \return returns true if error else returns false.
 */
bool synopGMAC_is_rx_frame_damaged(u32 status)
{
//bool synopGMAC_dma_rx_collisions(u32 status)
	return (((status & DescRxDamaged) == DescRxDamaged) | ((status & DescRxCollision) == DescRxCollision));
}

/*
 * Check for damaged frame due to collision.
 * Retruns true if rx frame was damaged due to late collision in half duplex mode.
 * @param[in] pointer to DmaDesc structure.
 * \return returns true if error else returns false.
 */
bool synopGMAC_is_rx_frame_collision(u32 status)
{
//bool synopGMAC_dma_rx_collisions(u32 status)
	return ((status & DescRxCollision) == DescRxCollision);
}

/*
 * Check for receive CRC error.
 * Retruns true if rx frame CRC error occured.
 * @param[in] pointer to DmaDesc structure.
 * \return returns true if error else returns false.
 */
bool synopGMAC_is_rx_crc(u32 status)
{
//u32 synopGMAC_dma_rx_crc(u32 status)
	return ((status & DescRxCrc) == DescRxCrc);
}

/*
 * Indicates rx frame has non integer multiple of bytes. (odd nibbles).
 * Retruns true if dribbling error in rx frame.
 * @param[in] pointer to DmaDesc structure.
 * \return returns true if error else returns false.
 */
bool synopGMAC_is_frame_dribbling_errors(u32 status)
{
	return ((status & DescRxDribbling) == DescRxDribbling);
}

/*
 * Indicates error in rx frame length.
 * Retruns true if received frame length doesnot match with the length field
 * @param[in] pointer to DmaDesc structure.
 * \return returns true if error else returns false.
 */
bool synopGMAC_is_rx_frame_length_errors(u32 status)
{
	return((status & DescRxLengthError) == DescRxLengthError);
}

/*
 * Driver Api to get the descriptor field information.
 * This returns the status, dma-able address of buffer1, the length of buffer1, virtual address of buffer1
 * dma-able address of buffer2, length of buffer2, virtural adddress of buffer2.
 * @param[in]  pointer to DmaDesc structure.
 * @param[out] pointer to status field fo descriptor.
 * @param[out] dma-able address of buffer1.
 * @param[out] length of buffer1.
 * @param[out] virtual address of buffer1.
 * @param[out] dma-able address of buffer2.
 * @param[out] length of buffer2.
 * @param[out] virtual address of buffer2.
 * \return returns void.
 */
void synopGMAC_get_desc_data(DmaDesc * desc, u32 * Status, u32 * Buffer1, u32 * Length1, u32 * Data1)
{

	/*
	 * The first time, we map the descriptor as DMA_TO_DEVICE.
	 * Then we only wait for changes from device, so we use DMA_FROM_DEVICE.
	 */

	if (Status != 0)
		*Status = desc->status;

	if (Buffer1 != 0)
		*Buffer1 = desc->buffer1;
	if (Length1 != 0)
		*Length1 = (desc->length & DescSize1Mask) >> DescSize1Shift;
	if (Data1 != 0)
		*Data1 = desc->data1;
}

/*
 * Enable the DMA Reception.
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void synopGMAC_enable_dma_rx(synopGMACdevice * gmacdev)
{
	u32 data;
	data = synopGMACReadReg((u32 *)gmacdev->DmaBase, DmaControl);
	data |= DmaRxStart;
	synopGMACWriteReg((u32 *)gmacdev->DmaBase, DmaControl ,data);
}

/*
 * Enable the DMA Transmission.
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void synopGMAC_enable_dma_tx(synopGMACdevice * gmacdev)
{
	u32 data;
	data = synopGMACReadReg((u32 *)gmacdev->DmaBase, DmaControl);
	data |= DmaTxStart;
	synopGMACWriteReg((u32 *)gmacdev->DmaBase, DmaControl ,data);
}

/*
 * Take ownership of this Descriptor.
 * The function is same for both the ring mode and the chain mode DMA structures.
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void synopGMAC_take_desc_ownership(DmaDesc * desc)
{
	if (desc) {
		desc->status &= ~DescOwnByDma;  //Clear the DMA own bit
	}
}

/*
 * Take ownership of all the rx Descriptors.
 * This function is called when there is fatal error in DMA transmission.
 * When called it takes the ownership of all the rx descriptor in rx descriptor pool/queue from DMA.
 * The function is same for both the ring mode and the chain mode DMA structures.
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 * \note Make sure to disable the transmission before calling this function, otherwise may result in racing situation.
 */
void synopGMAC_take_desc_ownership_rx(synopGMACdevice * gmacdev)
{
	s32 i;
	DmaDesc *desc;
	desc = gmacdev->RxDesc;
	for (i = 0; i < gmacdev->RxDescCount; i++) {
		synopGMAC_take_desc_ownership(desc + i);
	}
}

/*
 * Take ownership of all the rx Descriptors.
 * This function is called when there is fatal error in DMA transmission.
 * When called it takes the ownership of all the tx descriptor in tx descriptor pool/queue from DMA.
 * The function is same for both the ring mode and the chain mode DMA structures.
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 * \note Make sure to disable the transmission before calling this function, otherwise may result in racing situation.
 */
void synopGMAC_take_desc_ownership_tx(synopGMACdevice * gmacdev)
{
	s32 i;
	DmaDesc *desc;
	desc = gmacdev->TxDesc;
	for (i = 0; i < gmacdev->TxDescCount; i++) {
		synopGMAC_take_desc_ownership(desc + i);
	}
}


/*
 * Enables the ip checksum offloading in receive path.
 * When set GMAC calculates 16 bit 1's complement of all received ethernet frame payload.
 * It also checks IPv4 Header checksum is correct. GMAC core appends the 16 bit checksum calculated
 * for payload of IP datagram and appends it to Ethernet frame transferred to the application.
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void synopGMAC_enable_rx_chksum_offload(synopGMACdevice *gmacdev)
{
        synopGMACSetBits((u32 *)gmacdev->MacBase, GmacConfig, GmacRxIpcOffload);
}


/*
 * Disable the DMA for Transmission.
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */

void synopGMAC_disable_dma_tx(synopGMACdevice * gmacdev)
{
	u32 data;

	data = synopGMACReadReg((u32 *)gmacdev->DmaBase, DmaControl);
	data &= (~DmaTxStart);
	synopGMACWriteReg((u32 *)gmacdev->DmaBase, DmaControl ,data);
}

/*
 * Disable the DMA for Reception.
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void synopGMAC_disable_dma_rx(synopGMACdevice * gmacdev)
{
	u32 data;

	data = synopGMACReadReg((u32 *)gmacdev->DmaBase, DmaControl);
	data &= (~DmaRxStart);
	synopGMACWriteReg((u32 *)gmacdev->DmaBase, DmaControl ,data);
}

/*
 * Checks whether the packet received is a magic packet?.
 * @param[in] pointer to synopGMACdevice.
 * \return returns True if magic packet received else returns false.
 */
bool synopGMAC_is_magic_packet_received(synopGMACdevice *gmacdev)
{
        u32 data;
        data = synopGMACReadReg((u32 *)gmacdev->MacBase, GmacPmtCtrlStatus);
        return ((data & GmacPmtMagicPktReceived) == GmacPmtMagicPktReceived);
}

/*
 * Checks whether the packet received is a wakeup frame?.
 * @param[in] pointer to synopGMACdevice.
 * \return returns true if wakeup frame received else returns false.
 */
bool synopGMAC_is_wakeup_frame_received(synopGMACdevice *gmacdev)
{
        u32 data;
        data = synopGMACReadReg((u32 *)gmacdev->MacBase, GmacPmtCtrlStatus);
        return ((data & GmacPmtWakeupFrameReceived) == GmacPmtWakeupFrameReceived);
}

/*
 * Disables the assertion of PMT interrupt.
 * This disables the assertion of PMT interrupt due to Magic Pkt or Wakeup frame
 * reception.
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void synopGMAC_pmt_int_disable(synopGMACdevice *gmacdev)
{
        synopGMACSetBits((u32 *)gmacdev->MacBase, GmacInterruptMask, GmacPmtIntMask);
}

/*
 * Instruct the DMA to drop the packets fails tcp ip checksum.
 * This is to instruct the receive DMA engine to drop the recevied packet if they
 * fails the tcp/ip checksum in hardware. Valid only when full checksum offloading is enabled(type-2).
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void synopGMAC_rx_tcpip_chksum_drop_enable(synopGMACdevice *gmacdev)
{
        synopGMACClearBits((u32 *)gmacdev->DmaBase, DmaControl, DmaDisableDropTcpCs);
}


/*
 * Disable the MMC Rx interrupt.
 * The MMC rx interrupts are masked out as per the mask specified.
 * @param[in] pointer to synopGMACdevice.
 * @param[in] rx interrupt bit mask for which interrupts needs to be disabled.
 * \return returns void.
 */
void synopGMAC_disable_mmc_rx_interrupt(synopGMACdevice *gmacdev, u32 mask)
{
        synopGMACSetBits((u32 *)gmacdev->MacBase, GmacMmcIntrMaskRx, mask);
}

/*
 * Disable the MMC ipc rx checksum offload interrupt.
 * The MMC ipc rx checksum offload interrupts are masked out as per the mask specified.
 * @param[in] pointer to synopGMACdevice.
 * @param[in] rx interrupt bit mask for which interrupts needs to be disabled.
 * \return returns void.
 */
void synopGMAC_disable_mmc_ipc_rx_interrupt(synopGMACdevice *gmacdev, u32 mask)
{
        synopGMACSetBits((u32 *)gmacdev->MacBase, GmacMmcRxIpcIntrMask, mask);
}


/*
 * Disable the MMC Tx interrupt.
 * The MMC tx interrupts are masked out as per the mask specified.
 * @param[in] pointer to synopGMACdevice.
 * @param[in] tx interrupt bit mask for which interrupts needs to be disabled.
 * \return returns void.
 */
void synopGMAC_disable_mmc_tx_interrupt(synopGMACdevice *gmacdev, u32 mask)
{
        synopGMACSetBits((u32 *)gmacdev->MacBase, GmacMmcIntrMaskTx, mask);
}




#if 0

/*******************PMT APIs***************************************/

/*
 * Enables the assertion of PMT interrupt.
 * This enables the assertion of PMT interrupt due to Magic Pkt or Wakeup frame
 * reception.
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void synopGMAC_pmt_int_enable(synopGMACdevice *gmacdev)
{
	synopGMACClearBits((u32 *)gmacdev->MacBase, GmacInterruptMask, GmacPmtIntMask);
}

/*
 * Disables the assertion of PMT interrupt.
 * This disables the assertion of PMT interrupt due to Magic Pkt or Wakeup frame
 * reception.
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void synopGMAC_pmt_int_disable(synopGMACdevice *gmacdev)
{
	synopGMACSetBits((u32 *)gmacdev->MacBase, GmacInterruptMask, GmacPmtIntMask);
}

/*
 * Enables the power down mode of GMAC.
 * This function puts the Gmac in power down mode.
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void synopGMAC_power_down_enable(synopGMACdevice *gmacdev)
{
	synopGMACSetBits((u32 *)gmacdev->MacBase, GmacPmtCtrlStatus, GmacPmtPowerDown);
}

/*
 * Disables the powerd down setting of GMAC.
 * If the driver wants to bring up the GMAC from powerdown mode, even though the magic packet or the
 * wake up frames received from the network, this function should be called.
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void synopGMAC_power_down_disable(synopGMACdevice *gmacdev)
{
	synopGMACClearBits((u32 *)gmacdev->MacBase, GmacPmtCtrlStatus, GmacPmtPowerDown);
}

/*
 * Enables GMAC to look for Magic packet.
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void synopGMAC_magic_packet_enable(synopGMACdevice *gmacdev)
{
	synopGMACSetBits((u32 *)gmacdev->MacBase, GmacPmtCtrlStatus, GmacPmtMagicPktEnable);
}

/*
 * Enables GMAC to look for wake up frame.
 * Wake up frame is defined by the user.
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void synopGMAC_wakeup_frame_enable(synopGMACdevice *gmacdev)
{
	synopGMACSetBits((u32 *)gmacdev->MacBase, GmacPmtCtrlStatus, GmacPmtWakeupFrameEnable);
}

/*
 * Enables wake-up frame filter to handle unicast packets.
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void synopGMAC_pmt_unicast_enable(synopGMACdevice *gmacdev)
{
	synopGMACSetBits((u32 *)gmacdev->MacBase,GmacPmtCtrlStatus,GmacPmtGlobalUnicast);
}

/*
 * Checks whether the packet received is a magic packet?.
 * @param[in] pointer to synopGMACdevice.
 * \return returns True if magic packet received else returns false.
 */
bool synopGMAC_is_magic_packet_received(synopGMACdevice *gmacdev)
{
	u32 data;
	data = synopGMACReadReg((u32 *)gmacdev->MacBase, GmacPmtCtrlStatus);
	return ((data & GmacPmtMagicPktReceived) == GmacPmtMagicPktReceived);
}

/*
 * Checks whether the packet received is a wakeup frame?.
 * @param[in] pointer to synopGMACdevice.
 * \return returns true if wakeup frame received else returns false.
 */
bool synopGMAC_is_wakeup_frame_received(synopGMACdevice *gmacdev)
{
	u32 data;
	data = synopGMACReadReg((u32 *)gmacdev->MacBase, GmacPmtCtrlStatus);
	return ((data & GmacPmtWakeupFrameReceived) == GmacPmtWakeupFrameReceived);
}

/*
 * Populates the remote wakeup frame registers.
 * Consecutive 8 writes to GmacWakeupAddr writes the wakeup frame filter registers.
 * Before commensing a new write, frame filter pointer is reset to 0x0000.
 * A small delay is introduced to allow frame filter pointer reset operation.
 * @param[in] pointer to synopGMACdevice.
 * @param[in] pointer to frame filter contents array.
 * \return returns void.
 */
void synopGMAC_write_wakeup_frame_register(synopGMACdevice *gmacdev, u32 * filter_contents)
{
	s32 i;
	synopGMACSetBits((u32 *)gmacdev->MacBase, GmacPmtCtrlStatus, GmacPmtFrmFilterPtrReset);
	__udelay(10);
	for (i = 0; i < WAKEUP_REG_LENGTH; i++)
		synopGMACWriteReg((u32 *)gmacdev->MacBase, GmacWakeupAddr, *(filter_contents + i));
}

/*******************PMT APIs***************************************/

/*******************MMC APIs***************************************/

/*
 * Freezes the MMC counters.
 * This function call freezes the MMC counters. None of the MMC counters are updated
 * due to any tx or rx frames until synopGMAC_mmc_counters_resume is called.
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void synopGMAC_mmc_counters_stop(synopGMACdevice *gmacdev)
{
	synopGMACSetBits((u32 *)gmacdev->MacBase, GmacMmcCntrl, GmacMmcCounterFreeze);
}

/*
 * Resumes the MMC counter updation.
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void synopGMAC_mmc_counters_resume(synopGMACdevice *gmacdev)
{
	synopGMACClearBits((u32 *)gmacdev->MacBase, GmacMmcCntrl, GmacMmcCounterFreeze);
}

/*
 * Configures the MMC in Self clearing mode.
 * Programs MMC interface so that counters are cleared when the counters are read.
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void synopGMAC_mmc_counters_set_selfclear(synopGMACdevice *gmacdev)
{
	synopGMACSetBits((u32 *)gmacdev->MacBase, GmacMmcCntrl, GmacMmcCounterResetOnRead);
}

/*
 * Configures the MMC in non-Self clearing mode.
 * Programs MMC interface so that counters are cleared when the counters are read.
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void synopGMAC_mmc_counters_reset_selfclear(synopGMACdevice *gmacdev)
{
	synopGMACClearBits((u32 *)gmacdev->MacBase, GmacMmcCntrl, GmacMmcCounterResetOnRead);
}

/*
 * Configures the MMC to stop rollover.
 * Programs MMC interface so that counters will not rollover after reaching maximum value.
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void synopGMAC_mmc_counters_disable_rollover(synopGMACdevice *gmacdev)
{
	synopGMACSetBits((u32 *)gmacdev->MacBase, GmacMmcCntrl, GmacMmcCounterStopRollover);
}

/*
 * Configures the MMC to rollover.
 * Programs MMC interface so that counters will rollover after reaching maximum value.
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void synopGMAC_mmc_counters_enable_rollover(synopGMACdevice *gmacdev)
{
	synopGMACClearBits((u32 *)gmacdev->MacBase, GmacMmcCntrl, GmacMmcCounterStopRollover);
}

/*
 * Read the MMC Counter.
 * @param[in] pointer to synopGMACdevice.
 * @param[in] the counter to be read.
 * \return returns the read count value.
 */
u32 synopGMAC_read_mmc_counter(synopGMACdevice *gmacdev, u32 counter)
{
	return(	synopGMACReadReg((u32 *)gmacdev->MacBase, counter));
}

/*
 * Read the MMC Rx interrupt status.
 * @param[in] pointer to synopGMACdevice.
 * \return returns the Rx interrupt status.
 */
u32 synopGMAC_read_mmc_rx_int_status(synopGMACdevice *gmacdev)
{
	return(	synopGMACReadReg((u32 *)gmacdev->MacBase, GmacMmcIntrRx));
}

/*
 * Read the MMC Tx interrupt status.
 * @param[in] pointer to synopGMACdevice.
 * \return returns the Tx interrupt status.
 */
u32 synopGMAC_read_mmc_tx_int_status(synopGMACdevice *gmacdev)
{
	return(	synopGMACReadReg((u32 *)gmacdev->MacBase, GmacMmcIntrTx));
}

/*
 * Disable the MMC Tx interrupt.
 * The MMC tx interrupts are masked out as per the mask specified.
 * @param[in] pointer to synopGMACdevice.
 * @param[in] tx interrupt bit mask for which interrupts needs to be disabled.
 * \return returns void.
 */
void synopGMAC_disable_mmc_tx_interrupt(synopGMACdevice *gmacdev, u32 mask)
{
	synopGMACSetBits((u32 *)gmacdev->MacBase, GmacMmcIntrMaskTx, mask);
}

/*
 * Enable the MMC Tx interrupt.
 * The MMC tx interrupts are enabled as per the mask specified.
 * @param[in] pointer to synopGMACdevice.
 * @param[in] tx interrupt bit mask for which interrupts needs to be enabled.
 * \return returns void.
 */
void synopGMAC_enable_mmc_tx_interrupt(synopGMACdevice *gmacdev, u32 mask)
{
	synopGMACClearBits((u32 *)gmacdev->MacBase, GmacMmcIntrMaskTx, mask);
}

/*
 * Disable the MMC Rx interrupt.
 * The MMC rx interrupts are masked out as per the mask specified.
 * @param[in] pointer to synopGMACdevice.
 * @param[in] rx interrupt bit mask for which interrupts needs to be disabled.
 * \return returns void.
 */
void synopGMAC_disable_mmc_rx_interrupt(synopGMACdevice *gmacdev, u32 mask)
{
	synopGMACSetBits((u32 *)gmacdev->MacBase, GmacMmcIntrMaskRx, mask);
}

/*
 * Enable the MMC Rx interrupt.
 * The MMC rx interrupts are enabled as per the mask specified.
 * @param[in] pointer to synopGMACdevice.
 * @param[in] rx interrupt bit mask for which interrupts needs to be enabled.
 * \return returns void.
 */
void synopGMAC_enable_mmc_rx_interrupt(synopGMACdevice *gmacdev, u32 mask)
{
	synopGMACClearBits((u32 *)gmacdev->MacBase, GmacMmcIntrMaskRx, mask);
}

/*
 * Disable the MMC ipc rx checksum offload interrupt.
 * The MMC ipc rx checksum offload interrupts are masked out as per the mask specified.
 * @param[in] pointer to synopGMACdevice.
 * @param[in] rx interrupt bit mask for which interrupts needs to be disabled.
 * \return returns void.
 */
void synopGMAC_disable_mmc_ipc_rx_interrupt(synopGMACdevice *gmacdev, u32 mask)
{
	synopGMACSetBits((u32 *)gmacdev->MacBase, GmacMmcRxIpcIntrMask, mask);
}

/*
 * Enable the MMC ipc rx checksum offload interrupt.
 * The MMC ipc rx checksum offload interrupts are enabled as per the mask specified.
 * @param[in] pointer to synopGMACdevice.
 * @param[in] rx interrupt bit mask for which interrupts needs to be enabled.
 * \return returns void.
 */
void synopGMAC_enable_mmc_ipc_rx_interrupt(synopGMACdevice *gmacdev, u32 mask)
{
	synopGMACClearBits((u32 *)gmacdev->MacBase, GmacMmcRxIpcIntrMask, mask);
}

/*******************MMC APIs***************************************/

/*******************Ip checksum offloading APIs***************************************/

/*
 * Enables the ip checksum offloading in receive path.
 * When set GMAC calculates 16 bit 1's complement of all received ethernet frame payload.
 * It also checks IPv4 Header checksum is correct. GMAC core appends the 16 bit checksum calculated
 * for payload of IP datagram and appends it to Ethernet frame transferred to the application.
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void synopGMAC_enable_rx_chksum_offload(synopGMACdevice *gmacdev)
{
	synopGMACSetBits((u32 *)gmacdev->MacBase, GmacConfig, GmacRxIpcOffload);
}

/*
 * Disable the ip checksum offloading in receive path.
 * Ip checksum offloading is disabled in the receive path.
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void synopGMAC_disable_rx_Ipchecksum_offload(synopGMACdevice *gmacdev)
{
	synopGMACClearBits((u32 *)gmacdev->MacBase, GmacConfig, GmacRxIpcOffload);
}

/*
 * Instruct the DMA to drop the packets fails tcp ip checksum.
 * This is to instruct the receive DMA engine to drop the recevied packet if they
 * fails the tcp/ip checksum in hardware. Valid only when full checksum offloading is enabled(type-2).
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void synopGMAC_rx_tcpip_chksum_drop_enable(synopGMACdevice *gmacdev)
{
	synopGMACClearBits((u32 *)gmacdev->DmaBase, DmaControl, DmaDisableDropTcpCs);
}

/*
 * Instruct the DMA not to drop the packets even if it fails tcp ip checksum.
 * This is to instruct the receive DMA engine to allow the packets even if recevied packet
 * fails the tcp/ip checksum in hardware. Valid only when full checksum offloading is enabled(type-2).
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */
void synopGMAC_rx_tcpip_chksum_drop_disable(synopGMACdevice *gmacdev)
{
	synopGMACSetBits((u32 *)gmacdev->DmaBase, DmaControl, DmaDisableDropTcpCs);
}

#endif
