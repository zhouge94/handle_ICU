/*
 * sys.c
 *
 *  Created on: Dec 5, 2016
 *      Author: ubuntu-gnome
 */
#include "sys.h"
char Debug_buffer[500];
Sys_S sys;
uint8_t debugf(char *fmt, ...)
{
    va_list argptr;
    int cnt;
    va_start(argptr, fmt);
    cnt = vsprintf(Debug_buffer, fmt, argptr);
    OUTF((uint8_t *)Debug_buffer,strlen(Debug_buffer));
    va_end(argptr);
    return(cnt);
}
void delayns(int i)
{
	int j;
	for(;i>0;i--)
		for(j=2000;j>0;j--);
}
double ZhongZhiFilter(int N,float *DATE_RAW )
{
    double value_buf[100],temp;
    double  sum=0;
    int count,i,j;
    for  (count=0;count<N;count++)
    {
        value_buf[count] = DATE_RAW[count];
    }
    for (j=0;j<N-1;j++)
    {
        for (i=0;i<N-j;i++)
        {
            if ( value_buf[i]>value_buf[i+1] )
            {
                temp = value_buf[i];
                value_buf[i] = value_buf[i+1];
                value_buf[i+1] = temp;
            }
        }
    }
    for(count=2;count<N-2;count++)
        sum += value_buf[count];
    return (double)(sum/(N-4));
}
void sys_data_init(void)
{
	sys.uart1_count=0;
} 
void USER_PWM_SetDutyRatio(TIM_HandleTypeDef *htim,uint32_t Channel,uint8_t value)  
{  
    TIM_OC_InitTypeDef sConfigOC;  
      
    uint32_t period=htim->Init.Period+1;  
    uint32_t pluse=(value * period)/100;  
      
    sConfigOC.OCMode = TIM_OCMODE_PWM1;  
    sConfigOC.Pulse = pluse;  
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;  
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;  
    HAL_TIM_PWM_ConfigChannel(htim, &sConfigOC, Channel);  
    HAL_TIM_PWM_Start(htim, Channel);     
}
