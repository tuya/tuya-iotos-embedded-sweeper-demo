#!/bin/bash

# cmake命令路径
CMAKE=/usr/bin/cmake

# 编译代码
build() {
	APP=$(echo $1 | sed 's/[/]//g')
	echo && echo "[INFO] build robot test app for $APP "
    # export PATH=$PATH
    export PATH=$PATH:$TOOLCHAIN_PATH
    if [ -d build ]; then
        echo "build"
    else
        echo "mkdir build"
        mkdir build
    fi
    cd build
    $CMAKE -G "Unix Makefiles" .. -DAPP=$APP -DPLATFORMS=$2  -DAPP_VERSION=$APP_VER
    if [ $? = 0 ] ; then
        make clean
        make
        echo "[INFO] build finish."
    fi
    # 可执行文件拷贝到bin目录
    mv ./$APP ../bin/${APP}"_"${2}"_"${APP_VER}
    echo "compile success!"
    ls ../bin -l
}

echo "==================Tuya Gyro Demo Project Compile Menu=================="
echo "      1 -> compile gyro1 in Raspi board"
echo "      2 -> compile gyro2 in Raspi board"
echo "      3 -> compile gyro3 in Raspi board"
echo "      4 -> compile gyro1 in ubuntu pc"
echo "      5 -> compile gyro2 in ubuntu pc"
echo "      6 -> compile gyro3 in ubuntu pc"
echo "      7 -> clean build file"
echo ""
read -p "Please enter the command to execute: "   EXE_CMD
read -p "Please enter the app to version: "   APP_VER
case $EXE_CMD in
"1" | "2" | "3")
    rm -rf ./build/*
    PROJECT="gyro"${EXE_CMD}
    TOOLCHAIN_PATH=/home/samba_share/LinusZhao/toolchain/arm-linux-gnueabihf-4.8.3/bin/
    build $PROJECT arm;;
"4")
    rm -rf ./build/*
    PROJECT="gyro1"
    build $PROJECT pc;;
"5")
    rm -rf ./build/*
    PROJECT="gyro2" 
    build $PROJECT pc;;
"6")
    rm -rf ./build/*
    PROJECT="gyro3"
    build $PROJECT pc;;
"7")
    rm -rf ./build/*
	echo "clean complete";;
*)
    echo "Sorry,cannot not support for \"$EXE_CMD\"!!"
    exit 1;;
esac

exit 0
