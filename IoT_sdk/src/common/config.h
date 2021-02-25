// 用于配置文件夹路径，适配不同的PC 
#ifndef _CONFIG_H
#define _CONFIG_H
/////////////////////////////////////////////////////////////////////////////////////////////////////
// gyro1工程的配置
#define GYRO1_UUID                "tuya7e38d79792c24b0d"  
#define GYRO1_AUTHKEY             "8xeh7y2I7MV3xmtX18AxurBpPmGzweRV"
#define GYRO1_CFG_STORAGE_PATH    "./tydb/gyro1_"   
#define GYRO1_DEMO_FILE_RECORD    "./files/gyro1/dev_smart_1.txt"

// gyro2工程的配置
#define GYRO2_UUID                "tuyaa9eb4a467b5d4ba5"
#define GYRO2_AUTHKEY             "sVwVTXWrh1Duc5zRG1mgWrHEiiv9moAO"
#define GYRO2_CFG_STORAGE_PATH    "./tydb/gyro2_"   
#define GYRO2_DEMO_FILE_RECORD    "./files/gyro1/dev_smart_1.txt"

// gyro3工程的配置
#define GYRO3_UUID                "tuyac40769a7b1d9fe78"
#define GYRO3_AUTHKEY             "nfkAN09PufB7jAH6z5cCKxDG1AeTBH7h"
#define GYRO3_CFG_STORAGE_PATH    "./tydb/gyro3_"   
#define GYRO3_DEMO_FILE_RECORD    "./files/gyro1/dev_smart_1.txt"

#if defined(USE_GYRO1)
        #define GYRO_UUID                GYRO1_UUID
        #define GYRO_AUTHKEY             GYRO1_AUTHKEY
        #define GYRO_CFG_STORAGE_PATH    GYRO1_CFG_STORAGE_PATH   
        #define GYRO_DEMO_FILE_RECORD    GYRO1_DEMO_FILE_RECORD
#elif defined(USE_GYRO2)
        #define GYRO_UUID                GYRO2_UUID
        #define GYRO_AUTHKEY             GYRO2_AUTHKEY
        #define GYRO_CFG_STORAGE_PATH    GYRO2_CFG_STORAGE_PATH   
        #define GYRO_DEMO_FILE_RECORD    GYRO2_DEMO_FILE_RECORD
#elif defined(USE_GYRO3)
        #define GYRO_UUID                GYRO3_UUID
        #define GYRO_AUTHKEY             GYRO3_AUTHKEY
        #define GYRO_CFG_STORAGE_PATH    GYRO3_CFG_STORAGE_PATH   
        #define GYRO_DEMO_FILE_RECORD    GYRO3_DEMO_FILE_RECORD
# else
        
#endif

#endif