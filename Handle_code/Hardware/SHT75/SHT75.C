/*
@version 01
@date    2016.3.22
@author  尚小虎
@brief 温湿度传感器（SHT75） 
@note  管脚配置需在 STM32CubeMX 中设置
*/

#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"

#include "SHT75.h"
#include "math.h"
#include "oled.h"

extern UART_HandleTypeDef huart2;

uint16_t TEM,HUI;

uint8_t teeest;

extern void DisplayAT(uint16_t color,uint16_t backgroundcolor);
extern void DisplayAH(uint16_t color,uint16_t backgroundcolor);


void SHT75_IOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	__GPIOB_CLK_ENABLE();
	
	/*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pins : PB0 PB1 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/* */
char SHT75_Write_Byte(unsigned char value)
//----------------------------------------------------------------------------------
// writes a byte on the Sensibus and checks the acknowledge
{
	unsigned char i,error=0;
	for (i=0x80;i>0;i/=2) //shift bit for masking
	{ 
		if (i & value) SHT_DATA_H;//masking value with i , write to SENSI-BUS
		else SHT_DATA_L;
		HAL_Delay(1); //observe setup time
		SHT_SCK_H;//clk for SENSI-BUS
		HAL_Delay(1);//pulswith approx. 5 us
		SHT_SCK_L;
		HAL_Delay(1); //observe hold time
	}
	SHT_SCK_H;
	HAL_Delay(1);//observe hold time
	error=SHT_DATA_READ;//check ack (DATA will be pulled down by SHT11)
	SHT_SCK_L;
	HAL_Delay(1);//observe hold time
	
	SHT_DATA_H;//release DATA-line
	return error; //error=1 in case of no acknowledge
}
//----------------------------------------------------------------------------------
char SHT75_Read_Byte(unsigned char ack)
//----------------------------------------------------------------------------------
// reads a byte form the Sensibus and gives an acknowledge in case of "ack=1"
{
	unsigned char i,val=0;
	for (i=0x80;i>0;i/=2) //shift bit for masking
	{ 
		SHT_SCK_H; //clk for SENSI-BUS
		HAL_Delay(1);//observe setup time
		if(SHT_DATA_READ) val=(val | i); //read bit
		SHT_SCK_L;
		HAL_Delay(1); //observe setup time
	}
	if(!ack) SHT_DATA_H;
	else SHT_DATA_L;
	//in case of "ack==1" pull down DATA-Line
	HAL_Delay(1);//observe setup time
	SHT_SCK_H; //clk #9 for ack
	HAL_Delay(1);//pulswith approx. 5 us
	SHT_SCK_L;
	HAL_Delay(1); //observe hold time
	
	return val;
}
//----------------------------------------------------------------------------------
void SHT75_Trans_Start(void)
//----------------------------------------------------------------------------------
// generates a transmission start
//         _____         ________
// DATA:        |_______|
//             ____    ____
// SCK :    ___|  |____|  |______
{
	SHT_DATA_H;//DATA=1; 
	SHT_SCK_L;//SCK=0; //Initial state
	HAL_Delay(1);//_nop_();
	
	SHT_SCK_H;//SCK=1;
	HAL_Delay(1);//_nop_();
	SHT_DATA_L;//DATA=0;
	HAL_Delay(1);//_nop_();
	SHT_SCK_L;//SCK=0;
	HAL_Delay(1);//_nop_();_nop_();_nop_();
	
	SHT_SCK_H;//SCK=1;
	HAL_Delay(1);//_nop_();
	SHT_DATA_H;//DATA=1;
	HAL_Delay(1);//_nop_();
	SHT_SCK_L;//SCK=0;
	HAL_Delay(1);//_nop_();
}

//----------------------------------------------------------------------------------
void SHT75_Connection_Reset(void)
//----------------------------------------------------------------------------------
// communication reset: DATA-line=1 and at least 9 SCK cycles followed by transstart
//        _____________________________________________________        ________
// DATA:                                                      |_______|
//          _    _    _    _    _    _    _    _    _       _____   _____
// SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|   |___|   |______
{
	unsigned char i;
	SHT_DATA_H;//DATA=1; 
	SHT_SCK_L;//SCK=0; //Initial state
	HAL_Delay(1);//_nop_();
	
	for(i=0;i<9;i++) //9 SCK cycles
	{ 
		SHT_SCK_H;//SCK=1;
		HAL_Delay(1);
		SHT_SCK_L;//SCK=0;
		HAL_Delay(1);//_nop_();
	}
	SHT75_Trans_Start();
}
//----------------------------------------------------------------------------------
char SHT75_Soft_Reset(void)
//----------------------------------------------------------------------------------
// resets the sensor by a softreset
{
	unsigned char error=0;
	SHT75_Connection_Reset(); //reset communication
	error+=SHT75_Write_Byte(RESET); //send RESET-command to sensor
	return error; //error=1 in case of no response form the sensor
}

//----------------------------------------------------------------------------------
char SHT75_Write_StausReg(unsigned char *p_value)
//----------------------------------------------------------------------------------
// writes the status register with checksum (8-bit)
{
	unsigned char error=0;
	SHT75_Trans_Start(); //transmission start
	error+=SHT75_Write_Byte(STATUS_REG_W);//send command to sensor
	error+=SHT75_Write_Byte(*p_value); //send value of status register
	return error; //error>=1 in case of no response form the sensor
}
//----------------------------------------------------------------------------------
char SHT75_Read_StausReg(unsigned char *p_value, unsigned char *p_checksum)
//----------------------------------------------------------------------------------
// reads the status register with checksum (8-bit)
{
	unsigned char error=0;
	SHT75_Trans_Start(); //transmission start
	error=SHT75_Write_Byte(STATUS_REG_R); //send command to sensor
	*p_value=SHT75_Read_Byte(ACK); //read status register (8-bit)
	*p_checksum=SHT75_Read_Byte(noACK); //read checksum (8-bit)
	return error; //error=1 in case of no response form the sensor
}
//----------------------------------------------------------------------------------
char SHT75_Measure(unsigned int *p_value, unsigned char *p_checksum, unsigned char mode)
//----------------------------------------------------------------------------------
// makes a measurement (humidity/temperature) with checksum
{
	unsigned char error=0;
	unsigned char val1,val2;
	SHT75_Trans_Start(); //transmission start
	switch(mode)
	{ //send command to sensor
		case 0 : error+=SHT75_Write_Byte(MEASURE_TEMP); break;
		case 1 : error+=SHT75_Write_Byte(MEASURE_HUMI); break;
		default   : break;
	}
		
	uint32_t tickstart = 0U;
  tickstart = HAL_GetTick();
	while((HAL_GetTick() - tickstart) < 320)
  {
		if(SHT_DATA_READ==0) break;
  }
 //wait until sensor has finished the measurement
	
	if(SHT_DATA_READ) error+=1; // if(DATA) error+=1; // or timeout (~2 sec.) is reached
	
	val1 =SHT75_Read_Byte(ACK); //read the first byte (MSB)
	val2=SHT75_Read_Byte(ACK); //read the second byte (LSB)
	*p_value=val1*256+val2;
	*p_checksum =SHT75_Read_Byte(noACK); //read checksum
	return error;
}
//----------------------------------------------------------------------------------
void Calc_SHT75(float *p_humidity ,float *p_temperature)
//----------------------------------------------------------------------------------
// calculates temperature [癈] and humidity [%RH]
// input : humi [Ticks] (12 bit)
// temp [Ticks] (14 bit)
// output: humi [%RH]
// temp [癈]
{ 
	const float C1=-2.0468; // for 12 Bit RH
	const float C2=+0.0367; // for 12 Bit RH
	const float C3=-0.0000015955; // for 12 Bit RH
	const float T1=+0.01; // for 12 Bit RH
	const float T2=+0.00008; // for 12 Bit RH
	float rh=*p_humidity; // rh: Humidity [Ticks] 12 Bit
	float t=*p_temperature; // t: Temperature [Ticks] 14 Bit
	float rh_lin; // rh_lin: Humidity linear
	float rh_true; // rh_true: Temperature compensated humidity
	float t_C; // t_C : Temperature [癈]
	t_C=t*0.01 - 40.1; //calc. temperature[癈]from 14 bit temp.ticks @5V
	rh_lin=C3*rh*rh + C2*rh + C1; //calc. humidity from ticks to [%RH]
	rh_true=(t_C-25)*(T1+T2*rh)+rh_lin; //calc. temperature compensated humidity[%RH]
	if(rh_true>100)rh_true=100; //cut if the value is outside of
	if(rh_true<0.1)rh_true=0.1; //the physical possible range
	*p_temperature=t_C; //return temperature [癈]
	*p_humidity=rh_true; //return humidity[%RH]
}
//--------------------------------------------------------------------
float Calc_Dewpoint(float h,float t)
//--------------------------------------------------------------------
// calculates dew point
// input: humidity [%RH], temperature [癈]
// output: dew point [癈]
{ 
	float k,dew_point ;
	k = (log10(h)-2)/0.4343 + (17.62*t)/(243.12+t);
	dew_point = 243.12*k/(17.62-k);
	return dew_point;
	
}
char SHT75_Measure_Result(float *p_temp,float *p_humi,float *p_dewpoint)
{
	unsigned int temp_1,humi_1;
	float temp_2,humi_2;
	unsigned char checksum,error=0;
	SHT75_Connection_Reset();
	error+=SHT75_Measure(&temp_1,&checksum,0);
	error+=SHT75_Measure(&humi_1,&checksum,1);
	if(error==0)
	{
		temp_2=(float)temp_1;
		humi_2=(float)humi_1;
		Calc_SHT75(&humi_2,&temp_2);
		*p_temp=temp_2;
		*p_humi=humi_2;
		*p_dewpoint=Calc_Dewpoint(humi_2,temp_2);
	}
	return error;
}
void GET_SHT75(void)
{
    unsigned int temp_1,humi_1;
	  float temp_2,humi_2;
	  unsigned char checksum,error=0;
	
	  SHT75_Connection_Reset();
	  error+=SHT75_Measure(&temp_1,&checksum,0);
	
	  teeest=error;
	
	  error+=SHT75_Measure(&humi_1,&checksum,1);
	
	  teeest=error;
	
	  if(error==0)
	  {
		    temp_2=(float)temp_1;
		    humi_2=(float)humi_1;
		    Calc_SHT75(&humi_2,&temp_2);
		    
			  HUI=humi_2*10;
			  TEM=temp_2*10;
        
			  DisplayAT(Green,Black);
			  DisplayAH(Green,Black);
        			
	  }
}