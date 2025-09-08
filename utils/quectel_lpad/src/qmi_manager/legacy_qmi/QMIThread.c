/******************************************************************************
  @file    QMIThread.c
  @brief   QMI WWAN connectivity manager.

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
#ifndef MIN
#define MIN(a, b)	((a) < (b)? (a): (b))
#endif

#define qmi_rsp_check_and_return() do { \
        if (err < 0 || pResponse == NULL) { \
            dbg_time("%s err = %d", __func__, err); \
            return err; \
        } \
        pMUXMsg = &pResponse->MUXMsg; \
        if (le16_to_cpu(pMUXMsg->QMUXMsgHdrResp.QMUXResult) || le16_to_cpu(pMUXMsg->QMUXMsgHdrResp.QMUXError)) { \
            USHORT QMUXError = le16_to_cpu(pMUXMsg->QMUXMsgHdrResp.QMUXError); \
            dbg_time("%s QMUXResult = 0x%x, QMUXError = 0x%x", __func__, \
                le16_to_cpu(pMUXMsg->QMUXMsgHdrResp.QMUXResult), QMUXError); \
            free(pResponse); \
            return QMUXError; \
        } \
} while(0)

#define qmi_rsp_check() do { \
        if (err < 0 || pResponse == NULL) { \
            dbg_time("%s err = %d", __func__, err); \
            return err; \
        } \
        pMUXMsg = &pResponse->MUXMsg; \
        if (le16_to_cpu(pMUXMsg->QMUXMsgHdrResp.QMUXResult) || le16_to_cpu(pMUXMsg->QMUXMsgHdrResp.QMUXError)) { \
            USHORT QMUXError = le16_to_cpu(pMUXMsg->QMUXMsgHdrResp.QMUXError); \
            dbg_time("%s QMUXResult = 0x%x, QMUXError = 0x%x", __func__, \
                le16_to_cpu(pMUXMsg->QMUXMsgHdrResp.QMUXResult), QMUXError); \
        } \
} while(0)

static void uchar2char(char *dst_ptr, size_t dst_len, const UCHAR *src_ptr, size_t src_len) { 
    size_t copy = MIN(dst_len-1, src_len);

    if (copy)
        memcpy(dst_ptr, src_ptr, copy);
    dst_ptr[copy] = 0;
}

typedef USHORT (*CUSTOMQMUX)(PQMUX_MSG pMUXMsg, void *arg);

extern UCHAR qmiclientId[QMUX_TYPE_MAX];

// To retrieve the ith (Index) TLV
PQMI_TLV_HDR GetTLV (PQCQMUX_MSG_HDR pQMUXMsgHdr, int TLVType) {
    int TLVFind = 0;
    USHORT Length = le16_to_cpu(pQMUXMsgHdr->Length);
    PQMI_TLV_HDR pTLVHdr = (PQMI_TLV_HDR)(pQMUXMsgHdr + 1);

    while (Length >= sizeof(QMI_TLV_HDR)) {
        TLVFind++;
        if (TLVType > 0x1000) {
            if ((TLVFind + 0x1000) == TLVType)
                return pTLVHdr;
        } else  if (pTLVHdr->TLVType == TLVType) {
            return pTLVHdr;
        }

        Length -= (le16_to_cpu((pTLVHdr->TLVLength)) + sizeof(QMI_TLV_HDR));
        pTLVHdr = (PQMI_TLV_HDR)(((UCHAR *)pTLVHdr) + le16_to_cpu(pTLVHdr->TLVLength) + sizeof(QMI_TLV_HDR));
    }

   return NULL;
}

static USHORT GetQMUXTransactionId(void) {
    static int TransactionId = 0;
    if (++TransactionId > 0xFFFF)
        TransactionId = 1;
    return TransactionId;
}

static PQCQMIMSG ComposeQMUXMsg(UCHAR QMIType, USHORT Type, CUSTOMQMUX customQmuxMsgFunction, void *arg) {
    UCHAR QMIBuf[WDM_DEFAULT_BUFSIZE];
    PQCQMIMSG pRequest = (PQCQMIMSG)QMIBuf;
    int Length;

    memset(QMIBuf, 0x00, sizeof(QMIBuf));
    pRequest->QMIHdr.IFType = USB_CTL_MSG_TYPE_QMI;
    pRequest->QMIHdr.CtlFlags = 0x00;
    pRequest->QMIHdr.QMIType = QMIType;
    pRequest->QMIHdr.ClientId = (UCHAR)qmiclientId[QMUX_TYPE_UIM_HTTP];

    pRequest->MUXMsg.QMUXHdr.CtlFlags = QMUX_CTL_FLAG_SINGLE_MSG | QMUX_CTL_FLAG_TYPE_CMD;
    pRequest->MUXMsg.QMUXHdr.TransactionId = cpu_to_le16(GetQMUXTransactionId());
    pRequest->MUXMsg.QMUXMsgHdr.Type = cpu_to_le16(Type);
    if (customQmuxMsgFunction)
        pRequest->MUXMsg.QMUXMsgHdr.Length = cpu_to_le16(customQmuxMsgFunction(&pRequest->MUXMsg, arg) - sizeof(QCQMUX_MSG_HDR));
    else
        pRequest->MUXMsg.QMUXMsgHdr.Length = cpu_to_le16(0x0000);

    pRequest->QMIHdr.Length = cpu_to_le16(le16_to_cpu(pRequest->MUXMsg.QMUXMsgHdr.Length) + sizeof(QCQMUX_MSG_HDR) + sizeof(QCQMUX_HDR)
        + sizeof(QCQMI_HDR) - 1);
    Length = le16_to_cpu(pRequest->QMIHdr.Length) + 1;

    pRequest = (PQCQMIMSG)malloc(Length);
    if (pRequest == NULL) {
        dbg_time("%s fail to malloc", __func__);
    } else {
        memcpy(pRequest, QMIBuf, Length);
    }

    // uint8_t *u8Ptr = (uint8_t*)pRequest;
    // uint32_t i=0;
    // for(i=0; i<Length; i++){
    //     if(0 == i%16){
	// 		printf("\n");
	// 	}
    //     printf("%02x ", u8Ptr[i]);
    // }

    return pRequest;
}


enum peripheral_ep_type {
	DATA_EP_TYPE_RESERVED	= 0x0,
	DATA_EP_TYPE_HSIC	= 0x1,
	DATA_EP_TYPE_HSUSB	= 0x2,
	DATA_EP_TYPE_PCIE	= 0x3,
	DATA_EP_TYPE_EMBEDDED	= 0x4,
	DATA_EP_TYPE_BAM_DMUX	= 0x5,
};

static PQCQMIMSG s_pRequest;
static PQCQMIMSG s_pResponse;

static int is_response(const PQCQMIMSG pRequest, const PQCQMIMSG pResponse) {
    if ((pRequest->QMIHdr.QMIType == pResponse->QMIHdr.QMIType)
        && (pRequest->QMIHdr.ClientId == pResponse->QMIHdr.ClientId)) {
            USHORT requestTID, responseTID;
        if (pRequest->QMIHdr.QMIType == QMUX_TYPE_CTL) {
            requestTID = pRequest->CTLMsg.QMICTLMsgHdr.TransactionId;
            responseTID = pResponse->CTLMsg.QMICTLMsgHdr.TransactionId;
        } else {
            requestTID = le16_to_cpu(pRequest->MUXMsg.QMUXHdr.TransactionId);
            responseTID = le16_to_cpu(pResponse->MUXMsg.QMUXHdr.TransactionId);
        }
        return (requestTID == responseTID);
    }
    return 0;
}

int (*qmidev_send)(PQCQMIMSG pRequest);

int QmiThreadSendQMITimeout(PQCQMIMSG pRequest, PQCQMIMSG *ppResponse, unsigned msecs, const char *funcname) {
    int ret;
    
    if (!pRequest)
        return -EINVAL;

    pthread_mutex_lock(&cm_command_mutex);

    if (ppResponse)
        *ppResponse = NULL;

    dump_qmi(pRequest, le16_to_cpu(pRequest->QMIHdr.Length) + 1);

    s_pRequest = pRequest;
    s_pResponse = NULL;

    ret = qmidev_send(pRequest);

    if (ret == 0) {
        ret = pthread_cond_timeout_np(&cm_command_cond, &cm_command_mutex, msecs);
        if (!ret) {
            if (s_pResponse && ppResponse) {
                *ppResponse = s_pResponse;
            } else {
                if (s_pResponse) {
                    free(s_pResponse);
                    s_pResponse = NULL;
                }
            }
        } else {
            dbg_time("%s message timeout", funcname);
        }
    }

    pthread_mutex_unlock(&cm_command_mutex);

    return ret;
}

void QmiThreadRecvQMI(PQCQMIMSG pResponse) {
    pthread_mutex_lock(&cm_command_mutex);
    if (pResponse == NULL) {
        if (s_pRequest) {
            free(s_pRequest);
            s_pRequest = NULL;
            s_pResponse = NULL;
            pthread_cond_signal(&cm_command_cond);
        }
        pthread_mutex_unlock(&cm_command_mutex);
        return;
    }
    dump_qmi(pResponse, le16_to_cpu(pResponse->QMIHdr.Length) + 1);
    if (s_pRequest && is_response(s_pRequest, pResponse)) {
        free(s_pRequest);
        s_pRequest = NULL;
        s_pResponse = malloc(le16_to_cpu(pResponse->QMIHdr.Length) + 1);
        if (s_pResponse != NULL) {
            memcpy(s_pResponse, pResponse, le16_to_cpu(pResponse->QMIHdr.Length) + 1);
        }
        pthread_cond_signal(&cm_command_cond);
    } else if ((pResponse->QMIHdr.QMIType == QMUX_TYPE_CTL)
                    && (le16_to_cpu(pResponse->CTLMsg.QMICTLMsgHdrRsp.QMICTLType == QMICTL_REVOKE_CLIENT_ID_IND))) {
        qmidevice_send_event_to_manager(MODEM_REPORT_RESET_EVENT);
    } else if ((pResponse->QMIHdr.QMIType == QMUX_TYPE_NAS)
                    && (le16_to_cpu(pResponse->MUXMsg.QMUXMsgHdrResp.Type) == QMINAS_SERVING_SYSTEM_IND)) {
        qmidevice_send_event_to_manager(RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED);
    } else if ((pResponse->QMIHdr.QMIType == QMUX_TYPE_NAS)
                    && (le16_to_cpu(pResponse->MUXMsg.QMUXMsgHdrResp.Type) == QMINAS_SYS_INFO_IND)) {
        qmidevice_send_event_to_manager(RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED);
    } else if ((pResponse->QMIHdr.QMIType == QMUX_TYPE_NAS) 
                    && (le16_to_cpu(pResponse->MUXMsg.QMUXMsgHdrResp.Type) == QMINAS_NR5G_TIME_SYNC_PULSE_REPORT_IND)) {
        qmidevice_send_event_to_manager_ext(NR5G_TIME_SYNC_PULSE_REPORT_IND, &pResponse->MUXMsg.SetNR5GPulseInd, sizeof(pResponse->MUXMsg.SetNR5GPulseInd));
    } else if ((pResponse->QMIHdr.QMIType == QMUX_TYPE_UIM_HTTP) 
                    && (le16_to_cpu(pResponse->MUXMsg.QMUXMsgHdrResp.Type) == QMI_UIM_HTTP_TRANSACTION_IND)) { 
        qmidevice_send_event_to_manager_ext(UIM_HTTP_TRANSACTION_IND, pResponse, sizeof(QCQMIMSG));
    } else if((pResponse->QMIHdr.QMIType == QMUX_TYPE_UIM) 
                    && (le16_to_cpu(pResponse->MUXMsg.QMUXMsgHdrResp.Type) == QMI_UIM_ADD_PROFILE_IND)){
        qmidevice_send_event_to_manager_ext(UIM_ADD_PROFILE_IND, pResponse, sizeof(QCQMIMSG));
    }else {
        if (debug_qmi)
            dbg_time("nobody care this qmi msg!!");
    }
    pthread_mutex_unlock(&cm_command_mutex);
}


#ifdef CONFIG_VERSION
static int requestBaseBandVersion(PROFILE_T *profile) {
    PQCQMIMSG pRequest;
    PQCQMIMSG pResponse;
    PQMUX_MSG pMUXMsg;
    PDEVICE_REV_ID revId;
    int err;

    pRequest = ComposeQMUXMsg(QMUX_TYPE_DMS, QMIDMS_GET_DEVICE_REV_ID_REQ, NULL, NULL);
    err = QmiThreadSendQMI(pRequest, &pResponse);
    qmi_rsp_check_and_return();

    revId = (PDEVICE_REV_ID)GetTLV(&pResponse->MUXMsg.QMUXMsgHdr, 0x01);

    if (revId && le16_to_cpu(revId->TLVLength))
    {
        uchar2char(profile->BaseBandVersion, sizeof(profile->BaseBandVersion), &revId->RevisionID, le16_to_cpu(revId->TLVLength));
        dbg_time("%s %s", __func__, profile->BaseBandVersion);
    }

    free(pResponse);
    return 0;
}
#endif

//Add by Demon
//
static USHORT NasIndRegisterReqSend(PQMUX_MSG pMUXMsg, void *arg)
{
    PQMINAS_IND_REGISTER pQMINasIndReg;
    PQMINAS_REG_NR5G_PULSE_IND_REQ_MSG           pNasNR5GSyncPulse; 
    PQMINAS_REG_NR5G_LOST_FRAME_SYNC_IND_REQ_MSG pNasNR5GLostSync;
    PQMINAS_REG_NR5G_RSRC_CFG_IND_REQ_MSG        pNasNR5GRsrcCfg;
    UCHAR *pTLV;
    USHORT TLVLength = 0;
    
    pQMINasIndReg= (PQMINAS_IND_REGISTER)arg;
    
    pTLV = (UCHAR *)(&pMUXMsg->ReqRegisterInd + 1);
    pMUXMsg->ReqRegisterInd.Length = 0;
    
    //Enable/disable reg_nr5g_time_sync_pulse_report_ind
    pNasNR5GSyncPulse = (PQMINAS_REG_NR5G_PULSE_IND_REQ_MSG)(pTLV + TLVLength);

    pNasNR5GSyncPulse->TLVType         = 0x4F;
    pNasNR5GSyncPulse->TLVLength       = cpu_to_le16(0x01);
    pNasNR5GSyncPulse->RegNR5GPulseInd = pQMINasIndReg->RegNR5GPulseInd;
    
    TLVLength +=(le16_to_cpu(pNasNR5GSyncPulse->TLVLength) + sizeof(QCQMICTL_TLV_HDR));
    
    //Enable/disable reg_nr5g_lost_sync_frame_ind
    pNasNR5GLostSync = (PQMINAS_REG_NR5G_LOST_FRAME_SYNC_IND_REQ_MSG)(pTLV + TLVLength);
    
    pNasNR5GLostSync->TLVType         = 0x50;
    pNasNR5GLostSync->TLVLength       = cpu_to_le16(0x01);
    pNasNR5GLostSync->RegNR5GLostSync = pQMINasIndReg->RegNR5GLostSync;
    
    TLVLength +=(le16_to_cpu(pNasNR5GLostSync->TLVLength) + sizeof(QCQMICTL_TLV_HDR));

    // Enable/disable reg_nr5g_resource_config_ind
    pNasNR5GRsrcCfg = (PQMINAS_REG_NR5G_RSRC_CFG_IND_REQ_MSG)(pTLV + TLVLength);

    pNasNR5GRsrcCfg->TLVType        = 0x53;
    pNasNR5GRsrcCfg->TLVLength      = cpu_to_le16(0x01);
    pNasNR5GRsrcCfg->RegNR5GRsrcCfg = pQMINasIndReg->RegNR5GRsrcCfg;
    
    TLVLength +=(le16_to_cpu(pNasNR5GRsrcCfg->TLVLength) + sizeof(QCQMICTL_TLV_HDR));

    return (sizeof(QMINAS_INDICATION_REGISTER_REQ_MSG) + TLVLength); 
}

static int requestIndicationRegister(QMINAS_IND_REGISTER qmiNasIndReg)
{
    PQCQMIMSG pRequest;
    PQCQMIMSG pResponse;
    PQMUX_MSG pMUXMsg;
    
    int err;
    
    dbg_time("%s(%d)(%d)", __func__, qmiNasIndReg.RegNR5GPulseInd, qmiNasIndReg.RegNR5GLostSync);
    
    pRequest = ComposeQMUXMsg(QMUX_TYPE_NAS, QMINAS_INDICATION_REGISTER_REQ, NasIndRegisterReqSend, (void *)&qmiNasIndReg);
    err = QmiThreadSendQMI(pRequest, &pResponse);
    qmi_rsp_check_and_return();
    
    free(pResponse);
    return err;
}

static USHORT NasSetSyncPulseGen(PQMUX_MSG pMUXMsg, void *arg)
{
    USHORT TLVLength = 0;
    UCHAR *pTLV;
//    PSYNC_PULSE_GEN_PULSE_PERIOD   pPulsePeriod;
    PSYNC_PULSE_GEN_START_SFN      pStartSfn;
    PSYNC_PULSE_GEN_REPORT_PERIOD  pReportPeriod;
    PINFO_SYNC_PULSE_GEN pInfoSyncGen = (PINFO_SYNC_PULSE_GEN)arg;

    pMUXMsg->SetSyncPulseGen.Length      = cpu_to_le16(sizeof(QMINAS_SET_NR5G_SYNC_PULSE_GEN_REQ_MSG) - 4);
    pMUXMsg->SetSyncPulseGen.TLVType     = 0x01;
    pMUXMsg->SetSyncPulseGen.TLVLength   = cpu_to_le16(sizeof(UINT));
    pMUXMsg->SetSyncPulseGen.pulsePeriod = pInfoSyncGen->pulsePeriod;

    pTLV = (UCHAR *)(&pMUXMsg->SetSyncPulseGen + 1);
    pMUXMsg->SetSyncPulseGen.Length = 0;

    //set pulse period
    // pPulsePeriod = (PSYNC_PULSE_GEN_PULSE_PERIOD)(pTLV + TLVLength);
    // pPulsePeriod->TLVType     = 0x01;
    // pPulsePeriod->pulsePeriod = pInfoSyncGen->pulsePeriod;
    // pPulsePeriod->TLVLength   = cpu_to_le16(sizeof(UINT));

    // TLVLength +=(le16_to_cpu(pPulsePeriod->TLVLength) + sizeof(QCQMICTL_TLV_HDR));

    //set start sfn
    pStartSfn = (PSYNC_PULSE_GEN_START_SFN)(pTLV + TLVLength);
    pStartSfn->TLVType   = 0x10;
    pStartSfn->TLVLength = cpu_to_le16(sizeof(UINT));
    pStartSfn->startSFN  = pInfoSyncGen->startSFN;

    TLVLength +=(le16_to_cpu(pStartSfn->TLVLength) + sizeof(QCQMICTL_TLV_HDR));

    //set report period
    pReportPeriod = (PSYNC_PULSE_GEN_REPORT_PERIOD)(pTLV + TLVLength);
    pReportPeriod->TLVType      = 0x11;
    pReportPeriod->TLVLength    = cpu_to_le16(sizeof(UINT));
    pReportPeriod->reportPeriod = pInfoSyncGen->reportPeriod;

    TLVLength +=(le16_to_cpu(pReportPeriod->TLVLength) + sizeof(QCQMICTL_TLV_HDR));

    return sizeof(QMINAS_SET_NR5G_SYNC_PULSE_GEN_REQ_MSG) + TLVLength;
}

static uint8_t* byteAppend(uint8_t *dest, uint8_t *src, uint32_t nByte)
{
    memcpy(dest, src, nByte);
    return dest + nByte;
}

static USHORT UimHttpTransactionReq(PQMUX_MSG pMUXMsg, void *arg)
{
    uim_http_transaction_req_msg_v01 *pReq = (uim_http_transaction_req_msg_v01*)arg;
    memset(&pMUXMsg->UimHttpTransactionReq, 0x00, sizeof(pMUXMsg->UimHttpTransactionReq));
    uint16_t reqTotalSize = 0;
    uint8_t *qmiHead = (uint8_t*)&pMUXMsg->UimHttpTransactionReq;
    uint8_t *qmiTail = 0;

    pMUXMsg->UimHttpTransactionReq.Type = QMI_UIM_HTTP_TRANSACTION_REQ;
    pMUXMsg->UimHttpTransactionReq.result.TLVType = 0x01;
    pMUXMsg->UimHttpTransactionReq.result.TLVLength = 4;
    pMUXMsg->UimHttpTransactionReq.result.result = (uint32_t)pReq->result;

    pMUXMsg->UimHttpTransactionReq.tokenId.TLVType = 0x02;
    pMUXMsg->UimHttpTransactionReq.tokenId.TLVLength = 4;
    pMUXMsg->UimHttpTransactionReq.tokenId.tokenId = (uint32_t)pReq->token_id;

    pMUXMsg->UimHttpTransactionReq.headers.TLVType = 0x10;
    pMUXMsg->UimHttpTransactionReq.headers.customHeaderLen = pReq->headers_len;
    uint16_t headersSize = 1;
    qmiTail = (uint8_t*)pMUXMsg->UimHttpTransactionReq.headers.customHeader;
    uint8_t u8Temp = 0, i = 0;
    uint16_t u16Temp = 0;
    for(i=0; i< pReq->headers_len; i++){
        u8Temp = strlen(pReq->headers[i].name);
        qmiTail = byteAppend(qmiTail, &u8Temp, 1);
        qmiTail = byteAppend(qmiTail, (uint8_t*)pReq->headers[i].name, u8Temp);
        u16Temp = strlen(pReq->headers[i].value);
        qmiTail = byteAppend(qmiTail, (uint8_t*)&u16Temp, 2);
        qmiTail = byteAppend(qmiTail, (uint8_t*)pReq->headers[i].value, u16Temp);
        // dbg_time("header(%d) name(%d): %s, value(%d): %s", i, u8Temp, pReq->headers[i].name, u16Temp, pReq->headers[i].value);
        headersSize += u8Temp + u16Temp + 3;
    }
    pMUXMsg->UimHttpTransactionReq.headers.TLVLength = headersSize;

    UIM_HTTP_TRANSACTION_SEGMENT segmentInfo;
    segmentInfo.TLVType = 0x11;
    segmentInfo.TLVLength = 8;
    segmentInfo.totalSize = pReq->segment_info.total_size;
    segmentInfo.offset = pReq->segment_info.segment_offset;
    qmiTail = byteAppend(qmiTail, (uint8_t*)&segmentInfo, sizeof(segmentInfo));

    u8Temp = 0x12;
    u16Temp = 2 + pReq->payload_body_len;
    qmiTail = byteAppend(qmiTail, &u8Temp, 1);
    qmiTail = byteAppend(qmiTail, (uint8_t*)&u16Temp, 2);
    qmiTail = byteAppend(qmiTail, (uint8_t*)&pReq->payload_body_len, 2);
    qmiTail = byteAppend(qmiTail, (uint8_t*)&pReq->payload_body, u16Temp);

    reqTotalSize = qmiTail - qmiHead - 2;
    pMUXMsg->UimHttpTransactionReq.Length = reqTotalSize - 4;

    dbg_time("Segment Total Size = %d, Segment Offset= %d, Request Total Size = %d", segmentInfo.totalSize, segmentInfo.offset, reqTotalSize);
    return reqTotalSize;
}

static USHORT UimAddProfileReq(PQMUX_MSG pMUXMsg, void *arg)
{
    uim_add_profile_req_msg_v01 *pReq = (uim_add_profile_req_msg_v01*)arg;
    USHORT activeCodeLen = strlen(pReq->activation_code);
    USHORT confirmationCodeLen = strlen(pReq->confirmation_code);

    pMUXMsg->UimAddProfileReq.Type = QMI_UIM_ADD_PROFILE_REQ;
    pMUXMsg->UimAddProfileReq.Length = sizeof(UIM_ADD_PROFILE_SLOT) + activeCodeLen + 5 + confirmationCodeLen + 5;
    //slot
    pMUXMsg->UimAddProfileReq.slot.TLVType = 0x01;
    pMUXMsg->UimAddProfileReq.slot.TLVLength = cpu_to_le16(1);
    pMUXMsg->UimAddProfileReq.slot.slot = pReq->slot;
    //activation
    pMUXMsg->UimAddProfileReq.activeCode.TLVType = 0x02;
    pMUXMsg->UimAddProfileReq.activeCode.TLVLength = cpu_to_le16(activeCodeLen + 2);
    pMUXMsg->UimAddProfileReq.activeCode.activeCodeLen = cpu_to_le16(activeCodeLen);

    UIM_ADD_PROFILE_ACTIVE_CODE confirmation_code;
    confirmation_code.TLVType = 0x10;
    confirmation_code.TLVLength = cpu_to_le16(confirmationCodeLen + 2);
	
    uint8_t *QmiAddProfileTail = (uint8_t *)pMUXMsg->UimAddProfileReq.activeCode.activeCode;
    QmiAddProfileTail = byteAppend(QmiAddProfileTail,(uint8_t *)pReq->activation_code,(uint32_t)activeCodeLen);
    QmiAddProfileTail = byteAppend(QmiAddProfileTail,(uint8_t *)(&confirmation_code.TLVType),1);
    QmiAddProfileTail = byteAppend(QmiAddProfileTail,(uint8_t *)(&confirmation_code.TLVLength),2);
    QmiAddProfileTail = byteAppend(QmiAddProfileTail,(uint8_t*)(&pReq->confirmation_code_len),2);
    QmiAddProfileTail = byteAppend(QmiAddProfileTail,(uint8_t *)pReq->confirmation_code,(uint32_t)confirmationCodeLen);

    return activeCodeLen + 9 + 4 + confirmationCodeLen + 5;
}

static USHORT UimDeleteProfileReq(PQMUX_MSG pMUXMsg, void *arg)
{
    uim_delete_profile_req_msg_v01 *pReq = (uim_delete_profile_req_msg_v01*)arg;

    pMUXMsg->UimDeleteProfileReq.Type = QMI_UIM_DELETE_PROFILE_REQ;
    pMUXMsg->UimDeleteProfileReq.Length = sizeof(QMIUIM_DELETE_PROFILE_REQ_MSG);
    //slot
    pMUXMsg->UimDeleteProfileReq.slot.TLVType = 0x01;
    pMUXMsg->UimDeleteProfileReq.slot.TLVLength = cpu_to_le16(1);
    pMUXMsg->UimDeleteProfileReq.slot.slot = pReq->slot;
    //profile_id
    pMUXMsg->UimDeleteProfileReq.profile_id.TLVType = 0x02;
    pMUXMsg->UimDeleteProfileReq.profile_id.TLVLength = cpu_to_le16(4);
    pMUXMsg->UimDeleteProfileReq.profile_id.profile_id = cpu_to_le16(pReq->profile_id);

    return sizeof(QMIUIM_DELETE_PROFILE_REQ_MSG);
}

static int requestSetSyncPulseGen(INFO_SYNC_PULSE_GEN infoSyncPulseGen)
{
    PQCQMIMSG pRequest;
    PQCQMIMSG pResponse;
    PQMUX_MSG pMUXMsg;
    int err = 0;

    pRequest = ComposeQMUXMsg(QMUX_TYPE_NAS, QMINAS_SET_NR5G_SYNC_PULSE_GEN_REQ, NasSetSyncPulseGen, (void *)&infoSyncPulseGen);
    err = QmiThreadSendQMI(pRequest, &pResponse);
    qmi_rsp_check();
    
    free(pResponse);
    return err;
}

static int requestUimHttpTransaction(uim_http_transaction_req_msg_v01 req)
{
    PQCQMIMSG pRequest;
    PQCQMIMSG pResponse;
    PQMUX_MSG pMUXMsg;
    int err = 0;

    pRequest = ComposeQMUXMsg(QMUX_TYPE_UIM_HTTP, QMI_UIM_HTTP_TRANSACTION_REQ, UimHttpTransactionReq, (void *)&req);
    err = QmiThreadSendQMI(pRequest, &pResponse);
    qmi_rsp_check_and_return();
    return err;
}

static int requestUimAddProfile(uim_add_profile_req_msg_v01 req)
{
    PQCQMIMSG pRequest;
    PQCQMIMSG pResponse;
    PQMUX_MSG pMUXMsg;
    int err = 0;

    pRequest = ComposeQMUXMsg(QMUX_TYPE_UIM, QMI_UIM_ADD_PROFILE_REQ, UimAddProfileReq, (void *)&req);
    err = QmiThreadSendQMI(pRequest, &pResponse);
    qmi_rsp_check_and_return();
    return err;
}

static int requestUimDeleteProfile(uim_delete_profile_req_msg_v01 req)
{
    PQCQMIMSG pRequest;
    PQCQMIMSG pResponse;
    PQMUX_MSG pMUXMsg;
    int err = 0;

    pRequest = ComposeQMUXMsg(QMUX_TYPE_UIM, QMI_UIM_DELETE_PROFILE_REQ, UimDeleteProfileReq, (void *)&req);
    err = QmiThreadSendQMI(pRequest, &pResponse);
    qmi_rsp_check_and_return();
    return err;
}

//End - add by Demon

const struct request_ops qmi_request_ops = {
#ifdef CONFIG_VERSION
    .requestBaseBandVersion = requestBaseBandVersion,
#endif
    //Add by Demon.
    .requestIndicationRegister = requestIndicationRegister,
    .requestSetSyncPulseGen    = requestSetSyncPulseGen,
    .requestUimHttpTransaction = requestUimHttpTransaction,
    .requestUimAddProfile      = requestUimAddProfile,
    .requestUimDeleteProfile   = requestUimDeleteProfile,
    //End add by Demon
};
