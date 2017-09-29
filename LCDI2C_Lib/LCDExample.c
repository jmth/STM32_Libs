/**
*@file main.c
*@author Jose Manuel Terres jmth94@electronicaengeneral.com
*@date 5 September 2017
*@brief LCD I2C example program for STM32 (used STM32F100 @24MHz)
*/

#include "stm32f10x.h" // NOTE: Change to match your device family
#include <stdio.h>
#include "delay.h"
#include "LiquidCrystal_I2C.C"

int main(){
	DelayInit();
	LCDI2C_init(0x27,16,2); // NOTE: Change 0x27 to your LCD I2C Address and 16,2 to your LCD dimensions
	LCDI2C_clear(); // This will clear the LCD and set cursor to 0,0
	LCDI2C_backlight(); // Turn on backlight
	LCDI2C_write_String("Hello"); // Write something
	LCDI2C_setCursor(0,1); // Set cursor to the next line
	LCDI2C_write_String("World"); // Write something else
	
	// RECOMMENDATION: You can use this too: (please set startup_stm32fxxx.s Stack_Size EQU to 0x00001000)
	// char line1[16], line2[16];
	// int i = 212;
	// sprintf(line1,"Something here");
	// sprintf(line2,"%d",i);
	// 
	// LCDI2C_write_String(line1);
	// LCDI2C_setCursor(0,1);
	// LCDI2C_write_String(line2);
	// NOTE: With this you can for example switch between menu screens using sprintf twice and then clear/write the screen only once
	while(1);
}