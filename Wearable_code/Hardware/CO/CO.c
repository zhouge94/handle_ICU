#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"	 

#include "adc.h"
#include "dma.h"
#include "usart.h"
#include  <math.h>    //Keil library  //==========================================
#include "stdlib.h"

#include "CO.h"
#include "SDCard.h"

extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;

__IO uint16_t uhADCxConvertedValue[2];

uint16_t ADC1_CH1Value=0;
uint16_t ADC1_CH2Value=0;
/**
* @brief �ض���ADC1ת������ն˻ص�����.
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
	if(HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&uhADCxConvertedValue, 2) != HAL_OK)
  {
    /*Start Conversation Error*/
		printf("Start ADC Conversation Error!\r\n");
		Error_Handler();
  }
}
/**
* @brief ��ȡCOŨ��.
*/
void GET_CO_Concentration(void)
{
//    float CO=0,dat=0;
//	  if(dat<1300)
//		  CO=0;
//	  else
//	  {
//		   CO=(dat-1300)/4095.0*3.3/0.0045;
//	  } 
}
/**
* @brief ��ȡ���ʣ�����.
*/
void GET_BAT_Level(void)
{
	
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
