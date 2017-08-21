#include "stm32f4xx.h"	 
#include "stdlib.h"	
#include "oled.h"
#include "ECG.h"
#include "SDCard.h"

uint16_t ECG1_Data_Temp=0;//	�ĵ�������ʱ���
uint16_t ECG2_Data_Temp=0;//	�ĵ�������ʱ���
uint16_t ECG3_Data_Temp=0;//	�ĵ�������ʱ���

float ECG1_Data=0;//�ĵ�����
float ECG2_Data=0;//�ĵ�����
float ECG3_Data=0;//�ĵ�����

float ECG1_last=0;//�ĵ�����
float ECG2_last=0;//�ĵ�����
float ECG3_last=0;//�ĵ�����

float ECG1_Display=0;//��������֮��Ҫ��ʾ���ĵ�����
float ECG2_Display=0;//��������֮��Ҫ��ʾ���ĵ�����
float ECG3_Display=0;//��������֮��Ҫ��ʾ���ĵ�����

float Last_ECG1_Data=0;//��һ�βɼ����ĵ�����
float Last_ECG2_Data=0;//��һ�βɼ����ĵ�����
float Last_ECG3_Data=0;//��һ�βɼ����ĵ�����

float ECG1_Display_Last=0;//��һ����ʾ�����ĵ�����
float ECG2_Display_Last=0;//��һ����ʾ�����ĵ�����
float ECG3_Display_Last=0;//��һ����ʾ�����ĵ�����

uint8_t ECG_DIV=200;//�ĵ�������С�ı���

uint16_t ECG_Data_x=2;//�ĵ�����x������

uint8_t Heart_Rate=0;//����
uint8_t Lead_Off=0;//���������־ Lead_Off=1 ��ʾ��������
uint8_t Sample_Rate_Div=2;//��ʾ��ʱ������ʽ��͵ı���,�˴���2�������޸ģ�Ҫ���޸ĵĻ�ECG_DATA_Handler�����ECG1_Data_buf[1]Ӧ�޸�
uint8_t Rcv_Cnt=0;//

float ECG1_Data_buf[6]={0};
float ECG2_Data_buf[6]={0};
float ECG3_Data_buf[6]={0};

float Heart_Rate_BW=0;
uint8_t Heart_Rate_SW=0;
uint8_t Heart_Rate_GW=0;

extern uint8_t Pointer;
extern uint8_t DisplayECGWave;//��ʾ�ĵ粨�α�־λ��Ĭ��ֻ��ʾ����ֵ
extern uint8_t DisplayBRWave;//��ʾ�������α�־λ��Ĭ��ֻ��ʾ������ֵ

extern uint8_t rx_buffer[128];//����DMA�������ݴ洢��

extern uint8_t SDCardValid;

extern void DisplayHR(uint16_t color,uint16_t backgroundcolor);
void DisplayStatus(uint16_t color,uint16_t backgroundcolor,uint8_t status);

/**
* @brief �����ĵ�����.
*/
void ECG_DATA_Handler(uint8_t *pData,uint8_t Data_Num)
{
	 uint8_t iii=0;
	/*************** ���ĵ����ݴ���SD��������******************/
	if(SDCardValid==1)
	  Write_Data_To_SDBuffer(pData,Data_Num);
	
	for(iii=0;iii<5;iii++)
	{
	
	  /*************** ���ĵ�����ת����ԭ��******************/
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
		    Display_ECG_Wave();	//��ʾ�ĵ粨��
	    }
      else
      {
		    DisplayHR(Green,Black);
				DisplayStatus(Green,Black,2);
      }
		}
    else
	  {
	    /*************** ���������βɼ���������ƽ��ֵ��Ϊ�˴βɼ�������******************/
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
* @brief ���ĵ�ͼ������ʾ��oled��.
*/
void Display_ECG_Wave(void)
{
  OLED_FillBlock(24,240,ECG_Data_x+1,ECG_Data_x+4,0x0000);//
		
	OLED_DrawLine(ECG1_Display_Last,ECG_Data_x-1,ECG1_Display,ECG_Data_x,0x07E0);
  OLED_DrawLine(ECG2_Display_Last,ECG_Data_x-1,ECG2_Display,ECG_Data_x,0x07E0);
	OLED_DrawLine(ECG3_Display_Last,ECG_Data_x-1,ECG3_Display,ECG_Data_x,0x07E0);
  
	/*************** ��ʾ�ĵ�Ļ���******************/
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
* @brief ����������ֵ�Լ����������־λ.
*/
void HR_DATA_Handler(uint8_t *pData,uint8_t Data_Num)
{
  uint16_t Heart_Rate_Temp=0;
	uint8_t  ii=0;
	
	/*************** ���������ݴ���SD��������******************/
	if(SDCardValid==1)
	  Write_Data_To_SDBuffer(pData,Data_Num);
	
	/*************** ������������******************/
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
	
		DisplayHR(Green,Black);//��ʾ����
	  DisplayStatus(Green,Black,2);
  }			
}
/**
* @brief ��oled����ʾ������ֵ.
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
* @brief ���ĵ���ֵ�������ʺ���ʾ�ķ�Χ.
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
* @brief ��������ԭ��.
*/
float Complement_To_True(uint16_t Data)
{
  float True_form=0;
	if(Data>32767)//����Ϊ����
	{
	  True_form=Data-65536;
	}	
	else
	{
	  True_form=Data;
	}
	return True_form;
}

