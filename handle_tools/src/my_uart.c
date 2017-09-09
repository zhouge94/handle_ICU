#include "my_uart.h"

int Is_connect=0;
int Uart_fd;
void Init(const char *path,int rate)
{/*{{{*/
	int iResult = -1;
	int iBaudRate,iDataSize,iStopBit;
	int err=0;
	char cParity;
	Uart_fd=-1;
	printf("Opening the uart ,path[%s],bandrate[%d]\r\n",path,rate);
	Uart_fd = open_port(path);
	if( Uart_fd<0 )
	{
		printf("open_port error !");
		err=1;
	}

	iBaudRate = rate;
	iDataSize = 8;
	cParity = 'N';
	iStopBit = 1;
	iResult = set_port(Uart_fd,iBaudRate,iDataSize,cParity,iStopBit);
	if( iResult<0 )
	{
		printf("set_port error !");
		err=2;
	}
	if(err==0)
	{
		Is_connect=1;
	}else
	{
		Is_connect=0;
	}
}/*}}}*/
int open_port(const char *Portname)
{/*{{{*/
	int fd = -1;
	fd = open(Portname,O_RDWR|O_NOCTTY);
	if( fd<0 )
	{
		printf("Can't Open Serial Port !");
		return (-1);
	}
	else
	{
		printf("Open %s ......\n",Portname);
	}

	if( fcntl(fd,F_SETFL,0)<0 )/*恢复串口的状态为阻塞状态，用于等待串口数据的读入*/
	{
		printf("fcntl failed !\n");
		return (-1);
	}
	else
	{
		printf("fcntl = %d !\n",fcntl(fd,F_SETFL,0));
	}

	printf("open_port file ID= %d !\n",fd);

	return fd;

}/*}}}*/
int set_port(int fd,int iBaudRate,int iDataSize,char cParity,int iStopBit)
{/*{{{*/
	int iResult = 0;
	struct termios oldtio,newtio;


	iResult = tcgetattr(fd,&oldtio);/*保存原先串口配置*/
	if( iResult )
	{
		printf("Can't get old terminal description !");
		return (-1);
	}


	bzero(&newtio,sizeof(newtio));
	newtio.c_cflag |= CLOCAL | CREAD;/*设置本地连接和接收使用*/

	/*设置输入输出波特率*/
	switch( iBaudRate )
	{
		case 2400:
			cfsetispeed(&newtio,B2400);
			cfsetospeed(&newtio,B2400);
			break;
		case 4800:
			cfsetispeed(&newtio,B4800);
			cfsetospeed(&newtio,B4800);
			break;
		case 9600:
			cfsetispeed(&newtio,B9600);
			cfsetospeed(&newtio,B9600);
			break;
		case 19200:
			cfsetispeed(&newtio,B19200);
			cfsetospeed(&newtio,B19200);
			break;
		case 38400:
			cfsetispeed(&newtio,B38400);
			cfsetospeed(&newtio,B38400);
			break;
		case 57600:
			cfsetispeed(&newtio,B57600);
			cfsetospeed(&newtio,B57600);
			break;
		case 115200:
			cfsetispeed(&newtio,B115200);
			cfsetospeed(&newtio,B115200);
			break;
		case 460800:
			cfsetispeed(&newtio,B460800);
			cfsetospeed(&newtio,B460800);
			break;
		default     :
			/*perror("Don't exist iBaudRate !");*/
			printf("Don't exist iBaudRate %d !\n",iBaudRate);
			return (-1);
	}

	/*设置数据位*/
	newtio.c_cflag &= (~CSIZE);
	switch( iDataSize )
	{
		case    7:
			newtio.c_cflag |= CS7;
			break;
		case    8:
			newtio.c_cflag |= CS8;
			break;
		default:
			/*perror("Don't exist iDataSize !");*/
			printf("Don't exist iDataSize %d !\n",iDataSize);
			return (-1);
	}

	/*设置校验位*/
	switch( cParity )
	{
		case    'N':                    /*无校验*/
			newtio.c_cflag &= (~PARENB);
			break;
		case    'O':                    /*奇校验*/
			newtio.c_cflag |= PARENB;
			newtio.c_cflag |= PARODD;
			newtio.c_iflag |= (INPCK | ISTRIP);
			break;
		case    'E':                    /*偶校验*/
			newtio.c_cflag |= PARENB;
			newtio.c_cflag &= (~PARODD);
			newtio.c_iflag |= (INPCK | ISTRIP);
			break;
		default:
			/*perror("Don't exist cParity  !");*/
			printf("Don't exist cParity %c !\n",cParity);
			return (-1);
	}

	/*设置停止位*/
	switch( iStopBit )
	{
		case    1:
			newtio.c_cflag &= (~CSTOPB);
			break;
		case    2:
			newtio.c_cflag |= CSTOPB;
			break;
		default:
			/*perror("Don't exist iStopBit !");*/
			printf("Don't exist iStopBit %d !\n",iStopBit);
			return (-1);
	}

	newtio.c_cc[VTIME] = 0; /*设置等待时间*/
	newtio.c_cc[VMIN] = 0;  /*设置最小字符*/
	tcflush(fd,TCIFLUSH);       /*刷新输入队列(TCIOFLUSH为刷新输入输出队列)*/
	iResult = tcsetattr(fd,TCSANOW,&newtio);    /*激活新的设置使之生效,参数TCSANOW表示更改立即发生*/

	if( iResult )
	{
		perror("Set new terminal description error !");
		return (-1);
	}

	printf("set_port success !\n");

	return 0;
}/*}}}*/
void SendStr(const char *str)
{/*{{{*/
	if(Is_connect)write_port(str,strlen(str));
	else printf("you haven't conect!");
}/*}}}*/
void Sendbyte(unsigned char byte)
{/*{{{*/
	char data;
	data=byte;
	if(Is_connect)write_port((const char *)&data,1);
	else printf("you haven't conect!");
}/*}}}*/
int read_port(const char *buf, int iByte)
{/*{{{*/
	int iLen = 0,t=0;
	if( !iByte )
	{
		printf("Read byte number error !\n");
		return iLen;
	}
	do
	{
		iLen += read(Uart_fd,(void *)(buf+iLen),(iByte-iLen));
		if(iLen==0)usleep(1);
	}while(iLen<iByte&&(t++<5000));
	// if(t>=5000)printf("read port timeout [%d]us \r\n",t);
	return iLen;
}/*}}}*/
int write_port(const char *buf,int iByte)
{/*{{{*/
	int iLen = 0;
	if( !iByte )
	{
		printf("Write byte number error !\n");
		return iLen;
	}
	iLen = write(Uart_fd,(void *)buf,iByte);
	return iLen;
}/*}}}*/

