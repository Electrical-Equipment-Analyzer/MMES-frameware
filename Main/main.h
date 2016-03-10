/*
 * main.h
 *
 *  Created on: 2015/2/28
 *      Author: Leo
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "mbed.h"

#include "TextLCD.h"
#include "Joystick.h"
#include "config.h"

//#define BOARD_DEV

//Global Objects
static Serial pc(P0_0, P0_1);
static SPI spi(p5, p6, p7);
static I2C iic(P0_19, P0_20);
static Config conf(&iic);

#ifdef BOARD_V1.0

static Joystick joystick(P2_4, P2_5, P2_7, P2_8, P2_6);
static TextLCD lcd(P1_19, P1_20, P1_25, P1_26, P1_27, P1_28, TextLCD::LCD16x2);
#define FLASH_EN P0_6
#define SD_EN P0_5

#else

static Joystick joystick(P2_3, P0_15, P2_4, P0_16, P0_17);
//static TextLCD lcd(P1_18, P1_19, P1_20, P1_21, P1_22, P1_23, TextLCD::LCD16x2);
static TextLCD lcd(P1_18, P1_19, P1_26, P1_27, P1_28, P1_29, TextLCD::LCD16x2);
#define FLASH_EN P2_1
#define SD_EN P2_2

#endif

void mem();

#endif /* MAIN_H_ */
