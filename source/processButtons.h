/*
 * processButtons.h
 *
 *  Created on: Oct 1, 2022
 *      Author: Waskevich
 */

#ifndef PROCESSBUTTONS_H_
#define PROCESSBUTTONS_H_

#include "stdint.h"

#define NO_WIDGETS_ACTIVE			(0u)
#define TOUCH_ACTIVE				(1u)
#define LIFT_OFF					(0u)

#define MY_LED_OFF					(1u)
#define MY_LED_ON					(0u)

/* enumeration for buttons (useful for symbolic/easy-to-understand meaning) ... add any additional CapSense buttons here */
enum {
	Button0,
	Button1,
	maxButtons
};

uint32_t processButtons(void);
void processTouchEvents(uint32_t numberActiveWidgets, bool newEvent, uint32_t bitFieldActiveButtons);

void btn_Button0(uint32_t eventType, bool newEvent);
void btn_Button1(uint32_t eventType, bool newEvent);


#endif /* PROCESSBUTTONS_H_ */
