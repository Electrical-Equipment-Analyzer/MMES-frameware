/*
 * Test.cpp
 *
 *  Created on: 2016/2/25
 *      Author: D10307009
 */

#include "Test.h"
#include "main.h"
#include "Selection.h"

void mem() {
	uint64_t stack;
	uint64_t *heap = new uint64_t;
	pc.printf("mem: stack %x, heap %x, free %d\r\n", &stack, heap, &stack - heap);
	free(heap);
}

#include "wave_player.h"
#include "SDFileSystem.h"

AnalogOut DACout(p18);
wave_player waver(&DACout);

void test_wav() {
	mem();
//	SDFileSystem sd(P0_18, P0_17, P0_15, P0_16, "sd");
	SDFileSystem sd(PIN_SD_SI, PIN_SD_SO, PIN_SD_CK, PIN_SD_CS, "sd");
	sd.disk_initialize();

	FILE *fp = fopen("/sd/test.wav", "r");
	if (fp == NULL) {
		pc.printf("File couldn't open\n");
	}

	pc.printf("File Playing\n");
	lcd.cls();
	lcd.printf("Audio Testing");
	waver.play(fp);

	fclose(fp);
	sd.unmount();
}

#include "EthernetInterface.h"
#include "HTTPClient.h"
#include "HTTPFile.h"
#include "Base64.h"

#include "lpc_phy.h"

void stat() {
	uint32_t s = LPC_EMAC->Status;
	pc.printf("S:%x\r\n", s);
	pc.printf("IP Address is %s\r\n", eth.getIPAddress());
}

void test_eth_dcon() {
	pc.printf("eth dcon s\r\n");
	eth.disconnect();
	pc.printf("eth dcon e\r\n");
}

void test_base() {

	Base64 base64;
	//	const char * txt = "0123456789";
	//	size_t tens;
	//	char * txen;

	//	mem();
	//	txen = base64.Encode(txt, strlen(txt), &tens);
	//	pc.printf(txen);

	FILE *source = fopen("/sd/0.txt", "r");
	if (source == NULL) {
		pc.printf("Could not open file\r\n");
	}

	mem();
	FILE *base = fopen("/sd/0.tmp", "w");
	if (base == NULL) {
		pc.printf("Could not open file\r\n");
	}

	mem();
	base64.Encode(base, source);

	mem();
	fclose(source);
	fclose(base);
}

void test_eth() {
	SDFileSystem sd(PIN_SD_SI, PIN_SD_SO, PIN_SD_CK, PIN_SD_CS, "sd");
	sd.disk_initialize();

	FILE *tFile = fopen("/sd/test.xml", "r");
	if (tFile == NULL) {
		pc.printf("Could not open file\r\n");
	}

	char str[512] = "mmm\r\n";
	HTTPFile hFile(tFile);
	HTTPText inText(str, 512);

	HTTPClient http;

	int ret = http.post("http://192.168.0.100:8080/MotorWeb/StreamingImpl", hFile, &inText);
	pc.printf("Result: %s\n", str);
	if (!ret) {
		pc.printf("Page fetched successfully - read %d characters\n", strlen(str));
	} else {
		pc.printf("Error - ret = %d - HTTP return code = %d\n", ret, http.getHTTPResponseCode());
	}
	fclose(tFile);

	sd.unmount();
}

#include "SerRAM.h"

void test_ram() {
	lcd.cls();
	lcd.printf("SRAM Testing");
	pc.printf("sram");
	SerRAM sram(P1_24, P1_23, P1_20, P1_21, 1024);
	uint8_t i;
	uint8_t d[] = { 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0, 0x12, 0x34 };
	uint8_t r[10];
	pc.printf("read:");
	sram.read(0, (char*) &r, 10, false);
	for (i = 0; i < 10; i++) {
		pc.printf(" %x", r[i]);
	}
	pc.printf("\r\n");
	pc.printf("write\r\n");
	sram.write(0, (char*) &d, 10, false);
	pc.printf("read:");
	sram.read(0, (char*) &r, 10, false);
	for (i = 0; i < 10; i++) {
		pc.printf(" %x", r[i]);
	}
	pc.printf("\r\n");
	pc.printf("read:");
	sram.read(1, (char*) &r, 10, false);
	for (i = 0; i < 10; i++) {
		pc.printf(" %x", r[i]);
	}
	pc.printf("\r\n");
//	int r = sram.read(0);
//	pc.printf("r:%x", r);
}

const char * TXT_MENU = "Develop Test";

FunctionPointer fun_eth(&test_eth);
FunctionPointer fun_wav(&test_wav);
FunctionPointer fun_ram(&test_ram);
FunctionPointer fun_eth_stat(&stat);

Test::Test(Menu *parent) :
		m_menu(TXT_MENU, parent) {
	m_menu.add(Selection(&fun_eth, 0, NULL, " Ethernet"));
	m_menu.add(Selection(&fun_wav, 1, NULL, " Audio"));
	m_menu.add(Selection(&fun_ram, 2, NULL, " SRAM"));
}

Menu *Test::getMenu() {
	return &m_menu;
}
