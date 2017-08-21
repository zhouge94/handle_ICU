#ifndef   DS18B20_H
#define   DS18B20_H

#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"

#define MAXNUM 5 //���������5����DS18B20

#define DS18B20_NUM     5 

#define WR_DS18B20_GP GPIOB
#define RD_DS18B20_GP GPIOB

#define WR_DS18B20_IO GPIO_PIN_1
#define RD_DS18B20_IO GPIO_PIN_0

#define WR_DS18B20_0 HAL_GPIO_WritePin(WR_DS18B20_GP, WR_DS18B20_IO, GPIO_PIN_RESET)
#define WR_DS18B20_1 HAL_GPIO_WritePin(WR_DS18B20_GP, WR_DS18B20_IO, GPIO_PIN_SET)
#define RD_DS18B20   HAL_GPIO_ReadPin(RD_DS18B20_GP,RD_DS18B20_IO)
 
  
//DS18B20ָ��  
typedef enum   
{  
    SEARCH_ROM          =   0xf0,   //����ROMָ��  
    READ_ROM            =   0x33,   //��ȡROMָ��  
    MATH_ROM            =   0x55,   //ƥ��ROMָ��  
    SKIP_ROM            =   0xcc,   //����ROMָ��  
    ALARM_SEARCH        =   0xec,   //��������ָ��  
    CONVERT_T           =   0x44,   //�¶�ת��ָ��  
    WRITE_SCRATCHPAD    =   0x4e,   //д�ݴ���ָ��  
    READ_SCRATCHPAD     =   0xbe,   //��ȡ�ݴ���ָ��  
    COPY_SCRATCHPAD     =   0x48,   //�����ݴ���ָ��  
    RECALL_E2           =   0xb8,   //�ٻ�EEPROMָ��  
    READ_POWER_SUPPLY   =   0xb4,   //��ȡ��Դģʽָ��  
} DS18B20_CMD;  
  
  
  
//DS18B20 ROM����  
typedef struct  
{  
    uint8_t  DS18B20_CODE;   //DS18B20�����߱���:0x19  
    uint8_t  SN_1;           //���кŵ�1�ֽ�  
    uint8_t  SN_2;           //���кŵ�2�ֽ�  
    uint8_t  SN_3;           //���кŵ�3�ֽ�  
    uint8_t  SN_4;           //���кŵ�4�ֽ�  
    uint8_t  SN_5;           //���кŵ�5�ֽ�  
    uint8_t  SN_6;           //���кŵ�6�ֽ�  
    uint8_t  crc8;           //CRC8У����     
} DS18B20_ROM_CODE;     

void DS18B20_IOInit(void);           //��ʼ��DS18B20�˿ڣ���Ҫ�õ���/д�����˿�
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
