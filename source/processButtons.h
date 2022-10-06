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

/*******************************************************************************
 * user inputs here
 *******************************************************************************/
/* enter CapSense scan time (measured or calculated from CapSense scan parameters) */
#define TOUCH_COUNT_MSEC			(1u)
/* define desired "pause" (i.e. short hold time/delay) after touch before repeat */
#define TOUCH_HOLD_TIME_MSEC		(800u)
/* define desired repeat rate (in msec) - e.g. 200 msec for 5x per second repeat rate */
#define TOUCH_REPEAT_RATE			(200u)
/* define long hold time */
#define LONG_HOLD_TIME_MSEC			(5000u)
/* define long hold time hysteresis */
#define LONG_HOLD_TIME_HYST_MSEC	(10000u)

/*******************************************************************************
 * pre-processor macros for calculating hold and repeat counts
 *  - note - if msec time base is available, it can be used instead of counts
 ********************************************************************************/
#define TOUCH_REPEAT_MSEC			((TOUCH_REPEAT_RATE / TOUCH_COUNT_MSEC) * TOUCH_COUNT_MSEC)
#define TOUCH_HOLD_TIME_COUNTS		(TOUCH_HOLD_TIME_MSEC / TOUCH_COUNT_MSEC)
#define TOUCH_REPEAT_COUNTS			(TOUCH_REPEAT_MSEC / TOUCH_COUNT_MSEC)
#define LONG_HOLD_TIME_COUNTS		(LONG_HOLD_TIME_MSEC / TOUCH_COUNT_MSEC)
#define LONG_HOLD_HYSTERESIS_COUNTS	(LONG_HOLD_TIME_HYST_MSEC / TOUCH_COUNT_MSEC)

#define MY_LED_OFF					(1u)
#define MY_LED_ON					(0u)

/* enumeration for buttons (useful for symbolic/easy-to-understand meaning) ... add any additional CapSense buttons here */
enum {
	Button0 = CY_CAPSENSE_BUTTON0_WDGT_ID,
	Button1 = CY_CAPSENSE_BUTTON1_WDGT_ID
};

uint32_t processButtons(void);
void processTouchEvents(uint32_t numberActiveWidgets, bool newEvent, uint32_t bitFieldActiveButtons);

void btn_Button0(uint32_t eventType, bool newEvent);
void btn_Button1(uint32_t eventType, bool newEvent);


#endif /* PROCESSBUTTONS_H_ */
