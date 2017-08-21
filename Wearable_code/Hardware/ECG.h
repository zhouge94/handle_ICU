#ifndef __ECG_H
#define __ECG_H	
#include "stm32f4xx.h"

void ECG1_Data_ReciveHandle(uint8_t *pData,uint8_t size);
void ECG2_Data_ReciveHandle(uint8_t *pData,uint8_t size);
void ECG3_Data_ReciveHandle(uint8_t *pData,uint8_t size);
void Send_ECG_DATA(void);

typedef struct
{
	short int ECG_DATA;
	unsigned char ECG_HQ;
	unsigned char ECG_RTHR;

}ECG_DATA_S;
extern ECG_DATA_S ECG_DATA1;
extern ECG_DATA_S ECG_DATA2;
extern ECG_DATA_S ECG_DATA3;

#endif
