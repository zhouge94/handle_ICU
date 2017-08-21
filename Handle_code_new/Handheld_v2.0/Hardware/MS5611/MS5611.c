#include "stm32f4xx.h"
#include "ms5611.h"

#include "oled.h"

uint8_t exchange_Pres_num[8];//ѹ������
uint8_t exchange_Temp_num[8];//�¶�����

uint16_t Cal_C[7];  //���ڴ��PROM�е�8������
uint32_t D1_Pres,D2_Temp; // �������ѹ�����¶�
float Pressure;				//�¶Ȳ�������ѹ
float dT,Temperature1,Temperature2;//ʵ�ʺͲο��¶�֮��Ĳ���,ʵ���¶�,�м�ֵ
double OFF,SENS;  //ʵ���¶ȵ���,ʵ���¶�������

uint32_t ex_Pressure;	//���ڶ���ת��ֵ

extern unsigned char KEY;
extern UART_HandleTypeDef huart2;
uint8_t PRE_DATA_X[7]={0XAA,0XAA,0X01,0X00,0X00,0X00,0X00};

extern void DisplayAP(uint16_t color,uint16_t backgroundcolor);

void delay_usm(uint32_t n)
{
	uint8_t j;
	while(n--)
	for(j=0;j<10;j++);
}

void delay_msm(uint32_t n)
{
	while(n--)
	delay_usm(1000);
}
/**************************ʵ�ֺ���********************************************
*����ԭ��:		void IIC_Init(void)
*��������:		��ʼ��I2C��Ӧ�Ľӿ����š�
*******************************************************************************/
void IIC_GPIO_Config(void)
{			
   GPIO_InitTypeDef GPIO_InitStruct;
	  __GPIOB_CLK_ENABLE();
	
	  /**SimI2C GPIO Configuration    
    PB6     ------> SimI2C_SCL
    PB7     ------> SimI2C_SDA 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode =GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	IIC_SCL_Set();
  IIC_SDA_Set();  
	//IIC_SCL=1;
	//IIC_SDA=1;
}


/**************************ʵ�ֺ���********************************************
*����ԭ��:		void IIC_Start(void)
*��������:		����IIC��ʼ�ź�
*******************************************************************************/
void IIC_Start(void)
{
    SDA_OUT();     //sda�����
	  SDA_OUT();     //sda�����
	  
    IIC_SDA_Set();
	  IIC_SCL_Set();
	  delay_usm(4);
 	  IIC_SDA_Clr();//START:when CLK is high,DATA change form high to low 
	  delay_usm(4);
	  IIC_SCL_Clr();//ǯסI2C���ߣ�׼�����ͻ�������� 
}
/**************************ʵ�ֺ���********************************************
*����ԭ��:		void IIC_Stop(void)
*��������:	    //����IICֹͣ�ź�
*******************************************************************************/	  
void IIC_Stop(void)
{
	SDA_OUT();//sda�����
	IIC_SCL_Clr();
	IIC_SDA_Clr();//STOP:when CLK is high DATA change form low to high	   ֹͣ��CLK��������
 	delay_usm(10);
	IIC_SCL_Set();
 	delay_usm(10);
	IIC_SDA_Set();//����I2C���߽����ź�
	delay_usm(10);							   	
}
/**************************ʵ�ֺ���********************************************
*����ԭ��:		uint8_t IIC_Wait_Ack(void)
*��������:	    �ȴ�Ӧ���źŵ��� 
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
*******************************************************************************/
uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	IIC_SDA_Set();
	SDA_IN();      //SDA����Ϊ����  
	delay_usm(10);	   
	IIC_SCL_Set();
	delay_usm(10);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>50)
		{
			IIC_Stop();
			return 1;
		}
	  delay_usm(10);
	}
	IIC_SCL_Clr();//ʱ�����0 	   
	return 0;  
} 
/**************************ʵ�ֺ���********************************************
*����ԭ��:		void IIC_Ack(void)
*��������:	    ����ACKӦ��
*******************************************************************************/
void IIC_Ack(void)
{
	SDA_OUT();
	IIC_SCL_Clr();
	IIC_SDA_Clr();
	delay_usm(10);
	IIC_SCL_Set();;
	delay_usm(10);
	IIC_SCL_Clr();
}
/**************************ʵ�ֺ���********************************************
*����ԭ��:		void IIC_NAck(void)
*��������:	    ����NACKӦ��
*******************************************************************************/	    
void IIC_NAck(void)
{
	SDA_OUT();
	IIC_SCL_Clr();
	IIC_SDA_Set();
	delay_usm(10);
	IIC_SCL_Set();
	delay_usm(10);
	IIC_SCL_Clr();
}					 				     
/**************************ʵ�ֺ���********************************************
*����ԭ��:		void IIC_Send_Byte(uint8_t txd)
*��������:	    IIC����һ���ֽ�
*******************************************************************************/		  
void IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	SDA_OUT(); 	    
    IIC_SCL_Clr();//����ʱ�ӿ�ʼ���ݴ���
	delay_usm(10);   
    for(t=0;t<8;t++)
    {   
		if((txd&0x80) == 0x80)
		{
			IIC_SDA_Set();
		}else
		{
			IIC_SDA_Clr();	
		}
        txd<<=1; 	  
		delay_usm(10);   
		IIC_SCL_Set();
		delay_usm(10); 
		IIC_SCL_Clr();	
		delay_usm(10);
    }	 
} 	 
/**************************ʵ�ֺ���********************************************
*����ԭ��:		uint8_t IIC_Read_Byte(unsigned char ack)
*��������:	    //��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK 
*******************************************************************************/  
uint8_t IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0x00;
	SDA_IN();//SDA����Ϊ����
    for(i=0;i<8;i++ )
	{
 		IIC_SCL_Set();
		delay_usm(10);
        receive<<=1;
        if(READ_SDA == 1)
		{
			receive += 0x01; 
		}			
		delay_usm(10); 
        IIC_SCL_Clr();	
		delay_usm(10); 
    }					 
    if (ack)
	{
        IIC_Ack(); //����ACK
	}else
	{
		IIC_NAck();//����nACK  
	}
    return receive;
} 
/**************************ʵ�ֺ���********************************************
*����ԭ��:		unsigned char I2C_ReadOneByte(unsigned char rI2C_Add,unsigned char addr)
*��������:	    ��ȡָ���豸 ָ���Ĵ�����һ��ֵ
����	I2C_Addr  Ŀ���豸��ַ
		addr	   �Ĵ�����ַ
����   ��������ֵ
*******************************************************************************/ 
unsigned char I2C_ReadOneByte(unsigned char I2C_Addr,unsigned char addr)
{
	unsigned char res=0;
	
	IIC_Start();	
	IIC_Send_Byte(I2C_Addr);	   //����д����
	res++;
	IIC_Wait_Ack();
	IIC_Send_Byte(addr); res++;  //���͵�ַ
	IIC_Wait_Ack();	  
	IIC_Stop();//����һ��ֹͣ����	
	IIC_Start();
	IIC_Send_Byte(I2C_Addr+1); res++;          //�������ģʽ			   
	IIC_Wait_Ack();
	res=IIC_Read_Byte(0);	   
    IIC_Stop();//����һ��ֹͣ����

	return res;
}
/**************************ʵ�ֺ���********************************************
*����ԭ��:		uint8_t IICreadBytes(uint8_t dev, uint8_t reg, uint8_t length, uint8_t *data)
*��������:	    ��ȡָ���豸 ָ���Ĵ����� length��ֵ
����	dev  Ŀ���豸��ַ
		reg	  �Ĵ�����ַ
		length Ҫ�����ֽ���
		*data  ���������ݽ�Ҫ��ŵ�ָ��
����   ���������ֽ�����
*******************************************************************************/ 
uint8_t IICreadBytes(uint8_t dev, uint8_t reg, uint8_t length, uint8_t *data){
    uint8_t count = 0;
	
	IIC_Start();
	IIC_Send_Byte(dev);	   //����д����
	IIC_Wait_Ack();
	IIC_Send_Byte(reg);   //���͵�ַ
    IIC_Wait_Ack();	  
	IIC_Start();
	IIC_Send_Byte(dev+1);  //�������ģʽ	
	IIC_Wait_Ack();
	
    for(count=0;count<length;count++){
		 
		 if(count!=length-1)data[count]=IIC_Read_Byte(1);  //��ACK�Ķ�����
		 	else  data[count]=IIC_Read_Byte(0);	 //���һ���ֽ�NACK
	}
    IIC_Stop();//����һ��ֹͣ����
    return count;
}

/**************************ʵ�ֺ���********************************************
*����ԭ��:		uint8_t IICwriteBytes(uint8_t dev, uint8_t reg, uint8_t length, uint8_t* data)
*��������:	    ������ֽ�д��ָ���豸 ָ���Ĵ���
����	dev  Ŀ���豸��ַ
		reg	  �Ĵ�����ַ
		length Ҫд���ֽ���
		*data  ��Ҫд�����ݵ��׵�ַ
����   �����Ƿ�ɹ�
*******************************************************************************/ 
uint8_t IICwriteBytes(uint8_t dev, uint8_t reg, uint8_t length, uint8_t* data){
  
 	uint8_t count = 0;
	IIC_Start();
	IIC_Send_Byte(dev);	   //����д����
	IIC_Wait_Ack();
	IIC_Send_Byte(reg);   //���͵�ַ
    IIC_Wait_Ack();	  
	for(count=0;count<length;count++){
		IIC_Send_Byte(data[count]); 
		IIC_Wait_Ack(); 
	 }
	IIC_Stop();//����һ��ֹͣ����

    return 1; //status == 0;
}
/**************************ʵ�ֺ���********************************************
*����ԭ��:		uint8_t IICreadByte(uint8_t dev, uint8_t reg, uint8_t *data)
*��������:	    ��ȡָ���豸 ָ���Ĵ�����һ��ֵ
����	dev  Ŀ���豸��ַ
		reg	   �Ĵ�����ַ
		*data  ���������ݽ�Ҫ��ŵĵ�ַ
����   1
*******************************************************************************/ 
uint8_t IICreadByte(uint8_t dev, uint8_t reg, uint8_t *data){
	*data=I2C_ReadOneByte(dev, reg);
    return 1;
}

/**************************ʵ�ֺ���********************************************
*����ԭ��:		unsigned char IICwriteByte(unsigned char dev, unsigned char reg, unsigned char data)
*��������:	    д��ָ���豸 ָ���Ĵ���һ���ֽ�
����	dev  Ŀ���豸��ַ
		reg	   �Ĵ�����ַ
		data  ��Ҫд����ֽ�
����   1
*******************************************************************************/ 
unsigned char IICwriteByte(unsigned char dev, unsigned char reg, unsigned char data){
    return IICwriteBytes(dev, reg, 1, &data);
}

/**************************ʵ�ֺ���********************************************
*����ԭ��:		uint8_t IICwriteBits(uint8_t dev,uint8_t reg,uint8_t bitStart,uint8_t length,uint8_t data)
*��������:	    �� �޸� д ָ���豸 ָ���Ĵ���һ���ֽ� �еĶ��λ
����	dev  Ŀ���豸��ַ
		reg	   �Ĵ�����ַ
		bitStart  Ŀ���ֽڵ���ʼλ
		length   λ����
		data    ��Ÿı�Ŀ���ֽ�λ��ֵ
����   �ɹ� Ϊ1 
 		ʧ��Ϊ0
*******************************************************************************/ 
uint8_t IICwriteBits(uint8_t dev,uint8_t reg,uint8_t bitStart,uint8_t length,uint8_t data)
{

    uint8_t b;
    if (IICreadByte(dev, reg, &b) != 0) {
        uint8_t mask = (0xFF << (bitStart + 1)) | 0xFF >> ((8 - bitStart) + length - 1);
        data <<= (8 - length);
        data >>= (7 - bitStart);
        b &= mask;
        b |= data;
        return IICwriteByte(dev, reg, b);
    } else {
        return 0;
    }
}

/**************************ʵ�ֺ���********************************************
*����ԭ��:		uint8_t IICwriteBit(uint8_t dev, uint8_t reg, uint8_t bitNum, uint8_t data)
*��������:	    �� �޸� д ָ���豸 ָ���Ĵ���һ���ֽ� �е�1��λ
����	dev  Ŀ���豸��ַ
		reg	   �Ĵ�����ַ
		bitNum  Ҫ�޸�Ŀ���ֽڵ�bitNumλ
		data  Ϊ0 ʱ��Ŀ��λ������0 ���򽫱���λ
����   �ɹ� Ϊ1 
 		ʧ��Ϊ0
*******************************************************************************/ 
uint8_t IICwriteBit(uint8_t dev, uint8_t reg, uint8_t bitNum, uint8_t data){
    uint8_t b;
    IICreadByte(dev, reg, &b);
    b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
    return IICwriteByte(dev, reg, b);
}


//=========================================================
//******MS561101BA����********
//=========================================================

void MS561101BA_RESET(void)//�����ݶ���������������ܲ���ȥ����ע�͵��˴�while(IIC_Wait_Ack());
{	
	IIC_Start();
	IIC_Send_Byte(MS561101BA_SlaveAddress);
	while(IIC_Wait_Ack());
	delay_usm(100);
	IIC_Send_Byte(MS561101BA_RST);
	while(IIC_Wait_Ack());
	delay_usm(100);
	IIC_Stop();
}
//��PROM��ȡ����У׼����
void MS561101BA_PROM_READ(void)
{
	uint16_t d1,d2;
	uint8_t i;
	for(i=0;i<=6;i++)
	{
		IIC_Start();
		IIC_Send_Byte(MS561101BA_SlaveAddress);
		while(IIC_Wait_Ack());
		IIC_Send_Byte((MS561101BA_PROM_RD+i*2));
		while(IIC_Wait_Ack());

		IIC_Start();
		IIC_Send_Byte(MS561101BA_SlaveAddress+0x01);
		while(IIC_Wait_Ack());
		d1=IIC_Read_Byte(1);
		d2=IIC_Read_Byte(0);
		IIC_Stop();

		delay_msm(10);

		Cal_C[i]=(d1<<8)|d2;
	}//��ӡPROM��ȡ����У׼���ݣ�������ݴ����Ƿ�����
//	printf("C1 =%d\n",Cal_C[1]);
//	printf("C2 =%d\n",Cal_C[2]);
//	printf("C3 =%d\n",Cal_C[3]);
//	printf("C4 =%d\n",Cal_C[4]);
//	printf("C5 =%d\n",Cal_C[5]);
//	printf("C6 =%d\n",Cal_C[6]);
}		 
uint32_t MS561101BA_DO_CONVERSION(uint8_t command)
{
	uint32_t conversion=0;
	uint32_t conv1,conv2,conv3; 
	IIC_Start();
	IIC_Send_Byte(MS561101BA_SlaveAddress);
	while(IIC_Wait_Ack());
	IIC_Send_Byte(command);	
	while(IIC_Wait_Ack());
	IIC_Stop();

	delay_msm(20);	

	IIC_Start();
	IIC_Send_Byte(MS561101BA_SlaveAddress);
	while(IIC_Wait_Ack());
	IIC_Send_Byte(0x00);
	while(IIC_Wait_Ack());
	IIC_Stop();

	IIC_Start();
	IIC_Send_Byte(MS561101BA_SlaveAddress+1);
	while(IIC_Wait_Ack());
	conv1=IIC_Read_Byte(1);
	conv2=IIC_Read_Byte(1);
	conv3=IIC_Read_Byte(0);
	IIC_Stop();

	conversion= (conv1<<16) + (conv2<<8) + conv3;

	return conversion;
}
//��ȡ�����¶�
void MS561101BA_GetTemperature(uint8_t OSR_Temp)
{
    
	D2_Temp= MS561101BA_DO_CONVERSION(OSR_Temp);	
	delay_msm(10);
	
	dT=D2_Temp - (((uint32_t)Cal_C[5])<<8);
	Temperature1=2000+dT*((uint32_t)Cal_C[6])/8388608;//

}
//��ȡ������ѹ
void MS561101BA_GetPressure(uint8_t OSR_Pres)
{
	float Aux,OFF2,SENS2;  //�¶�У��ֵ	
	
	D1_Pres= MS561101BA_DO_CONVERSION(OSR_Pres);
	delay_msm(2); 
	
	OFF=(uint32_t)(Cal_C[2]<<16)+((uint32_t)Cal_C[4]*dT)/128;
	SENS=(uint32_t)(Cal_C[1]<<15)+((uint32_t)Cal_C[3]*dT)/256;
	//�¶Ȳ���
	if(Temperature1 < 2000)// second order temperature compensation when under 20 degrees C
	{
		Temperature2 = (dT*dT) / 0x80000000;
		Aux = (Temperature1-2000)*(Temperature1-2000);
		OFF2 = 2.5*Aux ;//�˴�����f�о��棬������Ĭ��ת��Ϊ˫����
		SENS2 = 1.25*Aux;//�˴�����f�о��棬������Ĭ��ת��Ϊ˫����
		if(Temperature1 < -1500)
		{
			Aux = (Temperature1+1500)*(Temperature1+1500);
			OFF2 = OFF2 + 7*Aux;
			SENS2 = SENS + 5.5*Aux;//�˴�����f�о��棬������Ĭ��ת��Ϊ˫����
		}
	}else  //(Temperature > 2000)
	{
		Temperature2 = 0;
		OFF2 = 0;
		SENS2 = 0;
	}
	
	Temperature1 = Temperature1 - Temperature2;
	OFF = OFF - OFF2;
	SENS = SENS - SENS2;	

	Pressure=(D1_Pres*SENS/2097152-OFF)/32768;
}

void MS561101BA_Init(void)
{
	MS561101BA_RESET();
	delay_msm(5);
	MS561101BA_PROM_READ();
	delay_msm(5);
} 

void read_MS5611_temputer(void)
{
    uint32_t ex_Temperature; //���ڶ���ת��ֵ
    MS561101BA_GetTemperature(MS561101BA_D2_OSR_4096);
  
	  ex_Temperature=(long)(Temperature1);
	
	  exchange_Temp_num[3]=ex_Temperature/1000%10+'0';//�¶�ʮλ
	  exchange_Temp_num[4]=ex_Temperature/100%10+'0';//�¶ȸ�λ
	  //exchange_Temp_num[5]='.';
	  exchange_Temp_num[6]=ex_Temperature/10%10+'0'; //�¶�С�����һλ
	
		
	  	
}
void READ_MS5611_Pressure(void)
{
    uint32_t ex_Pressure;	//���ڶ���ת��ֵ

	  MS561101BA_GetPressure(MS561101BA_D1_OSR_4096);
	
	  ex_Pressure=(long)(Pressure); 
	  ex_Pressure=ex_Pressure+3180;
	  ex_Pressure=ex_Pressure/100;
	
	  exchange_Pres_num[0]=ex_Pressure/1000+'0';//��ѹʮ��λ
	  exchange_Pres_num[1]=ex_Pressure/100%10+'0';//��ѹ��λ
	  exchange_Pres_num[2]=ex_Pressure/10%10+'0';//��ѹǧλ
	  exchange_Pres_num[3]=ex_Pressure%10+'0';//��ѹ��λ
	  
	  
}
//***************����ת��******************* 
void Exchange_Number(void) 
{ 
	uint32_t ex_Pressure;	//���ڶ���ת��ֵ
	uint32_t ex_Temperature; //���ڶ���ת��ֵ

	MS561101BA_GetTemperature(MS561101BA_D2_OSR_4096);
	MS561101BA_GetPressure(MS561101BA_D1_OSR_4096);
	
	//ex_Pressure=(long)(100.00); 
	ex_Pressure=(long)(Pressure); 
	
//	exchange_Pres_num[0]=ex_Pressure/1000000+'0';
//	exchange_Pres_num[1]=ex_Pressure/100000%10+'0';
//	exchange_Pres_num[2]=ex_Pressure/10000%10+'0';
//	exchange_Pres_num[3]=ex_Pressure/1000%10+'0';
//	exchange_Pres_num[4]=ex_Pressure/100%10+'0';
//	exchange_Pres_num[5]='.';
//	exchange_Pres_num[6]=ex_Pressure/10%10+'0'; 
//	exchange_Pres_num[7]=ex_Pressure%10+'0'; 
	//printf("\r\nMPU6050������ѹ:%d  Pa\r\n",ex_Pressure*100); 
  printf("\r\n----------MS5611  ������ѹ---------%d Pa\r\n",ex_Pressure+3180);
 	
	//ex_Temperature=(long)(26.2);
	ex_Temperature=(long)(Temperature1);
	
//	exchange_Temp_num[0]=ex_Temperature/1000000+'0';
//	exchange_Temp_num[1]=ex_Temperature/100000%10+'0';
//	exchange_Temp_num[2]=ex_Temperature/10000%10+'0';
//	exchange_Temp_num[3]=ex_Temperature/1000%10+'0';
//	exchange_Temp_num[4]=ex_Temperature/100%10+'0';
//	exchange_Temp_num[5]='.';
//	exchange_Temp_num[6]=ex_Temperature/10%10+'0'; 
//	exchange_Temp_num[7]=ex_Temperature%10+'0'; 
  printf("\r\n----------MS5611  �����¶�---------%d ��\r\n",ex_Temperature);
	//printf("\r\n----------MPU6050  �����¶�---------\n");
	//Usart_Send(exchange_Temp_num,8);
	printf("\r\n");
} 

void GET_Pressure(void)
{
	
	uint32_t ex_Pressure_sum=0;	//���ڶ���ת��ֵ
	uint32_t ex_Temperature; //���ڶ���ת��ֵ
	uint8_t i;
	
//	IIC_GPIO_Config(); 
//	MS561101BA_Init(); //MS561101BA��ʼ��
	//Exchange_Number();
	
  for(i=0;i<5;i++)
	{
	     MS561101BA_GetTemperature(MS561101BA_D2_OSR_4096);
	     MS561101BA_GetPressure(MS561101BA_D1_OSR_4096);
		   ex_Pressure_sum=ex_Pressure_sum+(long)(Pressure);
	}
	
	ex_Pressure=(ex_Pressure_sum/5+3180)&0X00FFFFFF; 
	
	//ex_Pressure=0X123456;//���Է�������
	
  DisplayAP(Green,Black);
	
	//printf("\r\n----------MS5611  ������ѹ---------%d Pa\r\n",ex_Pressure);
}