/**
 * @file log.c
 * 
 * @brief 
 * 
 * @copyright Copyright (c) 2021 Quectel Wierless Solution,Co.,Ltd. All Rights Reserved.
 * 
 * ============================== EDIT HISTORY FOR MODULE ==============================
 * This section contains comments describing the changes made to the module.
 * Notice that the latest change comment should be at the bottom.
 * WHEN         WHO                 WHAT,WHERE,WHY
 * ----------   -----------------   ----------------------------------------------------
 * 2021/08/19   Remy SHI            First commit
 */
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "log.h"

#define LOG_BUFF_MAX            (1024*14)

#define LOG_DEBUG_ESC_FLAG      "\033[m[DEBUG]"
#define LOG_INFO_ESC_FLAG       "\033[0;32m[INFO ]"
#define LOG_WARN_ESC_FLAG       "\033[0;33m[WARN ]"
#define LOG_ERROR_ESC_FLAG      "\033[0;31m[ERROR]"

#define LOG_HEADER_ESC_MAXLEN   (7)
#define LOG_HEADER_FLAG_MAXLEN  (7)     /* Including left and right brackets */
#define LOG_HEADER_TAG_MAXLEN   (7)
#define LOG_HEADER_FILE_MAXLEN  (40)
#define LOG_HEADER_LINE_MAXLEN  (11)

#define LOG_HEADER_ESC_FLAG_MAXLEN  (LOG_HEADER_ESC_MAXLEN + LOG_HEADER_FLAG_MAXLEN)
#define LOG_HEADER_MAXLEN   (LOG_HEADER_ESC_MAXLEN + LOG_HEADER_FLAG_MAXLEN + 1 + \
                            LOG_HEADER_TAG_MAXLEN + 1 + LOG_HEADER_FILE_MAXLEN + 1 \
                            + LOG_HEADER_LINE_MAXLEN)

static uint8_t global_log_level = LOG_LEVEL_DEBUG;
static char global_log_buff[LOG_BUFF_MAX + 1];

void log_level_set(uint8_t level)
{
    if(level > LOG_LEVEL_MAX){
        return ;
    }
    global_log_level = level;
}

void log_output(const log_args_t log_args, const char *fmt, ...)
{
    if(!(global_log_level && (log_args.level >= global_log_level))){
        return ;
    }

    char log_header[LOG_HEADER_MAXLEN + 1] = {0};
    char header_esc_flag[LOG_HEADER_ESC_FLAG_MAXLEN + 1] = {0};
    char header_tag[LOG_HEADER_TAG_MAXLEN + 1] = {0};
    char header_file[LOG_HEADER_FILE_MAXLEN + 1] = {0};
    char *esc_flag_str = NULL;
    
    switch(log_args.level)
    {
        case LOG_LEVEL_DEBUG:
            esc_flag_str = LOG_DEBUG_ESC_FLAG;
            break;
        case LOG_LEVEL_INFO:
            esc_flag_str = LOG_INFO_ESC_FLAG;
            break;
        case LOG_LEVEL_WARN:
            esc_flag_str = LOG_WARN_ESC_FLAG;
            break;
        case LOG_LEVEL_ERROR:
            esc_flag_str = LOG_ERROR_ESC_FLAG;
            break;
        default:
            /* Never reach */
            return;
    }
    
    snprintf(header_tag, LOG_HEADER_TAG_MAXLEN, "%s", log_args.tag);
    snprintf(header_file, LOG_HEADER_FILE_MAXLEN, "%s", log_args.file);
    snprintf(header_esc_flag, LOG_HEADER_ESC_FLAG_MAXLEN + 1, "%s", esc_flag_str);
    snprintf(log_header, LOG_HEADER_MAXLEN, "%s [%s] [%s] [%d] ", \
                     header_esc_flag, header_tag, header_file, log_args.line);
    uint16_t header_len = strlen(log_header);
    memcpy(global_log_buff, log_header, header_len);

    va_list args;
    va_start(args, fmt);
    vsnprintf(&global_log_buff[header_len], LOG_BUFF_MAX - header_len, fmt, args);
    va_end(args);

    fprintf(stdout, "%s\033[m\n", global_log_buff);
    memset(global_log_buff, 0x00, LOG_BUFF_MAX);
}
