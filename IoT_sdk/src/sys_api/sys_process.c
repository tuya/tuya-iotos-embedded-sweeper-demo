/*****************************************************************
 * @File: src/sys_api/sys_process.c
 * @Author: LinusZhao
 * @Date: 2019-10-14 16:33:51
 * @LastEditors: LinusZhao
 * @LastEditTime: 2019-10-23 15:49:17
 * @Description: 系统进程相关操作：启动，退出
 *****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <math.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/sysinfo.h>

#include "sys_process.h"
#include "uni_log.h"

/*****************************************************************
 * @Function: sys_shell
 * @Description: system接口的再封装，支持格式化字符串命令参数
 * @Param: Do not edit
 * @Return: void
 *****************************************************************/
#define SYSTEM_SHELL_MAX_LEN        128
int sys_shell(const char *fmt, ...)
{
    int ret;
    char cmd[SYSTEM_SHELL_MAX_LEN];

    va_list ap;
    va_start(ap, fmt);
    vsnprintf(cmd, SYSTEM_SHELL_MAX_LEN, fmt, ap); 
    va_end(ap);

    cmd[SYSTEM_SHELL_MAX_LEN - 1] = '\0';
    PR_DEBUG("%s",cmd);
    ret = system(cmd);
    if (ret == -1){
        // 创建子进程失败
        return -1;
    }
    if (!WIFEXITED(ret)){
        // shell脚本执行错误
        return -1;
    }
    // shell返回值
    ret = WEXITSTATUS(ret);

    return ret;
}

/*****************************************************************
 * @Function: kill_process
 * @Description: 根据进程名kill进程
 * @Param: Do not edit
 * @Return: void
 *****************************************************************/
void kill_process(char *pCmd)
{
    // snprintf(pid_str,sizeof(pid_str),"ps -ef | grep %s | grep -v \"grep\" | awk '{print $2}'", pCmd);
    // pid = atoi(pid_str);
    // sys_shell("kill -9 %d",pid);
    FILE *pp;
	pid_t pid;
    char tmp[64];

    sprintf(tmp, "ps -ef | grep %s | grep -v \"grep\" | awk '{print $2}'", pCmd);
    pp = popen(tmp, "r");
    if (!pp){
        PR_ERR("popen error");
        return;
    }

    while (fgets(tmp, sizeof(tmp), pp) != NULL){
		sscanf(tmp, "%d", &pid);
		if (pid > 0){
			sys_shell("kill -9 %d", pid);
        	PR_DEBUG("kill %s PID: %d", pCmd, pid);
		}
    }

    pclose(pp);
}
