#include "stm32f4xx.h"
#include "ms5611.h"

#include "oled.h"

uint8_t exchange_Pres_num[8];//压力数据
uint8_t exchange_Temp_num[8];//温度数据

uint16_t Cal_C[7];  //用于存放PROM中的8组数据
uint32_t D1_Pres,D2_Temp; // 存放数字压力和温度
float Pressure;				//温度补偿大气压
float dT,Temperature1,Temperature2;//实际和参考温度之间的差异,实际温度,中间值
double OFF,SENS;  //实际温度抵消,实际温度灵敏度

uint32_t ex_Pressure;	//串口读数转换值

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
/**************************实现函数********************************************
*函数原型:		void IIC_Init(void)
*功　　能:		初始化I2C对应的接口引脚。
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


/**************************实现函数********************************************
*函数原型:		void IIC_Start(void)
*功　　能:		产生IIC起始信号
*******************************************************************************/
void IIC_Start(void)
{
    SDA_OUT();     //sda线输出
	  SDA_OUT();     //sda线输出
	  
    IIC_SDA_Set();
	  IIC_SCL_Set();
	  delay_usm(4);
 	  IIC_SDA_Clr();//START:when CLK is high,DATA change form high to low 
	  delay_usm(4);
	  IIC_SCL_Clr();//钳住I2C总线，准备发送或接收数据 
}
/**************************实现函数********************************************
*函数原型:		void IIC_Stop(void)
*功　　能:	    //产生IIC停止信号
*******************************************************************************/	  
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	IIC_SCL_Clr();
	IIC_SDA_Clr();//STOP:when CLK is high DATA change form low to high	   停止：CLK的上升沿
 	delay_usm(10);
	IIC_SCL_Set();
 	delay_usm(10);
	IIC_SDA_Set();//发送I2C总线结束信号
	delay_usm(10);							   	
}
/**************************实现函数********************************************
*函数原型:		uint8_t IIC_Wait_Ack(void)
*功　　能:	    等待应答信号到来 
//返回值：1，接收应答失败
//        0，接收应答成功
*******************************************************************************/
uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	IIC_SDA_Set();
	SDA_IN();      //SDA设置为输入  
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
	IIC_SCL_Clr();//时钟输出0 	   
	return 0;  
} 
/**************************实现函数********************************************
*函数原型:		void IIC_Ack(void)
*功　　能:	    产生ACK应答
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
/**************************实现函数********************************************
*函数原型:		void IIC_NAck(void)
*功　　能:	    产生NACK应答
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
/**************************实现函数********************************************
*函数原型:		void IIC_Send_Byte(uint8_t txd)
*功　　能:	    IIC发送一个字节
*******************************************************************************/		  
void IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	SDA_OUT(); 	    
    IIC_SCL_Clr();//拉低时钟开始数据传输
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
/**************************实现函数********************************************
*函数原型:		uint8_t IIC_Read_Byte(unsigned char ack)
*功　　能:	    //读1个字节，ack=1时，发送ACK，ack=0，发送nACK 
*******************************************************************************/  
uint8_t IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0x00;
	SDA_IN();//SDA设置为输入
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
        IIC_Ack(); //发送ACK
	}else
	{
		IIC_NAck();//发送nACK  
	}
    return receive;
} 
/**************************实现函数********************************************
*函数原型:		unsigned char I2C_ReadOneByte(unsigned char rI2C_Add,unsigned char addr)
*功　　能:	    读取指定设备 指定寄存器的一个值
输入	I2C_Addr  目标设备地址
		addr	   寄存器地址
返回   读出来的值
*******************************************************************************/ 
unsigned char I2C_ReadOneByte(unsigned char I2C_Addr,unsigned char addr)
{
	unsigned char res=0;
	
	IIC_Start();	
	IIC_Send_Byte(I2C_Addr);	   //发送写命令
	res++;
	IIC_Wait_Ack();
	IIC_Send_Byte(addr); res++;  //发送地址
	IIC_Wait_Ack();	  
	IIC_Stop();//产生一个停止条件	
	IIC_Start();
	IIC_Send_Byte(I2C_Addr+1); res++;          //进入接收模式			   
	IIC_Wait_Ack();
	res=IIC_Read_Byte(0);	   
    IIC_Stop();//产生一个停止条件

	return res;
}
/**************************实现函数********************************************
*函数原型:		uint8_t IICreadBytes(uint8_t dev, uint8_t reg, uint8_t length, uint8_t *data)
*功　　能:	    读取指定设备 指定寄存器的 length个值
输入	dev  目标设备地址
		reg	  寄存器地址
		length 要读的字节数
		*data  读出的数据将要存放的指针
返回   读出来的字节数量
*******************************************************************************/ 
uint8_t IICreadBytes(uint8_t dev, uint8_t reg, uint8_t length, uint8_t *data){
    uint8_t count = 0;
	
	IIC_Start();
	IIC_Send_Byte(dev);	   //发送写命令
	IIC_Wait_Ack();
	IIC_Send_Byte(reg);   //发送地址
    IIC_Wait_Ack();	  
	IIC_Start();
	IIC_Send_Byte(dev+1);  //进入接收模式	
	IIC_Wait_Ack();
	
    for(count=0;count<length;count++){
		 
		 if(count!=length-1)data[count]=IIC_Read_Byte(1);  //带ACK的读数据
		 	else  data[count]=IIC_Read_Byte(0);	 //最后一个字节NACK
	}
    IIC_Stop();//产生一个停止条件
    return count;
}

/**************************实现函数********************************************
*函数原型:		uint8_t IICwriteBytes(uint8_t dev, uint8_t reg, uint8_t length, uint8_t* data)
*功　　能:	    将多个字节写入指定设备 指定寄存器
输入	dev  目标设备地址
		reg	  寄存器地址
		length 要写的字节数
		*data  将要写的数据的首地址
返回   返回是否成功
*******************************************************************************/ 
uint8_t IICwriteBytes(uint8_t dev, uint8_t reg, uint8_t length, uint8_t* data){
  
 	uint8_t count = 0;
	IIC_Start();
	IIC_Send_Byte(dev);	   //发送写命令
	IIC_Wait_Ack();
	IIC_Send_Byte(reg);   //发送地址
    IIC_Wait_Ack();	  
	for(count=0;count<length;count++){
		IIC_Send_Byte(data[count]); 
		IIC_Wait_Ack(); 
	 }
	IIC_Stop();//产生一个停止条件

    return 1; //status == 0;
}
/**************************实现函数********************************************
*函数原型:		uint8_t IICreadByte(uint8_t dev, uint8_t reg, uint8_t *data)
*功　　能:	    读取指定设备 指定寄存器的一个值
输入	dev  目标设备地址
		reg	   寄存器地址
		*data  读出的数据将要存放的地址
返回   1
*******************************************************************************/ 
uint8_t IICreadByte(uint8_t dev, uint8_t reg, uint8_t *data){
	*data=I2C_ReadOneByte(dev, reg);
    return 1;
}

/**************************实现函数********************************************
*函数原型:		unsigned char IICwriteByte(unsigned char dev, unsigned char reg, unsigned char data)
*功　　能:	    写入指定设备 指定寄存器一个字节
输入	dev  目标设备地址
		reg	   寄存器地址
		data  将要写入的字节
返回   1
*******************************************************************************/ 
unsigned char IICwriteByte(unsigned char dev, unsigned char reg, unsigned char data){
    return IICwriteBytes(dev, reg, 1, &data);
}

/**************************实现函数********************************************
*函数原型:		uint8_t IICwriteBits(uint8_t dev,uint8_t reg,uint8_t bitStart,uint8_t length,uint8_t data)
*功　　能:	    读 修改 写 指定设备 指定寄存器一个字节 中的多个位
输入	dev  目标设备地址
		reg	   寄存器地址
		bitStart  目标字节的起始位
		length   位长度
		data    存放改变目标字节位的值
返回   成功 为1 
 		失败为0
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

/**************************实现函数********************************************
*函数原型:		uint8_t IICwriteBit(uint8_t dev, uint8_t reg, uint8_t bitNum, uint8_t data)
*功　　能:	    读 修改 写 指定设备 指定寄存器一个字节 中的1个位
输入	dev  目标设备地址
		reg	   寄存器地址
		bitNum  要修改目标字节的bitNum位
		data  为0 时，目标位将被清0 否则将被置位
返回   成功 为1 
 		失败为0
*******************************************************************************/ 
uint8_t IICwriteBit(uint8_t dev, uint8_t reg, uint8_t bitNum, uint8_t data){
    uint8_t b;
    IICreadByte(dev, reg, &b);
    b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
    return IICwriteByte(dev, reg, b);
}


//=========================================================
//******MS561101BA程序********
//=========================================================

void MS561101BA_RESET(void)//如数据读不出来，或程序跑不下去，请注释掉此处while(IIC_Wait_Ack());
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
//从PROM读取出厂校准数据
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
	}//打印PROM读取出厂校准数据，检测数据传输是否正常
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
//读取数字温度
void MS561101BA_GetTemperature(uint8_t OSR_Temp)
{
    
	D2_Temp= MS561101BA_DO_CONVERSION(OSR_Temp);	
	delay_msm(10);
	
	dT=D2_Temp - (((uint32_t)Cal_C[5])<<8);
	Temperature1=2000+dT*((uint32_t)Cal_C[6])/8388608;//

}
//读取数字气压
void MS561101BA_GetPressure(uint8_t OSR_Pres)
{
	float Aux,OFF2,SENS2;  //温度校验值	
	
	D1_Pres= MS561101BA_DO_CONVERSION(OSR_Pres);
	delay_msm(2); 
	
	OFF=(uint32_t)(Cal_C[2]<<16)+((uint32_t)Cal_C[4]*dT)/128;
	SENS=(uint32_t)(Cal_C[1]<<15)+((uint32_t)Cal_C[3]*dT)/256;
	//温度补偿
	if(Temperature1 < 2000)// second order temperature compensation when under 20 degrees C
	{
		Temperature2 = (dT*dT) / 0x80000000;
		Aux = (Temperature1-2000)*(Temperature1-2000);
		OFF2 = 2.5*Aux ;//此处不加f有警告，单精度默认转化为双精度
		SENS2 = 1.25*Aux;//此处不加f有警告，单精度默认转化为双精度
		if(Temperature1 < -1500)
		{
			Aux = (Temperature1+1500)*(Temperature1+1500);
			OFF2 = OFF2 + 7*Aux;
			SENS2 = SENS + 5.5*Aux;//此处不加f有警告，单精度默认转化为双精度
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
    uint32_t ex_Temperature; //串口读数转换值
    MS561101BA_GetTemperature(MS561101BA_D2_OSR_4096);
  
	  ex_Temperature=(long)(Temperature1);
	
	  exchange_Temp_num[3]=ex_Temperature/1000%10+'0';//温度十位
	  exchange_Temp_num[4]=ex_Temperature/100%10+'0';//温度个位
	  //exchange_Temp_num[5]='.';
	  exchange_Temp_num[6]=ex_Temperature/10%10+'0'; //温度小数点后一位
	
		
	  	
}
void READ_MS5611_Pressure(void)
{
    uint32_t ex_Pressure;	//串口读数转换值

	  MS561101BA_GetPressure(MS561101BA_D1_OSR_4096);
	
	  ex_Pressure=(long)(Pressure); 
	  ex_Pressure=ex_Pressure+3180;
	  ex_Pressure=ex_Pressure/100;
	
	  exchange_Pres_num[0]=ex_Pressure/1000+'0';//气压十万位
	  exchange_Pres_num[1]=ex_Pressure/100%10+'0';//气压万位
	  exchange_Pres_num[2]=ex_Pressure/10%10+'0';//气压千位
	  exchange_Pres_num[3]=ex_Pressure%10+'0';//气压百位
	  
	  
}
//***************数据转换******************* 
void Exchange_Number(void) 
{ 
	uint32_t ex_Pressure;	//串口读数转换值
	uint32_t ex_Temperature; //串口读数转换值

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
	//printf("\r\nMPU6050所测气压:%d  Pa\r\n",ex_Pressure*100); 
  printf("\r\n----------MS5611  测量气压---------%d Pa\r\n",ex_Pressure+3180);
 	
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
  printf("\r\n----------MS5611  测量温度---------%d ℃\r\n",ex_Temperature);
	//printf("\r\n----------MPU6050  测量温度---------\n");
	//Usart_Send(exchange_Temp_num,8);
	printf("\r\n");
} 

void GET_Pressure(void)
{
	
	uint32_t ex_Pressure_sum=0;	//串口读数转换值
	uint32_t ex_Temperature; //串口读数转换值
	uint8_t i;
	
//	IIC_GPIO_Config(); 
//	MS561101BA_Init(); //MS561101BA初始化
	//Exchange_Number();
	
  for(i=0;i<5;i++)
	{
	     MS561101BA_GetTemperature(MS561101BA_D2_OSR_4096);
	     MS561101BA_GetPressure(MS561101BA_D1_OSR_4096);
		   ex_Pressure_sum=ex_Pressure_sum+(long)(Pressure);
	}
	
	ex_Pressure=(ex_Pressure_sum/5+3180)&0X00FFFFFF; 
	
	//ex_Pressure=0X123456;//测试发送数据
	
  DisplayAP(Green,Black);
	
	//printf("\r\n----------MS5611  测量气压---------%d Pa\r\n",ex_Pressure);
}