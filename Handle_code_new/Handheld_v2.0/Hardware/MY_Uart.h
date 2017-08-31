#ifndef _MY_UART_H_
#define _MY_UART_H_
#include "usart.h"
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
	
typedef struct
{
	uint8_t s1;
	uint8_t s2;
	uint8_t size;
	uint8_t nsize;
	uint8_t addr;
	uint8_t checksum;
	uint8_t buffer[256];
}REV_S;	
	
extern uint8_t rx_len_1;
extern uint8_t recv_end_flag_1;
extern REV_S rx_buffer_1;

extern uint8_t rx_len_2;
extern uint8_t recv_end_flag_2;
extern uint8_t rx_buffer_2[128];

extern uint8_t rx_len_6;
extern uint8_t recv_end_flag_6;
extern uint8_t rx_buffer_6[128];

#endif //_MY_UART_H
	
