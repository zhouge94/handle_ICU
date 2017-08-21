#include "stm32f4xx_hal.h"
#include "DataScope_DP.h"
#include "ECG.h"
#include "sys.h"

ECG_DATA_S ECG_DATA1;
ECG_DATA_S ECG_DATA2;
ECG_DATA_S ECG_DATA3;

void ECG1_Data_ReciveHandle(uint8_t *pData,uint8_t size)
{
	  uint8_t date_size;
	  int16_t temp;
	  uint8_t *p=pData;
	  /* 判断是心电数据还是，心率数据 */
	  if(size==8)
	  {
		  if(p[0]==0XAA&&p[1]==0XAA)
		  {
			  temp=p[5];
			  temp<<=8;
			  temp+=p[6];
			  ECG_DATA1.ECG_DATA=temp;
		  }
	  }
	  else if(size==86)
	  {
		  while(p!=pData+86)
		  {
			  if(p[0]==0XAA&&p[1]==0XAA&&p[2]==18)
			  {
				  ECG_DATA1.ECG_HQ=p[4]*1.0;
				  ECG_DATA1.ECG_RTHR=p[6]*1.0;
				  break;
			  }
			  p++;
		  }
	  }
}
void ECG2_Data_ReciveHandle(uint8_t *pData,uint8_t size)
{
	  uint8_t date_size;
	  int16_t temp;
	  uint8_t *p=pData;
	  /* 判断是心电数据还是，心率数据 */
	  if(size==8)
	  {
		  if(p[0]==0XAA&&p[1]==0XAA)
		  {
			  temp=p[5];
			  temp<<=8;
			  temp+=p[6];
			  ECG_DATA2.ECG_DATA=temp;
		  }
	  }
	  else if(size==86)
	  {
		  while(p!=pData+86)
		  {
			  if(p[0]==0XAA&&p[1]==0XAA&&p[2]==18)
			  {
				  ECG_DATA2.ECG_HQ=p[4]*1.0;
				  ECG_DATA2.ECG_RTHR=p[6]*1.0;
				  break;
			  }
			  p++;
		  }
	  }
}
void ECG3_Data_ReciveHandle(uint8_t *pData,uint8_t size)
{
	  uint8_t date_size;
	  int16_t temp;
	  uint8_t *p=pData;
	  /* 判断是心电数据还是，心率数据 */
	  if(size==8)
	  {
		  if(p[0]==0XAA&&p[1]==0XAA)
		  {
			  temp=p[5];
			  temp<<=8;
			  temp+=p[6];
			  ECG_DATA3.ECG_DATA=temp;
		  }
	  }
	  else if(size==86)
	  {
		  while(p!=pData+86)
		  {
			  if(p[0]==0XAA&&p[1]==0XAA&&p[2]==18)
			  {
				  ECG_DATA3.ECG_HQ=p[4]*1.0;
				  ECG_DATA3.ECG_RTHR=p[6]*1.0;
				  break;
			  }
			  p++;
		  }
	  }
}