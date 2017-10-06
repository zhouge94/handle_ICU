#include "my_msg.h"
void SendECGmsq(int size,void *data)
{/*{{{*/
	static int first_in=1,msqid,ret;
	struct msg
	{
		long msg_types;
		char msg_buf[512];
	}pmsg;	
	if(first_in)
	{
		first_in=0;
		msqid=msgget(MSGKEY_ECG,IPC_EXCL);
		if(msqid<0)
		{
			msqid=msgget(MSGKEY_ECG,IPC_CREAT|0666);
			if (msqid<0)
			{
				printf("err while create msq\r\n");
				first_in=1;
			}	
		}
		printf("sucess get msqid[%d]\r\n",msqid);
	}else
	{
		memcpy((void *)pmsg.msg_buf,data,size);
		pmsg.msg_types=1;
		ret=msgsnd(msqid,&pmsg,size,IPC_NOWAIT);
		if(ret<0)
		{
			int ret_value;
			do
			{        
				ret_value=msgrcv(msqid,&pmsg,size,1,IPC_NOWAIT);

			}while(ret_value>0);
			printf("\r\n err while send ecg_msg\r\n");
		}
	}
}/*}}}*/
void SendMPUmsq(int size,void *data)
{/*{{{*/
	static int first_in=1,msqid,ret;
	struct msg
	{
		long msg_types;
		char msg_buf[512];
	}pmsg;	
	if(first_in)
	{
		first_in=0;
		msqid=msgget(MSGKEY_MPU,IPC_EXCL);
		if(msqid<0)
		{
			msqid=msgget(MSGKEY_MPU,IPC_CREAT|0666);
			if (msqid<0)
			{
				printf("err while create msq\r\n");
				first_in=1;
			}	
		}
		printf("sucess get msqid[%d]\r\n",msqid);

	}else
	{
		memcpy((void *)pmsg.msg_buf,data,size);
		pmsg.msg_types=1;
		ret=msgsnd(msqid,&pmsg,size,IPC_NOWAIT);
		if(ret<0)
		{
			int ret_value;
			do
			{        
				ret_value=msgrcv(msqid,&pmsg,size,1,IPC_NOWAIT);
			}while(ret_value>0);
			printf("\r\n err while send mpu_msg \r\n");
		}
	}
}/*}}}*/
void SendHxmsq(int size,void *data)
{/*{{{*/
	static int first_in=1,msqid,ret;
	struct msg
	{
		long msg_types;
		char msg_buf[512];
	}pmsg;	
	if(first_in)
	{
		first_in=0;
		msqid=msgget(MSGKEY_Hx,IPC_EXCL);
		if(msqid<0)
		{
			msqid=msgget(MSGKEY_Hx,IPC_CREAT|0666);
			if (msqid<0)
			{
				printf("err while create msq\r\n");
				first_in=1;
			}	
		}
		printf("sucess get msqid[%d]\r\n",msqid);

	}else
	{
		memcpy((void *)pmsg.msg_buf,data,size);
		pmsg.msg_types=1;
		ret=msgsnd(msqid,&pmsg,size,IPC_NOWAIT);
		if(ret<0)
		{
			int ret_value;
			do
			{        
				ret_value=msgrcv(msqid,&pmsg,size,1,IPC_NOWAIT);
			}while(ret_value>0);
			printf("\r\n err while send hx_msg \r\n");
		}
	}
}/*}}}*/
void SendKeymsq(unsigned char key_value)
{/*{{{*/
	static int first_in=1,msqid,ret;
	struct msg
	{
		long msg_types;
		char msg_buf[1];
	}pmsg;	
	if(first_in)
	{
		first_in=0;
		msqid=msgget(MSGKEY_Key,IPC_EXCL);
		if(msqid<0)
		{
			msqid=msgget(MSGKEY_Key,IPC_CREAT|0666);
			if (msqid<0)
			{
				printf("err while create msq\r\n");
				first_in=1;
			}	
		}
		printf("sucess get msqid[%d]\r\n",msqid);
	}
	if(!first_in)
	{
		int ret_value;
		do
		{        
			ret_value=msgrcv(msqid,&pmsg,1,1,IPC_NOWAIT);
		}while(ret_value>0);
		pmsg.msg_types=1;
		pmsg.msg_buf[0]=key_value;
		ret=msgsnd(msqid,&pmsg,1,IPC_NOWAIT);
		if(key_value==0x21)
		{
			system("echo  mem>/sys/power/state");
		}
	}

}/*}}}*/
void SendRTHQmsq(int size,void *data)
{/*{{{*/
	static int first_in=1,msqid,ret;
	struct msg
	{
		long msg_types;
		char msg_buf[512];
	}pmsg;	
	if(first_in)
	{
		first_in=0;
		msqid=msgget(MSGKEY_RTHQ,IPC_EXCL);
		if(msqid<0)
		{
			msqid=msgget(MSGKEY_RTHQ,IPC_CREAT|0666);
			if (msqid<0)
			{
				printf("err while create msq\r\n");
				first_in=1;
			}	
		}
		printf("sucess get msqid[%d]\r\n",msqid);

	}else
	{
		memcpy((void *)pmsg.msg_buf,data,size);
		pmsg.msg_types=1;
		ret=msgsnd(msqid,&pmsg,size,IPC_NOWAIT);
		if(ret<0)
		{
			int ret_value;
			do
			{        
				ret_value=msgrcv(msqid,&pmsg,size,1,IPC_NOWAIT);
			}while(ret_value>0);
			printf("\r\n err while send hx_msg \r\n");
		}
		unsigned char HR;
		unsigned char *p=(unsigned char*)data;
		if(p[3]>0)HR=p[0];
		else if(p[4]>0)HR=p[1];
		else if(p[5]>0)HR=p[2];
		//else HR=0;
		char cmd[50];
		sprintf(cmd,"echo '%3d' >> /tmp/ch2_fifo &",HR);
		//printf(cmd);
		system(cmd);
	}
}/*}}}*/
void Sendtempmsq(int size,void *data)
{/*{{{*/
	static int first_in=1,msqid,ret;
	int i;
	unsigned short int max,min,*p=(unsigned short int *)data;
	struct msg
	{
		long msg_types;
		char msg_buf[512];
	}pmsg;	
	if(first_in)
	{
		first_in=0;
		msqid=msgget(MSGKEY_TEMP,IPC_EXCL);
		if(msqid<0)
		{
			msqid=msgget(MSGKEY_TEMP,IPC_CREAT|0666);
			if (msqid<0)
			{
				printf("err while create msq\r\n");
				first_in=1;
			}	
		}
		printf("sucess get msqid[%d]\r\n",msqid);

	}else
	{
		max=p[0];
		min=p[0];
		for(i=1;i<5;i++)
		{
			if(p[i]>max)max=p[i];
			if(p[i]<min)min=p[i];
		}
		char cmd[100];
		sprintf(cmd,"echo '%2.1f-%2.1f'>> /tmp/ch4_fifo &",min*1.0/100.0,max*1.0/100.0);
		system(cmd);
		memcpy((void *)pmsg.msg_buf,data,size);
		pmsg.msg_types=1;
		ret=msgsnd(msqid,&pmsg,size,IPC_NOWAIT);
		if(ret<0)
		{
			int ret_value;
			do
			{        
				ret_value=msgrcv(msqid,&pmsg,size,1,IPC_NOWAIT);
			}while(ret_value>0);
			printf("\r\n err while send temp_msg \r\n");
		}
	}
}/*}}}*/
void Sendenvmsq(int size,void *data)
{/*{{{*/
	static int first_in=1,msqid,ret;
	float *p=(float *)data;
	struct msg
	{
		long msg_types;
		char msg_buf[512];
	}pmsg;	
	if(first_in)
	{
		first_in=0;
		msqid=msgget(MSGKEY_ENV,IPC_EXCL);
		if(msqid<0)
		{
			msqid=msgget(MSGKEY_ENV,IPC_CREAT|0666);
			if (msqid<0)
			{
				printf("err while create msq\r\n");
				first_in=1;
			}	
		}
		printf("sucess get msqid[%d]\r\n",msqid);

	}else
	{
		char cmd[100];
		sprintf(cmd,"echo '%2.1f'>> /tmp/ch5_fifo &",p[0]);
		system(cmd);
		sprintf(cmd,"echo '%2.1f'>> /tmp/ch6_fifo &",p[1]);
		system(cmd);
		sprintf(cmd,"echo '%2.1f-%2.1f'>> /tmp/ch7_fifo &",p[2],p[5]);
		system(cmd);
		sprintf(cmd,"echo '%2.5f'>> /tmp/ch8_fifo &",p[3]);
		system(cmd);
		sprintf(cmd,"echo '%5.1f'>> /tmp/ch9_fifo &",p[4]);
		system(cmd);

		memcpy((void *)pmsg.msg_buf,data,size);
		pmsg.msg_types=1;
		ret=msgsnd(msqid,&pmsg,size,IPC_NOWAIT);
		if(ret<0)
		{
			int ret_value;
			do
			{        
				ret_value=msgrcv(msqid,&pmsg,size,1,IPC_NOWAIT);
			}while(ret_value>0);
			printf("\r\n err while send temp_msg \r\n");
		}
	}
}/*}}}*/
