/*
 * state.h
 *
 *  Created on: 2015/2/28
 *      Author: Leo
 */

#ifndef STATE_H_
#define STATE_H_

#include "TextLCD.h"
#include "Joystick.h"
#include "config.h"


class State {
    public:
        State(TextLCD *lcd, Joystick *joystick, Config *conf);

        void setting_date();
        void setting_timezone();
        void setting_motor_type();
        void setting_motor_spec();
        void setting_motor_rpms();
        void setting_task();

    private:
        TextLCD *_lcd;
        Joystick *_joystick;
        Config *_conf;

        void date();
        void date_config(int dir);


        void print_timezone();
        void config_timezone(int dir);
        void print_motor_type();
        void config_motor_type(int dir);
        void print_motor_spec();
        void config_motor_spec(int dir);
        void print_motor_rpms();
        void config_motor_rpms(int dir);
        void print_task();
        void config_task(int dir);


};


#endif /* STATE_H_ */
