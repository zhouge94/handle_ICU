#ifndef SYS_H
#define SYS_H
#include "my_ini.h"
#include "stdio.h"
#define CONFIG_FILT "config.ini"
typedef struct
{
    enum {PC=0, ARM_4418 }planform;
}sys_S;
void init_sys_param();

extern sys_S sys;
#endif // SYS_H
