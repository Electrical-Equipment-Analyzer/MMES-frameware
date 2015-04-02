/*
 * config.h
 *
 *  Created on: 2015/3/31
 *      Author: Leo
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "mbed.h"



#define EEPROM_Address  0xa0

class Config {
    public:

        typedef enum {
            TIMEZONE = 0x00,
            MOTOR_TYPE = 0x10,
            MOTOR_SPEC = 0x11,
            MOTOR_RPMS = 0x12,
            TASK = 0x15,
            STATE = 16
        } CONFIG_TYPE;

        Config(I2C *i2c);

        void set(CONFIG_TYPE type, uint16_t conf);

        uint16_t get(CONFIG_TYPE type);


    private:

        void write(uint16_t addr, uint16_t conf, uint16_t length);

        uint16_t read(uint16_t addr, uint16_t length);

        I2C *_eeprom;

};



#endif /* CONFIG_H_ */
