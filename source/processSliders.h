/*
 * processSliders.h
 *
 *  Created on: Sep 30, 2022
 *      Author: Waskevich
 */

#ifndef PROCESSSLIDERS_H_
#define PROCESSSLIDERS_H_

#include "stdint.h"
#include "stdbool.h"

#define NUMBER_OF_SLIDERS				(1u)
#define NO_SLIDERS_ACTIVE				(0xFFFF)

/* enumeration for sliders */
enum {
	Slider0 = CY_CAPSENSE_LINEARSLIDER0_WDGT_ID
};

typedef struct
{
	uint16_t 	currentSliderValue;
	uint16_t 	previousSliderValue;
	uint32_t 	lastUpdateCounterValue;
	bool 		touchStatus;
} sliderValues_t;

uint32_t processSliders(void);
void processSliderEvents(uint32_t sliderNum, bool newEvent, uint16_t sliderValue);

void slider_0(bool newEvent, uint16_t sliderValue);


#endif /* PROCESSSLIDERS_H_ */
