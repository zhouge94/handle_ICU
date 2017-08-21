#ifndef __MS5611_H__
#define __MS5611_H__

#include "stm32f4xx.h"

#include <math.h> //Keil library 
#include <stdlib.h> //Keil library 
#include <stdio.h> //Keil library  


#define  MS561101BA_SlaveAddress 0xee  //定义器件在IIC总线中的从地址

#define  MS561101BA_D1 0x40 
#define  MS561101BA_D2 0x50 
#define  MS561101BA_RST 0x1E 

//#define  MS561101BA_D1_OSR_256 0x40 
//#define  MS561101BA_D1_OSR_512 0x42 
//#define  MS561101BA_D1_OSR_1024 0x44 
//#define  MS561101BA_D1_OSR_2048 0x46 
#define  MS561101BA_D1_OSR_4096 0x48 

//#define  MS561101BA_D2_OSR_256 0x50 
//#define  MS561101BA_D2_OSR_512 0x52 
//#define  MS561101BA_D2_OSR_1024 0x54 
//#define  MS561101BA_D2_OSR_2048 0x56 
#define  MS561101BA_D2_OSR_4096 0x58 

#define  MS561101BA_ADC_RD 0x00 
#define  MS561101BA_PROM_RD 0xA0 
#define  MS561101BA_PROM_CRC 0xAE 



#define	SDA_OUT()    {GPIO_InitTypeDef GPIO_InitStruct;\
	                    __GPIOB_CLK_ENABLE();\
                      GPIO_InitStruct.Pin = GPIO_PIN_9;\
                      GPIO_InitStruct.Mode =GPIO_MODE_OUTPUT_OD;\
                      GPIO_InitStruct.Pull = GPIO_PULLUP;\
                      GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;\
                      HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);}


#define	SDA_IN()    {GPIO_InitTypeDef GPIO_InitStruct;\
	                    __GPIOB_CLK_ENABLE();\
                      GPIO_InitStruct.Pin = GPIO_PIN_9;\
                      GPIO_InitStruct.Mode =GPIO_MODE_INPUT;\
                      GPIO_InitStruct.Pull = GPIO_PULLUP;\
                      GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;\
                      HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);}


//IO操作函数
#define IIC_SCL_Clr() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_RESET)	//SCL PB8
#define IIC_SCL_Set() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_SET)

#define IIC_SDA_Clr() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_RESET)	//SDA PB9
#define IIC_SDA_Set() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_SET)

#define READ_SDA 		HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_9) //SDA PB9	

//////////////////////////////////////////////////////////////////////////////////// 

void MS561101BA_RESET(void); 
void MS561101BA_PROM_READ(void); 
uint32_t MS561101BA_DO_CONVERSION(uint8_t command); 
void MS561101BA_GetTemperature(uint8_t OSR_Temp);
void MS561101BA_GetPressure(uint8_t OSR_Pres);
void MS561101BA_Init(void); 
void Usart_Send(uint8_t *p,uint16_t len);
void SeriPushSend(uint8_t send_data); 
void Exchange_Number(void); 
void READ_MS5611_Pressure(void);
void read_MS5611_temputer(void);

void GET_Pressure(void);

void IIC_GPIO_Config(void);
void IIC_Start(void);
void IIC_Stop(void);
uint8_t IIC_Wait_Ack(void);
void IIC_Ack(void);
void IIC_NAck(void);
void IIC_Send_Byte(uint8_t txd);
uint8_t IIC_Read_Byte(unsigned char ack);
unsigned char I2C_ReadOneByte(unsigned char I2C_Addr,unsigned char addr);
uint8_t IICreadBytes(uint8_t dev, uint8_t reg, uint8_t length, uint8_t *data);
uint8_t IICwriteBytes(uint8_t dev, uint8_t reg, uint8_t length, uint8_t* data);
uint8_t IICreadByte(uint8_t dev, uint8_t reg, uint8_t *data);
unsigned char IICwriteByte(unsigned char dev, unsigned char reg, unsigned char data);
uint8_t IICwriteBits(uint8_t dev,uint8_t reg,uint8_t bitStart,uint8_t length,uint8_t data);
uint8_t IICwriteBit(uint8_t dev, uint8_t reg, uint8_t bitNum, uint8_t data);


#endif




