#include "gyro_mcu.h"

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#include "tuya_cloud_types.h"
#include "tuya_cloud_error_code.h"
#include "tuya_cloud_com_defs.h"
#include "tuya_iot_com_api.h"
#include "uni_log.h"
#include "uni_time.h"

#include "tuya_cloud_wifi_defs.h"
#include "tuya_iot_wifi_api.h"

#include "tuya_iot_protocol.h"

#include "define.h"


extern void *tuya_iot_soc_dev_entry(void *arg);

//////////////////////////////////////////////////////////////////
// struct robot_msg
// {
        
// }g_robot_msg;

int g_bIsPower = 0;
int g_bIsPowerGo = 0;
int g_iSetMode = 0;
int g_iSetDirect = 0;
int g_bIsResetEdge = 0;
int g_bIsResetRoll = 0;
int g_bIsResetFilter = 0;
int g_bIsSeek = 0;
int g_iSetSuction = 0;

int g_bIsFlowOpen = 0;
int g_iCurMapId = 100;

int g_bIsRun = 0;

int g_nEdgeBrush = 100;
int g_nRollBrush = 100;
int g_nFilter = 100;

int g_nElectricity = 100;
int g_nCleanTime = 0;
int g_nCleanArea = 0;

#define BUF_MAXLEN 10240

ST_POT m_szMap[BUF_MAXLEN];

int g_nMapBytes = 0;
int g_nMapLength = 0;


//////////////////////////////////////////////////////////////////
// void MCU_Update_Init()
// {
//     set_map_config(0x00, 0xff);

//     sleep(1);

//     // init_flow_channel();

//     dp_update_init_value();
// }

/*****************************************************************
 * @Function: MCU_SetSwitch
 * @Description: switch 开关指令处理
 * @Param: Do not edit
 * @Return: void
 *****************************************************************/
int MCU_SetSwitch(int data)
{
#if defined(USE_GYRO1)
    g_bIsPower = data;

    if (g_bIsPower == 1){
        dp_update_mode(mode_smart);
        dp_update_status(status_smart_clean);
        start_clean();
    }else{
        g_bIsRun = 0;    
        dp_update_mode(mode_standby);
        dp_update_status(status_standby);
        if(g_iCurMapId){
                end_flow_trans(g_iCurMapId);
                MCU_UploadRecord();
                g_nCleanArea = 0;
                g_nCleanTime = 0;
        }
    }
#endif
    return data;
}

/*****************************************************************
 * @Function: MCU_SetSwitchGo
 * @Description: switch_go 清扫开关指令处理
 * @Param: Do not edit
 * @Return: void
 *****************************************************************/
int MCU_SetSwitchGo(int data)
{
    g_bIsPowerGo = data;

    if (g_bIsPowerGo == 1){
        dp_update_mode(mode_smart);
        dp_update_status(status_smart_clean);
        start_clean();
    }else{
        g_bIsRun = 0;    
        dp_update_mode(mode_standby);
        dp_update_status(status_standby);
        if(g_iCurMapId){
                end_flow_trans(g_iCurMapId);
                MCU_UploadRecord();
                g_nCleanArea = 0;
                g_nCleanTime = 0;
        }
    }

    return data;
}

void start_clean(void)
{
        // 先结束上一次清扫
        if(g_iCurMapId){
                end_flow_trans(g_iCurMapId);
                MCU_UploadRecord();
                g_nCleanArea = 0;
                g_nCleanTime = 0;
                printf("g_nCleanArea:%d g_nCleanTime:%d\r\n",g_nCleanArea,g_nCleanTime);
        }    
        g_bIsRun = 1;
        start_flow_trans(g_iCurMapId);
}

extern pthread_mutex_t mutex;

/*****************************************************************
 * @Function: MCU_SetMode
 * @Description: 
 * @Param: Do not edit
 * @Return: void
 *****************************************************************/
int MCU_SetMode(int data)
{
    pthread_mutex_lock(&mutex);    
    g_bIsRun = 0;
    // sleep(1);
    int sts = 0;
    g_iSetMode = data;
    if (g_iSetMode == mode_standby){
        g_bIsPowerGo = 0;
        sts = status_standby;
        // return 0;
    }else{
        g_bIsPowerGo = 1;
        dp_update_powergo(g_bIsPowerGo);

        switch (g_iSetMode)
        {
            case mode_random:        sts = status_smart_clean; break; // 随机
            case mode_smart:         sts = status_smart_clean; break; // 自动
            case mode_wall_follow:   sts = status_wall_clean;  break; // 沿边
            case mode_spiral:        sts = status_spot_clean;   break;
            case mode_mop:           sts = status_mop_clean;   break;

            case mode_left_spiral:    
            case mode_right_spiral:   
            case mode_right_bow:      
            case mode_left_bow:       
            case mode_partial_bow:    sts = status_smart_clean;break;

            case mode_chargego:      sts = status_charging;break; 
            default: break;
        }
  
        //回充模式，这里不上报地图
        if(g_iSetMode!=mode_chargego){
                start_clean();
        }
    }

    dp_update_status(sts);
    pthread_mutex_unlock(&mutex);    
    return data;
}

int MCU_SetDirect(int data)
{
    return data;
}

int MCU_ResetEdge(int data)
{
    g_nEdgeBrush = 100;

    dp_update_edge_brush(g_nEdgeBrush);

    return data;
}

int MCU_ResetRoll(int data)
{
    g_nRollBrush = 100;

    dp_update_roll_brush(g_nRollBrush);

    return data;
}

int MCU_ResetFilter(int data)
{
    g_nFilter = 100;

    dp_update_edge_brush(g_nFilter);

    return data;
}

/*****************************************************************
 * @Function: MCU_SetSeek
 * @Description: 寻找机器功能，这里作为三个设备面板的进程切换，调试用
 * @Param: Do not edit
 * @Return: void
 *****************************************************************/
extern void gyro_dev_switch(void);
int MCU_SetSeek(int data)
{
        gyro_dev_switch();
        return data;
}

int MCU_SetSuction(int data)
{
    return data;
}

/*****************************************************************
 * @Function: MCU_UploadRecord
 * @Description: 上报清扫记录
 * @Param: Do not edit
 * @Return: void
 *****************************************************************/
int MCU_UploadRecord()
{
    OPERATE_RET op_ret;

    PR_NOTICE("g_nCleanTime:%d g_nCleanArea:%d",g_nCleanTime,g_nCleanArea);
    if((g_nCleanTime <= 0)|| (g_nCleanArea <= 0)){
        return -1;
    }

    // 获取当前时间
    POSIX_TM_S CurrentTm;
    op_ret =  uni_local_time_get_custom(0,&CurrentTm);
    if(op_ret != OPRT_OK){
        PR_ERR("uni_local_time_get->CurrentTm op_ret:%d\n", op_ret);
        return op_ret;
    }
    PR_DEBUG("CurrentTm:%d-%d-%d %d:%d:%d",CurrentTm.tm_year+1900,CurrentTm.tm_mon+1,CurrentTm.tm_mday,\
                                            CurrentTm.tm_hour,CurrentTm.tm_min,CurrentTm.tm_sec);
    CHAR_T clean_record[30];
#if 0 // MCU上报清扫记录时间
    snprintf(clean_record,30,"%d%02d%02d%02d%02d%03d%03d%05d",CurrentTm.tm_year+1900,CurrentTm.tm_mon+1,CurrentTm.tm_mday,\
                                                        CurrentTm.tm_hour,CurrentTm.tm_min,g_nCleanTime,g_nCleanArea,g_iCurMapId);
#else  // MCU不上报清扫记录时间
    snprintf(clean_record,30,"%03d%03d%05d",g_nCleanTime,g_nCleanArea,g_iCurMapId);
#endif
    PR_NOTICE("clean_record: %s\n",clean_record);

    dp_update_clean_record(clean_record, strlen(clean_record));

    return 0;
}


//////////////////////////////////////////////////////////////////
/*****************************************************************
 * @Function: OpenDemoData
 * @Description: 从磁盘文件中读取地图数据到内存
 * @Param: Do not edit
 * @Return: void
 *****************************************************************/
void OpenDemoData(void)
{
     // 读取地图
    FILE * infile = fopen(GYRO_DEMO_FILE_RECORD, "rt");
    if (NULL == infile){
        return;
    }

    long curpos = 0;
    long length = 0;
    curpos = ftell(infile);  
    fseek(infile, 0L, SEEK_END);
    length = ftell(infile);  
    fseek(infile, curpos, SEEK_SET);  

    char *pbuffer = (char *)malloc(length);
    fread(pbuffer, sizeof(char), length, infile);
    fclose(infile);
    infile = NULL;

    int i = 0,j = 0;
    int WTLEN = 6;
    char cdata[3] = {0};

    for (i = 0; i < length - WTLEN; i = i + WTLEN)
    {
        cdata[0] = pbuffer[i];
        cdata[1] = pbuffer[i+1];
        m_szMap[j].x = strtol(cdata, NULL, 16);

        cdata[0] = pbuffer[i+2];
        cdata[1] = pbuffer[i+3];
        m_szMap[j].y = strtol(cdata, NULL, 16);

        cdata[0] = pbuffer[i+4];
        cdata[1] = pbuffer[i+5];
        m_szMap[j].type = strtol(cdata, NULL, 16);

        //printf("point %d  x  %d,  y  %d,  type  %d \n", j, m_szMap[j].x, m_szMap[j].y, m_szMap[j].type);

        j++;
    }

    g_nMapLength = length / WTLEN;

    printf("Success Read BYTES = %d \n", length);

}

void StartWork()
{
    pthread_t thread_tuya_iot;
    pthread_t thread_robot_run;

    // 创建Tuya SDK主线程
    int ret = pthread_create(&thread_tuya_iot, NULL, tuya_iot_soc_dev_entry, NULL);
    if (ret)
    {
        printf("ERROR create thread_tuya_iot ERROR!\n");
        return;
    }

    // 创建扫地机工作线程
    ret = pthread_create(&thread_robot_run, NULL, threadAutoRun, NULL);
    if (ret)
    {
        printf("ERROR create threadAutoRun ERROR!\n");
        return;
    }

    sleep(2);

//     MCU_Update_Init();
}

void update_real_time_data(int cycle)
{
    g_nEdgeBrush = 100 - cycle * 2 * 0.01;
    g_nRollBrush = g_nEdgeBrush;
    g_nFilter = g_nEdgeBrush;

    g_nElectricity = 100 - cycle * 2 * 0.01;
    g_nCleanArea = cycle * 2 * 0.03;
    g_nCleanTime = cycle * 2 / 60;

    if (cycle % 2 == 0)
    {
        dp_update_area(g_nCleanArea);
        dp_update_time(g_nCleanTime);
        dp_update_electricity(g_nElectricity);
    }

    if (cycle % 15 == 0)
    {
        dp_update_edge_brush(g_nEdgeBrush);
        dp_update_roll_brush(g_nRollBrush);
        dp_update_filter(g_nFilter);
    }
}

void upload_dev_fault(int bres)
{
    unsigned int curfault = 0x00;

    if (bres == 1)
    {
        unsigned short ifault[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

        curfault = ifault[rand() % 7];
    }
    PR_NOTICE("curfault:0x%x",curfault);    
    dp_update_fault(curfault);
}

void *threadAutoRun(void *args)
{
    int count = 0;
    int cycle = 0;
    int DELTA = 3;  // 单次上报的路径点

    while(1)
    {
        // 是否开始清扫
        if(g_bIsRun){
                // 更新清扫时间、清扫面积等实时数据
                update_real_time_data(cycle);

                // 机器故障上报
                if(count == 48){
                        upload_dev_fault(1);
                }
                else if(count == 60){
                        // upload_dev_fault(0);
                }

                // 超出当前地图文件最大路径数，停止清扫
                if (count > g_nMapLength - 20){
                        g_bIsPowerGo = 0;
                        g_bIsRun = 0;
                        dp_update_mode(mode_standby);
                        dp_update_status(status_standby);
                        dp_update_powergo(0);
                }

                ST_POT stPoint[DELTA];
                int i = 0;
                for(i = 0; i < DELTA; i++){
                        stPoint[i].x = m_szMap[count+i].x;
                        stPoint[i].y = m_szMap[count+i].y;
                        stPoint[i].type = m_szMap[count+i].type;

                        // PR_NOTICE("--i %d --mapid %d    pt count  %d x %d   y %d   type  %d ",i , g_iCurMapId, count, stPoint[i].x, stPoint[i].y, stPoint[i].type);
                }

                // 故障验证
                if( count == 6){
                    trans_flow_data(((BYTE_T *)stPoint)+1, DELTA*sizeof(ST_POT)-1);
                }else{
                    trans_flow_data(stPoint, DELTA*sizeof(ST_POT));
                }

                count = count + DELTA;
                cycle++;
        }
        usleep(1000*500);
    }
}



// 地图及流服务相关操作
//////////////////////////////////////////////////////////////////////////////////////

/*****************************************************************************
函数名称 : set_map_config
功能描述 : 设置地图配置参数k
输入参数 : pt 原点位置，size 地图大小
          格式：原点(XX) + 大小(XXXX)，共三个字节，0x00=左上角， 0x01=左下角
          如： 0x0000ff 表示，左上角、最大长宽值为255
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : MCU连接云端时上报
*****************************************************************************/
OPERATE_RET set_map_config(BYTE_T pt, BYTE_T size)
{
    UCHAR_T map_config[] = {pt, 0x00, size};

    TY_RECV_RAW_DP_S dp_raw;
    dp_raw.dpid = DPID_MAP_CONFIG;
    dp_raw.len = 3;
    memcpy((UCHAR_T*)dp_raw.data, map_config, 3);

    OPERATE_RET op_ret = dev_report_dp_raw_sync(NULL, dp_raw.dpid, dp_raw.data, dp_raw.len, 0);

    if (OPRT_OK != op_ret)
    {
        printf("set_map_config op_ret:%d/n", op_ret);
    }

    return op_ret;
}
// 开启流服务通道
// OPERATE_RET init_flow_channel()
// {
//     OPERATE_RET op_ret = tuya_iot_media_init();  
//     if (op_ret != OPRT_OK)
//     {
//         printf("tuya_iot_media_init is error %d/n",op_ret);
//     }
//     return op_ret;
// }

// 开启流数据传输
OPERATE_RET start_flow_trans(INT16_T map_id)
{
    FLOW_BODY_ST flow_data;
    memset(&flow_data, 0, SIZEOF(FLOW_BODY_ST));

    flow_data.id = map_id;
    flow_data.posix = uni_time_get_posix();
    flow_data.step = TS_START;
    flow_data.offset = 0;
    flow_data.len = 0;

    OPERATE_RET op_ret = tuya_iot_media_data_report(&flow_data, 5); // 超时5s

    if (op_ret != OPRT_OK)
    {
        printf("tuya_iot_media_data_report->TS_START is error %d\n", op_ret);
    }
    else
    {
        g_bIsFlowOpen = 1;

        g_iCurMapId = map_id;
    }

    return op_ret;
}


OPERATE_RET trans_flow_data(BYTE_T *pdata, int count)
{
    if(g_bIsFlowOpen == 0)
    {
        return 0;
    }
    int size = sizeof(FLOW_BODY_ST) + count;
    FLOW_BODY_ST* flow_data = (FLOW_BODY_ST*)malloc(size);
 
    flow_data->id = g_iCurMapId;
    flow_data->posix = uni_time_get_posix();
    flow_data->step = TS_TRANSFER;
    flow_data->offset = 0;
    flow_data->len = count;
    memcpy((BYTE_T*)flow_data->data, (BYTE_T*)pdata, flow_data->len);
    OPERATE_RET op_ret = tuya_iot_media_data_report(flow_data, 5); // 超时5s
    if (op_ret != OPRT_OK){
        PR_ERR("tuya_iot_media_data_report->TS_TRANSFER is error %d\n", op_ret);
    }
    
    #if 1
    printf("map_id:%d len:%d,",g_iCurMapId,flow_data->len);
    int i = 0;
    for(i = 0; i < flow_data->len; i++){
        printf("%02X ",flow_data->data[i]);
    }
    printf("\r\n");
    #endif

    free(flow_data);

    return op_ret;
}

OPERATE_RET end_flow_trans(INT16_T map_id)
{
    FLOW_BODY_ST flow_data;
    memset(&flow_data, 0, SIZEOF(FLOW_BODY_ST));

    flow_data.id = map_id;
    flow_data.posix = uni_time_get_posix();
    flow_data.step = TS_END;
    flow_data.offset = 0;
    flow_data.len = 0;

    OPERATE_RET op_ret = tuya_iot_media_data_report(&flow_data, 5); // 超时5s

    if (op_ret != OPRT_OK){
        PR_ERR("tuya_iot_media_data_report->TS_END is error %d\n", op_ret);
        // return op_ret;
    }
    return op_ret;
}