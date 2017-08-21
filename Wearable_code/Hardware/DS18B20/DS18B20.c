#include "DS18b20.h"
//#include "SDCard.h"

uint8_t ID_Buff[DS18B20_NUM][8]; 

uint8_t TEM_DATA_X[14]={0XAA,0XAA,0X05,0X09,0XF6,0X09,0XF6,0X09,0XF6,0X09,0XF6,0X09,0XF6,0X00};

uint8_t DS18b20NUM=0;

extern uint8_t SDCardValid;

extern UART_HandleTypeDef huart2;

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** ��������: Delay_Ms_Us
** ��������: ��ʱ1us (��ͨ���������ж�����׼ȷ��)
** ����������time (us) ע��time<65535				 
** ��  ����: Dream
** �ա�  ��: 2011��6��20��
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void Delay_Us(uint16_t time)   //��ʱ����
{ 
	uint16_t i,j;
	for(i=0;i<time;i++)
  		for(j=0;j<11;j++);
}

void DS18B20_IOInit()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	__GPIOB_CLK_ENABLE();
	
	GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/*******************************************************************************
* �� �� ��         : Ds18b20Init
* ��������		   : ��ʼ��
* ��    ��         : ��
* ��    ��         : ��ʼ���ɹ�����1��ʧ�ܷ���0
*******************************************************************************/
uint8_t DS18B20_Reset(void)  
{  
    uint8_t i = 0;  
      
    WR_DS18B20_0;			 //����������480us~960us
	  Delay_Us(642);    //��ʱ642us
	  WR_DS18B20_1;			//Ȼ���������ߣ����DS18B20������Ӧ�Ὣ��15us~60us���������� 
    while(RD_DS18B20)    //�ȴ��͵�ƽӦ���ź�  
    {  
        i ++;  
        if(i>5)//�ȴ�>5MS
		    {
			      printf("DS18B20 error!\r\n"); 
					  return 0;//��ʼ��ʧ��
		    }
		    Delay_Us(100);   

    }  
    Delay_Us(250);          //�����ظ��ź�  
    return 1;//    ��⵽DS18B20,���ҳ�ʼ���ɹ�  
}  


void DS18B20_WriteBit(uint8_t bit)
{
     WR_DS18B20_0;	     	  //ÿд��һλ����֮ǰ�Ȱ���������1us
		 Delay_Us(1);
		 if(bit & 0x01)			//Ȼ��д��һ�����ݣ������λ��ʼ
		 WR_DS18B20_1;
		 else
		 WR_DS18B20_0;
		 Delay_Us(68); //��ʱ68us������ʱ������60us
		 WR_DS18B20_1;	//Ȼ���ͷ����ߣ�����1us�����߻ָ�ʱ����ܽ���д��ڶ�����ֵ
		 Delay_Us(5);
}
/*******************************************************************************
* �� �� ��         : Ds18b20WriteByte
* ��������		   : ��18B20д��һ���ֽ�
* ��    ��         : com
* ��    ��         : ��
*******************************************************************************/

void DS18B20_WriteData(uint8_t data)  
{
	uint16_t  j;
	for(j=0; j<8; j++)
	{
		 DS18B20_WriteBit(data); 
		data >>= 1;
	}
}

uint8_t DS18B20_ReadBit(void) 
{
     uint8_t data=0;
	
	   WR_DS18B20_0;//�Ƚ���������1us
		 Delay_Us(1);
		 WR_DS18B20_1;//Ȼ���ͷ�����
		 Delay_Us(6);//��ʱ6us�ȴ������ȶ�
	 
	   if(RD_DS18B20)   //��ȡ����,��ȡ���Լ��ʱ40-45us  
        data = 0x01;  
     Delay_Us(40);  
     return data;  
	   
}
/*******************************************************************************
* �� �� ��         : Ds18b20ReadByte
* ��������		     : ��ȡһ���ֽ�
* ��    ��         : com
* ��    ��         : ��
*******************************************************************************/

uint8_t DS18B20_ReadData(void) 
{
	uint8_t i, data;
      
    for(i = 0;i < 8;i ++)  
    {  
  
        data >>= 1;  
        if(DS18B20_ReadBit())  
            data |= 0x80;  
    }  
    return data;  
}
/************************************************************************************************************************* 
*����        :    s16 DS18B20_ReadTemper(void) 
*����        :    ��ȡDS18B20�¶� 
*����        :    �� 
*����        :    �¶�ֵ 
*���� :   �ײ�궨�� 
*����        :    cp1300@139.com 
*ʱ��     :   20120917 
*����޸�ʱ��:    20120917 
*˵��        :�¶�ֵ������100��,�¶�ֵ�Ǹ��з�����. 
*************************************************************************************************************************/  
uint16_t DS18B20_ReadTemper(void)  
{  
    uint8_t th, tl;  
    uint16_t data;  
      
    if(DS18B20_Reset() == 0)      
    {  
        return 0xffff;  //���ش���  
    }  
  
    DS18B20_WriteData(0xcc);    //���������к�  
    DS18B20_WriteData(0x44);    //�����¶�ת��  
    DS18B20_Reset();  
    DS18B20_WriteData(0xcc);    //���������к�  
    DS18B20_WriteData(0xbe);    //��ȡ�¶�  
    tl = DS18B20_ReadData();    //��ȡ�Ͱ�λ  
    th = DS18B20_ReadData();    //��ȡ�߰�λ  
    data = th;  
    data <<= 8;  
    data |= tl;  
    data *= 6.25;               //�¶�ֵ����100������ȷ��2λС��  
      
    return data;  
}  

/************************************************************************************************************************* 
*����        :    __inline u8 DS18B20_Read2Bit(void) 
*����        :    ��ȡDS18B20 2bit���� 
*����        :    �� 
*����        :    ���� 
*����     :   �ײ�궨�� 
*����        :    cp1300@139.com 
*ʱ��         :   20120917 
*����޸�ʱ��:    20120917 
*˵��        :�� 
*************************************************************************************************************************/  
uint8_t DS18B20_Read2Bit(void)  
{  
    uint8_t i,data = 0;  
      
    for(i = 0;i < 2;i ++)  
    {  
  
        data <<= 1;  
        if(DS18B20_ReadBit())  
            data |= 1;  
    }  
    return data;  
}  
/************************************************************************************************************************* 
*����         :   u8 DS18B20_SearchROM(u8 (*pID)[8],u8 Num) 
*����         :   ����DS18B20 ROM 
*����         :   pID:DS18B20 ID������ָ�� 
                    Num:DS18B20��Ŀ,��������֪�� 
*����         :   ��������DS18B20��Ŀ 
*����         :   �ײ�궨�� 
*����         :   cp1300@139.com 
*ʱ��             :   2013-04-17 
*����޸�ʱ�� :   2013-04-17 
*˵��         :   �����������֪��DS18B20��Ŀ,��������Ŀֻ��С�������Ŀ 
                    ������ֲ�ڻ����� 
*************************************************************************************************************************/  
uint8_t DS18B20_SearchROM(uint8_t (*pID)[8],uint8_t Num)  
{   
    unsigned char k,l,chongtuwei,m,n;  
    unsigned char zhan[(MAXNUM)];  
    unsigned char ss[64];  
    uint8_t num = 0;  
    l=0;  
      
    do  
    {  
        DS18B20_Reset();  
        DS18B20_WriteData(0xf0);      
        for(m=0;m<8;m++)  
        {  
            unsigned char s=0;  
            for(n=0;n<8;n++)  
            {  
                k=DS18B20_Read2Bit();//����λ����  
                k=k&0x03;  
                s>>=1;  
                if(k==0x01)//01����������Ϊ0 д0 ��λΪ0��������Ӧ  
                {             
                    DS18B20_WriteBit (0);  
                    ss[(m*8+n)]=0;  
                }  
                else if(k==0x02)//����������Ϊ1 д1 ��λΪ1��������Ӧ  
                {  
                    s=s|0x80;  
                    DS18B20_WriteBit (1);  
                    ss[(m*8+n)]=1;  
                }  
                else if(k==0x00)//����������Ϊ00 �г�ͻλ �жϳ�ͻλ   
                {               //�����ͻλ����ջ��д0 С��ջ��д��ǰ���� ����ջ��д1  
                    chongtuwei=m*8+n+1;                   
                    if(chongtuwei>zhan[l])  
                    {                         
                        DS18B20_WriteBit (0);  
                        ss[(m*8+n)]=0;                                                
                        zhan[++l]=chongtuwei;                         
                    }  
                    else if(chongtuwei<zhan[l])  
                    {  
                        s=s|((ss[(m*8+n)]&0x01)<<7);  
                        DS18B20_WriteBit (ss[(m*8+n)]);  
                    }  
                    else if(chongtuwei==zhan[l])  
                    {  
                        s=s|0x80;  
                        DS18B20_WriteBit (1);  
                        ss[(m*8+n)]=1;  
                        l=l-1;  
                    }  
                }  
                else  
                {  
                    return num; //�������,//�����������ĸ���  
                }  
            }  
            pID[num][m]=s;        
        }  
        num=num+1;  
    }  
    while(zhan[l]!=0&&(num<MAXNUM));   
      
    return num;     //�����������ĸ���  
}  
/************************************************************************************************************************* 
*����         :   s16 DS18B20_ReadDesignateTemper(u8 pID[8]) 
*����         :   ��ȡָ��ID��DS18B20�¶� 
*����         :   pID:DS18B20 ID,��������֪��,�����֪��������ROM���� 
*����         :   �¶�ֵ 
*����         :   �ײ�궨�� 
*����         :   cp1300@139.com 
*ʱ��             :   2013-04-17 
*����޸�ʱ�� :   2013-04-17 
*˵��         :   �¶�ֵ������100��,�¶�ֵ�Ǹ��з�����. 
*************************************************************************************************************************/  
uint16_t DS18B20_ReadDesignateTemper(uint8_t pID[8])  
{  
    uint8_t th, tl;  
    uint16_t data;  
      
    if(DS18B20_Reset() == 0)      
    {  
        return 0xffff;              //���ش���  
    }  
  
    DS18B20_WriteData(0xcc);        //���������к�  
    DS18B20_WriteData(0x44);        //�����¶�ת��  
    DS18B20_Reset();  
    DS18B20_WriteData(0x55);        //�������к�ƥ������  
    for(data = 0;data < 8;data ++)   //����8byte�����к�     
    {  
       DS18B20_WriteData(pID[data]);  
    }  
    Delay_Us(10);  
    DS18B20_WriteData(0xbe);    //��ȡ�¶�  
    tl = DS18B20_ReadData();    //��ȡ�Ͱ�λ  
    th = DS18B20_ReadData();    //��ȡ�߰�λ  
    data = th;  
    data <<= 8;  
    data |= tl;  
    data *= 6.25;               //�¶�ֵ����100������ȷ��2λС��  
      
    return data;  
}

/************************************************************************************************************************* 
*����         :   s16 DS18B20_ReadDesignateTemper(u8 pID[8]) 
*����         :   ��ȡָ��ID��DS18B20�¶� 
*����         :   pID:DS18B20 ID,��������֪��,�����֪��������ROM���� 
*����         :   �¶�ֵ 
*����         :   �ײ�궨�� 
*����         :   cp1300@139.com 
*ʱ��             :   2013-04-17 
*����޸�ʱ�� :   2013-04-17 
*˵��         :   ���ʮ�������¶����ݣ��߰�λ�Ͱ�λ
*************************************************************************************************************************/  
uint16_t DS18B20_ReadDesignateTemper_HEX(uint8_t pID[8])  
{  
    uint8_t th, tl;  
    uint16_t data;  
      
    if(DS18B20_Reset() == 0)      
    {  
        return 0xffff;              //���ش���  
    }  
  
    DS18B20_WriteData(0xcc);        //���������к�  
    DS18B20_WriteData(0x44);        //�����¶�ת��  
    DS18B20_Reset();  
    DS18B20_WriteData(0x55);        //�������к�ƥ������  
    for(data = 0;data < 8;data ++)   //����8byte�����к�     
    {  
       DS18B20_WriteData(pID[data]);  
    }  
    Delay_Us(10);  
    DS18B20_WriteData(0xbe);    //��ȡ�¶�  
    tl = DS18B20_ReadData();    //��ȡ�Ͱ�λ  
    th = DS18B20_ReadData();    //��ȡ�߰�λ  
    data = th;  
    data <<= 8;  
    data |= tl;  
      
    return data;  
}
void GET_DS18B20(void)
{
	uint8_t i;
	uint16_t temp;
	
	for(i=0;i<DS18b20NUM;i++)
	{
			temp = DS18B20_ReadDesignateTemper(ID_Buff[i]);
			//printf("\r\n DS18B20 No%d Temp:%.2d��",i,temp/100); 
//			TEM_DATA_X[2*(i+1)+1] = (temp&0x0000ff00)/0x100;		//�¶�ֵ��16λ�����ֽڷŵ��¶����ݷ��ͻ�����
//			TEM_DATA_X[2*(i+1)+2] = temp&0x000000ff;		//���ֽ�
		  TEM_DATA_X[2*(i+1)+1] = (0x9f6&0x0000ff00)/0x100;		//�¶�ֵ��16λ�����ֽڷŵ��¶����ݷ��ͻ�����
			TEM_DATA_X[2*(i+1)+2] = 0x9f6&0x000000ff;		//���ֽ�
	}	
	HAL_UART_Transmit_DMA(&huart2 , TEM_DATA_X, 14);
	
	/*************** ���������ݴ���SD��������******************/
	//if(SDCardValid==1)
	 // Write_Data_To_SDBuffer(TEM_DATA_X, 14); 
}

void READ_DS18B20(void)
{
	uint8_t i;
	uint16_t temp;
	
	for(i=0;i<DS18b20NUM;i++)
	{
			temp = DS18B20_ReadDesignateTemper(ID_Buff[i]);
		  
			printf("\r\n DS18B20 No%d Temp:%.2f��",i,(float)temp/100); 
	}
  printf("\r\n "); 	
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

void DS18B20_Search_ROM(void)
{  
	 uint8_t i,j,num;
	 uint16_t temp;

	  DS18B20_IOInit();
	  num = DS18B20_SearchROM(ID_Buff,DS18B20_NUM);//����DS18B20 ROM   
    printf("������ʵ�ʹ���DS18B20����: %d\r\n",DS18B20_NUM);  
    printf("��������DS18B20����: %d\r\n",num); 

    for(i = 0;i < num;i ++)  
    {  
        printf("\r\n DS18B20 No%d ID: ",i);  
        for(j = 0;j < 8;j ++)  
        {  
            printf("%02X ",ID_Buff[i][j]);  
        }  
    }  
//  while(1)
//		
//	{
//	    GET_DS18B20();
//		  HAL_Delay(500); 
//	}
	
	
//    i = 0;  
//    while(1)  
//    {  
//  
// 
//        HAL_Delay(500);  
//        temp = DS18B20_ReadDesignateTemper(ID_Buff[i ++]);  
//        printf("\r\n DS18B20 No%d Temp:%.1f��",i,0.05+temp/100); 				
//        if(i == num)  
//        {  
//            i = 0;  
//            printf("\r\n");  
//        }				
//  
//    }  
//			
	
}


