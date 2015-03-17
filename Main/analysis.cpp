/*
 * analysis.cpp
 *
 *  Created on: 2015/2/28
 *      Author: Leo
 */

#include "analysis.h"

#include "main.h"

#include "acceleration.h"

Acceleration acc;

void acquire() {
    acc.sample();
    acc.count();
    acc.log();
    acc.write();
}

void test_ISO() {
    lcd.cls();
    lcd.printf(" ISO-10816\n");
    acquire();
    lcd.printf("%.2f %.2f %.2f", acc._v_x_rms * 1000, acc._v_y_rms * 1000, acc._v_z_rms * 1000);
    while (joystick.getStatus() == Joystick::none)
        ;
}

void test_NEMA() {
    lcd.cls();
    lcd.printf(" NEMA MG1\n");
    acquire();
    lcd.printf("%3.2f %3.2f %3.2f", acc._s_x_vpp * 1000000, acc._s_y_vpp * 1000000, acc._s_z_vpp * 1000000);
    while (joystick.getStatus() == Joystick::none)
        ;
}

