#include "stdio.h"
#include "stdlib.h"
#include "strings.h"
#include "unistd.h"
#include "fcntl.h"
#include "termios.h"
#include "string.h"
#include "errno.h"
#include "sys/types.h"
#include "sys/stat.h"
int open_port(const char *Portname);
int set_port(int fd,int iBaudRate,int iDataSize,char cParity,int iStopBit);
int read_port(const char *buf, int iByte);
int write_port(const char *buf,int iByte);
void Init(const char *path,int rate);
