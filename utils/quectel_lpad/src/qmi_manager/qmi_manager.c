/*
**  This QMI manager only manages UIM and UIM HTTP, 
**  specifically QMI_UIM_ADD_PROFILE and QMI_UIM_HTTP_TRANSACTION
*/

#define LOG_TAG "QMIM"
#include "qmi_manager.h"
#include "http_manager.h"

#ifdef USING_LEGACY_QMI
#include "QMIThread.h"

extern int ql_get_netcard_driver_info(const char*);
extern int ql_capture_usbmon_log(PROFILE_T *profile, const char *log_path);
extern void ql_stop_usbmon_log(PROFILE_T *profile);

typedef struct qm_qmi_client{
    uint32_t init_flag;
    qm_client_ind_cb ind_cb;
}qm_qmi_client_t;

qm_qmi_client_t qm_client_list[QM_CLIENT_TYPE_NUM];
uint32_t qm_client_init_flag[QM_CLIENT_TYPE_NUM] = {0};
const uint32_t qm_client_init_magic[QM_CLIENT_TYPE_NUM] = \
{
    0x0055494D, // "UIM"
    0x48545450, // "HTTP"
};

int qmidevice_control_fd[2];
static int signal_control_fd[2];
static pthread_t gQmiThreadID = 0;
int triger_event = 0;
pthread_t qmi_recv_thread_id = 0;
PROFILE_T profile;
extern uint8_t deleteInProgress;
void qmidevice_send_event_to_manager(int triger_event) {
     if (write(qmidevice_control_fd[1], &triger_event, sizeof(triger_event)) == -1) {};
}
void qmidevice_send_event_to_manager_ext(int triger_event, void *data, unsigned len) {
     if (write(qmidevice_control_fd[1], &triger_event, sizeof(triger_event)) == -1) {};
     if (write(qmidevice_control_fd[1], data, len) == -1) {};
}

static void manager_send_event_to_qmidevice(int triger_event) {
     if (write(qmidevice_control_fd[0], &triger_event, sizeof(triger_event)) == -1) {};
}

static void send_signo_to_main(int signo) {
     if (write(signal_control_fd[0], &signo, sizeof(signo)) == -1) {};
}

static void ql_sigaction(int signo) {
     if (SIGALRM == signo)
        ;//send_signo_to_main(SIG_EVENT_START);
     else
     {
        send_signo_to_main(SIG_EVENT_STOP);
        manager_send_event_to_qmidevice(SIG_EVENT_STOP); //main may be wating qmi response
    }
}

static PQMI_TLV_HDR GetTLV (PQCQMUX_MSG_HDR pQMUXMsgHdr, int TLVType) {
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

static err_type uim_add_profile_ind_msg_decode(QCQMIMSG qmiIndMsg, uim_add_profile_ind_msg_v01 *ind )
{
    int TLVFind = 0;
    PQMI_TLV_HDR TLVHdr = NULL;
    while(1){
        TLVHdr = GetTLV(&qmiIndMsg.MUXMsg.QMUXMsgHdr, 0x1000 + (++TLVFind));
        if (TLVHdr == NULL)
            break;

        switch(TLVHdr->TLVType){
            case 0x01:
            {
                uint8_t slot = *(uint8_t*)(TLVHdr+1);
                LOG_D_TAG("slot = %d", slot);
                ind->slot = slot;
            }
                break;
            case 0x02:
            {
                uint32_t download_status = le32_to_cpu(*(uint32_t*)(TLVHdr+1));
                LOG_D_TAG("download status = %d", download_status);
                ind->profile_download_status = download_status;
            }
                break;
            case 0x10:
            {
                uint32_t error_cause = le32_to_cpu(*(uint32_t*)(TLVHdr+1));
                LOG_D_TAG("error cause = %d", error_cause);
                ind->error_cause_valid = TRUE;
                ind->error_cause = error_cause;
            }
                break;
            case 0x11:
            {
                uint8_t percentage = *(uint8_t*)(TLVHdr+1);
                LOG_D_TAG("percentage = %d", percentage);
                ind->percentage_valid = TRUE;
                ind->percentage = percentage;
            }
                break;
            case 0x12:
            {
                LOG_D_TAG("profile policy rule, TODO");
            }
                break;
            default:
                break;

        }
    }
    return EOK;
}

static err_type uim_http_transaction_ind_msg_decode(QCQMIMSG qmiIndMsg, uim_http_transaction_ind_msg_v01 *ind)
{
    int TLVFind = 0;
    PQMI_TLV_HDR TLVHdr = NULL;
    while (1) {
        TLVHdr = GetTLV(&qmiIndMsg.MUXMsg.QMUXMsgHdr, 0x1000 + (++TLVFind));
        if (TLVHdr == NULL)
            break;

        switch(TLVHdr->TLVType){
            case 0x01:
            {
                uint32_t token_id = le32_to_cpu(*(uint32_t*)(TLVHdr+1));
                LOG_D_TAG("token id = %d", token_id);
                ind->token_id = token_id;
            }
                break;
            case 0x10:
            {
                ind->url_valid = TRUE;
                memcpy(ind->url, (char*)(TLVHdr+1), TLVHdr->TLVLength);
                LOG_D_TAG("URL = %s", ind->url);
            }
                break;
            case 0x11:
            {
                // TODO
            }
                break;
            case 0x12:
            {
                ind->segment_info_valid = TRUE;
                uint32_t *u32_ptr = (uint32_t*)(TLVHdr+1);
                uint32_t total_size = le32_to_cpu(*u32_ptr);
                uint32_t offset = le32_to_cpu(*(u32_ptr+1));
                LOG_D_TAG("total size = %d, offset = %d", total_size, offset);
                ind->segment_info.total_size = total_size;
                ind->segment_info.segment_offset = offset;
            }
                break;
            case 0x13:
            {
                ind->payload_body_valid = TRUE;
                uint16_t *u16_ptr = (uint16_t*)(TLVHdr+1);
                uint16_t payload_len = le16_to_cpu(*u16_ptr);
                char *payload_body_ptr = (char*)(u16_ptr+1);
                ind->payload_body_len = payload_len;
                memcpy(ind->payload_body, payload_body_ptr, payload_len);
                LOG_D_TAG("payload len = %d, >> %s", payload_len, ind->payload_body);
            }
                break;
            default:
                break;

        }
    }  // while
    return EOK;
}

static void *qmi_recv_thread_entry(void *data)
{
    if(NULL == data){
        LOG_E_TAG("Bad Parameter(s)");
        pthread_exit(NULL);
        return NULL;
    }
    PROFILE_T *profile = (PROFILE_T *)data;
    int signo;
    LOG_D_TAG("Enter QMI Recieve Thread Main Loop");
    while (1)
    {
        struct pollfd pollfds[] = {{signal_control_fd[1], POLLIN, 0}, {qmidevice_control_fd[0], POLLIN, 0}};
        int ne, ret, nevents = sizeof(pollfds)/sizeof(pollfds[0]);

        do {
            ret = poll(pollfds, nevents,  15*1000);
        } while ((ret < 0) && (errno == EINTR));

        if (ret < 0) {
            LOG_D_TAG("poll=%d, errno: %d (%s)",  ret, errno, strerror(errno));
            LOG_D_TAG("Exit QMI Recieve Thread Main Loop");
            goto __main_quit;
        }
        if (0 == ret)
        {
            continue;
        }

        for (ne = 0; ne < nevents; ne++) {
            int fd = pollfds[ne].fd;
            short revents = pollfds[ne].revents;

            if (revents & (POLLERR | POLLHUP | POLLNVAL)) {
                LOG_D_TAG("%s poll err/hup", __func__);
                LOG_D_TAG("epoll fd = %d, events = 0x%04x", fd, revents);
                manager_send_event_to_qmidevice(RIL_REQUEST_QUIT);
                if (revents & POLLHUP){
                    LOG_D_TAG("Exit QMI Recieve Thread Main Loop");
                    goto __main_quit;
                }
            }

            if ((revents & POLLIN) == 0)
                continue;
            
            if (fd == signal_control_fd[1])
            {
                if (read(fd, &signo, sizeof(signo)) == sizeof(signo))
                {
                    alarm(0);
                    switch (signo)
                    {
                        case SIG_EVENT_STOP:
                            if (profile->qmi_ops->deinit)
                                profile->qmi_ops->deinit();
                            manager_send_event_to_qmidevice(RIL_REQUEST_QUIT);
                            LOG_D_TAG("Exit QMI Recieve Thread Main Loop");
                            goto __main_quit;
                        break;
                    }
                }
            }

            if (fd == qmidevice_control_fd[0]) {
                if (read(fd, &triger_event, sizeof(triger_event)) == sizeof(triger_event)) {
                    switch (triger_event) {
                        case MODEM_REPORT_RESET_EVENT:
                        {
                            unsigned int time_to_wait = 20;
                            unsigned int time_expired = 0;
                            LOG_D_TAG("main recv MODEM RESET SIGNAL");
                            LOG_D_TAG("quit QMI thread and wait %ds and try to restart", time_to_wait);
                            manager_send_event_to_qmidevice(RIL_REQUEST_QUIT);
                            /** NOTICE
                             * DO NOT CALL usbnet_link_change(0, profile) DIRECTLLY
                             * for, the modem may go into wrong state(only ttyUSB0 left) and wont go back
                             */
                            /* close FDs, for we want restart. */
                            close(signal_control_fd[0]);
                            close(signal_control_fd[1]);
                            close(qmidevice_control_fd[0]);
                            close(qmidevice_control_fd[1]);
                            while (time_expired++ < time_to_wait) {
                                sleep(1);
                                char qmidev[64] = {'\0'};
                                snprintf(qmidev, sizeof(qmidev), "/dev/bus/usb/%03d/%03d", profile->usb_dev.busnum, profile->usb_dev.devnum);
                                if (access(qmidev, F_OK)) {
                                    LOG_D_TAG("whoo, fatal error info, qmi device node disappeared!!! cannot continue!\n");
                                    goto __main_quit;
                                }
                            }
                            LOG_D_TAG("main try do restart");
                            goto __main_quit;
                        }
                        break;
                        case UIM_ADD_PROFILE_IND:
                        {
                            LOG_D_TAG("Receive Add Profile Indication");
                            QCQMIMSG qmiIndMsg;
                            uim_add_profile_ind_msg_v01 ind = {0};
                            if(read(fd, &qmiIndMsg, sizeof(qmiIndMsg)) == sizeof(qmiIndMsg)){
                                LOG_D_TAG("Read Add Profile Indication Successfully");
                                if(EOK != uim_add_profile_ind_msg_decode(qmiIndMsg, &ind)){
                                    LOG_E_TAG("Decode Failure, Send NULL To Indication Call Back Function");
                                    qm_client_list[QM_CLIENT_TYPE_UIM].ind_cb(QMI_UIM_ADD_PROFILE_IND_V01, NULL, 0);
                                }
                                LOG_D_TAG("Decode Successfully, Send To Indication Call Back Function");
                                qm_client_list[QM_CLIENT_TYPE_UIM].ind_cb(QMI_UIM_ADD_PROFILE_IND_V01, &ind, sizeof(ind));
                            }
                            else{
                                LOG_W_TAG("Read Add Profile Indication Failure");
                            }
                        }
                            break;
                        case UIM_HTTP_TRANSACTION_IND:
                        {
                            LOG_D_TAG("Recieve HTTP Transaction Indication");
                            QCQMIMSG qmiIndMsg;
                            uim_http_transaction_ind_msg_v01 ind = {0};
                            if(read(fd, &qmiIndMsg, sizeof(qmiIndMsg)) == sizeof(qmiIndMsg)){
                                LOG_D_TAG("Read HTTP Transaction Indication Successfully");
                                if(EOK != uim_http_transaction_ind_msg_decode(qmiIndMsg, &ind)){
                                    LOG_E_TAG("Decode Failure, Send NULL To Indication Call Back Function");
                                    qm_client_list[QM_CLIENT_TYPE_UIM_HTTP].ind_cb(QMI_UIM_HTTP_TRANSACTION_IND_V01, NULL, 0);
                                }
                                LOG_D_TAG("Decode Successfully, Send To Indication Call Back Function");
                                qm_client_list[QM_CLIENT_TYPE_UIM_HTTP].ind_cb(QMI_UIM_HTTP_TRANSACTION_IND_V01, &ind, sizeof(ind));
                            }
                            else{
                                LOG_D_TAG("Read indication failed");
                            }
                        }
                            break;
                        default:
                            break;
                    }
                }
            }
        }
    }

__main_quit:
    if (gQmiThreadID && pthread_join(gQmiThreadID, NULL)) {
        LOG_D_TAG("Error joining to gQmi thread (%s)",  strerror(errno));
    }

    close(signal_control_fd[0]);
    close(signal_control_fd[1]);
    close(qmidevice_control_fd[0]);
    close(qmidevice_control_fd[1]); 
    pthread_exit(NULL);
    return NULL;
}

static err_type qmi_main(PROFILE_T *profile)
{
    /* signal trigger quit event */
    signal(SIGINT, ql_sigaction);
    signal(SIGTERM, ql_sigaction);
    /* timer routine */
    signal(SIGALRM, ql_sigaction);

    //sudo apt-get install udhcpc
    //sudo apt-get remove ModemManager

    /* try to recreate FDs*/
    if (socketpair( AF_LOCAL, SOCK_STREAM, 0, signal_control_fd) < 0 ) {
        LOG_E_TAG("Faild to create main_control_fd: %d (%s)", errno, strerror(errno));
        return ERROR;
    }

    if ( socketpair( AF_LOCAL, SOCK_STREAM, 0, qmidevice_control_fd ) < 0 ) {
        LOG_D_TAG("Failed to create thread control socket pair: %d (%s)", errno, strerror(errno));
        return ERROR;
    }

    if (pthread_create( &gQmiThreadID, 0, profile->qmi_ops->read, (void *)profile) != 0) {
        LOG_D_TAG("Failed to create QMIThread: %d (%s)", errno, strerror(errno));
        return ERROR;
    }

    if ((read(qmidevice_control_fd[0], &triger_event, sizeof(triger_event)) != sizeof(triger_event))
        || (triger_event != RIL_INDICATE_DEVICE_CONNECTED)) {
        LOG_D_TAG("Failed to init QMIThread: %d (%s)", errno, strerror(errno));
        return ERROR;
    }

    if (profile->qmi_ops->init && profile->qmi_ops->init(profile)) {
        LOG_D_TAG("Failed to qmi init: %d (%s)", errno, strerror(errno));
        return ERROR;
    }

    if(0 != pthread_create(&qmi_recv_thread_id, 0, qmi_recv_thread_entry, (void *)profile)){
        LOG_E_TAG("Create QMI Recieve Thread Failed: %d (%s)", errno, strerror(errno));
        return ERROR;
    }

    return EOK;
}

err_type qm_client_device_init(void)
{
    err_type result = ERROR;
    const char *usbmon_logfile = NULL;
    char qmichannel[32+1] = {'\0'};
    char usbnet_adapter[32+1] = {'\0'};

    if (profile.expect_adapter[0])
        strncpy(usbnet_adapter, profile.expect_adapter, sizeof(usbnet_adapter));
    
    if (qmidevice_detect(qmichannel, usbnet_adapter, sizeof(qmichannel), &profile)){
    	profile.hardware_interface = HARDWARE_USB;
    }
    else if (mhidevice_detect(qmichannel, usbnet_adapter, &profile)){
        profile.hardware_interface = HARDWARE_PCIE;
    }
    else{
        LOG_D_TAG("detect qmi device failed");
        return ERROR;
    }

    strncpy(profile.qmichannel, qmichannel, sizeof(profile.qmichannel));
    strncpy(profile.usbnet_adapter, usbnet_adapter, sizeof(profile.usbnet_adapter));
    ql_get_netcard_driver_info(profile.usbnet_adapter);

    if((profile.hardware_interface == HARDWARE_USB) && usbmon_logfile)
        ql_capture_usbmon_log(&profile, usbmon_logfile);

    if(profile.hardware_interface == HARDWARE_USB){
        profile.software_interface = get_driver_type(&profile);
    }

    if (profile.software_interface == SOFTWARE_QMI) {
        LOG_D_TAG("module works in QMI mode");
        profile.request_ops = &qmi_request_ops;
        if (qmidev_is_gobinet(profile.qmichannel)) {
            profile.qmi_ops = &gobi_qmidev_ops;
        }
        else {
            profile.qmi_ops = &qmiwwan_qmidev_ops;
        }
        qmidev_send = profile.qmi_ops->send;
        result = qmi_main(&profile);
    }
    else {
        LOG_D_TAG("unsupport software_interface %d", profile.software_interface);
    }

    ql_stop_usbmon_log(&profile);
    if (logfilefp)
    fclose(logfilefp);
    return result;
}

static uint8_t is_qm_client_init(uint8_t client_type)
{
    if(client_type >= QM_CLIENT_TYPE_NUM){
        LOG_D_TAG("invalid client type(%d)", client_type);
        return ERROR;
    }
    return (qm_client_init_magic[client_type] == qm_client_list[client_type].init_flag) ? TRUE : FALSE;
}

err_type qm_client_init(uint8_t client_type, qm_client_ind_cb ind_cb)
{
    if(client_type >= QM_CLIENT_TYPE_NUM){
        LOG_E_TAG("Invalid Client Type(%d)", client_type);
        return ERROR;
    }
    if(NULL == ind_cb){
        LOG_E_TAG("No Indication Calback Function");
        return ERROR;
    }
    if(is_qm_client_init(client_type)){
        LOG_W_TAG("Client(%d) Already Init", client_type);
        return EOK;
    }

    qm_client_list[client_type].ind_cb = ind_cb;
    qm_client_list[client_type].init_flag = qm_client_init_magic[client_type];

    LOG_I_TAG("Init QMI Client(%d) Success", client_type);
    return EOK;
}


#define QMI_MSG_ENCODE_MAX_SIZE 5000
uint8_t qmi_msg_encode[QMI_MSG_ENCODE_MAX_SIZE];
err_type qm_client_send_msg_sync(uint8_t client_type, uint32_t msg_id, \
                             void *req_c_struct, uint32_t req_c_struct_len, \
                             void *rsp_c_struct, uint32_t rsp_c_struct_len, \
                             uint32_t timeout_ms)
{
    if(!is_qm_client_init(client_type)){
        LOG_E_TAG("Client(%d) Not Init", client_type);
        return ERROR;
    }
    int32_t result = 0;
    const struct request_ops *request_ops = profile.request_ops;
    if(request_ops->requestUimHttpTransaction && (QM_CLIENT_TYPE_UIM_HTTP == client_type)){
        LOG_D_TAG("Do UIM HTTP Transaction Request");
        result = request_ops->requestUimHttpTransaction(*(uim_http_transaction_req_msg_v01*)req_c_struct);
    }
    else if(QM_CLIENT_TYPE_UIM == client_type){
        if(request_ops->requestUimAddProfile &&
           (!lpa_deleting_status_handler(DELETING_STATUS_QUERY,DELETING_STATUS_NONE))){
            LOG_D_TAG("Do UIM Add Profile Request");
	    result = request_ops->requestUimAddProfile(*(uim_add_profile_req_msg_v01*)req_c_struct);			
        }
        else if(request_ops->requestUimDeleteProfile &&
                (lpa_deleting_status_handler(DELETING_STATUS_QUERY,DELETING_STATUS_NONE))){
            LOG_D("Do UIM Delete Profile Request");
            result = request_ops->requestUimDeleteProfile(*(uim_delete_profile_req_msg_v01*)req_c_struct);
            deleteInProgress = TRUE;
        }
    }
    LOG_D_TAG("Send Result : %d", result);
    return (0 == result) ? EOK : ERROR;
}

#elif USING_IDL_QMI
#include "qmi_client.h"
/*
**  TODO
*/

#else
#error Please choice the type of QMI
#endif
