#ifndef _TUYA_IOT_DEFINE_H_
#define _TUYA_IOT_DEFINE_H_

#include "tuya_cloud_types.h"
#include "tuya_cloud_com_defs.h"
#include "tuya_iot_com_api.h"
#include <stdio.h>
#include <math.h>
#include "config.h"

#ifdef USE_GYRO1
    // 产品PID 陀螺仪1#公版面板
    #define PRODUCT_KEY         "oj4pide3mjgqkt8k"                 // 产品PID，唯一标识
#endif

#ifdef USE_GYRO2
    // 产品PID 陀螺仪扫地机模拟#2
    #define PRODUCT_KEY         "d1tlxli0xiqzebcf"                 // 产品PID，唯一标识
#endif

#ifdef USE_GYRO3
    // 产品PID 陀螺仪3#公版面板  uiid=000000044h
    #define PRODUCT_KEY         "acbgcs2742xnb7a2"                 // 产品PID，唯一标识
    /*
        1. 面板无开机，开始清扫按钮了
        {"range":[
            "standby","random","smart","wall_follow","mop","spiral","left_spiral","right_spiral","right_bow","left_bow","partial_bow","chargego"
        ],"type":"enum"}
    */
#endif




/******************************************************************************
                        0:相关数据类型定义
******************************************************************************/


/******************************************************************************
                        1:dp数据点序列号重新定义
******************************************************************************/

#define DPID_SWITCH              1   //开关(可下发可上报) 
#define DPID_SWITCH_GO           2   // 清扫开关(可下发可上报)
#define DPID_MODE               3   //工作模式(可下发可上报)
#define DPID_DIRECTION_CONTROL  4   //方向(可下发可上报)
#define DPID_STATUS             5   //工作状态(只上报)
#define DPID_ELECTRICITY_LEFT   6   //剩余电量(只上报)
#define DPID_EDGE_BRUSH         7   //边刷寿命(只上报)
#define DPID_ROLL_BRUSH         8   //滚刷寿命(只上报)
#define DPID_FILTER             9   //滤网寿命(只上报)
#define DPID_RESET_EDGE_BRUSH   10  //边刷重置(可下发可上报)
#define DPID_RESET_ROLL_BRUSH   11  //滚刷重置(可下发可上报)
#define DPID_RESET_FILTER       12  //滤网重置(可下发可上报)
#define DPID_SEEK               13  //寻找机器(可下发可上报)
#define DPID_SUCTION            14  //吸力选择(可下发可上报)
#define DPID_CLEAN_RECORD       15  //清扫记录(只上报)
#define DPID_CLEAN_AREA         16  //清扫面积(只上报)
#define DPID_CLEAN_TIME         17  //清扫时间(只上报)
#define DPID_FAULT              18  //故障告警(只上报)
#define DPID_MAP_CONFIG         19  //地图参数(只上报)


//备注:0 正常；1边扫异常；2中扫异常；3左轮异常；4右轮异常；5垃圾盒异常；6 地检异常；7 碰撞异常；8其它问题

/******************************************************************************
                        5:DP枚举值等数据定义，与IOT平台一致
******************************************************************************/

typedef unsigned char switch_t;
typedef unsigned char switch_go_t;

/* !1 mode 工作模式 */
enum enum_mode
{
    mode_standby              = 0x00, // 待机
    mode_random,                      // 随机
    mode_smart,                       // 自动
    mode_wall_follow,                 // 沿墙
    mode_mop,                         // 拖地
    mode_spiral,                      // 定点清扫
    mode_left_spiral,                  // 左螺旋
    mode_right_spiral,                 // 右螺旋
    mode_right_bow,                    // 右⼸弓字
    mode_left_bow,                     // 左⼸弓字
    mode_partial_bow,                  // 局部弓字
    mode_chargego,                    // 回充模式
};

/* !2 direction_control 清扫方向 */

enum enum_direction_control
{
    direction_control_forward= 0x00,            // 前进
    direction_control_backward,                 // 后退
    direction_control_turn_left,                // 左转
    direction_control_turn_right,               // 右转
    direction_control_stop,                     // 停止
};

/* !3 status 设备状态 */
enum enum_status
{
    status_standby              = 0x00, // 待机
    status_smart_clean,                 // 自动清扫
    status_wall_clean,                  // 沿边清扫
    status_spot_clean,                  // 定点清扫
    status_mop_clean,                   // 拖地
    status_goto_charge,                 // 回充中
    status_charging,                    // 充电中
    status_charge_done,                 // 停止充电
    status_paused,                      // 暂停
};

/* !4 residual_electricity 剩余电量 */
typedef unsigned char residual_electricity_t;

/* !5 edge_brush 边刷寿命 */
typedef unsigned char edge_brush_t;

/* !6 roll_brush 滚刷寿命 */
typedef unsigned char roll_brush_t;

/* !7 filter 滤网寿命*/
typedef unsigned char filter_t;

/* suction 吸力选择 */
enum enum_suction
{
    suction_strong = 0x00,
    suction_normal,
};

/* clean_record 清扫记录 */

/* clean_area 清扫面积 */

/* clean_time 清扫时间 */

/* fault 故障告警 */
enum enum_fault
{
    clear_falut         = 0x00,   // 清除故障显示
    edge_sweep_fault    = 0x01,
    middle_sweep_fault  = 0x02,
    left_wheel_fault    = 0x04,
    right_wheel_fault   = 0x08,
    garbage_box_fault   = 0x10,
    land_check_fault    = 0x20,
    collision_fault     = 0x40,
//     TY_FAULT_8          = 0x80,
};

/* map_config 地图参数配置 */

/******************************************************************************
                        2:dp数据点操作响应(针对下发的指令)
******************************************************************************/
int dp_handle_power(int bres);
int dp_handle_power_go(int bres);
int dp_handle_mode(int bres);
int dp_handle_direction_control(int bres);
int dp_handle_reset_edge_brush(int bres);
int dp_handle_reset_roll_brush(int bres);
int dp_handle_reset_filter(int bres);
int dp_handle_seek(int bres);
int dp_handle_suction(int bres);

/******************************************************************************
                        3:dp数据点更新上报(针对上报的结果)
******************************************************************************/
int dp_update_status(int data);
int dp_update_electricity(int data);
int dp_update_edge_brush(int data);
int dp_update_roll_brush(int data);
int dp_update_filter(int data);
int dp_update_area(int data);
int dp_update_time(int data);
int dp_update_fault(unsigned int data);
int dp_update_clean_record(char *pdata, unsigned short len);

int dp_update_powergo(int data);
int dp_update_mode(int data);

int dp_update_init_value();

void connect_cloud_success_cb(void);

#endif

