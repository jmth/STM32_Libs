/**
*@file main.c
*@author Jose Manuel Terres jmth94@electronicaengeneral.com
*@date 5 September 2017
*@brief ADC_DMALib example to read 3 ADC pins and transfer values directly to memory with DMA
*@note May be compatible with nearly every STM32 with correct modifications, tested in STM32F10x
*/

#include "stm32f10x.h" // NOTE: Change to match your device family
#include <stdio.h>
#include "ADC_DMALib.h"

// Okay this is simple, you initialize the hardware and forget about it, use the values you get
int main(){
	ADC_DMA(); // Just change the periphs numbers, pins and includes to use with your device
	if(ADC_GetVal(0) > 3000){
		// Do something
	}
while(1);
}