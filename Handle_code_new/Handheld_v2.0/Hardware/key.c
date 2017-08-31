 #include "key.h"
 #include "cmsis_os.h"
//  GPIO_InitStruct.Pin = Submit_Button_Pin|Exit_Button_Pin|Up_Button_Pin|Down_Button_Pin;
//  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
#define READ_PIN_Submit  HAL_GPIO_ReadPin(GPIOB,Submit_Button_Pin)
#define READ_PIN_Exit    HAL_GPIO_ReadPin(GPIOB,Exit_Button_Pin)
#define READ_PIN_Up      HAL_GPIO_ReadPin(GPIOB,Up_Button_Pin)
#define READ_PIN_Down    HAL_GPIO_ReadPin(GPIOB,Down_Button_Pin)
#define LONG_PRESS 200
int  GetPress(void)
{
  int press_time=0;
  if(!(READ_PIN_Submit&&READ_PIN_Exit&&READ_PIN_Up&&READ_PIN_Down))
  {
      osDelay(10);//防抖
      if(!(READ_PIN_Submit&&READ_PIN_Exit&&READ_PIN_Up&&READ_PIN_Down))
      {
          if(READ_PIN_Submit==0)
          {
            while(READ_PIN_Submit==0)//等待按键抬起。
            {
              press_time++;
              osDelay(10);
              if(press_time>LONG_PRESS)BELL_ON;
            }
            if(press_time<LONG_PRESS)return 0x07;
            else return 0x11;
          }else if(READ_PIN_Exit==0)
          {
            while(READ_PIN_Exit==0)//等待按键抬起。
            {
              press_time++;
              osDelay(10);
              if(press_time>LONG_PRESS)BELL_ON;
            }
            if(press_time<LONG_PRESS)return 0x09;
            else return 0x21;
          }else if(READ_PIN_Up==0)
          {
            while(READ_PIN_Up==0)//等待按键抬起。
            {
              press_time++;
              osDelay(10);
              if(press_time>LONG_PRESS)BELL_ON;
            }
            if(press_time<LONG_PRESS)return 0x08;
            else return 0x31;
          }else if(READ_PIN_Down==0)
          {
            while(READ_PIN_Down==0)//等待按键抬起。
            {
              press_time++;
              osDelay(10);
              if(press_time>LONG_PRESS)BELL_ON;
            }
            if(press_time<LONG_PRESS)return 0x02;
            else return 0x41;
          }else return 0;
      }
      else return 0;
  }
  else    return 0;
}