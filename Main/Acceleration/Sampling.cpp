/*
 * Sampling.cpp
 *
 *  Created on: 2015/3/4
 *      Author: Leo
 */

#include "Sampling.h"

Sampling::Sampling(PinName x, PinName y, PinName z) :
        _x(x), _y(y), _z(z) {

}

void Sampling::start(timestamp_t us) {
    _index = 0;
    _flipper.attach_us(this, &Sampling::tick, us);
}

bool Sampling::isStop() {
    return !(_index < _SAMPLING_LENGTH);
}

void Sampling::tick() {
    if (_index < _SAMPLING_LENGTH) {
        x[_index] = _x.read_u16() >> 4;
        y[_index] = _y.read_u16() >> 4;
        z[_index] = _z.read_u16() >> 4;
        _index++;
    } else {
        _flipper.detach();
    }
}
