#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"	 

#include  <math.h>    //Keil library  //==========================================
#include "stdlib.h"
#include "sys.h"
#include "AD5933.h"
#include "FreeRTOS.h"
uint8_t AD5933_CMD_BUF_1[1]={0X10}; //复位AD5933
uint8_t AD5933_CMD_BUF_2[1]={0X00};
uint8_t AD5933_CMD_BUF_3[1]={0X91};////启动温度测量

uint8_t AD5933_CMD_BUF_4[1]={0X01};
uint8_t AD5933_CMD_BUF_5[1]={0X10};
uint8_t AD5933_CMD_BUF_6[1]={0X8F};

uint8_t AD5933_CMD_BUF_7[1]={0X00};
uint8_t AD5933_CMD_BUF_8[1]={0X00};
uint8_t AD5933_CMD_BUF_9[1]={0X33}; 
	
uint8_t AD5933_CMD_BUF_10[1]={0X4F};
uint8_t AD5933_CMD_BUF_11[1]={0X00}; 
uint8_t AD5933_CMD_BUF_12[1]={0X00};

uint8_t AD5933_CMD_BUF_13[1]={0X19};//25次
uint8_t AD5933_CMD_BUF_14[1]={0X00};

uint8_t AD5933_CMD_BUF_15[1]={0X0F};
uint8_t AD5933_CMD_BUF_16[1]={0X00};

uint8_t AD5933_CMD_BUF_17[1]={0X20};
uint8_t AD5933_CMD_BUF_18[1]={0XA1};

uint8_t AD5933_CMD_BUF_19[1]={0X08}; 
uint8_t AD5933_CMD_BUF_20[1]={0XB1};
uint8_t AD5933_CMD_BUF_21[1]={0X11}; 
uint8_t AD5933_CMD_BUF_22[1]={0X21};

uint8_t AD5933_CMD_BUF_23[1]={0X31};
uint8_t AD5933_CMD_BUF_24[1]={0X41};

uint8_t AD5933_CMD_BUF_25[1]={0XB1};

uint8_t AD5933_CMD_BUF_26[1]={0X00};
uint8_t AD5933_CMD_BUF_27[1]={0X00};
uint8_t AD5933_CMD_BUF_28[1]={0X00};
uint8_t AD5933_CMD_BUF_29[1]={0X00}; 
uint8_t AD5933_CMD_BUF_30[1]={0X00};


short int status_register ;

uint8_t real_byte_high;
uint8_t real_byte_low;
uint8_t imag_byte_high;
uint8_t imag_byte_low;

int16_t Imagdata;
int16_t Realdata;

double imag_data;
double real_data;

unsigned char Fuhao_imag=0,Fuhao_real;
float Magnitude,Impedance1,ImpedanceSUM;//幅值
double Gainfactor=5.966067195828e-005 ,Gain_factor,GainFactorSum=0;//系数，阻抗 官方评估板 Gainfactor=5.966067195828e-009
float Impedance;//系数，阻抗

double impedance,new_impedance,last_impedance,last_last_impedance;
double last_last_impedance_tx=0,last_impedance_tx=0,impedance_tx=0,new_impedance_tx=0;
uint16_t Send_impedance_x;
double phase,phaseSUM,phase1,phase1SUM;
unsigned int phase1_cnt,phase_cnt;//相位采样点数
unsigned int sample_cnt=0;//采样求平均值的点数
unsigned int GainFactor_cnt=0;//计算增益时采样的点数

unsigned char  rsp_cnt=0,rspk=1,rsp_cnt2=0;
uint32_t RSP_Data_sum=0;
double RSP_Data_rev=0;
double DIF_RSP_Data_sum=0;
double DIF_RSP_Data_rev=0;
uint8_t START_MEA_RSP=0;

double DIF_RSP_value_buf[128];
uint32_t RSP_value_buf[128];
uint32_t RSP_value_buf2[128];
uint32_t RSP_Min=0XFFFFFFFF;

uint8_t RSP_DATA_X[6]={0XAA,0XAA,0X03,0X00,0X00,0X00};

double last_Dif_RSP_value,Dif_RSP_value,new_Dif_RSP_value;
uint8_t Peak_value;
uint8_t Breath_rate;
extern I2C_HandleTypeDef hi2c1;

int AD5933_read (uint8_t register_address)
{
	uint8_t  RX_BUF;
	HAL_StatusTypeDef temp;
	while(HAL_I2C_Mem_Read(&hi2c1, 0x1B, register_address, I2C_MEMADD_SIZE_8BIT, &RX_BUF, 1,500)!=HAL_OK);
	return RX_BUF;
}


void Read_AD5933_Temputer(void)
{
	uint8_t Status;
	uint16_t Tm;
	float temputer=0;
	HAL_I2C_MspInit(&hi2c1);
	//复位AD5933
	HAL_I2C_Mem_Write(&hi2c1, AD5933_addr, 0X81, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_1, 1,500);
	HAL_I2C_Mem_Write(&hi2c1, AD5933_addr, 0X81, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_2, 1,500);
	//启动测量
	HAL_I2C_Mem_Write(&hi2c1, AD5933_addr, 0X80, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_3, 1,500);
	//等待转换完成
	do
	{
		Status=AD5933_read (0x8F);
	}while(!(Status & 0x01));
	Tm=AD5933_read (0x92);
	Tm<<=8;
	Tm+=AD5933_read (0x93);
	Tm=Tm&0X3FFF;
	temputer=(float)(Tm/32);
}
void BeginSample(void)
{
	HAL_I2C_Mem_Write(&hi2c1, AD5933_addr, 0X80, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_21, 1,500);//以起始频率初始化
	osDelay(100);
	HAL_I2C_Mem_Write(&hi2c1, AD5933_addr, 0X80, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_22, 1,500);//启动频率扫描
	osDelay(100);

}
float Sample (void)//7ms
{	 
	status_register = AD5933_read (0x8F);     // read the status register device address. set i2c address(LSB = 1, Master Read)
	status_register = (status_register & 0x2); // mask off the valid data bit
	for(;;)
	{
		status_register = AD5933_read (0x8F);     // read the status register device address. set i2c address(LSB = 1, Master Read)
		status_register = (status_register & 0x2); // mask off the valid data bit
		if( ((status_register)| 0xFD )==  0xFF)  // valid data should be present after start freqy command
		{
			if( (AD5933_read (0x8F)| 0xFB )!=  0xFF)// D2 test condition
			{
				real_byte_high = AD5933_read (0x94);    //==============================/////////////
				real_byte_low = AD5933_read (0x95);
				imag_byte_high = AD5933_read (0x96);                //读这四个要1.6ms
				imag_byte_low =  AD5933_read (0x97);                //==============================/////////////
				Realdata = real_byte_high*0x100 +real_byte_low;
				Imagdata = imag_byte_high*0x100 +imag_byte_low;
				HAL_I2C_Mem_Write(&hi2c1, AD5933_addr, 0X80, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_24, 1,500);//重复频率
       return (1/(sqrt(Imagdata*Imagdata+Realdata*Realdata)*Gainfactor));
       //return ((Imagdata*Imagdata+Realdata*Realdata)*Gainfactor);
			 break;
			} // end of D2 test condition
       } // end of D1 true condition
	 }


 }// end of sweep function
//初始化AD5933，根据需要请参考pdf进行修改************************
void Init_AD5933(void)
{
	HAL_StatusTypeDef temp;
	while(HAL_I2C_Mem_Write(&hi2c1, AD5933_addr, 0X80, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_4, 1,500)!=HAL_OK);
	while(HAL_I2C_Mem_Write(&hi2c1, AD5933_addr, 0X81, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_5, 1,500)!=HAL_OK);//复位
	while(HAL_I2C_Mem_Write(&hi2c1, AD5933_addr, 0X81, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_6, 1,500)!=HAL_OK);//选择外部系统时钟
  //start frequency
	//program 100khz start frequency assuming internal osc of 16.776Khz
	while(HAL_I2C_Mem_Write(&hi2c1, AD5933_addr, 0X84, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_7, 1,500)!=HAL_OK);
	while(HAL_I2C_Mem_Write(&hi2c1, AD5933_addr, 0X83, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_8, 1,500)!=HAL_OK);
	while(HAL_I2C_Mem_Write(&hi2c1, AD5933_addr, 0X82, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_9, 1,500)!=HAL_OK);
	// Transmit to frequency increment register
  // program 1hz frequency increment assuming internal osc of 16.776Khz
	while(HAL_I2C_Mem_Write(&hi2c1, AD5933_addr, 0X87, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_10, 1,500)!=HAL_OK);
	while(HAL_I2C_Mem_Write(&hi2c1, AD5933_addr, 0X86, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_11, 1,500)!=HAL_OK);
	while(HAL_I2C_Mem_Write(&hi2c1, AD5933_addr, 0X85, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_12, 1,500)!=HAL_OK);
	// Transmit to NUMBER OF INCREMENTS register
  // program 10 frequency increments
	while(HAL_I2C_Mem_Write(&hi2c1, AD5933_addr, 0X89, I2C_MEMADD_SIZE_8BIT,AD5933_CMD_BUF_13, 1,500)!=HAL_OK);
	while(HAL_I2C_Mem_Write(&hi2c1, AD5933_addr, 0X88, I2C_MEMADD_SIZE_8BIT,AD5933_CMD_BUF_14, 1,500)!=HAL_OK);
	// Transmit to settling time cycles register
  // program 15 output cycles at each frequency before a adc conversion
	while(HAL_I2C_Mem_Write(&hi2c1, AD5933_addr, 0X8B, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_15, 1,500)!=HAL_OK);
	while(HAL_I2C_Mem_Write(&hi2c1, AD5933_addr, 0X8A, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_16, 1,500)!=HAL_OK);
	// Transmit to CONTROL register
  // place the AD5933 in standby mode
	while(HAL_I2C_Mem_Write(&hi2c1, AD5933_addr, 0X81, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_17, 1,500)!=HAL_OK);
	while(HAL_I2C_Mem_Write(&hi2c1, AD5933_addr, 0X80, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_18, 1,500)!=HAL_OK);
	while(HAL_I2C_Mem_Write(&hi2c1, AD5933_addr, 0X81, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_19, 1,500)!=HAL_OK);
	while(HAL_I2C_Mem_Write(&hi2c1, AD5933_addr, 0X80, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_20, 1,500)!=HAL_OK);
	while(HAL_I2C_Mem_Write(&hi2c1, AD5933_addr, 0X80, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_21, 1,500)!=HAL_OK);
	while(HAL_I2C_Mem_Write(&hi2c1, AD5933_addr, 0X80, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_22, 1,500)!=HAL_OK);//启动频率扫描
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
