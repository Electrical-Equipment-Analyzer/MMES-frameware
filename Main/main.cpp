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

DigitalOut led2(P0_22);
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
