#include     <stdio.h>      /*标准输入输出定义*/
#include     <stdlib.h>     /*标准函数库定义*/
#include     <unistd.h>     /*Unix标准函数定义*/
#include     <sys/types.h>  /**/
#include     <sys/stat.h>   /**/
#include     <fcntl.h>      /*文件控制定义*/
#include     <termios.h>    /*PPSIX终端控制定义*/
#include     <errno.h>      /*错误号定义*/
#include	 <sys/time.h>
#include     <string.h>
#include     <getopt.h>


/***@brief  设置串口通信速率
 *@param  fd     类型 int  打开串口的文件句柄
 *@param  speed  类型 int  串口速度
 *@return  void*/
#define TRUE 1
#define FALSE -1

int speed_arr[] = {B115200, B57600,B38400, B19200, B9600, B4800, B2400, B1200, B300,
	B38400, B19200, B9600, B4800, B2400, B1200, B300, };
int name_arr[] = {115200, 57600,38400,  19200,  9600,  4800,  2400,  1200,  300,
	38400,  19200,  9600, 4800, 2400, 1200,  300, };
void set_speed(int fd, int speed)
{/*{{{*/
	int   i;
	int   status;
	struct termios   Opt;
	tcgetattr(fd, &Opt);

	for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++)
	{
		if  (speed == name_arr[i])
		{
			tcflush(fd, TCIOFLUSH);
			/*  设置串口的波特率 */
			cfsetispeed(&Opt, speed_arr[i]);
			cfsetospeed(&Opt, speed_arr[i]);
			status = tcsetattr(fd, TCSANOW, &Opt);
			if  (status != 0)
				perror("tcsetattr fd1");
			return;
		}
		tcflush(fd,TCIOFLUSH);
	}

}/*}}}*/
/**
 *@brief   设置串口数据位，停止位和效验位
 *@param  fd     类型  int  打开的串口文件句柄*
 *@param  databits 类型  int 数据位   取值 为 7 或者8*
 *@param  stopbits 类型  int 停止位   取值为 1 或者2*
 *@param  parity  类型  int  效验类型 取值为N,E,O,,S
 */
int set_Parity(int fd,int databits,int stopbits,int parity)
{/*{{{*/
	struct termios options;
	if  ( tcgetattr( fd,&options)  !=  0)
	{
		perror("SetupSerial 1");
		return(FALSE);
	}
	options.c_cflag &= ~CSIZE;
	switch (databits) /*设置数据位数*/
	{
		case 7:
			options.c_cflag |= CS7;
			break;
		case 8:
			options.c_cflag |= CS8;
			break;
		default:
			fprintf(stderr,"Unsupported data size\n");
			return (FALSE);
	}
	switch (parity)
	{
		case 'n':
		case 'N':
			options.c_cflag &= ~PARENB;   /* Clear parity enable */
			options.c_iflag &= ~INPCK;     /* Enable parity checking */
			options.c_iflag &= ~(ICRNL|IGNCR);
			options.c_lflag &= ~(ICANON );
			break;
		case 'o':
		case 'O':
			options.c_cflag |= (PARODD | PARENB);  /* 设置为奇效验*/
			options.c_iflag |= INPCK;             /* Disnable parity checking */
			break;
		case 'e':
		case 'E':
			options.c_cflag |= PARENB;     /* Enable parity */
			options.c_cflag &= ~PARODD;   /* 转换为偶效验*/
			options.c_iflag |= INPCK;       /* Disnable parity checking */
			break;
		case 'S':
		case 's':  /*as no parity*/
			options.c_cflag &= ~PARENB;
			options.c_cflag &= ~CSTOPB;
			break;
		default:
			fprintf(stderr,"Unsupported parity\n");
			return (FALSE);
	}
	/* 设置停止位*/
	switch (stopbits)
	{
		case 1:
			options.c_cflag &= ~CSTOPB;
			break;
		case 2:
			options.c_cflag |= CSTOPB;
			break;
		default:
			fprintf(stderr,"Unsupported stop bits\n");
			return (FALSE);
	}
	/* Set input parity option */
	if (parity != 'n')
		options.c_iflag |= INPCK;
	options.c_lflag &= ~(ICANON|ISIG);
	options.c_iflag &= ~(ICRNL|IGNCR|IXON);
	//options.c_iflag=0;
	options.c_cc[VTIME] = 10; // 1 seconds
	options.c_cc[VMIN] = 1;

	tcflush(fd,TCIFLUSH); /* Update the options and do it NOW */
	if (tcsetattr(fd,TCSANOW,&options) != 0)
	{
		perror("SetupSerial 3");
		return (FALSE);
	}
	return (TRUE);
}/*}}}*/
/**
 *@breif 打开串口
 */
int OpenDev(char *Dev)
{
	int	fd = open(Dev, O_RDWR| O_NOCTTY );         //| O_NOCTTY | O_NDELAY
	if (-1 == fd)
	{ /*设置数据位数*/
		perror("Can't Open Serial Port");
		return -1;
	}
	else
		return fd;

}
unsigned char checksum(void* data,int n)
{
	int i;
	unsigned char sum=0;
	unsigned char* p=(unsigned char *)data;
	for(i=0;i<n;i++)
	{
		sum+=p[i];
	}
	return sum;
}
void SendECG(int ecg1,int ecg2,int ecg3)
{
	static int fd[3];
	char w_buf[50]={0};
	int w_num;
	int err=0,i;
	const char *fifo_name[3]={"/tmp/ecg1_fifo","/tmp/ecg2_fifo","/tmp/ecg3_fifo"};
	static int first_in=0;
	if(first_in==0)
	{
		first_in=1;
		for(i=0;i<1;i++)
		{
			if(access(fifo_name[i],F_OK)==-1)
			{
				if(mkfifo(fifo_name[i],0777)<0&&errno!=EEXIST)
				{
					printf("err  while creat fifo");
					err=-1;
				}
			}
			if(err==0)
			{
				fd[i]=open(fifo_name[i],O_WRONLY|O_NONBLOCK);
				if(fd[i]==-1)err=-1;
				else printf("creat and open fifo[%s],fd[%d] success\r\n",fifo_name[i],fd[i]);
			}else first_in=0;
		}
	}
	if(first_in>0)
	{
		sprintf(w_buf,"[%+07d],[%+07d],[%+07d]\n\r",ecg1,ecg2,ecg3);
		w_num=write(fd[0],w_buf,strlen(w_buf)+1);
		//sprintf(w_buf,"%d",ecg2);
		//w_num=write(fd[1],w_buf,strlen(w_buf)+1);
		//sprintf(w_buf,"%d",ecg3);
		//w_num=write(fd[2],w_buf,strlen(w_buf)+1);
	}

}
/**
 *@breif 	main()
 */
int main(int argc, char **argv)
{
	int fd;
	int nread;
	unsigned char buffer[512];
	//char *dev ="/dev/ttyAMA5";
	int n=0,i=0;
	char* dev   = NULL;
	int speed=115200;
	struct termios options;

	int next_option,havearg = 0,flow = 0;
	const char *const short_opt = "fd:r:";
	const struct option long_opt[] = {
		{"devices",1,NULL,'d'},
		{"hard_flow",0,NULL,'f'},
		{NULL,0,NULL,0},
	};
	do{
		next_option = getopt_long(argc,argv,short_opt,long_opt,NULL);
		switch (next_option) {
			case 'd':
				dev = optarg;
				havearg = 1;
				break;
			case 'f':
				flow = 1;
				break;
			case '?':
				printf("please input correct arguments\n");
				break;
			case 'r':
				speed=atoi(optarg);
				printf("set bandrate[%d]\n",speed);
				break;
			case -1:
				if(havearg)
					break;
			default:
				printf("please input correct arguments\n");
				exit(1);

		}
	}while(next_option != -1);

	if(dev==NULL)
	{
		printf("Please input seria device name ,for exmaple /dev/ttyO1.\nNote:This is loop test application. Make sure that your serial is loop\n");

		exit(1);
	}
	/*  打开串口 */
	fd = OpenDev(dev);
	if (fd>0)
	{
		set_speed(fd,speed); //设置波特率
		printf("Open Serial Port[%s] sucess,speed[%d],fd[%d] !\n",dev,speed,fd);
	}
	else
	{
		exit(0);
	}

	/*nomatter what close hardflow first*/
	if(tcgetattr(fd,&options) != 0)
	{
		perror("flow control");
		exit(1);
	}
	else
	{
		options.c_cflag &= ~CRTSCTS;
	}
	if(tcsetattr(fd,TCSAFLUSH,&options) != 0)
	{
		perror("flow control setattr ");
		exit(1);
	}

	if(flow == 1)
	{
		if(tcgetattr(fd,&options) != 0)
		{
			perror("flow control");
			exit(1);
		}
		else
		{
			options.c_cflag |= CRTSCTS;
		}
		if(tcsetattr(fd,TCSAFLUSH,&options) != 0)
		{
			perror("flow control setattr ");
			exit(1);
		}
	}
	if (set_Parity(fd,8,1,'N')== FALSE)  //设置传递参数
	{
		printf("Set Parity Error\n");
		exit(1);
	}
	/*
	   printf("\nWelcome to TTYtest\n\n");
	   memset(buffer,0,sizeof(buffer));
	   char test[100]="forlinx am335x uart test......";
	   printf("Send test data------%s\n",test);
	   write(fd, test, strlen(test));*/
	struct REV_BUF
	{
		unsigned char s1;
		unsigned char s2;
		unsigned char size;
		unsigned char nsize;
		unsigned char addr;
		unsigned char checksum;
		unsigned char buffer[256];
	};
	struct REV_BUF  Rbuff={0};
	struct ECG_REV
	{
		short int ecg1;
		short int ecg2;
		short int ecg3;
	};
	struct ECG_REV ecg;
	unsigned char tmp;
	memset(buffer,0,sizeof(buffer));
	int allnum=0,oknum=0;
	int loop_count=0;
	while(1)
	{
		printf("Loop:[%06d],oknum=[%d]\r\n",loop_count++,oknum);
begin:		nread = read(fd,&tmp,1);
		if(tmp==0xAA)
		{
			nread = read(fd,&tmp,1);
			if(tmp==0xAA)
			{
				nread = read(fd,&tmp,1);
				Rbuff.size=tmp;
				nread = read(fd,&tmp,1);
				if(Rbuff.size+tmp!=0xff)goto begin;
				nread=0;
				do
				{
					nread=nread+ read(fd,&buffer[nread],(Rbuff.size-nread));
				}while(nread<Rbuff.size);
				memcpy(&Rbuff.addr,&buffer[0],Rbuff.size);
				if(Rbuff.addr==1)
				{
					memcpy(&ecg,Rbuff.buffer,sizeof(ecg));
					unsigned char sum;
					sum=checksum((void *)&(Rbuff.buffer),sizeof(ecg));
					if(sum==Rbuff.checksum||Rbuff.checksum==0x00)
					{
						printf("ECG:CH1[%06d],CH2[%06d],CH3[%06d],count[% 3d],checksum[0x%02X],sum=[0x%02X]\r\n",ecg.ecg1,ecg.ecg2,ecg.ecg3,nread,Rbuff.checksum,sum);
						SendECG(ecg.ecg1,ecg.ecg2,ecg.ecg3);
						oknum++;
					}
					else
					{
						printf("checksum err,getsum=[%02X],sum=[%02X],count=[%d]\r\n",Rbuff.checksum,sum,nread);
						printf("the byte is ");
						int x;
						for(x=0;x<nread;x++)
						{
							printf(" %02X",buffer[x]);
						}
						printf("\r\n");

					}
				}
			else
				{
					printf("ADDR[%d]\r\n",Rbuff.addr);
				}
			}
		}
	//	else printf("ERR BYTE [%02x]\r\n",tmp);
	}
}


