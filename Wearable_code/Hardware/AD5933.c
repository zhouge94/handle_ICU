#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"	 

#include  <math.h>    //Keil library  //==========================================
#include "stdlib.h"
#include "sys.h"
#include "AD5933.h"
#include "FreeRTOS.h"
uint8_t AD5933_CMD_BUF_1[1]={0X10}; //��λAD5933
uint8_t AD5933_CMD_BUF_2[1]={0X00};
uint8_t AD5933_CMD_BUF_3[1]={0X91};////�����¶Ȳ���

uint8_t AD5933_CMD_BUF_4[1]={0X01};
uint8_t AD5933_CMD_BUF_5[1]={0X10};
uint8_t AD5933_CMD_BUF_6[1]={0X8F};

uint8_t AD5933_CMD_BUF_7[1]={0X00};
uint8_t AD5933_CMD_BUF_8[1]={0X00};
uint8_t AD5933_CMD_BUF_9[1]={0X33}; 
	
uint8_t AD5933_CMD_BUF_10[1]={0X4F};
uint8_t AD5933_CMD_BUF_11[1]={0X00}; 
uint8_t AD5933_CMD_BUF_12[1]={0X00};

uint8_t AD5933_CMD_BUF_13[1]={0X19};//25��
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
float Magnitude,Impedance1,ImpedanceSUM;//��ֵ
double Gainfactor=5.966067195828e-005 ,Gain_factor,GainFactorSum=0;//ϵ�����迹 �ٷ������� Gainfactor=5.966067195828e-009
float Impedance;//ϵ�����迹

double impedance,new_impedance,last_impedance,last_last_impedance;
double last_last_impedance_tx=0,last_impedance_tx=0,impedance_tx=0,new_impedance_tx=0;
uint16_t Send_impedance_x;
double phase,phaseSUM,phase1,phase1SUM;
unsigned int phase1_cnt,phase_cnt;//��λ��������
unsigned int sample_cnt=0;//������ƽ��ֵ�ĵ���
unsigned int GainFactor_cnt=0;//��������ʱ�����ĵ���

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
	//��λAD5933
	HAL_I2C_Mem_Write(&hi2c1, AD5933_addr, 0X81, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_1, 1,500);
	HAL_I2C_Mem_Write(&hi2c1, AD5933_addr, 0X81, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_2, 1,500);
	//��������
	HAL_I2C_Mem_Write(&hi2c1, AD5933_addr, 0X80, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_3, 1,500);
	//�ȴ�ת�����
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
	HAL_I2C_Mem_Write(&hi2c1, AD5933_addr, 0X80, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_21, 1,500);//����ʼƵ�ʳ�ʼ��
	osDelay(100);
	HAL_I2C_Mem_Write(&hi2c1, AD5933_addr, 0X80, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_22, 1,500);//����Ƶ��ɨ��
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
				imag_byte_high = AD5933_read (0x96);                //�����ĸ�Ҫ1.6ms
				imag_byte_low =  AD5933_read (0x97);                //==============================/////////////
				Realdata = real_byte_high*0x100 +real_byte_low;
				Imagdata = imag_byte_high*0x100 +imag_byte_low;
				HAL_I2C_Mem_Write(&hi2c1, AD5933_addr, 0X80, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_24, 1,500);//�ظ�Ƶ��
       return (1/(sqrt(Imagdata*Imagdata+Realdata*Realdata)*Gainfactor));
       //return ((Imagdata*Imagdata+Realdata*Realdata)*Gainfactor);
			 break;
			} // end of D2 test condition
       } // end of D1 true condition
	 }


 }// end of sweep function
//��ʼ��AD5933��������Ҫ��ο�pdf�����޸�************************
void Init_AD5933(void)
{
	HAL_StatusTypeDef temp;
	while(HAL_I2C_Mem_Write(&hi2c1, AD5933_addr, 0X80, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_4, 1,500)!=HAL_OK);
	while(HAL_I2C_Mem_Write(&hi2c1, AD5933_addr, 0X81, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_5, 1,500)!=HAL_OK);//��λ
	while(HAL_I2C_Mem_Write(&hi2c1, AD5933_addr, 0X81, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_6, 1,500)!=HAL_OK);//ѡ���ⲿϵͳʱ��
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
	while(HAL_I2C_Mem_Write(&hi2c1, AD5933_addr, 0X80, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_22, 1,500)!=HAL_OK);//����Ƶ��ɨ��
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
