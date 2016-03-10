/*
 * Sampling.cpp
 *
 *  Created on: 2015/3/4
 *      Author: Leo
 */

#include "main.h"
#include "Sampling.h"

Sampling::Sampling(PinName pin_x, PinName pin_y, PinName pin_z, SerRAM &sram, size_t length) :
		_x(pin_x), _y(pin_y), _z(pin_z), _sram(sram) {
	_length = length;
}

//void Sampling::setbuf(uint16_t *data_x, uint16_t *data_y, uint16_t *data_z) {
//	x = data_x;
//	y = data_y;
//	z = data_z;
//}

void Sampling::start(timestamp_t us) {
	_index = 0;
	_flipper.attach_us(this, &Sampling::tick, us);
}

bool Sampling::isStop() {
	return !(_index < _length);
}

DigitalOut tt(P3_25);
void Sampling::tick() {
	tt = 0;
	if (_index < _length) {
		uint16_t buff[3];
		tt = 1;
		buff[0] = _x.read_u16() >> 4;
		tt = 0;
		buff[1] = _y.read_u16() >> 4;
		tt = 1;
		buff[2] = _z.read_u16() >> 4;
		tt = 0;
		_sram.write(_index * 6, &buff, sizeof(buff), false);
		_index++;
	} else {
		_flipper.detach();
	}
	tt = 1;
}

void Sampling::print() {
	uint16_t buff[3];
	pc.printf("===== print data =====\r\n");
	for (_index = 0; _index < _length; _index++) {
		_sram.read(_index * 6, &buff, sizeof(buff), false);
		pc.printf("%d, %d, %d, %d\r\n", _index, buff[0], buff[1], buff[2]);
		wait_ms(5);
	}
	pc.printf("\r\n===== end of data =====\r\n");
}

