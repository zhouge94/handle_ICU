#include "MY_Uart.h"
PUTCHAR_PROTOTYPE
{
    HAL_UART_Transmit_DMA(&huart2 , (uint8_t *)&ch, 1);
	//    HAL_UART_Transmit(&huart2 , (uint8_t *)&ch, 1, 0xFFFF);

    return ch;
}
