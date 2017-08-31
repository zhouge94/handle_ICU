#include "MY_Uart.h"
#include "sys.h"
#include "cmsis_os.h"
extern osSemaphoreId send_semHandle;
PUTCHAR_PROTOTYPE
{
   ////HAL_UART_Transmit_DMA(&huart2 , (uint8_t *)&ch, 1);
	 HAL_UART_Transmit(&huart2 , (uint8_t *)&ch, 1, 0xFFFF);
    return ch;
}

REV_S rx_buffer_1;
int unpack(REV_S* Rbuff);
unsigned char checksum(void* data,int n);
void uart1_rev_cb(void)
{
	static unsigned char cur_state=0,next_state=0;
	switch(cur_state)
	{
		case 0:
			if(rx_buffer_1.s1==0xAA)
      {
        next_state=1;
      }
			else next_state=0;
			break;	 
		case 1:
			if(rx_buffer_1.s2==0xAA)
      {
        next_state=2;
      }
			else  next_state=0;
 			break;
		case 2:
			if((rx_buffer_1.size+rx_buffer_1.nsize)==0xff)
      {
        next_state=3;
      }
			else next_state=0;
			break;
		case 3:
      sys.Get_Flat=unpack(&rx_buffer_1);
			next_state=0;	 
			break;
		default:
			next_state=0;	 
			break;
	 }
  switch(next_state)
  {
    case 0:
         HAL_UART_Receive_IT(&huart1,&rx_buffer_1.s1,1);
      break;
    case 1:
         HAL_UART_Receive_IT(&huart1,&rx_buffer_1.s2,1);
      break;
    case 2:
        HAL_UART_Receive_DMA(&huart1,&rx_buffer_1.size,4);
      break;
    case 3:
        HAL_UART_Receive_DMA(&huart1,&rx_buffer_1.buffer[0],rx_buffer_1.size-2);
      break;
    default:
      break;
  }
	cur_state=next_state;		

}
unsigned char checksum(void* data,int n)
{
  int i;
  unsigned char sum=0;
  unsigned char* p=(unsigned char *)data;
  for(i=0;i<n;i++)
  {
       sum+=p[i];
  }
  return sum;
}
int unpack(REV_S* Rbuff)
{
  static unsigned int err_count=0;
  if(checksum(Rbuff->buffer,Rbuff->size-2)==Rbuff->checksum)
  {
    if(Rbuff->addr==0xff)
    {
      int i=0;
      short int ecgs1[10],ecgs2[10],ecgs3[10];
      short int ax[10],ay[10],az[10];
      do
      {
        if(Rbuff->buffer[i]==0x01&&Rbuff->buffer[i+1]==20)
        {
          memcpy(sys.ecg1buff,&Rbuff->buffer[i+2],20);
          i+=22;
          continue;
        }
        else if(Rbuff->buffer[i]==0x02&&Rbuff->buffer[i+1]==20)
        {
          memcpy(sys.ecg2buff,&Rbuff->buffer[i+2],20);
          i+=22;
          continue;
        }
        else if(Rbuff->buffer[i]==0x03&&Rbuff->buffer[i+1]==20)
        {
          memcpy(sys.ecg3buff,&Rbuff->buffer[i+2],20);
          i+=22;
          continue;
        }
        else if(Rbuff->buffer[i]==0x04&&Rbuff->buffer[i+1]==8)
        {
          memcpy(ax,&Rbuff->buffer[i+2],8);
          i+=10;
          continue;
        }
        else if(Rbuff->buffer[i]==0x05&&Rbuff->buffer[i+1]==8)
        {
          memcpy(ay,&Rbuff->buffer[i+2],8);
          i+=10;
          continue;
        }
        else if(Rbuff->buffer[i]==0x06&&Rbuff->buffer[i+1]==8)
        {
          memcpy(az,&Rbuff->buffer[i+2],8);
          i+=10;
          continue;
        }
        else if(Rbuff->buffer[i]==0x07&&Rbuff->buffer[i+1]==4)
        {
          //memcpy(&hx->hu_data,&Rbuff->buffer[i+2],4);
          i+=6;
          continue;
        }
        else i++;
      }while(i<Rbuff->size-2);
      return 1;
    }
  }else
  {
    err_count++;
  }
  return 0;
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart==&huart1)
	{
		uart1_rev_cb();
	}
}
