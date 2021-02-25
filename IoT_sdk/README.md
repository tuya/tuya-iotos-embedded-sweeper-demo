# 工程：基于IoT_sdk陀螺仪扫地机demo

## 功能说明
- 支持AP配网模式
- 支持涂鸦陀螺仪三种陀螺仪面板
- 支持设备OTA
- 编译时可选是运行在ubuntu设备上还是树莓派

## 编译：
```shell
tuya_gyro_robot_demo$ sh build_app.sh 
==================Tuya Gyro Demo Project Compile Menu==================
      1 -> compile gyro1 in Raspi board
      2 -> compile gyro2 in Raspi board
      3 -> compile gyro3 in Raspi board
      4 -> compile gyro1 in ubuntu pc
      5 -> compile gyro2 in ubuntu pc
      6 -> compile gyro3 in ubuntu pc
      7 -> clean build file

Please enter the command to execute:
```

## 执行：
```shell

```

## 目录说明
```
rbtdemo$ tree -L 1
.
├── bin             可执行文件目录
├── build           cmake编译目录
├── build_app.sh    编译脚本
├── CMakeLists.txt  
├── files           地图坐标信息文件
├── README.md
├── src             源码
|    ├── common
|    ├── gyro_mcu
|    ├── gyro_wifi_dev
|    ├── sdk        iot_sdk静态库
|    └── sys_api
├── tuya_iot_iot_wifi_tls_arm-linux-gnueabihf-gcc-4.8.3_4.1.4.tar.gz ， iot_sdk包
└── tydb
```






