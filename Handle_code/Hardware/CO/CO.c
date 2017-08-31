#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"	 

#include "adc.h"
#include "dma.h"
#include "usart.h"
#include  <math.h>    //Keil library  //==========================================
#include "stdlib.h"

#include "CO.h"
#include "oled.h"

extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;

__IO uint16_t uhADCxConvertedValue[2];

uint16_t ADC1_CH1Value=0;
uint16_t ADC1_CH2Value=0;

float CO=0;

extern void DisplayCO(uint16_t color,uint16_t backgroundcolor);

/**
* @brief 重定义ADC1转换完成终端回掉函数.
*/ 
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* AdcHandle)
{
  HAL_ADC_Stop_DMA(&hadc1);
	if(hadc1.Instance==ADC1)
	{
	  ADC1_CH1Value=(uint16_t)uhADCxConvertedValue[0];
		ADC1_CH2Value=(uint16_t)uhADCxConvertedValue[1];
		
		uhADCxConvertedValue[0]=0;
		uhADCxConvertedValue[1]=0;
	}
//	if(HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&uhADCxConvertedValue, 2) != HAL_OK)
//  {
//    /*Start Conversation Error*/
//		printf("Start ADC Conversation Error!\r\n");
//		Error_Handler();
//  }
}
/**
* @brief 获取CO浓度.
*/
void GET_CO_Concentration(void)
{
  float dat=0;
	
	dat=ADC1_CH1Value;

	if(dat<1300)
	  CO=0;
	else
	{
		 CO=(dat-1300)/4095.0*3.3/0.0045;
  }
	
	DisplayCO(Green,Black);
	
	//再次开启ADC的DMA转换
	/*-3- Start the conversion process and enable interrupt ###########[size=16]#*/
  if(HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&uhADCxConvertedValue, 2) != HAL_OK)
  {
    /*Start Conversation Error*/
		printf("Start ADC Conversation Error!\r\n");
		Error_Handler();
  }
	
	
    		
}
/**
* @brief 获取电池剩余电量.
*/
void GET_BAT_Level(void)
{
	
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
