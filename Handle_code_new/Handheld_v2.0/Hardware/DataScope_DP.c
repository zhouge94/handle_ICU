#include "DataScope_DP.h"
#include "sys.h"
unsigned char DataScope_OutPut_Buffer[400] = {0};	   //串口发送缓冲区
//函数说明：将单精度浮点数据转成4字节数据并存入指定地址
//附加说明：用户无需直接操作此函数
//target:目标单精度数据
//buf:待写入数组
//beg:指定从数组第几个元素开始写入
//函数无返回
void Float2Byte(float *target,unsigned char *buf,unsigned char beg)
{
    unsigned char *point;
    point = (unsigned char*)target;	  //得到float的地址
    buf[beg]   = point[0];
    buf[beg+1] = point[1];
    buf[beg+2] = point[2];
    buf[beg+3] = point[3];
}
 
//函数说明：将待发送通道的单精度浮点数据写入发送缓冲区
//Data：通道数据
//Channel：选择通道（1-10）
//函数无返回
void DataScope_Get_Channel_Data(float Data,unsigned char Channel)
{
	if ( (Channel > 255) || (Channel == 0) ) return;  //通道个数大于10或等于0，直接跳出，不执行函数
  else
  {
	  Float2Byte(&Data,DataScope_OutPut_Buffer,Channel*4-2);
  }	 
}


//函数说明：生成 DataScopeV1.0 能正确识别的帧格式
//Channel_Number，需要发送的通道个数
//返回发送缓冲区数据个数
//返回0表示帧格式生成失败
unsigned char DataScope_Data_Generate(unsigned char Channel_Number)
{
	unsigned char i,checksum,Send_Count;
	if ((Channel_Number > 255) || (Channel_Number == 0 )) { return 0; }  //通道个数大于255或等于0，直接跳出，不执行函数
	else
	{
		DataScope_OutPut_Buffer[0] = '$';  //帧头
		DataScope_OutPut_Buffer[1] = Channel_Number;  //帧头
		Send_Count= Channel_Number*4+2;
		checksum=0;
		for( i = 0 ; i < Send_Count; i++)
		{
			checksum+=DataScope_OutPut_Buffer[i];
		}
		DataScope_OutPut_Buffer[i++]=checksum;
		DataScope_OutPut_Buffer[i++]='\n';
		DataScope_OutPut_Buffer[i++]='\r';
		return Send_Count+3;
  }
}
void SendFloat(unsigned char Addr,float FloatData)//have tested
{
    unsigned char i = 0;
    unsigned char *p = ( unsigned char *)&FloatData;
    unsigned char sum = 0;
    static unsigned char buffer[9];
    buffer[0]='{';
    buffer[1]=Addr;
    for(i=0;i<4;i++)buffer[2+i]=*p++;
    for(i=0;i<6;i++)sum+=buffer[i];
    buffer[6]=sum;
    buffer[7]='\n';
    buffer[8]='\r';
    while(huart2.gState == HAL_UART_STATE_BUSY);
    HAL_UART_Transmit_DMA(&huart2, buffer, 8);
 }
struct ECG_S
{
	__IO short int ecg1;
	__IO short int ecg2;
	__IO short int ecg3;
};
struct SEND_S
{
	__IO unsigned char s1;
	__IO unsigned char s2;
	__IO unsigned char size;
	__IO unsigned char nsize;
	__IO unsigned char addr;
	__IO unsigned char checksum;
	__IO unsigned char buffer[256];
};
void Send3ECG(unsigned char Addr,short int ecg1,short int ecg2,short int ecg3)//have tested
{
    unsigned char i = 0;
		unsigned  char sum=0;
		static struct SEND_S buff={0xAA,0xAA,0,0,0,0,{0}};
		static struct ECG_S ECG_buf;
    static unsigned char *p=(unsigned char *)&ECG_buf;
		buff.addr=Addr;
		ECG_buf.ecg1=ecg1;
		ECG_buf.ecg2=ecg2;
		ECG_buf.ecg3=ecg3;
		buff.size=sizeof(ECG_buf)+2;
		buff.nsize=~(buff.size);
		buff.checksum=0;
		for(i=0;i<sizeof(ECG_buf);i++)
		{
			buff.buffer[i]=p[i];
			sum+=p[i];
		}
		buff.checksum=sum;
    while(huart2.gState == HAL_UART_STATE_BUSY);
    while(HAL_UART_Transmit_IT(&huart2,(unsigned char*)&buff, sizeof(ECG_buf)+6)!=HAL_OK);
}// 0xaa 0xaa addr num=3 float float float checksun end 

void SendMPU(unsigned char Addr,unsigned char accx,unsigned char accy,unsigned char accz,unsigned char acca)//have tested
{
    unsigned char i = 0;
    unsigned char *p;
    unsigned char sum = 0;
	
    static unsigned char buffer[20];
    buffer[0]=0xAA;
    buffer[1]=0xAA;
    buffer[2]=Addr;
    buffer[3]=1;

    buffer[4]=accx;
    buffer[5]=accy;
    buffer[6]=accz;
    buffer[7]=acca;
	
    for(i=0;i<8;i++)sum+=buffer[i];
    buffer[8]=sum;
    while(huart2.gState == HAL_UART_STATE_BUSY);
    HAL_UART_Transmit_DMA(&huart2, buffer, 9);
}// 0xaa 0xaa addr num=1 u8 u8 u8 u8 checksun end 








