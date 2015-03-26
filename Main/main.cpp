#include "mbed.h"
#include "main.h"

#include "FunctionPointer.h"
#include "rtos.h"

#include "Selection.h"
#include "Menu.h"
#include "Navigator.h"
#include <string>

#include "state.h"
#include "analysis.h"

//#include "USBHID.h"
#include "SDFileSystem.h"

#include "usbctl.h"

void test() {
    char t[100];
//    time_t _timestamp = time(NULL);
//    strftime(t, 20, "%Y-%m-%dT%H:%M:%S", localtime(&_timestamp));

    SDFileSystem sd(p5, p6, p7, P2_2, "sd");
    sd.disk_initialize();

    FILE *fp = fopen("/sd/MDES/log/monitor.log", "r");
    if (fp == NULL) {
        pc.printf("Could not open file for write\r\n");
    } else {
//        getline(fp->, t);
        fscanf(fp, "%100[^\r^\n]s", t);
        pc.printf("%s\r\n", t);
        fclose(fp);
    }

    sd.unmount();
}

DigitalOut led(P0_22);
void led2_thread(void const *args) {
    while (true) {
        led = !led;
        Thread::wait(500);
    }
}

//typedef enum {
//    LOG_LIST, LOG_DATA, CONTROL
//} USB_COMMAND;


DigitalIn usb_en(P1_31);
//void usb_thread(void const *args) {
//    while (true) {
//        while (usb_en != 0) {
//            Thread::wait(1000);
//        }
//        pc.printf("USB init...\r\n");
//        USBHID hid(8, 8, 0x1234, 0x0006, 0x0001, false);
//        HID_REPORT send_report;
//        HID_REPORT recv_report;
//        send_report.length = 8;
//
//        pc.printf("USB connecting...\r\n");
//        hid.connect(false);
//
//        while (usb_en != 0) {
//            pc.printf("USB read...\r\n");
//            if (hid.read(&recv_report)) {
//                for (int i = 0; i < recv_report.length; i++) {
//                    pc.printf("%x ", recv_report.data[i]);
//                }
//                pc.printf("\r\n");
//
//                switch (recv_report.data[0]) {
//                    case LOG_LIST:
////                        read(&send_report.data);
//                        test();
//                        hid.send(&send_report);
//                        break;
//                    case LOG_DATA:
////                        test();
//                        hid.send(&send_report);
//                        break;
//                    case CONTROL:
//                        pc.printf("USB send...\r\n");
//                        for (int i = 0; i < send_report.length; i++)
//                            send_report.data[i] = rand() & 0xff;
//                        hid.send(&send_report);
//                        break;
//                }
//                continue;
//            }
//            Thread::wait(5000);
//        }
//        hid.disconnect();
//    }
//}

//#include "Sampling.h"

//USBHID hid(8, 64, 0x1234, 0x0006, 0x0001, false);
//HID_REPORT send_report;
//HID_REPORT recv_report;

//void usb_ad() {
//    uint8_t length = 10;
//
//    uint16_t x[length];
//    uint16_t y[length];
//    uint16_t z[length];
//
//    Sampling sampling(A0, A1, A2, 10);
//    sampling.setbuf(x, y, z);
//
//    while (true) {
//        if (hid.readNB(&recv_report)) {
//            if (recv_report.data[0] != CONTROL) {
//                break;
//            }
//            while (!sampling.isStop()) {
//            }
//            memcpy(send_report.data, &x, sizeof(x));
//            sampling.start(1000000.0f / 10000);
//            hid.send(&send_report);
//        }
//    }
//
//}

int main() {
    pc.baud(115200);

    pc.printf("Booting...\r\n");

    State state(&lcd, &joystick);

    lcd.setPower(true);
    lcd.cls();
    lcd.setAddress(0, 0);
    lcd.printf("Taiwan Tech  UNI");
    lcd.setAddress(0, 1);
    lcd.printf(" Motor Detector");
    wait(1);

    Thread thread_led(led2_thread, NULL, osPriorityNormal, (DEFAULT_STACK_SIZE / 16));
//    Thread thread_usb(usb_thread, NULL, osPriorityNormal, (DEFAULT_STACK_SIZE * 2.25));

//    send_report.length = 64;

//    pc.printf("USB connecting...\r\n");
//    hid.connect(false);
    Usbctl usb;

    //User Menu
    Menu menu_root(" Motor Detector", NULL);

    FunctionPointer fun_iso(&test_ISO);
    FunctionPointer fun_nema(&test_NEMA);

    //Menu - Run Test
    Menu menu_test(" Run Test", &menu_root);
    menu_test.add(Selection(&fun_iso, 0, NULL, " ISO-10816")); // ISO-10816
    menu_test.add(Selection(&fun_nema, 1, NULL, " NEMA MG1")); // NEMA MG1

    //Menu - Status
    Menu menu_status(" Status", &menu_root);
    menu_status.add(Selection(NULL, 0, NULL, " OK"));
    menu_status.add(Selection(NULL, 1, NULL, " XYZ"));

    FunctionPointer fun_sd(&state, &State::setting_date);
    //Menu - Setting
    Menu menu_setting(" Setting", &menu_root);
    menu_setting.add(Selection(NULL, 0, NULL, " inch/mm"));
    menu_setting.add(Selection(NULL, 1, NULL, " USB"));
    menu_setting.add(Selection(NULL, 2, NULL, " Network"));
    menu_setting.add(Selection(&fun_sd, 3, NULL, "Date/Time"));

    //Menu - About
    Menu menu_about(" About", &menu_root);
    menu_about.add(Selection(NULL, 0, NULL, " NTUST EE-305"));
    menu_about.add(Selection(NULL, 1, NULL, " Version: 0.1"));

    // Selections to the root menu should be added last
    FunctionPointer fun_test(&test);
    menu_root.add(Selection(NULL, 0, &menu_test, menu_test.menuID));
    menu_root.add(Selection(NULL, 1, &menu_status, menu_status.menuID));
    menu_root.add(Selection(NULL, 2, &menu_setting, menu_setting.menuID));
    menu_root.add(Selection(NULL, 3, &menu_about, menu_about.menuID));
    menu_root.add(Selection(&fun_test, 4, NULL, "Develop Test"));

    // Here is the heart of the system: the navigator.
    // The navigator takes in a reference to the root, an interface, and a reference to an lcd
    Navigator navigator(&menu_root, &lcd);

    FunctionPointer fun_none(&navigator, &Navigator::actionNone);
    FunctionPointer fun_up(&navigator, &Navigator::actionUp);
    FunctionPointer fun_down(&navigator, &Navigator::actionDown);
    FunctionPointer fun_back(&navigator, &Navigator::actionBack);
    FunctionPointer fun_enter(&navigator, &Navigator::actionEnter);
    FunctionPointer fun_home(&navigator, &Navigator::actionHome);

    joystick.functions(&fun_none, &fun_up, &fun_down, &fun_back, &fun_enter, &fun_home);

    while (true) {
//        if (hid.readNB(&recv_report)) {
//            pc.printf("USB read : ");
//            for (int i = 0; i < recv_report.length; i++) {
//                pc.printf("%x ", recv_report.data[i]);
//            }
//            pc.printf("\r\n");
//
//            switch (recv_report.data[0]) {
//                case LOG_LIST:
//                    //                        read(&send_report.data);
//                    test();
//                    hid.send(&send_report);
//                    break;
//                case LOG_DATA:
//                    //                        test();
//                    hid.send(&send_report);
//                    break;
//                case CONTROL:
//                    usb_ad();
//                    hid.send(&send_report);
//                    break;
//            }
//            continue;
//        }
        usb.poll();
        joystick.poll();
    }
}
