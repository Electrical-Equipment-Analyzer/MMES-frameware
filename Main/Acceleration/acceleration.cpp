#include "acceleration.h"

Acceleration::Acceleration() {
}

void Acceleration::sample() {
}

void Acceleration::print(Serial *console) {
        uint16_t i;
    {
        Sampling sampling(A0, A1, A2);

        sampling.start(500);
        while (!sampling.isStop()) {
        }

        for (i = 0; i < _SAMPLING_LENGTH; i++) {
//        pc.printf("%f, %f, %f, \r", acc.ax[i], acc.vx[i], acc.sx[i]);
            console->printf("%x, ", sampling.x[i]);
            if ((i & 0x07) == 0x07) {
                console->printf("\r\n");
                wait_ms(10);
            }
        }
        console->printf("==========\r\n");
    }

    double a[500];

    for (i = 0; i < 500; i++) {
        a[i] = i;
    }
    for (i = 0; i < 500; i++) {
        console->printf("%f, ", a[i]);
        if ((i & 0x07) == 0x07) {
            console->printf("\r\n");
            wait_ms(10);
        }
    }
    console->printf("end++++++++++\r\n");

}

//double math_avg(double *v, uint8_t length) {
//    double total = 0;
//    uint8_t i = length;
//    while (i--) {
//        total += *v++;
//    }
//    return total / length;
//}
//
//double math_voltage(double ad) {
//    return ad * _ACCELERATION_VCC / 0xFFF;
//}
//
//double math_g(double voltage, double avg) {
//    return (voltage - avg) / 0.016;
//}
//
//double math_a(double g) {
//    return g / 9.80665;
//}
//
//double math_v(double v, double a, double t) {
//    return v + (a * t);
//}
//
//double math_s(double v0, double v, double t) {
//    return (v0 + v) / 2 * t;
//}
//
//void mathArray_v(double *v, double *a, double t, uint8_t length) {
//    double zero = 0;
//    while (length--) {
//        zero = math_v(zero, *a++, t);
//        *v++ = zero;
//    }
//}
//
//void mathArray_s(double *s, double *v, double t, uint8_t length) {
//    double now, zero = 0;
//    while (length--) {
//        now = *v++;
//        *s++ = math_s(zero, now, t);
//        zero = now;
//    }
//}

