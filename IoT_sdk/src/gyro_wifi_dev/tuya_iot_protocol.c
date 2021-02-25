/*****************************************************************
 * @File: tuya_iot_protocol.c
 * @Author: LinusZhao
 * @Date: 2019-10-21 11:05:23
 * @LastEditors: LinusZhao
 * @LastEditTime: 2019-10-21 11:10:58
 * @Description: 陀螺仪设备功能点逻辑处理，兼容1 2 3三个APP界面
 *****************************************************************/

#include "tuya_iot_protocol.h"
#include "gyro_mcu.h"
#include "uni_log.h"
#include "uni_time.h"

//////////////////////////////////////////////////////////////////

extern int g_bIsFlowOpen;
extern int g_iCurMapId;



OPERATE_RET dp_update_singe_obj(BYTE_T id, DP_PROP_TP_E type, int value)
{
    TY_OBJ_DP_S dp_data;

    dp_data.dpid = id;
    dp_data.type = type;
    dp_data.time_stamp = 0;

    switch (type)
    {
        case PROP_BOOL: dp_data.value.dp_bool = value; break;
        case PROP_ENUM: dp_data.value.dp_enum = value; break;
        case PROP_VALUE: dp_data.value.dp_value = value; break;
        case PROP_BITMAP: dp_data.value.dp_bitmap = value; break;
        default: break;
    }

    OPERATE_RET op_ret = dev_report_dp_json_async(NULL, &dp_data, 1);
    if(op_ret != OPRT_OK){
        PR_ERR("dp_update_singe_obj, op_ret:%d",op_ret);
    }

    return op_ret;
}

// 下发型DP，指令处理
///////////////////////////////////////////////////////////////////

int dp_handle_power(int bres)
{
    // MCU处理当前DP指令
    int ret = MCU_SetSwitch(bres);

    // MCU上报处理结果
    int op_ret = dp_update_singe_obj(DPID_SWITCH, PROP_BOOL, ret);

    // MCU上报异常处理
    if (op_ret == OPRT_OK)
        return 1;
    else
        return 0;
}

int dp_handle_power_go(int bres)
{
    // MCU处理当前DP指令
    int ret = MCU_SetSwitchGo(bres);

    // MCU上报处理结果
    int op_ret = dp_update_singe_obj(DPID_SWITCH_GO, PROP_BOOL, ret);

    // MCU上报异常处理
    if (op_ret == OPRT_OK)
        return 1;
    else
        return 0;
}

int dp_handle_mode(int bres)
{
    // MCU处理当前DP指令
    int ret = MCU_SetMode(bres);

    // MCU上报处理结果
    int op_ret = dp_update_singe_obj(DPID_MODE, PROP_ENUM, ret);

    // MCU上报异常处理
    if (op_ret == OPRT_OK)
        return 1;
    else
        return 0;
}

int dp_handle_direction_control(int bres)
{
    // MCU处理当前DP指令
    int ret = MCU_SetDirect(bres);

    // MCU上报处理结果
    int op_ret = dp_update_singe_obj(DPID_DIRECTION_CONTROL, PROP_ENUM, ret);

    // MCU上报异常处理
    if (op_ret == OPRT_OK)
        return 1;
    else
        return 0;
}

int dp_handle_reset_edge_brush(int bres)
{
    // MCU处理当前DP指令
    int ret = MCU_ResetEdge(bres);

    // MCU上报处理结果
    int op_ret = dp_update_singe_obj(DPID_RESET_EDGE_BRUSH, PROP_BOOL, ret);

    // MCU上报异常处理
    if (op_ret == OPRT_OK)
        return 1;
    else
        return 0;
}

int dp_handle_reset_roll_brush(int bres)
{
    // MCU处理当前DP指令
    int ret = MCU_ResetRoll(bres);

    // MCU上报处理结果
    int op_ret = dp_update_singe_obj(DPID_RESET_ROLL_BRUSH, PROP_BOOL, ret);;

    // MCU上报异常处理
    if (op_ret == OPRT_OK)
        return 1;
    else
        return 0;
}

int dp_handle_reset_filter(int bres)
{
    // MCU处理当前DP指令
    int ret = MCU_ResetFilter(bres);

    // MCU上报处理结果
    int op_ret = dp_update_singe_obj(DPID_RESET_FILTER, PROP_BOOL, ret);

    // MCU上报异常处理
    if (op_ret == OPRT_OK)
        return 1;
    else
        return 0;
}

int dp_handle_seek(int bres)
{
    // MCU处理当前DP指令
    int ret = MCU_SetSeek(bres);

    // MCU上报处理结果
    int op_ret = dp_update_singe_obj(DPID_SEEK, PROP_BOOL, ret);

    // MCU上报异常处理
    if (op_ret == OPRT_OK)
        return 1;
    else
        return 0;
}

int dp_handle_suction(int bres)
{
    // MCU处理当前DP指令
    int ret = MCU_SetSuction(bres);

    // MCU上报处理结果
    int op_ret = dp_update_singe_obj(DPID_SUCTION, PROP_ENUM, ret);

    // MCU上报异常处理
    if (op_ret == OPRT_OK)
        return 1;
    else
        return 0;
}

// 机器开机初始化，状态更新上报
//////////////////////////////////////////////////////////////////////////////////////
int dp_update_init_value()
{
    TY_OBJ_DP_S dp_data[8];

    dp_data[0].dpid = DPID_SWITCH;
    dp_data[0].type = PROP_BOOL;
    dp_data[0].time_stamp = 0;
    dp_data[0].value.dp_bool = 0;

    dp_data[1].dpid = DPID_SWITCH_GO;
    dp_data[1].type = PROP_BOOL;
    dp_data[1].time_stamp = 0;
    dp_data[1].value.dp_bool = 0;

    dp_data[2].dpid = DPID_MODE;
    dp_data[2].type = PROP_ENUM;
    dp_data[2].time_stamp = 0;
    dp_data[2].value.dp_enum = mode_standby;

    dp_data[3].dpid = DPID_STATUS;
    dp_data[3].type = PROP_ENUM;
    dp_data[3].time_stamp = 0;
    dp_data[3].value.dp_enum = status_standby;

    dp_data[4].dpid = DPID_CLEAN_TIME;
    dp_data[4].type = PROP_VALUE;
    dp_data[4].time_stamp = 0;
    dp_data[4].value.dp_value = 0;

    dp_data[5].dpid = DPID_CLEAN_AREA;
    dp_data[5].type = PROP_VALUE;
    dp_data[5].time_stamp = 0;
    dp_data[5].value.dp_value = 0;

    dp_data[6].dpid = DPID_ELECTRICITY_LEFT;
    dp_data[6].type = PROP_VALUE;
    dp_data[6].time_stamp = 0;
    dp_data[6].value.dp_value = 100;

    dp_data[7].dpid = DPID_FAULT;
    dp_data[7].type = PROP_BITMAP;
    dp_data[7].time_stamp = 0;
    dp_data[7].value.dp_bitmap = 0x00;

    OPERATE_RET op_ret = dev_report_dp_json_async(NULL, dp_data, \
                        sizeof(dp_data)/sizeof(TY_OBJ_DP_S));

    // MCU上报异常处理
    if (op_ret == OPRT_OK)
        return 1;
    else
        return 0;
}


// 下发及上报型DP，状态更新上报
//////////////////////////////////////////////////////////////////////////////////////
int dp_update_powergo(int data)
{
    // MCU上报处理结果
    int op_ret = dp_update_singe_obj(DPID_SWITCH_GO, PROP_BOOL, data);

    // MCU上报异常处理
    if (op_ret == OPRT_OK)
        return 1;
    else
        return 0;
}

int dp_update_mode(int data)
{
    // MCU上报处理结果
    int op_ret = dp_update_singe_obj(DPID_MODE, PROP_ENUM, data);
    // MCU上报异常处理
    if (op_ret == OPRT_OK)
        return 1;
    else
        return 0;
}

// 仅上报型DP，状态更新上报
//////////////////////////////////////////////////////////////////////////////////////
int dp_update_status(int data)
{
    // MCU上报处理结果
    int op_ret = dp_update_singe_obj(DPID_STATUS, PROP_ENUM, data);

    // MCU上报异常处理
    if (op_ret == OPRT_OK)
        return 1;
    else
        return 0;
}

int dp_update_electricity(int data)
{
    // MCU上报处理结果
    int op_ret = dp_update_singe_obj(DPID_ELECTRICITY_LEFT, PROP_VALUE, data);
    // MCU上报异常处理
    if (op_ret == OPRT_OK)
        return 1;
    else
        return 0;
}

int dp_update_edge_brush(int data)
{
    // MCU上报处理结果
    int op_ret = dp_update_singe_obj(DPID_EDGE_BRUSH, PROP_VALUE, data);
    // MCU上报异常处理
    if (op_ret == OPRT_OK)
        return 1;
    else
        return 0;
}

int dp_update_roll_brush(int data)
{
    // MCU上报处理结果
    int op_ret = dp_update_singe_obj(DPID_ROLL_BRUSH, PROP_VALUE, data);

    // MCU上报异常处理
    if (op_ret == OPRT_OK)
        return 1;
    else
        return 0;
}

int dp_update_filter(int data)
{
    // MCU上报处理结果
    int op_ret = dp_update_singe_obj(DPID_FILTER, PROP_VALUE, data);

    // MCU上报异常处理
    if (op_ret == OPRT_OK)
        return 1;
    else
        return 0;
}

int dp_update_area(int data)
{
    // MCU上报处理结果
    int op_ret = dp_update_singe_obj(DPID_CLEAN_AREA, PROP_VALUE, data);

    // MCU上报异常处理
    if (op_ret == OPRT_OK)
        return 1;
    else
        return 0;
}

int dp_update_time(int data)
{
    // MCU上报处理结果
    int op_ret = dp_update_singe_obj(DPID_CLEAN_TIME, PROP_VALUE, data);

    // MCU上报异常处理
    if (op_ret == OPRT_OK)
        return 1;
    else
        return 0;
}

int dp_update_fault(unsigned int data)
{
     // MCU上报处理结果
    int op_ret = dp_update_singe_obj(DPID_FAULT, PROP_BITMAP, data);

    // MCU上报异常处理
    if (op_ret == OPRT_OK)
        return 1;
    else
        return 0;

}

/*****************************************************************************
函数名称 : dp_update_clean_record
功能描述 : 上报本次清扫记录
输入参数 : pdata 清扫记录
          格式：清扫日期(YYYYMMDDTTRR) + 清扫时长(XXX) + 清扫面积(xxx) + 地图ID(XXXXX)
          如： "20180411051102008000020" 代表2018年4月11日05点11分，
               清扫了20分钟，80平方米 ，id号为20
             （假如MCU没有RTC，时间可不上报。记录则默认显示为数据上报时间）
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 清扫完成或停止后上报
*****************************************************************************/
int dp_update_clean_record(char* pdata, unsigned short len)
{
    PR_NOTICE("send record: %s\n",pdata);  

    TY_OBJ_DP_S dp_data;

    dp_data.dpid = DPID_CLEAN_RECORD;
    dp_data.type = PROP_STR;
    dp_data.time_stamp = 0;
    dp_data.value.dp_str = pdata;

    OPERATE_RET op_ret = dev_report_dp_json_async(NULL, &dp_data, 1);

    if (OPRT_OK != op_ret)
    {
        printf("set_map_config op_ret:%d/n", op_ret);
    }

    return op_ret;
}

void connect_cloud_success_cb(void)
{
    set_map_config(0x00, 0xff);
//     sleep(1);
    // init_flow_channel();
    dp_update_init_value();
}
