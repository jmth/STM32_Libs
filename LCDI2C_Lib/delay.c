/**
  ******************************************************************************
  * @file		delay.c
  * @author	Yohanes Erwin Setiawan
	* @note 	Modified by Jose Manuel Terres for STM32F100 (24 MHz) compatibility
  * @date		10 January 2016
  ******************************************************************************
  */
	
#include "delay.h"


// For store tick counts in us
static __IO uint32_t usTicks;

// SysTick_Handler function will be called every 2 us

/**@brief SysTick_Handler, for SysTick interruptions
*@param None
*@return None
*@warning Do not change "Code for delays" section
*@note SysTick set at 2 us interruptions for LCD driver
*/
void SysTick_Handler()
{
// Code for delays
	if (usTicks != 0){
			usTicks--;
	}
}

/**@brief Set the SysTick timer to 2 us
*@warning Do not change divider value
*@note May need changes if device is not compatible or clock is set wrong
*@note Possible nonsense warning here
*/
void DelayInit()
{
	// Update SystemCoreClock value
	SystemCoreClockUpdate();
	// Configure the SysTick timer to overflow every 2 us
	SysTick_Config(SystemCoreClock/500000); // Possible nonsense warning here
}

/**@brief load usTicks and wait, causing a delay
*@note using this method will not affect other timers
*@param us Microseconds
*/
void DelayUs(uint32_t us)
{
	// Reload us value
	usTicks = us/2.0;
	// Wait until usTick reach zero
	while (usTicks);
}

/**@brief tell DelayUs to load usTicks
*@param ms Milliseconds
*/
void DelayMs(uint32_t ms)
{
	// Wait until ms reach zero
	while (ms--)
	{
		// Delay 1ms
		DelayUs(1000);
	}
}

/********************************* END OF FILE ********************************/
/******************************************************************************/
