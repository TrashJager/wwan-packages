/*
**  This QMI manager only manages UIM and UIM HTTP, 
**  specifically QMI_UIM_ADD_PROFILE and QMI_UIM_HTTP_TRANSACTION
*/

#ifndef QMI_MANAGER_H
#define QMI_MANAGER_H

#include "common_def.h"

#define QM_CLIENT_TYPE_UIM          0
#define QM_CLIENT_TYPE_UIM_HTTP     1
#define QM_CLIENT_TYPE_NUM          2

typedef void (*qm_client_ind_cb)(uint32_t msg_id, void *ind_buff, uint32_t ind_buff_len);

err_type qm_client_init(uint8_t client_type, qm_client_ind_cb ind_cb);
err_type qm_client_send_msg_sync(uint8_t client_type, uint32_t msg_id, \
                             void *req_c_struct, uint32_t req_c_struct_len, \
                             void *rsp_c_struct, uint32_t rsp_c_struct_len, \
                             uint32_t timeout_ms);
err_type qm_client_device_init(void);

#endif // QMI_MANAGER_H
