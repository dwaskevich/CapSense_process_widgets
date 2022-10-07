/*
 * systickTimer.h
 *
 *  Created on: Oct 7, 2022
 *      Author: Waskevich
 */

#ifndef SOURCE_SYSTICKTIMER_H_
#define SOURCE_SYSTICKTIMER_H_

#include "cycfg.h"

/* Set SysTick interval to 1 msec */
#define SYSTICK_INTERVAL (CY_SYSCLK_IMO_FREQ / 1000)

#define TOUCH_TICK_MSEC				(50u)

void SysTickTimerInit(void);
void SysTickISRCallback(void);

#endif /* SOURCE_SYSTICKTIMER_H_ */
