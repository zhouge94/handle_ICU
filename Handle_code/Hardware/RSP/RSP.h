#ifndef __RSP_H
#define __RSP_H	

#include "stm32f4xx_hal.h"

void RSP_DATA_Handler(uint8_t *pData,uint8_t Data_Num);
void TEM_DATA_Handler(uint8_t *pData,uint8_t Data_Num);
void ACC_DATA_Handler(uint8_t *pData,uint8_t Data_Num);

#endif
