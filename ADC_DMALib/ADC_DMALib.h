/**
*@file ADC_DMALib.h
*@author Jose Manuel Terres jmth94@electronicaengeneral.com
*@date 5 September 2017
*@brief ADC Initialization with automatic Peripheral to Memory value transfer by DMA
*@note Function is set to work with only 3 ADC pins in GPIOA, you will need to change some things (NOTEd), but take it as an example
*/

void ADC_DMA(void);
uint16_t ADC_GetVal(uint8_t i);
