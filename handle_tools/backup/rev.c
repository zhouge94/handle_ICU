/*receive.c */  
#include "stdio.h"
#include <sys/types.h>   
#include <sys/ipc.h>   
#include <sys/msg.h>   
#include <errno.h>   
#include "stdlib.h"
#include "string.h"
#define MSGKEY 0x1101   
struct ECG_S
{
	short int ecg1;
	short int ecg2;
	short int ecg3;
}ecgs;
struct msgstru  
{  
	long msg_type;
	char msg_buf[512];
}rev_stu; 
/*子进程，监听消息队列*/  
void childproc()
{  
	int msgid,ret_value;  
	char str[512];  
	int count=0;
	msgid = msgget(MSGKEY,IPC_EXCL );/*检查消息队列是否存在 */  
	if(msgid < 0)
	{  
		printf("msq not existed! errno=%d [%s]\n",errno,strerror(errno));  
	}
	while(1)
	{  
		/*接收消息队列*/  
		usleep(1);
		ret_value = msgrcv(msgid,&rev_stu,sizeof(ecgs),1,0);
		if(ret_value>0)
		{
			memcpy(&ecgs,&rev_stu.msg_buf,sizeof(ecgs));
			printf("pid[%d],count[%d],msg_type[%d],get[%d,%d,%d]\r\n",getpid(),count++,rev_stu.msg_type,ecgs.ecg1,ecgs.ecg2,ecgs.ecg3);  

		}
	}  
	return;  
}  

int main()  
{  
	int i,cpid;  
	childproc();  
}
