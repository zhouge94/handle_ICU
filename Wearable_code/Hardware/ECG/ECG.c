#include "stm32f4xx.h"	 
#include "stdlib.h"	
#include "ECG.h"
#include  <math.h>    //Keil library  //==========================================
#include "math.h"

extern UART_HandleTypeDef huart2;

uint8_t FuHao_1=0;//心电数据符号标志位，0表示正数，1表示负数
uint8_t LEAD_OFF_1=0;//导联脱落标志位，0表示未脱落，1表示脱落
uint8_t ECG1_Data_H=0; // 心电数据高八位
uint8_t ECG1_Data_L=0; // 心电数据低八位
uint16_t ECG1_Data_Temp=0;//	心电数据临时存放	
uint16_t ECG1_Data=0;//	最终要显示的心电数据
uint16_t Heart_Rate_1=0;//	心率数据

uint8_t FuHao_2=0;//心电数据符号标志位，0表示正数，2表示负数
uint8_t LEAD_OFF_2=0;//导联脱落标志位，0表示未脱落，2表示脱落
uint8_t ECG2_Data_H=0; // 心电数据高八位
uint8_t ECG2_Data_L=0; // 心电数据低八位
uint16_t ECG2_Data_Temp=0;//	心电数据临时存放	
uint16_t ECG2_Data=0;//	最终要显示的心电数据
uint16_t Heart_Rate_2=0;//	心率数据

uint8_t FuHao_3=0;//心电数据符号标志位，0表示正数，2表示负数
uint8_t LEAD_OFF_3=0;//导联脱落标志位，0表示未脱落，2表示脱落
uint8_t ECG3_Data_H=0; // 心电数据高八位
uint8_t ECG3_Data_L=0; // 心电数据低八位
uint16_t ECG3_Data_Temp=0;//	心电数据临时存放	
uint16_t ECG3_Data=0;//	最终要显示的心电数据
uint16_t Heart_Rate_3=0;//	心率数据

uint8_t ECG1_DATA_Ready=0;//第n路心电数据准备发送标志位
uint8_t ECG2_DATA_Ready=0;
uint8_t ECG3_DATA_Ready=0;

uint8_t ECG1_DATA_cnt=0;
uint8_t ECG2_DATA_cnt=0;
uint8_t ECG3_DATA_cnt=0;

uint8_t HR1_DATA_Ready=0;//第n路心率数据准备发送标志位
uint8_t HR2_DATA_Ready=0;
uint8_t HR3_DATA_Ready=0;

uint16_t ECG1_DATA_BUFFER_CUR[3]={0};//第n路心电当前数据存放
uint16_t ECG2_DATA_BUFFER_CUR[3]={0};
uint16_t ECG3_DATA_BUFFER_CUR[3]={0};

uint16_t ECG1_DATA_BUFFER_PREV[3]={0};//第n路心电上一组数据存放
uint16_t ECG2_DATA_BUFFER_PREV[3]={0};
uint16_t ECG3_DATA_BUFFER_PREV[3]={0};

uint8_t THREE_LEAD_ECG[34]={0XAA,0XAA,0X06,0X00,0X00,0X00,0X00,0X00,0X00,
	                                         0X00,0X00,0X00,0X00,0X00,0X00,
	                                         0X00,0X00,0X00,0X00,0X00,0X00,
	                                         0X00,0X00,0X00,0X00,0X00,0X00,
	                                         0X00,0X00,0X00,0X00,0X00,0X00,
                                           0X00};

uint8_t THREE_HR_DATA[30]={0XAA,0XAA,0X12,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	                         0XAA,0XAA,0X12,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
                           0XAA,0XAA,0X12,0X00,0X00,0X00,0X00,0X00,0X00,0X00};

													 
uint8_t PP_Data[6]={0XAA,0XAA,0X10,0X00,0X00,0X00};

uint8_t HR_BUFFER[256]={0};//实时心率数据缓存
uint8_t HR_CNT;//心率数据个数


 float X1,X2,X3,X4,X5,X6,X7,Y;
 float mi;//为第i个R-R间期的值
 float di;//
 float ki;//
 float ni;//
 float p;//
 uint16_t  ii;
 float RR_SUM=0;
 float di_SUM=0;
 float ki_SUM=0;
 float C=0;
 float f=0;	
 float HeartRate_BUFFER[256]={0};//实时心率数据缓存
 float di_BUFFER[256]={0};//数据缓存
 float ki_BUFFER[256]={0};//数据缓存
 float ni_BUFFER[256]={0};//数据缓存
 uint16_t h=0;
 uint16_t  PP;
 
 
extern uint8_t rx_buffer[128];
extern uint8_t rx_buffer_2[128];
extern uint8_t rx_buffer_6[128];

/**
  * @brief  第一路心电处理子程序
  */
void ECG1_Data_ReciveHandle(uint8_t *pData)
{
	  /* 判断是心电数据还是，心率数据 */
	  if(pData[0]==0XAA&&pData[1]==0XAA&&pData[2]==0X04)
		{
		  ECG1_DATA_cnt++;
      switch(ECG1_DATA_cnt)
			{
			  case 1 :   THREE_LEAD_ECG[3]=pData[5];THREE_LEAD_ECG[4]=pData[6];  break;
				case 2 :   THREE_LEAD_ECG[9]=pData[5];THREE_LEAD_ECG[10]=pData[6];  break;
				case 3 :   THREE_LEAD_ECG[15]=pData[5];THREE_LEAD_ECG[16]=pData[6];  break;
				case 4 :   THREE_LEAD_ECG[21]=pData[5];THREE_LEAD_ECG[22]=pData[6];  break;
				case 5 :   THREE_LEAD_ECG[27]=pData[5];THREE_LEAD_ECG[28]=pData[6];
				           ECG1_DATA_cnt=0;  ECG1_DATA_Ready=1;                    break;
			  default:   break;
			}				
				
				
		}
}

/**
  * @brief  第二路心电处理子程序
  * @param  huart: pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be received
  * @note   When the UART parity is enabled (PCE = 1) the data received contain the parity bit.
  * @retval HAL status
  */
void ECG2_Data_ReciveHandle(uint8_t *pData)
{
     
	  /* 判断是心电数据还是，心率数据 */
	  if(pData[0]==0XAA&&pData[1]==0XAA&&pData[2]==0X04)
		{
		  ECG2_DATA_cnt++;
      switch(ECG2_DATA_cnt)
			{
			  case 1 :   THREE_LEAD_ECG[5]=pData[5];THREE_LEAD_ECG[6]=pData[6];  break;
				case 2 :   THREE_LEAD_ECG[11]=pData[5];THREE_LEAD_ECG[12]=pData[6];  break;
				case 3 :   THREE_LEAD_ECG[17]=pData[5];THREE_LEAD_ECG[18]=pData[6];  break;
				case 4 :   THREE_LEAD_ECG[23]=pData[5];THREE_LEAD_ECG[24]=pData[6];  break;
				case 5 :   THREE_LEAD_ECG[29]=pData[5];THREE_LEAD_ECG[30]=pData[6];
				           ECG2_DATA_cnt=0;  ECG2_DATA_Ready=1;                    break;
			  default:   break;
			}				
				
		}

}
void ECG3_Data_ReciveHandle(uint8_t *pData)
{
	  uint8_t i;
	  static uint16_t testcnt=0;
	
	  /* 判断是心电数据还是，心率数据 */
	  if(pData[0]==0XAA&&pData[1]==0XAA&&pData[2]==0X04)
		{
			ECG3_DATA_cnt++;
      switch(ECG3_DATA_cnt)
			{
			  case 1 :   THREE_LEAD_ECG[7]=pData[5];THREE_LEAD_ECG[8]=pData[6];  break;
				case 2 :   THREE_LEAD_ECG[13]=pData[5];THREE_LEAD_ECG[14]=pData[6];  break;
				case 3 :   THREE_LEAD_ECG[19]=pData[5];THREE_LEAD_ECG[20]=pData[6];  break;
				case 4 :   THREE_LEAD_ECG[25]=pData[5];THREE_LEAD_ECG[26]=pData[6];  break;
				case 5 :   THREE_LEAD_ECG[31]=pData[5];THREE_LEAD_ECG[32]=pData[6];
				           ECG3_DATA_cnt=0;  ECG3_DATA_Ready=1;THREE_LEAD_ECG[27]=00;  break;
			  default:   break;
			}	
		}			
}

void DMA_Send_ECG_DATA(void)
{
    HAL_UART_Transmit_DMA(&huart2 , THREE_LEAD_ECG, 34);	
}

void DMA_Send_HR_DATA(void)
{
      THREE_HR_DATA[3]=rx_buffer[12];//第一路心电数据导联脱落标志位
	    THREE_HR_DATA[4]=rx_buffer[14];//第一路心电数据心率值
	    
//	    THREE_HR_DATA[13]=rx_buffer[12];//第一路心电数据导联脱落标志位
//	    THREE_HR_DATA[14]=rx_buffer[14];//第一路心电数据心率值
//	    
//	    THREE_HR_DATA[23]=rx_buffer[12];//第一路心电数据导联脱落标志位
//	    THREE_HR_DATA[24]=rx_buffer[14];//第一路心电数据心率值
	  
	
	    THREE_HR_DATA[5]=rx_buffer_2[12];//第2路心电数据导联脱落标志位
	    THREE_HR_DATA[6]=rx_buffer_2[14];//第2路心电数据心率值
	    
//	    THREE_HR_DATA[15]=rx_buffer_2[12];//第2路心电数据导联脱落标志位
//	    THREE_HR_DATA[16]=rx_buffer_2[14];//第2路心电数据心率值
//	
//	    THREE_HR_DATA[25]=rx_buffer_2[12];//第2路心电数据导联脱落标志位
//	    THREE_HR_DATA[26]=rx_buffer_2[14];//第2路心电数据心率值
	   
	
	    THREE_HR_DATA[7]=rx_buffer_2[12];//第3路心电数据导联脱落标志位
	    THREE_HR_DATA[8]=rx_buffer_2[14];//第3路心电数据心率值
	
//	    THREE_HR_DATA[17]=rx_buffer_2[12];//第3路心电数据导联脱落标志位
//	    THREE_HR_DATA[18]=rx_buffer_2[14];//第3路心电数据心率值
//			
//			THREE_HR_DATA[27]=rx_buffer_2[12];//第3路心电数据导联脱落标志位
//	    THREE_HR_DATA[28]=rx_buffer_2[14];//第3路心电数据心率值
	
	    HAL_UART_Transmit_DMA(&huart2 , THREE_HR_DATA, 10);
}

void get_PsychologicalPressure(void)
{

 
 for(ii=0;ii<HR_CNT;ii++)
 {
   mi=60000/HR_BUFFER[ii];//将心率转换为R-R间期
	 HeartRate_BUFFER[ii]=mi;
	 RR_SUM+=mi;
 }
 C=RR_SUM/HR_CNT;//90s内的R-R间期平均值
 
 RR_SUM=0;
 
 for(ii=0;ii<HR_CNT;ii++)
 {
	 RR_SUM=(HeartRate_BUFFER[ii]-C)*(HeartRate_BUFFER[ii]-C);
 }
 
 X1=sqrt(RR_SUM/(HR_CNT-1));
 RR_SUM=0;
 
 for(ii=0;ii<HR_CNT-1;ii++)
 {
	 di=HeartRate_BUFFER[ii+1]-HeartRate_BUFFER[ii];
	 if (di>50) h++;
	 
	 di_BUFFER[ii]=di;
	 di_SUM+=di;
	 RR_SUM+=di*di;
 }
 X2=sqrt(RR_SUM/(HR_CNT-2));
 
 f=di_SUM/(HR_CNT-2);
 di_SUM=0;
 for(ii=0;ii<HR_CNT-1;ii++)
 {
	 di_SUM+=(di_BUFFER[ii]-f)*(di_BUFFER[ii]-f);
 }
 X3=sqrt(di_SUM/(HR_CNT-2));
 di_SUM=0;
 
 X4=h;
 X5=h/HR_CNT;
 
 for(ii=0;ii<HR_CNT-2;ii++)
 {
   ki=atan2((HeartRate_BUFFER[ii+2]-HeartRate_BUFFER[ii+1]),(HeartRate_BUFFER[ii+1]-HeartRate_BUFFER[ii]));
	 ki_BUFFER[ii]=ki;
	 ki_SUM+=abs(ki-45);
 }
 X6=ki_SUM/(HR_CNT-2);
 ki_SUM=0;
 
 for(ii=0;ii<HR_CNT-1;ii++)
 {
   ki_SUM=(HeartRate_BUFFER[ii+1]-HeartRate_BUFFER[ii])*(HeartRate_BUFFER[ii+1]-HeartRate_BUFFER[ii]);
	 ni=sqrt(ki_SUM);
	 ni_BUFFER[ii]=ni;
 }
 ki_SUM=0;
 
 for(ii=0;ii<HR_CNT-1;ii++)
 {
   ki_SUM+=ni_BUFFER[ii];
 }
 p=ki_SUM/(HR_CNT-1);
 ki_SUM=0;
 
 for(ii=0;ii<HR_CNT-1;ii++)
 {
   ki_SUM+=(ni_BUFFER[ii]-p)*(ni_BUFFER[ii]-p);
 }
 X7=sqrt(ki_SUM/(HR_CNT-1));
 ki_SUM=0;
 
 Y=0.22*X1+0.25*X2+0.3*X3+0.42*X4+0.25*X5+0.35*X6+0.44*X7+12.876;
 PP=Y*10;//将Y扩大10倍，保存小数小数点后一位
 
 PP_Data[3]=PP/0X100;
 PP_Data[4]=PP%0X100;

 HAL_UART_Transmit_DMA(&huart2 , PP_Data, 6); 
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
