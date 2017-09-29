/**
*@file ADC_DMALib.c
*@author Jose Manuel Terres jmth94@electronicaengeneral.com
*@date 5 September 2017
*@brief ADC Initialization with automatic Peripheral to Memory value transfer by DMA
*@note Function is set to work with only 3 ADC pins in GPIOA, you will need to change some things (NOTEd), but take it as an example
*/

#include "stm32f10x.h" //NOTE: Change this to match your device family
#include "ADC_DMALib.h"

//NOTE: Increase array size to match your number of channels
uint16_t ADC_DualConvertedValueTab[3]; //!< To save the ADC values (0: GPIOA_0, 1: GPIOA_1 2: GPIOA_2)

/**
*@brief Initialize the ADC for pins PA0-PA2 with DMA transferring values directly to an array
*/
void ADC_DMA(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE); //NOTE: Change to your port if ADC is not in GPIOA
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2; // Initialize the analog pins NOTE: Add more if needed
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; // Analog input mode
	GPIO_Init(GPIOA,&GPIO_InitStructure); // NOTE: Change to your port if ADC is not in GPIOA
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE); // Enable ADC Clock NOTE: Some STM32 may not have ADC in APB2 bus, and maybe you're not using ADC1
	
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; // Independent mode
	ADC_InitStructure.ADC_ScanConvMode = ENABLE; // Scan mode, read the next one without being triggered
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; // Continuously reading
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // No external trigger
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; // Data aligned to the right
	ADC_InitStructure.ADC_NbrOfChannel = 3; // Will read 3 channels NOTE: Change to your number of channels
	ADC_Init(ADC1,&ADC_InitStructure); // NOTE: Change ADC1 to your ADC Peripheral number
	
	// This sorts the pins to read in a specific order NOTE: add more if needed
	ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_55Cycles5); // Read Channel 0 first
	ADC_RegularChannelConfig(ADC1,ADC_Channel_1,2,ADC_SampleTime_55Cycles5); // Read Channel 1 second
	ADC_RegularChannelConfig(ADC1,ADC_Channel_2,3,ADC_SampleTime_55Cycles5); // Read Channel 2 third
	
	ADC_Cmd(ADC1,ENABLE); // Enable the ADC1 Peripheral
	ADC_DMACmd(ADC1,ENABLE); // Enable communication with DMA
	
	ADC_ResetCalibration(ADC1); // Calibrate the ADC
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));

	
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE); // Enable DMA Clock
	
	DMA_DeInit(DMA1_Channel1); // Reset configuration for Channel 1
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR; // Read from here (remember & returns an address, this needs that address)
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_DualConvertedValueTab; // To here
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; // Direction peripheral to memory
	DMA_InitStructure.DMA_BufferSize = 3; // Will transfer 3 values
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // Do not increase peripheral memory position
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; // Do increase memory (memory) position
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; // 16-bit
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; // 16-bit
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; // Circular mode will restart ADC and read continuously
	DMA_InitStructure.DMA_Priority = DMA_Priority_High; // High priority to use the bus
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; // Disable memory-to-memory
	DMA_Init(DMA1_Channel1,&DMA_InitStructure); // NOTE: Maybe you're not using DMA1 or Channel1, change everything to your periph number

	DMA_Cmd(DMA1_Channel1,ENABLE); // Enable DMA Peripheral
	ADC_SoftwareStartConvCmd(ADC1,ENABLE); // Start ADC conversions
}

/**
*@brief This little function will help you read the values from the array
*@param i (0: GPIOA_0, 1: GPIOA_1 2: GPIOA_2) or whatever order you configured
*/
uint16_t ADC_GetVal(uint8_t i){
	return ADC_DualConvertedValueTab[i];
}