#include <sys/time.h>
#include <stdio.h>
#include <time.h>
#include "define.h"


char *get_cur_time()
{
    static char s[20];
    time_t t;
    struct tm *ltime;
    time(&t);
    ltime = localtime(&t);
    strftime(s, 20, "%Y%m%d %H:%M:%S", ltime);
    return s;
}

void FS_FREE_BUF(void **pbuff)
{
    if((*pbuff) != NULL)
    {
        free(*pbuff);
        (*pbuff) = NULL;
    }
}
