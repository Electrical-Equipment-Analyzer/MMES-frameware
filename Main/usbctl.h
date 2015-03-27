/*
 * usbctl.h
 *
 *  Created on: 2015/3/26
 *      Author: Leo
 */

#ifndef USBCTL_H_
#define USBCTL_H_

#include "mbed.h"


#include "USBHID.h"

class Usbctl {
    public:
        Usbctl();

        bool poll();


    private:

        void send(char *send, uint8_t length);

        void usb_ad();

        void log();

        USBHID hid;
        HID_REPORT send_report;
        HID_REPORT recv_report;


};



#endif /* USBCTL_H_ */
