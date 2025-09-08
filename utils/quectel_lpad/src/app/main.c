#define LOG_TAG "MAIN"

#include "QMIThread.h"
#include "common_def.h"
#include "qmi_manager.h"
#include "http_manager.h"
// #include <mcheck.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <getopt.h>

#define ESIM_SLOT_DEFAULT       (1)
#define SINGLE_CHUNK_MAX_SIZE   (2048)
#define ACTIVATION_CODE_MINLEN  (10)

#define LPA_ADD_PROFILE_USING_DEFAULT_SMDP "UseSMDP"

typedef struct {
    uint8_t debug;
    uim_add_profile_req_msg_v01 add_profile_req_msg;
    uim_delete_profile_req_msg_v01 delete_profile_req_msg;
    char proxy[32];
}use_args_t;

typedef struct {
    char *data;
    uint32_t total_size;
}payload_cache_t;

extern pthread_t qmi_recv_thread_id; // Extern from qmi manager
static pthread_t progress_bar_thread_id;
static pthread_t esim_notify_thread_id;    //profile install result notification timer thread

int debug_qmi = 0;
static use_args_t public_args = {0};
static uint8_t is_http_start = FALSE;

uint8_t deleteInProgress = FALSE;
uint8_t profileInstallStatus = PROFILE_INSTALL_IDLE;

extern PROFILE_T profile;

const char *CONTENT_TYPE_NAME = "Content-type";
const char *CONTENT_TYPE_VALUE = "application/json";
const char *USER_AGENT_NAME = "User-Agent";
const char *USER_AGENT_VALUE = "gsma-rsp-lpad";
const char *PROTOCOL_NAME = "X-Admin-Protocol";
const char *PROTOCOL_VALUE = "gsma/rsp/v2.2.0";

static uint8_t args_parse(int argc, char **argv, use_args_t *args_ptr)
{
    memset(args_ptr, 0, sizeof(use_args_t));
    int i = 0;
    for(i=0; i<argc; ){
        if(0 == strcmp(argv[i], "-A")){
            args_ptr->add_profile_req_msg.slot = ESIM_SLOT_DEFAULT;
            if(0 == strcmp(argv[i+1],LPA_ADD_PROFILE_USING_DEFAULT_SMDP))
            {
                LOG_D("Using default smdp to add profile");
                strncpy(args_ptr->add_profile_req_msg.activation_code, argv[i+1], QMI_UIM_EUICC_DOWNLOAD_CODE_MAX_V01);
                args_ptr->add_profile_req_msg.activation_code_len = strnlen(argv[i+1], QMI_UIM_EUICC_DOWNLOAD_CODE_MAX_V01);
                i += 2;
                profileInstallStatus = PROFILE_INSTALL_IN_PROGRESS;
                continue;
            }
            else if(ACTIVATION_CODE_MINLEN <= strlen(argv[i+1])){
                strncpy(args_ptr->add_profile_req_msg.activation_code, argv[i+1], QMI_UIM_EUICC_DOWNLOAD_CODE_MAX_V01);
                args_ptr->add_profile_req_msg.activation_code_len = strnlen(argv[i+1], QMI_UIM_EUICC_DOWNLOAD_CODE_MAX_V01);
                i += 2;				
                profileInstallStatus = PROFILE_INSTALL_IN_PROGRESS;
                continue;
            }
            else{
                LOG_D("ERROR : The activation code is too short !");				
                profileInstallStatus = PORFILE_INSTALL_DONE;
                return FALSE;
            }
        }
        if(0 == strcmp(argv[i], "-C")){
            args_ptr->add_profile_req_msg.confirmation_code_valid = TRUE;
            strncpy(args_ptr->add_profile_req_msg.confirmation_code, argv[i+1], QMI_UIM_EUICC_DOWNLOAD_CODE_MAX_V01);
            args_ptr->add_profile_req_msg.confirmation_code_len = strnlen(argv[i+1], QMI_UIM_EUICC_DOWNLOAD_CODE_MAX_V01);
            i += 2;
            continue;
        }
        if(0 == strcmp(argv[i], "-R")){
            args_ptr->delete_profile_req_msg.slot = ESIM_SLOT_DEFAULT;
            args_ptr->delete_profile_req_msg.profile_id = atoi(argv[i+1]);
            if(QMI_UIM_PROFILE_ID_MIN > args_ptr->delete_profile_req_msg.profile_id ||
                QMI_UIM_PROFILE_ID_MAX < args_ptr->delete_profile_req_msg.profile_id){
                LOG_D("ERROR:Profile id is invalid !");
                return FALSE;
            }
            lpa_deleting_status_handler(DELETING_STATUS_SET,DELETING_STATUS_IN_PROGRESS);
            i += 2;
            continue;
        }
        if(0 == strcmp(argv[i], "-D")){
            int log_level = atoi(argv[i+1]);
            if((LOG_LEVEL_MIN <= log_level) && (log_level <= LOG_LEVEL_MAX)){
                args_ptr->debug = log_level;
                i += 2;
                continue;
            }
            else{
                LOG_D("ERROR : The input log level is invalid !");
                return FALSE;
            }
        }
        if(0 == strcmp(argv[i], "-P")){
            char* arg_tmp = argv[i+1];
            LOG_D("argv: %s!", arg_tmp);

            if (!strcmp(arg_tmp, QUECTEL_QMI_PROXY) || !strcmp(arg_tmp, QUECTEL_MBIM_PROXY)
                || !strcmp(arg_tmp, LIBQMI_PROXY) || !strcmp(arg_tmp, LIBMBIM_PROXY)) {
                
                strncpy(args_ptr->proxy, arg_tmp, sizeof(args_ptr->proxy) - 1);
                i += 2;
                continue;
            }
            else{
                LOG_D("ERROR : The input qmi-proxy is invalid !");
                return FALSE;
            }
        }
        if(0 == strcmp(argv[i], "-i")){
	    strncpy(profile.expect_adapter, argv[i+1], sizeof(profile.expect_adapter) - 1);
        }			
        i++;
    }
    if(!lpa_deleting_status_handler(DELETING_STATUS_QUERY,DELETING_STATUS_NONE)){
	if((ACTIVATION_CODE_MINLEN > strlen(args_ptr->add_profile_req_msg.activation_code)) &&
	    (0 != strcmp(args_ptr->add_profile_req_msg.activation_code,LPA_ADD_PROFILE_USING_DEFAULT_SMDP))){
	    LOG_D("ERROR : Not find the activation code !");
	    return FALSE;
	}
    }
    return TRUE;
}

static void args_dump(use_args_t args)
{
    uim_add_profile_req_msg_v01 *add_profile_msg_ptr = &args.add_profile_req_msg;
    uim_delete_profile_req_msg_v01 *delete_profile_msg_ptr = &args.delete_profile_req_msg;
    LOG_D("Log level         : %d", args.debug);
    LOG_D("########## Add profile ##########");
    LOG_D("SIM slot          : %d", add_profile_msg_ptr->slot);
    LOG_D("Activation code   : %s", add_profile_msg_ptr->activation_code);
    LOG_D("Confirmation code : %s", add_profile_msg_ptr->confirmation_code_valid ? \
                                    add_profile_msg_ptr->confirmation_code : "");
    LOG_D("User consent      : %d", add_profile_msg_ptr->user_consent_supported_valid ? \
                                    add_profile_msg_ptr->user_consent_supported : 0);
    LOG_D("########## Delete profile ##########");
    LOG_D("SIM slot          : %d", delete_profile_msg_ptr->slot);
    LOG_D("Delete profile id : %d", delete_profile_msg_ptr->profile_id);
}

static void usage(void)
{
    LOG_D("\nUsage: quectel_lpad [OPTIONS]");
    LOG_D("\nOptions:");
    LOG_D("-D          Set log level [0--Disable log(default), 1--Debug, 2--Infor, 3--Warn, 4--Error]");
    // LOG_D("-S          Slot");
    LOG_D("-A          Activation code");
    LOG_D("-C          Confirmation code");
    // LOG_D("-U          User consent");
    LOG_D("-R          Profile id");
    LOG_D("-I          Iface");
    LOG_D("\nExample:");
    LOG_D("########## Add profile ##########");
    LOG_D("quectel_lpad -A 123");
    LOG_D("########## Delete profile ##########");
    LOG_D("quectel_lpad -R 1");
}

static void show_profile_install_success(void)
{
    printf("\033[0;32m"); // Green
    printf("##################################################################################################\n");
    printf("#                    __ _ _        _           _        _ _                                      #\n");
    printf("#   _ __  _ __ ___  / _(_) | ___  (_)_ __  ___| |_ __ _| | |   ___ _   _  ___ ___ ___  ___ ___   #\n");
    printf("#  | '_ \\| '__/ _ \\| |_| | |/ _ \\ | | '_ \\/ __| __/ _` | | |  / __| | | |/ __/ __/ _ \\/ __/ __|  #\n");
    printf("#  | |_) | | | (_) |  _| | |  __/ | | | | \\__ \\ || (_| | | |  \\__ \\ |_| | (_| (_|  __/\\__ \\__ \\  #\n");
    printf("#  | .__/|_|  \\___/|_| |_|_|\\___| |_|_| |_|___/\\__\\__,_|_|_|  |___/\\__,_|\\___\\___\\___||___/___/  #\n");
    printf("#  |_|                                                                                           #\n");
    printf("#                                                                                                #\n");
    printf("##################################################################################################\n");
    printf("\033[m"); // Clear Green
}

static void show_profile_download_error(void)
{
    printf("\033[0;31m"); // Red
    printf("################################################################################################################\n");
    printf("#                    __ _ _            _                     _                 _                               #\n");
    printf("#   _ __  _ __ ___  / _(_) | ___    __| | _____      ___ __ | | ___   __ _  __| |    ___ _ __ _ __ ___  _ __   #\n");
    printf("#  | '_ \\| '__/ _ \\| |_| | |/ _ \\  / _` |/ _ \\ \\ /\\ / / '_ \\| |/ _ \\ / _` |/ _` |   / _ \\ '__| '__/ _ \\| '__|  #\n");
    printf("#  | |_) | | | (_) |  _| | |  __/ | (_| | (_) \\ V  V /| | | | | (_) | (_| | (_| |  |  __/ |  | | | (_) | |     #\n");
    printf("#  | .__/|_|  \\___/|_| |_|_|\\___|  \\__,_|\\___/ \\_/\\_/ |_| |_|_|\\___/ \\__,_|\\__,_|   \\___|_|  |_|  \\___/|_|     #\n");
    printf("#  |_|                                                                                                         #\n");
    printf("#                                                                                                              #\n");
    printf("################################################################################################################\n");
    printf("\033[m"); // Clear Red
}

uint8_t lpa_deleting_status_handler(uint8_t mode,uint8_t status){
    static uint8_t deleting_status = DELETING_STATUS_NONE;
    if(DELETING_STATUS_SET == mode){
        deleting_status = status;
    }
    return deleting_status;
}

void *esim_profile_download_notification_handler(void *arg)
{
    int retime = 10;
    for(retime = 10;retime > 0 ; retime--)
    {
        LOG_D("Notification is being sent, remaining time: %d s......",retime);
        sleep(1);
    }
    LOG_D("Timer expired,exit......");
    exit(0);
}

err_type lpa_http_req_error_handler(uint32_t token_id)
{
    uim_http_transaction_req_msg_v01 qmi_http_req;
    uim_http_transaction_resp_msg_v01 qmi_http_resp;
    memset(&qmi_http_req, 0, sizeof(qmi_http_req));
    memset(&qmi_http_resp, 0, sizeof(qmi_http_resp));
    qmi_http_req.token_id = token_id;
    qmi_http_req.result = UIM_HTTP_NETWORK_ERROR_V01;

    if(EOK != qm_client_send_msg_sync(QM_CLIENT_TYPE_UIM_HTTP, \
                                        QMI_UIM_HTTP_TRANSACTION_REQ_V01, \
                                        &qmi_http_req, \
                                        sizeof(qmi_http_req), \
                                        &qmi_http_resp, \
                                        sizeof(qmi_http_resp), \
                                        0))
    {
        LOG_E_TAG("Send QMI Message Failure");
        return ERROR;
    }
    return EOK;
}

static void uim_qmi_ind_cb(uint32_t msg_id, void *ind_buff, uint32_t ind_buff_len)
{
    if(lpa_deleting_status_handler(DELETING_STATUS_QUERY,DELETING_STATUS_NONE)){
        return;
    }
    if((NULL == ind_buff) || (sizeof(uim_add_profile_ind_msg_v01) != ind_buff_len)){
        LOG_E_TAG("Bad Parameter(s)");
        show_profile_download_error();
        exit(1);
    }
    uim_add_profile_ind_msg_v01 *ind = (uim_add_profile_ind_msg_v01 *)ind_buff;
    LOG_D_TAG("SLOT : %d", ind->slot);
    LOG_D_TAG("DOWNLOAD STATUS: %d", ind->profile_download_status);

    switch(ind->profile_download_status){
        case UIM_PROFILE_DOWNLOAD_ERROR_V01:
        case UIM_PROFILE_INSTALLATION_COMPLETE_V01:
        {
            int ret = -1;
		
            if(UIM_PROFILE_DOWNLOAD_ERROR_V01 == ind->profile_download_status){
                if(ind->error_cause_valid){
                    LOG_D_TAG("ERROR CAUSE: %d",ind->error_cause);
                }
                if(!is_http_start){
                    LOG_W_TAG("It Could Be The Result Of The Past, Ignore It");
                    break;
                }
                else{
                    LOG_E_TAG("Add profile QMI ERROR");
                    show_profile_download_error();
                    profileInstallStatus = PORFILE_INSTALL_DONE;
                }
            }
            else if(UIM_PROFILE_INSTALLATION_COMPLETE_V01 == ind->profile_download_status){
                progress_bar_percent_set(100);
                progress_bar_clear();
                progress_bar_flush();
                show_profile_install_success();
                profileInstallStatus = PORFILE_INSTALL_DONE;
            }
		
            ret = pthread_create(&esim_notify_thread_id, NULL, esim_profile_download_notification_handler, NULL);
		
            if (ret != 0){
                LOG_D("Error creating esim_notify_thread\n");
                exit(1);
            }	
            break;
        }
        case UIM_PROFILE_DOWNLOAD_IN_PROGRESS_V01:
            // No Break
        case UIM_PROFILE_DOWNLOAD_COMPLETE_INSTALLATION_IN_PROGRESS_V01:
            if(ind->percentage_valid){
                LOG_D_TAG("PERCENTAGE: %d", ind->percentage);
            }
            break;
        case UIM_PROFILE_USER_CONSENT_REQUIRED_V01:
            break;
        default:
            break;
    }
}

static void uim_http_qmi_ind_cb(uint32_t msg_id, void *ind_buff, uint32_t ind_buff_len)
{
    if((NULL == ind_buff) || (sizeof(uim_http_transaction_ind_msg_v01) != ind_buff_len)){
        LOG_E_TAG("Bad Parameter(s)");
        if(!lpa_deleting_status_handler(DELETING_STATUS_QUERY,DELETING_STATUS_NONE))
        show_profile_download_error();
        exit(1);
    }
    is_http_start = TRUE;
    static uim_http_transaction_ind_msg_v01 ind_cache = {0};
    static payload_cache_t payload_cache = {0};
    uim_http_transaction_ind_msg_v01 *ind = (uim_http_transaction_ind_msg_v01*)ind_buff;
    LOG_D_TAG("TOKEN ID : %d", ind->token_id);
    // New Session
    if(ind->token_id != ind_cache.token_id){
        // if((!ind->url_valid) || (!ind->headers_valid)){
        if(!ind->url_valid){
            LOG_E_TAG("Miss URL Or Headers In First Chunk");
            return;
        }

        LOG_D_TAG("Cache Token Id : %d, Clean Cache", ind_cache.token_id);
        memset(&ind_cache, 0, sizeof(ind_cache));
        if(NULL != payload_cache.data){
            free(payload_cache.data);
            payload_cache.data = NULL;
            payload_cache.total_size = 0;
        }
        // Cache
        ind_cache.token_id = ind->token_id;
        memcpy(ind_cache.url, ind->url, strlen(ind->url));
        ind_cache.url_valid = TRUE;
        ind_cache.headers = ind->headers;
        ind_cache.headers_valid = TRUE;
    }
    if(!lpa_deleting_status_handler(DELETING_STATUS_QUERY,DELETING_STATUS_NONE))
    progress_bar_percent_add(12);
    uint32_t total_size = 0;
    uint32_t offset = 0;
    if(ind->segment_info_valid){
        total_size = ind->segment_info.total_size;
        offset = ind->segment_info.segment_offset;
        LOG_D_TAG("SEGMENT TOTAL SIZE : %d, OFFSET : %d", total_size, offset);
        // Recieve new transaction
        if((0 == payload_cache.total_size) && (NULL == payload_cache.data)){
            payload_cache.data = malloc(total_size + 1);
            if(NULL != payload_cache.data){
                LOG_D_TAG("Memory Alloc Successfully, Size = %d", total_size);
                memset(payload_cache.data, 0, total_size + 1);
                payload_cache.total_size = total_size;
            }
            else{
                LOG_E_TAG("Memory Alloc Failure");
                return;
            }
        }
        else if((NULL != payload_cache.data) && (payload_cache.total_size == total_size)){
            LOG_D_TAG("Same Session, New chunk");
        }
        else{
            LOG_E_TAG("Unknown Condition");
            return;
        }
    }

    if(ind->payload_body_valid){
        LOG_D_TAG("PAYLOAD BODY LEN : %d, PAYLOAD BODY : %s", ind->payload_body_len, ind->payload_body);
        if(payload_cache.total_size < (offset + ind->payload_body_len)){
            LOG_E_TAG("Wrong Length");
            return;
        }
        memcpy(&payload_cache.data[offset], ind->payload_body, ind->payload_body_len);
        if(payload_cache.total_size == (offset + ind->payload_body_len)){
            LOG_D_TAG("Recv Total Payload");
            http_manager_req_msg_t http_req;
            char *http_resp = NULL;
            long http_code = 0;
            http_req.data = payload_cache.data;
            http_req.url = ind_cache.url;
            http_req.header_num = 3;
            http_req.headers[0].name = (char*)CONTENT_TYPE_NAME;
            http_req.headers[0].value = (char*)CONTENT_TYPE_VALUE;
            http_req.headers[1].name = (char*)USER_AGENT_NAME;
            http_req.headers[1].value = (char*)USER_AGENT_VALUE;
            http_req.headers[2].name = (char*)PROTOCOL_NAME;
            http_req.headers[2].value = (char*)PROTOCOL_VALUE;
            http_resp = http_manager_req(http_req, &http_code);
            uint8_t is_http_ok = FALSE;
            switch(http_code)
            {
                case 204:   // HTTP OK but No Data
                    http_manager_req_cleanup();
                    if(deleteInProgress && (lpa_deleting_status_handler(DELETING_STATUS_QUERY,DELETING_STATUS_NONE))){
                        LOG_D("\033[32mProfile is deleted and smdp+ is notified of success!\033[0m");
                        deleteInProgress = FALSE;
                        lpa_deleting_status_handler(DELETING_STATUS_SET,DELETING_STATUS_NONE);
                        exit(0);
                    }
                    else if(PORFILE_INSTALL_DONE == profileInstallStatus){
                        LOG_D("\033[32mThe result of profile installation notifies smdp+ success!\033[0m");
                        profileInstallStatus = PROFILE_INSTALL_IDLE;
                        exit(0);
                    }
                    return;
                case 200:
                    if(NULL != http_resp){
                        is_http_ok = TRUE;
                    }
                    else{
                        LOG_E_TAG("HTTP Response NULL");
                    }
                    break;
                default:
                    LOG_E_TAG("Bad HTTP Code = %ld", http_code);
                    break;
            }
	    if(lpa_deleting_status_handler(DELETING_STATUS_QUERY,DELETING_STATUS_NONE)){
	        LOG_D("\033[31mdelete profile notification smdp+ failed!\033[0m");
	        http_manager_req_cleanup();
	        lpa_deleting_status_handler(DELETING_STATUS_SET,DELETING_STATUS_NONE);
	        exit(1);
	    }
            if(!is_http_ok){
                if(!http_code){
                    lpa_http_req_error_handler(ind_cache.token_id);
                }
                http_manager_req_cleanup();
                show_profile_download_error();
                exit(1);
            }
            uim_http_transaction_req_msg_v01 qmi_http_req;
            uim_http_transaction_resp_msg_v01 qmi_http_resp;
            memset(&qmi_http_req, 0, sizeof(qmi_http_req));
            memset(&qmi_http_resp, 0, sizeof(qmi_http_resp));
            qmi_http_req.token_id = ind_cache.token_id;
            qmi_http_req.result = UIM_HTTP_TRANSACTION_SUCCESSFUL_V01;
            qmi_http_req.headers_len = 2;
            memcpy(qmi_http_req.headers[0].name, USER_AGENT_NAME, strlen(USER_AGENT_NAME));
            memcpy(qmi_http_req.headers[0].value, USER_AGENT_VALUE, strlen(USER_AGENT_VALUE));
            memcpy(qmi_http_req.headers[1].name, PROTOCOL_NAME, strlen(PROTOCOL_NAME));
            memcpy(qmi_http_req.headers[1].value, PROTOCOL_VALUE, strlen(PROTOCOL_VALUE));
            uint16_t http_resp_len = strlen(http_resp);
            uint8_t total_chunk = (http_resp_len -1) / SINGLE_CHUNK_MAX_SIZE + 1;
            LOG_D_TAG("HTTP Response Length = %d, Total Chunk = %d", http_resp_len, total_chunk);
            uint8_t current_chunk = 1;
            uint16_t current_payload_len = 0;
            uint16_t current_offset = 0;
            while(current_chunk <= total_chunk){
                if(1 == current_chunk){
                    qmi_http_req.headers_valid = TRUE;
                }
                qmi_http_req.segment_info_valid = TRUE;
                qmi_http_req.segment_info.total_size = http_resp_len;
                qmi_http_req.segment_info.segment_offset = current_offset = (current_chunk - 1) * SINGLE_CHUNK_MAX_SIZE;

                if(current_chunk == total_chunk){
                    current_payload_len = http_resp_len % SINGLE_CHUNK_MAX_SIZE;
                }
                else
                {
                    current_payload_len = SINGLE_CHUNK_MAX_SIZE;
                }
                qmi_http_req.payload_body_len = current_payload_len;
                memcpy(qmi_http_req.payload_body, &http_resp[current_offset], current_payload_len);
                qmi_http_req.payload_body_valid = TRUE;
                if(EOK != qm_client_send_msg_sync(QM_CLIENT_TYPE_UIM_HTTP, \
                                                    QMI_UIM_HTTP_TRANSACTION_REQ_V01, \
                                                    &qmi_http_req, \
                                                    sizeof(qmi_http_req), \
                                                    &qmi_http_resp, \
                                                    sizeof(qmi_http_resp), \
                                                    0))
                {
                    LOG_E_TAG("Send QMI Message Failure");
                    http_manager_req_cleanup();
                    show_profile_download_error();
                    exit(1);
                }
                                        
                current_chunk++;
            }
            http_manager_req_cleanup();
            progress_bar_percent_add(15);
        }
    }
}

int main(int argc, char **argv)
{
    // setenv("mtrace", "output", 1);
    // mtrace();
    LOG_D("Quctel LPAd Version: %s", LPAD_VERSION);

    // Parse arguments
    use_args_t *args_ptr = &public_args;
    if(TRUE != args_parse(argc, argv, args_ptr)){
        usage();
        exit(0);
    }
    args_dump(*args_ptr);

    (void)strncpy(profile.proxy, args_ptr->proxy, sizeof(profile.proxy) - 1);

    /* Set Global Log Level */
    log_level_set(args_ptr->debug);
    if(!lpa_deleting_status_handler(DELETING_STATUS_QUERY,DELETING_STATUS_NONE)){
        // Progress Bar Init
        if(0 != pthread_create(&progress_bar_thread_id, NULL, progress_bar_thread_entry, NULL)){
            LOG_E_TAG("Init Progress Bar Failed");
            exit(1);
        }
    }
    // QMI Client Init
    if(EOK != qm_client_device_init()){
        LOG_E_TAG("Init QMI Client Device Failed");
        exit(1);
    }
    if((EOK != qm_client_init(QM_CLIENT_TYPE_UIM, uim_qmi_ind_cb)) || \
       (EOK != qm_client_init(QM_CLIENT_TYPE_UIM_HTTP, uim_http_qmi_ind_cb))){
        LOG_E_TAG("Init QMI Client Failure");
        exit(1);
    }

    // Send Empty Message For Register
    uim_http_transaction_req_msg_v01 http_req;
    uim_http_transaction_resp_msg_v01 http_rsp;
    memset(&http_req, 0, sizeof(http_req));
    memset(&http_rsp, 0, sizeof(http_rsp));
    if(EOK != qm_client_send_msg_sync(QM_CLIENT_TYPE_UIM_HTTP, QMI_UIM_HTTP_TRANSACTION_REQ_V01, &http_req, sizeof(http_req), &http_rsp, sizeof(http_rsp), 0)){
        LOG_W_TAG("Send QMI UIM HTTP Message Failure, But It Matched Expectation");
    }
    //while(1);
    if(lpa_deleting_status_handler(DELETING_STATUS_QUERY,DELETING_STATUS_NONE)){
        uim_delete_profile_req_msg_v01 uim_req = public_args.delete_profile_req_msg;
        uim_delete_profile_resp_msg_v01 uim_rsp;
        memset(&uim_rsp, 0, sizeof(uim_rsp));
        uim_req.slot = ESIM_SLOT_DEFAULT;
        if(EOK != qm_client_send_msg_sync(QM_CLIENT_TYPE_UIM, QMI_UIM_DELETE_PROFILE_REQ_V01, &uim_req, sizeof(uim_req), &uim_rsp, sizeof(uim_rsp), 0)){
            LOG_E_TAG("Send UIM Delete Profile Message Failure");
            exit(1);
        }
    }
    else{
        uim_add_profile_req_msg_v01 uim_req = public_args.add_profile_req_msg;
        uim_add_profile_resp_msg_v01 uim_rsp;
        memset(&uim_rsp, 0, sizeof(uim_rsp));
        uim_req.slot = ESIM_SLOT_DEFAULT;
        if(EOK != qm_client_send_msg_sync(QM_CLIENT_TYPE_UIM, QMI_UIM_ADD_PROFILE_REQ_V01, &uim_req, sizeof(uim_req), &uim_rsp, sizeof(uim_rsp), 0)){
            LOG_E_TAG("Send UIM Add Profile Message Failure");
            exit(1);
        }
        progress_bar_percent_set(10);
        if(progress_bar_thread_id && pthread_join(progress_bar_thread_id, NULL)){
            LOG_D_TAG("Error joining to qmi recv thread");
            exit(1);
        }
    }
    if(qmi_recv_thread_id && pthread_join(qmi_recv_thread_id, NULL)){
        LOG_D_TAG("Error joining to qmi recv thread");
        exit(1);
    }
    if(esim_notify_thread_id && pthread_join(esim_notify_thread_id, NULL)){
        LOG_D_TAG("Error joining to qmi recv thread");
        exit(1);
    }
    exit(0);
}
