/*
 * @File: src/sys_api/sys_process.h
 * @Author: LinusZhao
 * @Date: 2019-10-14 16:35:59
 * @LastEditors: LinusZhao
 * @LastEditTime: 2019-10-14 16:36:10
 * @Description: Do not edit
 */

#ifndef _SYS_PROCESS_H
#define _SYS_PROCESS_H

#include <stddef.h>

int sys_shell(const char *fmt, ...);
void kill_process(char *pCmd);

#endif
