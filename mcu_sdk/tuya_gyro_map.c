/*****************************************************************
 * @File: tuya_gyro_map.c
 * @Author: LinusZhao
 * @Date: 2019-08-13 16:35:23
 * @LastEditors: LinusZhao
 * @LastEditTime: 2019-10-30 08:34:39
 * @Description: 
 *****************************************************************/

#include "wifi.h"

// 地图配置参数定义
typedef struct
{
    unsigned char   origin; // 地图原点位置，0-左上角  1-左下角
    unsigned char   width;  // 地图宽度，最大255
    unsigned char   hight;  // 地图高度，最大255      
}ST_CONFIG;

// 地图坐标点定义
typedef struct
{
    unsigned char   x;
    unsigned char   y;
    unsigned char   type;
} ST_POT;

// map_id
typedef unsigned short MAP_ID_S;

// 坐标点类型定义
enum enum_point_type
{
    point_type_current   = 0x00,  // 当前点
    point_type_barrier   = 0x01,  // 障碍点
    point_type_cleaned   = 0x02,  // 已清扫点
    point_type_charge    = 0x03 // 充电桩
};

ST_CONFIG app_panel_config = {0x00,0xff,0xff};

/*****************************************************************
 * @Function: app_panel_init
 * @Description: 配置涂鸦智能APP面板显示的地图尺寸和原点位置            
 * @Return: void
 *****************************************************************/
void app_panel_init(void)
{
    unsigned char buffer[3] = {0};
    buffer[0] = app_panel_config.origin;
    buffer[1] = (app_panel_config.width) >> 8;
    buffer[2] = (app_panel_config.hight) & 0xFF;
    mcu_dp_raw_update(DPID_MAP_CONFIG,buffer,sizeof(buffer)); //RAW型数据上报;
}

/*****************************************************************
 * @Function: mcu_map_data_report
 * @Description: 地图数据上报            
 * @Param: id, 地图id号，用于标识一次完整的清扫过程
 * @Param: buffer, 地图坐标点数组
 * @Param: point_num, 坐标点数量
 * @Return: void
 *****************************************************************/
unsigned char  mcu_map_data_report(MAP_ID_S id, ST_POT *buffer, unsigned short point_num)
{
    static unsigned int map_offset = 0;
    static MAP_ID_S last_id = 0;
    unsigned short this_len = 0;

    if(stop_update_flag == ENABLE)
        return SUCCESS;

    if(last_id != id){ // 开始新的清扫，map_id更新
        map_offset = 0;
    }

    this_len = sizeof(GYRO_MAP_S) * point_num;
    stream_trans(id, map_offset, (unsigned char *)buffer, this_len);
    map_offset += this_len;

    last_id = id;

    return SUCCESS;
}

/*****************************************************************
 * @Function: mcu_map_record_upload
 * @Description: mcu上报扫地机清扫记录
 * @Param: map_id
 * @Param: clean_time, 清扫面积 0～ 999
 * @Param: clean_area, 清扫时间 0～ 999
 * @Return: void
 *****************************************************************/
void mcu_map_record_upload(MAP_ID_S map_id, unsigned short clean_time,unsigned short clean_area)
{
    char clean_record[12];
    snprintf(clean_record,30,"%03d%03d%05d",clean_time,clean_area,map_id);
    mcu_dp_string_update(DPID_CLEAN_RECORD,clean_record,sizeof(clean_record));
}

int main(void)
{
    // 相关初始化
    app_panel_init(); // 等待模组联网成功后调用

    while (1)
    {
        // mcu_map_data_report
        
    }
    
}
