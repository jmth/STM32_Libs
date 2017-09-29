/**
*@file main.c
*@author Jose Manuel Terres jmth94@electronicaengeneral.com
*@date 5 September 2017
*@brief Example for Intelligent DAC Library. Please read the documentation.
*/

#include "stm32f10x.h" // NOTE: Change to match your device family
#include <stdio.h>
#include "IntelligentDAC.h"

// Simply start the DAC with the desired values of time and levels
int main(){
	// Output level: 0 V Low, 3.3 V HIGH
	// Time: 100 ms HIGH, 500 ms TOTAL (400 ms LOW)
	IntelligentDAC_Init(0,4095,100,500);
while(1);
}