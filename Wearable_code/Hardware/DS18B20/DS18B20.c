#include "DS18b20.h"
//#include "SDCard.h"

uint8_t ID_Buff[DS18B20_NUM][8]; 

uint8_t TEM_DATA_X[14]={0XAA,0XAA,0X05,0X09,0XF6,0X09,0XF6,0X09,0XF6,0X09,0XF6,0X09,0XF6,0X00};

uint8_t DS18b20NUM=0;

extern uint8_t SDCardValid;

extern UART_HandleTypeDef huart2;

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** 函数名称: Delay_Ms_Us
** 功能描述: 延时1us (可通过仿真来判断他的准确度)
** 参数描述：time (us) 注意time<65535				 
** 作  　者: Dream
** 日　  期: 2011年6月20日
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void Delay_Us(uint16_t time)   //延时函数
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
* 函 数 名         : Ds18b20Init
* 函数功能		   : 初始化
* 输    入         : 无
* 输    出         : 初始化成功返回1，失败返回0
*******************************************************************************/
uint8_t DS18B20_Reset(void)  
{  
    uint8_t i = 0;  
      
    WR_DS18B20_0;			 //将总线拉低480us~960us
	  Delay_Us(642);    //延时642us
	  WR_DS18B20_1;			//然后拉高总线，如果DS18B20做出反应会将在15us~60us后总线拉低 
    while(RD_DS18B20)    //等待低电平应答信号  
    {  
        i ++;  
        if(i>5)//等待>5MS
		    {
			      printf("DS18B20 error!\r\n"); 
					  return 0;//初始化失败
		    }
		    Delay_Us(100);   

    }  
    Delay_Us(250);          //跳过回复信号  
    return 1;//    检测到DS18B20,并且初始化成功  
}  


void DS18B20_WriteBit(uint8_t bit)
{
     WR_DS18B20_0;	     	  //每写入一位数据之前先把总线拉低1us
		 Delay_Us(1);
		 if(bit & 0x01)			//然后写入一个数据，从最低位开始
		 WR_DS18B20_1;
		 else
		 WR_DS18B20_0;
		 Delay_Us(68); //延时68us，持续时间最少60us
		 WR_DS18B20_1;	//然后释放总线，至少1us给总线恢复时间才能接着写入第二个数值
		 Delay_Us(5);
}
/*******************************************************************************
* 函 数 名         : Ds18b20WriteByte
* 函数功能		   : 向18B20写入一个字节
* 输    入         : com
* 输    出         : 无
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
	
	   WR_DS18B20_0;//先将总线拉低1us
		 Delay_Us(1);
		 WR_DS18B20_1;//然后释放总线
		 Delay_Us(6);//延时6us等待数据稳定
	 
	   if(RD_DS18B20)   //读取数据,读取后大约延时40-45us  
        data = 0x01;  
     Delay_Us(40);  
     return data;  
	   
}
/*******************************************************************************
* 函 数 名         : Ds18b20ReadByte
* 函数功能		     : 读取一个字节
* 输    入         : com
* 输    出         : 无
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
*函数        :    s16 DS18B20_ReadTemper(void) 
*功能        :    读取DS18B20温度 
*参数        :    无 
*返回        :    温度值 
*依赖 :   底层宏定义 
*作者        :    cp1300@139.com 
*时间     :   20120917 
*最后修改时间:    20120917 
*说明        :温度值扩大了100倍,温度值是个有符号数. 
*************************************************************************************************************************/  
uint16_t DS18B20_ReadTemper(void)  
{  
    uint8_t th, tl;  
    uint16_t data;  
      
    if(DS18B20_Reset() == 0)      
    {  
        return 0xffff;  //返回错误  
    }  
  
    DS18B20_WriteData(0xcc);    //跳过读序列号  
    DS18B20_WriteData(0x44);    //启动温度转换  
    DS18B20_Reset();  
    DS18B20_WriteData(0xcc);    //跳过读序列号  
    DS18B20_WriteData(0xbe);    //读取温度  
    tl = DS18B20_ReadData();    //读取低八位  
    th = DS18B20_ReadData();    //读取高八位  
    data = th;  
    data <<= 8;  
    data |= tl;  
    data *= 6.25;               //温度值扩大100倍，精确到2位小数  
      
    return data;  
}  

/************************************************************************************************************************* 
*函数        :    __inline u8 DS18B20_Read2Bit(void) 
*功能        :    读取DS18B20 2bit数据 
*参数        :    无 
*返回        :    数据 
*依赖     :   底层宏定义 
*作者        :    cp1300@139.com 
*时间         :   20120917 
*最后修改时间:    20120917 
*说明        :无 
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
*函数         :   u8 DS18B20_SearchROM(u8 (*pID)[8],u8 Num) 
*功能         :   搜索DS18B20 ROM 
*参数         :   pID:DS18B20 ID缓冲区指针 
                    Num:DS18B20数目,必须事先知道 
*返回         :   搜索到的DS18B20数目 
*依赖         :   底层宏定义 
*作者         :   cp1300@139.com 
*时间             :   2013-04-17 
*最后修改时间 :   2013-04-17 
*说明         :   程序必须事先知道DS18B20数目,搜索的数目只会小于这个数目 
                    代码移植于互联网 
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
                k=DS18B20_Read2Bit();//读两位数据  
                k=k&0x03;  
                s>>=1;  
                if(k==0x01)//01读到的数据为0 写0 此位为0的器件响应  
                {             
                    DS18B20_WriteBit (0);  
                    ss[(m*8+n)]=0;  
                }  
                else if(k==0x02)//读到的数据为1 写1 此位为1的器件响应  
                {  
                    s=s|0x80;  
                    DS18B20_WriteBit (1);  
                    ss[(m*8+n)]=1;  
                }  
                else if(k==0x00)//读到的数据为00 有冲突位 判断冲突位   
                {               //如果冲突位大于栈顶写0 小于栈顶写以前数据 等于栈顶写1  
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
                    return num; //搜索完成,//返回搜索到的个数  
                }  
            }  
            pID[num][m]=s;        
        }  
        num=num+1;  
    }  
    while(zhan[l]!=0&&(num<MAXNUM));   
      
    return num;     //返回搜索到的个数  
}  
/************************************************************************************************************************* 
*函数         :   s16 DS18B20_ReadDesignateTemper(u8 pID[8]) 
*功能         :   读取指定ID的DS18B20温度 
*参数         :   pID:DS18B20 ID,必须事先知道,如果不知道请启动ROM搜索 
*返回         :   温度值 
*依赖         :   底层宏定义 
*作者         :   cp1300@139.com 
*时间             :   2013-04-17 
*最后修改时间 :   2013-04-17 
*说明         :   温度值扩大了100倍,温度值是个有符号数. 
*************************************************************************************************************************/  
uint16_t DS18B20_ReadDesignateTemper(uint8_t pID[8])  
{  
    uint8_t th, tl;  
    uint16_t data;  
      
    if(DS18B20_Reset() == 0)      
    {  
        return 0xffff;              //返回错误  
    }  
  
    DS18B20_WriteData(0xcc);        //跳过读序列号  
    DS18B20_WriteData(0x44);        //启动温度转换  
    DS18B20_Reset();  
    DS18B20_WriteData(0x55);        //发送序列号匹配命令  
    for(data = 0;data < 8;data ++)   //发送8byte的序列号     
    {  
       DS18B20_WriteData(pID[data]);  
    }  
    Delay_Us(10);  
    DS18B20_WriteData(0xbe);    //读取温度  
    tl = DS18B20_ReadData();    //读取低八位  
    th = DS18B20_ReadData();    //读取高八位  
    data = th;  
    data <<= 8;  
    data |= tl;  
    data *= 6.25;               //温度值扩大100倍，精确到2位小数  
      
    return data;  
}

/************************************************************************************************************************* 
*函数         :   s16 DS18B20_ReadDesignateTemper(u8 pID[8]) 
*功能         :   读取指定ID的DS18B20温度 
*参数         :   pID:DS18B20 ID,必须事先知道,如果不知道请启动ROM搜索 
*返回         :   温度值 
*依赖         :   底层宏定义 
*作者         :   cp1300@139.com 
*时间             :   2013-04-17 
*最后修改时间 :   2013-04-17 
*说明         :   输出十六进制温度数据，高八位低八位
*************************************************************************************************************************/  
uint16_t DS18B20_ReadDesignateTemper_HEX(uint8_t pID[8])  
{  
    uint8_t th, tl;  
    uint16_t data;  
      
    if(DS18B20_Reset() == 0)      
    {  
        return 0xffff;              //返回错误  
    }  
  
    DS18B20_WriteData(0xcc);        //跳过读序列号  
    DS18B20_WriteData(0x44);        //启动温度转换  
    DS18B20_Reset();  
    DS18B20_WriteData(0x55);        //发送序列号匹配命令  
    for(data = 0;data < 8;data ++)   //发送8byte的序列号     
    {  
       DS18B20_WriteData(pID[data]);  
    }  
    Delay_Us(10);  
    DS18B20_WriteData(0xbe);    //读取温度  
    tl = DS18B20_ReadData();    //读取低八位  
    th = DS18B20_ReadData();    //读取高八位  
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
			//printf("\r\n DS18B20 No%d Temp:%.2d℃",i,temp/100); 
//			TEM_DATA_X[2*(i+1)+1] = (temp&0x0000ff00)/0x100;		//温度值共16位，高字节放到温度数据发送缓冲区
//			TEM_DATA_X[2*(i+1)+2] = temp&0x000000ff;		//低字节
		  TEM_DATA_X[2*(i+1)+1] = (0x9f6&0x0000ff00)/0x100;		//温度值共16位，高字节放到温度数据发送缓冲区
			TEM_DATA_X[2*(i+1)+2] = 0x9f6&0x000000ff;		//低字节
	}	
	HAL_UART_Transmit_DMA(&huart2 , TEM_DATA_X, 14);
	
	/*************** 将体温数据存入SD卡缓存区******************/
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
		  
			printf("\r\n DS18B20 No%d Temp:%.2f℃",i,(float)temp/100); 
	}
  printf("\r\n "); 	
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

void DS18B20_Search_ROM(void)
{  
	 uint8_t i,j,num;
	 uint16_t temp;

	  DS18B20_IOInit();
	  num = DS18B20_SearchROM(ID_Buff,DS18B20_NUM);//搜索DS18B20 ROM   
    printf("总线上实际挂载DS18B20数量: %d\r\n",DS18B20_NUM);  
    printf("搜索到的DS18B20数量: %d\r\n",num); 

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
//        printf("\r\n DS18B20 No%d Temp:%.1f℃",i,0.05+temp/100); 				
//        if(i == num)  
//        {  
//            i = 0;  
//            printf("\r\n");  
//        }				
//  
//    }  
//			
	
}


