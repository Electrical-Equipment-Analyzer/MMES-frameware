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

#include "SDFileSystem.h"

//#define BOARD_V1_0

//Global Objects
static Serial pc(P0_0, P0_1);
static SPI spi(p5, p6, p7);
static I2C iic(P0_19, P0_20);
static Config conf(&iic);

#ifdef BOARD_V1_0

static Joystick joystick(P2_4, P2_5, P2_7, P2_8, P2_6);
static TextLCD lcd(P1_19, P1_20, P1_25, P1_26, P1_27, P1_28, TextLCD::LCD16x2);
#define FLASH_EN P0_6
#define SD_EN P0_5

#else

#define FLASH_EN P2_1

static const char* NAME_SD = "sd";
#define PIN_SD_SI P0_9
#define PIN_SD_SO P0_8
#define PIN_SD_CK P0_7
#define PIN_SD_CS P2_2


//static DigitalOut TestPIN(P3_25);

static Joystick joystick(P2_3, P0_15, P2_4, P0_16, P0_17);
//static Joystick joystick(P2_10, P1_31, P2_4, P2_3, P3_25);
//static TextLCD lcd(P1_18, P1_19, P1_20, P1_21, P1_22, P1_23, TextLCD::LCD16x2);
static TextLCD lcd(P1_18, P1_19, P1_26, P1_27, P1_28, P1_29, TextLCD::LCD16x2);

#endif

void mem();

#endif /* MAIN_H_ */
