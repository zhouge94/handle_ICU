#include "stdio.h"
#include "stdlib.h"
#include "strings.h"
#include "unistd.h"
#include "string.h"
#include "errno.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "sys/ipc.h"
#include "sys/msg.h"
#define MSGKEY_ECG 0x1101 
#define MSGKEY_MPU 0x1102 
#define MSGKEY_Hx 0x1103 
#define MSGKEY_Key 0x1100
void SendMPUmsq(int size,void *data);
void SendECGmsq(int size,void *data);
void SendHxmsq(int size,void *data);
void SendKeymsq(unsigned char key_value);

