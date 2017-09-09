/*************************************************************************
    > File Name: adc.c
    > Author:张明志
    > Mail:862207403@qq.com 
    > Created Time: Tue 07 Mar 2017 04:59:27 AM EST
 ************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>


int main(int argc, const char *argv[])
{
    char buf[24];
    char *dir[]={"/sys/bus/iio/devices/iio:device0/in_voltage0_raw","/sys/bus/iio/devices/iio:device0/in_voltage1_raw",\
	"/sys/bus/iio/devices/iio:device0/in_voltage2_raw","/sys/bus/iio/devices/iio:device0/in_voltage3_raw","/sys/bus/iio/devices/iio:device0/in_voltage4_raw",\
	"/sys/bus/iio/devices/iio:device0/in_voltage5_raw","/sys/bus/iio/devices/iio:device0/in_voltage6_raw","/sys/bus/iio/devices/iio:device0/in_voltage7_raw"};	
	int fd;
	int i;
	for(i=0;i<8;i++)
	{
	   memset(buf,0,sizeof(buf));
	  fd=open(dir[i],O_RDONLY);
	  read(fd,buf,sizeof(buf));
	  printf("%s",buf);
	}
	return 0;
}
