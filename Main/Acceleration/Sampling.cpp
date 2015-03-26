/*
 * Sampling.cpp
 *
 *  Created on: 2015/3/4
 *      Author: Leo
 */

#include "Sampling.h"

Sampling::Sampling(PinName pin_x, PinName pin_y, PinName pin_z, uint16_t length) :
        _x(pin_x), _y(pin_y), _z(pin_z) {
    _length = length;
}

void Sampling::setbuf(uint16_t *data_x, uint16_t *data_y, uint16_t *data_z) {
    x = data_x;
    y = data_y;
    z = data_z;
}

void Sampling::start(timestamp_t us) {
    _index = 0;
    _flipper.attach_us(this, &Sampling::tick, us);
}

bool Sampling::isStop() {
    return !(_index < _length);
}

void Sampling::tick() {
    if (_index < _length) {
        x[_index] = _x.read_u16() >> 4;
        y[_index] = _y.read_u16() >> 4;
        z[_index] = _z.read_u16() >> 4;
        _index++;
    } else {
        _flipper.detach();
    }
}
