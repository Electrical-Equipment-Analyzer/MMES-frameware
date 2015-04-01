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
#include "usbctl.h"


void test() {
}

DigitalOut led(P0_22);
void led2_thread(void const *args) {
    while (true) {
        led = !led;
        Thread::wait(500);
    }
}

int main() {
    pc.baud(115200);

    pc.printf("Booting...\r\n");

    State state(&lcd, &joystick, &conf);

    lcd.setPower(true);
    lcd.cls();
    lcd.setAddress(0, 0);
    lcd.printf("Taiwan Tech  UNI");
    lcd.setAddress(0, 1);
    lcd.printf(" Motor Detector");
    wait(1);

    Thread thread_led(led2_thread, NULL, osPriorityNormal, (DEFAULT_STACK_SIZE / 16));
//    Thread thread_usb(usb_thread, NULL, osPriorityNormal, (DEFAULT_STACK_SIZE * 2.25));

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

    FunctionPointer fun_set_date(&state, &State::setting_date);
    FunctionPointer fun_set_tmze(&state, &State::setting_timezone);
    FunctionPointer fun_set_motor_t(&state, &State::setting_motor_type);
    FunctionPointer fun_set_motor_s(&state, &State::setting_motor_spec);
    FunctionPointer fun_set_motor_r(&state, &State::setting_motor_rpms);
    FunctionPointer fun_set_task(&state, &State::setting_task);
    //Menu - Setting
    Menu menu_setting(" Setting", &menu_root);
    menu_setting.add(Selection(&fun_set_date, 0, NULL, "Date & Time"));
    menu_setting.add(Selection(&fun_set_tmze, 1, NULL, "Time Zone"));
    menu_setting.add(Selection(&fun_set_motor_t, 2, NULL, "Motor Type"));
    menu_setting.add(Selection(&fun_set_motor_s, 3, NULL, "Motor Spec"));
    menu_setting.add(Selection(&fun_set_motor_r, 4, NULL, "Motor rpms"));
    menu_setting.add(Selection(&fun_set_task, 5, NULL, "Task"));
    menu_setting.add(Selection(NULL, 6, NULL, " inch/mm"));
    menu_setting.add(Selection(NULL, 7, NULL, " USB"));
    menu_setting.add(Selection(NULL, 8, NULL, " Network"));

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
        if (usb.poll()) {
            navigator.printMenu();
        }
        joystick.poll();
    }
}
