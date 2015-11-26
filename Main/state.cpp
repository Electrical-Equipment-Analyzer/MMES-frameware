/*
 * state.cpp
 *
 *  Created on: 2015/2/28
 *      Author: Leo
 */

#include "state.h"

#include "mbed.h"

State::State(TextLCD *lcd, Joystick *joystick, Config *conf) :
        _lcd(lcd), _joystick(joystick), _conf(conf) {
}

void State::test() {
	_conf->test();
}

static const uint8_t dt[][2] = { { 0, 5 }, { 0, 6 }, { 0, 7 }, { 0, 8 }, { 0, 10 }, { 0, 11 }, { 0, 13 }, { 0, 14 }, {
        1, 6 }, { 1, 7 }, { 1, 9 }, { 1, 10 }, { 1, 12 }, { 1, 13 } };

uint8_t c = 0;

void State::date() {
    int32_t timezone = _conf->get(Config::TIMEZONE) * 60;
    time_t seconds = time(NULL) + timezone;
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
                    }
                    break;
                case Joystick::press:
                    _lcd->setCursor(TextLCD::CurOff_BlkOff);
                    flipper.detach();
                    return;
                    break;
                default:
                    break;
            }
            date();
            last_status = status;
        }
    }
}

static const uint8_t tz[] = { 11, 12, 14, 15 };

void State::print_timezone() {
    int16_t timezone = _conf->get(Config::TIMEZONE);
    uint8_t minute = abs(timezone % 60);
    uint8_t hour = abs(timezone / 60);
    _lcd->cls();
    _lcd->printf("Time Zone:%c%02d:%02d", (timezone < 0 ? '-' : '+'), hour, minute);
    _lcd->setAddress(tz[c], 0);
}

void State::config_timezone(int dir) {
    int16_t timezone = _conf->get(Config::TIMEZONE);
    switch (c) {
        case 0:
            timezone += dir * 600;
            break;
        case 1:
            timezone += dir * 60;
            break;
        case 2:
            timezone += dir * 10;
            break;
        case 3:
            timezone += dir;
            break;
    }
    timezone %= 720;
    _conf->set(Config::TIMEZONE, timezone);
}

void State::setting_timezone() {
    Ticker flipper;
    flipper.attach(this, &State::print_timezone, 1);
    Joystick::Status last_status = Joystick::right;
    _lcd->setCursor(TextLCD::CurOff_BlkOn);
    while (true) {
        Joystick::Status status = _joystick->getStatus();
        if (status != last_status) {
            switch (status) {
                case Joystick::none:
                    break;
                case Joystick::up:
                    config_timezone(1);
                    break;
                case Joystick::down:
                    config_timezone(-1);
                    break;
                case Joystick::left:
                    if (c > 0) {
                        c--;
                    }
                    break;
                case Joystick::right:
                    if (c < 3) {
                        c++;
                    }
                    break;
                case Joystick::press:
                    _lcd->setCursor(TextLCD::CurOff_BlkOff);
                    flipper.detach();
                    return;
                    break;
                default:
                    break;
            }
            print_timezone();
            last_status = status;
        }
    }
}

static const char motor_type[][17] = { "Small Machines", "Medium Machines", "Large Rigid FD", "Large Soft FD" };

void State::print_motor_type() {
    uint8_t type = _conf->get(Config::MOTOR_TYPE);
    _lcd->cls();
    _lcd->printf("Motor : Class %d\n%s", type + 1, motor_type[type]);
    _lcd->setAddress(tz[c], 0);
}

void State::config_motor_type(int dir) {
    uint8_t type = _conf->get(Config::MOTOR_TYPE);
    type += dir;
    if (type >= 4) {
        type = 0;
    }
    _conf->set(Config::MOTOR_TYPE, type);
}

void State::setting_motor_type() {
    Joystick::Status last_status = Joystick::right;
    while (true) {
        Joystick::Status status = _joystick->getStatus();
        if (status != last_status) {
            switch (status) {
                case Joystick::none:
                    break;
                case Joystick::up:
                    config_motor_type(1);
                    break;
                case Joystick::down:
                    config_motor_type(-1);
                    break;
                case Joystick::left:
                    break;
                case Joystick::right:
                    break;
                case Joystick::press:
                    return;
                    break;
                default:
                    break;
            }
            print_motor_type();
            last_status = status;
        }
    }
}

static const char motor_spec[][17] = { "Stand Machines", "Special Machines" };

void State::print_motor_spec() {
    uint8_t type = _conf->get(Config::MOTOR_SPEC);
    _lcd->cls();
    _lcd->printf("Motor Spec:\n%s", motor_spec[type]);
    _lcd->setAddress(tz[c], 0);
}

void State::config_motor_spec(int dir) {
    uint8_t type = _conf->get(Config::MOTOR_SPEC);
    type += dir;
    if (type >= 2) {
        return;
    }
    _conf->set(Config::MOTOR_SPEC, type);
}

void State::setting_motor_spec() {
    Joystick::Status last_status = Joystick::right;
    while (true) {
        Joystick::Status status = _joystick->getStatus();
        if (status != last_status) {
            switch (status) {
                case Joystick::none:
                    break;
                case Joystick::up:
                    config_motor_spec(1);
                    break;
                case Joystick::down:
                    config_motor_spec(-1);
                    break;
                case Joystick::left:
                    break;
                case Joystick::right:
                    break;
                case Joystick::press:
                    return;
                    break;
                default:
                    break;
            }
            print_motor_spec();
            last_status = status;
        }
    }
}

static const char motor_rpms[] = { 2, 3 };

void State::print_motor_rpms() {
    uint8_t type = _conf->get(Config::MOTOR_RPMS);
    _lcd->cls();
    _lcd->printf("Motor RPM : \n %3d00 rpm", type);
    _lcd->setAddress(motor_rpms[c], 1);
}

void State::config_motor_rpms(int dir) {
    uint8_t type = _conf->get(Config::MOTOR_RPMS);
    switch (c) {
        case 0:
            type += dir * 10;
            break;
        case 1:
            type += dir;
            break;
    }
    _conf->set(Config::MOTOR_RPMS, type);
}

void State::setting_motor_rpms() {
    Joystick::Status last_status = Joystick::right;
    _lcd->setCursor(TextLCD::CurOn_BlkOn);
    while (true) {
        Joystick::Status status = _joystick->getStatus();
        if (status != last_status) {
            switch (status) {
                case Joystick::none:
                    break;
                case Joystick::up:
                    config_motor_rpms(1);
                    break;
                case Joystick::down:
                    config_motor_rpms(-1);
                    break;
                case Joystick::left:
                    if (c > 0) {
                        c--;
                    }
                    break;
                case Joystick::right:
                    if (c < 1) {
                        c++;
                    }
                    break;
                case Joystick::press:
                    _lcd->setCursor(TextLCD::CurOff_BlkOff);
                    return;
                    break;
                default:
                    break;
            }
            print_motor_rpms();
            last_status = status;
        }
    }
}

void State::print_task() {
    uint8_t minute = _conf->get(Config::TASK);
    _lcd->cls();
    _lcd->printf("Task :\n %3d minutes", minute);
    _lcd->setAddress(1 + c, 1);
}

void State::config_task(int dir) {
    uint8_t minute = _conf->get(Config::TASK);
    switch (c) {
        case 0:
            minute += dir * 100;
            break;
        case 1:
            minute += dir * 10;
            break;
        case 2:
            minute += dir;
            break;
    }
    _conf->set(Config::TASK, minute);
}

void State::setting_task() {
    ;
    Joystick::Status last_status = Joystick::right;
    _lcd->setCursor(TextLCD::CurOn_BlkOn);
    while (true) {
        Joystick::Status status = _joystick->getStatus();
        if (status != last_status) {
            switch (status) {
                case Joystick::none:
                    break;
                case Joystick::up:
                    config_task(1);
                    break;
                case Joystick::down:
                    config_task(-1);
                    break;
                case Joystick::left:
                    if (c > 0) {
                        c--;
                    }
                    break;
                case Joystick::right:
                    if (c < 2) {
                        c++;
                    }
                    break;
                case Joystick::press:
                    _lcd->setCursor(TextLCD::CurOff_BlkOff);
                    return;
                    break;
                default:
                    break;
            }
            print_task();
            last_status = status;
        }
    }
}
