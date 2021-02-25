/**
 * @file uni_dns_cache.h
 * @brief DNS缓存管理封装文件
 * @version 0.1
 * @date 2019-09-09
 * 
 * @copyright Copyright (c) tuya.inc 2019
 * 
 */


#ifndef __TUYA_BASE_UNI_DNS_CACHE_H__
#define __TUYA_BASE_UNI_DNS_CACHE_H__

#include "tuya_iot_config.h"
#include "tuya_hal_network.h"


#ifdef __cplusplus
    extern "C" {
#endif


#define MAX_DOMAIN_NAME_LEN     128
#define MAX_DNS_CNT             16

#if defined(TLS_MODE) && (TLS_MODE!=TLS_DISABLE)

#if defined(TLS_MODE) && ( (TLS_MODE==TLS_TUYA_PSK_ONLY) || (TLS_MODE==TLS_TUYA_ECC_PSK) )
/* HTTPS-PSK 模式 */
#define HTTP_DNS_SERVER_DOMAIN    "h3.iot-dns.com"
#else
/* HTTPS-ecc 模式 */
#define HTTP_DNS_SERVER_DOMAIN    "h2.iot-dns.com"
#endif

#else
/* HTTP 模式 */
#error "tls mode must be set."
#endif

typedef enum
{
    E_REGION_DYNAMIC = 0, //临时无固定区域
    E_REGION_CN,
    E_REGION_EU,
    E_REGION_US,
    E_REGION_UE,
    E_REGION_IN,
    E_REGION_MAX
}REGION_E;

typedef enum
{
    E_DNS_PRIO_REGION = 0,   //默认的最高优先级：使用region对应的IP地址
    E_DNS_PRIO_RANDOM,
    E_DNS_PRIO_SYSTEM,
    E_DNS_PRIO_MAX
}TY_DNS_PRIO_E;



/***********************************************************
*  Function: unw_gethostbyname
*  Desc:     change the domain to addr info
*  Input:    domain: domin info
*  Output:   addr: addr info
*  Return:   UNW_SUCCESS: success   others: fail
***********************************************************/
int unw_gethostbyname(const char *domain, UNW_IP_ADDR_T *addr);


typedef struct
{
    REGION_E        region;
    char            domain[MAX_DOMAIN_NAME_LEN];
    UNW_IP_ADDR_T   ip;
}TY_DNS_INFO_S;

typedef int (*UNW_QUERY_CUSTOM_DNS_CB)(const char *domain, UNW_IP_ADDR_T *addr);
typedef int (*UNW_SAVE_DNS_INFO_CB)(const TY_DNS_PRIO_E dns_prio, const REGION_E region);

int unw_init_dns_cache(UNW_QUERY_CUSTOM_DNS_CB dns_query_cb, UNW_SAVE_DNS_INFO_CB dns_save_cb);
int unw_clear_all_dns_cache(void);
int unw_clear_dns_cache(const char *domain);
int unw_add_dns_cache(const char *domain, const UNW_IP_ADDR_T ip);
void unm_lower_dns_cache_priority(void);
int unm_set_dns_cache_priority(TY_DNS_PRIO_E dns_prio);
int unm_set_dns_region(REGION_E region);

int unw_connect_to_domain(int *sockfd, char *hostname, uint16_t port, uint32_t retry_cnt, uint32_t timeout);


#ifdef __cplusplus
}
#endif

#endif // __TUYA_BASE_UNI_DNS_CACHE_H__

