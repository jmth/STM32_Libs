/**
*@file ds18b20.c
*@author Jose Manuel Terres
*@date 28 September 2017
*@brief Read Maxim DS18B20 temperature sensors
*@note Needs delay.c library and one timer
*/

#include "stm32f10x.h" // Change this to match your device family
#include "ds18b20.h"
#include "delay.h"

/**@function ds18b20_Init
*@brief Initialize pin, port and timer for OneWire communication
*@param GPIO_TypeDef* GPIOx sensor port {A,B,C}
*@param uint16_t pin sensor pin
*@param TIM_TypeDef* tim timer to generate delays {1,2,3,4,6,7,15,16,17}
*@note You can add new GPIOx ports or new Timers if required by simply adding more else if statements. Code optimized for STM32F10x series, 24 MHz SysClock, should be compatible with almost any STM32 family
*@return None
*/
void ds18b20_Init(GPIO_TypeDef* GPIOx,uint16_t pin,TIM_TypeDef* tim){
	uint8_t temp;
	ds18b20_pin = pin;
	ds18b20_gpio = GPIOx;
	ds18b20_tim = tim;
	if(GPIOx == GPIOA){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	} else if(GPIOx == GPIOB){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	} else if(GPIOx == GPIOC){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	}
	if(tim == TIM1){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	} else if(tim == TIM2){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	} else if(tim == TIM3){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	} else if(tim == TIM4){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	} else if(tim == TIM15){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM15,ENABLE);
	} else if(tim == TIM16){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16,ENABLE);
	} else if(tim == TIM17){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM17,ENABLE);
	} else if(tim == TIM6){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
	} else if(tim == TIM7){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);
	}
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 0;
	TIM_TimeBaseInitStruct.TIM_Period = 65535;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(tim, &TIM_TimeBaseInitStruct);
	TIM_Cmd(tim,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Pin = pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOx,&GPIO_InitStructure);
	do{
		temperature_convert();
		DelayMs(800);
		temp = get_temperature();
	}while(temp==85);
	temperature_convert();
}

/**@function temperature_convert
*@brief Tells all the sensors to start converting temperature
*@param None
*@return None
*/
void temperature_convert(void){
	ds18b20_reset();
	ds18b20_write_byte(0xCC);
	ds18b20_write_byte(0x44);
}

/**@function ds18b20_reset
*@brief Reset the bus to start communication
*@param None
*@return uint8_t Flag {1 if there are sensors, 0 if not}
*/
uint8_t ds18b20_reset(void){
	uint8_t Flag;
	
	GPIO_ResetBits(ds18b20_gpio,ds18b20_pin);
	microdelay(480);
	GPIO_SetBits(ds18b20_gpio,ds18b20_pin);
	DelayUs(60);
	if(!GPIO_ReadInputDataBit(ds18b20_gpio,ds18b20_pin)) Flag = 1;
	else Flag = 0;
	while(!GPIO_ReadInputDataBit(ds18b20_gpio,ds18b20_pin));
	return Flag;
}

/**@function ds18b20_write_byte
*@brief Transmit one byte to the bus
*@param uint8_t cdat byte data
*@return None
*/
void ds18b20_write_byte(uint8_t cdat){
	uint8_t i, one_bit;
	uint32_t t;
	for(i=0;i<8;i++){
		one_bit = cdat & 0x01;
		cdat = cdat >> 1;
		GPIO_ResetBits(ds18b20_gpio,ds18b20_pin);
		if(one_bit){
			microdelay(2);
			GPIO_SetBits(ds18b20_gpio,ds18b20_pin);
			microdelay(58);
		}
		else{
			microdelay(60);
		}
		GPIO_SetBits(ds18b20_gpio,ds18b20_pin);
		microdelay(2);
	}
}

/**@function get_temperature
*@brief Read temperature data from the sensors memory
*@param None
*@return uint16_t temperature data in 16-bit format
*/
uint16_t get_temperature(void){
	uint16_t a,b;
	uint16_t val;
	ds18b20_reset();
	ds18b20_write_byte(0xCC);
	ds18b20_write_byte(0xBE);
	a = (uint16_t)ds18b20_read_byte();
	b = (uint16_t)ds18b20_read_byte();
	val = (uint16_t)(b << 8 | a);
	ds18b20_reset();
	return val;
}

/**@function ds18b20_read_byte
*@brief Read one byte from the data bus
*@param None
*@return uint8_t data byte
*/
uint8_t ds18b20_read_byte(void){
	uint8_t i;
	uint8_t cdat;
	uint8_t value=0;
	for(i=0;i<8;i++){
		GPIO_ResetBits(ds18b20_gpio,ds18b20_pin);
		microdelay(2);
		GPIO_SetBits(ds18b20_gpio,ds18b20_pin);
		microdelay(15);
		cdat = GPIO_ReadInputDataBit(ds18b20_gpio,ds18b20_pin);
		if(cdat==0) while(!GPIO_ReadInputDataBit(ds18b20_gpio,ds18b20_pin));
		else microdelay(30);
		value = (uint16_t)(cdat << 7)|(value >> 1);
	}
	return value;
}

/**@function microdelay
*@brief Generate a delay from the selected timer
*@param uint16_t us {1 - 10000}
*@return None
*/
void microdelay(uint16_t us){
	uint16_t val = us*(SystemCoreClock/1000000);
	ds18b20_tim->CNT = 0x0000;
	while(ds18b20_tim->CNT<val);
}

/**@function ds18b20_celsius
*@brief Transform sensor temperature data to celsius
*@param uint16_t temperature data in 16-bit format
*@return float temperature in celsius
*/
float ds18b20_celsius(uint16_t byteval){
	uint16_t inv = byteval & 0b1111100000000000;
	if(inv){
		byteval = (byteval^0xffff)+1;
	}
	if(inv){
		float val = byteval/16.0;
		return -val;
	}
	else{
	float val = byteval/16.0;
	return val;
	}
}