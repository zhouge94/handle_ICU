#include "linux/input.h"
#include "my_uart.h"
#include "my_msg.h"
//0 for EV_KEY for release, 1 for keypress and 2 for autorepeat.
void simulate_key(int fd,int code,int value)
{
	struct input_event event,ev;

	event.type = EV_KEY;
	event.code = code;//要模拟成什么按键
	event.value = value;//是按下还是释放按键或者重复
	gettimeofday(&event.time,0);

	if(write(fd,&event,sizeof(event)) < 0){
		printf("simulate key error~~~/n");
		return ;
	}
	write(fd,&ev,sizeof(ev));
}
void keypress_one(int code)
{
	static int firstin=1;
	static int fd_kbd; // /dev/input/event1
	if(firstin)
	{
		fd_kbd = open("/dev/input/event1",O_RDWR);
		if(fd_kbd<=0){
			printf("error open keyboard:%s/n",strerror(errno));
		}else{
			firstin=0;
		}
	}
	if(!firstin)
	{
		printf("keypress_one:key[%d]\r\n",code);
		simulate_key(fd_kbd,code,1);
		usleep(1000);
		simulate_key(fd_kbd,code,0);
	}
}
int main()
{
	printf("enter loop\r\n");
	while(1)
	{
		keypress_one(KEY_8);
		sleep(2);
	}
}
