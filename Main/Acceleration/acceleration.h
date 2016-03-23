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

	typedef struct {

        time_t timestamp;
        size_t sps = 32000;
        size_t length = 2048;
        uint8_t channels = 3;
        float adc_high = 3.3f;
        float adc_low = 0;
        uint8_t adc_bit = 12;
		float rate = 1/0.016*9.80665;
        char tag[8] = "a";
	}FileADC;

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
        void printFile();

        double _v_x_rms, _v_y_rms, _v_z_rms;
        double _s_x_vpp, _s_y_vpp, _s_z_vpp;

    private:
    	SerRAM _sram;
    	FileADC _file;

        double sram_vac(uint8_t ch);
        void sram_vdc(double avg);
        void sram_integral();
        double sram_rms();
        double sram_vpp();
        void sram_print(size_t length);
        void sram_file();
};

#endif
