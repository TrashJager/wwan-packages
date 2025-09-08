/**
 * @file log.h
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

#ifndef LOG_H
#define LOG_H

#include <stdint.h>
#include <stdarg.h>

#define LOG_LEVEL_NONE  (0)
#define LOG_LEVEL_DEBUG (1)
#define LOG_LEVEL_INFO  (2)
#define LOG_LEVEL_WARN  (3)
#define LOG_LEVEL_ERROR (4)

#define LOG_LEVEL_MIN   LOG_LEVEL_NONE
#define LOG_LEVEL_MAX   LOG_LEVEL_ERROR

typedef struct {
    uint8_t level;
    char *tag;
    char *file;
    int line;
}log_args_t;

void log_level_set(uint8_t level);
void log_output(const log_args_t log_args, const char *fmt, ...);

#endif /* LOG_H */

