#include "stm32f4xx.h"

#include "MPU6050.h"
//#include "SDCard.h"

#include  <math.h>    //Keil library  //==========================================


unsigned char TBuff[6]={0x00,0x00,0x00,0x00,0x00,' '};//���ͻ�����
unsigned char ZhuanHua1[10]={'0','1','2','3','4','5','6','7','8','9'};//ת����ascii��
float GYRO_X,GYRO_Y,GYRO_Z,ACCEL_X,ACCEL_Y,ACCEL_Z,SUM_ACCEL;

unsigned char FUHAO=0;
unsigned char TX_DATA[4];  	 //��ʾ�ݻ�����
unsigned char BUF[30];       //�������ݻ�����
char  Mtest=0; 				 //IIC�õ�
float T_X,T_Y,T_Z,T_T,J_X,J_Y,J_Z;		 //X,Y,Z�ᣬ�¶�,X,Y,Z����ٶ�
unsigned char temp_bw=0,temp_sw=0,temp_gw=0;
unsigned char a_sw=0,a_gw=0;//���ٶ�
extern unsigned char KEY;

uint8_t ACC_DATA_X[5]={0XAA,0XAA,0X02,0X00,0X00};

extern uint8_t SDCardValid;

extern UART_HandleTypeDef huart2;

void MI2C_GPIO_Config(void)
{
  
	
	   GPIO_InitTypeDef GPIO_InitStruct;
	  __GPIOB_CLK_ENABLE();
	
	  /**SimI2C GPIO Configuration    
    PB10     ------> SimI2C_SCL
    PB3     ------> SimI2C_SDA 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_10;
    GPIO_InitStruct.Mode =GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

void MI2C_delay(void)
{
		
   uint8_t i=30; //��������Ż��ٶ�	����������͵�5����д��
   while(i) 
   { 
     i--; 
   }  
}
/*******************************************************************************
* Function Name  : I2C_Start
* Description    : Master Start Simulation IIC Communication
* Input          : None
* Output         : None
* Return         : Wheather	 Start
****************************************************************************** */
unsigned char MI2C_Start(void)
{
	MSDA_H;
	MSCL_H;
	MI2C_delay();
	if(!MSDA_read)return FALSE;	//SDA��Ϊ�͵�ƽ������æ,�˳�
	MSDA_L;
	MI2C_delay();
	if(MSDA_read) return FALSE;	//SDA��Ϊ�ߵ�ƽ�����߳���,�˳�
	MSDA_L;
	MI2C_delay();
	return TRUE;
}
/*******************************************************************************
* Function Name  : I2C_Stop
* Description    : Master Stop Simulation IIC Communication
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void MI2C_Stop(void)
{
	MSCL_L;
	MI2C_delay();
	MSDA_L;
	MI2C_delay();
	MSCL_H;
	MI2C_delay();
	MSDA_H;
	MI2C_delay();
} 
/*******************************************************************************
* Function Name  : I2C_Ack
* Description    : Master Send Acknowledge Single
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void MI2C_Ack(void)
{	
	MSCL_L;
	MI2C_delay();
	MSDA_L;
	MI2C_delay();
	MSCL_H;
	MI2C_delay();
	MSCL_L;
	MI2C_delay();
}   
/*******************************************************************************
* Function Name  : I2C_NoAck
* Description    : Master Send No Acknowledge Single
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void MI2C_NoAck(void)
{	
	MSCL_L;
	MI2C_delay();
	MSDA_H;
	MI2C_delay();
	MSCL_H;
	MI2C_delay();
	MSCL_L;
	MI2C_delay();
} 
/*******************************************************************************
* Function Name  : I2C_WaitAck
* Description    : Master Reserive Slave Acknowledge Single
* Input          : None
* Output         : None
* Return         : Wheather	 Reserive Slave Acknowledge Single
****************************************************************************** */
unsigned MI2C_WaitAck(void) 	 //����Ϊ:=1��ACK,=0��ACK
{
	MSCL_L;
	MI2C_delay();
	MSDA_H;			
	MI2C_delay();
	MSCL_H;
	MI2C_delay();
	if(MSDA_read)
	{
      MSCL_L;
	  MI2C_delay();
      return FALSE;
	}
	MSCL_L;
	MI2C_delay();
	return TRUE;
}

/*******************************************************************************
* Function Name  : I2C_SendByte
* Description    : Master Send a Byte to Slave
* Input          : Will Send Date
* Output         : None
* Return         : None
****************************************************************************** */
void MI2C_SendByte(uint8_t SendByte) //���ݴӸ�λ����λ//
{
    uint8_t i=8;
    while(i--)
    {
        MSCL_L;
        MI2C_delay();
      if(SendByte&0x80)
        MSDA_H;  
      else 
        MSDA_L;   
        SendByte<<=1;
        MI2C_delay();
		MSCL_H;
        MI2C_delay();
    }
    MSCL_L;
}  
/*******************************************************************************
* Function Name  : I2C_RadeByte
* Description    : Master Reserive a Byte From Slave
* Input          : None
* Output         : None
* Return         : Date From Slave 
****************************************************************************** */
unsigned char MI2C_RadeByte(void)  //���ݴӸ�λ����λ//
{ 
    uint8_t i=8;
    uint8_t ReceiveByte=0;

    MSDA_H;				
    while(i--)
    {
      ReceiveByte<<=1;      
      MSCL_L;
      MI2C_delay();
	  MSCL_H;
      MI2C_delay();	
      if(MSDA_read)
      {
        ReceiveByte|=0x01;
      }
    }
    MSCL_L;
    return ReceiveByte;
} 
//ZRX          
//���ֽ�д��*******************************************
//ZRX          
//���ֽ�д��*******************************************

unsigned char MSingle_Write(unsigned char SlaveAddress,unsigned char REG_Address,unsigned char REG_data)		     //void
{
  	if(!MI2C_Start())return FALSE;
    MI2C_SendByte(SlaveAddress);   //�����豸��ַ+д�ź�//MI2C_SendByte(((REG_Address & 0x0700) >>7) | SlaveAddress & 0xFFFE);//���ø���ʼ��ַ+������ַ 
    if(!MI2C_WaitAck()){MI2C_Stop(); return FALSE;}
    MI2C_SendByte(REG_Address );   //���õ���ʼ��ַ      
    MI2C_WaitAck();	
    MI2C_SendByte(REG_data);
    MI2C_WaitAck();   
    MI2C_Stop(); 

    return TRUE;
}

//���ֽڶ�ȡ*****************************************
unsigned char MSingle_Read(unsigned char SlaveAddress,unsigned char REG_Address)
{   unsigned char REG_data;     	
	if(!MI2C_Start())return FALSE;
    MI2C_SendByte(SlaveAddress); //MI2C_SendByte(((REG_Address & 0x0700) >>7) | REG_Address & 0xFFFE);//���ø���ʼ��ַ+������ַ 
    if(!MI2C_WaitAck()){MI2C_Stop();Mtest=1; return FALSE;}
    MI2C_SendByte((uint8_t) REG_Address);   //���õ���ʼ��ַ      
    MI2C_WaitAck();
    MI2C_Start();
    MI2C_SendByte(SlaveAddress+1);
    MI2C_WaitAck();

	  REG_data= MI2C_RadeByte();
    MI2C_NoAck();
    MI2C_Stop();
    //return TRUE;
	  return REG_data;

}		

unsigned int GetData(unsigned char REG_Address)
{
	char H,L;
	H=MSingle_Read(MPU6050_Addr,REG_Address);
	L=MSingle_Read(MPU6050_Addr,REG_Address);
	return ((H<<8)+L)/16.4;   //�ϳ�����
}
//��ʼ��MPU6050��������Ҫ��ο�pdf�����޸�************************
void Init_MPU6050(void)
{
/*
   Single_Write(MPU6050_Addr,PWR_M, 0x80);   //
   Single_Write(MPU6050_Addr,SMPL, 0x07);    //
   Single_Write(MPU6050_Addr,DLPF, 0x1E);    //��2000��
   Single_Write(MPU6050_Addr,INT_C, 0x00 );  //
   Single_Write(MPU6050_Addr,PWR_M, 0x00);   //
*/
  MSingle_Write(MPU6050_Addr,PWR_MGMT_1, 0x00);	//�������״̬
	MSingle_Write(MPU6050_Addr,SMPLRT_DIV, 0x07);
	MSingle_Write(MPU6050_Addr,CONFIG, 0x06);
	MSingle_Write(MPU6050_Addr,GYRO_CONFIG, 0x18);//ѡ��������������̡�2000��/S  ��������16.4LSB/��/S
	MSingle_Write(MPU6050_Addr,ACCEL_CONFIG, 0x09);//ѡ����ٶ���������̡�4g,������8192LSB/g
}

unsigned int Data_Change(unsigned int data)
{
    unsigned char ECG_Data_H,ECG_Data_L;
	  if(data>=0X8000&&data<=0XFFFF)//����Ϊ����
    { 
        data=~data+1;
			  FUHAO=1;
	  }
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-			
			  ECG_Data_H=(data&0XFF00)/0X100;  
        ECG_Data_L=data&0X00FF; 
			
			  data=ECG_Data_H*0x100+ECG_Data_L;//��֪��ΪʲôҪ�����⼸�䣬���ӾͲ���
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-				 	
    
    return data;
    
}

//******��ȡMPU6050����****************************************
void READ_MPU6050(void)
{
//******��ȡMPU6050�¶�����****************************************    
	  BUF[6]=MSingle_Read(MPU6050_Addr,TEMP_OUT_L); 
    BUF[7]=MSingle_Read(MPU6050_Addr,TEMP_OUT_H); 
    T_T=((uint16_t)BUF[7]<<8)|BUF[6];
	  T_T=36.53+((double)T_T)/340;
	  printf("MPU6050Temputer=%.2f��  ",T_T);
	 
	//******��ȡMPU6050�ϼ��ٶ�����****************************************
	  BUF[8]=MSingle_Read(MPU6050_Addr,ACCEL_XOUT_L);
    BUF[9]=MSingle_Read(MPU6050_Addr,ACCEL_XOUT_H);
    ACCEL_X=(BUF[9]<<8)+BUF[8];
	  ACCEL_X=10*Data_Change(ACCEL_X)/8192;
	
		
	  BUF[10]=MSingle_Read(MPU6050_Addr,ACCEL_YOUT_L);
    BUF[11]=MSingle_Read(MPU6050_Addr,ACCEL_YOUT_H);
    ACCEL_Y=(BUF[11]<<8)+BUF[10];
	  ACCEL_Y=10*Data_Change(ACCEL_Y)/8192;

	 BUF[12]=MSingle_Read(MPU6050_Addr,ACCEL_ZOUT_L);
   BUF[13]=MSingle_Read(MPU6050_Addr,ACCEL_ZOUT_H);  
	 ACCEL_Z=(BUF[13]<<8)+BUF[12];
	 ACCEL_Z=10*Data_Change(ACCEL_Z)/8192;	
	 
	 SUM_ACCEL=sqrt(ACCEL_X*ACCEL_X + ACCEL_Y*ACCEL_Y + ACCEL_Z*ACCEL_Z);
	 
	 //SUM_ACCEL=11;
	 printf("ACCEL=%fg\r\n\r\n",SUM_ACCEL/9);
   
}
void TEST_READ_MPU6050(void)
{
   BUF[0]=MSingle_Read(MPU6050_Addr,GYRO_XOUT_L); 
   BUF[1]=MSingle_Read(MPU6050_Addr,GYRO_XOUT_H);
	 GYRO_X=BUF[1]*0X100+BUF[0];
	 GYRO_X=Data_Change(GYRO_X)/16.4;	
	 printf("\r\nX=%.2f ",GYRO_X);
   
//==============================================================
   BUF[2]=MSingle_Read(MPU6050_Addr,GYRO_YOUT_L);
   BUF[3]=MSingle_Read(MPU6050_Addr,GYRO_YOUT_H);
	
	 GYRO_Y=BUF[3]*0X100+BUF[2];
	 GYRO_Y=Data_Change(GYRO_Y)/16.4;	
	printf("Y=%.2f ",GYRO_Y);
	
//==============================================================

   BUF[4]=MSingle_Read(MPU6050_Addr,GYRO_ZOUT_L);
   BUF[5]=MSingle_Read(MPU6050_Addr,GYRO_ZOUT_H);
	 GYRO_Z=BUF[5]*0X100+BUF[4];
	 GYRO_Z=Data_Change(GYRO_Z)/16.4;	
	printf("Z=%.2f ",GYRO_Z);
	 
//==============================================================

   BUF[6]=MSingle_Read(MPU6050_Addr,TEMP_OUT_L); 
   BUF[7]=MSingle_Read(MPU6050_Addr,TEMP_OUT_H); 
   T_T=((uint16_t)BUF[7]<<8)|BUF[6];
	 T_T=36.53+((double)T_T)/340;
	 T_T=Data_Change(T_T);	
	 printf("T=%.2f ",T_T);
  
//==============================================================
	
	 BUF[8]=MSingle_Read(MPU6050_Addr,ACCEL_XOUT_L);
   BUF[9]=MSingle_Read(MPU6050_Addr,ACCEL_XOUT_H);
   ACCEL_X=BUF[9]*0X100+BUF[8];
	 ACCEL_X=98*Data_Change(ACCEL_X)/8192;	
   printf("x=%.2f ",ACCEL_X);
	 
	 BUF[10]=MSingle_Read(MPU6050_Addr,ACCEL_YOUT_L);
   BUF[11]=MSingle_Read(MPU6050_Addr,ACCEL_YOUT_H);
   ACCEL_Y=BUF[11]*0X100+BUF[10];
	 ACCEL_Y=98*Data_Change(ACCEL_Y)/8192;
   printf("y=%.2f ",ACCEL_Y);	 
	 
	 BUF[12]=MSingle_Read(MPU6050_Addr,ACCEL_ZOUT_L);
   BUF[13]=MSingle_Read(MPU6050_Addr,ACCEL_ZOUT_H);
   
	 ACCEL_Z=BUF[13]*0X100+BUF[12];
	 ACCEL_Z=98*Data_Change(ACCEL_Z)/8192;	
	 printf("z=%.2f ",ACCEL_Z);
   
	 SUM_ACCEL=sqrt(ACCEL_X*ACCEL_X + ACCEL_Y*ACCEL_Y + ACCEL_Z*ACCEL_Z);
	 printf("S=%.2f\r\n",SUM_ACCEL);

}

void GET_ACC(void)
{
		
	  Init_MPU6050();		     //��ʼ��MPU6050
	
	  BUF[8]=MSingle_Read(MPU6050_Addr,ACCEL_XOUT_L);
    BUF[9]=MSingle_Read(MPU6050_Addr,ACCEL_XOUT_H);
    ACCEL_X=BUF[9]*0X100+BUF[8];
	  ACCEL_X=Data_Change(ACCEL_X)/8192;	
	  //printf("x=%.2f ",ACCEL_X);
	
	  BUF[10]=MSingle_Read(MPU6050_Addr,ACCEL_YOUT_L);
    BUF[11]=MSingle_Read(MPU6050_Addr,ACCEL_YOUT_H);
    ACCEL_Y=BUF[11]*0X100+BUF[10];
	  ACCEL_Y=Data_Change(ACCEL_Y)/8192;
	  //printf("y=%.2f ",ACCEL_Y);	 
	
	  BUF[12]=MSingle_Read(MPU6050_Addr,ACCEL_ZOUT_L);
    BUF[13]=MSingle_Read(MPU6050_Addr,ACCEL_ZOUT_H);
	  ACCEL_Z=BUF[13]*0X100+BUF[12];
	  ACCEL_Z=Data_Change(ACCEL_Z)/8192;	
    //printf("z=%.2f ",ACCEL_Z);
	
	  SUM_ACCEL=10*sqrt(ACCEL_X*ACCEL_X + ACCEL_Y*ACCEL_Y + ACCEL_Z*ACCEL_Z);
		//printf("S=%.2f\r\n",SUM_ACCEL);
		ACC_DATA_X[3]=SUM_ACCEL;
		HAL_UART_Transmit_DMA(&huart2 , ACC_DATA_X, 5);
		
		/*************** �����ٶ����ݴ���SD��������******************/
//		if(SDCardValid==1)
//			Write_Data_To_SDBuffer(ACC_DATA_X, 5); 
}


