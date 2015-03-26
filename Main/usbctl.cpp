/*
 * usbctl.cpp
 *
 *  Created on: 2015/3/26
 *      Author: Leo
 */

#include "usbctl.h"

#include "main.h"
#include "SDFileSystem.h"
#include "Sampling.h"

typedef enum {
    LOG_LIST, LOG_DATA, CONTROL
} USB_COMMAND;

Usbctl::Usbctl() : hid(8, 64, 0x1234, 0x0006, 0x0001, false) {
    send_report.length = 64;
    hid.connect(false);
}



void Usbctl::log() {
    char t[100];
    SDFileSystem sd(p5, p6, p7, P2_2, "sd");
    sd.disk_initialize();

    FILE *fp = fopen("/sd/MDES/log/monitor.log", "r");
    if (fp == NULL) {
        pc.printf("Could not open file for write\r\n");
    } else {
        while (!feof(fp)) {
            fscanf(fp, "%99[^\r^\n]s", t);
            fgetc(fp);
            char c = fgetc(fp);
            pc.printf("%s %c\r\n", t, c);
        }

        fclose(fp);
    }

    sd.unmount();
}


void Usbctl::usb_ad() {
    uint8_t length = 10;

    uint16_t x[length];
    uint16_t y[length];
    uint16_t z[length];

    Sampling sampling(A0, A1, A2, 10);
    sampling.setbuf(x, y, z);

    while (true) {
        if (hid.readNB(&recv_report)) {
            if (recv_report.data[0] != CONTROL) {
                break;
            }
            while (!sampling.isStop()) {
            }
            memcpy(send_report.data, &x, sizeof(x));
            sampling.start(1000000.0f / 10000);
            hid.send(&send_report);
        }
    }

}

bool Usbctl::poll() {
    bool state = false;
    while (hid.readNB(&recv_report)) {
        state = true;
        lcd.cls();
        lcd.printf("USB Mode");

        switch (recv_report.data[0]) {
            case LOG_LIST:
                log();
                hid.send(&send_report);
                break;
            case LOG_DATA:
                wait(2);
                hid.send(&send_report);
                break;
            case CONTROL:
                usb_ad();
                hid.send(&send_report);
                break;
        }
    }
    return state;
}

