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
