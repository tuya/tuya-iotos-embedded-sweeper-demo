#!/bin/bash

# https://www.raspberrypi.org/documentation/usage/gpio/README.md
# GPIO with C/C++ using 3rd party library pigio

# pigpio daemon 设置为自启动
# v=$(pigs hwver)

# if [[ $v < 0 ]]
# then
#    echo "The pigpio daemon wasn't found.  Did you sudo pigpiod?"
#    sudo pigpiod
#    exit
# fi