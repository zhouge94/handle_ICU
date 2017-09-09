#include "sys.h"

sys_S sys;
void init_sys_param()
{
    my_ini config(CONFIG_FILT);
    char *temp;
    temp=config.GetIniKeyString("Init","platform");
    if(temp!=NULL)
    {
        if(!strcmp(temp,"PC"))sys.planform=sys_S::PC;
        else if(!strcmp(temp,"ARM_4418"))sys.planform=sys_S::ARM_4418;
        printf("Platform[%s],%d \r\n",temp,sys.planform);
    }
}

