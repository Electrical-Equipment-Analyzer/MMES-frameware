#include "mbed.h"
#include "rtos.h"
#include "TextLCD.h"
#include "Joystick.h"

#include "FunctionPointer.h"

#include "Selection.h"
#include "Menu.h"
#include "Navigator.h"
#include <string>

#include "acceleration.h"

#include <math.h>

#include "state.h"

// Host PC Communication channels
Serial pc(P0_0, P0_1);

DigitalOut led2(P0_22);

Joystick joystick(P2_3, P0_15, P2_4, P0_16, P0_17);

TextLCD lcd(P2_12, P2_10, P2_5, P2_6, P2_7, P2_8, TextLCD::LCD16x2);

Acceleration acc(A0, A1, A2);

void led2_thread(void const *args) {
    while (true) {
        led2 = !led2;
        Thread::wait(500);
    }
}

void ain_thread(void const *args) {
    while (true) {
//        pc.printf("normalized: 0x%04X \r\n", ain.read_u16() >> 4);
//        Thread::wait(500);
    }
}

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

    Thread thread_led(led2_thread);
//    Thread thread_ain(ain_thread);

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
    menu_root.add(Selection(NULL, 0, &menu_test, menu_test.menuID));
    menu_root.add(Selection(NULL, 1, &menu_status, menu_status.menuID));
    menu_root.add(Selection(NULL, 2, &menu_setting, menu_setting.menuID));
    menu_root.add(Selection(NULL, 3, &menu_about, menu_about.menuID));

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
        joystick.poll();
    }
}
