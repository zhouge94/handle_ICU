#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"	 

#include  <math.h>    //Keil library  //==========================================
#include "stdlib.h"

#include "AD5933.h"
//#include "SDCard.h"

uint8_t AD5933_CMD_BUF_1[1]={0X10}; //��λAD5933
uint8_t AD5933_CMD_BUF_2[1]={0X00};
uint8_t AD5933_CMD_BUF_3[1]={0X91};////�����¶Ȳ���

uint8_t AD5933_CMD_BUF_4[1]={0X01};
uint8_t AD5933_CMD_BUF_5[1]={0X10};
uint8_t AD5933_CMD_BUF_6[1]={0X8F};

uint8_t AD5933_CMD_BUF_7[1]={0X33};
uint8_t AD5933_CMD_BUF_8[1]={0X33};
uint8_t AD5933_CMD_BUF_9[1]={0X33}; 
	
uint8_t AD5933_CMD_BUF_10[1]={0X4F};
uint8_t AD5933_CMD_BUF_11[1]={0X00}; 
uint8_t AD5933_CMD_BUF_12[1]={0X00};

uint8_t AD5933_CMD_BUF_13[1]={0X0F};//15��
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

uint16_t Imagdata;
uint16_t Realdata;

double imag_data;
double real_data;

float Magnitude,Impedance1,ImpedanceSUM;//��ֵ
double Gainfactor=5.966067195828e-009 ,Gain_factor,GainFactorSum=0;//ϵ�����迹 �ٷ������� Gainfactor=5.966067195828e-009
float Impedance;//ϵ�����迹
float RSP_Data_sum;

float new_impedance;
double phase,phaseSUM,phase1,phase1SUM;
unsigned int phase1_cnt,phase_cnt;//��λ��������
unsigned int sample_cnt=0;//������ƽ��ֵ�ĵ���
unsigned int GainFactor_cnt=0;//��������ʱ�����ĵ���

float value_buf[35];

uint8_t START_MEA_RSP=0;

uint8_t RSP_DATA_X[8]={0XAA,0XAA,0X03,0X00,0X00,0X00,0X00,0X00};

uint8_t  fliterBuffer_ok=0;//�˲�����������־λ
uint8_t  fliterBuffer_cnt=0;//�˲����������ݼ���
float fliterBufferRSP[32]={0};//�������ݻ�����
float    RSP_SUM  =0;
uint8_t  fliter_RSPNumber=11;//ѡ����л���ƽ���˲������ݸ���
float new_fliter_impedance=0;//������һ�λ���ƽ���˲�֮��ĺ�������

float after_fliterBufferRSP[8]={0};//������һ���˲�����֮��ĺ������ݻ�����
float fliter_impedance=0;//�����ڶ��λ���ƽ���˲�֮��ĺ�������

uint16_t fliter_impedance_hex=0;//�����ڶ��λ���ƽ���˲�֮��ĺ�������

float fliter_impedance_max=0;//��������ƽ���˲�֮��ĺ����������ֵ
float fliter_impedance_min=0;//��������ƽ���˲�֮��ĺ���������Сֵ

uint8_t  Buffer_ok;
float RSP_Buffer[64]={0};//�������ݻ�����
uint8_t  RSP_Buffer_cnt=0;//�������������ݼ���
uint8_t  RSP_Buffer_Number=25;//ѡ����з�ֵ���Ļ��������ݸ���
uint8_t  RSP_Buffer_Center=0;//����������λ��
uint8_t  PP_Distance=0;//���ֵ֮��ļ��
uint8_t  PT_Distance=0;//�Ӳ���ֵ��ʼ���������Ѱ����Сֵ��Ϊ����
uint8_t  FIND_peak=0;//����һ����ֵ
float fliter_impedance_tem=0;
uint8_t  Peak_number=0;
uint16_t  Breath_rate=0;
float impedance_peak=0;//����ֵ
uint16_t impedance_trough=0;//����ֵ
uint16_t impedance_peak_buffer[16]={0};
uint8_t  impedance_peak_cnt=4;

float impedance_peak_BUF[10]={0};//����ֵ������
uint8_t impedance_peak_BUF_CNT=0;//����ֵ����������
float impedance_peak_sum=0;//����������ֵ�ĺ�
float impedance_peak_rev=65535;//����������ֵƽ��ֵ
uint16_t impedance_peak_rev_hex=65535;//����������ֵƽ��ֵ
float d_impedance=0;//��ֵƽ��ֵ��ȥ�²ɼ���ֵ
short send_impedance=0;//Ҫ���͵ĺ�������

uint8_t impedance_AMP=0;//�������ݷŴ���

extern uint8_t SDCardValid;

extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;
extern UART_HandleTypeDef huart2;

int AD5933_read (uint8_t register_address)
{
		 uint8_t  RX_BUF[1]={0};
		   
		 while(HAL_I2C_Mem_Read_DMA(&hi2c1, 0X1B, register_address, I2C_MEMADD_SIZE_8BIT, RX_BUF, 1)!=HAL_OK){};
		 
		 /* Wait for the end of the transfer */  
     while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
     {
     }
	  
	   return RX_BUF[0];
		 
}


void Read_AD5933_Temputer(void)
{
	
	  uint8_t Status;
	  uint16_t Tm;
	  float temputer=0;
	
	  //��λAD5933
	  while(HAL_I2C_Mem_Write_DMA(&hi2c1, AD5933_addr, 0X81, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_1, 1)!=HAL_OK);
	  while(HAL_I2C_Mem_Write_DMA(&hi2c1, AD5933_addr, 0X81, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_2, 1)!=HAL_OK);
	 
	  //��������
  	while(HAL_I2C_Mem_Write_DMA(&hi2c1, AD5933_addr, 0X80, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_3, 1)!=HAL_OK);
	  
	  //�ȴ�ת�����
	  do
		{
		    Status=AD5933_read (0x8F);
		}while(!(Status & 0x01));
	
		Tm=AD5933_read (0x92);
//		printf("AD5933 Temputer H=%02X\r\n",Tm);//
		Tm<<=8;
		Tm+=AD5933_read (0x93);
//		printf("AD5933 Temputer L=%02X\r\n",Tm);//
//		printf("AD5933 Temputer X=%04X\r\n",Tm);//
//		
		Tm=Tm&0X3FFF;
		temputer=(float)(Tm/32);
	
		printf("AD5933 Temputer=%.2f��\r\n\r\n",temputer);//
}	




void Sample (void)//7ms
{	 
  char count,i,j,ii,iii; 
	float temp;
	
	while(HAL_I2C_Mem_Write_DMA(&hi2c1, AD5933_addr, 0X80, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_21, 1)!=HAL_OK){};//����ʼƵ�ʳ�ʼ��

  while(HAL_I2C_Mem_Write_DMA(&hi2c1, AD5933_addr, 0X80, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_22, 1)!=HAL_OK);//����Ƶ��ɨ��
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
            imag_byte_low =  AD5933_read (0x97);    
					                                          //==============================/////////////
					 
					 
                                                                         //==============================/////////////
            Realdata = real_byte_high*0x100 +real_byte_low; 					 
					  if(Realdata>32767) //��ԭ���Ϊ����
              real_data=Realdata-65536;
            else
              real_data=Realdata;							
						
						Imagdata = imag_byte_high*0x100 +imag_byte_low;	            						
					  if(Imagdata>32767) //��ԭ���Ϊ����
              imag_data=Imagdata-65536;
						else
							imag_data=Imagdata;
						
						Impedance1=(real_data*real_data + imag_data*imag_data)/2;
						
//            Magnitude=sqrt(real_data*real_data + imag_data*imag_data);

//						Impedance1=1/(Magnitude*Gainfactor);  
						                          
							
//						value_buf[count++] =Impedance1;//��λֵ�˲���4
//						value_buf[count++] =12345678;//��������
						
						RSP_Data_sum=RSP_Data_sum+ Impedance1; //��ֵ�˲���                                   
						                                                                //==============================/////////////
					  while(HAL_I2C_Mem_Write_DMA(&hi2c1, AD5933_addr, 0X80, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_23, 1)!=HAL_OK);//����Ƶ��  300us
						
						//while(HAL_I2C_Mem_Write_DMA(&hi2c1, AD5933_addr, 0X80, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_24, 1)!=HAL_OK);//�ظ�Ƶ��
         } // end of D2 test condition

         else // End of frequency sweep exit loop
         {
            while(HAL_I2C_Mem_Write_DMA(&hi2c1, AD5933_addr, 0X80, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_25, 1)!=HAL_OK); //����ģʽ PGA=X1
					  //impedance=RSP_Data_sum/20;//��ֵ�˲���
					 
					 //impedance=0X456789;//��������                                                              //150us
						
					 //��λֵ�˲�
//					  for(j=0;j<13-1;j++) 
//		        { 
//			          for (i=j+1;i<13;i++) 
//                { 
//                    if ( value_buf[j]>value_buf[i] ) 
//                    { 
//                        temp = value_buf[j]; 
//                        value_buf[j] = value_buf[i]; 
//                        value_buf[i] = temp; 
//                    } 
//                } 
//            }
//          new_impedance=((value_buf[4]+value_buf[5]+value_buf[6]+value_buf[7]+value_buf[8])-1000000)/20;
					  
					  /*============= ��ֵ�˲� ============= */
				    new_impedance=RSP_Data_sum/15;
						RSP_Data_sum=0;
						
//          new_impedance=12345678;
           
						/*============= ����ƽ���˲� ============= */
            if(fliterBuffer_cnt==fliter_RSPNumber)//�˲�������������λ��־λfliterBuffer_ok
					  {
					    fliterBuffer_ok=1;
					  }
						
					 if(fliterBuffer_ok==0)//�˲�����������δ��
					 {
					    fliterBufferRSP[fliterBuffer_cnt++]=new_impedance;//���²ɼ��ĺ������ݷŵ�������
					 }
					 else//������������������ʼ���л���ƽ���˲�����
					 {
					    for(ii=1;ii<fliter_RSPNumber;ii++)//ͨ����λ���������Ƚ��뻺�����������Ƴ���ͬʱ���²ɼ������ݷ��뻺����
						  {
							   fliterBufferRSP[ii-1]=fliterBufferRSP[ii];
							}
						  fliterBufferRSP[fliter_RSPNumber-1]=new_impedance;//���²ɼ��ĺ������ݷŵ���������ĩβ
							for(ii=0;ii<fliter_RSPNumber;ii++)//���
						  {
								 RSP_SUM+=fliterBufferRSP[ii];
							}
							new_fliter_impedance=RSP_SUM/fliter_RSPNumber;//��ƽ��	
							
//							new_fliter_impedance=(fliterBufferRSP[0]*0.0625+fliterBufferRSP[1]*0.25+fliterBufferRSP[2]*0.375+fliterBufferRSP[3]*0.25+fliterBufferRSP[4]*0.0625);
							RSP_SUM=0;//����			
					 }
					/*============= �ڶ��λ���ƽ���˲� ============= */
					 for(ii=1;ii<5;ii++)
					 {
					    after_fliterBufferRSP[ii-1]=after_fliterBufferRSP[ii];
					 }
					 after_fliterBufferRSP[4]=new_fliter_impedance;
					 
					 fliter_impedance=(after_fliterBufferRSP[0]+after_fliterBufferRSP[1]+after_fliterBufferRSP[2]+after_fliterBufferRSP[3]+after_fliterBufferRSP[4])/5;
					 
 					/*============= ��ֵ��� ============= */
					if(RSP_Buffer_cnt==RSP_Buffer_Number)//��ֵ��⻺����������������λ��־λBuffer_ok
					{
					    Buffer_ok=1;
					}
					if(Buffer_ok==0)//��ֵ��⻺��������δ��
					{
					    RSP_Buffer[RSP_Buffer_cnt++]=fliter_impedance;//�������˲�֮������ݷŵ���������ĩβ
					} 
					else
					{
					    for(ii=1;ii<RSP_Buffer_Number;ii++)//ͨ����λ���������Ƚ��뻺�����������Ƴ���ͬʱ���²ɼ������ݷ��뻺����
						  {							   								
								 RSP_Buffer[ii-1]=RSP_Buffer[ii];								
							}
							RSP_Buffer[RSP_Buffer_Number-1]=fliter_impedance;//�������˲�֮������ݷŵ���������ĩβ
					}
					RSP_Buffer_Center=(RSP_Buffer_Number-1)/2; 
					
					if(Buffer_ok==1)//���ݻ�����������ʼ��������
					{
						fliter_impedance_tem=RSP_Buffer[RSP_Buffer_Center];
						
						if(PP_Distance>35)//������ϸ���ֵ�����Ƿ�����Ҫ��
						{
						    if( (RSP_Buffer[RSP_Buffer_Center]-RSP_Buffer[RSP_Buffer_Center-7])>=0&&
									  (RSP_Buffer[RSP_Buffer_Center]-RSP_Buffer[RSP_Buffer_Center-6])>=0&&
									  (RSP_Buffer[RSP_Buffer_Center]-RSP_Buffer[RSP_Buffer_Center-5])>=0&&
									  (RSP_Buffer[RSP_Buffer_Center]-RSP_Buffer[RSP_Buffer_Center-4])>=0&&
									  (RSP_Buffer[RSP_Buffer_Center]-RSP_Buffer[RSP_Buffer_Center-3])>=0&&
									  (RSP_Buffer[RSP_Buffer_Center]-RSP_Buffer[RSP_Buffer_Center-2])>=0&&
							      (RSP_Buffer[RSP_Buffer_Center]-RSP_Buffer[RSP_Buffer_Center-1])>=0&&
                    (RSP_Buffer[RSP_Buffer_Center]-RSP_Buffer[RSP_Buffer_Center+1])>=0&&
								    (RSP_Buffer[RSP_Buffer_Center]-RSP_Buffer[RSP_Buffer_Center+2])>=0&&
								    (RSP_Buffer[RSP_Buffer_Center]-RSP_Buffer[RSP_Buffer_Center+3])>=0&&
						        (RSP_Buffer[RSP_Buffer_Center]-RSP_Buffer[RSP_Buffer_Center+4])>=0&&
								    (RSP_Buffer[RSP_Buffer_Center]-RSP_Buffer[RSP_Buffer_Center-5])>=0&&
								    (RSP_Buffer[RSP_Buffer_Center]-RSP_Buffer[RSP_Buffer_Center-6])>=0&&
								    (RSP_Buffer[RSP_Buffer_Center]-RSP_Buffer[RSP_Buffer_Center-7])>=0							
								   )//ͨ���ж����������ĸ�����ֵ�����ж��Ƿ����Ϊ��ֵ��
					      {
						       for(ii=0;ii<RSP_Buffer_Number;ii++) //Ѱ�һ������������ֵ
							     {
								      if(RSP_Buffer[ii]>fliter_impedance_max)  fliter_impedance_max=RSP_Buffer[ii];
							     }
							     if(RSP_Buffer[RSP_Buffer_Center]==fliter_impedance_max)//�ж���ֵ�Լ����ֵ�������ȷ���Ƿ�Ϊ��ֵ��
							     {
								      
										  Peak_number++;
//										  fliter_impedance+=100;//������ֵ����100��ͻ����ʾ  
								      PP_Distance=0;
										  impedance_peak=fliter_impedance_max;//����ֵ����
								      fliter_impedance_max =0;
										 if(impedance_peak_BUF_CNT<4)
						         {
						           impedance_peak_BUF[impedance_peak_BUF_CNT++]=impedance_peak;
						         }
							     }
						    }
					  }
						/*============= ��ȡ�仯�ĺ����� ============= */
						
						
						/*============= �������� ============= */
						
		
					  if(impedance_peak_BUF_CNT==4)
            {
						  for(iii=0;iii<4;iii++)
							{
							 impedance_peak_sum+=impedance_peak_BUF[iii];
							}
							impedance_peak_rev=impedance_peak_sum/4;
							impedance_peak_sum=0;
							impedance_peak_BUF_CNT=0;
						}							
//					  send_impedance=(impedance_peak_rev-fliter_impedance)/15;
//		
//					  RSP_DATA_X[3]=send_impedance/0X100;
//					  RSP_DATA_X[4]=send_impedance%0X100;	
						
            //ֱ�ӷ���ԭʼ����ֵ

						impedance_peak_rev_hex=(uint32_t)impedance_peak_rev;
						fliter_impedance_hex=(uint32_t)fliter_impedance-impedance_peak_rev_hex;
						
					  RSP_DATA_X[3]=fliter_impedance_hex/0X100;
					  RSP_DATA_X[4]=fliter_impedance_hex%0X100;	

            RSP_DATA_X[5]=impedance_peak_rev_hex/0X100;
					  RSP_DATA_X[6]=impedance_peak_rev_hex%0X100;							
						

					 RSP_DATA_X[7]=Breath_rate;
					 HAL_UART_Transmit_DMA(&huart2 , RSP_DATA_X, 8);
						
					/*************** �����ٶ����ݴ���SD��������******************/
//		      if(SDCardValid==1)
//			      Write_Data_To_SDBuffer(RSP_DATA_X, 6);
					 
//					 printf("%.0d\r\n",fliter_impedance);
						
					}
				 	PP_Distance++;						 
					break;
         }
       } // end of D1 true condition
       
				 
	 }	
 }// end of sweep function

//��ʼ��AD5933��������Ҫ��ο�pdf�����޸�************************
void Init_AD5933(void)
{
  while(HAL_I2C_Mem_Write_DMA(&hi2c1, AD5933_addr, 0X80, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_4, 1)!=HAL_OK);
	while(HAL_I2C_Mem_Write_DMA(&hi2c1, AD5933_addr, 0X81, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_5, 1)!=HAL_OK);//��λ
	while(HAL_I2C_Mem_Write_DMA(&hi2c1, AD5933_addr, 0X81, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_6, 1)!=HAL_OK);//ѡ���ⲿϵͳʱ��

	
                         //start frequency                                      
	                       // program 100khz start frequency assuming internal osc of 16.776Khz
	while(HAL_I2C_Mem_Write_DMA(&hi2c1, AD5933_addr, 0X84, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_7, 1)!=HAL_OK);
	while(HAL_I2C_Mem_Write_DMA(&hi2c1, AD5933_addr, 0X83, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_8, 1)!=HAL_OK);
	while(HAL_I2C_Mem_Write_DMA(&hi2c1, AD5933_addr, 0X82, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_9, 1)!=HAL_OK);
	                      // Transmit to frequency increment register 
                        // program 1hz frequency increment assuming internal osc of 16.776Khz
	while(HAL_I2C_Mem_Write_DMA(&hi2c1, AD5933_addr, 0X87, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_10, 1)!=HAL_OK);
	while(HAL_I2C_Mem_Write_DMA(&hi2c1, AD5933_addr, 0X86, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_11, 1)!=HAL_OK);
	while(HAL_I2C_Mem_Write_DMA(&hi2c1, AD5933_addr, 0X85, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_12, 1)!=HAL_OK);
	
	                      // Transmit to NUMBER OF INCREMENTS register 
                        // program 10 frequency increments
	while(HAL_I2C_Mem_Write_DMA(&hi2c1, AD5933_addr, 0X89, I2C_MEMADD_SIZE_8BIT,AD5933_CMD_BUF_13, 1)!=HAL_OK);
	while(HAL_I2C_Mem_Write_DMA(&hi2c1, AD5933_addr, 0X88, I2C_MEMADD_SIZE_8BIT,AD5933_CMD_BUF_14, 1)!=HAL_OK);
	
	
	                      // Transmit to settling time cycles register 
                        // program 15 output cycles at each frequency before a adc conversion
	while(HAL_I2C_Mem_Write_DMA(&hi2c1, AD5933_addr, 0X8B, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_15, 1)!=HAL_OK);
	while(HAL_I2C_Mem_Write_DMA(&hi2c1, AD5933_addr, 0X8A, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_16, 1)!=HAL_OK);
	
	                      // Transmit to CONTROL register 
                        // place the AD5933 in standby mode
	while(HAL_I2C_Mem_Write_DMA(&hi2c1, AD5933_addr, 0X81, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_17, 1)!=HAL_OK);
	
	while(HAL_I2C_Mem_Write_DMA(&hi2c1, AD5933_addr, 0X80, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_18, 1)!=HAL_OK);
	
	while(HAL_I2C_Mem_Write_DMA(&hi2c1, AD5933_addr, 0X81, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_19, 1)!=HAL_OK);
	
	while(HAL_I2C_Mem_Write_DMA(&hi2c1, AD5933_addr, 0X80, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_20, 1)!=HAL_OK);
	
	while(HAL_I2C_Mem_Write_DMA(&hi2c1, AD5933_addr, 0X80, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_21, 1)!=HAL_OK){};

  while(HAL_I2C_Mem_Write_DMA(&hi2c1, AD5933_addr, 0X80, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_22, 1)!=HAL_OK);//����Ƶ��ɨ��


}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
