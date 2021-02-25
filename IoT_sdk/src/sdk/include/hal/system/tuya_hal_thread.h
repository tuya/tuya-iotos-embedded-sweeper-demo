/**
 * @file tuya_hal_thread.h
 * @author qinfl@tuya.com
 * @brief thread相关接口封装
 * @version 0.1
 * @date 2019-08-15
 * 
 * @copyright Copyright (c) tuya.inc 2019
 * 
 */

#ifndef __TUYA_HAL_THREAD_H__
#define __TUYA_HAL_THREAD_H__


#include <stdbool.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif



/**
 * @brief handle of thread
 * 
 */
typedef void* THREAD_HANDLE;

/**
 * @brief thread function type
 * 
 */
typedef void (*THREAD_FUNC_T)(void*);

/**
 * @brief create and start a tuya sdk thread
 * 
 * @param[out] thread       the thread handle
 * @param[in] name          the thread name
 * @param[in] stack_size    stack size of thread
 * @param[in] priority      priority of thread
 * @param[in] func          the main thread process function
 * @param[in] arg           the args of the func, can be null
 * @retval OPRT_OK          success
 * @retval Other            fail
 */
int tuya_hal_thread_create(THREAD_HANDLE* thread,
                           const char* name,
                           uint32_t stack_size,
                           uint32_t priority,
                           THREAD_FUNC_T func,
                           void* const arg);

/**
 * @brief terminal thread and release thread resources
 * 
 * @param[in] thread    the input thread handle
 * @retval OPRT_OK      success
 * @retval Other        fail
 */
int tuya_hal_thread_release(THREAD_HANDLE thread);

/**
 * @brief check thread is self thread
 * 
 * @param[in] thread    the thread handle
 * @param[out] is_self  output is self thread
 * @retval OPRT_OK      success
 * @retval Other        fail
 */
int tuya_hal_thread_is_self(THREAD_HANDLE thread, bool* is_self);

/**
 * @brief set name of self thread
 * 
 * @param[in] name      thread name
 * @retval OPRT_OK      success
 * @retval Other        fail
 */
int tuya_hal_thread_set_self_name(const char* name);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif // __TUYA_HAL_THREAD_H__

