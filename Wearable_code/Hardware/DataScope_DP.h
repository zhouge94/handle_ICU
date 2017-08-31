#ifndef _DATASCOPE_
#define _DATASCOPE_

#include "stm32f4xx_hal.h"
typedef struct 
{
	__IO short int ecg1;
	__IO short int ecg2;
	__IO short int ecg3;
}ECG_S;
typedef struct 
{
	__IO unsigned char key;
}KEY_S;

typedef struct 
{
	__IO short int Ax;
	__IO short int Ay;
	__IO short int Az;
  __IO short int Gx;
	__IO short int Gy;
	__IO short int Gz;

}MPU_S;

typedef struct 
{
	__IO unsigned char buffer[256];
}STRING_S;

typedef struct 
{
	unsigned char size ;
	unsigned char buffer[256];
}PACK_S;
extern PACK_S Data_Pack;
typedef struct 
{
	__IO unsigned char s1;
	__IO unsigned char s2;
	__IO unsigned char size;
	__IO unsigned char nsize;
	__IO unsigned char addr;
	__IO unsigned char checksum;
	__IO unsigned char buffer[256];
}SEND_S;
void SendKey(unsigned char Addr,unsigned char key);//have tested
void Send3ECG(unsigned char Addr,short int ecg1,short int ecg2,short int ecg3);//have tested
void SendMPU(unsigned char Addr,unsigned char accx,unsigned char accy,unsigned char accz,unsigned char gx,unsigned char gy,unsigned char gz);
void SendByUart(SEND_S *send_p,unsigned char *data_p,unsigned char num);
unsigned char Generate_Data(PACK_S *data,unsigned char addr,unsigned char length,unsigned char *value);
void SendPACK(unsigned char Addr,unsigned char size,unsigned char * p_data);
#endif

