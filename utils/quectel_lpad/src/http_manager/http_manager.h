#ifndef HTTP_MANAGER_H
#define HTTP_MANAGER_H

#include "common_def.h"
#define HTTP_HEADER_NUM_MAX     (10)
#define HTTP_HEADER_SIZE_MAX    (50)

typedef struct{
    char *url;
    uint8_t header_num;
    struct{
        char *name;
        char *value;
    }headers[HTTP_HEADER_NUM_MAX];
    char *data;
}http_manager_req_msg_t;

/*
** Usage:
**
**      1. Init http manager by call http_manager_init();
**      2. Send http request by call http_manager_req(req, resp);
**      3. End http request by call http_manager_req_cleanup();
**      4. Deinit http manager by call http_manager_deinit();
**
** Warning:
**
**      http_manager_init() and http_manager_deinit() must be used in pairs.
**      http_manager_req() and http_manager_req_cleanup() must be used in pairs.
*/
err_type http_manager_init(void);
err_type http_manager_deinit(void);
char *http_manager_req(http_manager_req_msg_t req, long *code);
void http_manager_req_cleanup(void);

#endif // HTTP_MANAGER_H
