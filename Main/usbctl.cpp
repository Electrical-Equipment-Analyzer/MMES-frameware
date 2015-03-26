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



void usbtest() {
    char t[100];
    SDFileSystem sd(p5, p6, p7, P2_2, "sd");
    sd.disk_initialize();

    FILE *fp = fopen("/sd/MDES/log/monitor.log", "r");
    if (fp == NULL) {
        pc.printf("Could not open file for write\r\n");
    } else {
        fscanf(fp, "%100[^\r^\n]s", t);
        pc.printf("%s\r\n", t);
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

void Usbctl::poll() {
    if (hid.readNB(&recv_report)) {
        pc.printf("USB read : ");
        for (int i = 0; i < recv_report.length; i++) {
            pc.printf("%x ", recv_report.data[i]);
        }
        pc.printf("\r\n");

        switch (recv_report.data[0]) {
            case LOG_LIST:
                //                        read(&send_report.data);
                usbtest();
                hid.send(&send_report);
                break;
            case LOG_DATA:
                //                        test();
                hid.send(&send_report);
                break;
            case CONTROL:
                usb_ad();
                hid.send(&send_report);
                break;
        }
    }
}

