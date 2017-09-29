/**
*@file ds18b20.h
*@author Jose Manuel Terres
*@date 28 September 2017
*@brief Read Maxim DS18B20 temperature sensors
*/

#include "stm32f10x.h"

static uint16_t ds18b20_pin; //!< Pin number where sensor is connected
static GPIO_TypeDef* ds18b20_gpio; //!< GPIO Port where sensor is connected {A,B,C}
static TIM_TypeDef* ds18b20_tim; //!< Timer to use to generate delays for onewire communication {1,2,3,4,6,7,15,16,17}

void ds18b20_Init(GPIO_TypeDef* GPIOx,uint16_t pin,TIM_TypeDef* tim);
void temperature_convert(void);
uint8_t ds18b20_reset(void);
void ds18b20_write_byte(uint8_t cdat);
uint16_t get_temperature(void);
uint8_t ds18b20_read_byte(void);
void microdelay(uint16_t us);
float ds18b20_celsius(uint16_t byteval);