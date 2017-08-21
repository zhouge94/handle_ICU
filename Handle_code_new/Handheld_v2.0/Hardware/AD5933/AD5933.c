#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"	 

#include  <math.h>    //Keil library  //==========================================
#include "stdlib.h"

#include "AD5933.h"

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

uint16_t Imagdata;
uint16_t Realdata;

double imag_data;
double real_data;

float Magnitude,Impedance1,ImpedanceSUM;//��ֵ
double Gainfactor=5.966067195828e-009 ,Gain_factor,GainFactorSum=0;//ϵ�����迹 �ٷ������� Gainfactor=5.966067195828e-009
float Impedance;//ϵ�����迹

double new_impedance;
double phase,phaseSUM,phase1,phase1SUM;
unsigned int phase1_cnt,phase_cnt;//��λ��������
unsigned int sample_cnt=0;//������ƽ��ֵ�ĵ���
unsigned int GainFactor_cnt=0;//��������ʱ�����ĵ���

uint8_t START_MEA_RSP=0;

uint8_t RSP_DATA_X[6]={0XAA,0XAA,0X03,0X00,0X00,0X00};

uint8_t  fliterBuffer_ok=0;//�˲�����������־λ
uint8_t  fliterBuffer_cnt=0;//�˲����������ݼ���
uint16_t fliterBufferRSP[32]={0};//�������ݻ�����
float    RSP_SUM  =0;
uint8_t  fliter_RSPNumber=5;//ѡ����л���ƽ���˲������ݸ���
uint16_t fliter_impedance=0;//��������ƽ���˲�֮��ĺ�������
uint16_t fliter_impedance_min=65535;//��������ƽ���˲�֮��ĺ���������Сֵ

uint8_t  Buffer_ok;
uint16_t RSP_Buffer[64]={0};//�������ݻ�����
uint8_t  RSP_Buffer_cnt=0;//�������������ݼ���
uint8_t  RSP_Buffer_Number=11;//ѡ����з�ֵ���Ļ��������ݸ���
uint8_t  RSP_Buffer_Center=0;//����������λ��
uint8_t  PP_Distance=0;//���ֵ֮��ļ��
uint8_t  FIND_peak=0;//����һ����ֵ
uint16_t fliter_impedance_tem=0;
uint8_t  Peak_number=0;
uint8_t  Breath_rate=0;
uint16_t impedance_min=0;
uint16_t impedance_min_buffer[16]={0};
uint8_t  impedance_min_cnt=0;
float    Send_impedance=0;
uint16_t TX_impedance=0;

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
//		  while(HAL_I2C_Mem_Read_DMA(&hi2c1, 0X1B, register_address, I2C_MEMADD_SIZE_8BIT, RX_BUF, 1)!=HAL_OK)
//		 {
//		 
//		 }
	  
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


void GetGainFactor(void)
{
	  Init_AD5933();
	  for(;;)
    {
			status_register = AD5933_read (0x8F);     // read the status register device address. set i2c address(LSB = 1, Master Read)
      status_register = (status_register & 0x2); // mask off the valid data bit
      if( ((status_register)| 0xFD )==  0xFF)  // valid data should be present after start freqy command
      {

         if( (AD5933_read (0x8F)| 0xFB )!=  0xFF)// D2 test condition
         {
					 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
            real_byte_high = AD5933_read (0x94);
            real_byte_low = AD5933_read (0x95);
            imag_byte_high = AD5933_read (0x96);
            imag_byte_low =  AD5933_read (0x97);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
            Realdata = ((real_byte_high << 8) |  real_byte_low);
            
					  if(Realdata>=0X8000&&Realdata<=0XFFFF)//����Ϊ����
						{
						   Realdata=~Realdata+1;
							 Realdata=Realdata&0XFFFF;
							 real_data=0-(double)Realdata;	
						}
						else
						{  
							  real_data=(double)Realdata;
						}
						
						Imagdata = ((imag_byte_high << 8) |  imag_byte_low); 
						if(Imagdata>=0X8000&&Imagdata<=0XFFFF)//����Ϊ����
						{
						    
							 Imagdata=~Imagdata+1;
							 Imagdata=Imagdata&0XFFFF;
							 imag_data=0-(double)Imagdata;
						}
						else
						{  
							  imag_data=(double)Imagdata;
						}
						
            Magnitude=sqrt(real_data*real_data + imag_data*imag_data);
						
						Gain_factor=1/(Magnitude*calibrationRes);
						
            	
//						if((real_data>0)&&(imag_data>0))//��һ����
//						{
//						    phase1=(atan(imag_data/real_data))*180/3.141592654;
//						}
//						else if((real_data>0)&&(imag_data<0))//��������
//						{
//						     phase1=360+(atan(imag_data/real_data))*180/3.141592654;
//						}
//						else if((real_data<0)&&(imag_data<0))//��������
//						{
//						     phase1=180+(atan(imag_data/real_data))*180/3.141592654;
//						}
//						else if((real_data<0)&&(imag_data>0))//�ڶ�����
//						{
//						     phase1=180+(atan(imag_data/real_data))*180/3.141592654;
//						}
//						
            GainFactor_cnt++;	
						phase1SUM=phase1SUM+phase1;
						GainFactorSum=GainFactorSum+Gain_factor;
						//Single_Write(AD5933_addr,0x80, 0x31);	
						if(GainFactor_cnt>=40)
						{
						    while(HAL_I2C_Mem_Write_DMA(&hi2c1, AD5933_addr, 0X80, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_23, 1)!=HAL_OK);//����Ƶ��
						
						}
						else
						   while(HAL_I2C_Mem_Write_DMA(&hi2c1, AD5933_addr, 0X80, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_24, 1)!=HAL_OK);//�ظ�Ƶ��
						
         } // end of D2 test condition

         else // End of frequency sweep exit loop
         {
             Gain_factor=GainFactorSum/40;
					   phase1=phase1SUM/40;
					   GainFactor_cnt=0;
					   GainFactorSum=0;
					   phase1SUM=0;
					 					 
					   printf("Gain_factor%fE-7\r\n",Gain_factor*10000000);//��������ϵ��
					   break;
         }

         } // end of D1 true condition
      } // end of for loop

}


void Sample (void)//7ms
{	 
  char count,i,j,ii; 
	uint32_t temp;
  uint32_t value_buf[35];
	
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
						Magnitude=sqrt(real_data*real_data + imag_data*imag_data);

						Impedance1=1/(Magnitude*Gainfactor);                            //==============================/////////////
							
						value_buf[count++] =(uint32_t)(Impedance1);//��λֵ�˲���
						
						//RSP_Data_sum=RSP_Data_sum+ Impedance1; //��ֵ�˲���                                   
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
					  for(j=0;j<25-1;j++) 
		        { 
			          for (i=j+1;i<25;i++) 
                { 
                    if ( value_buf[j]>value_buf[i] ) 
                    { 
                        temp = value_buf[j]; 
                        value_buf[j] = value_buf[i]; 
                        value_buf[i] = temp; 
                    } 
                } 
            }
           new_impedance=(value_buf[10]+value_buf[11]+value_buf[12]+value_buf[13]+value_buf[14])/250;                                                             //150us
           
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
							fliter_impedance=RSP_SUM/fliter_RSPNumber;//��ƽ��		
							RSP_SUM=0;//����			
					 }
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
						
						if(PP_Distance>10)//������ϸ���ֵ�����Ƿ�����Ҫ��
						{
						    if( (RSP_Buffer[RSP_Buffer_Center-2]-RSP_Buffer[RSP_Buffer_Center])>=0&&
							    (RSP_Buffer[RSP_Buffer_Center-1]-RSP_Buffer[RSP_Buffer_Center])>=0&&
                  (RSP_Buffer[RSP_Buffer_Center+1]-RSP_Buffer[RSP_Buffer_Center])>=0&&
						      (RSP_Buffer[RSP_Buffer_Center+2]-RSP_Buffer[RSP_Buffer_Center])>=0)//ͨ������жϿ���Ϊ��ֵ��
					      {
						       for(ii=0;ii<RSP_Buffer_Number;ii++) //Ѱ�һ�����������Сֵ
							     {
								      if(RSP_Buffer[ii]<fliter_impedance_min)  fliter_impedance_min=RSP_Buffer[ii];
							     }
							     if(RSP_Buffer[RSP_Buffer_Center]==fliter_impedance_min)//�ж���ֵ�Լ����ֵ�������ȷ���Ƿ�Ϊ��ֵ��
							     {
								      
										  Peak_number++;
//										  fliter_impedance_tem-=1000;//������ֵ��С200��ͻ����ʾ  
								      PP_Distance=0;
										  impedance_min=fliter_impedance_min;
								      fliter_impedance_min =65535;
							     }
						    }
					  }
						/*============= �������� ============= */
						
						
					 Send_impedance=(fliter_impedance_tem-impedance_min)/30;
						
					 if(Send_impedance<0)
					 {
					     TX_impedance=-Send_impedance;
							 TX_impedance=(TX_impedance&0x0000ffff)|0x8000;//������ԭ��	    
							 TX_impedance=(((~TX_impedance)&0x0000ffff)|0x8000)+1;
					 }
					 else
           {
					     TX_impedance=Send_impedance;
					 }						 
//					 RSP_DATA_X[3]=TX_impedance/0X100;
//					 RSP_DATA_X[4]=TX_impedance%0X100;
					 
					 RSP_DATA_X[3]=fliter_impedance_tem/0X100;
					 RSP_DATA_X[4]=fliter_impedance_tem%0X100;
					 RSP_DATA_X[5]=Breath_rate;
					 HAL_UART_Transmit_DMA(&huart2 , RSP_DATA_X, 6);
						
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
