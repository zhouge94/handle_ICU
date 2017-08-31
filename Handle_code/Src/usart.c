/**
  ******************************************************************************
  * File Name          : USART.c
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
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
#include "usart.h"

#include "gpio.h"
#include "dma.h"

/* USER CODE BEGIN 0 */
#include "ECG.h"
#include "RSP.h"

uint8_t Raw_ECG_buf[10]={0XAA,0XAA,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00};//数据存放数组用来存放原始的心电数据8个八进制数
uint8_t ECG_cnt=2;//一组心电数据接收个数计数

uint8_t RcvData_Flag=0;//接收到数据标识： RcvData_Flag=1 接收到数据
uint8_t ReciveECG_total=0;  //接收到串口发送的心电数据计数
uint8_t Head=0;//判断接收到的是不是一组心电数据的数据头的第一位
uint8_t Data_type=0;//判断接收到的数据是什么类型的，
uint8_t USART1_ReciveData=0;//串口1接收到的数据
uint8_t ReciveData_Buffer[20]={0XAA,0XAA,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00};//串口接收数据存储区
uint8_t FindFrameHeader=0;//串口接收到的数据发现帧头
uint8_t ReciveDataComplete=0;//数据接收完成
uint8_t ECG_Display_cnt=0;

uint8_t ECG_Data_Div=1;//心电数据取数间隔

extern uint8_t Pointer;

extern uint8_t Rx_buffer[128];//串口接收数据存储区
extern uint8_t rx_len;//接收到的数据长度
extern uint8_t rx_buffer[128];//串口DMA接收数据存储区

/* USER CODE END 0 */

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 57600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }

}
/* USART2 init function */

void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 57600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();
  
    /**USART1 GPIO Configuration    
    PA10     ------> USART1_RX
    PA15     ------> USART1_TX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


    /* Peripheral DMA init*/

    /* Peripheral interrupt init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
  else if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();
  
    /**USART2 GPIO Configuration    
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Peripheral DMA init*/
  
    hdma_usart2_rx.Instance = DMA1_Stream5;
    hdma_usart2_rx.Init.Channel = DMA_CHANNEL_4;
    hdma_usart2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart2_rx.Init.Mode = DMA_CIRCULAR;
    hdma_usart2_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart2_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart2_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart2_rx);

    hdma_usart2_tx.Instance = DMA1_Stream6;
    hdma_usart2_tx.Init.Channel = DMA_CHANNEL_4;
    hdma_usart2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart2_tx.Init.Mode = DMA_NORMAL;
    hdma_usart2_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart2_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart2_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart2_tx);
		

  /* USER CODE BEGIN USART2_MspInit 1 */

  /* USER CODE END USART2_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();
  
    /**USART1 GPIO Configuration    
    PA10     ------> USART1_RX
    PA15     ------> USART1_TX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_10|GPIO_PIN_15);

    /* Peripheral interrupt Deinit*/
    HAL_NVIC_DisableIRQ(USART1_IRQn);

  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();
  
    /**USART2 GPIO Configuration    
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);

    /* Peripheral DMA DeInit*/
    HAL_DMA_DeInit(uartHandle->hdmarx);
    HAL_DMA_DeInit(uartHandle->hdmatx);
		
  /* USER CODE BEGIN USART2_MspDeInit 1 */

  /* USER CODE END USART2_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */

/**
* @brief 缓存心电数据.
*/
void Save_ECG_Data(void)
{
	Head=0;
	ReciveData_Buffer[ECG_cnt++]=USART1_ReciveData;
	
  if(ECG_cnt==34) 
  {
	  ReciveECG_total=0;
		ECG_cnt=2;
    Data_type=0;
			
		ECG_DATA_Handler(ReciveData_Buffer,34);		
   }
}
/**
* @brief 缓存心率及导联脱落数据.
*/
void Save_HR_Data(void)
{
	Head=0;
	ReciveData_Buffer[ECG_cnt++]=USART1_ReciveData;
	
  if(ECG_cnt==10) 
  {
	  ReciveECG_total=0;
		ECG_cnt=2;
    Data_type=0;
			
		HR_DATA_Handler(ReciveData_Buffer,10);		
   }
}
/**
* @brief 缓存呼吸数据.
*/
void Save_RSP_Data(void)
{
	Head=0;
	ReciveData_Buffer[ECG_cnt++]=USART1_ReciveData;
	
  if(ECG_cnt==6) 
  {
	  ReciveECG_total=0;
		ECG_cnt=2;
    Data_type=0;
			
		RSP_DATA_Handler(ReciveData_Buffer,6);		
   }
}


/**
* @brief 缓存体温数据.
*/
void Save_TEM_Data(void)
{
	Head=0;
	ReciveData_Buffer[ECG_cnt++]=USART1_ReciveData;
	
  if(ECG_cnt==14) 
  {
	  ReciveECG_total=0;
		ECG_cnt=2;
    Data_type=0;
			
		TEM_DATA_Handler(ReciveData_Buffer,14);		
   }
}
/**
* @brief 缓存加速度数据.
*/
void Save_ACC_Data(void)
{
	Head=0;
	ReciveData_Buffer[ECG_cnt++]=USART1_ReciveData;
	
  if(ECG_cnt==5) 
  {
	  ReciveECG_total=0;
		ECG_cnt=2;
    Data_type=0;
			
		ACC_DATA_Handler(ReciveData_Buffer,5);		
   }
}
//串口接收中断处理回调函数
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
	USART1_ReciveData=rx_buffer[0];
	ReciveECG_total++;
	
	if(ReciveECG_total==1) 
  {
    if(USART1_ReciveData==0xAA) Head=1;   
    else{  ReciveECG_total=0; Head=0; }  
  }
  if(ReciveECG_total==2) 
  {
    if(USART1_ReciveData==0xAA) Head=1;   
		else{  ReciveECG_total=0; Head=0; }  
  }		
  if(ReciveECG_total==3&&Head==1) ReciveData_Buffer[ReciveECG_total-1]=USART1_ReciveData;
	
	if(ReciveECG_total==3&&Head==1)//发现帧头，之后第三位数据是数据类型标识，检查数据类型
  {	
	 switch(ReciveData_Buffer[2])
   {
     case 0X06 :   Data_type=0x01;   break; //心电波形数据
     case 0X12 :   Data_type=0x02;   break; //心率及导联脱落数据
     case 0X03 :   Data_type=0x03;   break; //呼吸数据
      case 0X05 :  Data_type=0x04;  break;
      case 0X02 :  Data_type=0x05;  break;
//		  case 8 :  dis_env();        break;
//		  case 9 :  Clear_OLED();     break;
//			case 10 : Save_HR_Data();   break;
			default:  {ReciveECG_total=0;Head=0;} break;
    }	
	}
		switch(Data_type)
    {
      case 1:   Save_ECG_Data();break; //心电波形数据
      case 2 :  Save_HR_Data() ;  break;  
      case 3 :  Save_RSP_Data();  break;
      case 4 :  Save_TEM_Data();  break;
      case 5 :  Save_ACC_Data();  break;
			default:   break;
    }	
  HAL_UART_Receive_IT(&huart1,rx_buffer,1);
}

/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
