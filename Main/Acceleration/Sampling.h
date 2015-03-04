/*
 * Sampling.h
 *
 *  Created on: 2015/3/4
 *      Author: Leo
 */

#ifndef SAMPLING_H_
#define SAMPLING_H_

//required to use mbed functions
#include "mbed.h"

#define _SAMPLING_LENGTH 2048

/** Class: Sampling
 *
 * Eduction of a continuous signal to a discrete signal.
 *
 * Example:
 *
 * > #include "mbed.h"
 *
 * > Acceleration acceleration(A0, A1, A2);
 */

class Sampling {
    public:
        /** Create a Sampling
         *
         * @param x    X asix
         * @param y    Y asix
         * @param z    Z asix
         */
        Sampling(PinName x, PinName y, PinName z);

        void tick();

        void start(timestamp_t t);

        bool isStop();

        uint16_t x[_SAMPLING_LENGTH];
        uint16_t y[_SAMPLING_LENGTH];
        uint16_t z[_SAMPLING_LENGTH];

    private:

        /** Regular mbed pins bus
         */
        AnalogIn _x, _y, _z;

        uint16_t _index;

        Ticker _flipper;

        double _s;
};



#endif /* SAMPLING_H_ */
