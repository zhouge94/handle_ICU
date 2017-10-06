#include "my_uart.h"
#include "my_msg.h"
struct REV_BUF
{/*{{{*/
	unsigned char s1;
	unsigned char s2;
	unsigned char size;
	unsigned char nsize;
	unsigned char addr;
	unsigned char checksum;
	unsigned char buffer[256];
};/*}}}*/
struct REV_BUF  Rbuff;
struct ECG_REV
{/*{{{*/
	short int ecg1;
	short int ecg2;
	short int ecg3;
};/*}}}*/
struct ECG_REV ecg;
struct MPU_REV
{/*{{{*/
	short int ax;
	short int ay;
	short int az;
};/*}}}*/
struct MPU_REV mpu;
struct HX_REV
{/*{{{*/
	float hu_data;
};/*}}}*/
struct HX_REV hx;
unsigned char HR_HQ[6];
unsigned char temp_ok=0;
unsigned short int temp_5[5];
float V_Bat;
unsigned char env_ok=0;
float env_f[6];
unsigned char checksum(void* data,int n);
void SendKey(unsigned char key);
void setTimer(int seconds, int mseconds); 
int main(int argc,char **argv)
{/*{{{*/
	unsigned int  nread=0,debug=0;
	unsigned char buffer[256];
	const char *usart_path="/dev/ttyUSB0";
	unsigned int uart_rate=115200;
	const char *tmp=(const char *)buffer;
	printf("hello,this is the uart moudle\r\n");
	int next_option;
	const char *short_opt = "d:r:t";
	do
	{/*{{{*/
		next_option=getopt(argc,argv,short_opt);
		switch(next_option)
		{
			case 'd':
				usart_path=optarg;
				break;
			case 'r':
				uart_rate=atoi(optarg);
				break;
			case 't':
				debug=1;;
			default:break;
		}
	}while(next_option!=-1);/*}}}*/
	Init(usart_path,uart_rate);
	float t=0,T=0.004;	
	while(debug)
	{/*{{{*/
		ecg.ecg1=(int)(1000*t)%1000;
		ecg.ecg2=(int)(1000*t)%2000;
		ecg.ecg3=(int)(1000*t)%4000;
		printf("Debug Mode! ECG:CH1[%06d],CH2[%06d],CH3[%06d],t[% f] \r",ecg.ecg1,ecg.ecg2,ecg.ecg3,t);
		SendECGmsq(sizeof(ecg),(void *)&ecg);
		SendKeymsq(8);
		t+=T;
		setTimer(0,4000);
	}	/*}}}*/
	SendKeymsq(0);
	SendStr("RESET");
	while(1)
	{/*{{{*/
begin:		do
		{
			nread = read_port(tmp,1);
		}while(!(buffer[0]==0xAA&&nread==1));
		nread = read_port(tmp,1);
		if(buffer[0]!=0xAA)goto begin;
		else
		{
			nread = read_port(tmp,1);
			Rbuff.size=buffer[0];
			nread = read_port(tmp,1);
			Rbuff.nsize=buffer[0];
			if(Rbuff.size+Rbuff.nsize!=0xff)
			{
				printf("get size data err!size1[%02X],size2[%02X]\r\n",Rbuff.size,Rbuff.nsize);
				goto begin;
			}
			nread=0;
			do
			{
				nread=nread+ read_port((const char *)(&buffer[nread]),(Rbuff.size-nread));
			}while(nread<Rbuff.size);
			memcpy(&Rbuff.addr,&buffer[0],Rbuff.size);

			unsigned char sum;
			sum=checksum((void *)&(Rbuff.buffer),Rbuff.size-2);
			if(sum==Rbuff.checksum||Rbuff.checksum==0x00)
			{
				SendStr("OK");
				if(Rbuff.addr==1)
				{
					memcpy(&ecg,Rbuff.buffer,sizeof(ecg));
					SendECGmsq(sizeof(ecg),(void *)&ecg);
				}else if(Rbuff.addr==0xfe)
				{
					printf("getkey[0x%02X]\n\r",Rbuff.buffer[0]);
					SendKeymsq(Rbuff.buffer[0]);
				}else if(Rbuff.addr==0xff)
				{
					int i=0;
					short int ecgs1[10],ecgs2[10],ecgs3[10];
					short int ax[10],ay[10],az[10];
					do
					{
						if(Rbuff.buffer[i]==0x01&&Rbuff.buffer[i+1]==20)
						{
							memcpy(ecgs1,&Rbuff.buffer[i+2],20);
							i+=22;
							continue;
						}
						else if(Rbuff.buffer[i]==0x02&&Rbuff.buffer[i+1]==20)
						{
							memcpy(ecgs2,&Rbuff.buffer[i+2],20);
							i+=22;
							continue;
						}
						else if(Rbuff.buffer[i]==0x03&&Rbuff.buffer[i+1]==20)
						{
							memcpy(ecgs3,&Rbuff.buffer[i+2],20);
							i+=22;
							continue;
						}
						else if(Rbuff.buffer[i]==0x04&&Rbuff.buffer[i+1]==8)
						{
							memcpy(ax,&Rbuff.buffer[i+2],8);
							i+=10;
							continue;
						}
						else if(Rbuff.buffer[i]==0x05&&Rbuff.buffer[i+1]==8)
						{
							memcpy(ay,&Rbuff.buffer[i+2],8);
							i+=10;
							continue;
						}
						else if(Rbuff.buffer[i]==0x06&&Rbuff.buffer[i+1]==8)
						{
							memcpy(az,&Rbuff.buffer[i+2],8);
							i+=10;
							continue;
						}
						else if(Rbuff.buffer[i]==0x07&&Rbuff.buffer[i+1]==4)
						{
							memcpy(&hx.hu_data,&Rbuff.buffer[i+2],4);
							i+=6;
							continue;
						}
						else if(Rbuff.buffer[i]==0x08&&Rbuff.buffer[i+1]==6)
						{
							memcpy(HR_HQ,&Rbuff.buffer[i+2],6);
							i+=8;
							continue;
						}
						else if(Rbuff.buffer[i]==0x09&&Rbuff.buffer[i+1]==10)
						{
							memcpy(temp_5,&Rbuff.buffer[i+2],10);
							i+=12;
							temp_ok=1;
							continue;
						}
						else if(Rbuff.buffer[i]==10&&Rbuff.buffer[i+1]==4)
						{
							memcpy(&V_Bat,&Rbuff.buffer[i+2],4);
							i+=6;
							continue;
						}
						else if(Rbuff.buffer[i]==11&&Rbuff.buffer[i+1]==20)
						{
							memcpy(env_f,&Rbuff.buffer[i+2],20);
							i+=22;
							env_ok=1;
							continue;
						}
						else i++;
					}while(i<Rbuff.size-2);
					for(i=0;i<10;i++)
					{/*{{{*/
						ecg.ecg1=ecgs1[i];
						ecg.ecg2=ecgs2[i];
						ecg.ecg3=ecgs3[i];
						SendECGmsq(sizeof(ecg),(void *)&ecg);
					}/*}}}*/
					for(i=0;i<4;i++)
					{/*{{{*/
						mpu.ax=ax[i];
						mpu.ay=ay[i];
						mpu.az=az[i];
						SendMPUmsq(sizeof(mpu),(void *)&mpu);
					}/*}}}*/
					SendHxmsq(sizeof(hx),(void *)&hx);
					static int count=0;
					if(count++%25==0)
					{
						SendRTHQmsq(sizeof(HR_HQ),(void *)HR_HQ);
					}
					if(temp_ok)
					{
						temp_ok=0;
						Sendtempmsq(sizeof(temp_5),(void *)temp_5);
					}
					if(env_ok)
					{
						env_ok=0;
						env_f[5]=V_Bat;
//						printf("get env data,[%f],[%f],[%f],[%f][%f],[%f]\r\n",env_f[0],env_f[1],env_f[2],env_f[3],env_f[4],env_f[5]);
						Sendenvmsq(sizeof(env_f),(void *)env_f);
					}
				}
			}
			else
			{
				printf("\r\n checksum err,getsum=[%02X],sum=[%02X],count=[%d]\r\n",Rbuff.checksum,sum,nread);
				printf("\r\n the byte is ");
				int x;
				for(x=0;x<nread;x++)
				{
					printf(" %02X",buffer[x]);
				}
				printf("\r\n");

			}
		}
	}/*}}}*/
}/*}}}*/
unsigned char checksum(void* data,int n)
{/*{{{*/
	int i;
	unsigned char sum=0;
	unsigned char* p=(unsigned char *)data;
	for(i=0;i<n;i++)
	{
		sum+=p[i];
	}
	return sum;
}/*}}}*/
void setTimer(int seconds, int mseconds) 
{  /*{{{*/
        struct timeval temp;  
        temp.tv_sec = seconds;  
        temp.tv_usec = mseconds;  
        select(0, NULL, NULL, NULL, &temp);  
        printf("timer\n");  
}/*}}}*/
