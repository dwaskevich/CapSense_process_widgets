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

#define NO_SLIDERS_ACTIVE				(0xFFFF)

/* enumeration for sliders */
enum {
	Slider0,
	numSliders
};

typedef struct
{
	uint16_t 	currentSliderValue;
	uint16_t 	previousSliderValue;
	uint32_t 	lastUpdateCounterValue;
	bool 		touchStatus;
} sliderValues_t;

uint32_t processSliders(void);
uint16_t calculateSliderValue(uint16_t sliderValue);


#endif /* PROCESSSLIDERS_H_ */
