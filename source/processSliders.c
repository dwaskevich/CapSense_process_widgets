/*
 * processSliders.c
 *
 *  Created on: Sep 30, 2022
 *      Author: Waskevich
 *
 *  Shell/template for processing CapSense sliders and handling associated events.
 *
 *  Description: This file contains code for processing CapSense sliders and handling
 *  			 associated events.
 *
 *  			 It identifies and provides the following features and event placeholders:
 *  			 	- Prioritizes active slider (if there's more than one slider in the design).
 *  			 	  - This is similar to multi-key lockout for buttons (i.e. only one slider
 *  			 	  	processed at a time). This can be updated/changed as needed.
 *  			 	- Otherwise, looks for active slider (if none are currently active)
 *  			 	- Distinguishes between touchdown, on-going and liftoff events
 *
 *  			 The primary function (processSliders) implements the following tasks:
 *  			 	- checks for previously active slider and updates position
 *  			 		- does nothing until there is a change in position (or lift-off occurs)
 *  			 	- searches for active slider (if none are previously active)
 *  			 	- reports touchdown event for newly active slider
 *  			 	- updates touch status (true/false) in sliderValues array
 *  			 	- identifies and reports lift-off events
 *
 *  			The secondary function (processSliderEvents) calls the appropriate slider
 *  			event handler via simple switch() statement. TODO - replace with a
 *  			Function Pointer State Machine.
 *
 *  			Finally, the slider event handler provides placeholders for touchdown, ongoing
 *  			and lift-off event actions.
 *
 *  Usage:
 *  			- include processSliders.h file in main.c (#include "processSliders.h")
 *  			- add descriptive slider names to enum in processSliders.h file
 *  			- add event actions in placeholder sections of the template code
 *  			- call processSliders() from main loop when CapSense engine is not busy
 *
 */

#include "cyhal.h"
#include "cybsp.h"
#include "cycfg_capsense.h"
#include "processSliders.h"

sliderValues_t sliderValues[NUMBER_OF_SLIDERS];

uint32_t processSliders(void)
{
	uint32_t status = 0;
	static uint32_t capsenseScanCounter = 0;
    cy_stc_capsense_touch_t *ptrTouch;
    static uint32_t activeSlider = NO_SLIDERS_ACTIVE;

    capsenseScanCounter++; /* elapsed time counter ... future use */

    if(Cy_CapSense_IsAnyWidgetActive(&cy_capsense_context)) /* check to see if any widgets are active  */
    {
		/* check for previously active slider */
		if(NO_SLIDERS_ACTIVE != activeSlider) /* prioritize previously active slider, skip others ... only one slider active at a time */
		{
			ptrTouch = Cy_CapSense_GetTouchInfo(activeSlider, &cy_capsense_context); /* use pointer to get slider data */
			sliderValues[activeSlider - Slider0].currentSliderValue = ptrTouch->ptrPosition->x; /* get slider centroid/position */
			sliderValues[activeSlider - Slider0].lastUpdateCounterValue = capsenseScanCounter; /* update scan counter (future use) */

			if(sliderValues[activeSlider - Slider0].previousSliderValue != sliderValues[activeSlider - Slider0].currentSliderValue) /* only process changes in position */
			{
				processSliderEvents(activeSlider, false, sliderValues[activeSlider - Slider0].currentSliderValue);
			}
		}
		else /* no sliders currently active ... look for active slider */
		{
			for(uint32_t wdgtIndex = Slider0; wdgtIndex <= Slider0; wdgtIndex++)
			{
				if(Cy_CapSense_IsWidgetActive(wdgtIndex, &cy_capsense_context)) /* check to see if any widgets are active  */
				{
					ptrTouch = Cy_CapSense_GetTouchInfo(wdgtIndex, &cy_capsense_context); /* use pointer to get slider data */
					sliderValues[wdgtIndex - Slider0].currentSliderValue = ptrTouch->ptrPosition->x; /* get slider centroid/position */
					sliderValues[wdgtIndex - Slider0].lastUpdateCounterValue = capsenseScanCounter; /* update scan counter (future use) */

					if(false == sliderValues[wdgtIndex - Slider0].touchStatus) /* this is a new event */
					{
						sliderValues[wdgtIndex - Slider0].touchStatus = true;
						sliderValues[wdgtIndex - Slider0].previousSliderValue = sliderValues[wdgtIndex - Slider0].currentSliderValue;
						processSliderEvents(wdgtIndex, true, sliderValues[activeSlider - Slider0].currentSliderValue);
					}

					activeSlider = wdgtIndex; /* prioritize active slider ... save the index (first-come, first-serve) */

					break; /* only one slider active at a time ... can be updated to allow multiple sliders or different priority (i.e. signal strength) */
				}
				else
				{
					sliderValues[wdgtIndex - Slider0].touchStatus = false; /* mark slider inactive */
				}
			}
		}
    }
    else
    {
		/* check sliders to see if they went inactive on this scan */
		if(NO_SLIDERS_ACTIVE != activeSlider)
		{
			for(uint32_t wdgtIndex = Slider0; wdgtIndex <= Slider0; wdgtIndex++)
			{
				if(true == sliderValues[wdgtIndex - Slider0].touchStatus) /* this is a new lift-off event */
				{
					sliderValues[wdgtIndex - Slider0].touchStatus = false; /* mark slider inactive */
					processSliderEvents(wdgtIndex, true, sliderValues[activeSlider - Slider0].currentSliderValue);
					break;
				}
			}

			activeSlider = NO_SLIDERS_ACTIVE;
		}
    }

	return status;
}


void processSliderEvents(uint32_t sliderNum, bool newEvent, uint16_t sliderValue)
{
	switch(sliderNum)
	{
		case Slider0:
			slider_0(newEvent, sliderValue);
			break;

		default:
			break;
	}
}


void slider_0(bool newEvent, uint16_t sliderValue)
{
	if(true == sliderValues[Slider0 - Slider0].touchStatus)
	{
		if(true == newEvent) /* touchdown actions here */
		{
			cyhal_gpio_write(CYBSP_USER_LED, 0);
		}
		else /* ongoing actions here */
		{
			if(sliderValues[Slider0 - Slider0].currentSliderValue - sliderValues[Slider0 - Slider0].previousSliderValue > 25) /* arbitrary demo effect */
			{
				cyhal_gpio_toggle(CYBSP_USER_LED); /* arbitrary effect */
				sliderValues[Slider0 - Slider0].previousSliderValue = sliderValues[Slider0 - Slider0].currentSliderValue; /* update previous value to current value */
			}
		}
	}
	else /* lift-off actions here */
	{
		cyhal_gpio_write(CYBSP_USER_LED, 1);
	}
}

