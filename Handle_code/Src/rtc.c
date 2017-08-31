/**
  ******************************************************************************
  * File Name          : RTC.c
  * Description        : This file provides code for the configuration
  *                      of the RTC instances.
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
#include "rtc.h"

/* USER CODE BEGIN 0 */
uint16_t RTC_CON_SET=1;
uint16_t RTC_CON_GET=2;

uint16_t g_rtc_con         = 2;
uint16_t g_rtc_year        = 2016;
uint16_t g_rtc_month       = 3;
uint16_t g_rtc_day         = 25;
uint16_t g_rtc_hour        = 0;
uint16_t g_rtc_minute      = 0;
uint16_t g_rtc_second      = 0;
uint16_t g_rtc_millisecond = 0;
/* USER CODE END 0 */

RTC_HandleTypeDef hrtc;

/* RTC init function */
void MX_RTC_Init(void)
{
  RTC_TimeTypeDef sTime;
  RTC_DateTypeDef sDate;

    /**Initialize RTC and set the Time and Date 
    */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }

  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 0x1;
  sDate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }

    /**Enable the WakeUp 
    */
  if (HAL_RTCEx_SetWakeUpTimer(&hrtc, 0, RTC_WAKEUPCLOCK_RTCCLK_DIV16) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_RTC_MspInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspInit 0 */

  /* USER CODE END RTC_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_RTC_ENABLE();
  /* USER CODE BEGIN RTC_MspInit 1 */

  /* USER CODE END RTC_MspInit 1 */
  }
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspDeInit 0 */

  /* USER CODE END RTC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_RTC_DISABLE();
  }
  /* USER CODE BEGIN RTC_MspDeInit 1 */

  /* USER CODE END RTC_MspDeInit 1 */
} 

/* USER CODE BEGIN 1 */
void APP_RTC_Set(void)
{
    RTC_TimeTypeDef  sTime;
	  RTC_DateTypeDef  sData;
	  
	  if(g_rtc_con==RTC_CON_SET)
		{
		    g_rtc_con=RTC_CON_SET;
			  sData.WeekDay = RTC_WEEKDAY_MONDAY ;
			  sData.Month   =g_rtc_month;
			  sData.Date    =g_rtc_day;
			  sData.Year    =g_rtc_year%100;
			  HAL_RTC_SetDate (&hrtc,&sData,FORMAT_BIN);
			  
			  sTime.Hours  =  g_rtc_hour ;
			  sTime.Minutes=  g_rtc_minute ;
			  sTime.Seconds=  g_rtc_second ;
			   
			  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
			  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
			  HAL_RTC_SetTime(&hrtc,&sTime,FORMAT_BIN);
		}
}


//read the data and time
void APP_RTC_Get(void)
{
    RTC_TimeTypeDef  sTime;
	  RTC_DateTypeDef  sData;
	  
	  if(g_rtc_con==RTC_CON_GET)
		{
		    
			  HAL_RTC_GetTime(&hrtc,&sTime,FORMAT_BIN);
			  
			  g_rtc_hour  =   sTime.Hours ;
			  g_rtc_minute =  sTime.Minutes ;
			  g_rtc_second =  sTime.Seconds ;
			
			  HAL_RTC_GetDate(&hrtc,&sData,FORMAT_BIN);
			 
			  g_rtc_month=sData.Month;
			  g_rtc_day  =sData.Date;
			  g_rtc_year =sData.Year+2000;
			
			 // printf("The Current Time: %d-%d-%d %d:%d:%d\r\n",g_rtc_year,g_rtc_month,g_rtc_day,g_rtc_hour,g_rtc_minute,g_rtc_second);
			   
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
