//
#ifndef MBED_ACCELERATION_H
#define MBED_ACCELERATION_H

//required to use mbed functions
#include "mbed.h"

#define _ACCELERATION_LENGTH 100
#define _ACCELERATION_VCC 3.2f

/** Class: Acceleration
 *
 * Used for reading from an digital joystick
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
        Acceleration(PinName x, PinName y, PinName z);

        void tick();
        
        void start(timestamp_t t);
        
        bool isStop();
        
        double ax[_ACCELERATION_LENGTH];
        double ay[_ACCELERATION_LENGTH];
        double az[_ACCELERATION_LENGTH];

        double vx[_ACCELERATION_LENGTH];
        double vy[_ACCELERATION_LENGTH];
        double vz[_ACCELERATION_LENGTH];

        double sx[_ACCELERATION_LENGTH];
        double sy[_ACCELERATION_LENGTH];
        double sz[_ACCELERATION_LENGTH];

    private:

        /** Regular mbed pins bus
         */
        AnalogIn _x, _y, _z;
        
        uint8_t _index;
        
        Ticker _flipper;

        double _t;
};

#endif
