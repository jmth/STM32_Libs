/**
*@file main.c
*@author Jose Manuel Terres
*@date 28 September 2017
*@brief Example program for ds18b20 library
*/

#include "stm32f10x.h"
#include "delay.h"
#include "ds18b20.h"

int main(){
	DelayInit();
	// Initialize the sensor pin
	ds18b20_Init(GPIOA,GPIO_Pin_12,TIM15); // NOTE: Change to match your port, pin and timer
	float cels = 0;
	temperature_convert(); // Initialize the sensor conversion
	DelayMs(1000);
	cels = ds18b20_celsius(get_temperature()); // Read and convert temperature in celsius
	while(1);
}