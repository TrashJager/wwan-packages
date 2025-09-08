/******************************************************************************
  @file    MPQMUX.c
  @brief   QMI mux.

  DESCRIPTION
  Connectivity Management Tool for USB network adapter of Quectel wireless cellular modules.

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

  ---------------------------------------------------------------------------
  Copyright (c) 2016 - 2020 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/

#include "QMIThread.h"
static char line[1024];
static pthread_mutex_t dumpQMIMutex = PTHREAD_MUTEX_INITIALIZER;
#undef dbg
#define dbg( format, arg... ) do {if (strlen(line) < sizeof(line)) snprintf(&line[strlen(line)], sizeof(line) - strlen(line), format, ## arg);} while (0)

PQMI_TLV_HDR GetTLV (PQCQMUX_MSG_HDR pQMUXMsgHdr, int TLVType);

typedef struct {
    UINT type;
    const char *name;
} QMI_NAME_T;

#define qmi_name_item(type) {type, #type}

#if 0
static const QMI_NAME_T qmi_IFType[] = {
{USB_CTL_MSG_TYPE_QMI, "USB_CTL_MSG_TYPE_QMI"},
};

static const QMI_NAME_T qmi_CtlFlags[] = {
qmi_name_item(QMICTL_CTL_FLAG_CMD),
qmi_name_item(QCQMI_CTL_FLAG_SERVICE),
};

static const QMI_NAME_T qmi_QMIType[] = {
qmi_name_item(QMUX_TYPE_CTL),
qmi_name_item(QMUX_TYPE_WDS),
qmi_name_item(QMUX_TYPE_DMS),
qmi_name_item(QMUX_TYPE_NAS),
qmi_name_item(QMUX_TYPE_QOS),
qmi_name_item(QMUX_TYPE_WMS),
qmi_name_item(QMUX_TYPE_PDS),
qmi_name_item(QMUX_TYPE_WDS_ADMIN),
};

static const QMI_NAME_T qmi_ctl_CtlFlags[] = {
qmi_name_item(QMICTL_FLAG_REQUEST),
qmi_name_item(QMICTL_FLAG_RESPONSE),
qmi_name_item(QMICTL_FLAG_INDICATION),
};
#endif

static const QMI_NAME_T qmux_ctl_QMICTLType[] = {
// QMICTL Type
qmi_name_item(QMICTL_SET_INSTANCE_ID_REQ), //    0x0020
qmi_name_item(QMICTL_SET_INSTANCE_ID_RESP), //   0x0020
qmi_name_item(QMICTL_GET_VERSION_REQ), //        0x0021
qmi_name_item(QMICTL_GET_VERSION_RESP), //       0x0021
qmi_name_item(QMICTL_GET_CLIENT_ID_REQ), //      0x0022
qmi_name_item(QMICTL_GET_CLIENT_ID_RESP), //     0x0022
qmi_name_item(QMICTL_RELEASE_CLIENT_ID_REQ), //  0x0023
qmi_name_item(QMICTL_RELEASE_CLIENT_ID_RESP), // 0x0023
qmi_name_item(QMICTL_REVOKE_CLIENT_ID_IND), //   0x0024
qmi_name_item(QMICTL_INVALID_CLIENT_ID_IND), //  0x0025
qmi_name_item(QMICTL_SET_DATA_FORMAT_REQ), //    0x0026
qmi_name_item(QMICTL_SET_DATA_FORMAT_RESP), //   0x0026
qmi_name_item(QMICTL_SYNC_REQ), //               0x0027
qmi_name_item(QMICTL_SYNC_RESP), //              0x0027
qmi_name_item(QMICTL_SYNC_IND), //               0x0027
};

static const QMI_NAME_T qmux_CtlFlags[] = {
qmi_name_item(QMUX_CTL_FLAG_TYPE_CMD),
qmi_name_item(QMUX_CTL_FLAG_TYPE_RSP),
qmi_name_item(QMUX_CTL_FLAG_TYPE_IND),
};

//Add by Remy
static const QMI_NAME_T qmux_uim_http_Type[] = {
qmi_name_item(QMI_UIM_HTTP_TRANSACTION_REQ),
qmi_name_item(QMI_UIM_HTTP_TRANSACTION_RESP),
qmi_name_item(QMI_UIM_HTTP_TRANSACTION_IND)
};
// Add by Remy

static const QMI_NAME_T qmux_nas_Type[] = {
// ======================= NAS ==============================
qmi_name_item(QMINAS_SET_EVENT_REPORT_REQ), //             0x0002
qmi_name_item(QMINAS_SET_EVENT_REPORT_RESP), //            0x0002
qmi_name_item(QMINAS_EVENT_REPORT_IND), //                 0x0002

//Add by Demon. 
qmi_name_item(QMINAS_INDICATION_REGISTER_REQ),//           0x0003
qmi_name_item(QMINAS_INDICATION_REGISTER_RESP),//          0x0003
qmi_name_item(QMINAS_NR5G_TIME_SYNC_PULSE_REPORT_IND), //  0x0120
qmi_name_item(QMINAS_NR5G_LOST_FRAME_SYNC_IND),         // 0x0121
qmi_name_item(QMINAS_SET_NR5G_SYNC_PULSE_GEN_REQ),      // 0X0122
qmi_name_item(QMINAS_SET_NR5G_SYNC_PULSE_GEN_RESP),     // 0X0122
qmi_name_item(QMINAS_NR5G_RESOURCE_CONFIG_IND),         // 0x0131
//End add by Demon.

qmi_name_item(QMINAS_GET_SIGNAL_STRENGTH_REQ), //          0x0020
qmi_name_item(QMINAS_GET_SIGNAL_STRENGTH_RESP), //         0x0020
qmi_name_item(QMINAS_PERFORM_NETWORK_SCAN_REQ), //         0x0021
qmi_name_item(QMINAS_PERFORM_NETWORK_SCAN_RESP), //        0x0021
qmi_name_item(QMINAS_INITIATE_NW_REGISTER_REQ), //         0x0022
qmi_name_item(QMINAS_INITIATE_NW_REGISTER_RESP), //        0x0022
qmi_name_item(QMINAS_INITIATE_ATTACH_REQ), //              0x0023
qmi_name_item(QMINAS_INITIATE_ATTACH_RESP), //             0x0023
qmi_name_item(QMINAS_GET_SERVING_SYSTEM_REQ), //           0x0024
qmi_name_item(QMINAS_GET_SERVING_SYSTEM_RESP), //          0x0024
qmi_name_item(QMINAS_SERVING_SYSTEM_IND), //               0x0024
qmi_name_item(QMINAS_GET_HOME_NETWORK_REQ), //             0x0025
qmi_name_item(QMINAS_GET_HOME_NETWORK_RESP), //            0x0025
qmi_name_item(QMINAS_GET_PREFERRED_NETWORK_REQ), //        0x0026
qmi_name_item(QMINAS_GET_PREFERRED_NETWORK_RESP), //       0x0026
qmi_name_item(QMINAS_SET_PREFERRED_NETWORK_REQ), //        0x0027
qmi_name_item(QMINAS_SET_PREFERRED_NETWORK_RESP), //       0x0027
qmi_name_item(QMINAS_GET_FORBIDDEN_NETWORK_REQ), //        0x0028
qmi_name_item(QMINAS_GET_FORBIDDEN_NETWORK_RESP), //       0x0028
qmi_name_item(QMINAS_SET_FORBIDDEN_NETWORK_REQ), //        0x0029
qmi_name_item(QMINAS_SET_FORBIDDEN_NETWORK_RESP), //       0x0029
qmi_name_item(QMINAS_SET_TECHNOLOGY_PREF_REQ), //          0x002A
qmi_name_item(QMINAS_SET_TECHNOLOGY_PREF_RESP), //         0x002A
qmi_name_item(QMINAS_GET_RF_BAND_INFO_REQ), //             0x0031
qmi_name_item(QMINAS_GET_RF_BAND_INFO_RESP), //            0x0031
qmi_name_item(QMINAS_GET_PLMN_NAME_REQ), //                0x0044
qmi_name_item(QMINAS_GET_PLMN_NAME_RESP), //               0x0044
qmi_name_item(QUECTEL_PACKET_TRANSFER_START_IND), //       0X100
qmi_name_item(QUECTEL_PACKET_TRANSFER_END_IND), //         0X101
qmi_name_item(QMINAS_GET_SYS_INFO_REQ), //                 0x004D
qmi_name_item(QMINAS_GET_SYS_INFO_RESP), //                0x004D
qmi_name_item(QMINAS_SYS_INFO_IND), //                     0x004D
};

static const char * qmi_name_get(const QMI_NAME_T *table, size_t size, int type, const char *tag) {
    static char unknow[40];
    size_t i;

    if (qmux_CtlFlags == table) {
        if (!strcmp(tag, "_REQ"))
            tag = "_CMD";
        else  if (!strcmp(tag, "_RESP"))
            tag = "_RSP";
    }
    
    for (i = 0; i < size; i++) {
        if (table[i].type == (UINT)type) {
            if (!tag || (strstr(table[i].name, tag)))
                return table[i].name;
        }
    }
    sprintf(unknow, "unknow_%x", type);
    return unknow;
}

#define QMI_NAME(table, type) qmi_name_get(table, sizeof(table) / sizeof(table[0]), type, 0)
#define QMUX_NAME(table, type, tag) qmi_name_get(table, sizeof(table) / sizeof(table[0]), type, tag)

void dump_tlv(PQCQMUX_MSG_HDR pQMUXMsgHdr) {
    int TLVFind = 0;
    int i;
    dbg("QCQMUX_TLV-----------------------------------\n");
    dbg("{Type,\tLength,\tValue}\n");

    while (1) {
        PQMI_TLV_HDR TLVHdr = GetTLV(pQMUXMsgHdr, 0x1000 + (++TLVFind));
        if (TLVHdr == NULL)
            break;

        //if ((TLVHdr->TLVType == 0x02) && ((USHORT *)(TLVHdr+1))[0])
        {        
            dbg("{%02x,\t%04x,\t", TLVHdr->TLVType, le16_to_cpu(TLVHdr->TLVLength));
            for (i = 0; i < le16_to_cpu(TLVHdr->TLVLength); i++) {
                dbg("%02x ", ((UCHAR *)(TLVHdr+1))[i]);
                //dbg("%c", ((UCHAR *)(TLVHdr+1))[i]);
            }
            dbg("}\n");
        }
    }  // while
}

void dump_ctl(PQCQMICTL_MSG_HDR CTLHdr) {
    const char *tag;
    
    //dbg("QCQMICTL_MSG--------------------------------------------\n");
    //dbg("CtlFlags:           %02x\t\t%s\n", CTLHdr->CtlFlags, QMI_NAME(qmi_ctl_CtlFlags, CTLHdr->CtlFlags));
   dbg("TransactionId:      %02x\n", CTLHdr->TransactionId);
        switch (CTLHdr->CtlFlags) {
            case QMICTL_FLAG_REQUEST: tag = "_REQ"; break;
            case QMICTL_FLAG_RESPONSE: tag = "_RESP"; break;
            case QMICTL_FLAG_INDICATION: tag = "_IND"; break;
            default: tag = 0; break;
       }
        dbg("QMICTLType:         %04x\t%s\n", le16_to_cpu(CTLHdr->QMICTLType),
        QMUX_NAME(qmux_ctl_QMICTLType, le16_to_cpu(CTLHdr->QMICTLType), tag));     
        dbg("Length:             %04x\n", le16_to_cpu(CTLHdr->Length));

     dump_tlv((PQCQMUX_MSG_HDR)(&CTLHdr->QMICTLType));
}

int dump_qmux(QMI_SERVICE_TYPE serviceType, PQCQMUX_HDR QMUXHdr) {
    PQCQMUX_MSG_HDR QMUXMsgHdr = (PQCQMUX_MSG_HDR) (QMUXHdr + 1);
    const char *tag;

    //dbg("QCQMUX--------------------------------------------\n");
    switch (QMUXHdr->CtlFlags&QMUX_CTL_FLAG_MASK_TYPE) {
        case QMUX_CTL_FLAG_TYPE_CMD: tag = "_REQ"; break;
        case QMUX_CTL_FLAG_TYPE_RSP: tag = "_RESP"; break;
        case QMUX_CTL_FLAG_TYPE_IND: tag = "_IND"; break;
        default: tag = 0; break;
    }
    //dbg("CtlFlags:           %02x\t\t%s\n", QMUXHdr->CtlFlags, QMUX_NAME(qmux_CtlFlags, QMUXHdr->CtlFlags, tag));
    dbg("TransactionId:    %04x\n", le16_to_cpu(QMUXHdr->TransactionId));

    //dbg("QCQMUX_MSG_HDR-----------------------------------\n");
    switch (serviceType) {
        case QMUX_TYPE_NAS:
            dbg("Type:               %04x\t%s\n", le16_to_cpu(QMUXMsgHdr->Type),
            QMUX_NAME(qmux_nas_Type, le16_to_cpu(QMUXMsgHdr->Type), tag));
        break;
        case QMUX_TYPE_UIM_HTTP:
            dbg("Type:               %04x\t%s\n", le16_to_cpu(QMUXMsgHdr->Type),
            QMUX_NAME(qmux_uim_http_Type, le16_to_cpu(QMUXMsgHdr->Type), tag));
            break;
        case QMUX_TYPE_CTL:                
        default:
            dbg("Type:               %04x\t%s\n", le16_to_cpu(QMUXMsgHdr->Type), "PDS/QOS/CTL/unknown!");
        break;    
    }
    dbg("Length:             %04x\n", le16_to_cpu(QMUXMsgHdr->Length));

    dump_tlv(QMUXMsgHdr);
    
    return 0;
}

void dump_qmi(void *dataBuffer, int dataLen) 
{
    PQCQMI_HDR QMIHdr = (PQCQMI_HDR)dataBuffer;
    PQCQMUX_HDR QMUXHdr = (PQCQMUX_HDR) (QMIHdr + 1);
    PQCQMICTL_MSG_HDR CTLHdr =  (PQCQMICTL_MSG_HDR) (QMIHdr + 1);

    int i;

    if (!debug_qmi)
        return;

    pthread_mutex_lock(&dumpQMIMutex);
    line[0] = 0;
    for (i = 0; i < dataLen; i++) {
        dbg("%02x ", ((unsigned char *)dataBuffer)[i]);
        // dbg("%c", ((unsigned char *)dataBuffer)[i]);
    }
    dbg_time("%s", line);
    line[0] = 0;
    
    //dbg("QCQMI_HDR-----------------------------------------");
    //dbg("IFType:             %02x\t\t%s", QMIHdr->IFType, QMI_NAME(qmi_IFType, QMIHdr->IFType));
    //dbg("Length:             %04x", le16_to_cpu(QMIHdr->Length));
    //dbg("CtlFlags:           %02x\t\t%s", QMIHdr->CtlFlags, QMI_NAME(qmi_CtlFlags, QMIHdr->CtlFlags));
    //dbg("QMIType:            %02x\t\t%s", QMIHdr->QMIType, QMI_NAME(qmi_QMIType, QMIHdr->QMIType));
    //dbg("ClientId:           %02x", QMIHdr->ClientId);

    if (QMIHdr->QMIType == QMUX_TYPE_CTL) {
        dump_ctl(CTLHdr);
    } else {
        dump_qmux(QMIHdr->QMIType, QMUXHdr);
    }
    dbg_time("%s", line);
    pthread_mutex_unlock(&dumpQMIMutex);
}
