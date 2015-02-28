/*
 * analysis.cpp
 *
 *  Created on: 2015/2/28
 *      Author: Leo
 */

#include "analysis.h"

#include "main.h"

#include "acceleration.h"

Acceleration acc(A0, A1, A2);


double rms(double *v, uint8_t length) {
    uint8_t i;
    double sum = 0.0;
    for (i = 0; i < length; i++)
        sum += v[i] * v[i];
    return sqrt(sum / length);
}

double fmax(double a, double b) {
    return (a > b ? a : b);
}

double fmin(double a, double b) {
    return (a < b ? a : b);
}

double max(double *v, uint8_t length) {
    double max = 0;
    while (length--) {
        max = fmax(max, *v++);
    }
    return max;
}

double min(double *v, uint8_t length) {
    double min = 0;
    while (length--) {
        min = fmin(min, *v++);
    }
    return min;
}

double vpp(double *v, uint8_t length) {
    return max(v, length) - min(v, length);
}

void acquire() {
    acc.start(500);
    while (!acc.isStop()) {
    }
    uint8_t i, j;
    for (i = 0; i < _ACCELERATION_LENGTH; i++) {
        pc.printf("%f, %f, %f, \r", acc.ax[i], acc.vx[i], acc.sx[i]);
        wait_ms(10);
    }
    pc.printf("x\r\n");
    pc.printf("Vrms:%f\r\n", rms(acc.vx, _ACCELERATION_LENGTH));
    pc.printf("Spp:%f\r\n", vpp(acc.sx, _ACCELERATION_LENGTH));
}

void test_ISO() {
    lcd.cls();
    lcd.printf(" ISO-10816\n");
    acquire();
    lcd.printf("%.2f %.2f %.2f", rms(acc.vx, _ACCELERATION_LENGTH) * 1000, rms(acc.vy, _ACCELERATION_LENGTH) * 1000,
            rms(acc.vz, _ACCELERATION_LENGTH) * 1000);
    while (joystick.getStatus() == Joystick::none);
}

void test_NEMA() {
    lcd.cls();
    lcd.printf(" NEMA MG1\n");
    acquire();
    lcd.printf("%3.2f %3.2f %3.2f", vpp(acc.sx, _ACCELERATION_LENGTH) * 1000000,
            vpp(acc.sy, _ACCELERATION_LENGTH) * 1000000, vpp(acc.sz, _ACCELERATION_LENGTH) * 1000000);
    while (joystick.getStatus() == Joystick::none);
}


