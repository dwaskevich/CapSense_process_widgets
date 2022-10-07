/*
 * systickTimer.c
 *
 *  Created on: Oct 7, 2022
 *      Author: Waskevich
 */

#include "stdint.h"
#include "systickTimer.h"

/* SysTick counters (updated in ISR) */
volatile uint16_t sysTickTimer = 0;
volatile uint16_t sysTickTouchTimer = 0;


void SysTickTimerInit(void)
{
	Cy_SysTick_Init(CY_SYSTICK_CLOCK_SOURCE_CLK_IMO, SYSTICK_INTERVAL);

	/* find next available SysTick callback slot */
	for(uint8_t i = 0; i < CY_SYS_SYST_NUM_OF_CALLBACKS; i++)
	{
		if(Cy_SysTick_GetCallback(i) == NULL)
		{
			Cy_SysTick_SetCallback(i, SysTickISRCallback);
			break;
		}
	}
}


/*******************************************************************************
* Function Name: SysTickISRCallback
********************************************************************************
*
* Summary:
*  This API is called from SysTick timer interrupt handler to update the
*  sysTickCount counter.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void SysTickISRCallback(void)
{
	sysTickTimer++;
}
