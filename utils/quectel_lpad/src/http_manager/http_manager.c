#define LOG_TAG "HTTPM"
#include "http_manager.h"
#include "curl/curl.h"
#include <stdlib.h>
#include <string.h>

#define USER_CURLOPT_SSL_VERIFYPEER 0   // FALSE
#define USER_CURLOPT_SSL_VERIFYHOST 0   // FALSE
#define USER_CURLOPT_VERBOSE        0   // FALSE
#define USER_CURLOPT_POST           1
#define USER_CURLOPT_NOSIGNAL       1
#define USER_CURLOPT_CONNECTTIMEOUT 30  // 30s
#define USER_CURLOPT_TIMEOUT        30  // 30s
#define USER_CURLOPT_IPRESOLVE      CURL_IPRESOLVE_V4

typedef struct{
    char *data;
    size_t size;
}resp_struct_t;

typedef struct
{
    CURL *curl_ptr;
    resp_struct_t resp_struct;
    struct curl_slist *headers;
}http_manager_obj_t;

static http_manager_obj_t http_obj = {0};

static err_type _http_req_msg_check(http_manager_req_msg_t req)
{
    if((NULL == req.url) || (0 == req.header_num) || (HTTP_HEADER_NUM_MAX < req.header_num) || (NULL == req.data)){
        LOG_E_TAG("Bad Parameter(s)");
        return ERROR;
    }
    uint8_t i = 0;
    for(i=0; i<req.header_num; i++){
        if((NULL == req.headers[i].name) || (NULL == req.headers[i].value)){
            LOG_E_TAG("Bad Header, No.%d", i+1);
            return ERROR;
        }
    }

    LOG_I_TAG("Success");
    return EOK;
}

static err_type _http_session_open(http_manager_obj_t *http_obj_ptr)
{
    if(NULL == http_obj_ptr){
        LOG_E_TAG("Bad Parameter(s)");
        return ERROR;
    }
    CURL *curl_ptr = curl_easy_init();
    if(NULL == curl_ptr){
        LOG_E_TAG("Open HTTP Session Failed");
        return ERROR;
    }
    http_obj_ptr->curl_ptr = curl_ptr;

    curl_version_info_data *info_ptr = curl_version_info(CURLVERSION_NOW);
    if(NULL == info_ptr){
        LOG_E_TAG("Get CURL Version Information Failed");
        return ERROR;
    }
    LOG_D_TAG("CURL Version:        %s", info_ptr->version ? info_ptr->version : "NULL");
    LOG_D_TAG("CURL Version Number: 0x%06X", info_ptr->version_num);
    LOG_D_TAG("CURL Host:           %s", info_ptr->host ? info_ptr->host : "NULL");
    LOG_D_TAG("CURL SSL Version:    %s", info_ptr->ssl_version ? info_ptr->ssl_version : "NULL");

    if(info_ptr->version_num < 0x072200){ //  version > 7.34.0
        LOG_E_TAG("CURL Version Unsupported!");
        return ERROR;
    }

    if(CURLE_OK != curl_easy_setopt(curl_ptr, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2)){
        LOG_E_TAG("Set CURL TLS Version Failed!");
        return ERROR;
    }

    CURLcode code_ssl_verifypeer = curl_easy_setopt(curl_ptr, CURLOPT_SSL_VERIFYPEER, USER_CURLOPT_SSL_VERIFYPEER);
    CURLcode code_ssl_verifyhost = curl_easy_setopt(curl_ptr, CURLOPT_SSL_VERIFYHOST, USER_CURLOPT_SSL_VERIFYHOST);
    CURLcode code_verbose = curl_easy_setopt(curl_ptr, CURLOPT_VERBOSE, USER_CURLOPT_VERBOSE);

    if((CURLE_OK != code_ssl_verifypeer) || (CURLE_OK != code_ssl_verifyhost) || (CURLE_OK != code_verbose)){
        LOG_E_TAG("Set CURL SSL Verify Option Failed!");
        return ERROR;
    }

    LOG_I_TAG("Success");
    return EOK;
}

static err_type _http_req_msg_make(http_manager_req_msg_t req, http_manager_obj_t *obj_ptr)
{
    if((NULL == obj_ptr) || (NULL ==obj_ptr->curl_ptr)){
        LOG_E_TAG("Bad Parameter(s)");
        return ERROR;
    }
    CURL *curl_ptr = obj_ptr->curl_ptr;
    struct curl_slist *curl_headers = NULL;
    // Feel free to use request message because it has been checked
    CURLcode code = CURLE_OK;
    // Set URL
    LOG_D_TAG("Set HTTP URL: %s", req.url);
    code = curl_easy_setopt(curl_ptr, CURLOPT_URL, req.url);
    if(CURLE_OK != code){
            LOG_E_TAG("CURL: (%d) %s", code, curl_easy_strerror(code));
        return ERROR;
    }
    // Set Headers
    char http_header[HTTP_HEADER_SIZE_MAX] = {0};
    uint8_t headers_valid = TRUE;
    uint8_t i = 0;
    for(i=0; i<req.header_num; i++){
        if(0 > snprintf(http_header, HTTP_HEADER_SIZE_MAX, "%s:%s", req.headers[i].name, req.headers[i].value)){
            headers_valid = FALSE;
            break;  // Direct return may lead to memory leak, curl_headers need to be saved.
        }
        // else
        LOG_D_TAG("Set HTTP Header: %s", http_header);
        curl_headers = curl_slist_append(curl_headers, http_header);
    }
    obj_ptr->headers = curl_headers;
    if(FALSE == headers_valid){
        LOG_E_TAG("Bad Header [%d], Name Length = %ld, Value Length = %ld", i+1, \
                  strlen(req.headers[i].name), strlen(req.headers[i].value));
        return ERROR;
    }
    code = curl_easy_setopt(curl_ptr, CURLOPT_HTTPHEADER, curl_headers);
    if(CURLE_OK != code){
        LOG_E_TAG("CURL: (%d) %s", code, curl_easy_strerror(code));
        return ERROR;
    }
    // Set Data
    LOG_D_TAG("Set HTTP Data: %s", req.data);
    code = curl_easy_setopt(curl_ptr, CURLOPT_POSTFIELDS, req.data);
    if(CURLE_OK != code){
        LOG_E_TAG("CURL: (%d) %s", code, curl_easy_strerror(code));
        return ERROR;
    }

    LOG_I_TAG("Success");
    return EOK;
}

static size_t _user_write_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
    LOG_D_TAG("Entry User Write Response Call Back");
    size_t realsize = size * nmemb;

    if (NULL == contents || NULL == userp)
    {
        LOG_E_TAG("Bad Parameter(s)");
        return 0;
    }

    resp_struct_t *resp_ptr = (resp_struct_t *)userp;
    void* ptrRealloc = realloc(resp_ptr->data, resp_ptr->size + realsize + 1);
    if (NULL == ptrRealloc)
    {
        LOG_E_TAG("Not Enough Memory (realloc returned NULL)");
        return 0;
    }

    resp_ptr->data = ptrRealloc;
    memcpy(&(resp_ptr->data[resp_ptr->size]), contents, realsize);
    resp_ptr->size += realsize;
    resp_ptr->data[resp_ptr->size] = 0;
    LOG_D_TAG("Exit User Write Resp Call Back, Realloc Size = %ld, Total Size = %ld", realsize, resp_ptr->size);
    return realsize;
}

static err_type _http_req_config(CURL *curl_ptr, void *wdata_cb)
{
    if(NULL == curl_ptr){
        LOG_E_TAG("Bad Parameter(s)");
        return ERROR;
    }

    CURLcode code = curl_easy_setopt(curl_ptr, CURLOPT_POST, USER_CURLOPT_POST);
    // If code == CURLERROR, then go straight to the end. The same below.
    if(CURLE_OK == code){
        code = curl_easy_setopt(curl_ptr, CURLOPT_NOSIGNAL, USER_CURLOPT_NOSIGNAL);
    }

    if(CURLE_OK == code){
        code = curl_easy_setopt(curl_ptr, CURLOPT_CONNECTTIMEOUT, USER_CURLOPT_CONNECTTIMEOUT);
    }

    if(CURLE_OK == code){
        code = curl_easy_setopt(curl_ptr, CURLOPT_TIMEOUT, USER_CURLOPT_TIMEOUT);
    }

    if(CURLE_OK == code){
		code = curl_easy_setopt(curl_ptr, CURLOPT_WRITEFUNCTION, _user_write_callback);
    }

    if(CURLE_OK == code){
        code = curl_easy_setopt(curl_ptr, CURLOPT_WRITEDATA, wdata_cb);
    }

    if(CURLE_OK == code){
        code = curl_easy_setopt(curl_ptr, CURLOPT_IPRESOLVE, USER_CURLOPT_IPRESOLVE);
    }

    if(CURLE_OK == code){
        LOG_I_TAG("Success");
        return EOK;
    }
  
    LOG_E_TAG("CURL: (%d) %s", code, curl_easy_strerror(code));
    return ERROR;
}

// static err_type _http_req_cert_inject()
static err_type _http_post(CURL *curl_ptr, long *http_code)
{
    LOG_D_TAG("Start HTTP Post ......");
    CURLcode code = curl_easy_perform(curl_ptr);
    if(CURLE_OK != code){
        LOG_E_TAG("CURL: (%d) %s", code, curl_easy_strerror(code));
        return ERROR;
    }
    
    if(CURLE_OK != curl_easy_getinfo(curl_ptr, CURLINFO_RESPONSE_CODE, http_code)){
        LOG_E_TAG("Get HTTP Resp Code Failed!");
        return ERROR;
    }
    LOG_D_TAG("Get HTTP Resp Code : %ld", *http_code);

    LOG_I_TAG("Success");
    return EOK;
}

err_type http_manager_init(void)
{
    CURLcode code = curl_global_init(CURL_GLOBAL_ALL);
    if(CURLE_OK != code){
        LOG_E_TAG("CURL: (%d) %s", code, curl_easy_strerror(code));
        return ERROR;
    }

    LOG_I_TAG("Success");
    return EOK;
}

err_type http_manager_deinit(void)
{
    curl_global_cleanup();
    LOG_D_TAG("Return OK");
    return EOK;
}

char *http_manager_req(http_manager_req_msg_t req, long *http_code)
{
    if((EOK != _http_req_msg_check(req)) || \
       (EOK != _http_session_open(&http_obj)) || \
       (EOK != _http_req_msg_make(req, &http_obj)) || \
       (EOK != _http_req_config(http_obj.curl_ptr, &http_obj.resp_struct)) || \
       (EOK != _http_post(http_obj.curl_ptr, http_code))){
        LOG_E_TAG("HTTP Request Failed");
        return NULL;
    }
    
    LOG_I_TAG("HTTP Request Success");
    if((http_obj.resp_struct.size > 0) && (NULL != http_obj.resp_struct.data)){
        LOG_D_TAG("Get HTTP Resp Data (%ld): %s", http_obj.resp_struct.size, http_obj.resp_struct.data);
    }
    return http_obj.resp_struct.data;
}

void http_manager_req_cleanup()
{
    LOG_D_TAG("Do Cleanup ......");
    if(NULL != http_obj.headers){
        LOG_D_TAG("Clean HTTP Headers");
        curl_slist_free_all(http_obj.headers);
        http_obj.headers = NULL;
    }

    if(NULL != http_obj.resp_struct.data){
        LOG_D_TAG("Clean HTTP Resp Data");
        free(http_obj.resp_struct.data);
        http_obj.resp_struct.data = NULL;
        http_obj.resp_struct.size = 0;
    }

    if(NULL != http_obj.curl_ptr){
        LOG_D_TAG("Clean CURL Object");
        curl_easy_cleanup(http_obj.curl_ptr);
        http_obj.curl_ptr = NULL;
    }
}
