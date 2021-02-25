#include "tuya_cloud_types.h"
#include "tuya_cloud_error_code.h"
#include "tuya_cloud_com_defs.h"
#include "tuya_iot_com_api.h"
#include "uni_log.h"

#include "tuya_cloud_wifi_defs.h"
#include "tuya_iot_wifi_api.h"

#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/stat.h>
#include "cJSON.h"
#include "uni_time.h"
#include "mem_pool.h"

#include "tuya_iot_protocol.h"

// UserTODO
// SOC固件版本，用于OTA管理，格式必须为"XX.XX.XX"，其中XX必须为数字
#ifndef USER_SW_VER
#define USER_SW_VER         "1.2.3"
#endif

// 涂鸦云上的产品KEY，需登陆tuya.com创建产品分配唯一key
// #define PRODUCT_KEY         "hDbYlEbRkVkUuj6R"
// SD本地配置存储路径，该路径必须对应一个可读写文件系统分区
// #define CFG_STORAGE_PATH    "./"
// UUID和AUTHKEY用于涂鸦云设备的安全认证，每个设备所用key均为唯一
// #define UUID                "tuya985d5f5255e0342b"
// #define AUTHKEY             "xONNZA9yFvLZeEDQRh0CpaE2wAAKAZJT"
// OTA文件本地保存绝对路径，SDK会自动进行OTA管理
#define SOC_OTA_FILE        "/tmp/soc_upgrade.ota"



STATIC VOID __soc_dev_rev_upgrade_info_cb(IN CONST FW_UG_S *fw);//SOC设备升级入口
STATIC VOID __soc_dev_status_changed_cb(IN CONST GW_STATUS_E status);//SOC设备云端状态变更回调
STATIC VOID __soc_dev_dp_query_cb(IN CONST TY_DP_QUERY_S *dp_qry);//SOC设备特定数据查询入口
STATIC VOID __soc_dev_obj_dp_cmd_cb(IN CONST TY_RECV_OBJ_DP_S *dp);//SOC设备格式化指令数据下发入口
STATIC VOID __soc_dev_raw_dp_cmd_cb(IN CONST TY_RECV_RAW_DP_S *dp);//SOC设备透传指令数据下发入口
STATIC VOID __soc_dev_reset_req_cb(GW_RESET_TYPE_E type);//SOC设备进程重启请求入口
STATIC VOID __soc_dev_net_status_cb(IN CONST GW_WIFI_NW_STAT_E stat);//SOC外网状态变动回调

int ituyanet = -1;  //SDK联网状态

int tuya_iot_get_net_status(void)
{
    return ituyanet;
}

static int cmd_output(char *cmd, unsigned char *result)
{
    char buffer[64];
    FILE* pipe = popen(cmd, "r");
    if (!pipe){
        return -1;
    }
    while(!feof(pipe)) {
        if(fgets(buffer, sizeof(buffer), pipe)){
            // strcat(result, buffer);
            *result = atoi(buffer);
        }
    }
    pclose(pipe);
    return 0;
}

void tuya_sdk_log_cb(const char *str)
{
    //UserTODO 用户具体实现日志输出位置，以下示例输出到文件
    FILE *log_fd;
    log_fd = fopen("debug.log", "a+"); // 采用追加方式，用户需限制文件大小
    if(log_fd == NULL){
        printf("log_fd fopen is error");
    }
    UINT16_T str_len = strlen(str);
    if( fwrite(str,sizeof(CHAR_T),strlen(str),log_fd) != str_len){
        printf("fwrite log_fd is error");
    }
    fflush(log_fd); // 冲洗缓存区，立即写到磁盘文件
    fclose(log_fd);
}

void* tuya_iot_soc_dev_entry(void* arg)
{
    printf("IOT SDK Version: %s \r\n", tuya_iot_get_sdk_info());
    printf("PRODUCT_KEY: %s \r\n", PRODUCT_KEY);
    printf("USER_SW_VER: %s \r\n", USER_SW_VER);
    printf("CFG_STORAGE_PATH: %s \r\n", GYRO_CFG_STORAGE_PATH);
    printf("UUID: %s \r\n", GYRO_UUID);
    printf("AUTHKEY: %s \r\n", GYRO_AUTHKEY);

    OPERATE_RET op_ret = tuya_iot_init(GYRO_CFG_STORAGE_PATH);
    if(OPRT_OK != op_ret) {
        PR_ERR("tuya_iot_init err:%d PATH:%s", op_ret, GYRO_CFG_STORAGE_PATH);
        return -1;
    }
    PR_DEBUG("tuya_iot_init success");

    // op_ret = AddOutputTerm("tuya_sdk_log",tuya_sdk_log_cb);
    // if(OPRT_OK != op_ret){
    //     PR_ERR("AddOutputTerm op_ret is %d\n",op_ret);
    // }

    op_ret = SetLogManageAttr(TY_LOG_LEVEL_DEBUG);
    if(OPRT_OK != op_ret){
        PR_ERR("SetLogManageAttr op_ret is %d\n",op_ret);
    }

    WF_GW_PROD_INFO_S prod_info = {GYRO_UUID, GYRO_AUTHKEY, NULL, NULL};
    op_ret = tuya_iot_set_wf_gw_prod_info(&prod_info);
    if(OPRT_OK != op_ret) {
        PR_ERR("tuya_iot_set_wf_gw_prod_info err:%d", op_ret);
        return -2;
    }
    PR_DEBUG("tuya_iot_set_wf_gw_prod_info success");

    TY_IOT_CBS_S iot_cbs = {
        __soc_dev_status_changed_cb,
        __soc_dev_rev_upgrade_info_cb,
        __soc_dev_reset_req_cb,
        __soc_dev_obj_dp_cmd_cb,
        __soc_dev_raw_dp_cmd_cb,
        __soc_dev_dp_query_cb,
        NULL,
    };
    op_ret = tuya_iot_wf_soc_dev_init(GWCM_OLD, WF_START_AP_ONLY, &iot_cbs, PRODUCT_KEY, USER_SW_VER);
    if(OPRT_OK != op_ret) {
        PR_ERR("tuya_iot_wf_soc_dev_init err:%d",op_ret);
        return -3;
    }
    PR_DEBUG("tuya_iot_wf_soc_dev_init success");

    op_ret = tuya_iot_reg_get_wf_nw_stat_cb(__soc_dev_net_status_cb);
    if(OPRT_OK != op_ret) {
        PR_ERR("tuya_iot_reg_get_wf_nw_stat_cb err:%d",op_ret);
        return -4;
    }
    PR_DEBUG("tuya_iot_reg_get_wf_nw_stat_cb success");

    static unsigned char gpio_count = 0;
    while (1)
    {
        sleep(1);
        unsigned char gpio2 = 0;
        int ret = cmd_output("pigs r 2",&gpio2);
        // PR_NOTICE("GPIO2 is %d",gpio2);
        if(!gpio2){
            gpio_count++;
        }
        if(gpio_count >= 5)// 本地reset
        {
            gpio_count = 0;
            OPERATE_RET ret = tuya_iot_wf_gw_unactive();
            if(OPRT_OK != ret) {
                PR_ERR("tuya_iot_gw_unactive err:%d", ret);
                return -5;
            }
            PR_NOTICE("tuya_iot_gw_unactive success");
        }

    }

    return 0;
}

//SOC设备升级相关代码开始
STATIC VOID __upgrade_notify_cb(IN CONST FW_UG_S *fw, IN CONST INT_T download_result, IN PVOID_T pri_data)
{
    FILE *p_upgrade_fd = (FILE *)pri_data;
    fclose(p_upgrade_fd);

    if(download_result == 0) {
        PR_DEBUG("SOC Upgrade File Download Success");
        //User TODO

    }else {
        PR_ERR("SOC Upgrade File Download Fail.ret = %d", download_result);
    }
}

STATIC OPERATE_RET __get_file_data_cb(IN CONST FW_UG_S *fw, IN CONST UINT_T total_len, IN CONST UINT_T offset,
                                      IN CONST BYTE_T *data, IN CONST UINT_T len, OUT UINT_T *remain_len, IN PVOID_T pri_data)
{
//    PR_DEBUG("Rev File Data");
//    PR_DEBUG("Total_len:%u", total_len);
//    PR_DEBUG("Offset:%u Len:%u", offset, len);
    FILE *p_upgrade_fd = (FILE *)pri_data;
    fwrite(data, 1, len, p_upgrade_fd);
    *remain_len = 0;

    return OPRT_OK;
}

//SOC设备升级入口
VOID __soc_dev_rev_upgrade_info_cb(IN CONST FW_UG_S *fw)
{
    PR_DEBUG("SOC Rev Upgrade Info");
    PR_DEBUG("fw->tp:%d", fw->tp);
    PR_DEBUG("fw->fw_url:%s", fw->fw_url);
    PR_DEBUG("fw->fw_hmac:%s", fw->fw_hmac);
    PR_DEBUG("fw->sw_ver:%s", fw->sw_ver);
    PR_DEBUG("fw->file_size:%u", fw->file_size);

    OPERATE_RET op_ret;

    // 如果设备不满足升级条件,拒绝升级，比如电量过低
#if 0
    op_ret = tuya_iot_refuse_upgrade(fw,tuya_iot_get_gw_id());
    if (op_ret != OPRT_OK) {
        PR_ERR("tuya_iot_refuse_upgrade op_ret:%d",op_ret);
        return ;
    }
    else{
        PR_DEBUG("tuya_iot_refuse_upgrade is ok");
        return;
    }
#endif

    FILE *p_upgrade_fd = fopen(SOC_OTA_FILE, "w+b");
    if(NULL == p_upgrade_fd){
        PR_ERR("open upgrade file fail. upgrade fail %s", SOC_OTA_FILE);
        return;
    }
#if 1  // tuya_sdk上报进度
    op_ret = tuya_iot_upgrade_gw_notify(fw, __get_file_data_cb, __upgrade_notify_cb, p_upgrade_fd,TRUE,0);
#else  // 关闭tuya_sdk进度上报，由应用层上报
    op_ret = tuya_iot_upgrade_gw_notify(fw, __get_file_data_cb, __upgrade_notify_cb, p_upgrade_fd,FALSE,0);
#endif    
    if(OPRT_OK != op_ret) {
        PR_ERR("tuya_iot_upgrade_gw_notify err:%d",op_ret);
    }

}
//升级相关代码结束

//SOC设备云端状态变更回调
VOID __soc_dev_status_changed_cb(IN CONST GW_STATUS_E status)
{
    PR_DEBUG("SOC TUYA-Cloud Status:%d", status);
}

//SOC设备特定数据查询入口
VOID __soc_dev_dp_query_cb(IN CONST TY_DP_QUERY_S *dp_qry)
{
    PR_DEBUG("SOC Rev DP Query Cmd");
    if(dp_qry->cid != NULL) PR_ERR("soc not have cid.%s", dp_qry->cid);

    if(dp_qry->cnt == 0) {
        PR_DEBUG("soc rev all dp query");
        //User TODO
    }else {
        PR_DEBUG("soc rev dp query cnt:%d", dp_qry->cnt);
        UINT_T index = 0;
        for(index = 0; index < dp_qry->cnt; index++) {
            PR_DEBUG("rev dp query:%d", dp_qry->dpid[index]);
            //User TODO
        }
    }
}

//SOC设备格式化指令数据下发入口
VOID __soc_dev_obj_dp_cmd_cb(IN CONST TY_RECV_OBJ_DP_S *dp)
{
    PR_DEBUG("SOC Rev DP Obj Cmd t1:%d t2:%d CNT:%u", dp->cmd_tp, dp->dtt_tp, dp->dps_cnt);
    if(dp->cid != NULL) PR_ERR("soc not have cid.%s", dp->cid);

    UINT_T index = 0;
    for(index = 0; index < dp->dps_cnt; index++)
    {
        CONST TY_OBJ_DP_S *p_dp_obj = dp->dps + index;

        // 日志打印，查看当前DP指令的内容
        PR_NOTICE("idx:%d dpid:%d type:%d ts:%u", index, p_dp_obj->dpid, p_dp_obj->type, p_dp_obj->time_stamp);
        switch (p_dp_obj->type) 
        {
            case PROP_BOOL:     { PR_NOTICE("bool value:%d", p_dp_obj->value.dp_bool); break;}
            case PROP_VALUE:    { PR_NOTICE("INT value:%d", p_dp_obj->value.dp_value); break;}
            case PROP_STR:      { PR_NOTICE("str value:%s", p_dp_obj->value.dp_str); break;}
            case PROP_ENUM:     { PR_NOTICE("enum value:%u", p_dp_obj->value.dp_enum); break;}
            case PROP_BITMAP:   { PR_NOTICE("bits value:0x%X", p_dp_obj->value.dp_bitmap); break;}
            default:            { PR_ERR("idx:%d dpid:%d type:%d ts:%u is invalid", index, p_dp_obj->dpid, p_dp_obj->type, p_dp_obj->time_stamp); break;}
        }

        // MCU 处理当前DP指令
        switch (p_dp_obj->dpid)
        {
            case DPID_SWITCH:             dp_handle_power(p_dp_obj->value.dp_bool); break;
            case DPID_SWITCH_GO:          dp_handle_power_go(p_dp_obj->value.dp_bool); break;
            case DPID_MODE:               dp_handle_mode(p_dp_obj->value.dp_enum); break;
            case DPID_DIRECTION_CONTROL:  dp_handle_direction_control(p_dp_obj->value.dp_enum); break;
            case DPID_RESET_EDGE_BRUSH:   dp_handle_reset_edge_brush(p_dp_obj->value.dp_bool); break;
            case DPID_RESET_ROLL_BRUSH:   dp_handle_reset_roll_brush(p_dp_obj->value.dp_bool); break;
            case DPID_RESET_FILTER:       dp_handle_reset_filter(p_dp_obj->value.dp_bool); break;
            case DPID_SEEK:               dp_handle_seek(p_dp_obj->value.dp_bool); break;
            case DPID_SUCTION:            dp_handle_suction(p_dp_obj->value.dp_enum); break;
            default: break;
        }
    }
}

//SOC设备透传指令数据下发入口
VOID __soc_dev_raw_dp_cmd_cb(IN CONST TY_RECV_RAW_DP_S *dp)
{
    PR_DEBUG("SOC Rev DP Raw Cmd t1:%d t2:%d dpid:%d len:%u", dp->cmd_tp, dp->dtt_tp, dp->dpid, dp->len);
    if(dp->cid != NULL) PR_ERR("soc not have cid.%s", dp->cid);

    //User TODO

    //用户处理完成之后需要主动上报最新状态，这里简单起见，直接返回收到的数据，认为处理全部成功。
    OPERATE_RET op_ret = dev_report_dp_raw_sync(dp->cid,dp->dpid,dp->data,dp->len,0);
    if(OPRT_OK != op_ret) {
        PR_ERR("dev_report_dp_json_async op_ret:%d",op_ret);
    }
}

//SOC设备进程重启请求入口
VOID __soc_dev_reset_req_cb(GW_RESET_TYPE_E type)
{
    PR_DEBUG("SOC Rev Reset Req %d", type);
    
    //User TODO
}

// SOC外网状态变动回调
STATIC VOID __soc_dev_net_status_cb(IN CONST GW_WIFI_NW_STAT_E stat)
{
    PR_DEBUG("network status:%d", stat);
    if(stat == STAT_CLOUD_CONN){
            connect_cloud_success_cb();
    }
}

