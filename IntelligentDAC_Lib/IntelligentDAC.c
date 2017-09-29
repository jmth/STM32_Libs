/**
*@file IntelligentDAC.c
*@author Jose Manuel Terres jmth94@electronicaengeneral.com
*@date 5 September 2017
*@brief Please read the document
*/

	#include "stm32f10x.h" //NOTE: Change this to match your device family
	#include "IntelligentDAC.h"
	
	uint16_t Square12bit[2]; //!< This will have the square high and low values
	uint32_t DualSquare12bit[2]; //!< This will fit the Square12bit values to the DAC register
	uint16_t TimerSetTable[2]; //!< This will have the timer AutoReload values to make a false PWM
	
	/* Init Structure definition */
	DAC_InitTypeDef            DAC_InitStructure;
	DMA_InitTypeDef            DMA_InitStructure;
	TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
	uint32_t Idx = 0;  

/**
*@function IntelligentDAC_Init
*@brief The intelligent DAC initialization
*@param val1 LOW DAC value
*@param val2 HIGH DAC value
*@param time1 HIGH time
*@param time2 TOTAL (HIGH+LOW) time
*/
void IntelligentDAC_Init(uint16_t val1, uint16_t val2,uint16_t time1, uint16_t time2){
	Square12bit[1] = val1;
	Square12bit[0] = val2;
	TimerSetTable[1] = (uint16_t)((time1*2.4)-1);
	TimerSetTable[0] = (uint16_t)(((time2-time1)*2.4)-1);
	/* Enable peripheral clocks ------------------------------------------------*/
#if !defined STM32F10X_LD_VL && !defined STM32F10X_MD_VL
  /* DMA2 clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
#else
  /* DMA1 clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
#endif
  /* GPIOA Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  /* DAC Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
  /* TIM2 Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
  /* Once the DAC channel is enabled, the corresponding GPIO pin is automatically 
     connected to the DAC converter. In order to avoid parasitic consumption, 
     the GPIO pin should be configured in analog */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* TIM3 Configuration */
  /* Time base configuration */
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); 
  TIM_TimeBaseStructure.TIM_Period = 2399;          
  TIM_TimeBaseStructure.TIM_Prescaler = 9999;       
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;    
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	// PWM Channel 1 initialization
	TIM_OCInitTypeDef OC_InitStructure;
	OC_InitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	OC_InitStructure.TIM_OutputState = TIM_OutputState_Enable;
	OC_InitStructure.TIM_Pulse = 1999;
	OC_InitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	
	// Enable PWM and preload
	TIM_OC1Init(TIM3,&OC_InitStructure);

  /* TIM2 TRGO selection */
  TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update); // Timer update creates an internal trigger
	
  /* DAC channel1 Configuration */
  DAC_InitStructure.DAC_Trigger = DAC_Trigger_T3_TRGO; // DAC will be triggered by T3 internal trigger
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
  DAC_Init(DAC_Channel_1, &DAC_InitStructure);

  /* DAC channel2 Configuration */
  DAC_Init(DAC_Channel_2, &DAC_InitStructure);

  /* Fill Sine32bit table */
  for (Idx = 0; Idx < 2; Idx++)
  {
    DualSquare12bit[Idx] = (Square12bit[Idx] << 16) + (Square12bit[Idx]);
  }

#if !defined STM32F10X_LD_VL && !defined STM32F10X_MD_VL
  /* DMA2 channel4 configuration */
  DMA_DeInit(DMA2_Channel4);
#else
  /* DMA1 channel4 configuration */
  DMA_DeInit(DMA1_Channel4);
#endif
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&DAC->DHR12RD; // DAC dual 12bit register addresss
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&DualSquare12bit; // Signal array address
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST; 
  DMA_InitStructure.DMA_BufferSize = 2; // Will transfer 2 values
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // No Peripheral memory address increase
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; // Memory address increase
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word; // 32-bit
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word; // 32-bit
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; // Will start again when needed
  DMA_InitStructure.DMA_Priority = DMA_Priority_High; // High priority to use the bus
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; // No memory to memory

#if !defined STM32F10X_LD_VL && !defined STM32F10X_MD_VL
  DMA_Init(DMA2_Channel4, &DMA_InitStructure);
  /* Enable DMA2 Channel4 */
  DMA_Cmd(DMA2_Channel4, ENABLE);
#else
  DMA_Init(DMA1_Channel4, &DMA_InitStructure);
  /* Enable DMA1 Channel4 */
  DMA_Cmd(DMA1_Channel4, ENABLE);
#endif
  /* Enable DAC Channel1: Once the DAC channel1 is enabled, PA.04 is 
     automatically connected to the DAC converter. */
  DAC_Cmd(DAC_Channel_1, ENABLE); 
  /* Enable DAC Channel2: Once the DAC channel2 is enabled, PA.05 is 
     automatically connected to the DAC converter. */
  DAC_Cmd(DAC_Channel_2, ENABLE);

  /* Enable DMA for DAC Channel2 */
  DAC_DMACmd(DAC_Channel_2, ENABLE); // Once triggered by Timer, DAC requests another value to DMA
	
	// Another DMA configuration for timer set
	DMA_DeInit(DMA1_Channel3);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&TimerSetTable; // Array address
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&TIM3->ARR; // Timer autoreload address
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = 2; // Will transfer two values
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //16-bit
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; // 16-bit
	DMA_Init(DMA1_Channel3,&DMA_InitStructure);
	DMA_Cmd(DMA1_Channel3,ENABLE);
	TIM_DMACmd(TIM3,TIM_DMA_Update,ENABLE); // Magic happens here
	// The same Timer trigger requests another DMA channel to change the Autoreload register, creating a PWM
	
  /* TIM2 enable counter */
  TIM_Cmd(TIM3, ENABLE); // Enable Timer to start the cycle

}

/**
*@function StopDAC
*@brief Stop all the Intelligent DAC peripherals and set DAC to 0
*/
void StopDAC(){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
	TIM_Cmd(TIM3,DISABLE);
	TIM_DMACmd(TIM3,TIM_DMA_Update,DISABLE);
	DMA_Cmd(DMA1_Channel3,DISABLE);
	DMA_Cmd(DMA1_Channel4,DISABLE);
	DAC_DMACmd(DAC_Channel_1,DISABLE);
	DAC_DMACmd(DAC_Channel_2,DISABLE);
	DAC_StructInit(&DAC_InitStructure);
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_Software;
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
  DAC_Init(DAC_Channel_1, &DAC_InitStructure);
	DAC_Init(DAC_Channel_2, &DAC_InitStructure);
	DAC_SetDualChannelData(DAC_Align_12b_R,0,0);
	DAC_DualSoftwareTriggerCmd(ENABLE);
}
