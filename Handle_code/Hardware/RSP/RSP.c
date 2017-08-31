#include "stm32f4xx.h"	 
#include "stdlib.h"	
#include "oled.h"
#include "RSP.h"
#include "SDCard.h"
#include "ECG.h"

uint16_t RSP_Data_Temp;
uint8_t  Breath_Rate;
float    RSP_Data;
float  RSP_DIV=20;

float last_RSP_Data=0,new_RSP_Data=0;
float RSP_Data_Last;

uint16_t RSP_Data_x=2;

uint16_t TEM1,TEM2,TEM3,TEM4,TEM5;
uint16_t BodyTemperature=0; 

uint16_t SUM_ACCEL=0;

extern uint8_t Pointer;
extern uint8_t DisplayBRWave;//��ʾ�������α�־λ��Ĭ��ֻ��ʾ������ֵ
extern uint8_t DisplayECGWave;//��ʾ�ĵ粨�α�־λ��Ĭ��ֻ��ʾ����ֵ

extern void DisplayBR(uint16_t color,uint16_t backgroundcolor);
extern void DisplayBT(uint16_t color,uint16_t backgroundcolor);
extern void DisplayACC(uint16_t color,uint16_t backgroundcolor);

extern uint8_t SDCardValid;

void Display_RSP_Wave(void);
void DisplayStatus(uint16_t color,uint16_t backgroundcolor,uint8_t status);

/**
* @brief �����������.
*/
void RSP_DATA_Handler(uint8_t *pData,uint8_t Data_Num)
{
	/*************** ���������ݴ���SD��������******************/
	if(SDCardValid==1)
	  Write_Data_To_SDBuffer(pData,Data_Num);
	
	/*************** ����������ת����ԭ��******************/
	RSP_Data_Temp=pData[3]*0x100+pData[4];

	RSP_Data=Complement_To_True(RSP_Data_Temp)/RSP_DIV;
	
	
	
	Breath_Rate=pData[5];
	
	if(DisplayBRWave==1&&Pointer==3)
  {
		Display_RSP_Wave();	//��ʾ��������
	}
  else
  {
	  DisplayBR(Green,Black);
	  DisplayStatus(Green,Black,3);
	}			
		
}

/**
* @brief ������������ʾ��oled��.
*/
void Display_RSP_Wave(void)
{
	RSP_Data+=140;
	
	if(RSP_Data<=39) RSP_Data=40;
	if(RSP_Data>=238) RSP_Data=239;
	
	new_RSP_Data=RSP_Data; 
			
	RSP_Data=(RSP_Data_Last+new_RSP_Data)/2;
    
	OLED_FillBlock(40,239,RSP_Data_x+1,RSP_Data_x+8,0x0000);//
          
	OLED_DisplayChar(0,235,'B',0xffe0);//H
  OLED_DisplayChar(0,249,'R',0xffe0);//1
  OLED_DisplayChar(0,263,':',0xffe0);//:
  OLED_DisplayChar(0,277,Breath_Rate/10+'0',0xffe0);//2
  OLED_DisplayChar(0,291,Breath_Rate%10+'0',0xffe0);//1
	
	RSP_Data_x+=2;
		 
	OLED_DrawLine(RSP_Data_Last,RSP_Data_x-3,RSP_Data,RSP_Data_x,0x07E0);	
  OLED_DrawLine(RSP_Data_Last+1,RSP_Data_x-3,RSP_Data+1,RSP_Data_x,0x07E0);
//	OLED_DrawLine(RSP_Data_Last+2,RSP_Data_x-3,RSP_Data+2,RSP_Data_x,0x07E0);
	RSP_Data_Last=RSP_Data;
		
  if(RSP_Data_x>=316)
  {  
     RSP_Data_x=4;
		 OLED_Clear();
	 // OLED_FillBlock(40,239,0,2,0x0000);
  }
	
}

/**
* @brief ������������.
*/
void TEM_DATA_Handler(uint8_t *pData,uint8_t Data_Num)
{
	/*************** ���������ݴ���SD��������******************/
	if(SDCardValid==1)
	  Write_Data_To_SDBuffer(pData,Data_Num);
	
	/*************** ����������ת����ԭ��******************/
	/*************** ����������ת����ԭ��******************/
	TEM1=(pData[3]*0x100+pData[4])/10; //�¶�������10�� 
  TEM2=(pData[5]*0x100+pData[6])/10; //�¶�������10�� 
  TEM3=(pData[7]*0x100+pData[8])/10; //�¶�������10�� 
  TEM4=(pData[9]*0x100+pData[10])/10; //�¶�������10�� 
  TEM5=(pData[11]*0x100+pData[12])/10; //�¶�������10�� 
	
	BodyTemperature=TEM1;
	
	if((DisplayBRWave==1&&Pointer==2)||(DisplayECGWave==1&&Pointer==3))
  {

	}
  else
  {
	  DisplayBT(Green,Black);
		DisplayStatus(Green,Black,1);
	}			
		
}
/**
* @brief ������ٶ�����.
*/
void ACC_DATA_Handler(uint8_t *pData,uint8_t Data_Num)
{
	/*************** ���������ݴ���SD��������******************/
	if(SDCardValid==1)
	  Write_Data_To_SDBuffer(pData,Data_Num);
	
	
  SUM_ACCEL=pData[3]*100; //���ٶ�����������100���� ��λΪmg
	
	if((DisplayBRWave==1&&Pointer==2)||(DisplayECGWave==1&&Pointer==3))
  {

	}
  else
  {
	  DisplayACC(Green,Black);
		DisplayStatus(Green,Black,1);
	}			
		
}