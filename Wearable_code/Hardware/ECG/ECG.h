#ifndef __ECG_H
#define __ECG_H	

#include "stm32f4xx_hal.h"

void ECG1_Data_ReciveHandle(uint8_t *pData);
void ECG2_Data_ReciveHandle(uint8_t *pData);
void ECG3_Data_ReciveHandle(uint8_t *pData);
void Send_ECG_DATA(void);

void DMA_Send_ECG_DATA(void);
void DMA_Send_HR_DATA(void);
	
void Send_ECG1_Data_Neg(uint16_t ECG_Data);
void Send_ECG2_Data_Neg(uint16_t ECG_Data);
void Send_ECG3_Data_Neg(uint16_t ECG_Data);

void DMA_Send_ECG_DATA(void);

void get_PsychologicalPressure(void);

#endif
