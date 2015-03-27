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

Usbctl::Usbctl() :
        hid(8, 64, 0x1234, 0x0006, 0x0001, false) {
    send_report.length = 64;
    hid.connect(false);
}

#define USB_PACKET_DATA 60

void Usbctl::send(char *send, uint8_t length) {
    while (length > 0) {
        uint8_t sendLength = USB_PACKET_DATA < length ? USB_PACKET_DATA : length;
        memset(send_report.data, 0, send_report.length);
        memcpy(&send_report.data[4], send, sendLength);
        length -= sendLength;
        send += sendLength;
        send_report.data[0] = length > 0 ? 2 : 1;
        send_report.data[1] = sendLength;
        hid.send(&send_report);
    }
}

#define USB_LOG_LIST 0
#define USB_LOG_DATA 1
#define USB_LOG_FILE 2

void Usbctl::log() {
    char t[100];
    SDFileSystem sd(p5, p6, p7, P2_2, "sd");
    sd.disk_initialize();
    FILE *fp;

    switch (recv_report.data[1]) {
        case USB_LOG_LIST:
            fp = fopen("/sd/MDES/log/monitor.log", "r");
            if (fp == NULL) {
                pc.printf("Could not open file for write\r\n");
            } else {
                while (!feof(fp)) {
                    int r = fscanf(fp, "%99[^\r^\n]s", t);
                    if (r == 0) {
                        fgetc(fp);
                        fgetc(fp);
                        continue;
                    }
                    send(t, strlen(t));
                    pc.printf("%s\r\n", t);
                }
                fclose(fp);
            }
            break;
        case USB_LOG_DATA:
            DIR *d;
            struct dirent *p;
            d = opendir("/sd/MDES/data");
            if (d != NULL) {
                while ((p = readdir(d)) != NULL) {
                    send(p->d_name, strlen(p->d_name));
                    pc.printf(" - %s\r\n", p->d_name);
                }
                closedir(d);
            } else {
                pc.printf("Could not open directory!\n");
            }
            break;
        case USB_LOG_FILE:
            char name[33];
            long lSize;
            size_t result;
            strftime(name, 33, "/sd/MDES/data/%Y%m%d%H%M%S.adc", localtime((time_t*) &recv_report.data[4]));
            fp = fopen(name, "r");
            if (fp == NULL) {
                pc.printf("Could not open file for write\r\n");
            } else {
                fseek(fp, 0, SEEK_END);
                lSize = ftell(fp);
                rewind (fp);

                while (result > 0) {
                    result = fread(t, 1, 60, fp);
                    send(t, result);
                    pc.printf("%60s\r\n", t);
                }
                fclose(fp);
            }
            break;
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
                send_report.data[0] = 0;
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

