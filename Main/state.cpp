/*
 * state.cpp
 *
 *  Created on: 2015/2/28
 *      Author: Leo
 */

#include "state.h"

#include "mbed.h"
#include "rtos.h"

State::State(TextLCD *lcd, Joystick *joystick) : _lcd(lcd), _joystick(joystick) {

}

static const uint8_t dt[][2] = { { 0, 5 }, { 0, 6 }, { 0, 7 }, { 0, 8 }, { 0, 10 }, { 0, 11 }, { 0, 13 }, { 0, 14 }, {
        1, 6 }, { 1, 7 }, { 1, 9 }, { 1, 10 }, { 1, 12 }, { 1, 13 } };

uint8_t c = 0;

void State::date() {
    time_t seconds = time(NULL);
    char buffer[32];
    strftime(buffer, 32, "Date:%Y/%m/%d\nTime: %H:%M:%S", localtime(&seconds));
    _lcd->cls();
    _lcd->printf("%s", buffer);
    _lcd->setAddress(dt[c][1], dt[c][0]);
}

void State::date_config(int dir) {
    time_t seconds = time(NULL);
    struct tm *t = localtime(&seconds);
    switch (c) {
        case 0:
//            t->tm_year += 1000;
            break;
        case 1:
            t->tm_year += dir * 100;
            break;
        case 2:
            t->tm_year += dir * 10;
            break;
        case 3:
            t->tm_year += dir;
            break;
        case 4:
            t->tm_mon += dir * 10;
            break;
        case 5:
            t->tm_mon += dir;
            break;
        case 6:
            t->tm_mday += dir * 10;
            break;
        case 7:
            t->tm_mday += dir;
            break;
        case 8:
            t->tm_hour += dir * 10;
            break;
        case 9:
            t->tm_hour += dir;
            break;
        case 10:
            t->tm_min += dir * 10;
            break;
        case 11:
            t->tm_min += dir;
            break;
        case 12:
            t->tm_sec += dir * 10;
            break;
        case 13:
            t->tm_sec += dir;
            break;
    }
    set_time(mktime(t));
}

void State::setting_date() {
    Ticker flipper;
    flipper.attach(this, &State::date, 1);
    Joystick::Status last_status = Joystick::right;
    _lcd->setCursor(TextLCD::CurOn_BlkOn);
    while (true) {
        Joystick::Status status = _joystick->getStatus();
        if (status != last_status) {
            switch (status) {
                case Joystick::none:
                    break;
                case Joystick::up:
                    date_config(1);
                    break;
                case Joystick::down:
                    date_config(-1);
                    break;
                case Joystick::left:
                    if (c > 0) {
                        c--;
                    }
                    break;
                case Joystick::right:
                    if (c < 13) {
                        c++;
                    } else {
                        _lcd->setCursor(TextLCD::CurOff_BlkOff);
                        flipper.detach();
                        return;
                    }
                    break;
                case Joystick::press:
                    break;
                default:
                    break;
            }
            date();
            last_status = status;
        }
    }
}
