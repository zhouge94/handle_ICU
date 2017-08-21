#ifndef __SDCard_H
#define __SDCard_H	
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"

void Write_Data_To_SDCard(char *pData);
uint8_t Write_StartData_To_SDCard(void);
void SDCard_Buffer_Handler(void);
void Write_Data_To_SDBuffer(uint8_t *pData,uint8_t NUM);

#endif
