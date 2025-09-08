/**
 * @file progress_bar.c
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

#include "common_def.h"
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define BAR_MAXLEN  (51)

static uint8_t percentage = 0;

extern uint8_t profileInstallStatus;

void progress_bar_percent_add(uint8_t percent)
{
    if(100 > (percentage + percent)){
        percentage += percent;
    }
    else{
        percentage = 99;
    }
}

void progress_bar_percent_set(uint8_t percent)
{
    if((percent <= 100) && (percentage < percent)){
        percentage = percent;
    }
}

void progress_bar_flush(void)
{
    if(percentage > 100){
        return;
    }
    
    static uint8_t sym_count = 0;
    static char sym[] = "/|-\\";
    char bar[BAR_MAXLEN + 1] = {0};
    memset(bar, '#', BAR_MAXLEN);

    fprintf(stdout, "\n[%-50s][%3d%%][%c]\n", &bar[BAR_MAXLEN - percentage / 2], percentage, sym[(sym_count++)%4]);
    fflush(stdout);
}

void progress_bar_clear(void)
{
    fprintf(stdout, "\033[1F");
    fprintf(stdout, "%70s\r", "");
    fprintf(stdout, "\033[1F");
}

void *progress_bar_thread_entry(void *arg)
{
    fprintf(stdout, "\n\n");
    while(1){
        if(PORFILE_INSTALL_DONE == profileInstallStatus){
            pthread_exit(NULL);
        }
        progress_bar_clear();
        progress_bar_flush();
        usleep(200000);
    }
    return 0;
}

