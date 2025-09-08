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

// ======================= NAS ==============================
#define QMINAS_SET_EVENT_REPORT_REQ             0x0002
#define QMINAS_SET_EVENT_REPORT_RESP            0x0002
#define QMINAS_EVENT_REPORT_IND                 0x0002

//Add by Demon. 
#define QMINAS_INDICATION_REGISTER_REQ          0x0003
#define QMINAS_INDICATION_REGISTER_RESP         0x0003
#define QMINAS_NR5G_TIME_SYNC_PULSE_REPORT_IND  0x0120
#define QMINAS_NR5G_LOST_FRAME_SYNC_IND         0x0121
#define QMINAS_SET_NR5G_SYNC_PULSE_GEN_REQ      0x0122
#define QMINAS_SET_NR5G_SYNC_PULSE_GEN_RESP     0x0122
#define QMINAS_NR5G_RESOURCE_CONFIG_IND         0x0131
//End add by Demon.

//Add by Remy
#define QMI_UIM_HTTP_TRANSACTION_REQ            0x0021
#define QMI_UIM_HTTP_TRANSACTION_RESP           0x0021
#define QMI_UIM_HTTP_TRANSACTION_IND            0x0021
#define QMI_UIM_ADD_PROFILE_REQ                 0x0068
#define QMI_UIM_ADD_PROFILE_RESP                0x0068
#define QMI_UIM_ADD_PROFILE_IND                 0x0068
//End add by Remy

#define QMI_UIM_DELETE_PROFILE_REQ 0x0066
#define QMI_UIM_DELETE_PROFILE_RESP 0x0066

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
#define QUECTEL_PACKET_TRANSFER_START_IND       0X100
#define QUECTEL_PACKET_TRANSFER_END_IND         0X101
#define QMINAS_GET_SYS_INFO_REQ                 0x004D
#define QMINAS_GET_SYS_INFO_RESP                0x004D
#define QMINAS_SYS_INFO_IND                     0x004D
#define QMINAS_GET_SIG_INFO_REQ                 0x004F
#define QMINAS_GET_SIG_INFO_RESP                0x004F

//Add by Demon.
typedef struct _QMINAS_INDICATION_REGISTER_REQ_MSG
{
   USHORT Type;             // 0x003
   USHORT Length;
} __attribute__ ((packed)) QMINAS_INDICATION_REGISTER_REQ_MSG, *PQMINAS_INDICATION_REGISTER_REQ_MSG;

//Controls the reporting of QMI_NAS_NR5G_TIME_SYNC_PULSE_REPORT_IND
typedef struct _QMINAS_REG_NR5G_PULSE_IND_REQ_MSG
{
   UCHAR  TLVType;             // 0x4F
   USHORT TLVLength;
   UCHAR  RegNR5GPulseInd;    //0x00-Disable; 0x-1-Enable
} __attribute__ ((packed)) QMINAS_REG_NR5G_PULSE_IND_REQ_MSG, *PQMINAS_REG_NR5G_PULSE_IND_REQ_MSG;

//Controls the reporting of QMI_NAS_NR5G_LOST_FRAME_SYNC_IND.
typedef struct _QMINAS_REG_NR5G_LOST_FRAME_SYNC_IND_REQ_MSG
{
   UCHAR  TLVType;             // 0x50
   USHORT TLVLength;
   UCHAR  RegNR5GLostSync;     //0x00-Disable; 0x-1-Enable
 
} __attribute__ ((packed)) QMINAS_REG_NR5G_LOST_FRAME_SYNC_IND_REQ_MSG, *PQMINAS_REG_NR5G_LOST_FRAME_SYNC_IND_REQ_MSG;

//Controls the reporting of QMI_NAS_NR5G_RESOURCE_CONFIG_IND.
typedef struct _QMINAS_REG_NR5G_RSRC_CFG_IND_REQ_MSG
{
   UCHAR  TLVType;             // 0x53
   USHORT TLVLength;
   UCHAR  RegNR5GRsrcCfg;     //0x00-Disable; 0x-1-Enable
} __attribute__ ((packed)) QMINAS_REG_NR5G_RSRC_CFG_IND_REQ_MSG, *PQMINAS_REG_NR5G_RSRC_CFG_IND_REQ_MSG;

typedef struct _NR5G_PULSE_GPS_TIME
{
   UCHAR   TLVType;             // 0x10
   USHORT  TLVLength;
   ULONG64 gps_time;  
} __attribute__ ((packed)) NR5G_PULSE_GPS_TIME, *PNR5G_PULSE_GPS_TIME;

typedef struct _NR5G_PULSE_SFN
{
   UCHAR  TLVType;             // 0x11
   USHORT TLVLength;
   UINT   sfn;   
} __attribute__ ((packed)) NR5G_PULSE_SFN, *PNR5G_PULSE_SFN;

typedef struct _NR5G_PULSE_NTA
{
   UCHAR  TLVType;             // 0x12
   USHORT TLVLength;
   int    nta;   
} __attribute__ ((packed)) NR5G_PULSE_NTA, *PNR5G_PULSE_NTA;

typedef struct _NR5G_PULSE_NTA_OFFSET
{
   UCHAR  TLVType;             // 0x13
   USHORT TLVLength;
   UINT   nta_offset;  
} __attribute__ ((packed)) NR5G_PULSE_NTA_OFFSET, *PNR5G_PULSE_NTA_OFFSET;

typedef struct _NR5G_PULSE_NTA_LEAPSECONDS
{
   UCHAR  TLVType;             // 0x13
   USHORT TLVLength;
   UCHAR  leapseconds;   
} __attribute__ ((packed)) NR5G_PULSE_NTA_LEAPSECONDS, *PNR5G_PULSE_NTA_LEAPSECONDS;

typedef struct _QMINAS_NR5G_PULSE_REPORT_IND_MSG
{
   USHORT Type;             // 0x120
   USHORT Length;  

   NR5G_PULSE_GPS_TIME          pulseGPSTime;
   NR5G_PULSE_SFN               pulseSFN;
   NR5G_PULSE_NTA               pulseNTA;
   NR5G_PULSE_NTA_OFFSET        pulseNTAOffset;
   NR5G_PULSE_NTA_LEAPSECONDS   pulseLeapSeconds;
} __attribute__ ((packed)) QMINAS_NR5G_PULSE_REPORT_IND_MSG, *PQMINAS_NR5G_PULSE_REPORT_IND_MSG;

//QMI_NAS_SET_NR5G_SYNC_PULSE_GEN_REQ_MSG
// typedef struct _SYNC_PULSE_GEN_PULSE_PERIOD
// {
//    UCHAR  TLVType;             // 0x01, Mandatory TLVs
//    USHORT TLVLength;
//    UINT   pulsePeriod;
// } __attribute__ ((packed)) SYNC_PULSE_GEN_PULSE_PERIOD, *PSYNC_PULSE_GEN_PULSE_PERIOD;

typedef struct
{
   UCHAR  TLVType;             // 0x01
   USHORT TLVLength;
   UINT   result;
} __attribute__ ((packed)) UIM_HTTP_TRANSACTION_RESULT, *PUIM_HTTP_TRANSACTION_RESULT;

typedef struct
{
   UCHAR  TLVType;             // 0x02
   USHORT TLVLength;
   UINT   tokenId;
} __attribute__ ((packed)) UIM_HTTP_TRANSACTION_TOKEN_ID, *PUIM_HTTP_TRANSACTION_TOKEN_ID;

typedef struct
{
   UCHAR  TLVType;             // 0x10
   USHORT TLVLength;
   UCHAR  url[512+1];
} __attribute__ ((packed)) UIM_HTTP_TRANSACTION_URL, *PUIM_HTTP_TRANSACTION_URL;

typedef struct
{
   UCHAR  TLVType;             // 0x11
   USHORT TLVLength;
   // uint16_t contentTypeLen;
   // UCHAR  contenType[256+1];
   uint8_t customHeaderLen;
   struct {
      uint8_t nameLen;
      UCHAR name[32+1];
      uint16_t valueLen;
      UCHAR value[256+1];
   }customHeader[3];
} __attribute__ ((packed)) UIM_HTTP_TRANSACTION_HEADER, *PUIM_HTTP_TRANSACTION_HEADER;

typedef struct
{
   UCHAR  TLVType;             // 0x12
   USHORT TLVLength;
   uint32_t totalSize;
   uint32_t offset;
} __attribute__ ((packed)) UIM_HTTP_TRANSACTION_SEGMENT, *PUIM_HTTP_TRANSACTION_SEGMENT;

typedef struct
{
   UCHAR  TLVType;             // 0x13
   USHORT TLVLength;
   uint32_t payloadBodyLen;
   uint8_t payloadBody[2048];
} __attribute__ ((packed)) UIM_HTTP_TRANSACTION_PAYLOAD_BODY, *PUIM_HTTP_TRANSACTION_PAYLOAD_BODY;

typedef struct _SYNC_PULSE_GEN_START_SFN
{
   UCHAR  TLVType;             // 0x10
   USHORT TLVLength;
   UINT   startSFN;
} __attribute__ ((packed)) SYNC_PULSE_GEN_START_SFN, *PSYNC_PULSE_GEN_START_SFN;

typedef struct _SYNC_PULSE_GEN_REPORT_PERIOD
{
   UCHAR  TLVType;             // 0x11
   USHORT TLVLength;
   UINT   reportPeriod;
} __attribute__ ((packed)) SYNC_PULSE_GEN_REPORT_PERIOD, *PSYNC_PULSE_GEN_REPORT_PERIOD;

typedef struct _QMINAS_SET_NR5G_SYNC_PULSE_GEN_REQ_MSG
{
    USHORT Type;             // 0x0122
    USHORT Length;

    UCHAR  TLVType;          // 0x01, Mandatory TLVs
    USHORT TLVLength;
    UINT   pulsePeriod;
} __attribute__ ((packed)) QMINAS_SET_NR5G_SYNC_PULSE_GEN_REQ_MSG, *PQMINAS_SET_NR5G_SYNC_PULSE_GEN_REQ_MSG;

typedef struct {
   USHORT Type;             // 0x0021
   USHORT Length;

   UIM_HTTP_TRANSACTION_RESULT result;
   UIM_HTTP_TRANSACTION_TOKEN_ID tokenId;
   UIM_HTTP_TRANSACTION_HEADER headers;
   UIM_HTTP_TRANSACTION_SEGMENT segmentInfo;
   UIM_HTTP_TRANSACTION_PAYLOAD_BODY payload;
}__attribute__ ((packed)) QMIUIM_HTTP_TRANSACTION_REQ_MSG, *PQMIUIM_HTTP_TRANSACTION_REQ_MSG;

typedef struct {
   USHORT Type;             // 0x0021
   USHORT Length;

   UIM_HTTP_TRANSACTION_TOKEN_ID tokenId;
   UIM_HTTP_TRANSACTION_URL url;
   UIM_HTTP_TRANSACTION_HEADER header;
   UIM_HTTP_TRANSACTION_SEGMENT segment;
   UIM_HTTP_TRANSACTION_PAYLOAD_BODY payload;
}__attribute__ ((packed)) QMIUIM_HTTP_TRANSACTION_IND_MSG, *PQMIUIM_HTTP_TRANSACTION_IND_MSG;

typedef struct {
   UCHAR  TLVType;             // 0x01
   USHORT TLVLength;
   UCHAR  slot;
}__attribute__ ((packed)) UIM_ADD_PROFILE_SLOT, *PUIM_ADD_PROFILE_SLOT;

typedef struct {
   UCHAR  TLVType;             // 0x01
   USHORT TLVLength;
   USHORT activeCodeLen;
   USHORT activeCode[256];
}__attribute__ ((packed)) UIM_ADD_PROFILE_ACTIVE_CODE, *PUIM_ADD_PROFILE_ACTIVE_CODE;

typedef struct {
   UCHAR  TLVType;             // 0x01
   USHORT TLVLength;
   USHORT confirmationCodeLen;
   USHORT confirmationCode[256];
}__attribute__ ((packed)) UIM_ADD_PROFILE_CONFIRMATION_CODE, *PUIM_ADD_PROFILE_CONFIRMATION_CODE;

typedef struct {
   USHORT Type;             // 0x0068
   USHORT Length;

   UIM_ADD_PROFILE_SLOT slot;
   UIM_ADD_PROFILE_ACTIVE_CODE activeCode;
   UIM_ADD_PROFILE_CONFIRMATION_CODE confirmationCode;
}__attribute__ ((packed)) QMIUIM_ADD_PROFILE_REQ_MSG, *PQMIUIM_ADD_PROFILE_REQ_MSG;

typedef struct {
   USHORT Type;             // 0x0068
   USHORT Length;
}__attribute__ ((packed)) QMIUIM_ADD_PROFILE_IND_MSG, *PQMIUIM_ADD_PROFILE_IND_MSG;

//End add by Demon

typedef struct {
   UCHAR  TLVType;             // 0x01
   USHORT TLVLength;
   UCHAR  slot;
}__attribute__ ((packed)) UIM_DELETE_PROFILE_SLOT, *PUIM_DELETE_PROFILE_SLOT;

typedef struct {
   UCHAR  TLVType;             // 0x02
   USHORT TLVLength;
   UINT   profile_id;
}__attribute__ ((packed)) UIM_DELETE_PROFILE_ID, *PUIM_DELETE_PROFILE_ID;

typedef struct {
   USHORT Type;             // 0x0068
   USHORT Length;

   UIM_DELETE_PROFILE_SLOT slot;
   UIM_DELETE_PROFILE_ID   profile_id;
}__attribute__ ((packed)) QMIUIM_DELETE_PROFILE_REQ_MSG, *PQMIUIM_DELETE_PROFILE_REQ_MSG;

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
   UCHAR  true_srv_status;
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

typedef enum {
  NAS_SYS_SRV_STATUS_NO_SRV_V01 = 0, 
  NAS_SYS_SRV_STATUS_LIMITED_V01 = 1, 
  NAS_SYS_SRV_STATUS_SRV_V01 = 2, 
  NAS_SYS_SRV_STATUS_LIMITED_REGIONAL_V01 = 3, 
  NAS_SYS_SRV_STATUS_PWR_SAVE_V01 = 4, 
}nas_service_status_enum_type_v01;

typedef enum {
  SYS_SRV_DOMAIN_NO_SRV_V01 = 0, 
  SYS_SRV_DOMAIN_CS_ONLY_V01 = 1, 
  SYS_SRV_DOMAIN_PS_ONLY_V01 = 2, 
  SYS_SRV_DOMAIN_CS_PS_V01 = 3, 
  SYS_SRV_DOMAIN_CAMPED_V01 = 4, 
}nas_service_domain_enum_type_v01;

typedef struct {
  UCHAR  TLVType;
  USHORT TLVLength;

  uint8_t srv_domain_valid;
  uint8_t srv_domain;
  uint8_t srv_capability_valid;
  uint8_t srv_capability;
  uint8_t roam_status_valid;
  uint8_t roam_status;
  uint8_t is_sys_forbidden_valid;
  uint8_t is_sys_forbidden;

  uint8_t lac_valid;
  uint16_t lac;
  uint8_t cell_id_valid;
  uint32_t cell_id;
  uint8_t reg_reject_info_valid;
  uint8_t reject_srv_domain;
  uint8_t rej_cause;
  uint8_t network_id_valid;
  UCHAR MCC[3];
  UCHAR MNC[3];

  uint8_t tac_valid;
  uint16_t tac;
} __attribute__ ((packed)) NR5G_SYSTEM_INFO, *PNR5G_SYSTEM_INFO;


typedef struct {
    UCHAR  TLVType;
    USHORT TLVLength;
    CHAR rssi;
    SHORT ecio;
} __attribute__ ((packed)) QMINAS_SIG_INFO_CDMA_TLV_MSG, *PQMINAS_SIG_INFO_CDMA_TLV_MSG;

typedef struct {
    UCHAR  TLVType;
    USHORT TLVLength;
    CHAR rssi;
    SHORT ecio;
    CHAR sinr;
    INT io;
} __attribute__ ((packed)) QMINAS_SIG_INFO_HDR_TLV_MSG, *PQMINAS_SIG_INFO_HDR_TLV_MSG;

typedef struct {
    UCHAR  TLVType;
    USHORT TLVLength;
    CHAR rssi;
} __attribute__ ((packed)) QMINAS_SIG_INFO_GSM_TLV_MSG, *PQMINAS_SIG_INFO_GSM_TLV_MSG;

typedef struct {
    UCHAR  TLVType;
    USHORT TLVLength;
    CHAR rssi;
    SHORT ecio;
} __attribute__ ((packed)) QMINAS_SIG_INFO_WCDMA_TLV_MSG, *PQMINAS_SIG_INFO_WCDMA_TLV_MSG;

typedef struct {
    UCHAR  TLVType;
    USHORT TLVLength;
    CHAR rssi;
    CHAR rsrq;
    SHORT rsrp;
    SHORT snr;
} __attribute__ ((packed)) QMINAS_SIG_INFO_LTE_TLV_MSG, *PQMINAS_SIG_INFO_LTE_TLV_MSG;

typedef struct {
    UCHAR  TLVType;
    USHORT TLVLength;
    CHAR rscp;
} __attribute__ ((packed)) QMINAS_SIG_INFO_TDSCDMA_TLV_MSG, *PQMINAS_SIG_INFO_TDSCDMA_TLV_MSG;

typedef struct {
    UCHAR  TLVType;
    USHORT TLVLength;
    SHORT rsrp;
    SHORT snr;
} __attribute__ ((packed)) QMINAS_SIG_INFO_5G_NSA_TLV_MSG, *PQMINAS_SIG_INFO_5G_NSA_TLV_MSG;

typedef struct {
    UCHAR  TLVType;
    USHORT TLVLength;
    SHORT nr5g_rsrq;
} __attribute__ ((packed)) QMINAS_SIG_INFO_5G_SA_TLV_MSG, *PQMINAS_SIG_INFO_5G_SA_TLV_MSG;
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

#define UIM_CARD_STATE_ABSENT     0x00
#define UIM_CARD_STATE_PRESENT    0x01
#define UIM_CARD_STATE_ERROR      0x02

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

typedef struct _QMIUIM_READ_TRANSPARENT_REQ_MSG
{
   USHORT Type;
   USHORT Length;
   UCHAR  TLVType;
   USHORT TLVLength;
   UCHAR  Session_Type;
   UCHAR  Aid_Len;
   UCHAR  TLV2Type;
   USHORT TLV2Length;
   USHORT file_id;
   UCHAR  path_len;
   UCHAR  path[];
} __attribute__ ((packed)) QMIUIM_READ_TRANSPARENT_REQ_MSG, *PQMIUIM_READ_TRANSPARENT_REQ_MSG;

typedef struct _READ_TRANSPARENT_TLV
{
   UCHAR  TLVType;
   USHORT TLVLength;
   USHORT Offset;
   USHORT Length;
} __attribute__ ((packed)) READ_TRANSPARENT_TLV, *PREAD_TRANSPARENT_TLV;

typedef struct _QMIUIM_CONTENT
{
   UCHAR  TLVType;
   USHORT TLVLength;
   USHORT content_len;
   UCHAR  content[];
} __attribute__ ((packed)) QMIUIM_CONTENT, *PQMIUIM_CONTENT;

typedef struct _QMIUIM_READ_TRANSPARENT_RESP_MSG
{
   USHORT Type;
   USHORT Length;
   UCHAR  TLVType;
   USHORT TLVLength;
   USHORT QMUXResult;
   USHORT QMUXError;
} __attribute__ ((packed)) QMIUIM_READ_TRANSPARENT_RESP_MSG, *PQMIUIM_READ_TRANSPARENT_RESP_MSG;

typedef struct _QMUX_MSG
{
   QCQMUX_HDR QMUXHdr;
   union
   {
      // Message Header
      QCQMUX_MSG_HDR                           QMUXMsgHdr;
      QCQMUX_MSG_HDR_RESP                      QMUXMsgHdrResp;

      QMINAS_GET_SERVING_SYSTEM_RESP_MSG        GetServingSystemResp;
      QMINAS_GET_SYS_INFO_RESP_MSG              GetSysInfoResp;
      QMINAS_SYS_INFO_IND_MSG                   NasSysInfoInd;
      QMINAS_REG_NR5G_PULSE_IND_REQ_MSG         SetNR5GPulseInd;
      QMINAS_INDICATION_REGISTER_REQ_MSG        ReqRegisterInd;
      QMINAS_SET_NR5G_SYNC_PULSE_GEN_REQ_MSG    SetSyncPulseGen;
      QMIUIM_HTTP_TRANSACTION_REQ_MSG           UimHttpTransactionReq;
      QMIUIM_HTTP_TRANSACTION_IND_MSG           UimHttpTransactionInd;
      QMIUIM_ADD_PROFILE_REQ_MSG                UimAddProfileReq;
      QMIUIM_ADD_PROFILE_IND_MSG                UimAddProfileInd;
      QMIUIM_DELETE_PROFILE_REQ_MSG             UimDeleteProfileReq;

      // QMIUIM Messages
      QMIUIM_GET_CARD_STATUS_RESP_MSG           UIMGetCardStatus;
      QMIUIM_VERIFY_PIN_REQ_MSG                 UIMUIMVerifyPinReq;
      QMIUIM_VERIFY_PIN_RESP_MSG                UIMUIMVerifyPinResp;  
   };
} __attribute__ ((packed)) QMUX_MSG, *PQMUX_MSG;

#pragma pack(pop)

#endif // MPQMUX_H
