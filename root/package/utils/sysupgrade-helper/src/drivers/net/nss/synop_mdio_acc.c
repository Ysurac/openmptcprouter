#include <linux/mii.h>
#include <asm/io.h>


#define DEFAULT_LOOP_VARIABLE 10000
#define DEFAULT_DELAY_VARIABLE  10
#define GMACNOERR   0
#define GMACNOMEM   1
#define GMACPHYERR  2
#define GMACBUSY    3

#define PHYBASE 0x0

enum GmacGmiiAddrReg {
	GmiiDevMask	= 0x0000F800,	/* (PA)GMII device address                 15:11     RW         0x00    */
	GmiiDevShift	= 11,

	GmiiRegMask	= 0x000007C0,	/* (GR)GMII register in selected Phy       10:6      RW         0x00    */
	GmiiRegShift	= 6,

	GmiiCsrClkMask	= 0x0000001C,	/*CSR Clock bit Mask                        4:2                         */
	GmiiCsrClk5	= 0x00000014,	/* (CR)CSR Clock Range     250-300 MHz      4:2      RW         000     */
	GmiiCsrClk4	= 0x00000010,	/*                         150-250 MHz                                  */
	GmiiCsrClk3	= 0x0000000C,	/*                         35-60 MHz                                    */
	GmiiCsrClk2	= 0x00000008,	/*                         20-35 MHz                                    */
	GmiiCsrClk1	= 0x00000004,	/*                         100-150 MHz                                  */
	GmiiCsrClk0	= 0x00000000,	/*                         60-100 MHz                                   */

	GmiiWrite	= 0x00000002,	/* (GW)Write to register                      1      RW                 */
	GmiiRead	= 0x00000000,	/* Read from register                                            0      */

	GmiiBusy	= 0x00000001,	/* (GB)GMII interface is busy                 0      RW          0      */
};

enum GmacRegisters {
	GmacGmiiAddr	= 0x0010,    /* GMII address Register(ext. Phy) */
	GmacGmiiData	= 0x0014,
};

enum MiiRegisters {
	PHY_CONTROL_REG			= 0x0000,	/*Control Register*/
	PHY_STATUS_REG			= 0x0001,	/*Status Register */
	PHY_ID_HI_REG			= 0x0002,	/*PHY Identifier High Register*/
	PHY_ID_LOW_REG			= 0x0003,	/*PHY Identifier High Register*/
	PHY_AN_ADV_REG			= 0x0004,	/*Auto-Negotiation Advertisement Register*/
	PHY_LNK_PART_ABl_REG		= 0x0005,	/*Link Partner Ability Register (Base Page)*/
	PHY_AN_EXP_REG			= 0x0006,	/*Auto-Negotiation Expansion Register*/
	PHY_AN_NXT_PAGE_TX_REG    	= 0x0007,	/*Next Page Transmit Register*/
	PHY_LNK_PART_NXT_PAGE_REG 	= 0x0008,	/*Link Partner Next Page Register*/
	PHY_1000BT_CTRL_REG		= 0x0009,	/*1000BASE-T Control Register*/
	PHY_1000BT_STATUS_REG		= 0x000a,	/*1000BASE-T Status Register*/
	PHY_SPECIFIC_CTRL_REG		= 0x0010,	/*Phy specific control register*/
	PHY_SPECIFIC_STATUS_REG   	= 0x0011,	/*Phy specific status register*/
	PHY_INTERRUPT_ENABLE_REG  	= 0x0012,	/*Phy interrupt enable register*/
	PHY_INTERRUPT_STATUS_REG  	= 0x0013,	/*Phy interrupt status register*/
	PHY_EXT_PHY_SPC_CTRL		= 0x0014,	/*Extended Phy specific control*/
	PHY_RX_ERR_COUNTER		= 0x0015,	/*Receive Error Counter*/
	PHY_EXT_ADDR_CBL_DIAG		= 0x0016,	/*Extended address for cable diagnostic register*/
	PHY_LED_CONTROL			= 0x0018,	/*LED Control*/
	PHY_MAN_LED_OVERIDE		= 0x0019,	/*Manual LED override register*/
	PHY_EXT_PHY_SPC_CTRL2		= 0x001a,	/*Extended Phy specific control 2*/
	PHY_EXT_PHY_SPC_STATUS    	= 0x001b,	/*Extended Phy specific status*/
	PHY_CBL_DIAG_REG		= 0x001c,	/*Cable diagnostic registers*/
	PHY_DEBUG_PORT			= 0x001d,	/*Debug port address offset registers*/
	PHY_DEBUG_PORT_2		= 0x001e,	/*Data port of Debug registers*/
};




enum Mii_GEN_CTRL {				/* Description                  bits  R/W default value  */
	Mii_Reset		= 0x8000,
	Mii_Speed_10		= 0x0000,	/* 10   Mbps                    6:13  RW */
	Mii_Speed_100		= 0x2000,	/* 100  Mbps                    6:13  RW */
	Mii_Speed_1000		= 0x0040,	/* 1000 Mbit/s                  6:13  RW */

	Mii_Duplex		= 0x0100,	/* Full Duplex mode             8     RW */

	Mii_Manual_Master_Config = 0x0800,	/* Manual Master Config         11    RW */

	Mii_Autoneg		= 0x1000,	/* Auto-negotiation             12    RW */

	Mii_Loopback		= 0x4000,	/* Enable Loop back             14    RW */
	Mii_NoLoopback		= 0x0000,	/* Enable Loop back             14    RW */
};


enum Mii_Phy_Status {
	Mii_phy_status_speed_10		= 0x0000,
	Mii_phy_status_speed_100	= 0x4000,
	Mii_phy_status_speed_1000	= 0x8000,

	Mii_phy_status_full_duplex	= 0x2000,
	Mii_phy_status_half_duplex	= 0x0000,

	Mii_phy_status_link_up		= 0x0400,
};
/* This is Status register layout. Status register is of 16 bit wide.
*/
enum Mii_GEN_STATUS {
	Mii_AutoNegCmplt	= 0x0020,   /* Autonegotiation completed 5 RW */
	Mii_Link		= 0x0004,   /* Link status               2 RW */
};

enum Mii_Link_Status {
	LINKDOWN	= 0,
	LINKUP		= 1,
};

enum Mii_Duplex_Mode {
	HALFDUPLEX = 1,
	FULLDUPLEX = 2,
};
enum Mii_Link_Speed {
	SPEED10		= 1,
	SPEED100	= 2,
	SPEED1000	= 3,
};

enum Mii_Loop_Back {
	NOLOOPBACK	= 0,
	LOOPBACK	= 1,
};



u32 GMACReadReg(u32  *RegBase, u32  RegOffset)
{
	unsigned int data;
	u32  addr = (u32 )RegBase + RegOffset;
	data = readl(addr);
	return data;

}


void  GMACWriteReg(u32 *RegBase, u32 RegOffset, u32 RegData)
{
	u32 addr = (u32)RegBase + RegOffset;
	writel(RegData, (u32)addr);
	return;
}



u32 GMAC_write_phy_reg(u32  *RegBase, u32  PhyBase, u32  RegOffset, u16 data)
{
	u32  addr;
	u32  loop_variable;

	GMACWriteReg(RegBase, GmacGmiiData, data); // write the data in to GmacGmiiData register of GMAC ip

	addr = ((PhyBase << GmiiDevShift) & GmiiDevMask) | ((RegOffset << GmiiRegShift) & GmiiRegMask) | GmiiWrite;

	addr = addr | GmiiBusy ; //set Gmii clk to 20-35 Mhz and Gmii busy bit

	addr = addr | 0x14;

	GMACWriteReg(RegBase, GmacGmiiAddr, addr);
	udelay(10);

	for (loop_variable = 0; loop_variable < DEFAULT_LOOP_VARIABLE; loop_variable++) {
		if (!(GMACReadReg(RegBase,GmacGmiiAddr) & GmiiBusy)) {
			break;
		}
		udelay(300);
	}

	if (loop_variable < DEFAULT_LOOP_VARIABLE) {
		return 1;
	} else {
		return 2;
	}
}

u32 GMAC_read_phy_reg(u32 *RegBase,u32 PhyBase, u32 RegOffset, u16 * data)
{
	u32 addr;
	u32 loop_variable;
	addr = ((PhyBase << GmiiDevShift) & GmiiDevMask) | ((RegOffset << GmiiRegShift) & GmiiRegMask);
	addr = addr | GmiiBusy ; //Gmii busy bit
	addr = addr | 0x14;

	GMACWriteReg(RegBase, GmacGmiiAddr, addr); //write the address from where the data to be read in GmiiGmiiAddr register of GMAC ip

	for (loop_variable = 0; loop_variable < DEFAULT_LOOP_VARIABLE; loop_variable++) { //Wait till the busy bit gets cleared with in a certain amount of time
		if (!(GMACReadReg(RegBase,GmacGmiiAddr) & GmiiBusy)) {
			break;
		}
		udelay(DEFAULT_DELAY_VARIABLE);
	}
	if (loop_variable < DEFAULT_LOOP_VARIABLE) {
		* data = (u16 )(GMACReadReg(RegBase,GmacGmiiData) & 0xFFFF);
	} else {
		return 1;
	}
	return 0;
}


void synop_phy_outofreset()
{
	u16 mdio_ctrl_reg = 0;

	//Debug reg write
	GMAC_write_phy_reg(0x37000000, PHYBASE, PHY_DEBUG_PORT, 0x05);
	GMAC_write_phy_reg(0x37000000, PHYBASE, PHY_DEBUG_PORT_2, 0x100);

	udelay(1000);

	//Debug reg write//3.4ns TX delay
	GMAC_write_phy_reg(0x37000000, PHYBASE, PHY_DEBUG_PORT, 0x0B);
	GMAC_write_phy_reg(0x37000000, PHYBASE, PHY_DEBUG_PORT_2, 0xBC40);

	udelay(1000);
}

int synop_phy_link_status(int speed)
{
	int temp;
	u32 mdio_ctrl_reg = 0;

	udelay(1000);

	GMAC_read_phy_reg(0x37000000, PHYBASE, PHY_SPECIFIC_STATUS_REG, &mdio_ctrl_reg);
	temp = (mdio_ctrl_reg >> 10) & 1;

	if(temp != 1) {
		printf("PHY Link is Down !!!!\n");
	} else {
		printf("PHY Link is Up !!!!\n");

	}
	/* TODO: Send the speed according to the status register */
	return SPEED1000;

}


void Phy_loop_back_on()
{
	u32 mdio_ctrl_reg = 0;

	GMAC_read_phy_reg(0x37000000, PHYBASE, PHY_CONTROL_REG, &mdio_ctrl_reg);
	GMAC_write_phy_reg(0x37000000, PHYBASE, PHY_CONTROL_REG, (mdio_ctrl_reg | Mii_Loopback));
}

/*
 * Function to read the Phy register. The access to phy register
 * is a slow process as the data is moved accross MDI/MDO interface
 * @param[in] pointer to Register Base (It is the mac base in our case) .
 * @param[in] PhyBase register is the index of one of supported 32 PHY devices.
 * @param[in] Register offset is the index of one of the 32 phy register.
 * return data from the respective phy register.
 */
u16 ATH_GMAC_read_phy_reg(u32 *RegBase, u32 PhyBase, u32 RegOffset)
{
	u16 data = 0;
	GMAC_read_phy_reg(RegBase, PhyBase, RegOffset, &data);
	return data;
}
