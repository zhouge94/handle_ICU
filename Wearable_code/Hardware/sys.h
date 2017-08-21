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
typedef enum
{
	Ascii,
	Datascope
}TranTypeDef;
typedef struct
{
	__IO float a;
	__IO float b;
	__IO TranTypeDef Transt;
	__IO float GYRO_X;
	__IO float GYRO_Y;
	__IO float GYRO_Z;
	__IO float ACCEL_X;
	__IO float ACCEL_Y;
	__IO float ACCEL_Z;
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
