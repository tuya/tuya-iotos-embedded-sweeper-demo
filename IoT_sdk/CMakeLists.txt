
# cmake版本 
CMAKE_MINIMUM_REQUIRED(VERSION 3.2)


ADD_DEFINITIONS(-w)

# ADD_COMPILE_OPTIONS(-std=c++11 -pthread -fno-stack-protector)
ADD_COMPILE_OPTIONS(-pthread )


# 交叉编译方式
SET(CMAKE_SYSTEM_NAME Linux)

# 根据PLATFORMS指定交叉编译器
if (PLATFORMS MATCHES "pc")
    message(STATUS "PLATFORMS is ${PLATFORMS}")
elseif (PLATFORMS MATCHES "arm")
    add_definitions(-DUSE_RASPI)
    SET(CMAKE_SYSTEM_PROCESSOR armhf)
    SET(CMAKE_C_COMPILER "arm-linux-gnueabihf-gcc")
    SET(CMAKE_C_COMPILER "arm-linux-gnueabihf-gcc")
    SET(CMAKE_CXX_COMPILER "arm-linux-gnueabihf-g++")
    SET(CMAKE_STRIP "arm-linux-gnueabihf-strip")
else (PLATFORMS MATCHES "pc")
    message(FATAL_ERROR "Cannot not support for \"${PLATFORMS}\"!!")
endif (PLATFORMS MATCHES "pc")


# 项目的名称
PROJECT(${APP})

# set(USER_SW_VER "\"${APP_VERSION}\"")
# add_definitions(-DUSER_SW_VER)
# message(STATUS "${USER_SW_VER}")

# 添加源文件中的宏，用于代码选择
if (APP MATCHES "gyro1")
    add_definitions(-DUSE_GYRO1)
elseif (APP MATCHES "gyro2")
    add_definitions(-DUSE_GYRO2)
elseif (APP MATCHES "gyro3")
    add_definitions(-DUSE_GYRO3)
else (APP MATCHES "gyro1")
    message(FATAL_ERROR "Cannot not support for \"${GYRO}\"!!")
endif (APP MATCHES "gyro1")

INCLUDE_DIRECTORIES(./src/sdk/include)
INCLUDE_DIRECTORIES(./src/sdk/include/hal)
INCLUDE_DIRECTORIES(./src/sdk/include/hal/driver)
INCLUDE_DIRECTORIES(./src/sdk/include/hal/system)
INCLUDE_DIRECTORIES(./src/common)
AUX_SOURCE_DIRECTORY(./src/common DIR_SRCS)
INCLUDE_DIRECTORIES(./src/sys_api)
AUX_SOURCE_DIRECTORY(./src/sys_api DIR_SRCS)

INCLUDE_DIRECTORIES(./src/gyro_wifi_dev)
AUX_SOURCE_DIRECTORY(./src/gyro_wifi_dev DIR_SRCS)
INCLUDE_DIRECTORIES(./src/gyro_mcu)
AUX_SOURCE_DIRECTORY(./src/gyro_mcu DIR_SRCS)
# 根据PLATFORMS选择库
if (PLATFORMS MATCHES "pc")
    LINK_DIRECTORIES(./src/sdk/lib_pc)
elseif (PLATFORMS MATCHES "arm")
    LINK_DIRECTORIES(./src/sdk/lib_arm)
else (PLATFORMS MATCHES "pc")
    message(FATAL_ERROR "Cannot not support for \"${PLATFORMS}\"!!")
endif (PLATFORMS MATCHES "pc")
LINK_LIBRARIES(tuya_iot)
LINK_DIRECTORIES(/usr/local/lib)
LINK_LIBRARIES(m)

# 编译选项
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -pthread -lm -lrt -Os -s -std=gnu99")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -pthread -lm -lrt -Os -s")
# SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread -lm -lrt -Os -g -std=gnu99 -rdynamic -funwind-tables -ffunction-sections")
# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -lm -lrt -Os -g -rdynamic -funwind-tables -ffunction-sections")


# 链接目标
ADD_EXECUTABLE(${PROJECT_NAME} ${DIR_SRCS})

