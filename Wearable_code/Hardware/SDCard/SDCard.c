#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"	 
#include "rtc.h"

#include "stdio.h"	//这三个头文件用于支持浮点数转换为字符串功能
#include "string.h"
#include "math.h"
#include "fatfs.h"

#include "SDCard.h"

#define SDCard_Buf_Len 6144//SD卡缓存区缓存数据长度

FATFS SDCard;

FIL file;//文件对象
uint8_t Status = 0;
char* String = "\nWrite some words to this file by STM32F411 MCU.\r\n";
UINT Number = 0;
char Path[16] = "/File/data.txt";

FRESULT res;
uint32_t pointer=0;//可以存4096M的内容
uint32_t i;
uint32_t cnt=0;//要写入的测试数据


char SDCard_Buf_start[128]={0};


char SDCard_Buf_1[SDCard_Buf_Len*3+64]={0};
char SDCard_Buf_2[SDCard_Buf_Len*3+64]={0};

uint8_t SDCard_Buf_1_Full=0;//往SD卡写数据缓存区1满标志位，SDCard_Buf_1_Full=0未满；SDCard_Buf_1_Full=1 已满。
uint8_t SDCard_Buf_2_Full=1;//往SD卡写数据缓存区1满标志位，SDCard_Buf_1_Full=0未满；SDCard_Buf_1_Full=1 已满。
uint8_t Start_Write_SDCard=0;//缓存区数据已满，开始往SD卡写数据，Start_Write_SDCard=1，表示可以开始写

uint8_t Write_Buffer_1o2=0;//表示数据是否正在存入缓存区1或者是缓存区2；Write_Buffer_1o2==0 表示当前正往缓存区1写入数据，Write_Buffer_1o2==1 表示当前正往缓存区2写入数据
uint16_t SDCard_Buf_cnt=0;//SD卡缓存区数据计数

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
		  printf("磁盘正常!\r\n");
			break;
		case FR_INVALID_DRIVE://驱动器号无效
			SDCard_Status = 1;
		  printf("驱动器号无效!\r\n");
	    break;
		case FR_NOT_READY://由于驱动器中没有存储介质或任何其他原因，而导致磁盘驱动器无法工作
			SDCard_Status = 2;
		  printf("磁盘驱动器无法工作!\r\n");
			break;
		case FR_DISK_ERR://由于底层磁盘I/O接口函数中的一个错误，而导致该函数失败
			SDCard_Status = 3;
		  printf("底层磁盘I/O错误!\r\n");
			break;
		case FR_NO_PATH:  //找不到该路径
			SDCard_Status = 4;
		  printf("找不到该路径!\r\n");
			break;
		case FR_INVALID_NAME: //文件名无效
			SDCard_Status = 5;
		  printf("文件名无效!\r\n");
			break;
		case FR_NO_FILE: //找不到该文件
			SDCard_Status = 6;
			printf("找不到该文件!\r\n");
			break;
		case FR_EXIST: //文件已存在
			SDCard_Status = 7;
			printf("文件已存在!\r\n");
			break;
		default: //其他的为Disk error
			SDCard_Status = 8;
			printf("其他的Disk error!\r\n");
			break;
		}	
    APP_RTC_Get();	
	  sprintf(SDCard_Buf_start,"\r\nStart at %04d-%02d-%02d %02d:%02d:%02d %x %x\r\n",g_rtc_year,g_rtc_month,g_rtc_day,g_rtc_hour,g_rtc_minute,g_rtc_second,0XAA,0XAA); 
		if(!res)
		{
		    pointer=file.fsize; //获得文件大小
		    res=f_lseek(&file,pointer);//移动文件指针
	      Status = f_write(&file, SDCard_Buf_start, strlen(SDCard_Buf_start) , &Number);
	      f_close(&file);
		    printf("写入日期数据成功!\r\n");
		
		}
		return SDCard_Status;
}


void Write_Data_To_SDCard(char *pData)
{
	res=f_open(&file, Path, FA_READ | FA_OPEN_EXISTING | FA_WRITE);	
//	switch(res)
//	{																
//		case FR_OK:	
//			printf("磁盘正常!\r\n");
//			break;
//		case FR_INVALID_DRIVE://驱动器号无效
//			printf("驱动器号无效!\r\n");
//	    break;
//		case FR_NOT_READY://由于驱动器中没有存储介质或任何其他原因，而导致磁盘驱动器无法工作
//			printf("磁盘驱动器无法工作!\r\n");
//			break;
//		case FR_DISK_ERR://由于底层磁盘I/O接口函数中的一个错误，而导致该函数失败
//			printf("底层磁盘I/O错误!\r\n");
//			break;
//		case FR_NO_PATH:  //找不到该路径
//			printf("找不到该路径!\r\n");
//			break;
//		case FR_INVALID_NAME: //文件名无效
//			printf("文件名无效!\r\n");
//			break;
//		case FR_NO_FILE: //找不到该文件
//			printf("找不到该文件!\r\n");
//			break;
//		case FR_EXIST: //文件已存在
//			printf("文件已存在!\r\n");
//			break;
//		default: //其他的为Disk error
//			printf("其他的Disk error!\r\n");
//			break;
//		}
	
		if(!res)
		{
		    pointer=file.fsize; //获得文件大小
		    res=f_lseek(&file,pointer);//移动文件指针
	      Status = f_write(&file, pData, strlen(pData) , &Number);
	      f_close(&file);
//		    printf("写入数据成功!\r\nstrlen(SDCard_Buffer)=%d\r\n",strlen(pData));
		}
}

void Write_Data_To_SDBuffer(uint8_t *pData,uint8_t NUM)
{
	uint8_t ii=0;
	uint8_t jj=0;
	
	if(SDCard_Buf_cnt+NUM<=SDCard_Buf_Len)//缓存区还可以存入新的数据
	{
	  if(Write_Buffer_1o2==0)//表示此阶段整在往缓存区1写入数据
	  {
	    for(ii=0;ii<NUM;ii++)
      {
		    sprintf(SDCard_Buf_1+(SDCard_Buf_cnt++)*3,"%02X ",pData[ii]); 
      }		
	  }
	  if(Write_Buffer_1o2==1)//表示此阶段整在往缓存区2写入数据
	  {
	    for(ii=0;ii<NUM;ii++)
      {
		    sprintf(SDCard_Buf_2+(SDCard_Buf_cnt++)*3,"%02X ",pData[ii]); 
      }		
	  }
	}
	else//缓存区不可以存入新的数据，缓存区已满，开始将数据写入SD卡
	{
	  jj=(SDCard_Buf_Len-SDCard_Buf_cnt)*3;//获取缓存区有多少数据未更新
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
		//切换数据存入缓存区
		if(Write_Buffer_1o2==0)
		{
			Write_Buffer_1o2=1;//随后将数据写入缓存区2
		}
		else
    {
		  Write_Buffer_1o2=0;//随后将数据写入缓存区1
		}
		
		SDCard_Buf_cnt=0;//将计数指针清零，从头开始写入数据
		
		Start_Write_SDCard=1;//置1，开始往SD卡写入数据
	}
}

void SDCard_Buffer_Handler(void)
{
  if(Start_Write_SDCard==1)
	{
	  Start_Write_SDCard=0;
		if(Write_Buffer_1o2==0)//正在往缓存区1写入数据，将缓存区2的数据写入SD卡
		{
			 Write_Data_To_SDCard(SDCard_Buf_2);
		}
	  else//正在往缓存区2写入数据，将缓存区1的数据写入SD卡
		{
			 Write_Data_To_SDCard(SDCard_Buf_1);
		}
	}
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
