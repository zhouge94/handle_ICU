#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"	 
#include "rtc.h"

#include "stdio.h"	//������ͷ�ļ�����֧�ָ�����ת��Ϊ�ַ�������
#include "string.h"
#include "math.h"
#include "fatfs.h"

#include "SDCard.h"

#define SDCard_Buf_Len 6144//SD���������������ݳ���

FATFS SDCard;

FIL file;//�ļ�����
uint8_t Status = 0;
char* String = "\nWrite some words to this file by STM32F411 MCU.\r\n";
UINT Number = 0;
char Path[16] = "/File/data.txt";

FRESULT res;
uint32_t pointer=0;//���Դ�4096M������
uint32_t i;
uint32_t cnt=0;//Ҫд��Ĳ�������


char SDCard_Buf_start[128]={0};


char SDCard_Buf_1[SDCard_Buf_Len*3+64]={0};
char SDCard_Buf_2[SDCard_Buf_Len*3+64]={0};

uint8_t SDCard_Buf_1_Full=0;//��SD��д���ݻ�����1����־λ��SDCard_Buf_1_Full=0δ����SDCard_Buf_1_Full=1 ������
uint8_t SDCard_Buf_2_Full=1;//��SD��д���ݻ�����1����־λ��SDCard_Buf_1_Full=0δ����SDCard_Buf_1_Full=1 ������
uint8_t Start_Write_SDCard=0;//������������������ʼ��SD��д���ݣ�Start_Write_SDCard=1����ʾ���Կ�ʼд

uint8_t Write_Buffer_1o2=0;//��ʾ�����Ƿ����ڴ��뻺����1�����ǻ�����2��Write_Buffer_1o2==0 ��ʾ��ǰ����������1д�����ݣ�Write_Buffer_1o2==1 ��ʾ��ǰ����������2д������
uint16_t SDCard_Buf_cnt=0;//SD�����������ݼ���

extern uint16_t g_rtc_con;
extern uint16_t g_rtc_year;
extern uint16_t g_rtc_month;
extern uint16_t g_rtc_day;
extern uint16_t g_rtc_hour;
extern uint16_t g_rtc_minute;
extern uint16_t g_rtc_second;

uint8_t Write_StartData_To_SDCard(void)
{
	uint8_t SDCard_Status = 0;
	res=f_open(&file, Path, FA_READ | FA_OPEN_EXISTING | FA_WRITE);	
	switch(res)
	{																
		case FR_OK:	
			SDCard_Status = 0;
		  printf("��������!\r\n");
			break;
		case FR_INVALID_DRIVE://����������Ч
			SDCard_Status = 1;
		  printf("����������Ч!\r\n");
	    break;
		case FR_NOT_READY://������������û�д洢���ʻ��κ�����ԭ�򣬶����´����������޷�����
			SDCard_Status = 2;
		  printf("�����������޷�����!\r\n");
			break;
		case FR_DISK_ERR://���ڵײ����I/O�ӿں����е�һ�����󣬶����¸ú���ʧ��
			SDCard_Status = 3;
		  printf("�ײ����I/O����!\r\n");
			break;
		case FR_NO_PATH:  //�Ҳ�����·��
			SDCard_Status = 4;
		  printf("�Ҳ�����·��!\r\n");
			break;
		case FR_INVALID_NAME: //�ļ�����Ч
			SDCard_Status = 5;
		  printf("�ļ�����Ч!\r\n");
			break;
		case FR_NO_FILE: //�Ҳ������ļ�
			SDCard_Status = 6;
			printf("�Ҳ������ļ�!\r\n");
			break;
		case FR_EXIST: //�ļ��Ѵ���
			SDCard_Status = 7;
			printf("�ļ��Ѵ���!\r\n");
			break;
		default: //������ΪDisk error
			SDCard_Status = 8;
			printf("������Disk error!\r\n");
			break;
		}	
    APP_RTC_Get();	
	  sprintf(SDCard_Buf_start,"\r\nStart at %04d-%02d-%02d %02d:%02d:%02d %x %x\r\n",g_rtc_year,g_rtc_month,g_rtc_day,g_rtc_hour,g_rtc_minute,g_rtc_second,0XAA,0XAA); 
		if(!res)
		{
		    pointer=file.fsize; //����ļ���С
		    res=f_lseek(&file,pointer);//�ƶ��ļ�ָ��
	      Status = f_write(&file, SDCard_Buf_start, strlen(SDCard_Buf_start) , &Number);
	      f_close(&file);
		    printf("д���������ݳɹ�!\r\n");
		
		}
		return SDCard_Status;
}


void Write_Data_To_SDCard(char *pData)
{
	res=f_open(&file, Path, FA_READ | FA_OPEN_EXISTING | FA_WRITE);	
//	switch(res)
//	{																
//		case FR_OK:	
//			printf("��������!\r\n");
//			break;
//		case FR_INVALID_DRIVE://����������Ч
//			printf("����������Ч!\r\n");
//	    break;
//		case FR_NOT_READY://������������û�д洢���ʻ��κ�����ԭ�򣬶����´����������޷�����
//			printf("�����������޷�����!\r\n");
//			break;
//		case FR_DISK_ERR://���ڵײ����I/O�ӿں����е�һ�����󣬶����¸ú���ʧ��
//			printf("�ײ����I/O����!\r\n");
//			break;
//		case FR_NO_PATH:  //�Ҳ�����·��
//			printf("�Ҳ�����·��!\r\n");
//			break;
//		case FR_INVALID_NAME: //�ļ�����Ч
//			printf("�ļ�����Ч!\r\n");
//			break;
//		case FR_NO_FILE: //�Ҳ������ļ�
//			printf("�Ҳ������ļ�!\r\n");
//			break;
//		case FR_EXIST: //�ļ��Ѵ���
//			printf("�ļ��Ѵ���!\r\n");
//			break;
//		default: //������ΪDisk error
//			printf("������Disk error!\r\n");
//			break;
//		}
	
		if(!res)
		{
		    pointer=file.fsize; //����ļ���С
		    res=f_lseek(&file,pointer);//�ƶ��ļ�ָ��
	      Status = f_write(&file, pData, strlen(pData) , &Number);
	      f_close(&file);
//		    printf("д�����ݳɹ�!\r\nstrlen(SDCard_Buffer)=%d\r\n",strlen(pData));
		}
}

void Write_Data_To_SDBuffer(uint8_t *pData,uint8_t NUM)
{
	uint8_t ii=0;
	uint8_t jj=0;
	
	if(SDCard_Buf_cnt+NUM<=SDCard_Buf_Len)//�����������Դ����µ�����
	{
	  if(Write_Buffer_1o2==0)//��ʾ�˽׶�������������1д������
	  {
	    for(ii=0;ii<NUM;ii++)
      {
		    sprintf(SDCard_Buf_1+(SDCard_Buf_cnt++)*3,"%02X ",pData[ii]); 
      }		
	  }
	  if(Write_Buffer_1o2==1)//��ʾ�˽׶�������������2д������
	  {
	    for(ii=0;ii<NUM;ii++)
      {
		    sprintf(SDCard_Buf_2+(SDCard_Buf_cnt++)*3,"%02X ",pData[ii]); 
      }		
	  }
	}
	else//�����������Դ����µ����ݣ���������������ʼ������д��SD��
	{
	  jj=(SDCard_Buf_Len-SDCard_Buf_cnt)*3;//��ȡ�������ж�������δ����
		for(ii=0;ii<jj;jj++)
		{
		  if(Write_Buffer_1o2==0)
			{
			   SDCard_Buf_1[SDCard_Buf_Len*3-ii]=0;
			}
			if(Write_Buffer_1o2==1)
			{
			   SDCard_Buf_2[SDCard_Buf_Len*3-ii]=0;
			}
		}	
		//�л����ݴ��뻺����
		if(Write_Buffer_1o2==0)
		{
			Write_Buffer_1o2=1;//�������д�뻺����2
		}
		else
    {
		  Write_Buffer_1o2=0;//�������д�뻺����1
		}
		
		SDCard_Buf_cnt=0;//������ָ�����㣬��ͷ��ʼд������
		
		Start_Write_SDCard=1;//��1����ʼ��SD��д������
	}
}

void SDCard_Buffer_Handler(void)
{
  if(Start_Write_SDCard==1)
	{
	  Start_Write_SDCard=0;
		if(Write_Buffer_1o2==0)//������������1д�����ݣ���������2������д��SD��
		{
			 Write_Data_To_SDCard(SDCard_Buf_2);
		}
	  else//������������2д�����ݣ���������1������д��SD��
		{
			 Write_Data_To_SDCard(SDCard_Buf_1);
		}
	}
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
