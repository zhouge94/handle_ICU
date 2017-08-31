/*
 * sys.h
 *
 *  Created on: Dec 5, 2016
 *      Author: ubuntu-gnome
 */

#ifndef APPLICATION_USER_SYS_H_
#define APPLICATION_USER_SYS_H_

#include "stdio.h"
#include "stdarg.h"
#include "stdlib.h"
#include "stm32f4xx_hal.h"
#include "string.h"
#include "i2c.h"
#include "usart.h"

extern UART_HandleTypeDef huart2;

#define DEBUG_ON 1
#if (DEBUG_ON == 1)
#define MyDEBUG debugf
#else
#define MyDEBUG
#endif

#define OUTF(buffer,cnt) HAL_UART_Transmit_DMA(&huart2,buffer,cnt);
uint8_t debugf(uint8_t level,char *fmt, ...);

typedef struct
{
  __IO unsigned char ecg_index;
  __IO short int ecg1buff[10];
  __IO short int ecg2buff[10];
  __IO short int ecg3buff[10];
  
  __IO unsigned char mpu_index;
  __IO short int mpuAxbuff[10];
  __IO short int mpuAybuff[10];
  __IO short int mpuAzbuff[10];
  
	__IO float a;
	__IO float b;

	__IO short int GYRO_X;
	__IO short int GYRO_Y;
	__IO short int GYRO_Z;
	__IO short int ACCEL_X;
	__IO short int ACCEL_Y;
	__IO short int ACCEL_Z;
  
	__IO float SUM_ACCEL;
	__IO float T_T;
	__IO float T_r;
	__IO float T_o;
	__IO float ZuKang;
	__IO float ZuKang_filter;
	__IO float V_Bat;


}Sys_S;
extern Sys_S sys;
void delayns(int i);
double ZhongZhiFilter(int N,float *DATE_RAW );
#endif /* APPLICATION_USER_SYS_H_ */
