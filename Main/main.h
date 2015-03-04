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


//Global Objects
static Serial pc(P0_0, P0_1);
static Joystick joystick(P2_3, P0_15, P2_4, P0_16, P0_17);
static TextLCD lcd(P1_18, P1_19, P1_20, P1_21, P1_22, P1_23, TextLCD::LCD16x2);


#endif /* MAIN_H_ */
