/*
 * processSliders.c
 *
 *  Created on: Sep 30, 2022
 *      Author: Waskevich
 *
 *  Process sliders and update slider LED rings
 *
 */

#include "cyhal.h"
#include "cybsp.h"
#include "cycfg_capsense.h"
#include "processSliders.h"

sliderValues_t sliderValues[numSliders] =
{
		{0, 0, 0, 0}
};

uint32_t processSliders(void)
{
	uint32_t status = 0;
	static uint32_t capsenseScanCounter = 0;
    cy_stc_capsense_touch_t *ptrTouch;
    static uint32_t activeSlider = NO_SLIDERS_ACTIVE;

    capsenseScanCounter++;

    if(Cy_CapSense_IsAnyWidgetActive(&cy_capsense_context))
    {
		/* check for previously active slider */
		if(NO_SLIDERS_ACTIVE != activeSlider) /* prioritize previously active slider, skip others ... only one slider active at a time */
		{
			ptrTouch = Cy_CapSense_GetTouchInfo(activeSlider, &cy_capsense_context);
			sliderValues[activeSlider].currentSliderValue = ptrTouch->ptrPosition->x;
			sliderValues[activeSlider].lastUpdateCounterValue = capsenseScanCounter;

			if(sliderValues[activeSlider].previousSliderValue != sliderValues[activeSlider].currentSliderValue)
			{
				if(sliderValues[activeSlider].currentSliderValue - sliderValues[activeSlider].previousSliderValue > 25)
				{
					cyhal_gpio_toggle(CYBSP_USER_LED);
					sliderValues[activeSlider].previousSliderValue = sliderValues[activeSlider].currentSliderValue;
				}
			}
		}
		else /* no sliders currently active ... look for active slider */
		{
			for(uint32_t wdgtIndex = CY_CAPSENSE_LINEARSLIDER0_WDGT_ID; wdgtIndex <= CY_CAPSENSE_LINEARSLIDER0_WDGT_ID; wdgtIndex++)
			{
				if(Cy_CapSense_IsWidgetActive(wdgtIndex, &cy_capsense_context))
				{
					ptrTouch = Cy_CapSense_GetTouchInfo(wdgtIndex, &cy_capsense_context);
					sliderValues[wdgtIndex].currentSliderValue = ptrTouch->ptrPosition->x;
					sliderValues[wdgtIndex].lastUpdateCounterValue = capsenseScanCounter;

					if(false == sliderValues[wdgtIndex].touchStatus) /* this is a new event */
					{
						sliderValues[wdgtIndex].touchStatus = true;
						sliderValues[wdgtIndex].previousSliderValue = sliderValues[wdgtIndex].currentSliderValue;
					}

					activeSlider = wdgtIndex;

					break;
				}
				else
				{
					sliderValues[wdgtIndex].touchStatus = false;
				}
			}
		}
    }
    else
    {
		/* check sliders to see if they went inactive on this scan */
		if(NO_SLIDERS_ACTIVE != activeSlider)
		{
			for(uint32_t wdgtIndex = CY_CAPSENSE_LINEARSLIDER0_WDGT_ID; wdgtIndex <= CY_CAPSENSE_LINEARSLIDER0_WDGT_ID; wdgtIndex++)
			{
				if(true == sliderValues[wdgtIndex].touchStatus)
				{
					sliderValues[wdgtIndex].touchStatus = false;
					break;
				}
			}

			activeSlider = NO_SLIDERS_ACTIVE;
		}
    }

	return status;
}


