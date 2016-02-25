/*
 * Test.cpp
 *
 *  Created on: 2016/2/25
 *      Author: D10307009
 */

#include "Test.h"
#include "main.h"

extern void mem() {
	int stack;
	int *heap = new int;
	pc.printf("mem: stack %x, heap %x, free %x\r\n", &stack, heap,
			&stack - heap);
	free(heap);
}

#include "wave_player.h"
#include "SDFileSystem.h"

AnalogOut DACout(p18);
wave_player waver(&DACout);

void test() {
	mem();
	SDFileSystem sd(p5, p6, p7, SD_EN, "sd");
	sd.disk_initialize();
	mem();
	FILE *fp = fopen("/sd/test.wav", "r");
	if (fp == NULL) {
		pc.printf("File couldn't open\n");
	}
	mem();
	pc.printf("File Playing\n");
	waver.play(fp);
	mem();
//		fseek(fp, 0, SEEK_SET);  // set file poiter to beginning
	fclose(fp);
	mem();
	sd.unmount();
	mem();
}

#include "EthernetInterface.h"
#include "HTTPClient.h"
#include "HTTPSOAP.h"
#include "Base64.h"

void test_eth() {
	EthernetInterface eth;
	if (eth.getIPAddress()[0] == '\0') {
		eth.init();
	}
	eth.connect();
	pc.printf("IP Address is %s\r\n", eth.getIPAddress());

	mem();
//	SDFileSystem sd(p5, p6, p7, SD_EN, "sd");
//	sd.disk_initialize();
//	FILE *fp = fopen("/sd/test.wav", "r");
//	if (fp == NULL) {
//		pc.printf("File couldn't open\n");
//	}

	const char * txt = "0123456789";
	size_t tens;
	char * txen;

	Base64 base64;
	txen = base64.Encode(txt, strlen(txt), &tens);
	pc.printf(txen);

	SDFileSystem sd(p5, p6, p7, SD_EN, "sd");
	sd.disk_initialize();

	FILE *source = fopen("/sd/0.txt", "r");
	if (source == NULL) {
		pc.printf("Could not open file\r\n");
	}

	FILE *base = fopen("/sd/0.tmp", "w");
	if (base == NULL) {
		pc.printf("Could not open file\r\n");
	}

	base64.Encode(base, source);

	fclose(source);
	fclose(base);

	FILE *file_base = fopen("/sd/test.txt", "r");
	if (file_base == NULL) {
		pc.printf("Could not open file\r\n");
	}

	char str[512] = "mmm\r\n";
	HTTPMultipart outText("id");
	HTTPSOAP tt("myid", file_base);
	HTTPText inText(str, 512);

//    HTTPMap map;
//    map.put("Hello", "World");
//    map.put("test", "1234");
	HTTPClient http;

	int ret = http.post("http://192.168.0.100:8080/MotorWeb/StreamingImpl", tt,
			&inText);
	pc.printf("Result: %s\n", str);
	if (!ret) {
		pc.printf("Page fetched successfully - read %d characters\n",
				strlen(str));
	} else {
		pc.printf("Error - ret = %d - HTTP return code = %d\n", ret,
				http.getHTTPResponseCode());
	}

	fclose(file_base);
	sd.unmount();

//	sd.unmount();
//    TCPSocketConnection sock;
//    sock.connect("192.168.0.194", 5140);
//
//    char http_cmd[] = "GET /media/uploads/mbed_official/hello.txt HTTP/1.0\n\n";
//    sock.send_all(http_cmd, sizeof(http_cmd) - 1);
//
//    char buffer[100];
//    int ret;
//
//    pc.printf("Received from server:\r\n");
//    while (true) {
//        ret = sock.receive(buffer, sizeof(buffer) - 1);
//        if (ret <= 0) break;
//        buffer[ret] = '\0';
//        pc.printf("%s", buffer);
//    }
//    pc.printf("====================\r\n");
//
//    sock.close();

//	eth.disconnect();
}


