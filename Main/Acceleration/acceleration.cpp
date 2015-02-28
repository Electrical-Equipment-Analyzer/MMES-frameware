#include "acceleration.h"

Acceleration::Acceleration(PinName x, PinName y, PinName z) :
        _x(x), _y(y), _z(z) {
    
}

void Acceleration::start(timestamp_t t) {
    _t = t / 1000000.0;
    _index = 0;
    _flipper.attach_us(this, &Acceleration::tick, t);
}

bool Acceleration::isStop() {
    return !(_index < _ACCELERATION_LENGTH);
}

double math_avg(double *v, uint8_t length) {
    double total = 0;
    uint8_t i = length;
    while (i--) {
        total += *v++;
    }
    return total / length;
}

double math_voltage(double ad) {
    return ad * _ACCELERATION_VCC / 0xFFF;
}

double math_g(double voltage, double avg) {
    return (voltage - avg) / 0.016;
}

double math_a(double g) {
    return g / 9.80665;
}

double math_v(double v, double a, double t) {
    return v + (a * t);
}

double math_s(double v0, double v, double t) {
    return (v0 + v) / 2 * t;
}

void mathArray_v(double *v, double *a, double t, uint8_t length) {
    double zero = 0;
    while (length--) {
        zero = math_v(zero, *a++, t);
        *v++ = zero;
    }
}

void mathArray_s(double *s, double *v, double t, uint8_t length) {
    double now, zero = 0;
    while (length--) {
        now = *v++;
        *s++ = math_s(zero, now, t);
        zero = now;
    }
}

/** Function: getStatus
 * Read the joystick status
 *
 * Variables:
 *  returns - A uint8_t values representing the bits
 */
void Acceleration::tick() {
    if (_index < _ACCELERATION_LENGTH) {
        ax[_index] = _x.read_u16() >> 4;
        ay[_index] = _y.read_u16() >> 4;
        az[_index] = _z.read_u16() >> 4;
        ax[_index] = math_voltage(ax[_index]);
        ay[_index] = math_voltage(ay[_index]);
        az[_index] = math_voltage(az[_index]);
        _index++;
    } else {
        uint8_t i;
        double avgX = math_avg(ax, _ACCELERATION_LENGTH);
        double avgY = math_avg(ay, _ACCELERATION_LENGTH);
        double avgZ = math_avg(az, _ACCELERATION_LENGTH);
        for (i = 0; i < _ACCELERATION_LENGTH; i++) {
            ax[i]  = math_a(math_g(ax[i], avgX));
            ay[i]  = math_a(math_g(ay[i], avgY));
            az[i]  = math_a(math_g(az[i], avgZ));
        }
        mathArray_v(vx, ax, _t, _ACCELERATION_LENGTH);
        mathArray_v(sx, vx, _t, _ACCELERATION_LENGTH);
        mathArray_v(vy, ay, _t, _ACCELERATION_LENGTH);
        mathArray_v(sy, vy, _t, _ACCELERATION_LENGTH);
        mathArray_v(vz, az, _t, _ACCELERATION_LENGTH);
        mathArray_v(sz, vz, _t, _ACCELERATION_LENGTH);
        _flipper.detach();
    }
}
