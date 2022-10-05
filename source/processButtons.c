/*
 * processButtons.c
 *
 *  Created on: Oct 1, 2022
 *      Author: Waskevich
 *
 *  Shell/template for processing CapSense buttons and handling associated events.
 *
 *  Description: This code example provides a shell (or template) for processing CapSense
 *  			 buttons and handling associated events. Currently it is designed for
 *  			 multiple key lockout but can easily be updated to accommodate multiple
 *  			 active keys.
 *
 *  			 The primary function (processButtons) takes care of:
 *  			 	- checking the status of CapSense buttons and building a bitfield map
 *  			 	- analyzing the resulting bitfield for active buttons or lift-off events
 *  			 	- determining if an active button is the result of a new (i.e. touchdown)
 *  			 	  event, hold (i.e. on-going) event or has experienced a lift-off event
 *  			 	- calling the corresponding button handler to process the event
 *
 *  			The secondary function (processTouchEvents) calls the appropriate button
 *  			event handler via simple switch() statement.
 *
 *  			The button event handlers are constructed from a common/identical framework.
 *  			Additional button event handlers can be added by copying and pasting the
 *  			framework/template. Specific/custom actions can then be added in the body of
 *  			the code.
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

	wdgtBitfield = NO_WIDGETS_ACTIVE;

    if(Cy_CapSense_IsAnyWidgetActive(&cy_capsense_context)) /* skip if no widgets are active */
    {
		/* look for key presses */
		numWdgtActive = 0;

		/* build active widget bitfield */
		for(uint32_t wdgtIndex = CY_CAPSENSE_BUTTON0_WDGT_ID; wdgtIndex <= CY_CAPSENSE_BUTTON1_WDGT_ID; wdgtIndex++)
		{
			if(Cy_CapSense_IsWidgetActive(wdgtIndex, &cy_capsense_context))
			{
				wdgtBitfield |= 1 << (wdgtIndex - CY_CAPSENSE_BUTTON0_WDGT_ID);
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

	}
}

/* NOTE - event handlers use identical framework. Additional handlers can be added by copy/paste. */

void btn_Button0(uint32_t eventType, bool newEvent)
{
	if(TOUCH_ACTIVE == eventType) /* number of active widgets is 1 */
	{
		if(true == newEvent) /* touchdown event */
		{
			/* do any touchdown actions here */
			cyhal_gpio_toggle(CYBSP_USER_LED);
		}
		else if(false == newEvent) /* ongoing touch event */
		{
			/* do on-going actions here */

		}
	}
	else if(LIFT_OFF == eventType) /* number of active widgets is 0 */
	{
		/* do any lift-off actions here */

	}
}

void btn_Button1(uint32_t eventType, bool newEvent)
{
	if(TOUCH_ACTIVE == eventType) /* number of active widgets is 1 */
	{
		if(true == newEvent) /* touchdown event */
		{
			/* do any touchdown actions here */
			cyhal_gpio_write(CYBSP_USER_LED, MY_LED_ON);
		}
		else if(false == newEvent) /* ongoing touch event */
		{
			/* do on-going actions here */

		}
	}
	else if(LIFT_OFF == eventType) /* number of active widgets is 0 */
	{
		/* do any lift-off actions here */
		cyhal_gpio_write(CYBSP_USER_LED, MY_LED_OFF);
	}
}


