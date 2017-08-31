/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
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
#include "stm32f4xx_hal.h"
#include "adc.h"
#include "dma.h"
#include "fatfs.h"
#include "i2c.h"
#include "rtc.h"
#include "sdio.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "oled.h"
#include "ECG.h"
#include "SDCard.h"
#include "CO.h"
#include "SHT75.h"
#include "ms5611.h"

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
extern __IO uint16_t uhADCxConvertedValue;
extern uint16_t ADC1_CH1Value;
extern uint16_t ADC1_CH2Value;

extern char SDCard_Buf_1[6150];
extern char SDCard_Buf_2[6150];

uint8_t DisplaySpeedDial=1;//默认九宫格显示数据
uint8_t DisplayECGWave=0;//显示心电波形标志位，默认只显示心率值
uint8_t DisplayBRWave=0;//显示呼吸波形标志位，默认只显示呼吸率值

uint8_t Pointer=1;//九宫格指针

uint8_t SDCardValid=1;

extern uint8_t Heart_Rate;//心率
extern uint8_t  Breath_Rate;

extern float Heart_Rate_BW;
extern uint8_t Heart_Rate_SW;
extern uint8_t Heart_Rate_GW;

extern float CO;
extern uint16_t TEM,HUI;
extern uint32_t ex_Pressure;	//串口气压读数转换值
extern uint16_t SUM_ACCEL;

extern uint8_t rx_buffer[128];//串口DMA接收数据存储区
extern uint16_t BodyTemperature;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);
void Speed_Dial(void);
void DisplaySelected(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__

  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
 set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else

  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

void DisplayStatus(uint16_t color,uint16_t backgroundcolor,uint8_t status);
	
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
	
/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */
  char* Words = malloc(256);//动态内存分配：向系统申请分配指定size个字节的内存空间
	uint16_t iii=0;
	//testtest
	SDCardValid=0;
	
//	/* 默认显示心电波形  */
//	DisplayECGWave=1;
//	Pointer=2;

//	/* 默认显示呼吸波形  */
//	DisplayBRWave=1;
//	Pointer=3;
	
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
//  MX_I2C1_Init();
//  MX_RTC_Init();
  MX_SDIO_SD_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_FATFS_Init();

  /* USER CODE BEGIN 2 */
	
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12, GPIO_PIN_RESET);//控制开机电路的引脚
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11, GPIO_PIN_RESET);//控制震动马达的引脚
	
	// SHT75 IO口初始化
	SHT75_IOInit();
	SHT75_Connection_Reset();
	
	 /* MS5611IO口初始化 */
//	 IIC_GPIO_Config(); 
//	 MS561101BA_Init(); //MS561101BA初始化
//	
	/******串口测试 *****/
	printf("usart1 printf test!\r\n");
	
	/******SD卡测试 *****/
  if(SDCardValid==1)
	  while(Write_StartData_To_SDCard()!=0){};//SD卡写入开机时间
//	for(iii=0;iii<2048;iii++)
//	{
//	  sprintf(SDCard_Buf_1+iii*3,"%02X ",iii);
//	}
//	Write_Data_To_SDCard(SDCard_Buf_1);	
	
	/******OLED测试 *****/
	OLED_GPIO_configuration();
  OLED_Init();//OLED初始化
  OLED_Clear();
	
	OLED_DisplayChar_BGC(11,37,'S',Green,Black);//S
	OLED_DisplayChar_BGC(11,51,'T',Green,Black);//T
	OLED_DisplayChar_BGC(11,65,'A',Green,Black);//A
	OLED_DisplayChar_BGC(11,79,'R',Green,Black);//R
	OLED_DisplayChar_BGC(11,93,'T',Green,Black);//T
	
	HAL_Delay(2000);
			
	OLED_Clear();

//	OLED_FillBlock(0,239,0,319,Red);
//	HAL_Delay(500);
//	OLED_Clear();
		
//	OLED_FillBlock(0,239,0,319,Green);
//	HAL_Delay(500);
//	OLED_Clear();
//		
//	OLED_FillBlock(0,239,0,319,Blue);
//	HAL_Delay(500);
//	OLED_Clear();  
 
  /******显示9宫格 *****/
  Speed_Dial();
	
	Driver_MOTOR();
	
	DisplayStatus(Green,Black,1);
	
  printf("OLED test!\r\n");

	/******打开TIM_2中断 *****/
  HAL_TIM_Base_Start_IT(&htim2);
	
//	Driver_Motor();//振动马达震动
	
	/******打开USART1接收中断 *****/	 		 
	HAL_UART_Receive_IT(&huart1,rx_buffer,1);
	
	/******开启ADC的DMA转换 *****/
	/*-3- Start the conversion process and enable interrupt ###########[size=16]#*/
  if(HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&uhADCxConvertedValue, 2) != HAL_OK)
  {
    /*Start Conversation Error*/
		printf("Start ADC Conversation Error!\r\n");
		Error_Handler();
  }
	/******获取CO浓度 *****/
	GET_CO_Concentration();
	
	/******获取环境温湿度 *****/
	GET_SHT75();
	
	/******获取CO浓度 *****/
	GET_CO_Concentration();
	
	/******获取环境温湿度 *****/
	GET_SHT75();
	
	/******获取环境气压 *****/
//	GET_Pressure();
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */
		
  /* USER CODE BEGIN 3 */
		
	/* 检查SD卡缓存区是否已满，如果满的话则写入SD卡 */
//	if(SDCardValid==1)
//	  SDCard_Buffer_Handler(); 
//		 /* MS5611测试代码 */
//		 IIC_GPIO_Config(); 
//		 MS561101BA_Init(); //MS561101BA初始化
//		 Exchange_Number();
		
//		/******获取CO浓度 *****/
//	  GET_CO_Concentration();
//	
//	  /******获取环境温湿度 *****/
//	  GET_SHT75();
		
//	  /******获取环境气压 *****/
//	  GET_Pressure();
  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */
PUTCHAR_PROTOTYPE
{
    HAL_UART_Transmit(&huart1 , (uint8_t *)&ch, 1, 0xFFFF);
    return ch;
}

void DisplayHR(uint16_t color,uint16_t backgroundcolor)
{
	OLED_DisplayChar_BGC(11,37,'H',color,backgroundcolor);//H
  OLED_DisplayChar_BGC(11,51,'R',color,backgroundcolor);//1

  OLED_DisplayChar_BGC(45,23,'0'+Heart_Rate_BW,color,backgroundcolor);//2
  OLED_DisplayChar_BGC(45,37,'0'+Heart_Rate_SW,color,backgroundcolor);//1
  OLED_DisplayChar_BGC(45,51,'0'+Heart_Rate_GW,color,backgroundcolor);//1
}

void DisplayBR(uint16_t color,uint16_t backgroundcolor)
{
  uint8_t Breath_Rate_SW=0;
	uint8_t Breath_Rate_GW=0;
	
	Breath_Rate_SW=Breath_Rate%100/10;
  Breath_Rate_GW=Breath_Rate%100%10;
	
	OLED_DisplayChar_BGC(11,143,'B',color,backgroundcolor);//H
  OLED_DisplayChar_BGC(11,157,'R',color,backgroundcolor);//1

  OLED_DisplayChar_BGC(45,143,'0'+Breath_Rate_SW,color,backgroundcolor);//2
  OLED_DisplayChar_BGC(45,157,'0'+Breath_Rate_GW,color,backgroundcolor);//1

}
void DisplayStatus(uint16_t color,uint16_t backgroundcolor,uint8_t status)
{
  switch(status)
	{
		case 1:   OLED_DisplayChar_BGC(11,249,'A',color,backgroundcolor);//H
              OLED_DisplayChar_BGC(11,263,'B',color,backgroundcolor);//1;
		          break; //心电波形数据
		case 2 :  OLED_DisplayChar_BGC(11,249,'H',color,backgroundcolor);//H
              OLED_DisplayChar_BGC(11,263,'R',color,backgroundcolor);//1;
		          break; //心电波形数据 
		case 3 :  OLED_DisplayChar_BGC(11,249,'B',color,backgroundcolor);//H
              OLED_DisplayChar_BGC(11,263,'R',color,backgroundcolor);//1;
		          break; //心电波形数据
		default:  break;
	}	

//  OLED_DisplayChar_BGC(45,242,'0',color,backgroundcolor);//2
//  OLED_DisplayChar_BGC(45,256,'0',color,backgroundcolor);//1
//	OLED_DisplayChar_BGC(45,270,'0',color,backgroundcolor);//2
//  OLED_DisplayChar_BGC(45,284,'0',color,backgroundcolor);//1

}
void DisplayBT(uint16_t color,uint16_t backgroundcolor)
{
  uint8_t BT_BW=0;
	uint8_t BT_SW=0;
	uint8_t BT_GW=0;
	
	BT_BW=BodyTemperature/100;
	BT_SW=BodyTemperature%100/10;
	BT_GW=BodyTemperature%100%10;
	
	OLED_DisplayChar_BGC(91,143,'B',color,backgroundcolor);//H
  OLED_DisplayChar_BGC(91,157,'T',color,backgroundcolor);//1

  OLED_DisplayChar_BGC(125,126,'0'+BT_BW,color,backgroundcolor);//2
  OLED_DisplayChar_BGC(125,140,'0'+BT_SW,color,backgroundcolor);//1
	OLED_DisplayChar_BGC(125,154,'.',color,backgroundcolor);//2
  OLED_DisplayChar_BGC(125,168,'0'+BT_GW,color,backgroundcolor);//1


}

void DisplayAT(uint16_t color,uint16_t backgroundcolor)
{
  uint8_t AT_BW=0;
	uint8_t AT_SW=0;
	uint8_t AT_GW=0;
	
	AT_BW=TEM/100;
	AT_SW=TEM%100/10;
  AT_GW=TEM%100%10;
	
	
	OLED_DisplayChar_BGC(91,249,'A',color,backgroundcolor);//H
  OLED_DisplayChar_BGC(91,263,'T',color,backgroundcolor);//1

  OLED_DisplayChar_BGC(125,242,'0'+AT_BW,color,backgroundcolor);//2
  OLED_DisplayChar_BGC(125,256,'0'+AT_SW,color,backgroundcolor);//1
	OLED_DisplayChar_BGC(125,270,'.',color,backgroundcolor);//2
  OLED_DisplayChar_BGC(125,284,'0'+AT_GW,color,backgroundcolor);//1

}

void DisplayAH(uint16_t color,uint16_t backgroundcolor)
{
  uint8_t AH_BW=0;
	uint8_t AH_SW=0;
	uint8_t AH_GW=0;
	
	AH_BW=HUI/100;
	AH_SW=HUI%100/10;
  AH_GW=HUI%100%10;
	
	OLED_DisplayChar_BGC(171,37,'A',color,backgroundcolor);//H
  OLED_DisplayChar_BGC(171,51,'H',color,backgroundcolor);//1

  OLED_DisplayChar_BGC(205,32,'0'+AH_BW,color,backgroundcolor);//2
  OLED_DisplayChar_BGC(205,46,'0'+AH_SW,color,backgroundcolor);//1
  OLED_DisplayChar_BGC(205,60,'%',color,backgroundcolor);//1
}

void DisplayAP(uint16_t color,uint16_t backgroundcolor)
{
  uint8_t AH_QW=0;
	uint8_t AH_BW=0;
	uint8_t AH_SW=0;
	uint8_t AH_GW=0;
	
	uint16_t AH_hPa=0;
	
	AH_hPa=ex_Pressure/100;//将Pa转换成hPa，1hPa=100Pa
	
//	AH_hPa=1234;
	
	AH_QW=AH_hPa/1000;
	AH_BW=AH_hPa%1000/100;
	AH_SW=AH_hPa%100/10;
  AH_GW=AH_hPa%1000%10;
	
	OLED_DisplayChar_BGC(171,143,'A',color,backgroundcolor);//H
  OLED_DisplayChar_BGC(171,157,'P',color,backgroundcolor);//1

  OLED_DisplayChar_BGC(205,126,'0'+AH_QW,color,backgroundcolor);//2
  OLED_DisplayChar_BGC(205,140,'0'+AH_BW,color,backgroundcolor);//1
	OLED_DisplayChar_BGC(205,154,'0'+AH_SW,color,backgroundcolor);//2
  OLED_DisplayChar_BGC(205,168,'0'+AH_GW,color,backgroundcolor);//1
}

void DisplayCO(uint16_t color,uint16_t backgroundcolor)
{
  uint8_t CO_BW=0;
	uint8_t CO_SW=0;
	uint8_t CO_GW=0;
	uint16_t data=0;
	
	data=CO;
	
	CO_BW=data/100;
	CO_SW=data/10%10;
  CO_GW=data%10;
	
	OLED_DisplayChar_BGC(171,249,'C',color,backgroundcolor);//H
  OLED_DisplayChar_BGC(171,263,'O',color,backgroundcolor);//1

  OLED_DisplayChar_BGC(205,242,'0'+CO_BW,color,backgroundcolor);//2
  OLED_DisplayChar_BGC(205,256,'0'+CO_SW,color,backgroundcolor);//1
	OLED_DisplayChar_BGC(205,270,'0'+CO_GW,color,backgroundcolor);//2
  OLED_DisplayChar_BGC(205,284,'P',color,backgroundcolor);//1

}

void DisplayACC(uint16_t color,uint16_t backgroundcolor)
{
  uint8_t ACC_QW=0;
	uint8_t ACC_BW=0;
	uint8_t ACC_SW=0;
	uint8_t ACC_GW=0;
	
	ACC_QW=SUM_ACCEL/1000;
	ACC_BW=SUM_ACCEL/100%10;
	ACC_SW=SUM_ACCEL/10%10;
	ACC_GW=SUM_ACCEL%10;
	
	OLED_DisplayChar_BGC(91,32,'A',color,backgroundcolor);//H
  OLED_DisplayChar_BGC(91,46,'C',color,backgroundcolor);//1
	OLED_DisplayChar_BGC(91,60,'C',color,backgroundcolor);//1

  OLED_DisplayChar_BGC(125,26,'0'+ACC_QW,color,backgroundcolor);//2
  OLED_DisplayChar_BGC(125,40,'0'+ACC_BW,color,backgroundcolor);//1
	OLED_DisplayChar_BGC(125,54,'0'+ACC_SW,color,backgroundcolor);//1
	OLED_DisplayChar_BGC(125,70,'0'+ACC_GW,color,backgroundcolor);//1
	
}
void HRWarning(void)
{
  OLED_FillBlock(0,78,0,104,Red);
	DisplayHR(Blue,Red);
	Driver_Motor();//振动马达震动
}

void BRWarning(void)
{
  OLED_FillBlock(0,78,106,210,Red);
	DisplayBR(Blue,Red);
	Driver_Motor();//振动马达震动
}

void BTWarning(void)
{
  OLED_FillBlock(80,158,106,210,Red);
	DisplayBT(Blue,Red);
	Driver_Motor();//振动马达震动
}

void ATWarning(void)
{
  OLED_FillBlock(80,158,212,318,Red);
	DisplayAT(Blue,Red);
	Driver_Motor();//振动马达震动
}

void AHWarning(void)
{
  OLED_FillBlock(160,238,0,104,Red);
	DisplayAH(Blue,Red);
	Driver_Motor();//振动马达震动
}

void APWarning(void)
{
  OLED_FillBlock(160,238,106,210,Red);
	DisplayAP(Blue,Red);
	Driver_Motor();//振动马达震动
}

void COWarning(void)
{
  OLED_FillBlock(160,238,212,318,Red);
	DisplayCO(Blue,Red);
	Driver_Motor();//振动马达震动
}


void Speed_Dial(void)//9宫格显示生理参数
{
  OLED_Clear();
	HAL_Delay(30);
	
	OLED_DrawVerticalLine(0,319,79,White);//画分割线
	
	OLED_DrawVerticalLine(0,319,159,White);//画分割线
	
	OLED_DrawLevelLine(0,239,105,White);//画分割线
	
	OLED_DrawLevelLine(0,239,211,White);//画分割线
	
	/* 显示心率 */	
	DisplayHR(Green,Black);
	DisplayBR(Green,Black);
	DisplayACC(Green,Black);
	DisplayBT(Green,Black);
	DisplayAT(Green,Black);
	DisplayAH(Green,Black);
	DisplayAP(Green,Black);
	DisplayCO(Green,Black);
	
	
}

void DisplaySelected(void)//突出显示选定的模块
{
  switch(Pointer)
	{
		case 1 : DisplayBR(Green,Black);DisplayHR(Green,Black); ;break;
		case 2 : DisplayHR(Blue,White);DisplayBR(Green,Black);  ;break;
		case 3 : DisplayBR(Blue,White); DisplayHR(Green,Black); ;break;
		default:  ;break;
		
	}
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
