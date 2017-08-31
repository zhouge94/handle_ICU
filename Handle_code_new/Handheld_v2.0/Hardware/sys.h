
#ifndef APPLICATION_USER_SYS_H_
#define APPLICATION_USER_SYS_H_

#include "stdio.h"
#include "stdarg.h"
#include "stdlib.h"
#include "stm32f4xx_hal.h"
#include "string.h"
#include "i2c.h"
#include "usart.h"
#include "MY_Uart.h"
#include "DataScope_DP.h"
#include "tim.h"

extern UART_HandleTypeDef huart2;

#define DEBUG_ON 1
#if (DEBUG_ON == 1)
#define MyDEBUG debugf
#else
#define MyDEBUG
#endif

#define OUTF(buffer,cnt)  SendString(0x11,buffer)
//#define OUTF(buffer,cnt)     while(huart2.gState == HAL_UART_STATE_BUSY); HAL_UART_Transmit_DMA(&huart2,buffer,cnt);
uint8_t debugf(char *fmt, ...);

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
  
  __IO unsigned char key;
  
  __IO unsigned char Get_Flat;
  __IO float a;
  __IO float b;
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

  
  __IO unsigned int uart1_count;

}Sys_S;
extern Sys_S sys;
void delayns(int i);
void sys_data_init(void);
double ZhongZhiFilter(int N,float *DATE_RAW );
void USER_PWM_SetDutyRatio(TIM_HandleTypeDef *htim,uint32_t Channel,uint8_t value);  


#define BELL_ON  USER_PWM_SetDutyRatio(&htim2, TIM_CHANNEL_1,50)
#define BELL_OFF  USER_PWM_SetDutyRatio(&htim2, TIM_CHANNEL_1,100)

#endif /* APPLICATION_USER_SYS_H_ */
