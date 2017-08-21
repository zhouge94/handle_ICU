#include "stm32f4xx.h"	 
#include "stdlib.h"	
#include "oled.h"
#include "ECG.h"
#include "SDCard.h"

uint16_t ECG1_Data_Temp=0;//	心电数据临时存放
uint16_t ECG2_Data_Temp=0;//	心电数据临时存放
uint16_t ECG3_Data_Temp=0;//	心电数据临时存放

float ECG1_Data=0;//心电数据
float ECG2_Data=0;//心电数据
float ECG3_Data=0;//心电数据

float ECG1_last=0;//心电数据
float ECG2_last=0;//心电数据
float ECG3_last=0;//心电数据

float ECG1_Display=0;//经过处理之后要显示的心电数据
float ECG2_Display=0;//经过处理之后要显示的心电数据
float ECG3_Display=0;//经过处理之后要显示的心电数据

float Last_ECG1_Data=0;//上一次采集的心电数据
float Last_ECG2_Data=0;//上一次采集的心电数据
float Last_ECG3_Data=0;//上一次采集的心电数据

float ECG1_Display_Last=0;//上一次显示过的心电数据
float ECG2_Display_Last=0;//上一次显示过的心电数据
float ECG3_Display_Last=0;//上一次显示过的心电数据

uint8_t ECG_DIV=200;//心电数据缩小的倍数

uint16_t ECG_Data_x=2;//心电数据x轴坐标

uint8_t Heart_Rate=0;//心率
uint8_t Lead_Off=0;//导联脱落标志 Lead_Off=1 表示导联脱落
uint8_t Sample_Rate_Div=2;//显示的时候采样率降低的倍数,此处是2，不能修改，要是修改的话ECG_DATA_Handler里面的ECG1_Data_buf[1]应修改
uint8_t Rcv_Cnt=0;//

float ECG1_Data_buf[6]={0};
float ECG2_Data_buf[6]={0};
float ECG3_Data_buf[6]={0};

float Heart_Rate_BW=0;
uint8_t Heart_Rate_SW=0;
uint8_t Heart_Rate_GW=0;

extern uint8_t Pointer;
extern uint8_t DisplayECGWave;//显示心电波形标志位，默认只显示心率值
extern uint8_t DisplayBRWave;//显示呼吸波形标志位，默认只显示呼吸率值

extern uint8_t rx_buffer[128];//串口DMA接收数据存储区

extern uint8_t SDCardValid;

extern void DisplayHR(uint16_t color,uint16_t backgroundcolor);
void DisplayStatus(uint16_t color,uint16_t backgroundcolor,uint8_t status);

/**
* @brief 处理心电数据.
*/
void ECG_DATA_Handler(uint8_t *pData,uint8_t Data_Num)
{
	 uint8_t iii=0;
	/*************** 将心电数据存入SD卡缓存区******************/
	if(SDCardValid==1)
	  Write_Data_To_SDBuffer(pData,Data_Num);
	
	for(iii=0;iii<5;iii++)
	{
	
	  /*************** 将心电数据转换成原码******************/
    ECG1_Data_Temp=pData[3+(6*iii)]*0x100+pData[4+(6*iii)];
	  ECG1_Data=Complement_To_True(ECG1_Data_Temp)/ECG_DIV;
	
	  ECG2_Data_Temp=pData[5+(6*iii)]*0x100+pData[6+(6*iii)];
	  ECG2_Data=Complement_To_True(ECG2_Data_Temp)/ECG_DIV;
	
	  ECG3_Data_Temp=pData[7+(6*iii)]*0x100+pData[8+(6*iii)];
	  ECG3_Data=Complement_To_True(ECG3_Data_Temp)/ECG_DIV;
	  
		if(Rcv_Cnt==Sample_Rate_Div)
    {
			Rcv_Cnt=0;
			
//			ECG1_Data=((ECG1_Data_buf[0]+ECG1_Data_buf[1]+Last_ECG1_Data)/3);
//		  ECG2_Data=((ECG2_Data_buf[0]+ECG2_Data_buf[1]+Last_ECG2_Data)/3);
//		  ECG3_Data=((ECG3_Data_buf[0]+ECG3_Data_buf[1]+Last_ECG3_Data)/3);
			
			ECG1_Data=((ECG1_Data_buf[0]+Last_ECG1_Data)/2);
		  ECG2_Data=((ECG2_Data_buf[0]+Last_ECG2_Data)/2);
		  ECG3_Data=((ECG3_Data_buf[0]+Last_ECG3_Data)/2);
			
			Last_ECG1_Data=ECG1_Data;
		  Last_ECG2_Data=ECG2_Data;
		  Last_ECG3_Data=ECG3_Data;
		  
			Adjustment_ECG_Data(ECG1_Data,ECG2_Data,ECG3_Data);
			
			if((DisplayBRWave==1&&Pointer==2)||(DisplayECGWave==1&&Pointer==3))
      {
		    Display_ECG_Wave();	//显示心电波形
	    }
      else
      {
		    DisplayHR(Green,Black);
				DisplayStatus(Green,Black,2);
      }
		}
    else
	  {
	    /*************** 将相邻两次采集的数据求平均值作为此次采集的数据******************/
//		  ECG1_Data=(ECG1_Data+ECG1_last)/2;
//		  ECG2_Data=(ECG2_Data+ECG2_last)/2;
//		  ECG3_Data=(ECG3_Data+ECG3_last)/2;
		
		  ECG1_Data_buf[Rcv_Cnt]   = ECG1_Data;
		  ECG2_Data_buf[Rcv_Cnt]   = ECG2_Data;
		  ECG3_Data_buf[Rcv_Cnt++] = ECG3_Data;
		
//		ECG1_last=ECG1_Data;
//		ECG2_last=ECG2_Data;
//		ECG3_last=ECG3_Data;
	}			
  }	
}

/**
* @brief 将心电图波形显示在oled上.
*/
void Display_ECG_Wave(void)
{
  OLED_FillBlock(24,240,ECG_Data_x+1,ECG_Data_x+4,0x0000);//
		
	OLED_DrawLine(ECG1_Display_Last,ECG_Data_x-1,ECG1_Display,ECG_Data_x,0x07E0);
  OLED_DrawLine(ECG2_Display_Last,ECG_Data_x-1,ECG2_Display,ECG_Data_x,0x07E0);
	OLED_DrawLine(ECG3_Display_Last,ECG_Data_x-1,ECG3_Display,ECG_Data_x,0x07E0);
  
	/*************** 显示心电的基线******************/
//	OLED_DrawVerticalLine(ECG_Data_x-1,ECG_Data_x,74,White);
//  OLED_DrawVerticalLine(ECG_Data_x-1,ECG_Data_x,146,White);
//  OLED_DrawVerticalLine(ECG_Data_x-1,ECG_Data_x,218,White);

	ECG1_Display_Last=ECG1_Display;
	ECG2_Display_Last=ECG2_Display;
	ECG3_Display_Last=ECG3_Display;
		
	ECG_Data_x++;
  if(ECG_Data_x>=319)
  {  
     ECG_Data_x=1;
		 OLED_FillBlock(24,240,0,2,0x0000);
  }
}

/**
* @brief 处理心率数值以及导联脱落标志位.
*/
void HR_DATA_Handler(uint8_t *pData,uint8_t Data_Num)
{
  uint16_t Heart_Rate_Temp=0;
	uint8_t  ii=0;
	
	/*************** 将心率数据存入SD卡缓存区******************/
	if(SDCardValid==1)
	  Write_Data_To_SDBuffer(pData,Data_Num);
	
	/*************** 处理心率数据******************/
	if(pData[3]==0xC8) 
	{
	  Heart_Rate_Temp+=pData[4];
		ii++;
	} 
  if(pData[5]==0xC8) 
	{
	  Heart_Rate_Temp+=pData[6];
		ii++;
	}  
  
  if(pData[7]==0xC8) 
	{
	  Heart_Rate_Temp+=pData[8];
		ii++;
	}  
  Heart_Rate=	Heart_Rate_Temp/3;
	
	
	
	if(DisplayECGWave==1&&Pointer==2)
	{
		Display_Heart_Rate();
	}
  else
  {
		Heart_Rate_BW=Heart_Rate/100;
    Heart_Rate_SW=Heart_Rate%100/10;
    Heart_Rate_GW=Heart_Rate%100%10;
			  
    if(Heart_Rate_BW==0)
	  {				
	    Heart_Rate_BW=-16;
	  }
	
		DisplayHR(Green,Black);//显示心率
	  DisplayStatus(Green,Black,2);
  }			
}
/**
* @brief 在oled上显示心率数值.
*/
void Display_Heart_Rate(void)
{
	
  Heart_Rate_BW=Heart_Rate/100;
  Heart_Rate_SW=Heart_Rate%100/10;
  Heart_Rate_GW=Heart_Rate%100%10;
	
  if(Heart_Rate_BW==0)
	{				
	  OLED_DisplayChar(0,235,'H',0xffe0);//H
    OLED_DisplayChar(0,249,'R',0xffe0);//1
    OLED_DisplayChar(0,263,':',0xffe0);//:
    OLED_DisplayChar(0,277,'0'+Heart_Rate_SW,0xffe0);//2
    OLED_DisplayChar(0,291,'0'+Heart_Rate_GW,0xffe0);//1
    OLED_DisplayChar(0,305,' ',0xffe0);//1
	}
  else
	{
	  OLED_DisplayChar(0,235,'H',0xffe0);//H
    OLED_DisplayChar(0,249,'R',0xffe0);//1
    OLED_DisplayChar(0,263,':',0xffe0);//:
    OLED_DisplayChar(0,277,'0'+Heart_Rate_BW,0xffe0);//2
    OLED_DisplayChar(0,291,'0'+Heart_Rate_SW,0xffe0);//1
    OLED_DisplayChar(0,305,'0'+Heart_Rate_GW,0xffe0);//1
	}	

} 

/**
* @brief 将心电数值调整到适合显示的范围.
*/
void Adjustment_ECG_Data(float Data_1,float Data_2,float Data_3)
{
  Data_1+=74;
	Data_2+=146;
	Data_3+=218;
	
//	Data_1=74-Data_1;
//	Data_2=146-Data_2;
//	Data_3=218-Data_3;
	
	if(Data_1<=54)   Data_1-=20;
	if(Data_1<=24)   Data_1=24;
	if(Data_1>=96)   Data_1=96;
	
	if(Data_2<=126)  Data_2-=20;
	if(Data_2<=96)   Data_2=96;
	if(Data_2>=168)  Data_2=168;
	
	if(Data_3<=198)  Data_3-=20;
	if(Data_3<=168)  Data_3=168;
	if(Data_3>=239)  Data_3=239;
	
	ECG1_Display=Data_1;
	ECG2_Display=Data_2;
	ECG3_Display=Data_3;
	  
}
/**
* @brief 将补码编程原码.
*/
float Complement_To_True(uint16_t Data)
{
  float True_form=0;
	if(Data>32767)//数据为负的
	{
	  True_form=Data-65536;
	}	
	else
	{
	  True_form=Data;
	}
	return True_form;
}

