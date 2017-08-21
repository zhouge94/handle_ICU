#ifndef __SimI2C_H
#define __SimI2C_H	
#include "stm32f4xx.h"

#define FALSE   0
#define TRUE   1

#define	SimI2C_SDA_OUT()   { GPIO_InitTypeDef GPIO_InitStruct;\
	                           __GPIOB_CLK_ENABLE();\
                             GPIO_InitStruct.Pin = GPIO_PIN_3;\
                             GPIO_InitStruct.Mode =GPIO_MODE_OUTPUT_OD;\
                             GPIO_InitStruct.Pull = GPIO_PULLUP;\
                             GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;\
                             HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);}


#define	SimI2C_SDA_IN()    { GPIO_InitTypeDef GPIO_InitStruct;\
	                           __GPIOB_CLK_ENABLE();\
                            GPIO_InitStruct.Pin = GPIO_PIN_3;\
                            GPIO_InitStruct.Mode =GPIO_MODE_INPUT;\
                            GPIO_InitStruct.Pull = GPIO_PULLUP;\
                            GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;\
                            HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);}

#define SCL_H         HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,GPIO_PIN_SET)
#define SCL_L         HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,GPIO_PIN_RESET)
   
#define SDA_H         HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_SET)
#define SDA_L         HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_RESET) 

#define SCL_read      HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_10)
#define SDA_read      HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_3)

void delay5ms(void);
void I2C_delay(void);
void MX_SimI2C_IO_Config(void);
unsigned char I2C_Start(void);
void I2C_Stop(void);
void I2C_Ack(void);
void I2C_NoAck(void);
unsigned I2C_WaitAck(void);
void I2C_SendByte(uint8_t SendByte);
unsigned char I2C_RadeByte(void);
unsigned char Single_Write(unsigned char SlaveAddress,unsigned char REG_Address,unsigned char REG_data);
unsigned char Single_Read(unsigned char SlaveAddress,unsigned char REG_Address);
#endif
