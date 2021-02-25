#ifndef _GYRO_ROBOT_H_
#define _GYRO_ROBOT_H_

#include "tuya_cloud_types.h"
#include "tuya_cloud_com_defs.h"
#include "tuya_iot_com_api.h"
#include "uni_log.h"
#include <stdint.h>


// 涂鸦陀螺仪地图传输协议
#pragma pack(1)
typedef struct
{
    uint8_t   x;
    uint8_t   y;
    uint8_t   type;
} ST_POT;
#pragma pack()

/* !1 mode 工作模式 */
enum enum_point_type
{
    point_type_current              = 0x00, // 当前点
    point_type_barrier,                     // 障碍点
    point_type_cleaned,                     // 已清扫点
    point_type_charge,                      // 充电桩
};


void OpenDemoData(void);

void StartWork();

static void *threadAutoRun(void *args);

// vector<ST_POT> m_vMapSmart;



int MCU_SetSwitch(int data);
int MCU_SetSwitchGo(int data);
int MCU_SetMode(int data);
int MCU_SetDirect(int data);
int MCU_ResetEdge(int data);
int MCU_ResetRoll(int data);
int MCU_ResetFilter(int data);
int MCU_SetSeek(int data);
int MCU_SetSuction(int data);
int MCU_UploadRecord();
void start_clean(void);

/******************************************************************************
                        4:地图相关等操作(流服务相关)
******************************************************************************/
OPERATE_RET set_map_config(BYTE_T pt, BYTE_T size);
// OPERATE_RET init_flow_channel();
OPERATE_RET start_flow_trans(INT16_T map_id);
OPERATE_RET trans_flow_data(BYTE_T *pdata, int count);
OPERATE_RET end_flow_trans(INT16_T map_id);
#endif