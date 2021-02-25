#ifndef _DEFINE_H_
#define _DEFINE_H_

#ifdef __cplusplus
extern "C" {
#endif

char *get_cur_time();

#define FS_INFO(fmt, ...)  printf("[FS_INFO] [%s]: " fmt "\n", get_cur_time(), ##__VA_ARGS__)
#define FS_ERROR(fmt, ...) printf("[FS_ERROR] [%s]: " fmt "\n", get_cur_time(), ##__VA_ARGS__)

void FS_FREE_BUF(void **pbuff);

#ifdef __cplusplus
}
#endif

#endif
