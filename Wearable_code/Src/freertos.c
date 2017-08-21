/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */     
#include "sys.h"
#include "DataScope_DP.h"
#include "ECG.h"
/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/
osThreadId defaultTaskHandle;
osTimerId SenderHandle;

/* USER CODE BEGIN Variables */
osThreadId MPU_TaskHandle;
osThreadId Hx_TaskHandle;
/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void StartDefaultTask(void const * argument);
void Callback_Sender(void const * argument);

extern void MX_FATFS_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN FunctionPrototypes */
void MPU_Task(void const * argument);
void Hx_Task(void const * argument);

/* USER CODE END FunctionPrototypes */

/* Hook prototypes */

/* Init FreeRTOS */

void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
       
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* definition and creation of Sender */
  osTimerDef(Sender, Callback_Sender);
  SenderHandle = osTimerCreate(osTimer(Sender), osTimerPeriodic, NULL);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
	osTimerStart(SenderHandle,1);
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  osThreadDef(MPUTask, MPU_Task, osPriorityNormal, 0, 128);
  MPU_TaskHandle = osThreadCreate(osThread(MPUTask), NULL);

  osThreadDef(HxTask, Hx_Task, osPriorityNormal, 0, 128);
  Hx_TaskHandle = osThreadCreate(osThread(HxTask), NULL);

  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
}

/* StartDefaultTask function */
void StartDefaultTask(void const * argument)
{
  /* init code for FATFS */
  MX_FATFS_Init();

  /* USER CODE BEGIN StartDefaultTask */

	/******´ò¿ªUSART1,2£¬6¿ÕÏÐÖÐ¶Ï *****/	 		 
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE); 
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
	__HAL_UART_ENABLE_IT(&huart6, UART_IT_IDLE);
	
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* Callback_Sender function */
void Callback_Sender(void const * argument)
{
  /* USER CODE BEGIN Callback_Sender */
	 static int count=0,countmpu=0,countother=0;
	 static unsigned char temp=0;
   //printf("hello\r\n");
	 switch(count++)
	 {
	 case 0:
	 case 2:
	 case 4:
	 case 6:
	 case 8:
		 Send3ECG(1,ECG_DATA1.ECG_DATA,ECG_DATA2.ECG_DATA,temp++);
		 //Send3ECG(1,0,1,temp++);
		 break;
	 case 1:
		 //SendMPU(1,1,1,1,1);
		 //if(countmpu%2) SendFloat(0,sys.ACCEL_X);
		 //else SendFloat(3,sys.GYRO_X);
		 break;
	 case 3:
		 //if(countmpu%2) SendFloat(1,sys.ACCEL_Y);
		 //else SendFloat(4,sys.GYRO_Y);
		 break;
	 case 5:
		 //if(countmpu%2) SendFloat(2,sys.ACCEL_Z);
		 //else SendFloat(5,sys.GYRO_Z);
		 break;
	 case 7:
	 break;
	 case 9:
		 count=0;
	 break;
	 default:break;
	 }
  /* USER CODE END Callback_Sender */
}

/* USER CODE BEGIN Application */
void MPU_Task(void const * argument)
{
	static int count=0;
	Init_MPU6050();
	while(1)
	{
		READ_MPU6050();
		count++;
		osDelay(10);
	}
}	

void Hx_Task(void const * argument)
{
	int count=0;
	float temp_f[20],sum=0;
	Init_AD5933();
	osDelay(200);
	BeginSample();
	osDelay(200);
	while(1)
	{
		if(count++<10)
		{
			 temp_f[count]=Sample();
			 sum+=temp_f[count];

		}else
		{
			count=0;	
		  sys.ZuKang=sum/10.0;
			sum=0;			
		}
		osDelay(5);
	}

}	
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
