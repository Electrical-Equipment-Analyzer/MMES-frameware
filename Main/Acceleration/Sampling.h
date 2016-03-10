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

#include "SerRAM.h"

#define MAX_SAMPLING_LENGTH 2112

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
        Sampling(PinName pin_x, PinName pin_y, PinName pin_z, SerRAM &sram, size_t length = MAX_SAMPLING_LENGTH);

//        void setbuf(uint16_t *data_x, uint16_t *data_y, uint16_t *data_z);

        void tick();

        void start(timestamp_t t);

        bool isStop();

        void print();

//        uint16_t *x;
//        uint16_t *y;
//        uint16_t *z;

    private:

        /** Regular mbed pins bus
         */
        AnalogIn _x, _y, _z;
    	SerRAM _sram;
    	size_t _length;
    	size_t _index;

        Ticker _flipper;
};



#endif /* SAMPLING_H_ */
