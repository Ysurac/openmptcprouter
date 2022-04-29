/*===========================================================================

                            M P Q M U X. H
DESCRIPTION:

    This file provides support for QMUX.

INITIALIZATION AND SEQUENCING REQUIREMENTS:

Copyright (C) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

#ifndef MPQMUX_H
#define MPQMUX_H

#include "MPQMI.h"

#pragma pack(push, 1)

#define QMIWDS_SET_EVENT_REPORT_REQ           0x0001
#define QMIWDS_SET_EVENT_REPORT_RESP          0x0001
#define QMIWDS_EVENT_REPORT_IND               0x0001
#define QMIWDS_START_NETWORK_INTERFACE_REQ    0x0020
#define QMIWDS_START_NETWORK_INTERFACE_RESP   0x0020
#define QMIWDS_STOP_NETWORK_INTERFACE_REQ     0x0021
#define QMIWDS_STOP_NETWORK_INTERFACE_RESP    0x0021
#define QMIWDS_GET_PKT_SRVC_STATUS_REQ        0x0022
#define QMIWDS_GET_PKT_SRVC_STATUS_RESP       0x0022
#define QMIWDS_GET_PKT_SRVC_STATUS_IND        0x0022
#define QMIWDS_GET_CURRENT_CHANNEL_RATE_REQ   0x0023
#define QMIWDS_GET_CURRENT_CHANNEL_RATE_RESP  0x0023
#define QMIWDS_GET_PKT_STATISTICS_REQ         0x0024
#define QMIWDS_GET_PKT_STATISTICS_RESP        0x0024
#define QMIWDS_MODIFY_PROFILE_SETTINGS_REQ    0x0028
#define QMIWDS_MODIFY_PROFILE_SETTINGS_RESP   0x0028
#define QMIWDS_GET_PROFILE_SETTINGS_REQ         0x002B
#define QMIWDS_GET_PROFILE_SETTINGS_RESP        0x002B
#define QMIWDS_GET_DEFAULT_SETTINGS_REQ       0x002C
#define QMIWDS_GET_DEFAULT_SETTINGS_RESP      0x002C
#define QMIWDS_GET_RUNTIME_SETTINGS_REQ       0x002D
#define QMIWDS_GET_RUNTIME_SETTINGS_RESP      0x002D
#define QMIWDS_GET_MIP_MODE_REQ               0x002F
#define QMIWDS_GET_MIP_MODE_RESP              0x002F
#define QMIWDS_GET_DATA_BEARER_REQ            0x0037
#define QMIWDS_GET_DATA_BEARER_RESP           0x0037
#define QMIWDS_DUN_CALL_INFO_REQ              0x0038
#define QMIWDS_DUN_CALL_INFO_RESP             0x0038
#define QMIWDS_DUN_CALL_INFO_IND              0x0038
#define QMIWDS_SET_CLIENT_IP_FAMILY_PREF_REQ  0x004D
#define QMIWDS_SET_CLIENT_IP_FAMILY_PREF_RESP 0x004D
#define QMIWDS_BIND_MUX_DATA_PORT_REQ         0x00A2
#define QMIWDS_BIND_MUX_DATA_PORT_RESP        0x00A2


// Stats masks
#define QWDS_STAT_MASK_TX_PKT_OK 0x00000001
#define QWDS_STAT_MASK_RX_PKT_OK 0x00000002
#define QWDS_STAT_MASK_TX_PKT_ER 0x00000004
#define QWDS_STAT_MASK_RX_PKT_ER 0x00000008
#define QWDS_STAT_MASK_TX_PKT_OF 0x00000010
#define QWDS_STAT_MASK_RX_PKT_OF 0x00000020

// TLV Types for xfer statistics
#define TLV_WDS_TX_GOOD_PKTS      0x10
#define TLV_WDS_RX_GOOD_PKTS      0x11
#define TLV_WDS_TX_ERROR          0x12
#define TLV_WDS_RX_ERROR          0x13
#define TLV_WDS_TX_OVERFLOW       0x14
#define TLV_WDS_RX_OVERFLOW       0x15
#define TLV_WDS_CHANNEL_RATE      0x16
#define TLV_WDS_DATA_BEARER       0x17
#define TLV_WDS_DORMANCY_STATUS   0x18

#define QWDS_PKT_DATA_DISCONNECTED    0x01
#define QWDS_PKT_DATA_CONNECTED        0x02
#define QWDS_PKT_DATA_SUSPENDED        0x03
#define QWDS_PKT_DATA_AUTHENTICATING   0x04

#define QMIWDS_ADMIN_SET_DATA_FORMAT_REQ      0x0020
#define QMIWDS_ADMIN_SET_DATA_FORMAT_RESP     0x0020
#define QMIWDS_ADMIN_GET_DATA_FORMAT_REQ      0x0021
#define QMIWDS_ADMIN_GET_DATA_FORMAT_RESP     0x0021
#define QMIWDS_ADMIN_SET_QMAP_SETTINGS_REQ    0x002B
#define QMIWDS_ADMIN_SET_QMAP_SETTINGS_RESP   0x002B
#define QMIWDS_ADMIN_GET_QMAP_SETTINGS_REQ    0x002C
#define QMIWDS_ADMIN_GET_QMAP_SETTINGS_RESP   0x002C

#define NETWORK_DESC_ENCODING_OCTET       0x00
#define NETWORK_DESC_ENCODING_EXTPROTOCOL 0x01
#define NETWORK_DESC_ENCODING_7BITASCII   0x02
#define NETWORK_DESC_ENCODING_IA5         0x03
#define NETWORK_DESC_ENCODING_UNICODE     0x04
#define NETWORK_DESC_ENCODING_SHIFTJIS    0x05
#define NETWORK_DESC_ENCODING_KOREAN      0x06
#define NETWORK_DESC_ENCODING_LATINH      0x07
#define NETWORK_DESC_ENCODING_LATIN       0x08
#define NETWORK_DESC_ENCODING_GSM7BIT     0x09
#define NETWORK_DESC_ENCODING_GSMDATA     0x0A
#define NETWORK_DESC_ENCODING_UNKNOWN     0xFF

typedef struct _QMIWDS_ADMIN_SET_DATA_FORMAT
{
   USHORT Type;             // QMUX type 0x0000
   USHORT Length;
} __attribute__ ((packed)) QMIWDS_ADMIN_SET_DATA_FORMAT, *PQMIWDS_ADMIN_SET_DATA_FORMAT;

typedef struct _QMIWDS_ADMIN_SET_DATA_FORMAT_TLV_QOS
{
   UCHAR  TLVType;
   USHORT TLVLength;
   UCHAR  QOSSetting;
} __attribute__ ((packed)) QMIWDS_ADMIN_SET_DATA_FORMAT_TLV_QOS, *PQMIWDS_ADMIN_SET_DATA_FORMAT_TLV_QOS;

typedef struct _QMIWDS_ADMIN_SET_DATA_FORMAT_TLV
{
   UCHAR  TLVType;
   USHORT TLVLength;
   ULONG  Value;
} __attribute__ ((packed)) QMIWDS_ADMIN_SET_DATA_FORMAT_TLV, *PQMIWDS_ADMIN_SET_DATA_FORMAT_TLV;


//#ifdef QC_IP_MODE

#define QMIWDS_GET_RUNTIME_SETTINGS_MASK_IPV4DNS_ADDR 0x0010
#define QMIWDS_GET_RUNTIME_SETTINGS_MASK_IPV4_ADDR 0x0100
#define QMIWDS_GET_RUNTIME_SETTINGS_MASK_IPV4GATEWAY_ADDR 0x0200
#define QMIWDS_GET_RUNTIME_SETTINGS_MASK_MTU              0x2000

typedef struct _QMIWDS_GET_RUNTIME_SETTINGS_REQ_MSG
{
   USHORT Type;            // QMIWDS_GET_RUNTIME_SETTINGS_REQ
   USHORT Length;
   UCHAR  TLVType;         // 0x10
   USHORT TLVLength;       // 0x0004
   ULONG  Mask;            // mask, bit 8: IP addr -- 0x0100
} __attribute__ ((packed)) QMIWDS_GET_RUNTIME_SETTINGS_REQ_MSG, *PQMIWDS_GET_RUNTIME_SETTINGS_REQ_MSG;

typedef struct _QMIWDS_BIND_MUX_DATA_PORT_REQ_MSG
{
   USHORT Type;
   USHORT Length;
   UCHAR  TLVType;
   USHORT TLVLength;
   ULONG  ep_type;
   ULONG  iface_id;
   UCHAR  TLV2Type;
   USHORT TLV2Length;
   UCHAR  MuxId;
   UCHAR  TLV3Type;
   USHORT TLV3Length;
   ULONG  client_type;
} __attribute__ ((packed)) QMIWDS_BIND_MUX_DATA_PORT_REQ_MSG, *PQMIWDS_BIND_MUX_DATA_PORT_REQ_MSG;

#define QMIWDS_GET_RUNTIME_SETTINGS_TLV_TYPE_IPV4PRIMARYDNS 0x15
#define QMIWDS_GET_RUNTIME_SETTINGS_TLV_TYPE_IPV4SECONDARYDNS 0x16
#define QMIWDS_GET_RUNTIME_SETTINGS_TLV_TYPE_IPV4 0x1E
#define QMIWDS_GET_RUNTIME_SETTINGS_TLV_TYPE_IPV4GATEWAY 0x20
#define QMIWDS_GET_RUNTIME_SETTINGS_TLV_TYPE_IPV4SUBNET 0x21

#define QMIWDS_GET_RUNTIME_SETTINGS_TLV_TYPE_IPV6             0x25
#define QMIWDS_GET_RUNTIME_SETTINGS_TLV_TYPE_IPV6GATEWAY      0x26
#define QMIWDS_GET_RUNTIME_SETTINGS_TLV_TYPE_IPV6PRIMARYDNS   0x27
#define QMIWDS_GET_RUNTIME_SETTINGS_TLV_TYPE_IPV6SECONDARYDNS 0x28
#define QMIWDS_GET_RUNTIME_SETTINGS_TLV_TYPE_MTU              0x29

typedef struct _QMIWDS_GET_RUNTIME_SETTINGS_TLV_MTU
{
   UCHAR  TLVType;         // QMIWDS_GET_RUNTIME_SETTINGS_TLV_TYPE_MTU
   USHORT TLVLength;       // 4
   ULONG  Mtu;             // MTU
} __attribute__ ((packed)) QMIWDS_GET_RUNTIME_SETTINGS_TLV_MTU, *PQMIWDS_GET_RUNTIME_SETTINGS_TLV_MTU;

typedef struct _QMIWDS_GET_RUNTIME_SETTINGS_TLV_IPV4_ADDR
{
   UCHAR  TLVType;         // QMIWDS_GET_RUNTIME_SETTINGS_TLV_TYPE_IPV4
   USHORT TLVLength;       // 4
   ULONG  IPV4Address;     // address
} __attribute__ ((packed)) QMIWDS_GET_RUNTIME_SETTINGS_TLV_IPV4_ADDR, *PQMIWDS_GET_RUNTIME_SETTINGS_TLV_IPV4_ADDR;

typedef struct _QMIWDS_GET_RUNTIME_SETTINGS_TLV_IPV6_ADDR
{
   UCHAR  TLVType;         // QMIWDS_GET_RUNTIME_SETTINGS_TLV_TYPE_IPV6
   USHORT TLVLength;       // 16
   UCHAR  IPV6Address[16]; // address
   UCHAR  PrefixLength;    // prefix length
} __attribute__ ((packed)) QMIWDS_GET_RUNTIME_SETTINGS_TLV_IPV6_ADDR, *PQMIWDS_GET_RUNTIME_SETTINGS_TLV_IPV6_ADDR;

typedef struct _QMIWDS_GET_RUNTIME_SETTINGS_RESP_MSG
{
   USHORT Type;            // QMIWDS_GET_RUNTIME_SETTINGS_RESP
   USHORT Length;
   UCHAR  TLVType;         // QCTLV_TYPE_RESULT_CODE
   USHORT TLVLength;       // 0x0004
   USHORT QMUXResult;      // result code
   USHORT QMUXError;       // error code
} __attribute__ ((packed)) QMIWDS_GET_RUNTIME_SETTINGS_RESP_MSG, *PQMIWDS_GET_RUNTIME_SETTINGS_RESP_MSG;

//#endif // QC_IP_MODE

typedef struct _QMIWDS_IP_FAMILY_TLV
{
   UCHAR  TLVType;          // 0x12
   USHORT TLVLength;        // 1
   UCHAR  IpFamily;         // IPV4-0x04, IPV6-0x06
} __attribute__ ((packed)) QMIWDS_IP_FAMILY_TLV, *PQMIWDS_IP_FAMILY_TLV;

typedef struct _QMIWDS_PKT_SRVC_TLV
{
   UCHAR  TLVType;
   USHORT TLVLength;
   UCHAR  ConnectionStatus;
   UCHAR  ReconfigReqd;
} __attribute__ ((packed)) QMIWDS_PKT_SRVC_TLV, *PQMIWDS_PKT_SRVC_TLV;


typedef struct _QMIWDS_TECHNOLOGY_PREFERECE
{
   UCHAR  TLVType;
   USHORT TLVLength;
   UCHAR  TechPreference;
} __attribute__ ((packed)) QMIWDS_TECHNOLOGY_PREFERECE, *PQMIWDS_TECHNOLOGY_PREFERECE;

typedef struct _QMIWDS_PROFILE_IDENTIFIER
{
   UCHAR  TLVType;
   USHORT TLVLength;
   UCHAR  ProfileIndex;
} __attribute__ ((packed)) QMIWDS_PROFILE_IDENTIFIER, *PQMIWDS_PROFILE_IDENTIFIER;


typedef struct _QMIWDS_PROFILENAME
{
   UCHAR  TLVType;
   USHORT TLVLength;
   UCHAR  ProfileName;
} __attribute__ ((packed)) QMIWDS_PROFILENAME, *PQMIWDS_PROFILENAME;

typedef struct _QMIWDS_PDPTYPE
{
   UCHAR  TLVType;
   USHORT TLVLength;
// 0 每 PDP-IP (IPv4)
// 1 每 PDP-PPP
// 2 每 PDP-IPv6
// 3 每 PDP-IPv4v6
    UCHAR  PdpType;
} __attribute__ ((packed)) QMIWDS_PDPTYPE, *PQMIWDS_PDPTYPE;

typedef struct _QMIWDS_USERNAME
{
   UCHAR  TLVType;
   USHORT TLVLength;
   UCHAR  UserName;
} __attribute__ ((packed)) QMIWDS_USERNAME, *PQMIWDS_USERNAME;

typedef struct _QMIWDS_PASSWD
{
   UCHAR  TLVType;
   USHORT TLVLength;
   UCHAR  Passwd;
} __attribute__ ((packed)) QMIWDS_PASSWD, *PQMIWDS_PASSWD;

typedef struct _QMIWDS_AUTH_PREFERENCE
{
   UCHAR  TLVType;
   USHORT TLVLength;
   UCHAR  AuthPreference;
} __attribute__ ((packed)) QMIWDS_AUTH_PREFERENCE, *PQMIWDS_AUTH_PREFERENCE;

typedef struct _QMIWDS_APNNAME
{
   UCHAR  TLVType;
   USHORT TLVLength;
   UCHAR  ApnName;
} __attribute__ ((packed)) QMIWDS_APNNAME, *PQMIWDS_APNNAME;

typedef struct _QMIWDS_AUTOCONNECT
{
   UCHAR  TLVType;
   USHORT TLVLength;
   UCHAR  AutoConnect;
} __attribute__ ((packed)) QMIWDS_AUTOCONNECT, *PQMIWDS_AUTOCONNECT;

typedef struct _QMIWDS_START_NETWORK_INTERFACE_REQ_MSG
{
   USHORT Type;
   USHORT Length;
} __attribute__ ((packed)) QMIWDS_START_NETWORK_INTERFACE_REQ_MSG, *PQMIWDS_START_NETWORK_INTERFACE_REQ_MSG;

typedef struct _QMIWDS_CALLENDREASON
{
   UCHAR  TLVType;
   USHORT TLVLength;
   USHORT Reason;
}__attribute__ ((packed)) QMIWDS_CALLENDREASON, *PQMIWDS_CALLENDREASON;

typedef struct _QMIWDS_START_NETWORK_INTERFACE_RESP_MSG
{
   USHORT Type;             // QMUX type 0x0040
   USHORT Length;
   UCHAR  TLVType;          // 0x02
   USHORT TLVLength;        // 4
   USHORT QMUXResult;       // QMI_RESULT_SUCCESS
                            // QMI_RESULT_FAILURE
   USHORT QMUXError;        // QMI_ERR_INVALID_ARG
                            // QMI_ERR_NO_MEMORY
                            // QMI_ERR_INTERNAL
                            // QMI_ERR_FAULT

   UCHAR  TLV2Type;         // 0x01
   USHORT TLV2Length;       // 20
   ULONG  Handle;          //
} __attribute__ ((packed)) QMIWDS_START_NETWORK_INTERFACE_RESP_MSG, *PQMIWDS_START_NETWORK_INTERFACE_RESP_MSG;

typedef struct _QMIWDS_STOP_NETWORK_INTERFACE_REQ_MSG
{
   USHORT Type;
   USHORT Length;
   UCHAR  TLVType;
   USHORT TLVLength;
   ULONG  Handle;
} __attribute__ ((packed)) QMIWDS_STOP_NETWORK_INTERFACE_REQ_MSG, *PQMIWDS_STOP_NETWORK_INTERFACE_REQ_MSG;

typedef struct _QMIWDS_STOP_NETWORK_INTERFACE_RESP_MSG
{
   USHORT Type;             // QMUX type 0x0040
   USHORT Length;
   UCHAR  TLVType;          // 0x02
   USHORT TLVLength;        // 4
   USHORT QMUXResult;       // QMI_RESULT_SUCCESS
                            // QMI_RESULT_FAILURE
   USHORT QMUXError;        // QMI_ERR_INVALID_ARG
                            // QMI_ERR_NO_MEMORY
                            // QMI_ERR_INTERNAL
                            // QMI_ERR_FAULT

} __attribute__ ((packed)) QMIWDS_STOP_NETWORK_INTERFACE_RESP_MSG, *PQMIWDS_STOP_NETWORK_INTERFACE_RESP_MSG;

typedef struct _QMIWDS_GET_DEFAULT_SETTINGS_REQ_MSG
{
   USHORT Type;
   USHORT Length;
   UCHAR  TLVType;
   USHORT TLVLength;
   UCHAR  ProfileType;
} __attribute__ ((packed)) QMIWDS_GET_DEFAULT_SETTINGS_REQ_MSG, *PQMIWDS_GET_DEFAULT_SETTINGS_REQ_MSG;

typedef struct _QMIWDS_GET_DEFAULT_SETTINGS_RESP_MSG
{
   USHORT Type;
   USHORT Length;
   UCHAR  TLVType;
   USHORT TLVLength;
   USHORT QMUXResult;
   USHORT QMUXError;
} __attribute__ ((packed)) QMIWDS_GET_DEFAULT_SETTINGS_RESP_MSG, *PQMIWDS_GET_DEFAULT_SETTINGS_RESP_MSG;

typedef struct _QMIWDS_MODIFY_PROFILE_SETTINGS_REQ_MSG
{
   USHORT Type;
   USHORT Length;
   UCHAR  TLVType;
   USHORT TLVLength;
   UCHAR  ProfileType;
   UCHAR  ProfileIndex;
} __attribute__ ((packed)) QMIWDS_MODIFY_PROFILE_SETTINGS_REQ_MSG, *PQMIWDS_MODIFY_PROFILE_SETTINGS_REQ_MSG;

typedef struct _QMIWDS_MODIFY_PROFILE_SETTINGS_RESP_MSG
{
   USHORT Type;
   USHORT Length;
   UCHAR  TLVType;
   USHORT TLVLength;
   USHORT QMUXResult;
   USHORT QMUXError;
} __attribute__ ((packed)) QMIWDS_MODIFY_PROFILE_SETTINGS_RESP_MSG, *PQMIWDS_MODIFY_PROFILE_SETTINGS_RESP_MSG;

typedef struct _QMIWDS_GET_PROFILE_SETTINGS_REQ_MSG
{
   USHORT Type;
   USHORT Length;
   UCHAR  TLVType;
   USHORT TLVLength;
   UCHAR  ProfileType;
   UCHAR  ProfileIndex;
} __attribute__ ((packed)) QMIWDS_GET_PROFILE_SETTINGS_REQ_MSG, *PQMIWDS_GET_PROFILE_SETTINGS_REQ_MSG;

// ======================= DMS ==============================
#define QMIDMS_SET_EVENT_REPORT_REQ           0x0001
#define QMIDMS_SET_EVENT_REPORT_RESP          0x0001
#define QMIDMS_EVENT_REPORT_IND               0x0001
#define QMIDMS_GET_DEVICE_CAP_REQ             0x0020
#define QMIDMS_GET_DEVICE_CAP_RESP            0x0020
#define QMIDMS_GET_DEVICE_MFR_REQ             0x0021
#define QMIDMS_GET_DEVICE_MFR_RESP            0x0021
#define QMIDMS_GET_DEVICE_MODEL_ID_REQ        0x0022
#define QMIDMS_GET_DEVICE_MODEL_ID_RESP       0x0022
#define QMIDMS_GET_DEVICE_REV_ID_REQ          0x0023
#define QMIDMS_GET_DEVICE_REV_ID_RESP         0x0023
#define QMIDMS_GET_MSISDN_REQ                 0x0024
#define QMIDMS_GET_MSISDN_RESP                0x0024
#define QMIDMS_GET_DEVICE_SERIAL_NUMBERS_REQ  0x0025
#define QMIDMS_GET_DEVICE_SERIAL_NUMBERS_RESP 0x0025
#define QMIDMS_UIM_SET_PIN_PROTECTION_REQ     0x0027
#define QMIDMS_UIM_SET_PIN_PROTECTION_RESP    0x0027
#define QMIDMS_UIM_VERIFY_PIN_REQ             0x0028
#define QMIDMS_UIM_VERIFY_PIN_RESP            0x0028
#define QMIDMS_UIM_UNBLOCK_PIN_REQ            0x0029
#define QMIDMS_UIM_UNBLOCK_PIN_RESP           0x0029
#define QMIDMS_UIM_CHANGE_PIN_REQ             0x002A
#define QMIDMS_UIM_CHANGE_PIN_RESP            0x002A
#define QMIDMS_UIM_GET_PIN_STATUS_REQ         0x002B
#define QMIDMS_UIM_GET_PIN_STATUS_RESP        0x002B
#define QMIDMS_GET_DEVICE_HARDWARE_REV_REQ    0x002C
#define QMIDMS_GET_DEVICE_HARDWARE_REV_RESP   0x002C
#define QMIDMS_GET_OPERATING_MODE_REQ         0x002D
#define QMIDMS_GET_OPERATING_MODE_RESP        0x002D
#define QMIDMS_SET_OPERATING_MODE_REQ         0x002E
#define QMIDMS_SET_OPERATING_MODE_RESP        0x002E
#define QMIDMS_GET_ACTIVATED_STATUS_REQ       0x0031
#define QMIDMS_GET_ACTIVATED_STATUS_RESP      0x0031
#define QMIDMS_ACTIVATE_AUTOMATIC_REQ         0x0032
#define QMIDMS_ACTIVATE_AUTOMATIC_RESP        0x0032
#define QMIDMS_ACTIVATE_MANUAL_REQ            0x0033
#define QMIDMS_ACTIVATE_MANUAL_RESP           0x0033
#define QMIDMS_UIM_GET_ICCID_REQ              0x003C
#define QMIDMS_UIM_GET_ICCID_RESP             0x003C
#define QMIDMS_UIM_GET_CK_STATUS_REQ          0x0040
#define QMIDMS_UIM_GET_CK_STATUS_RESP         0x0040
#define QMIDMS_UIM_SET_CK_PROTECTION_REQ      0x0041
#define QMIDMS_UIM_SET_CK_PROTECTION_RESP     0x0041
#define QMIDMS_UIM_UNBLOCK_CK_REQ             0x0042
#define QMIDMS_UIM_UNBLOCK_CK_RESP            0x0042
#define QMIDMS_UIM_GET_IMSI_REQ               0x0043
#define QMIDMS_UIM_GET_IMSI_RESP              0x0043
#define QMIDMS_UIM_GET_STATE_REQ              0x0044
#define QMIDMS_UIM_GET_STATE_RESP             0x0044
#define QMIDMS_GET_BAND_CAP_REQ               0x0045
#define QMIDMS_GET_BAND_CAP_RESP              0x0045

typedef struct _QMIDMS_GET_DEVICE_REV_ID_REQ_MSG
{
   USHORT Type;             // QMUX type 0x0005
   USHORT Length;
} __attribute__ ((packed)) QMIDMS_GET_DEVICE_REV_ID_REQ_MSG, *PQMIDMS_GET_DEVICE_REV_ID_REQ_MSG;

typedef struct _DEVICE_REV_ID
{
   UCHAR  TLVType;
   USHORT TLVLength;
   UCHAR  RevisionID;
} __attribute__ ((packed)) DEVICE_REV_ID, *PDEVICE_REV_ID;


typedef struct _QMIDMS_UIM_GET_IMSI_REQ_MSG
{
   USHORT Type;
   USHORT Length;
} __attribute__ ((packed)) QMIDMS_UIM_GET_IMSI_REQ_MSG, *PQMIDMS_UIM_GET_IMSI_REQ_MSG;

typedef struct _QMIDMS_UIM_GET_IMSI_RESP_MSG
{
   USHORT Type;
   USHORT Length;
   UCHAR  TLVType;
   USHORT TLVLength;
   USHORT QMUXResult;
   USHORT QMUXError;
   UCHAR  TLV2Type;
   USHORT TLV2Length;
   UCHAR IMSI;
} __attribute__ ((packed)) QMIDMS_UIM_GET_IMSI_RESP_MSG, *PQMIDMS_UIM_GET_IMSI_RESP_MSG;

typedef struct _QMIDMS_UIM_GET_STATE_REQ_MSG
{
   USHORT Type;
   USHORT Length;
} __attribute__ ((packed)) QMIDMS_UIM_GET_STATE_REQ_MSG, *PQMIDMS_UIM_GET_STATE_REQ_MSG;

typedef struct _QMIDMS_UIM_GET_STATE_RESP_MSG
{
   USHORT Type;
   USHORT Length;
   UCHAR  TLVType;
   USHORT TLVLength;
   USHORT QMUXResult;
   USHORT QMUXError;
   UCHAR  TLV2Type;
   USHORT TLV2Length;
   UCHAR  UIMState;
} __attribute__ ((packed)) QMIDMS_UIM_GET_STATE_RESP_MSG, *PQMIDMS_UIM_GET_STATE_RESP_MSG;

typedef struct _QMIDMS_UIM_GET_PIN_STATUS_REQ_MSG
{
   USHORT Type;             // QMUX type 0x0024
   USHORT Length;
} __attribute__ ((packed)) QMIDMS_UIM_GET_PIN_STATUS_REQ_MSG, *PQMIDMS_UIM_GET_PIN_STATUS_REQ_MSG;

typedef struct _QMIDMS_UIM_PIN_STATUS
{
   UCHAR  TLVType;
   USHORT TLVLength;
   UCHAR  PINStatus;
   UCHAR  PINVerifyRetriesLeft;
   UCHAR  PINUnblockRetriesLeft;
} __attribute__ ((packed)) QMIDMS_UIM_PIN_STATUS, *PQMIDMS_UIM_PIN_STATUS;

#define QMI_PIN_STATUS_NOT_INIT      0
#define QMI_PIN_STATUS_NOT_VERIF     1
#define QMI_PIN_STATUS_VERIFIED      2
#define QMI_PIN_STATUS_DISABLED      3
#define QMI_PIN_STATUS_BLOCKED       4
#define QMI_PIN_STATUS_PERM_BLOCKED  5
#define QMI_PIN_STATUS_UNBLOCKED     6
#define QMI_PIN_STATUS_CHANGED       7


typedef struct _QMIDMS_UIM_GET_PIN_STATUS_RESP_MSG
{
   USHORT Type;             // QMUX type 0x0024
   USHORT Length;
   UCHAR  TLVType;          // 0x02 - result code
   USHORT TLVLength;        // 4
   USHORT QMUXResult;       // QMI_RESULT_SUCCESS
                            // QMI_RESULT_FAILURE
   USHORT QMUXError;        // QMI_ERR_INVALID_ARG
                            // QMI_ERR_NO_MEMORY
                            // QMI_ERR_INTERNAL
                            // QMI_ERR_FAULT
   UCHAR PinStatus;
} __attribute__ ((packed)) QMIDMS_UIM_GET_PIN_STATUS_RESP_MSG, *PQMIDMS_UIM_GET_PIN_STATUS_RESP_MSG;

typedef struct _QMIDMS_UIM_VERIFY_PIN_REQ_MSG
{
   USHORT Type;             // QMUX type 0x0024
   USHORT Length;
   UCHAR  TLVType;          // 0x02 - result code
   USHORT TLVLength;        // 4
   UCHAR  PINID;
   UCHAR  PINLen;
   UCHAR  PINValue;
} __attribute__ ((packed)) QMIDMS_UIM_VERIFY_PIN_REQ_MSG, *PQMIDMS_UIM_VERIFY_PIN_REQ_MSG;

typedef struct _QMIDMS_UIM_VERIFY_PIN_RESP_MSG
{
   USHORT Type;             // QMUX type 0x0024
   USHORT Length;
   UCHAR  TLVType;          // 0x02 - result code
   USHORT TLVLength;        // 4
   USHORT QMUXResult;       // QMI_RESULT_SUCCESS
                            // QMI_RESULT_FAILURE
   USHORT QMUXError;        // QMI_ERR_INVALID_ARG
                            // QMI_ERR_NO_MEMORY
                            // QMI_ERR_INTERNAL
                            // QMI_ERR_FAULT
   UCHAR  TLV2Type;
   USHORT TLV2Length;
   UCHAR  PINVerifyRetriesLeft;
   UCHAR  PINUnblockRetriesLeft;
} __attribute__ ((packed)) QMIDMS_UIM_VERIFY_PIN_RESP_MSG, *PQMIDMS_UIM_VERIFY_PIN_RESP_MSG;


// ============================ END OF DMS ===============================

// ======================= QOS ==============================
typedef struct _MPIOC_DEV_INFO MPIOC_DEV_INFO, *PMPIOC_DEV_INFO;

#define QMI_QOS_SET_EVENT_REPORT_REQ  0x0001
#define QMI_QOS_SET_EVENT_REPORT_RESP 0x0001
#define QMI_QOS_EVENT_REPORT_IND      0x0001


// ======================= WMS ==============================
#define QMIWMS_SET_EVENT_REPORT_REQ           0x0001
#define QMIWMS_SET_EVENT_REPORT_RESP          0x0001
#define QMIWMS_EVENT_REPORT_IND               0x0001
#define QMIWMS_RAW_SEND_REQ                   0x0020
#define QMIWMS_RAW_SEND_RESP                  0x0020
#define QMIWMS_RAW_WRITE_REQ                  0x0021
#define QMIWMS_RAW_WRITE_RESP                 0x0021
#define QMIWMS_RAW_READ_REQ                   0x0022
#define QMIWMS_RAW_READ_RESP                  0x0022
#define QMIWMS_MODIFY_TAG_REQ                 0x0023
#define QMIWMS_MODIFY_TAG_RESP                0x0023
#define QMIWMS_DELETE_REQ                     0x0024
#define QMIWMS_DELETE_RESP                    0x0024
#define QMIWMS_GET_MESSAGE_PROTOCOL_REQ       0x0030
#define QMIWMS_GET_MESSAGE_PROTOCOL_RESP      0x0030
#define QMIWMS_LIST_MESSAGES_REQ              0x0031
#define QMIWMS_LIST_MESSAGES_RESP             0x0031
#define QMIWMS_GET_SMSC_ADDRESS_REQ           0x0034
#define QMIWMS_GET_SMSC_ADDRESS_RESP          0x0034
#define QMIWMS_SET_SMSC_ADDRESS_REQ           0x0035
#define QMIWMS_SET_SMSC_ADDRESS_RESP          0x0035
#define QMIWMS_GET_STORE_MAX_SIZE_REQ         0x0036
#define QMIWMS_GET_STORE_MAX_SIZE_RESP        0x0036


#define WMS_MESSAGE_PROTOCOL_CDMA             0x00
#define WMS_MESSAGE_PROTOCOL_WCDMA            0x01

// ======================= End of WMS ==============================


// ======================= NAS ==============================
#define QMINAS_SET_EVENT_REPORT_REQ             0x0002
#define QMINAS_SET_EVENT_REPORT_RESP            0x0002
#define QMINAS_EVENT_REPORT_IND                 0x0002
#define QMINAS_GET_SIGNAL_STRENGTH_REQ          0x0020
#define QMINAS_GET_SIGNAL_STRENGTH_RESP         0x0020
#define QMINAS_PERFORM_NETWORK_SCAN_REQ         0x0021
#define QMINAS_PERFORM_NETWORK_SCAN_RESP        0x0021
#define QMINAS_INITIATE_NW_REGISTER_REQ         0x0022
#define QMINAS_INITIATE_NW_REGISTER_RESP        0x0022
#define QMINAS_INITIATE_ATTACH_REQ              0x0023
#define QMINAS_INITIATE_ATTACH_RESP             0x0023
#define QMINAS_GET_SERVING_SYSTEM_REQ           0x0024
#define QMINAS_GET_SERVING_SYSTEM_RESP          0x0024
#define QMINAS_SERVING_SYSTEM_IND               0x0024
#define QMINAS_GET_HOME_NETWORK_REQ             0x0025
#define QMINAS_GET_HOME_NETWORK_RESP            0x0025
#define QMINAS_GET_PREFERRED_NETWORK_REQ        0x0026
#define QMINAS_GET_PREFERRED_NETWORK_RESP       0x0026
#define QMINAS_SET_PREFERRED_NETWORK_REQ        0x0027
#define QMINAS_SET_PREFERRED_NETWORK_RESP       0x0027
#define QMINAS_GET_FORBIDDEN_NETWORK_REQ        0x0028
#define QMINAS_GET_FORBIDDEN_NETWORK_RESP       0x0028
#define QMINAS_SET_FORBIDDEN_NETWORK_REQ        0x0029
#define QMINAS_SET_FORBIDDEN_NETWORK_RESP       0x0029
#define QMINAS_SET_TECHNOLOGY_PREF_REQ          0x002A
#define QMINAS_SET_TECHNOLOGY_PREF_RESP         0x002A
#define QMINAS_GET_RF_BAND_INFO_REQ             0x0031
#define QMINAS_GET_RF_BAND_INFO_RESP            0x0031
#define QMINAS_GET_PLMN_NAME_REQ                0x0044
#define QMINAS_GET_PLMN_NAME_RESP               0x0044
#define MEIGE_PACKET_TRANSFER_START_IND 0X100
#define MEIGE_PACKET_TRANSFER_END_IND 0X101
#define QMINAS_GET_SYS_INFO_REQ                 0x004D
#define QMINAS_GET_SYS_INFO_RESP                0x004D
#define QMINAS_SYS_INFO_IND                     0x004D

typedef struct _QMINAS_GET_HOME_NETWORK_REQ_MSG
{
   USHORT Type;             // QMUX type 0x0003
   USHORT Length;
} __attribute__ ((packed)) QMINAS_GET_HOME_NETWORK_REQ_MSG, *PQMINAS_GET_HOME_NETWORK_REQ_MSG;

typedef struct _HOME_NETWORK_SYSTEMID
{
   UCHAR  TLVType;
   USHORT TLVLength;
   USHORT SystemID;
   USHORT NetworkID;
} __attribute__ ((packed)) HOME_NETWORK_SYSTEMID, *PHOME_NETWORK_SYSTEMID;

typedef struct _HOME_NETWORK
{
   UCHAR  TLVType;
   USHORT TLVLength;
   USHORT MobileCountryCode;
   USHORT MobileNetworkCode;
   UCHAR  NetworkDesclen;
   UCHAR  NetworkDesc;
} __attribute__ ((packed)) HOME_NETWORK, *PHOME_NETWORK;


typedef struct _QMINAS_DATA_CAP
{
   UCHAR  TLVType;         // 0x01 - required parameter
   USHORT TLVLength;       // length of the mfr string
   UCHAR  DataCapListLen;
   UCHAR  DataCap;
} __attribute__ ((packed)) QMINAS_DATA_CAP, *PQMINAS_DATA_CAP;

typedef struct _QMINAS_CURRENT_PLMN_MSG
{
   UCHAR  TLVType;         // 0x01 - required parameter
   USHORT TLVLength;       // length of the mfr string
   USHORT MobileCountryCode;
   USHORT MobileNetworkCode;
   UCHAR  NetworkDesclen;
   UCHAR  NetworkDesc;
} __attribute__ ((packed)) QMINAS_CURRENT_PLMN_MSG, *PQMINAS_CURRENT_PLMN_MSG;

typedef struct _QMINAS_GET_SERVING_SYSTEM_RESP_MSG
{
   USHORT Type;
   USHORT Length;
   UCHAR  TLVType;
   USHORT TLVLength;
   USHORT QMUXResult;
   USHORT QMUXError;
} __attribute__ ((packed)) QMINAS_GET_SERVING_SYSTEM_RESP_MSG, *PQMINAS_GET_SERVING_SYSTEM_RESP_MSG;

typedef struct _SERVING_SYSTEM
{
   UCHAR  TLVType;
   USHORT TLVLength;
   UCHAR  RegistrationState;
   UCHAR  CSAttachedState;
   UCHAR  PSAttachedState;
   UCHAR  RegistredNetwork;
   UCHAR  InUseRadioIF;
   UCHAR  RadioIF;
} __attribute__ ((packed)) SERVING_SYSTEM, *PSERVING_SYSTEM;

typedef struct _QMINAS_GET_SYS_INFO_RESP_MSG
{
   USHORT Type;
   USHORT Length;
   UCHAR  TLVType;
   USHORT TLVLength;
   USHORT QMUXResult;
   USHORT QMUXError;
} __attribute__ ((packed)) QMINAS_GET_SYS_INFO_RESP_MSG, *PQMINAS_GET_SYS_INFO_RESP_MSG;

typedef struct _QMINAS_SYS_INFO_IND_MSG
{
   USHORT Type;
   USHORT Length;
} __attribute__ ((packed)) QMINAS_SYS_INFO_IND_MSG, *PQMINAS_SYS_INFO_IND_MSG;

typedef struct _SERVICE_STATUS_INFO
{  
   UCHAR  TLVType;
   USHORT TLVLength;
   UCHAR  SrvStatus;
   UCHAR  IsPrefDataPath;
} __attribute__ ((packed)) SERVICE_STATUS_INFO, *PSERVICE_STATUS_INFO;

typedef struct _CDMA_SYSTEM_INFO
{
   UCHAR  TLVType;
   USHORT TLVLength;
   UCHAR  SrvDomainValid;
   UCHAR  SrvDomain;
   UCHAR  SrvCapabilityValid;
   UCHAR  SrvCapability;
   UCHAR  RoamStatusValid;
   UCHAR  RoamStatus;
   UCHAR  IsSysForbiddenValid;
   UCHAR  IsSysForbidden;
   UCHAR  IsSysPrlMatchValid;
   UCHAR  IsSysPrlMatch;
   UCHAR  PRevInUseValid;
   UCHAR  PRevInUse;
   UCHAR  BSPRevValid;
   UCHAR  BSPRev;
   UCHAR  CCSSupportedValid;
   UCHAR  CCSSupported;
   UCHAR  CDMASysIdValid;
   USHORT SID;
   USHORT NID;
   UCHAR  BSInfoValid;
   USHORT BaseID;
   ULONG  BaseLAT;
   ULONG  BaseLONG;
   UCHAR  PacketZoneValid;
   USHORT PacketZone;
   UCHAR  NetworkIdValid;
   UCHAR  MCC[3];
   UCHAR  MNC[3];
} __attribute__ ((packed)) CDMA_SYSTEM_INFO, *PCDMA_SYSTEM_INFO;

typedef struct _HDR_SYSTEM_INFO
{
   UCHAR  TLVType;
   USHORT TLVLength;
   UCHAR  SrvDomainValid;
   UCHAR  SrvDomain;
   UCHAR  SrvCapabilityValid;
   UCHAR  SrvCapability;
   UCHAR  RoamStatusValid;
   UCHAR  RoamStatus;
   UCHAR  IsSysForbiddenValid;
   UCHAR  IsSysForbidden;
   UCHAR  IsSysPrlMatchValid;
   UCHAR  IsSysPrlMatch;
   UCHAR  HdrPersonalityValid;
   UCHAR  HdrPersonality;
   UCHAR  HdrActiveProtValid;
   UCHAR  HdrActiveProt;
   UCHAR  is856SysIdValid;
   UCHAR  is856SysId[16];
} __attribute__ ((packed)) HDR_SYSTEM_INFO, *PHDR_SYSTEM_INFO;

typedef struct _GSM_SYSTEM_INFO
{
   UCHAR  TLVType;
   USHORT TLVLength;
   UCHAR  SrvDomainValid;
   UCHAR  SrvDomain;
   UCHAR  SrvCapabilityValid;
   UCHAR  SrvCapability;
   UCHAR  RoamStatusValid;
   UCHAR  RoamStatus;
   UCHAR  IsSysForbiddenValid;
   UCHAR  IsSysForbidden;
   UCHAR  LacValid;
   USHORT Lac;
   UCHAR  CellIdValid;
   ULONG  CellId;
   UCHAR  RegRejectInfoValid;
   UCHAR  RejectSrvDomain;
   UCHAR  RejCause;
   UCHAR  NetworkIdValid;
   UCHAR  MCC[3];
   UCHAR  MNC[3];
   UCHAR  EgprsSuppValid;
   UCHAR  EgprsSupp;
   UCHAR  DtmSuppValid;
   UCHAR  DtmSupp;
} __attribute__ ((packed)) GSM_SYSTEM_INFO, *PGSM_SYSTEM_INFO;

typedef struct _WCDMA_SYSTEM_INFO
{
   UCHAR  TLVType;
   USHORT TLVLength;
   UCHAR  SrvDomainValid;
   UCHAR  SrvDomain;
   UCHAR  SrvCapabilityValid;
   UCHAR  SrvCapability;
   UCHAR  RoamStatusValid;
   UCHAR  RoamStatus;
   UCHAR  IsSysForbiddenValid;
   UCHAR  IsSysForbidden;
   UCHAR  LacValid;
   USHORT Lac;
   UCHAR  CellIdValid;
   ULONG  CellId;
   UCHAR  RegRejectInfoValid;
   UCHAR  RejectSrvDomain;
   UCHAR  RejCause;
   UCHAR  NetworkIdValid;
   UCHAR  MCC[3];
   UCHAR  MNC[3];
   UCHAR  HsCallStatusValid;
   UCHAR  HsCallStatus;
   UCHAR  HsIndValid;
   UCHAR  HsInd;
   UCHAR  PscValid;
   UCHAR  Psc;
} __attribute__ ((packed)) WCDMA_SYSTEM_INFO, *PWCDMA_SYSTEM_INFO;

typedef struct _LTE_SYSTEM_INFO
{
   UCHAR  TLVType;
   USHORT TLVLength;
   UCHAR  SrvDomainValid;
   UCHAR  SrvDomain;
   UCHAR  SrvCapabilityValid;
   UCHAR  SrvCapability;
   UCHAR  RoamStatusValid;
   UCHAR  RoamStatus;
   UCHAR  IsSysForbiddenValid;
   UCHAR  IsSysForbidden;
   UCHAR  LacValid;
   USHORT Lac;
   UCHAR  CellIdValid;
   ULONG  CellId;
   UCHAR  RegRejectInfoValid;
   UCHAR  RejectSrvDomain;
   UCHAR  RejCause;
   UCHAR  NetworkIdValid;
   UCHAR  MCC[3];
   UCHAR  MNC[3];
   UCHAR  TacValid;
   USHORT Tac;
} __attribute__ ((packed)) LTE_SYSTEM_INFO, *PLTE_SYSTEM_INFO;

typedef struct _TDSCDMA_SYSTEM_INFO
{
   UCHAR  TLVType;
   USHORT TLVLength;
   UCHAR  SrvDomainValid;
   UCHAR  SrvDomain;
   UCHAR  SrvCapabilityValid;
   UCHAR  SrvCapability;
   UCHAR  RoamStatusValid;
   UCHAR  RoamStatus;
   UCHAR  IsSysForbiddenValid;
   UCHAR  IsSysForbidden;
   UCHAR  LacValid;
   USHORT Lac;
   UCHAR  CellIdValid;
   ULONG  CellId;
   UCHAR  RegRejectInfoValid;
   UCHAR  RejectSrvDomain;
   UCHAR  RejCause;
   UCHAR  NetworkIdValid;
   UCHAR  MCC[3];
   UCHAR  MNC[3];
   UCHAR  HsCallStatusValid;
   UCHAR  HsCallStatus;
   UCHAR  HsIndValid;
   UCHAR  HsInd;
   UCHAR  CellParameterIdValid;
   USHORT CellParameterId;
   UCHAR  CellBroadcastCapValid;
   ULONG  CellBroadcastCap;
   UCHAR  CsBarStatusValid;
   ULONG  CsBarStatus;
   UCHAR  PsBarStatusValid;
   ULONG  PsBarStatus;
   UCHAR  CipherDomainValid;
   UCHAR  CipherDomain;
} __attribute__ ((packed)) TDSCDMA_SYSTEM_INFO, *PTDSCDMA_SYSTEM_INFO;

// ======================= End of NAS ==============================

// ======================= UIM ==============================
#define QMIUIM_READ_TRANSPARENT_REQ      0x0020
#define QMIUIM_READ_TRANSPARENT_RESP     0x0020
#define QMIUIM_READ_TRANSPARENT_IND      0x0020
#define QMIUIM_READ_RECORD_REQ           0x0021
#define QMIUIM_READ_RECORD_RESP          0x0021
#define QMIUIM_READ_RECORD_IND           0x0021
#define QMIUIM_WRITE_TRANSPARENT_REQ     0x0022
#define QMIUIM_WRITE_TRANSPARENT_RESP    0x0022
#define QMIUIM_WRITE_TRANSPARENT_IND     0x0022
#define QMIUIM_WRITE_RECORD_REQ          0x0023
#define QMIUIM_WRITE_RECORD_RESP         0x0023
#define QMIUIM_WRITE_RECORD_IND          0x0023
#define QMIUIM_SET_PIN_PROTECTION_REQ    0x0025
#define QMIUIM_SET_PIN_PROTECTION_RESP   0x0025
#define QMIUIM_SET_PIN_PROTECTION_IND    0x0025
#define QMIUIM_VERIFY_PIN_REQ            0x0026
#define QMIUIM_VERIFY_PIN_RESP           0x0026
#define QMIUIM_VERIFY_PIN_IND            0x0026
#define QMIUIM_UNBLOCK_PIN_REQ           0x0027
#define QMIUIM_UNBLOCK_PIN_RESP          0x0027
#define QMIUIM_UNBLOCK_PIN_IND           0x0027
#define QMIUIM_CHANGE_PIN_REQ            0x0028
#define QMIUIM_CHANGE_PIN_RESP           0x0028
#define QMIUIM_CHANGE_PIN_IND            0x0028
#define QMIUIM_DEPERSONALIZATION_REQ     0x0029
#define QMIUIM_DEPERSONALIZATION_RESP    0x0029
#define QMIUIM_EVENT_REG_REQ             0x002E
#define QMIUIM_EVENT_REG_RESP            0x002E
#define QMIUIM_GET_CARD_STATUS_REQ       0x002F
#define QMIUIM_GET_CARD_STATUS_RESP      0x002F
#define QMIUIM_STATUS_CHANGE_IND         0x0032


typedef struct _QMIUIM_GET_CARD_STATUS_RESP_MSG
{
   USHORT Type;             
   USHORT Length;
   UCHAR  TLVType;          
   USHORT TLVLength;        
   USHORT QMUXResult;       
   USHORT QMUXError;        
} __attribute__ ((packed)) QMIUIM_GET_CARD_STATUS_RESP_MSG, *PQMIUIM_GET_CARD_STATUS_RESP_MSG;

typedef struct _QMIUIM_CARD_STATUS
{
   UCHAR  TLVType;
   USHORT TLVLength;
   USHORT IndexGWPri;
   USHORT Index1XPri;
   USHORT IndexGWSec;
   USHORT Index1XSec;
   UCHAR  NumSlot;
   UCHAR  CardState;
   UCHAR  UPINState;
   UCHAR  UPINRetries;
   UCHAR  UPUKRetries;
   UCHAR  ErrorCode;
   UCHAR  NumApp;
   UCHAR  AppType;
   UCHAR  AppState;
   UCHAR  PersoState;
   UCHAR  PersoFeature;
   UCHAR  PersoRetries;
   UCHAR  PersoUnblockRetries;
   UCHAR  AIDLength;
} __attribute__ ((packed)) QMIUIM_CARD_STATUS, *PQMIUIM_CARD_STATUS;

typedef struct _QMIUIM_PIN_STATE
{
   UCHAR  UnivPIN;
   UCHAR  PIN1State;
   UCHAR  PIN1Retries;
   UCHAR  PUK1Retries;
   UCHAR  PIN2State;
   UCHAR  PIN2Retries;
   UCHAR  PUK2Retries;
} __attribute__ ((packed)) QMIUIM_PIN_STATE, *PQMIUIM_PIN_STATE;

typedef struct _QMIUIM_VERIFY_PIN_REQ_MSG
{
   USHORT Type;     
   USHORT Length;
   UCHAR  TLVType;  
   USHORT TLVLength;
   UCHAR  Session_Type;
   UCHAR  Aid_Len;
   UCHAR  TLV2Type;  
   USHORT TLV2Length;
   UCHAR  PINID;
   UCHAR  PINLen;
   UCHAR  PINValue;
} __attribute__ ((packed)) QMIUIM_VERIFY_PIN_REQ_MSG, *PQMIUIM_VERIFY_PIN_REQ_MSG;

typedef struct _QMIUIM_VERIFY_PIN_RESP_MSG
{
   USHORT Type;     
   USHORT Length;
   UCHAR  TLVType;  
   USHORT TLVLength;
   USHORT QMUXResult;
   USHORT QMUXError;
   UCHAR  TLV2Type; 
   USHORT TLV2Length; 
   UCHAR  PINVerifyRetriesLeft;
   UCHAR  PINUnblockRetriesLeft;
} __attribute__ ((packed)) QMIUIM_VERIFY_PIN_RESP_MSG, *PQMIUIM_VERIFY_PIN_RESP_MSG;


typedef struct _QMUX_MSG
{
   QCQMUX_HDR QMUXHdr;
   union
   {
      // Message Header
      QCQMUX_MSG_HDR                           QMUXMsgHdr;
      QCQMUX_MSG_HDR_RESP                      QMUXMsgHdrResp;

      //#ifdef QC_IP_MODE
      QMIWDS_GET_RUNTIME_SETTINGS_REQ_MSG       GetRuntimeSettingsReq;
      QMIWDS_GET_RUNTIME_SETTINGS_RESP_MSG      GetRuntimeSettingsRsp;
      //#endif // QC_IP_MODE

      QMIWDS_START_NETWORK_INTERFACE_REQ_MSG    StartNwInterfaceReq;
      QMIWDS_START_NETWORK_INTERFACE_RESP_MSG   StartNwInterfaceResp;
      QMIWDS_STOP_NETWORK_INTERFACE_REQ_MSG     StopNwInterfaceReq;
      QMIWDS_STOP_NETWORK_INTERFACE_RESP_MSG    StopNwInterfaceResp;
      QMIWDS_GET_DEFAULT_SETTINGS_REQ_MSG       GetDefaultSettingsReq;
      QMIWDS_GET_DEFAULT_SETTINGS_RESP_MSG      GetDefaultSettingsResp;
      QMIWDS_MODIFY_PROFILE_SETTINGS_REQ_MSG    ModifyProfileSettingsReq;
      QMIWDS_MODIFY_PROFILE_SETTINGS_RESP_MSG   ModifyProfileSettingsResp;
      QMIWDS_GET_PROFILE_SETTINGS_REQ_MSG    GetProfileSettingsReq;

      QMIDMS_UIM_GET_PIN_STATUS_REQ_MSG         UIMGetPinStatusReq;
      QMIDMS_UIM_GET_PIN_STATUS_RESP_MSG        UIMGetPinStatusResp;
      QMIDMS_UIM_VERIFY_PIN_REQ_MSG             UIMVerifyPinReq;
      QMIDMS_UIM_VERIFY_PIN_RESP_MSG            UIMVerifyPinResp;
      QMIDMS_UIM_GET_STATE_REQ_MSG              UIMGetStateReq;
      QMIDMS_UIM_GET_STATE_RESP_MSG             UIMGetStateResp;
      QMIDMS_UIM_GET_IMSI_REQ_MSG               UIMGetIMSIReq;
      QMIDMS_UIM_GET_IMSI_RESP_MSG              UIMGetIMSIResp;

      QMINAS_GET_SERVING_SYSTEM_RESP_MSG        GetServingSystemResp;
      QMINAS_GET_SYS_INFO_RESP_MSG              GetSysInfoResp;
      QMINAS_SYS_INFO_IND_MSG                   NasSysInfoInd;

      // QMIUIM Messages
      QMIUIM_GET_CARD_STATUS_RESP_MSG           UIMGetCardStatus;
      QMIUIM_VERIFY_PIN_REQ_MSG                 UIMUIMVerifyPinReq;
      QMIUIM_VERIFY_PIN_RESP_MSG                UIMUIMVerifyPinResp;

   };
} __attribute__ ((packed)) QMUX_MSG, *PQMUX_MSG;

#pragma pack(pop)

#endif // MPQMUX_H
