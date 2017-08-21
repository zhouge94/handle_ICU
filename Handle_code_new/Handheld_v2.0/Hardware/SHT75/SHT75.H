/*
@version 01
@date    2016.3.22
@author  尚小虎
@brief 温湿度传感器（SHT75） 头文件
@note  管脚配置需在 STM32CubeMX 中设置
*/

#ifndef __SHT75_H
#define __SHT75_H	
#include "stm32f4xx.h"

#define SHT_SCK_H         HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_SET)
#define SHT_SCK_L         HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_RESET)
   
#define SHT_DATA_H         HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_SET)
#define SHT_DATA_L         HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_RESET) 

#define SHT_SCK_READ      HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_8)
#define SHT_DATA_READ      HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_9)

#define noACK 0
#define ACK 1
//adr command r/w
#define STATUS_REG_W 0x06 //000 0011 0
#define STATUS_REG_R 0x07 //000 0011 1
#define MEASURE_TEMP 0x03 //000 0001 1
#define MEASURE_HUMI 0x05 //000 0010 1
#define RESET 0x1e //000 1111 0


void SHT75_IOInit(void);
char SHT75_Write_Byte(unsigned char value);
char SHT75_Read_Byte(unsigned char ack);
void SHT75_Trans_Start(void);
void SHT75_Connection_Reset(void);
char SHT75_Soft_Reset(void);
char SHT75_Write_StausReg(unsigned char *p_value);
char SHT75_Read_StausReg(unsigned char *p_value,unsigned char *p_checksum);
char SHT75_Measure(unsigned int *p_value,unsigned char *p_checksum,unsigned char mode);
void Calc_SHT75(float *p_humidity,float *p_temperature);
float Calc_Dewpoint(float h,float t);
char SHT75_Measure_Result(float *p_temp,float *p_humi,float *p_dewpoint);

void GET_SHT75(void);

#endif
