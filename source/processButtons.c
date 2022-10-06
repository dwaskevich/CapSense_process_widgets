/*
 * processButtons.c
 *
 *  Created on: Oct 1, 2022
 *      Author: Waskevich
 *
 *  Shell/template for processing CapSense buttons and handling associated events.
 *
 *  Description: This file contains code for processing CapSense buttons and handling
 *  			 associated events. Currently it is designed for multiple key lockout
 *  			 but can easily be updated to accommodate multiple active keys.
 *
 *  			 It identifies and provides the following touch event placeholders/features:
 *  			 	- Touchdown event/actions (one-time event until release)
 *  			 	- Hold event/actions (continuous, happens each scan cycle)
 *  			 	- Short hold event/actions (one-time event until release)
 *  			 	- Repeat event/actions (continuous until release or long-hold timeout)
 *  			 	- Long hold (with hysteresis) event/actions (intended for one-time event)
 *
 *  			 The primary function (processButtons) implements the following tasks:
 *  			 	- checks the status of CapSense buttons and builds a bitfield map
 *  			 	- analyzes the resulting bitfield for active buttons or lift-off events
 *  			 	- determines if an active button is the result of a new (i.e. touchdown)
 *  			 	  event, a hold (i.e. on-going) event or encountered a lift-off event
 *  			 	- calls the corresponding button handler to process the event
 *
 *  			The secondary function (processTouchEvents) calls the appropriate button
 *  			event handler via simple switch() statement. TODO - replace with a
 *  			Function Pointer State Machine.
 *
 *  			Finally, the button event handlers provide placeholders for touchdown, short
 *  			hold, repeat, long hold and liftoff event actions. The handler is a framework
 *  			and identical for all buttons. Additional buttons can be added by copying
 *  			and pasting the	framework/template. Specific/custom actions can then be added
 *  			in the placeholder sections of each handler.
 *
 *  Usage:
 *  			- include processButtons.h file in main.c (#include "processButtons.h")
 *  			- add descriptive button names (if desired) to enum in processButtons.h file
 *  				- note - these names are used as limits in the for() loop
 *  			- estimate and enter total CapSense scan time in TOUCH_COUNT_MSEC macro
 *  			- copy/paste (or delete) template handler to add (or remove) additional buttons
 *  			- add action code to desired events
 *  			- call processButtons() from main loop when CapSense engine is not busy
 *
 */

#include "cyhal.h"
#include "cybsp.h"
#include "cycfg_capsense.h"
#include "processButtons.h"

uint32_t processButtons(void)
{
    static uint32_t previousBitField;
    uint32_t wdgtBitfield, numWdgtActive;

	wdgtBitfield = NO_WIDGETS_ACTIVE; /* initialize bitfield */

    if(Cy_CapSense_IsAnyWidgetActive(&cy_capsense_context)) /* skip if no widgets are active */
    {
		/* look for key presses */
		numWdgtActive = 0;

		/* build active widget bitfield */
		for(uint32_t wdgtIndex = Button0; wdgtIndex <= Button1; wdgtIndex++)
		{
			if(Cy_CapSense_IsWidgetActive(wdgtIndex, &cy_capsense_context)) /* look for active widgets */
			{
				wdgtBitfield |= 1 << (wdgtIndex - Button0); /* build active bitfield */
				numWdgtActive++;
			}
		}
		if(wdgtBitfield > 0) /* at least one key is active */
		{
			if(previousBitField != wdgtBitfield) /* this is a new (touchdown) event */
			{
				previousBitField = wdgtBitfield;
				processTouchEvents(numWdgtActive, true, wdgtBitfield); /* process new touchdown event */
			}
			else /* this is a hold event */
			{
				processTouchEvents(1, false, previousBitField); /* not a new event, process hold */
			}
		}
    }
	else /* no widgets are active */
	{
		/* check buttons to see if they went inactive on this scan */
		if(0 != previousBitField) /* this is a new liftoff event */
		{
			processTouchEvents(NO_WIDGETS_ACTIVE, true, previousBitField); /* report and process lift-off event */
			previousBitField = 0;
		}
	}

	return wdgtBitfield;
}

void processTouchEvents(uint32_t numberActiveWidgets, bool newEvent, uint32_t bitFieldActiveButtons)
{
	if(numberActiveWidgets <= 1) /* only process single key presses or liftoff events (0 active widgets) */
	{
		switch(bitFieldActiveButtons)
		{
			case (1 << Button0):
				btn_Button0(numberActiveWidgets, newEvent);
				break;
			case (1 << Button1):
				btn_Button1(numberActiveWidgets, newEvent);
				break;

			default:
				break;
		}
	}
	else /* more than one button active */
	{
		/* handle multi-key events here */

	}
}

/* NOTE - event handlers use identical framework. Additional button handlers can be added by copy/paste. */

void btn_Button0(uint32_t eventType, bool newEvent)
{
	static uint32_t scanCounter, repeatCount;
	static bool shortHoldExpired, longHoldExpired;
	static uint32_t longHoldTime = LONG_HOLD_TIME;

	if(TOUCH_ACTIVE == eventType) /* number of active widgets is 1 */
	{
		if(true == newEvent) /* indicates a touchdown event */
		{
			scanCounter = 0; /* reset scan counter ... will be used to measure hold time */
			longHoldTime = LONG_HOLD_TIME;
			shortHoldExpired = false;
			longHoldExpired = false;

			/* do any touchdown actions here */
			cyhal_gpio_write(CYBSP_USER_LED, MY_LED_ON);

		}
		else /* ongoing touch event */
		{
			/* do on-going actions here (will happen each scan) */


			/* test for hold-time events */
			if(scanCounter++ > TOUCH_HOLD_TIME_COUNTS && false == longHoldExpired) /* time to do something */
			{
				if(false == shortHoldExpired) /* first expiration (i.e. hold time expired) */
				{
					shortHoldExpired = true; /* set flag */
					repeatCount = 0; /* initialize repeat counter */

					/* do any short hold actions here (will only happen once until button is released) */
					cyhal_gpio_write(CYBSP_USER_LED, MY_LED_OFF);

				}
				else /* execute "repeat" actions every TOUCH_REPEAT_COUNTS interval */
				{
					if(repeatCount++ > TOUCH_REPEAT_COUNTS)
					{
						repeatCount = 0; /* reset repeat counter */

						/* do any repeat actions here */
						cyhal_gpio_toggle(CYBSP_USER_LED);
					}
				}
			}

			if(scanCounter > longHoldTime)
			{
				longHoldTime = LONG_HOLD_TIME + LONG_HOLD_HYSTERESIS; /* add hysteresis in case button continues to be held */
				scanCounter = 0; /* reset CapSense scan counter */
				longHoldExpired = true; /* set flag */

				/* do any long-hold actions here */
				cyhal_gpio_write(CYBSP_USER_LED, MY_LED_OFF);
			}
		}
	}
	else if(LIFT_OFF == eventType) /* number of active widgets is 0 */
	{
		/* do any lift-off actions here */
		cyhal_gpio_write(CYBSP_USER_LED, MY_LED_OFF);
	}
}

void btn_Button1(uint32_t eventType, bool newEvent)
{
	static uint32_t scanCounter, repeatCount;
	static bool shortHoldExpired, longHoldExpired;
	static uint32_t longHoldTime = LONG_HOLD_TIME;

	if(TOUCH_ACTIVE == eventType) /* number of active widgets is 1 */
	{
		if(true == newEvent) /* indicates a touchdown event */
		{
			scanCounter = 0; /* reset scan counter ... will be used to measure hold time */
			longHoldTime = LONG_HOLD_TIME;
			shortHoldExpired = false;
			longHoldExpired = false;

			/* do any touchdown actions here */
			cyhal_gpio_write(CYBSP_USER_LED, MY_LED_ON);

		}
		else /* ongoing touch event */
		{
			/* do on-going actions here (will happen each scan) */


			/* test for hold-time events */
			if(scanCounter++ > TOUCH_HOLD_TIME_COUNTS && false == longHoldExpired) /* time to do something */
			{
				if(false == shortHoldExpired) /* first expiration (i.e. hold time expired) */
				{
					shortHoldExpired = true; /* set flag */
					repeatCount = 0; /* initialize repeat counter */

					/* do any short hold actions here (will only happen once until button is released) */
					cyhal_gpio_write(CYBSP_USER_LED, MY_LED_OFF);

				}
				else /* execute "repeat" actions every TOUCH_REPEAT_COUNTS interval */
				{
					if(repeatCount++ > TOUCH_REPEAT_COUNTS)
					{
						repeatCount = 0; /* reset repeat counter */

						/* do any repeat actions here */
						cyhal_gpio_toggle(CYBSP_USER_LED);
					}
				}
			}

			if(scanCounter > longHoldTime)
			{
				longHoldTime = LONG_HOLD_TIME + LONG_HOLD_HYSTERESIS; /* add hysteresis in case button continues to be held */
				scanCounter = 0; /* reset CapSense scan counter */
				longHoldExpired = true; /* set flag */

				/* do any long-hold actions here */
				cyhal_gpio_write(CYBSP_USER_LED, MY_LED_OFF);
			}
		}
	}
	else if(LIFT_OFF == eventType) /* number of active widgets is 0 */
	{
		/* do any lift-off actions here */
		cyhal_gpio_write(CYBSP_USER_LED, MY_LED_OFF);
	}
}


