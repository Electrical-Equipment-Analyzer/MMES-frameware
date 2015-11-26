//
#ifndef MBED_ACCELERATION_H
#define MBED_ACCELERATION_H

//required to use mbed functions
#include "mbed.h"

#include "Sampling.h"

//#define _ACCELERATION_LENGTH 2048
//#define _ACCELERATION_VCC 3.2f

/** Class: Acceleration
 *
 * Acceleration
 *
 * Example:
 *
 * > #include "mbed.h"
 *
 * > Acceleration acceleration(A0, A1, A2);
 */

class Acceleration {
    public:
        /** Create a Joystick HID for using regular mbed pins
         *
         * @param x    X asix
         * @param y    Y asix
         * @param z    Z asix
         */
        Acceleration();
        
        void test();

        void sample();

        void count();
        

        void check();

        void log();

        void write();

        double _v_x_rms, _v_y_rms, _v_z_rms;
        double _s_x_vpp, _s_y_vpp, _s_z_vpp;

    private:

        time_t _timestamp;
        uint32_t _sps;
        uint16_t _length;
        uint8_t _channels;
        float _adc_high;
        float _adc_low;
        uint8_t _adc_bit;
        float _rate;
        char _tag[8];

};

#endif
