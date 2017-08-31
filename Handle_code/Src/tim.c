/**
  ******************************************************************************
  * File Name          : TIM.c
  * Description        : This file provides code for the configuration
  *                      of the TIM instances.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "tim.h"

/* USER CODE BEGIN 0 */
#include "ECG.h"
#include "SDCard.h"
#include "usart.h"
#include "gpio.h"
#include "oled.h"

uint16_t MOTOR_cnt = 0;//确认键查询到按下次数

extern uint8_t MOTOR;
extern uint8_t Pointer;
extern uint8_t DisplayECGWave;//显示心电波形标志位，默认只显示心率值
extern uint8_t DisplayBRWave;//显示呼吸波形标志位，默认只显示呼吸率值

extern uint8_t SDCardValid;

extern void DisplaySelected(void);
extern void Speed_Dial(void);
/* USER CODE END 0 */

TIM_HandleTypeDef htim2;

/* TIM2 init function */
void MX_TIM2_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 100;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 10000;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspInit 0 */

  /* USER CODE END TIM2_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_TIM2_CLK_ENABLE();
		
		/* Peripheral interrupt init */
    HAL_NVIC_SetPriority(TIM2_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
  /* USER CODE BEGIN TIM2_MspInit 1 */

  /* USER CODE END TIM2_MspInit 1 */
  }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspDeInit 0 */

  /* USER CODE END TIM2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM2_CLK_DISABLE();
		
		 /* Peripheral interrupt Deinit*/
    HAL_NVIC_DisableIRQ(TIM2_IRQn);
		
  }
	
  /* USER CODE BEGIN TIM2_MspDeInit 1 */

  /* USER CODE END TIM2_MspDeInit 1 */
} 

/* USER CODE BEGIN 1 */
void HAL_SYSTICK_Callback(void)
{
//  if(SDCardValid==1)  
//		SDCard_Buffer_Handler();  
	
	if(MOTOR==1)
	{
	  MOTOR_cnt++;
		if(MOTOR_cnt==100)
		{
		  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11, GPIO_PIN_RESET);//关闭震动马达
		}
		if(MOTOR_cnt==300)
		{
//		  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11, GPIO_PIN_SET);//开启震动马达
		}
		if(MOTOR_cnt==400)
		{
		  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11, GPIO_PIN_RESET);//关闭震动马达
			MOTOR=0;//清除开启震动马达开启标志位
			MOTOR_cnt=0;//清除计数
		}
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim->Instance == TIM2)
	{
	  static uint8_t Submit_cnt = 0;//确认键查询到按下次数
		static uint8_t Exit_cnt   = 0;//退出键查询到按下次数
		static uint8_t Up_cnt     = 0;//向上键查询到按下次数
		static uint8_t Down_cnt   = 0;//向下键查询到按下次数
		
		//每隔10ms查询一次按键端口状态，连续为低，则每次加一，否则计数清0
		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_10)==GPIO_PIN_RESET)
		{
		  Submit_cnt++;
		}
		else
		{
		  Submit_cnt=0;
		}
		
		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_12)==GPIO_PIN_RESET)
		{
		  Exit_cnt++;
		}
		else
		{
		  Exit_cnt=0;
		}
//		
//		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_13)==GPIO_PIN_RESET)
//		{
//		  Up_cnt++;
//		}
//		else
//		{
//		  Up_cnt=0;
//		}
		
		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14)==GPIO_PIN_RESET)
		{
		  Down_cnt++;
		}
		else
		{
		  Down_cnt=0;
		}
		
		//按键确实按下
		if(Submit_cnt>=10)//确认按键确实按下
		{
		  Submit_cnt=0;
			
			DisplayECGWave=1;
			DisplayBRWave=1;
      OLED_Clear();			
		}
		
		if(Exit_cnt>=10)//退出按键确实按下
		{
		  Exit_cnt=0;
			
			DisplayECGWave=0;
			DisplayBRWave=0;
			Pointer=1;
			
		  Speed_Dial();
		}
		
//		if(Up_cnt>=20)//按键确实按下
//		{
//		  Up_cnt=0;
//			Pointer++;
//			if(Pointer>=3) Pointer=1;
//			
//			DisplaySelected();
//			Driver_Motor();
//		}
		
		if(Down_cnt>=10)//移动按键确实按下
		{
		  Down_cnt=0;
			Pointer++;
			if(Pointer>=4) Pointer=1;
			
			DisplaySelected();
//			Driver_Motor();
		}
	}
}
/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
