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

typedef struct MPUDATA_Sructure
{
	__IO  unsigned char BUF[30];
	__IO float GYRO_X;
	__IO float GYRO_Y;
	__IO float GYRO_Z;
	__IO float ACCEL_X;
	__IO float ACCEL_Y;
	__IO float ACCEL_Z;
	__IO float SUM_ACCEL;
}MPUDATA_Sructure;
unsigned char MSingle_Write(unsigned char SlaveAddress,unsigned char REG_Address,unsigned char REG_data);
unsigned char MSingle_Read(unsigned char SlaveAddress,unsigned char REG_Address);
unsigned int GetData(unsigned char REG_Address);
void Init_MPU6050(void);
void HMC5883_Init(void);
unsigned int Data_Change(unsigned int data);
void zhuanhuan(unsigned int Data);
void usart1_send_data(unsigned char *pdata);

void READ_MPU6050(void);

void GET_ACC(void);
#endif

