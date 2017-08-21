#ifndef   DS18B20_H
#define   DS18B20_H

#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"

#define MAXNUM 5 //定义最多有5个　DS18B20

#define DS18B20_NUM     5 

#define WR_DS18B20_GP GPIOB
#define RD_DS18B20_GP GPIOB

#define WR_DS18B20_IO GPIO_PIN_1
#define RD_DS18B20_IO GPIO_PIN_0

#define WR_DS18B20_0 HAL_GPIO_WritePin(WR_DS18B20_GP, WR_DS18B20_IO, GPIO_PIN_RESET)
#define WR_DS18B20_1 HAL_GPIO_WritePin(WR_DS18B20_GP, WR_DS18B20_IO, GPIO_PIN_SET)
#define RD_DS18B20   HAL_GPIO_ReadPin(RD_DS18B20_GP,RD_DS18B20_IO)
 
  
//DS18B20指令  
typedef enum   
{  
    SEARCH_ROM          =   0xf0,   //搜索ROM指令  
    READ_ROM            =   0x33,   //读取ROM指令  
    MATH_ROM            =   0x55,   //匹配ROM指令  
    SKIP_ROM            =   0xcc,   //忽略ROM指令  
    ALARM_SEARCH        =   0xec,   //报警索索指令  
    CONVERT_T           =   0x44,   //温度转换指令  
    WRITE_SCRATCHPAD    =   0x4e,   //写暂存器指令  
    READ_SCRATCHPAD     =   0xbe,   //读取暂存器指令  
    COPY_SCRATCHPAD     =   0x48,   //拷贝暂存器指令  
    RECALL_E2           =   0xb8,   //召回EEPROM指令  
    READ_POWER_SUPPLY   =   0xb4,   //读取电源模式指令  
} DS18B20_CMD;  
  
  
  
//DS18B20 ROM编码  
typedef struct  
{  
    uint8_t  DS18B20_CODE;   //DS18B20单总线编码:0x19  
    uint8_t  SN_1;           //序列号第1字节  
    uint8_t  SN_2;           //序列号第2字节  
    uint8_t  SN_3;           //序列号第3字节  
    uint8_t  SN_4;           //序列号第4字节  
    uint8_t  SN_5;           //序列号第5字节  
    uint8_t  SN_6;           //序列号第6字节  
    uint8_t  crc8;           //CRC8校验码     
} DS18B20_ROM_CODE;     

void DS18B20_IOInit(void);           //初始化DS18B20端口，需要用到读/写两个端口
void DS18B20_WriteBit(uint8_t bit);
void DS18B20_WriteData(uint8_t data)  ;
uint8_t DS18B20_ReadBit(void);
uint8_t DS18B20_ReadData(void) ;
uint16_t DS18B20_ReadTemper(void) ;
uint8_t DS18B20_SearchROM(uint8_t (*pID)[8],uint8_t Num)  ;
uint16_t DS18B20_ReadDesignateTemper(uint8_t pID[8]) ;

void GET_DS18B20(void);
void READ_DS18B20(void);
void DS18B20_Search_ROM(void);

#endif	
