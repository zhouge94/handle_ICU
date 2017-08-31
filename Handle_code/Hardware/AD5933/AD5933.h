#ifndef __AD5933_H
#define __AD5933_H	
#include "stm32f4xx.h"

#define AD5933_addr 0x1A  //´Ó»úµØÖ· £¨AD5933£©
#define	Start_frequency	     30000
#define	increment_frequency	 2
#define	number_increments	   10
#define calibrationRes       10000   //10K
#define pi                   3.141592654  

void Init_AD5933(void);
void Read_AD5933_Temputer(void);
void Sample (void);
void GetGainFactor(void);

 
#endif
