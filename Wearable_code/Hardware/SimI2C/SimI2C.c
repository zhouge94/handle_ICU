#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"

#include "SimI2C.h"

char  test=0; 				 //IIC用到

//PIN_value = HAL_GPIO_ReadPin (GPIOB,GPIO_PIN_13);

/* 模拟IIC引脚配置 */
void MX_SimI2C_IO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
	  __GPIOB_CLK_ENABLE();
	
	  /**SimI2C GPIO Configuration    
    PB8     ------> SimI2C_SCL
    PB9     ------> SimI2C_SDA 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_10;
    GPIO_InitStruct.Mode =GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	  
	  SDA_L;
	  SCL_L;
	  SDA_H;
	  SCL_H;
	
}

void I2C_delay(void)
{
		
   uint32_t i=20;//这里可以优化速度	，经测试最低到5还能写入
   while(i) 
   { 
     i--; 
   }  
}
void delay5ms(void)
{
		
   int i=5000;  
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
unsigned char I2C_Start(void)
{
	SDA_H;
	SCL_H;
	I2C_delay();
	I2C_delay();
	if(!SDA_read)return FALSE;	//SDA线为低电平则总线忙,退出
	SDA_L;
	I2C_delay();
	I2C_delay();
	
	if(SDA_read) return FALSE;	//SDA线为高电平则总线出错,退出
	SCL_L;
	I2C_delay();
	I2C_delay();
	return TRUE;
}
/*******************************************************************************
* Function Name  : I2C_Stop
* Description    : Master Stop Simulation IIC Communication
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_Stop(void)
{
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SDA_H;
	I2C_delay();
} 
/*******************************************************************************
* Function Name  : I2C_Ack
* Description    : Master Send Acknowledge Single
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_Ack(void)
{	
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
}   
/*******************************************************************************
* Function Name  : I2C_NoAck
* Description    : Master Send No Acknowledge Single
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_NoAck(void)
{	
	SCL_L;
	I2C_delay();
	SDA_H;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
} 
/*******************************************************************************
* Function Name  : I2C_WaitAck
* Description    : Master Reserive Slave Acknowledge Single
* Input          : None
* Output         : None
* Return         : Wheather	 Reserive Slave Acknowledge Single
****************************************************************************** */
unsigned I2C_WaitAck(void) 	 //返回为:=1有ACK,=0无ACK
{
	SCL_L;//拉低scl，等待应答
	I2C_delay();
	I2C_delay();
	
	SCL_H;			
	I2C_delay();
	I2C_delay();
	
	
	if(SDA_read)//检测是否应答
	{
      SCL_L;
	    I2C_delay();
      return FALSE;//没有应答返回错误标识
	}
	SCL_L;//
	I2C_delay();
	return TRUE;
}

/*******************************************************************************
* Function Name  : I2C_SendByte
* Description    : Master Send a Byte to Slave
* Input          : Will Send Date
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_SendByte(uint8_t SendByte) //数据从高位到低位//
{
    uint8_t i=8;
	  SCL_L;
    I2C_delay();
    while(i--)
    {
        SCL_L;
        I2C_delay();
      if(SendByte&0x80)
        SDA_H;  
      else 
        SDA_L;  
			
        SendByte<<=1;
        I2C_delay();
		SCL_H;
        I2C_delay();
			  I2C_delay();
    }
   // SCL_L;
}  
/*******************************************************************************
* Function Name  : I2C_RadeByte
* Description    : Master Reserive a Byte From Slave
* Input          : None
* Output         : None
* Return         : Date From Slave 
****************************************************************************** */
unsigned char I2C_RadeByte(void)  //数据从高位到低位//
{ 
    uint8_t i=8;
    uint8_t ReceiveByte=0;

    SDA_H;				
    while(i--)
    {
      ReceiveByte<<=1;      
      SCL_L;
      I2C_delay();
			I2C_delay();
	  SCL_H;
      I2C_delay();
      I2C_delay();			
      if(SDA_read)
      {
        ReceiveByte|=0x01;
      }
    }
    SCL_L;
    return ReceiveByte;
} 
//ZRX          
//单字节写入*******************************************
//ZRX          
//单字节写入*******************************************

unsigned char Single_Write(unsigned char SlaveAddress,unsigned char REG_Address,unsigned char REG_data)		     //void
{
  	if(!I2C_Start())return FALSE;
    else
		{
		    I2C_SendByte(SlaveAddress);   //发送设备地址+写信号//I2C_SendByte(((REG_Address & 0x0700) >>7) | SlaveAddress & 0xFFFE);//设置高起始地址+器件地址 
        if(!I2C_WaitAck()){I2C_Stop(); return FALSE;} 
				else
				{
				    I2C_SendByte(REG_Address );   //设置低起始地址   
            if(!I2C_WaitAck()){I2C_Stop(); return FALSE;}
            else
						{
						    I2C_SendByte(REG_data);
							  if(!I2C_WaitAck()){I2C_Stop(); return FALSE;} 
								else
								{
								    I2C_Stop(); 
                //    delay5ms();
                    return TRUE;
								}
        
						}						
        
				}
				
		}
	  
}

//单字节读取*****************************************
unsigned char Single_Read(unsigned char SlaveAddress,unsigned char REG_Address)
{   unsigned char REG_data;     	
	if(!I2C_Start())return FALSE;
    I2C_SendByte(0X1B); //I2C_SendByte(((REG_Address & 0x0700) >>7) | REG_Address & 0xFFFE);//设置高起始地址+器件地址 
    if(!I2C_WaitAck()){I2C_Stop();test=1; return FALSE;}
    
		I2C_SendByte((uint8_t) REG_Address);   //设置低起始地址      
    I2C_WaitAck();
//    I2C_Start();
//    I2C_SendByte(0X1B);
//    I2C_WaitAck();

	  REG_data= I2C_RadeByte();
    I2C_NoAck();
    I2C_Stop();
    //return TRUE;
	  return REG_data;

}	

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
