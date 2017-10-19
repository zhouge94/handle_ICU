#include "my_msg.h"
#include "math.h"
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
	float sum_acc,ax,ay,az,k=0.0011992168,diff_sum;
	short int *raw;
	static short int raw0[3]={0},diff_raw[3]={0};
	static int count=0;
	static float buffer_f[100];
	raw=(short int *)data;
	ax=raw[0]*k;
	ay=raw[1]*k;
	az=raw[2]*k;
	sum_acc=sqrt(ax*ax+ay*ay+az*az);
	
	diff_raw[0]=raw[0]-raw0[0];
	diff_raw[1]=raw[1]-raw0[1];
	diff_raw[2]=raw[2]-raw0[2];
	
	diff_sum=sqrt((diff_raw[0]*diff_raw[0]+diff_raw[1]*diff_raw[1]+diff_raw[2]*diff_raw[2])*1.0)*k;
	buffer_f[count++]=diff_sum;
	if(count>=20)
	{
		float temp_sum=0,temp_ave;
		for(count=0;count<20;count++)temp_sum+=buffer_f[count];
		temp_ave=temp_sum/20;
		//printf("get mpu data; ax[%f],ay[%f],ax[%f],sum[%f],d_sum[%f]\r\n",ax,ay,az,sum_acc,diff_sum);
		count=0;
		char cmd[50];
		sprintf(cmd,"%3.1f",temp_ave*10);
		SendString(3,(char *)cmd);
	}
	raw0[0]=raw[0];
	raw0[1]=raw[1];
	raw0[2]=raw[2];
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
		char cmd[100];
		sprintf(cmd,"./key.sh %d",key_value);
		system(cmd);
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
			printf("\r\n err while send rt_hq_msg \r\n");
		}
		unsigned char HR,hx_R;
		unsigned char *p=(unsigned char*)data;
		HR=p[0];
		hx_R=p[1];
		//else HR=0;
		char cmd[50];
		//sprintf(cmd,"echo -n '%3d' > /tmp/ch2_fifo &",HR);
		sprintf(cmd,"%3d",HR);
		SendString(2,(char *)cmd);
		
		sprintf(cmd,"%3d",hx_R);
		SendString(1,(char *)cmd);
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
		//sprintf(cmd,"echo -n '%3.1f-%3.1f'> /tmp/ch4_fifo &",min*1.0/100.0,max*1.0/100.0);
		sprintf(cmd,"%3.1f-%3.1f",min*1.0/100.0,max*1.0/100.0);
		//system(cmd);
		SendString(4,(char *)cmd);
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
		//sprintf(cmd,"echo -n '%4.1f'> /tmp/ch5_fifo &",p[0]);
		sprintf(cmd,"%4.1f",p[0]);
		//system(cmd);
		SendString(5,(char *)cmd);
		//sprintf(cmd,"echo -n '%4.1f'> /tmp/ch6_fifo &",p[1]);
		sprintf(cmd,"%4.1f",p[1]);
		//system(cmd);
		SendString(6,(char *)cmd);
		//sprintf(cmd,"echo -n '%3.1f-%3.1f'> /tmp/ch7_fifo &",p[2],p[5]);
		sprintf(cmd,"%3.1f-%3.1f",p[2],p[5]);
		//system(cmd);
		SendString(7,(char *)cmd);
		//sprintf(cmd,"echo -n '%6.4f'> /tmp/ch8_fifo &",p[3]*1.0);
		sprintf(cmd,"%6.4f",p[3]*1.0);
		//system(cmd);
		SendString(8,(char *)cmd);
		//sprintf(cmd,"echo -n '%6.3f'> /tmp/ch9_fifo &",p[4]*1.0);
		sprintf(cmd,"%6.3f",p[4]*1.0);
		//system(cmd);
		SendString(9,(char *)cmd);
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
void SendString(int chx,char *data)
{/*{{{*/
	static int first_in=1,msqid,ret;
	struct msg
	{
		long msg_types;
		char msg_buf[255];
	}pmsg;	
	if(first_in)
	{
		first_in=0;
		msqid=msgget(MSGKEY_String,IPC_EXCL);
		if(msqid<0)
		{
			msqid=msgget(MSGKEY_String,IPC_CREAT|0666);
			if (msqid<0)
			{
				printf("err while create msq\r\n");
				first_in=1;
			}	
		}
		printf("sucess get msqid[%d]\r\n",msqid);

	}else
	{
		memcpy((void *)pmsg.msg_buf,data,strlen(data)+1);
		pmsg.msg_types=chx;
		printf("Will Send String[%s] At Chx[%d]\r\n",pmsg.msg_buf,(int)pmsg.msg_types);
		ret=msgsnd(msqid,&pmsg,255,IPC_NOWAIT);
		if(ret<0)
		{
			int ret_value;
			do
			{        
				ret_value=msgrcv(msqid,&pmsg,255,0,IPC_NOWAIT);
			}while(ret_value>0);
			printf("\r\n err while send string_msg \r\n");
		}
	}
}/*}}}*/
