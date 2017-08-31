#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"	 

#include  <math.h>    //Keil library  //==========================================
#include "stdlib.h"

#include "AD5933.h"

uint8_t AD5933_CMD_BUF_1[1]={0X10}; //复位AD5933
uint8_t AD5933_CMD_BUF_2[1]={0X00};
uint8_t AD5933_CMD_BUF_3[1]={0X91};////启动温度测量

uint8_t AD5933_CMD_BUF_4[1]={0X01};
uint8_t AD5933_CMD_BUF_5[1]={0X10};
uint8_t AD5933_CMD_BUF_6[1]={0X8F};

uint8_t AD5933_CMD_BUF_7[1]={0X33};
uint8_t AD5933_CMD_BUF_8[1]={0X33};
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

uint16_t Imagdata;
uint16_t Realdata;

double imag_data;
double real_data;

float Magnitude,Impedance1,ImpedanceSUM;//幅值
double Gainfactor=5.966067195828e-009 ,Gain_factor,GainFactorSum=0;//系数，阻抗 官方评估板 Gainfactor=5.966067195828e-009
float Impedance;//系数，阻抗

double new_impedance;
double phase,phaseSUM,phase1,phase1SUM;
unsigned int phase1_cnt,phase_cnt;//相位采样点数
unsigned int sample_cnt=0;//采样求平均值的点数
unsigned int GainFactor_cnt=0;//计算增益时采样的点数

uint8_t START_MEA_RSP=0;

uint8_t RSP_DATA_X[6]={0XAA,0XAA,0X03,0X00,0X00,0X00};

uint8_t  fliterBuffer_ok=0;//滤波缓存区满标志位
uint8_t  fliterBuffer_cnt=0;//滤波缓存区数据计数
uint16_t fliterBufferRSP[32]={0};//呼吸数据缓冲区
float    RSP_SUM  =0;
uint8_t  fliter_RSPNumber=5;//选择进行滑动平均滤波的数据个数
uint16_t fliter_impedance=0;//经过滑动平均滤波之后的呼吸数据
uint16_t fliter_impedance_min=65535;//经过滑动平均滤波之后的呼吸数据最小值

uint8_t  Buffer_ok;
uint16_t RSP_Buffer[64]={0};//呼吸数据缓冲区
uint8_t  RSP_Buffer_cnt=0;//缓存区呼吸数据计数
uint8_t  RSP_Buffer_Number=11;//选择进行峰值检测的缓存区数据个数
uint8_t  RSP_Buffer_Center=0;//缓存区中心位置
uint8_t  PP_Distance=0;//峰峰值之间的间隔
uint8_t  FIND_peak=0;//发现一个峰值
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
	
	  //复位AD5933
	  while(HAL_I2C_Mem_Write_DMA(&hi2c1, AD5933_addr, 0X81, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_1, 1)!=HAL_OK);
	  while(HAL_I2C_Mem_Write_DMA(&hi2c1, AD5933_addr, 0X81, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_2, 1)!=HAL_OK);
	 
	  //启动测量
  	while(HAL_I2C_Mem_Write_DMA(&hi2c1, AD5933_addr, 0X80, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_3, 1)!=HAL_OK);
	  
	  //等待转换完成
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
	
		printf("AD5933 Temputer=%.2f℃\r\n\r\n",temputer);//
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
            
					  if(Realdata>=0X8000&&Realdata<=0XFFFF)//数据为负的
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
						if(Imagdata>=0X8000&&Imagdata<=0XFFFF)//数据为负的
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
						
            	
//						if((real_data>0)&&(imag_data>0))//第一象限
//						{
//						    phase1=(atan(imag_data/real_data))*180/3.141592654;
//						}
//						else if((real_data>0)&&(imag_data<0))//第四象限
//						{
//						     phase1=360+(atan(imag_data/real_data))*180/3.141592654;
//						}
//						else if((real_data<0)&&(imag_data<0))//第三象限
//						{
//						     phase1=180+(atan(imag_data/real_data))*180/3.141592654;
//						}
//						else if((real_data<0)&&(imag_data>0))//第二象限
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
						    while(HAL_I2C_Mem_Write_DMA(&hi2c1, AD5933_addr, 0X80, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_23, 1)!=HAL_OK);//递增频率
						
						}
						else
						   while(HAL_I2C_Mem_Write_DMA(&hi2c1, AD5933_addr, 0X80, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_24, 1)!=HAL_OK);//重复频率
						
         } // end of D2 test condition

         else // End of frequency sweep exit loop
         {
             Gain_factor=GainFactorSum/40;
					   phase1=phase1SUM/40;
					   GainFactor_cnt=0;
					   GainFactorSum=0;
					   phase1SUM=0;
					 					 
					   printf("Gain_factor%fE-7\r\n",Gain_factor*10000000);//发送增益系数
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
	
	while(HAL_I2C_Mem_Write_DMA(&hi2c1, AD5933_addr, 0X80, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_21, 1)!=HAL_OK){};//以起始频率初始化

  while(HAL_I2C_Mem_Write_DMA(&hi2c1, AD5933_addr, 0X80, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_22, 1)!=HAL_OK);//启动频率扫描
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
            imag_byte_low =  AD5933_read (0x97);    
					                                          //==============================/////////////
					 
					 
                                                                         //==============================/////////////
            Realdata = real_byte_high*0x100 +real_byte_low; 					 
					  if(Realdata>32767) //将原码变为补码
              real_data=Realdata-65536;
            else
              real_data=Realdata;							
						
						Imagdata = imag_byte_high*0x100 +imag_byte_low;	            						
					  if(Imagdata>32767) //将原码变为补码
              imag_data=Imagdata-65536;
						else
							imag_data=Imagdata;
						Magnitude=sqrt(real_data*real_data + imag_data*imag_data);

						Impedance1=1/(Magnitude*Gainfactor);                            //==============================/////////////
							
						value_buf[count++] =(uint32_t)(Impedance1);//中位值滤波法
						
						//RSP_Data_sum=RSP_Data_sum+ Impedance1; //均值滤波法                                   
						                                                                //==============================/////////////
					  while(HAL_I2C_Mem_Write_DMA(&hi2c1, AD5933_addr, 0X80, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_23, 1)!=HAL_OK);//递增频率  300us
						
						//while(HAL_I2C_Mem_Write_DMA(&hi2c1, AD5933_addr, 0X80, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_24, 1)!=HAL_OK);//重复频率
         } // end of D2 test condition

         else // End of frequency sweep exit loop
         {
            while(HAL_I2C_Mem_Write_DMA(&hi2c1, AD5933_addr, 0X80, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_25, 1)!=HAL_OK); //待机模式 PGA=X1
					  //impedance=RSP_Data_sum/20;//均值滤波法
					 
					 //impedance=0X456789;//测试数据                                                              //150us
						
					 //中位值滤波
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
           
					/*============= 滑动平均滤波 ============= */
            if(fliterBuffer_cnt==fliter_RSPNumber)//滤波缓存区满，置位标志位fliterBuffer_ok
					  {
					    fliterBuffer_ok=1;
					  }
						
					 if(fliterBuffer_ok==0)//滤波缓存区数据未满
					 {
					    fliterBufferRSP[fliterBuffer_cnt++]=new_impedance;//将新采集的呼吸数据放到缓冲区
					 }
					 else//缓存区数据已满，开始进行滑动平均滤波处理
					 {
					    for(ii=1;ii<fliter_RSPNumber;ii++)//通过移位操作将最先进入缓存区的数据移除，同时将新采集的数据放入缓存区
						  {
							   fliterBufferRSP[ii-1]=fliterBufferRSP[ii];
							}
						  fliterBufferRSP[fliter_RSPNumber-1]=new_impedance;//将新采集的红外数据放到缓冲区最末尾
							for(ii=0;ii<fliter_RSPNumber;ii++)//求和
						  {
								 RSP_SUM+=fliterBufferRSP[ii];
							}
							fliter_impedance=RSP_SUM/fliter_RSPNumber;//求平均		
							RSP_SUM=0;//清零			
					 }
 					/*============= 峰值检测 ============= */
					if(RSP_Buffer_cnt==RSP_Buffer_Number)//峰值检测缓存区数据已满，置位标志位Buffer_ok
					{
					    Buffer_ok=1;
					}
					if(Buffer_ok==0)//峰值检测缓存区数据未满
					{
					    RSP_Buffer[RSP_Buffer_cnt++]=fliter_impedance;//将最新滤波之后的数据放到缓冲区最末尾
					} 
					else
					{
					    for(ii=1;ii<RSP_Buffer_Number;ii++)//通过移位操作将最先进入缓存区的数据移除，同时将新采集的数据放入缓存区
						  {							   								
								 RSP_Buffer[ii-1]=RSP_Buffer[ii];								
							}
							RSP_Buffer[RSP_Buffer_Number-1]=fliter_impedance;//将最新滤波之后的数据放到缓冲区最末尾
					}
					RSP_Buffer_Center=(RSP_Buffer_Number-1)/2; 
					
					if(Buffer_ok==1)//数据缓存区满，开始处理数据
					{
						fliter_impedance_tem=RSP_Buffer[RSP_Buffer_Center];
						
						if(PP_Distance>10)//检测与上个峰值点间距是否满足要求
						{
						    if( (RSP_Buffer[RSP_Buffer_Center-2]-RSP_Buffer[RSP_Buffer_Center])>=0&&
							    (RSP_Buffer[RSP_Buffer_Center-1]-RSP_Buffer[RSP_Buffer_Center])>=0&&
                  (RSP_Buffer[RSP_Buffer_Center+1]-RSP_Buffer[RSP_Buffer_Center])>=0&&
						      (RSP_Buffer[RSP_Buffer_Center+2]-RSP_Buffer[RSP_Buffer_Center])>=0)//通过差分判断可能为峰值点
					      {
						       for(ii=0;ii<RSP_Buffer_Number;ii++) //寻找缓存区数据最小值
							     {
								      if(RSP_Buffer[ii]<fliter_impedance_min)  fliter_impedance_min=RSP_Buffer[ii];
							     }
							     if(RSP_Buffer[RSP_Buffer_Center]==fliter_impedance_min)//判断阈值以及峰峰值间隔，来确认是否为峰值点
							     {
								      
										  Peak_number++;
//										  fliter_impedance_tem-=1000;//将波谷值减小200，突出显示  
								      PP_Distance=0;
										  impedance_min=fliter_impedance_min;
								      fliter_impedance_min =65535;
							     }
						    }
					  }
						/*============= 发送数据 ============= */
						
						
					 Send_impedance=(fliter_impedance_tem-impedance_min)/30;
						
					 if(Send_impedance<0)
					 {
					     TX_impedance=-Send_impedance;
							 TX_impedance=(TX_impedance&0x0000ffff)|0x8000;//负数的原码	    
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

//初始化AD5933，根据需要请参考pdf进行修改************************
void Init_AD5933(void)
{
  while(HAL_I2C_Mem_Write_DMA(&hi2c1, AD5933_addr, 0X80, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_4, 1)!=HAL_OK);
	while(HAL_I2C_Mem_Write_DMA(&hi2c1, AD5933_addr, 0X81, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_5, 1)!=HAL_OK);//复位
	while(HAL_I2C_Mem_Write_DMA(&hi2c1, AD5933_addr, 0X81, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_6, 1)!=HAL_OK);//选择外部系统时钟

	
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

  while(HAL_I2C_Mem_Write_DMA(&hi2c1, AD5933_addr, 0X80, I2C_MEMADD_SIZE_8BIT, AD5933_CMD_BUF_22, 1)!=HAL_OK);//启动频率扫描


}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
