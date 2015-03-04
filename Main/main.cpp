
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

#include "ConfigFile.h"
ConfigFile cfg;

#include "SDFileSystem.h"
SDFileSystem sd(p5, p6, p7, P2_2, "sd"); // the pinout on the mbed Cool Components workshop board


#include "at45db161d.h"

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

void test_sdcf() {

    pc.printf("Hello World!\n");

    mkdir("/sd/mydir", 0777);

    FILE *fp = fopen("/sd/mydir/sdtest.txt", "w");
    if (fp == NULL) {
        pc.printf("Could not open file for write\n");
    }
    fprintf(fp, "Hello fun SD Card World!");
    fclose(fp);

    pc.printf("Goodbye World!\n");

    char *key = "MyKey";
    char value[BUFSIZ];
    /*
     * Read a configuration file from a mbed.
     */
    if (!cfg.read("/sd/output.cfg")) {
        pc.printf("Failure to read a configuration file.\n");
    }

    /*
     * Get a configuration value.
     */
    if (cfg.getValue(key, &value[0], sizeof(value))) {
        pc.printf("'%s'='%s'\n", key, value);
    }

    /*
     * Set a configuration value.
     */
    if (!cfg.setValue("MyKey", "TestValue")) {
        pc.printf("Failure to set a value.\n");
    }

    /*
     * Write to a file.
     */
    if (!cfg.write("/sd/output.cfg")) {
        pc.printf("Failure to write a configuration file.\n");
    }
}


#undef PAGE_SIZE
#define PAGE_SIZE 264 // AT45DB081D (1MB)
//#define PAGE_SIZE 528 // AT45DB321D (4MB)
#define PAGE_NUM 4095 // AT45DB081D (1MB)
//#define PAGE_NUM 8192 // AT45DB321D (4MB)

#define WRITE_BUFFER 1
#define READ_BUFFER 2

SPI spi(p5, p6, p7); // mosi, miso, sclk
ATD45DB161D memory(spi, P2_2);

void flash_write (int addr, char *buf, int len) {
    int i;
    memory.BufferWrite(WRITE_BUFFER, addr % PAGE_SIZE);
    for (i = 0; i < len; i ++) {
        spi.write(buf[i]);
    }
    memory.BufferToPage(WRITE_BUFFER, addr / PAGE_SIZE, 1);
}

void flash_read (int addr, char *buf, int len) {
    int i;
    memory.PageToBuffer(addr / PAGE_SIZE, READ_BUFFER);
    memory.BufferRead(READ_BUFFER, addr % PAGE_SIZE, 1);
    for (i = 0; i < len; i ++) {
        buf[i] = spi.write(0xff);
    }
}

void test_rom() {
    int i;
    char buf[PAGE_SIZE];
    Timer t;
    ATD45DB161D::ID id;

    spi.frequency(10000000);
    wait_ms(500);

    memory.ReadManufacturerAndDeviceID(&id);
    pc.printf("RAM Manufacturer ID : %02x\r\n", id.manufacturer);
    pc.printf("RAM Device ID : %02x %02x\r\n", id.device[0], id.device[1]);
    wait_ms(10);

    pc.printf("\r\nHELLO test\r\n");

    pc.printf("RAM write\r\n");
    strcpy(buf, "Hello!");
    flash_write(0, buf, 6);
    for (i = 0; i < PAGE_SIZE; i ++) {
        buf[i] = i;
    }
    flash_write(6, buf, PAGE_SIZE - 6);

    wait(1);
    memset(buf, 0, PAGE_SIZE);

    pc.printf("RAM read\r\n");
    flash_read(0, buf, PAGE_SIZE);
    for (i = 0; i < PAGE_SIZE; i ++) {
        pc.printf(" %02x", buf[i]);
        if ((i & 0x0f) == 0x0f)
            pc.printf("\r\n");
    }

    wait(1);

    pc.printf("\r\nWrite/Read time\r\n");

    pc.printf("RAM write\r\n");
    t.reset();
    t.start();
    for (i = 0; i < 0x20000; i += PAGE_SIZE) {
        buf[0] = (i >> 8) & 0xff;
        flash_write(i, buf, PAGE_SIZE);
        if ((i & 0x0fff) == 0) printf(".");
    }
    t.stop();
    pc.printf("\r\ntime %f, %f KBytes/sec\r\n", t.read(), (float)0x20000 / 1024 / t.read());

    wait(1);

    pc.printf("RAM read\r\n");
    t.reset();
    t.start();
    for (i = 0; i < 0x20000; i += PAGE_SIZE) {
        flash_read(i, buf, PAGE_SIZE);
        if (buf[0] != ((i >> 8) & 0xff)) {
            pc.printf("error %d\r\n", i);
            break;
        }
        if ((i & 0x0fff) == 0) printf(".");
    }
    t.stop();
    pc.printf("\r\ntime %f, %f KBytes/sec\r\n", t.read(), 0x20000 / 1024 / t.read());
}

int main() {
    pc.baud(115200);

    test_rom();

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
