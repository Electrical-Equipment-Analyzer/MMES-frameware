#include "acceleration.h"
#include "main.h"

#include "at45db161d.h"
ATD45DB161D flash(&spi, P2_2);

Acceleration::Acceleration() {
}

void Acceleration::sample() {
    uint16_t i;

    Sampling sampling(A0, A1, A2);

    sampling.start(200);
    while (!sampling.isStop()) {
    }

    for (i = 0; i < _SAMPLING_LENGTH; i++) {
//        pc.printf("%f, %f, %f, \r", acc.ax[i], acc.vx[i], acc.sx[i]);
        sampling.y[i] = i;
        pc.printf("%x, ", sampling.x[i]);
        if ((i & 0x07) == 0x07) {
            pc.printf("\r\n");
            wait_ms(10);
        }
    }
    pc.printf("==========\r\n");

    uint16_t ps = flash.getInfo()->pageSize;

    for (i = 2; i < 8; i++) {
        flash.BlockErase(i);
    }
    flash.write(ps * 16, sampling.x, sizeof(sampling.x));
    flash.write(ps * 32, sampling.y, sizeof(sampling.y));
    flash.write(ps * 48, sampling.z, sizeof(sampling.z));
}

void flash_adc_vdc(uint16_t from, uint16_t to, uint16_t length) {
    uint16_t i;
    uint16_t ps = flash.getInfo()->pageSize;
    uint16_t offset = 0;
    uint16_t row[ps];
    double d[ps];
    while (offset < length) {
        flash.read(ps * from + offset * 2, row, sizeof(row));
        for (i = 0; i < ps; i++) {
            d[i] = row[i] * _SAMPLING_VCC / 0xFFF;
        }
        flash.write(ps * to + offset * 8, d, sizeof(d));
        offset += ps;
    }
}

void flash_vdc_vac(uint16_t from, uint16_t to, uint16_t length) {
    uint16_t ps = flash.getInfo()->pageSize;
    uint16_t i;
    uint16_t offset;
    double d[ps];
    double avg = 0;
    offset = 0;
    while (offset < length) {
        flash.read(ps * from + offset * 8, d, sizeof(d));
        for (i = 0; i < ps; i++) {
            avg += d[i];
        }
        offset += ps;
    }
    avg /= length;
    offset = 0;
    while (offset < length) {
        flash.read(ps * from + offset * 8, d, sizeof(d));
        for (i = 0; i < ps; i++) {
            d[i] -= avg;
        }
        flash.write(ps * to + offset * 8, d, sizeof(d));
        offset += ps;
    }
}

void flash_print(uint16_t addr_x, uint16_t addr_y, uint16_t addr_z) {
    uint16_t i;
    uint16_t ps = flash.getInfo()->pageSize;
    uint16_t offset = 0;
    uint16_t length = ps;
    while (offset < _SAMPLING_LENGTH) {
        double x[length];
        double y[length];
        double z[length];
        flash.read(ps * addr_x + offset * 8, x, sizeof(x));
        flash.read(ps * addr_y + offset * 8, y, sizeof(y));
        flash.read(ps * addr_z + offset * 8, z, sizeof(z));
        for (i = 0; i < length; i++) {
            pc.printf("%d : %f, %f, %f, \r\n", i + offset, x[i], y[i], z[i]);
            wait_ms(1);
        }
        offset += length;
    }
}

#define _flash_page_row_x 16;

void Acceleration::print(Serial *console) {

    flash_adc_vdc(16, 64, _SAMPLING_LENGTH);
    flash_adc_vdc(32, 128, _SAMPLING_LENGTH);
    flash_adc_vdc(48, 192, _SAMPLING_LENGTH);

    flash_vdc_vac(64, 256, _SAMPLING_LENGTH);
    flash_vdc_vac(128, 320, _SAMPLING_LENGTH);
    flash_vdc_vac(192, 384, _SAMPLING_LENGTH);

    flash_print(64, 128, 192);
    flash_print(256, 320, 384);

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

