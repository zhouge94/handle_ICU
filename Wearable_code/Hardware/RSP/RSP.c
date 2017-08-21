#include "stm32f4xx.h"	 
#include "stdlib.h"	
#include "oled.h"
#include "RSP.h"
#include "SDCard.h"
#include "ECG.h"

uint16_t RSP_Data_Temp;
uint8_t  Breath_Rate;
float    RSP_Data;
uint8_t  RSP_DIV=1;

float last_RSP_Data=0,Current_RSP_Data=0,new_RSP_Data=0;
float RSP_Data_Last;

uint16_t RSP_Data_x=2;

extern uint8_t Pointer;
extern uint8_t DisplayBRWave;//显示呼吸波形标志位，默认只显示呼吸率值

extern void DisplayBR(uint16_t color,uint16_t backgroundcolor);

void Display_RSP_Wave(void);
/**
* @brief 处理呼吸数据.
*/
void RSP_DATA_Handler(uint8_t *pData,uint8_t Data_Num)
{
	/*************** 将呼吸数据存入SD卡缓存区******************/
	Write_Data_To_SDBuffer(pData,Data_Num);
	
	/*************** 将呼吸数据转换成原码******************/
	RSP_Data_Temp=pData[3]*0x100+pData[4];

	if(RSP_Data_Temp>32767)//数据为负的
	{
	  RSP_Data=RSP_Data_Temp-65536;
	}	
	else
	{
	  RSP_Data=RSP_Data_Temp;
	}
	Breath_Rate=pData[5];
	
	if(DisplayBRWave==1&&Pointer==3)
  {
		Display_RSP_Wave();	//显示心电波形
	}
  else
  {
	  DisplayBR(Green,Black);
	}			
		
}

/**
* @brief 将呼吸波形显示在oled上.
*/
void Display_RSP_Wave(void)
{
	RSP_Data+=140;
	if(RSP_Data<=40) RSP_Data=40;
	if(RSP_Data>=239) RSP_Data=40;
	
	new_RSP_Data=RSP_Data; 
			
	Current_RSP_Data=(RSP_Data_Last+new_RSP_Data)/2;
			 
	OLED_FillBlock(40,239,RSP_Data_x+1,RSP_Data_x+8,0x0000);//
          
			
	RSP_Data_x+=2;
		 
	OLED_DrawLine(RSP_Data_Last,RSP_Data_x-2,Current_RSP_Data,RSP_Data_x,0x07E0);		

	RSP_Data_Last=Current_RSP_Data;
		
  if(RSP_Data_x>=318)
  {  
     RSP_Data_x=2;
		 OLED_Clear();
	 // OLED_FillBlock(40,239,0,2,0x0000);
  }
	
}
