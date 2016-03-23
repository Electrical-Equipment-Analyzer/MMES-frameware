/*
 * analysis.cpp
 *
 *  Created on: 2015/2/28
 *      Author: Leo
 */

#include "analysis.h"

#include "main.h"

#include "acceleration.h"

Acceleration acc;

void acquire() {
	lcd.setAddress(0, 1);
	lcd.printf("acquiring\n");
	pc.printf("sample\r\n");
	acc.sample();
	pc.printf("write\r\n");
	acc.write();
	pc.printf("count\r\n");
	acc.count();
//	pc.printf("check\r\n");
//	acc.check();
//	pc.printf("log\r\n");
//	acc.log();
//	acc.printFile();
}

void format_num(char *txt, float num) {
	if (num < 10) {
		sprintf(txt, "%.2f", num);
	} else if (num < 100) {
		sprintf(txt, "%.1f", num);
	} else if (num < 10000) {
		uint16_t n = (uint16_t) num;
		sprintf(txt, "%4d", n);
	} else {
		sprintf(txt, "OVER");
	}
}

void test_ISO() {
	lcd.cls();
	lcd.printf("Processing\n");
	acquire();
	float data_v[3] = { acc._v_x_rms * 1000, acc._v_y_rms * 1000, acc._v_z_rms * 1000 };
	float data_s[3] = { acc._s_x_vpp * 1000000, acc._s_y_vpp * 1000000, acc._s_z_vpp * 1000000 };
	uint8_t i;
	char txt[5];

	lcd.cls();
	lcd.printf("I");
	for (i = 0; i < 3; i++) {
		format_num(txt, data_v[i]);
		lcd.printf(" %s", txt);
	}
	lcd.printf("N");
	for (i = 0; i < 3; i++) {
		format_num(txt, data_s[i]);
		lcd.printf(" %s", txt);
	}
	while (joystick.getStatus() == Joystick::none)
		;
}

