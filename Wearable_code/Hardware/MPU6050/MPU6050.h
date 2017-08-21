#ifndef __MPU6050_H__
#define __MPU6050_H__

#include "stm32f4xx.h"

#define   uchar unsigned char
#define   uint unsigned int	


// ����MPU6050�ڲ���ַ
//****************************************
#define	SMPLRT_DIV		0x19	//�����ǲ����ʣ�����ֵ��0x07(125Hz)
#define	CONFIG			0x1A	//��ͨ�˲�Ƶ�ʣ�����ֵ��0x06(5Hz)
#define	GYRO_CONFIG		0x1B	//�������Լ켰������Χ������ֵ��0x18(���Լ죬2000deg/s)
#define	ACCEL_CONFIG	0x1C	//���ټ��Լ졢������Χ����ͨ�˲�Ƶ�ʣ�����ֵ��0x01(���Լ죬2G��5Hz)

#define	ACCEL_XOUT_H	0x3B
#define	ACCEL_XOUT_L	0x3C
#define	ACCEL_YOUT_H	0x3D
#define	ACCEL_YOUT_L	0x3E
#define	ACCEL_ZOUT_H	0x3F
#define	ACCEL_ZOUT_L	0x40

#define	TEMP_OUT_H		0x41
#define	TEMP_OUT_L		0x42

#define	GYRO_XOUT_H		0x43
#define	GYRO_XOUT_L		0x44	
#define	GYRO_YOUT_H		0x45
#define	GYRO_YOUT_L		0x46
#define	GYRO_ZOUT_H		0x47
#define	GYRO_ZOUT_L		0x48

#define	PWR_MGMT_1		0x6B	//��Դ��������ֵ��0x00(��������)
#define	WHO_AM_I		0x75	//IIC��ַ�Ĵ���(Ĭ����ֵ0x68��ֻ��)

#define FALSE   0
#define TRUE   1

//****************************

#define	MPU6050_Addr   0xD0	  //����������IIC�����еĴӵ�ַ,����ALT  ADDRESS��ַ���Ų�ͬ�޸�

//************************************
/*ģ��IIC�˿�������붨��*/

//////IO��������
#define	MI2C_SDA_OUT()   { GPIO_InitTypeDef GPIO_InitStruct;\
	                           __GPIOB_CLK_ENABLE();\
                             GPIO_InitStruct.Pin = GPIO_PIN_3;\
                             GPIO_InitStruct.Mode =GPIO_MODE_OUTPUT_OD;\
                             GPIO_InitStruct.Pull = GPIO_PULLUP;\
                             GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;\
                             HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);}


#define	MI2C_SDA_IN()    { GPIO_InitTypeDef GPIO_InitStruct;\
	                           __GPIOB_CLK_ENABLE();\
                            GPIO_InitStruct.Pin = GPIO_PIN_3;\
                            GPIO_InitStruct.Mode =GPIO_MODE_INPUT;\
                            GPIO_InitStruct.Pull = GPIO_PULLUP;\
                            GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;\
                            HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);}
//#define SDA_IN()  {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=0<<9*2;}	//PB9����ģʽ
//#define SDA_OUT() {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=1<<9*2;} //PB9���ģʽ

//IO��������	
#define MSCL_H         HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,GPIO_PIN_SET)
#define MSCL_L         HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,GPIO_PIN_RESET)
   
#define MSDA_H         HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_SET)
#define MSDA_L         HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_RESET) 

#define MSCL_read      HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_10)
#define MSDA_read      HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_3)


void MI2C_delay(void);
void MI2C_GPIO_Config(void);
unsigned char MI2C_Start(void);
void MI2C_Stop(void);
void MI2C_Ack(void);
void MI2C_NoAck(void);
unsigned MI2C_WaitAck(void);
void MI2C_SendByte(uint8_t SendByte);
unsigned char MI2C_RadeByte(void);
unsigned char MSingle_Write(unsigned char SlaveAddress,unsigned char REG_Address,unsigned char REG_data);
unsigned char MSingle_Read(unsigned char SlaveAddress,unsigned char REG_Address);
unsigned int GetData(unsigned char REG_Address);
void Init_MPU6050(void);
void HMC5883_Init(void);
unsigned int Data_Change(unsigned int data);
void zhuanhuan(unsigned int Data);
void usart1_send_data(unsigned char *pdata);

void TEST_READ_MPU6050(void);
void READ_MPU6050(void);

void GET_ACC(void);
#endif


