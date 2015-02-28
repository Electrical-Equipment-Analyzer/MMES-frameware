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

class State {
    public:
        State(TextLCD *lcd, Joystick *joystick);

        void setting_date();

    private:
        TextLCD *_lcd;
        Joystick *_joystick;

        void date();
        void date_config(int dir);
};


#endif /* STATE_H_ */
