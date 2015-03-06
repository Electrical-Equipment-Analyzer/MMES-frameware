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
        

        void sample();

        void count();
        

    private:
        double _sps;

};

#endif
