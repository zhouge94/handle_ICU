#ifndef __ECG_H
#define __ECG_H	

#include "stm32f4xx_hal.h"

#define RSP_DATA 0X03
#define ECG_DATA 0X06
#define HR_DATA 0X12


void ECG_DATA_Handler(uint8_t *pData,uint8_t Data_Num);
void HR_DATA_Handler(uint8_t *pData,uint8_t Data_Num);
void Display_ECG_Wave(void);
void Display_Heart_Rate(void);
void Adjustment_ECG_Data(float Data_1,float Data_2,float Data_3);
float Complement_To_True(uint16_t Data);
void ECG_Data_ReciveHandle(void);

#endif
