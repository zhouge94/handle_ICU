#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "MPU6050.h"
#include "sys.h"
#include "math.h"
int __errno;
unsigned char ZhuanHua1[10]={'0','1','2','3','4','5','6','7','8','9'};//转换成ascii码

unsigned char FUHAO=0;
unsigned char TX_DATA[4];  	 //显示据缓存区
unsigned char BUF[30];       //接收数据缓存区

float T_X,T_Y,T_Z,T_T,J_X,J_Y,J_Z;		 //X,Y,Z轴，温度,X,Y,Z轴加速度
unsigned char temp_bw=0,temp_sw=0,temp_gw=0;
unsigned char a_sw=0,a_gw=0;//加速度

uint8_t ACC_DATA_X[5]={0XAA,0XAA,0X02,0X00,0X00};
extern I2C_HandleTypeDef hi2c2;
//**************************************
//向I2C设备写入一个字节数据
//**************************************
unsigned char MSingle_Write(unsigned char SlaveAddress,unsigned char REG_Address,unsigned char REG_data)
{
    uint8_t rxData[2] = {REG_Address,REG_data};
    while(HAL_I2C_Master_Transmit(&hi2c2,SlaveAddress,rxData,2,5000) != HAL_OK)
    {
        if(HAL_I2C_GetError(&hi2c2) != HAL_I2C_ERROR_AF)
        {}
    }
    return 0;//
}
//**************************************
//从I2C设备读取一个字节数据
//**************************************
unsigned char MSingle_Read(unsigned char SlaveAddress,unsigned char REG_Address)
{
    uint8_t REG_data;
    while(HAL_I2C_Master_Transmit(&hi2c2,SlaveAddress,&REG_Address,1,5000) != HAL_OK)
    {
        if(HAL_I2C_GetError(&hi2c2) != HAL_I2C_ERROR_AF)
        {}
    }

    if(HAL_I2C_Master_Receive(&hi2c2,SlaveAddress+1,&REG_data,1,5000) != HAL_OK)
    {
        if(HAL_I2C_GetError(&hi2c2) != HAL_I2C_ERROR_AF)
        {}
    }
    return REG_data;
}
unsigned int GetData(unsigned char REG_Address)
{
	char H,L;
	H=MSingle_Read(MPU6050_Addr,REG_Address);
	L=MSingle_Read(MPU6050_Addr,REG_Address);
	return ((H<<8)+L)/16.4;   //合成数据
}
//初始化MPU6050，根据需要请参考pdf进行修改************************
void Init_MPU6050(void)
{
  MSingle_Write(MPU6050_Addr,PWR_MGMT_1, 0x00);	//解除休眠状态
	MSingle_Write(MPU6050_Addr,SMPLRT_DIV, 0x07);
	MSingle_Write(MPU6050_Addr,CONFIG, 0x06);
	MSingle_Write(MPU6050_Addr,GYRO_CONFIG, 0x18);//选择陀螺仪输出量程±2000°/S  ，灵敏度16.4LSB/°/S
	MSingle_Write(MPU6050_Addr,ACCEL_CONFIG, 0x09);//选择加速度仪输出量程±4g,灵敏度8192LSB/g
}
unsigned int Data_Change(unsigned int data)
{
    unsigned char ECG_Data_H,ECG_Data_L;
	if(data>=0X8000&&data<=0XFFFF)//数据为负的
    {
        data=~data+1;
		FUHAO=1;
	}
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
	ECG_Data_H=(data&0XFF00)/0X100;
	ECG_Data_L=data&0X00FF;
	data=ECG_Data_H*0x100+ECG_Data_L;//不知道为什么要加上这几句，不加就不对
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    return data;
}
void READ_MPU6050(void)
{
  /*
	BUF[0]=MSingle_Read(MPU6050_Addr,GYRO_XOUT_L);
	BUF[1]=MSingle_Read(MPU6050_Addr,GYRO_XOUT_H);
	sys.GYRO_X=BUF[1]*0X100+BUF[0];
	//sys.GYRO_X=Data_Change(sys.GYRO_X)/16.4;
//==============================================================
	BUF[2]=MSingle_Read(MPU6050_Addr,GYRO_YOUT_L);
	BUF[3]=MSingle_Read(MPU6050_Addr,GYRO_YOUT_H);

	sys.GYRO_Y=BUF[3]*0X100+BUF[2];
	//sys.GYRO_Y=Data_Change(sys.GYRO_Y)/16.4;

//==============================================================

   BUF[4]=MSingle_Read(MPU6050_Addr,GYRO_ZOUT_L);
   BUF[5]=MSingle_Read(MPU6050_Addr,GYRO_ZOUT_H);
   sys.GYRO_Z=BUF[5]*0X100+BUF[4];
  // sys.GYRO_Z=Data_Change(sys.GYRO_Z)/16.4;

//==============================================================

	BUF[6]=MSingle_Read(MPU6050_Addr,TEMP_OUT_L);
	BUF[7]=MSingle_Read(MPU6050_Addr,TEMP_OUT_H);
	sys.T_T=((uint16_t)BUF[7]<<8)|BUF[6];
	sys.T_T=36.53+((float)sys.T_T)/340.0;
	//sys.T_T=Data_Change(sys.T_T);
*/
//==============================================================

	BUF[8]=MSingle_Read(MPU6050_Addr,ACCEL_XOUT_L);
	BUF[9]=MSingle_Read(MPU6050_Addr,ACCEL_XOUT_H);
	sys.ACCEL_X=BUF[9]*0X100+BUF[8];
	//sys.ACCEL_X=98*Data_Change(sys.ACCEL_X)/8192.0;

	BUF[10]=MSingle_Read(MPU6050_Addr,ACCEL_YOUT_L);
	BUF[11]=MSingle_Read(MPU6050_Addr,ACCEL_YOUT_H);
	sys.ACCEL_Y=BUF[11]*0X100+BUF[10];
	//sys.ACCEL_Y=98*Data_Change(sys.ACCEL_Y)/8192.0;

	BUF[12]=MSingle_Read(MPU6050_Addr,ACCEL_ZOUT_L);
	BUF[13]=MSingle_Read(MPU6050_Addr,ACCEL_ZOUT_H);

	sys.ACCEL_Z=BUF[13]*0X100+BUF[12];
	//sys.ACCEL_Z=98*Data_Change(sys.ACCEL_Z)/8192.0;

	//sys.SUM_ACCEL=sqrt(sys.ACCEL_X*sys.ACCEL_X + sys.ACCEL_Y*sys.ACCEL_Y + sys.ACCEL_Z*sys.ACCEL_Z);

}
