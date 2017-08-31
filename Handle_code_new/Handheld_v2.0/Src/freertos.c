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
#include "SDCard.h"
/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/
osThreadId defaultTaskHandle;
osTimerId sender_TimerHandle;

/* USER CODE BEGIN Variables */
osThreadId keyTaskHandle;

/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void StartDefaultTask(void const * argument);
void Callback_sender_Timer(void const * argument);

extern void MX_FATFS_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN FunctionPrototypes */
void KeyTask(void const * argument);

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
  /* definition and creation of sender_Timer */
  osTimerDef(sender_Timer, Callback_sender_Timer);
  sender_TimerHandle = osTimerCreate(osTimer(sender_Timer), osTimerPeriodic, NULL);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  osTimerStart(sender_TimerHandle,1);

  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 1280);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  osThreadDef(keyTask, KeyTask, osPriorityNormal, 0, 1280);
  keyTaskHandle = osThreadCreate(osThread(keyTask), NULL);
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
  uint32_t count;
  BELL_ON;
  osDelay(100);
  BELL_OFF;
  sys_data_init();
  Write_StartData_To_SDCard();
  HAL_UART_Receive_IT(&huart1,(uint8_t *)&rx_buffer_1.s1,1);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1); 
  
  /* Infinite loop */
  for(;;)
  {
    while(sys.Get_Flat==0)
    {
      if(sys.key>0)
      {
        SendKey(0xfe,sys.key);
        sys.key=0;          
      }
      osDelay(10);
    }
    if(sys.key>0)
    {
      SendKey(0xfe,sys.key);
      sys.key=0;          
    }
    sys.Get_Flat=0;
    Generate_Data(&Data_Pack,1,20,(unsigned char *)sys.ecg1buff);//ecg data
    Generate_Data(&Data_Pack,2,20,(unsigned char *)sys.ecg2buff);
    Generate_Data(&Data_Pack,3,20,(unsigned char *)sys.ecg3buff);
    Generate_Data(&Data_Pack,4,8,(unsigned char *)sys.mpuAxbuff);//mpu data
    Generate_Data(&Data_Pack,5,8,(unsigned char *)sys.mpuAybuff);
    Generate_Data(&Data_Pack,6,8,(unsigned char *)sys.mpuAzbuff);
    Generate_Data(&Data_Pack,7,4,(unsigned char *)&sys.ZuKang);//huxi data
    SendPACK(0xff,Data_Pack.size,Data_Pack.buffer);
    Data_Pack.size=0;
  }
  /* USER CODE END StartDefaultTask */
}

/* Callback_sender_Timer function */
void Callback_sender_Timer(void const * argument)
{
  /* USER CODE BEGIN Callback_sender_Timer */
    static int count=0,countmpu=0,countother=0,i;
    static unsigned char temp=0,temp2=0;
  //HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_5);
  switch(count++)
  {
    case 0:
      break;
    case 1:
      count=0;
      break;
    default:break;
  }
  /* USER CODE END Callback_sender_Timer */
}

/* USER CODE BEGIN Application */
void KeyTask(void const * argument)
{
  /* init code for FATFS */
  uint32_t count,key=0,key_new=0;

  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    key=GetPress();
    if(key>0)
    {
     sys.key=key;
      BELL_ON;
      osDelay(50);
      BELL_OFF;

       
    }
    osDelay(20);
  }
  /* USER CODE END StartDefaultTask */
}     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
