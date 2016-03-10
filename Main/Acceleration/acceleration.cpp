#include "acceleration.h"
#include "main.h"

#include "at45db161d.h"
ATD45DB161D flash(&spi, FLASH_EN);

#include "SDFileSystem.h"

#define _flash_page_adc_x 16
#define _flash_page_adc_y 32
#define _flash_page_adc_z 48

#define _flash_page_vdc_x 64
#define _flash_page_vdc_y 128
#define _flash_page_vdc_z 192
#define _flash_page_vac_x 256
#define _flash_page_vac_y 320
#define _flash_page_vac_z 384

#define _flash_page_g_x 256
#define _flash_page_g_y 320
#define _flash_page_g_z 384

#define _flash_page_a_x 448
#define _flash_page_a_y 512
#define _flash_page_a_z 576
#define _flash_page_v_x 640
#define _flash_page_v_y 704
#define _flash_page_v_z 768
#define _flash_page_s_x 832
#define _flash_page_s_y 896
#define _flash_page_s_z 960

Acceleration::Acceleration() : _sram(P1_24, P1_23, P1_20, P1_21, 1024) {
	_sps = 10000;
	_length = 1000;
	_channels = 3;
	_adc_high = 3.3;
	_adc_low = 0;
	_adc_bit = 12;
	_rate = 0.016;
	strcpy(_tag, "g");
}

void Acceleration::test() {
	pc.printf("class Acceleration : flash test start");
	uint16_t d[_length];
	uint16_t i;
	for (i = 0; i < _length; i++) {
		d[i] = i;
	}
	pc.printf("class Acceleration : write");
	flash.writeBuffer(0, d, sizeof(d));
	pc.printf("class Acceleration : read");
	flash.readBuffer(0, d, sizeof(d));

	pc.printf("class Acceleration : flash test end");
}

void Acceleration::sample() {
//	mem();
//	uint16_t x[_length];
//	uint16_t y[_length];
//	uint16_t z[_length];
//	uint16_t *x;
//	uint16_t *y;
//	uint16_t *z;
//	x = (uint16_t*) malloc(sizeof(uint16_t) * _length);
//	y = (uint16_t*) malloc(sizeof(uint16_t) * _length);
//	z = (uint16_t*) malloc(sizeof(uint16_t) * _length);
//	mem();

	size_t i;
	_timestamp = time(NULL);
	Sampling sampling(A4, A5, P0_3, _sram, _length);
//	sampling.setbuf(x, y, z);
	sampling.start(1000000.0f / _sps);
	while (!sampling.isStop()) {
	}
//	for (i = 2; i < 128; i++) {
//		flash.BlockErase(i);
//	}
//	flash.writeBuffer(_flash_page_adc_x, x, sizeof(x));
//	flash.writeBuffer(_flash_page_adc_y, y, sizeof(y));
//	flash.writeBuffer(_flash_page_adc_z, z, sizeof(z));

//	sampling.setbuf(NULL, NULL, NULL);

	sampling.print();
//    free(x);
//    free(y);
//    free(z);
}

double math_vac_g(double vac) {
	return vac / 0.016;
}

double math_g_a(double g) {
	return g * 9.80665;
}

void flash_adc_vdc(uint16_t from, uint16_t to, uint16_t length) {
	uint16_t i;
	uint16_t ps = flash.getInfo()->pageSize;
	uint16_t offset = 0;
	while (offset < length) {
		uint16_t row[ps];
		double d[ps];
		flash.readBuffer(from + (sizeof(uint16_t) * offset / ps), row,
				sizeof(row));
		for (i = 0; i < ps; i++) {
			d[i] = row[i] * 3.3f / 0xFFF;
		}
		flash.writeBuffer(to + (sizeof(double) * offset / ps), d, sizeof(d));
		offset += ps;
	}
}
void flash_vdc_vac(uint16_t from, uint16_t to, uint16_t length) {
	uint16_t ps = flash.getInfo()->pageSize;
	uint16_t i;
	uint16_t offset;
	double avg = 0;
	offset = 0;
	while (offset < length) {
		double d[ps];
		flash.readBuffer(from + (sizeof(double) * offset / ps), d, sizeof(d));
		for (i = 0; i < ps; i++) {
			avg += d[i];
		}
		offset += ps;
	}
	avg /= length;
	pc.printf("avg : %f\r\n", avg);
	offset = 0;
	while (offset < length) {
		double d[ps];
		flash.readBuffer(from + (sizeof(double) * offset / ps), d, sizeof(d));
		for (i = 0; i < ps; i++) {
			d[i] -= avg;
		}
		flash.writeBuffer(to + (sizeof(double) * offset / ps), d, sizeof(d));
		offset += ps;
	}
}

void flash_function(uint16_t from, uint16_t to, uint16_t length,
		double (*fun)(double)) {
	uint16_t ps = flash.getInfo()->pageSize;
	uint16_t i;
	uint16_t offset = 0;
	while (offset < length) {
		double d[ps];
		flash.readBuffer(from + (sizeof(double) * offset / ps), d, sizeof(d));
		for (i = 0; i < ps; i++) {
			d[i] = fun(d[i]);
		}
		flash.writeBuffer(to + (sizeof(double) * offset / ps), d, sizeof(d));
		offset += ps;
	}
}

void flash_integral(uint16_t from, uint16_t to, uint16_t length, double t) {
	uint16_t ps = flash.getInfo()->pageSize;
	uint16_t i;
	uint16_t offset = 0;
	double v = 0;
	while (offset < length) {
		double d[ps];
		flash.readBuffer(from + (sizeof(double) * offset / ps), d, sizeof(d));
		for (i = 0; i < ps; i++) {
			v += d[i] * t;
			d[i] = v;
		}
		flash.writeBuffer(to + (sizeof(double) * offset / ps), d, sizeof(d));
		offset += ps;
	}
}

double flash_rms(uint16_t from, uint16_t length) {
	uint16_t ps = flash.getInfo()->pageSize;
	uint16_t i;
	uint16_t offset = 0;
	double sum = 0;
	while (offset < length) {
		double d[ps];
		flash.readBuffer(from + (sizeof(double) * offset / ps), d, sizeof(d));
		for (i = 0; i < ps; i++) {
			sum += d[i] * d[i];
		}
		offset += ps;
	}
	return sqrt(sum / length);
}

double flash_vpp(uint16_t from, uint16_t length) {
	uint16_t ps = flash.getInfo()->pageSize;
	uint16_t i;
	uint16_t offset = 0;
	double max;
	double min;
	while (offset < length) {
		double d[ps];
		flash.readBuffer(from + (sizeof(double) * offset / ps), d, sizeof(d));
		for (i = 0; i < ps; i++) {
			max = max > d[i] ? max : d[i];
			min = min < d[i] ? min : d[i];
		}
		offset += ps;
	}
	return max - min;
}

void flash_print(uint16_t addr_x, uint16_t addr_y, uint16_t addr_z,
		uint16_t length) {
	uint16_t i;
	uint16_t ps = flash.getInfo()->pageSize;
	uint16_t offset = 0;
	while (offset < length) {
		double x[ps];
		double y[ps];
		double z[ps];
		flash.readBuffer(addr_x + (sizeof(double) * offset / ps), x, sizeof(x));
		flash.readBuffer(addr_y + (sizeof(double) * offset / ps), y, sizeof(y));
		flash.readBuffer(addr_z + (sizeof(double) * offset / ps), z, sizeof(z));
		for (i = 0; i < ps; i++) {
			pc.printf("%d : %f, %f, %f, \r\n", i + offset, x[i], y[i], z[i]);
			wait_ms(10);
		}
		offset += ps;
	}
}

void Acceleration::count() {
//	flash_print(_flash_page_vdc_x, _flash_page_vdc_y, _flash_page_vdc_z, _length);

	pc.printf("adc vdc\r\n");
	flash_adc_vdc(_flash_page_adc_x, _flash_page_vdc_x, _length);
	flash_adc_vdc(_flash_page_adc_y, _flash_page_vdc_y, _length);
	flash_adc_vdc(_flash_page_adc_z, _flash_page_vdc_z, _length);

//	flash_print(_flash_page_vdc_x, _flash_page_vdc_y, _flash_page_vdc_z, _length);
//	flash_print(_flash_page_vac_x, _flash_page_vac_y, _flash_page_vac_z, _length);

	pc.printf("vdc vac\r\n");
	flash_vdc_vac(_flash_page_vdc_x, _flash_page_vac_x, _length);
	flash_vdc_vac(_flash_page_vdc_y, _flash_page_vac_y, _length);
	flash_vdc_vac(_flash_page_vdc_z, _flash_page_vac_z, _length);

//	flash_print(_flash_page_vac_x, _flash_page_vac_y, _flash_page_vac_z, _length);

	pc.printf("vac g\r\n");
	flash_function(_flash_page_vac_x, _flash_page_g_x, _length, &math_vac_g);
	flash_function(_flash_page_vac_y, _flash_page_g_y, _length, &math_vac_g);
	flash_function(_flash_page_vac_z, _flash_page_g_z, _length, &math_vac_g);

	pc.printf("g a\r\n");
	flash_function(_flash_page_g_x, _flash_page_a_x, _length, &math_g_a);
	flash_function(_flash_page_g_y, _flash_page_a_y, _length, &math_g_a);
	flash_function(_flash_page_g_z, _flash_page_a_z, _length, &math_g_a);

	pc.printf("a v\r\n");
	double dt = 1.0f / _sps;
	flash_integral(_flash_page_a_x, _flash_page_v_x, _length, dt);
	flash_integral(_flash_page_a_y, _flash_page_v_y, _length, dt);
	flash_integral(_flash_page_a_z, _flash_page_v_z, _length, dt);

	pc.printf("v s\r\n");
	flash_integral(_flash_page_v_x, _flash_page_s_x, _length, dt);
	flash_integral(_flash_page_v_y, _flash_page_s_y, _length, dt);
	flash_integral(_flash_page_v_z, _flash_page_s_z, _length, dt);

	pc.printf("iso\r\n");
	_v_x_rms = flash_rms(_flash_page_v_x, _length);
	_v_y_rms = flash_rms(_flash_page_v_y, _length);
	_v_z_rms = flash_rms(_flash_page_v_z, _length);

	pc.printf("nema\r\n");
	_s_x_vpp = flash_vpp(_flash_page_s_x, _length);
	_s_y_vpp = flash_vpp(_flash_page_s_y, _length);
	_s_z_vpp = flash_vpp(_flash_page_s_z, _length);

//	flash_print(_flash_page_vdc_x, _flash_page_vac_x, _flash_page_g_x, _length);
//	flash_print(_flash_page_a_x, _flash_page_v_x, _flash_page_s_x, _length);
//    flash_print(_flash_page_vdc_x, _flash_page_vdc_y, _flash_page_vdc_z, _length);
//    flash_print(_flash_page_a_x, _flash_page_a_y, _flash_page_a_z, _length);
//    flash_print(_flash_page_v_x, _flash_page_v_y, _flash_page_v_z, _length);
//    flash_print(_flash_page_s_x, _flash_page_s_y, _flash_page_s_z, _length);
	pc.printf("iso  : %f, %f, %f\r\n", _v_x_rms, _v_y_rms, _v_z_rms);
	pc.printf("nema : %f, %f, %f\r\n", _s_x_vpp, _s_y_vpp, _s_z_vpp);
}

static const float ISO_10816[4][3] = { { 0.71, 1.8, 4.5 }, { 1.12, 2.8, 7.1 }, {
		1.8, 4.5, 11.2 }, { 2.8, 7.1, 18.0 } };
static const float NEMA_MG1[2][3][2] = { { { 18, 70 }, { 0, 90 } }, {
		{ 18, 50 }, { 12, 70 }, { 0, 76 } } };

void Acceleration::check() {
	uint8_t v_state;
	uint8_t s_state;
	double v_max = _v_x_rms;
	v_max = (v_max < _v_y_rms ? _v_y_rms : v_max);
	v_max = (v_max < _v_z_rms ? _v_z_rms : v_max);
	v_max *= 1000;
	double s_max = _s_x_vpp;
	s_max = (s_max < _s_y_vpp ? _s_y_vpp : s_max);
	s_max = (s_max < _s_z_vpp ? _s_z_vpp : s_max);
	s_max *= 1000000;
	uint8_t type = conf.get(Config::MOTOR_TYPE);
	for (v_state = 0; v_state < 3; v_state++) {
		if (v_max <= ISO_10816[type][v_state]) {
			break;
		}
	}
	uint8_t spec = conf.get(Config::MOTOR_SPEC);
	uint8_t rpms = conf.get(Config::MOTOR_RPMS);
	uint8_t i;
	for (i = 0; i < 3; i++) {
		if (rpms > NEMA_MG1[spec][i][0]) {
			break;
		}
	}
	s_state = s_max > NEMA_MG1[spec][i][1];
	pc.printf("state %d, %d\r\n", v_state, s_state);
	conf.set(Config::STATE, (s_state << 7) | v_state);
//    led_state[0] = (s_state << 7) | v_state;
//    led_state[0]++;
}

void Acceleration::log() {
	int16_t timezone = conf.get(Config::TIMEZONE);
	uint8_t minute = abs(timezone % 60);
	uint8_t hour = abs(timezone / 60);
	char zone[7];
	sprintf(zone, "%c%02d:%02d", (timezone < 0 ? '-' : '+'), hour, minute);
	time_t timestamp = _timestamp + (timezone * 60);
	char time[20];
	strftime(time, 20, "%Y-%m-%dT%H:%M:%S", localtime(&timestamp));

	SDFileSystem sd(p5, p6, p7, SD_EN, "sd");
	sd.disk_initialize();

	FILE *fp = fopen("/sd/MDES/log/monitor.log", "a");
	if (fp == NULL) {
		pc.printf("Could not open file for write\r\n");
	} else {
		fprintf(fp, "%s%s ISO: %f, %f, %f NEMA: %f, %f, %f\r\n", time, zone,
				_v_x_rms, _v_y_rms, _v_z_rms, _s_x_vpp, _s_y_vpp, _s_z_vpp);
		fclose(fp);
	}

	sd.unmount();
}

void flash_adc_file(uint16_t from, FILE *file, uint16_t length) {
	uint16_t ps = flash.getInfo()->pageSize;
	uint16_t offset = 0;
	while (offset < length) {
		uint16_t row[ps];
		flash.readBuffer(from + (sizeof(uint16_t) * offset / ps), row,
				sizeof(row));
		fwrite((const void*) &row, sizeof(uint16_t), ps, file);
		offset += ps;
	}
}

void Acceleration::write() {
	int16_t timezone = conf.get(Config::TIMEZONE);
	time_t timestamp = _timestamp + (timezone * 60);
	char name[33];
	strftime(name, 33, "/sd/MDES/data/%Y%m%d%H%M%S.adc", localtime(&timestamp));

	SDFileSystem sd(p5, p6, p7, SD_EN, "sd");
	sd.disk_initialize();

	FILE *fp = fopen(name, "w");
	if (fp == NULL) {
		pc.printf("Could not open file for write\r\n");
	} else {
		fwrite((const void*) &_timestamp, sizeof(time_t), 1, fp);
		fwrite((const void*) &_sps, sizeof(uint32_t), 1, fp);
		fwrite((const void*) &_length, sizeof(uint16_t), 1, fp);
		fwrite((const void*) &_channels, sizeof(uint8_t), 1, fp);
		fwrite((const void*) &_adc_high, sizeof(float), 1, fp);
		fwrite((const void*) &_adc_low, sizeof(float), 1, fp);
		fwrite((const void*) &_adc_bit, sizeof(uint8_t), 1, fp);
		fwrite((const void*) &_rate, sizeof(float), 1, fp);
		fwrite((const void*) &_tag, sizeof(char), 8, fp);
		flash_adc_file(_flash_page_adc_x, fp, _length);
		flash_adc_file(_flash_page_adc_y, fp, _length);
		flash_adc_file(_flash_page_adc_z, fp, _length);
		fclose(fp);
	}

	sd.unmount();
}
