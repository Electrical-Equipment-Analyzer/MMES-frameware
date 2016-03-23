#include "acceleration.h"
#include "main.h"

#include "SDFileSystem.h"

Acceleration::Acceleration() :
		_sram(P1_24, P1_23, P1_20, P1_21, 1024) {
}

void Acceleration::test() {
	pc.printf("class Acceleration : flash test start");
	uint16_t d[_file.length];
	uint16_t i;
	for (i = 0; i < _file.length; i++) {
		d[i] = i;
	}
	pc.printf("class Acceleration : write");
//	flash.writeBuffer(0, d, sizeof(d));
//	pc.printf("class Acceleration : read");
//	flash.readBuffer(0, d, sizeof(d));

	pc.printf("class Acceleration : flash test end");
}

void Acceleration::sample() {
	Sampling sampling(A0, A1, A2, _sram, _file.length);
	_file.timestamp = time(NULL);
	sampling.start(1000000.0f / _file.sps);
	while (!sampling.isStop()) {
	}
//	sampling.print();
}

void fileName(time_t time, char* name) {
	int16_t timezone = conf.get(Config::TIMEZONE);
	time_t timestamp = time + (timezone * 60);
	strftime(name, 33, "/sd/MDES/data/%Y%m%d%H%M%S.adc", localtime(&timestamp));
}

#define buff_len 8

double Acceleration::sram_vac(uint8_t ch) {
	char name[33];
	fileName(_file.timestamp, (char*) &name);

	SDFileSystem sd(PIN_SD_SI, PIN_SD_SO, PIN_SD_CK, PIN_SD_CS, NAME_SD);
	sd.disk_initialize();

	double avg = 0;

	pc.printf("Read file : %s\r\n", name);
	FILE *fp = fopen(name, "r");
	if (fp == NULL) {
		pc.printf("Could not open file for write\r\n");
	} else {
		fseek(fp, sizeof(FileADC), SEEK_SET);
		fseek(fp, sizeof(uint16_t) * ch, SEEK_CUR);
		size_t i, j;
		uint16_t null[2];
		uint16_t buff_i;
		float buff[buff_len];
		size_t buff_size = sizeof(buff), addr_size = _file.length
				* sizeof(float);
		for (i = 0; i < addr_size; i += buff_size) {
			for (j = 0; j < buff_len; j++) {
				fread(&buff_i, sizeof(uint16_t), 1, fp);
				fread(&null, sizeof(uint16_t), 2, fp);
				buff[j] = buff_i * 3.3f / 0xFFF;
				avg += buff[j];
			}
			_sram.write(i, &buff, buff_size, false);
		}
		fclose(fp);
	}

	sd.unmount();
	avg /= _file.length;
	return avg;
}

void Acceleration::sram_vdc(double avg) {
	float buff[buff_len];
	size_t i, j;
	size_t buff_size = sizeof(buff), addr_size = _file.length * sizeof(float);
	for (i = 0; i < addr_size; i += buff_size) {
		_sram.read(i, &buff, buff_size, false);
		for (j = 0; j < buff_len; j++) {
			buff[j] -= avg;
			buff[j] *= _file.rate;
		}
		_sram.write(i, &buff, buff_size, false);
	}
}

void Acceleration::sram_integral() {
	float buff[buff_len];
	double d = 0, t = 1.0 / _file.sps;
	size_t i, j;
	size_t buff_size = sizeof(buff), addr_size = _file.length * sizeof(float);
	for (i = 0; i < addr_size; i += buff_size) {
		_sram.read(i, &buff, buff_size, false);
		for (j = 0; j < buff_len; j++) {
			d += buff[j] * t;
			buff[j] = d;
		}
		_sram.write(i, &buff, buff_size, false);
	}
}

double Acceleration::sram_rms() {
	float buff[8];
	double sum = 0;
	size_t i, j;
	size_t buff_size = sizeof(buff), addr_size = _file.length * sizeof(float);
	for (i = 0; i < addr_size; i += buff_size) {
		_sram.read(i, &buff, buff_size, false);
		for (j = 0; j < 8; j++) {
			sum += buff[j] * buff[j];
		}
	}
	return sqrt(sum / _file.length);
}

double Acceleration::sram_vpp() {
	float buff[8];
	double max, min;
	size_t i, j;
	size_t buff_size = sizeof(buff), addr_size = _file.length * sizeof(float);
	for (i = 0; i < addr_size; i += buff_size) {
		_sram.read(i, &buff, buff_size, false);
		for (j = 0; j < 8; j++) {
			max = max > buff[j] ? max : buff[j];
			min = min < buff[j] ? min : buff[j];
		}
	}
	return max - min;
}

void Acceleration::sram_print(size_t length) {
	pc.printf("print sram\r\n");
	size_t i;
	float buff;
	for (i = 0; i < length; i++) {
		_sram.read(i * sizeof(buff), &buff, sizeof(buff), false);
		pc.printf("%d: %f\r\n", i, buff);
		wait_ms(5);
	}
}

void Acceleration::sram_file() {
	char name[33];
	fileName(_file.timestamp, (char*) &name);
	strcpy(&name[29], "tmp");

	SDFileSystem sd(PIN_SD_SI, PIN_SD_SO, PIN_SD_CK, PIN_SD_CS, NAME_SD);
	sd.disk_initialize();

	pc.printf("Saving file : %s\r\n", name);
	FILE *fp = fopen(name, "a");
	if (fp == NULL) {
		pc.printf("Could not open file for write\r\n");
	} else {
		size_t i;
		float buff;
		for (i = 0; i < _file.length; i++) {
			_sram.read(i * sizeof(buff), &buff, sizeof(buff), false);
			fprintf(fp, "%f, ", buff);
		}
		fprintf(fp, "\r\n");
		fclose(fp);
	}

	sd.unmount();
}

void Acceleration::count() {
	double avg;

	avg = sram_vac(0);
//	sram_file();
	sram_vdc(avg);
//	sram_file();
	sram_integral();
//	sram_file();
	_v_x_rms = sram_rms();
	sram_integral();
//	sram_file();
	_s_x_vpp = sram_vpp();

	avg = sram_vac(1);
	sram_vdc(avg);
	sram_integral();
	_v_y_rms = sram_rms();
	sram_integral();
	_s_y_vpp = sram_vpp();

	avg = sram_vac(2);
	sram_vdc(avg);
	sram_integral();
	_v_z_rms = sram_rms();
	sram_integral();
	_s_z_vpp = sram_vpp();

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
	time_t timestamp = _file.timestamp + (timezone * 60);
	char time[20];
	strftime(time, 20, "%Y-%m-%dT%H:%M:%S", localtime(&timestamp));

	SDFileSystem sd(PIN_SD_SI, PIN_SD_SO, PIN_SD_CK, PIN_SD_CS, "sd");
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

void Acceleration::write() {
	char name[33];
	fileName(_file.timestamp, (char*) &name);

	SDFileSystem sd(PIN_SD_SI, PIN_SD_SO, PIN_SD_CK, PIN_SD_CS, NAME_SD);
	sd.disk_initialize();

	pc.printf("Saving file : %s\r\n", name);
	FILE *fp = fopen(name, "w");
	if (fp == NULL) {
		pc.printf("Could not open file for write\r\n");
	} else {
		fwrite((const void*) &_file, sizeof(FileADC), 1, fp);
		size_t i;
		uint16_t buff[3];
		for (i = 0; i < _file.length; i++) {
			_sram.read(i * sizeof(buff), &buff, sizeof(buff), false);
			fwrite((const void*) &buff, sizeof(uint16_t),
					sizeof(buff) / sizeof(uint16_t), fp);
		}
		fclose(fp);
	}

	sd.unmount();
}

void Acceleration::printFile() {
	char name[33];
	fileName(_file.timestamp, (char*) &name);

	SDFileSystem sd(PIN_SD_SI, PIN_SD_SO, PIN_SD_CK, PIN_SD_CS, NAME_SD);
	sd.disk_initialize();

	pc.printf("Read file : %s\r\n", name);
	FILE *fp = fopen(name, "r");
	if (fp == NULL) {
		pc.printf("Could not open file for write\r\n");
	} else {
		FileADC file_head;
		fread(&file_head, sizeof(FileADC), 1, fp);
		size_t i;
		uint16_t buff[3];
		for (i = 0; i < _file.length; i++) {
			fread(&buff, sizeof(uint16_t), sizeof(buff) / sizeof(uint16_t), fp);
			pc.printf("%d, %d, %d, %d\r\n", i, buff[0], buff[1], buff[2]);
			wait_ms(5);
		}
		fclose(fp);
	}

	sd.unmount();
}
