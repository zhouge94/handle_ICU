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
	
	int speed_arr[] = {B115200, B38400, B19200, B9600, B4800, B2400, B1200, B300,
		    B38400, B19200, B9600, B4800, B2400, B1200, B300, };
	int name_arr[] = {115200, 38400,  19200,  9600,  4800,  2400,  1200,  300,
		    38400,  19200,  9600, 4800, 2400, 1200,  300, };
	void set_speed(int fd, int speed)
	{
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
	
	}
	/**
	*@brief   设置串口数据位，停止位和效验位
	*@param  fd     类型  int  打开的串口文件句柄*
	*@param  databits 类型  int 数据位   取值 为 7 或者8*
	*@param  stopbits 类型  int 停止位   取值为 1 或者2*
	*@param  parity  类型  int  效验类型 取值为N,E,O,,S
	*/
	int set_Parity(int fd,int databits,int stopbits,int parity)
	{
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
	    options.c_cc[VTIME] = 150; // 15 seconds
	    options.c_cc[VMIN] = 0;
	
	  tcflush(fd,TCIFLUSH); /* Update the options and do it NOW */
	  if (tcsetattr(fd,TCSANOW,&options) != 0)
	  	{
	  		perror("SetupSerial 3");
			return (FALSE);
		}
	  return (TRUE);
	 }
	/**
	*@breif 打开串口
	*/
	int OpenDev(char *Dev)
	{
		int	fd = open(Dev, O_RDWR );         //| O_NOCTTY | O_NDELAY
		if (-1 == fd)
			{ /*设置数据位数*/
				perror("Can't Open Serial Port");	
				return -1;
			}
		else
		return fd;
	
	}
	/**
	*@breif 	main()
	*/
	int main(int argc, char **argv)
	{
		int fd;
		int nread;
		char buffer[512];
	//char *dev ="/dev/ttyAMA5";
		int n=0,i=0;
		char* dev   = NULL;
		struct termios options;
	
		int next_option,havearg = 0,flow = 0;
		const char *const short_opt = "fd:";
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
	        set_speed(fd,115200); //设置波特率
		else	
		{
			printf("Can't Open Serial Port!\n");
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
		
		printf("\nWelcome to TTYtest\n\n");
	
	        memset(buffer,0,sizeof(buffer));
	
	        char test[100]="forlinx am335x uart test......";
	        
	        printf("Send test data------%s\n",test);
	        write(fd, test, strlen(test));
	
	  	while(1)
	  	{
			nread = read(fd,&buffer[n],1);	
	                printf("read char is %c \n",buffer[n]);
	                
		   	if (strlen(buffer)==strlen(test))
		   	{
	
		   		printf("Read Test Data finished,Read Test Data is-------%s\n",buffer);
	                        memset(buffer,0,sizeof(buffer));
	
	                        printf("Send test data again------%s\n",test);
	                        write(fd, test, strlen(test));
	                        n=0;
	                        sleep(1);
	                        continue;
				
			}
	
	               n++;
	         }
	}
	
	
