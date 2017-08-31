#include "sys.h"

PACK_S Data_Pack;

void SendByUart(SEND_S *send_p,unsigned char *data_p,unsigned char num)
{
	unsigned char i=0,sum=0;
	send_p->size=num+2;
	send_p->nsize=~(send_p->size);
	for(i=0;i<num;i++)
	{
		send_p->buffer[i]=data_p[i];
		sum+=data_p[i];
	}
	send_p->checksum=sum;
	while(huart2.gState == HAL_UART_STATE_BUSY);
	HAL_UART_Transmit_DMA(&huart2,(unsigned char*)send_p, num+6);	
}
void SendByUart_IT(SEND_S *send_p,unsigned char *data_p,unsigned char num)
{
	unsigned char i=0,sum=0;
	send_p->size=num+2;
	send_p->nsize=~(send_p->size);
	for(i=0;i<num;i++)
	{
		send_p->buffer[i]=data_p[i];
		sum+=data_p[i];
	}
	send_p->checksum=sum;
	while(huart2.gState == HAL_UART_STATE_BUSY);
	HAL_UART_Transmit_IT(&huart2,(unsigned char*)send_p, num+6);	
}
void Send3ECG(unsigned char Addr,short int ecg1,short int ecg2,short int ecg3)//have tested
{
	static SEND_S buff={0xAA,0xAA,0,0,0,0,{0}};
	static ECG_S ECG_buf;
	buff.addr=Addr;
	ECG_buf.ecg1=ecg1;
	ECG_buf.ecg2=ecg2;
	ECG_buf.ecg3=ecg3;
	SendByUart(&buff,(unsigned char *)&ECG_buf,sizeof(ECG_buf));
}

void SendKey(unsigned char Addr,unsigned char key)//have tested
{
	static SEND_S buff={0xAA,0xAA,0,0,0,0,{0}};
	static KEY_S key_buf;
	buff.addr=Addr;
  key_buf.key=key;
	SendByUart_IT(&buff,(unsigned char *)&key_buf,sizeof(key_buf));
}
void SendString(unsigned char Addr,unsigned char *s)
{
	unsigned char size;
	static SEND_S buff_string={0xAA,0xAA,0,0,0,0,{0}};
	buff_string.addr=Addr;
	size=strlen(s);
	SendByUart(&buff_string,s,size);
}
void SendPACK(unsigned char Addr,unsigned char size,unsigned char * p_data)
{
	static SEND_S buff={0xAA,0xAA,0,0,0,0,{0}};
	buff.addr=Addr;
	SendByUart(&buff,p_data,size);
}
unsigned char Generate_Data(PACK_S *data,unsigned char addr,unsigned char length,unsigned char *value)
{
  unsigned char size=data->size;
  if(length+size>253)return 0;
  else
  {
    data->buffer[size]=addr;
    data->buffer[size+1]=length;
    memcpy((void *)(&data->buffer[size+2]),value,length);
    data->size+=(length+2);
  }
  return length;
}






