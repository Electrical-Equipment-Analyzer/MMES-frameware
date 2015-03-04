#!/bin/bash

hg clone http://developer.mbed.org/users/mbed_official/code/mbed/
ls -d mbed/* |grep TARGET |grep -v 1768 |xargs rm -rf
ls -d mbed/TARGET*/* |grep TOOLCHAIN |grep -v GCC_CR |xargs rm -rf

rm -rf Library
mkdir Library
cd Library

hg clone http://developer.mbed.org/users/mbed_official/code/mbed-rtos/
rm -rf mbed-rtos/rtx/TARGET_CORTEX_A
rm -rf mbed-rtos/rtx/TARGET_CORTEX_M/TARGET_M0
rm -rf mbed-rtos/rtx/TARGET_CORTEX_M/TARGET_M0P
rm -rf mbed-rtos/rtx/TARGET_CORTEX_M/TARGET_M3/TOOLCHAIN_ARM
rm -rf mbed-rtos/rtx/TARGET_CORTEX_M/TARGET_M3/TOOLCHAIN_IAR
rm -rf mbed-rtos/rtx/TARGET_CORTEX_M/TARGET_M4

hg clone http://developer.mbed.org/teams/mbed-official/code/FATFileSystem/
hg clone http://developer.mbed.org/users/neilt6/code/SDFileSystem/
hg clone http://developer.mbed.org/users/shintamainjp/code/ConfigFile/
hg clone http://developer.mbed.org/users/okini3939/code/at45db161d/
hg clone http://developer.mbed.org/users/wim/code/TextLCD/

hg clone http://developer.mbed.org/users/LeoHsueh/code/Digital-Joystick/
hg clone http://developer.mbed.org/users/LeoHsueh/code/TextMenu/

