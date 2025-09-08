#ifndef LAPD_COMMON_DEF_H
#define LAPD_COMMON_DEF_H

#include <stdint.h>
#include <stdio.h>
#include "log.h"

#define LPAD_VERSION "1.0.7"

#ifndef TRUE
#define TRUE 1
#endif // TRUE

#ifndef FALSE
#define FALSE 0
#endif // FALSE

#ifndef NULL
#define NULL ((void*)0)
#endif // NULL

#define DELETING_STATUS_QUERY 0
#define DELETING_STATUS_SET   1

#define DELETING_STATUS_NONE   0
#define DELETING_STATUS_IN_PROGRESS 1

typedef enum{
    EOK,
    ERROR,
    ETIMEOUT,
}err_type;

typedef enum{
    PROFILE_INSTALL_IDLE = 0,
    PORFILE_INSTALL_DONE = 1,
    PROFILE_INSTALL_IN_PROGRESS = 2,
}profile_install_status_type;

extern void *progress_bar_thread_entry(void *arg);
extern void progress_bar_percent_set(uint8_t percent);
extern void progress_bar_percent_add(uint8_t percent);
extern void progress_bar_flush(void);
extern void progress_bar_clear(void);
extern uint8_t lpa_deleting_status_handler(uint8_t mode, uint8_t status);

#define LOG_D_TAG(...)  do{\
    progress_bar_clear(); \
    log_args_t log_args = {.level = LOG_LEVEL_DEBUG, .tag = LOG_TAG, .file = __FILE__, .line = __LINE__}; \
    log_output(log_args,  __VA_ARGS__); \
    progress_bar_flush(); \
}while(0)

#define LOG_I_TAG(...)  do{\
    progress_bar_clear(); \
    log_args_t log_args = {.level = LOG_LEVEL_INFO, .tag = LOG_TAG, .file = __FILE__, .line = __LINE__}; \
    log_output(log_args,  __VA_ARGS__); \
    progress_bar_flush(); \
}while(0)

#define LOG_W_TAG(...)  do{\
    progress_bar_clear(); \
    log_args_t log_args = {.level = LOG_LEVEL_WARN, .tag = LOG_TAG, .file = __FILE__, .line = __LINE__}; \
    log_output(log_args,  __VA_ARGS__); \
    progress_bar_flush(); \
}while(0)

#define LOG_E_TAG(...)  do{\
    progress_bar_clear(); \
    log_args_t log_args = {.level = LOG_LEVEL_ERROR, .tag = LOG_TAG, .file = __FILE__, .line = __LINE__}; \
    log_output(log_args,  __VA_ARGS__); \
    progress_bar_flush(); \
}while(0)

#define LOG_D(fmt, args...)  do{\
    fprintf(stdout, fmt "\n", ##args); \
}while(0)

#define LOG_HEX(...)

/*
** Using legacy QMI for host device
** Using idl QMI for module
*/
#define USING_LEGACY_QMI
// #define USING_IDL_QMI

#ifdef USING_LEGACY_QMI
#define QMI_UIM_EUICC_DOWNLOAD_CODE_MAX_V01 256

#define UIM_HTTP_PAYLOAD_BODY_CHUNK_MAX_LEN_V01 2048
#define UIM_HTTP_HEADER_NAME_MAX_LEN_V01 32
#define UIM_HTTP_HEADER_VALUE_MAX_LEN_V01 256
#define UIM_HTTP_CUST_HEADER_MAX_COUNT_V01 10
#define UIM_HTTP_URL_MAX_LEN_V01 512

#define QMI_UIM_PROFILE_ID_MIN 1
#define QMI_UIM_PROFILE_ID_MAX 8

#define QMI_UIM_DELETE_PROFILE_REQ_V01 0x0066
#define QMI_UIM_DELETE_PROFILE_RESP_V01 0x0066

#define QMI_UIM_ADD_PROFILE_REQ_V01 0x0068
#define QMI_UIM_ADD_PROFILE_RESP_V01 0x0068
#define QMI_UIM_ADD_PROFILE_IND_V01 0x0068

#define QMI_UIM_HTTP_TRANSACTION_REQ_V01 0x0021
#define QMI_UIM_HTTP_TRANSACTION_RESP_V01 0x0021
#define QMI_UIM_HTTP_TRANSACTION_IND_V01 0x0021

#define UIM_HTTP_TRANSACTION_SUCCESSFUL_V01 0x00
#define UIM_HTTP_NETWORK_ERROR_V01 0x04

typedef enum {
    UIM_PROFILE_DOWNLOAD_STATUS_ENUM_TYPE_MIN_ENUM_VAL_V01 = -2147483647,
    UIM_PROFILE_DOWNLOAD_ERROR_V01 = 0x01,
    UIM_PROFILE_DOWNLOAD_IN_PROGRESS_V01 = 0x02,
    UIM_PROFILE_DOWNLOAD_COMPLETE_INSTALLATION_IN_PROGRESS_V01 = 0x03,
    UIM_PROFILE_INSTALLATION_COMPLETE_V01 = 0x04,
    UIM_PROFILE_USER_CONSENT_REQUIRED_V01 = 0x05,
    UIM_PROFILE_DOWNLOAD_STATUS_ENUM_TYPE_MAX_ENUM_VAL_V01 = 2147483647
}uim_profile_download_status_enum_type_v01;

typedef struct {
    uint8_t slot;
    uint32_t activation_code_len;
    char activation_code[QMI_UIM_EUICC_DOWNLOAD_CODE_MAX_V01];

    uint8_t confirmation_code_valid;
    uint32_t confirmation_code_len;
    char confirmation_code[QMI_UIM_EUICC_DOWNLOAD_CODE_MAX_V01];

    uint8_t user_consent_supported_valid;
    uint8_t user_consent_supported;
}uim_add_profile_req_msg_v01;

typedef struct {
    uint8_t slot;
    uint32_t profile_download_status;

    uint8_t error_cause_valid;
    uint32_t error_cause;

    uint8_t percentage_valid;
    uint8_t percentage;

    uint8_t profile_policy_rules_valid;
    struct {
        uint64_t profile_policy_mask;
        uint8_t user_consent_needed;
    }profile_policy_rules;
}uim_add_profile_ind_msg_v01;

typedef struct {
    /*  Slot */
    uint8_t slot;
    /**<   Indicates the slot for delete profile. Valid values:\n
      - UIM_SLOT_1 (0x01) --  Slot 1 \n 
      - UIM_SLOT_2 (0x02) --  Slot 2 \n 
      - UIM_SLOT_3 (0x03) --  Slot 3 \n 
      - UIM_SLOT_4 (0x04) --  Slot 4 \n 
      - UIM_SLOT_5 (0x05) --  Slot 5 
   */

    /*  Profile ID */
    uint32_t profile_id;
    /**<   Indicates the profile ID to delete. Valid values:\n
      - UIM_PROFILE_ID_1 (0x01) --   \n Profile 1 \n 
      - UIM_PROFILE_ID_2 (0x02) --   \n Profile 2 \n 
      - UIM_PROFILE_ID_3 (0x03) --   \n Profile 3 \n 
      - UIM_PROFILE_ID_4 (0x04) --   \n Profile 4 \n 
      - UIM_PROFILE_ID_5 (0x05) --   \n Profile 5 \n 
      - UIM_PROFILE_ID_6 (0x06) --  \n Profile 6 \n 
      - UIM_PROFILE_ID_7 (0x07) --  \n Profile 7 \n 
      - UIM_PROFILE_ID_8 (0x08) --  \n Profile 8 
 */
}uim_delete_profile_req_msg_v01;

typedef struct {
    int32_t result;
    uint32_t token_id;

    uint8_t headers_valid;
    uint32_t headers_len;
    struct {
        char name[UIM_HTTP_HEADER_NAME_MAX_LEN_V01 + 1];
        char value[UIM_HTTP_HEADER_VALUE_MAX_LEN_V01 + 1];
    } headers[UIM_HTTP_CUST_HEADER_MAX_COUNT_V01];

    uint8_t segment_info_valid;
    struct {
        uint32_t total_size;
        uint32_t segment_offset;
    } segment_info;

    uint8_t payload_body_valid;
    uint32_t payload_body_len;
    uint8_t payload_body[UIM_HTTP_PAYLOAD_BODY_CHUNK_MAX_LEN_V01];
}uim_http_transaction_req_msg_v01;

typedef struct {
    uint32_t token_id;

    uint8_t url_valid;
    char url[UIM_HTTP_URL_MAX_LEN_V01 + 1];

    uint8_t headers_valid;
    struct {
        char content_type[UIM_HTTP_HEADER_VALUE_MAX_LEN_V01 + 1];
        uint32_t custom_header_len;
        struct {
            char name[UIM_HTTP_HEADER_NAME_MAX_LEN_V01 + 1];
            char value[UIM_HTTP_HEADER_VALUE_MAX_LEN_V01 + 1];
        } custom_header[UIM_HTTP_CUST_HEADER_MAX_COUNT_V01];
    } headers;

    uint8_t segment_info_valid;
    struct {
        uint32_t total_size;
        uint32_t segment_offset;
    } segment_info;

    uint8_t payload_body_valid;
    uint32_t payload_body_len;
    uint8_t payload_body[UIM_HTTP_PAYLOAD_BODY_CHUNK_MAX_LEN_V01];
}uim_http_transaction_ind_msg_v01;

typedef struct {
    struct {
        uint32_t result;
        uint32_t error;
    }resp;
}qmi_common_resp_msg_v01;

typedef qmi_common_resp_msg_v01 uim_add_profile_resp_msg_v01;
typedef qmi_common_resp_msg_v01 uim_http_transaction_resp_msg_v01;
typedef qmi_common_resp_msg_v01 uim_delete_profile_resp_msg_v01;

#endif // USING_LEGACY_QMI

#endif // LAPD_COMMON_DEF_H
