
#ifndef __OLED_H
#define __OLED_H	
//-----------------OLED端口定义----------------  					   
#include "stm32f4xx.h"	 
#include "stdlib.h"
#include "gpio.h"

#define OLED_CS_Clr() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET) //PB0
#define OLED_CS_Set() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET)

#define OLED_RST_Clr() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET)	//PB1
#define OLED_RST_Set() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET)

#define OLED_RS_Clr()  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2,GPIO_PIN_RESET)	// PB2
#define OLED_RS_Set()  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2,GPIO_PIN_SET)

#define OLED_WRD_Clr() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_RESET) // PB3
#define OLED_WRD_Set() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_SET)

#define VPP_EN_Clr() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET) // PA4
#define VPP_EN_Set() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET)

#define VPP_E_RDB_Clr() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_RESET) //PA5
#define VPP_E_RDB_Set() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET)

//PC0~7,作为数据线并行输出
#define DATAOUT(x)  GPIO_Write(GPIOC,x);// 

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Global Variables
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#define Max_Column	0x7f			// 128-1
#define Max_Row		0x7f			// 128-1
#define	Brightness	0x0F

#define	RED 	 0xf800	//??
#define	PINK	 0xf81f	//??
#define	YELLOW	 0xffe0	//??
#define	GREEN	 0x07e0	//??
#define	SKYBLUE	 0x07ff	//??(?)
#define BLUE	 0x001f	//??
#define PURPLE	 0xa11e	//??
#define	WHITE	 0xffff	//??
#define BLACK	 0x0000	//??

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  color
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#define White   0xFFFF
#define Yellow  0xFFE0
#define Purple  0xF81F
#define Cyan    0x07FF
#define Red     0xF800
#define Green   0x07E0
#define Blue    0x001F
#define Black   0x0000

#define  CHAR_WIDTH		2		 //?????ASIC???(Byte)??(bit)
#define  CHAR_HEIGHT	24
#define  CHAR_STEP		16		 //??????????,?????????

unsigned int my_abs(int a);
void OLED_GPIO_configuration(void);	
void OLED_Init(void);
//void OLED_Init(void);
//void Set_Column_Address(unsigned int start_y, unsigned int end_y);
//void Set_Row_Address(unsigned int start_x, unsigned int end_x) ;
//void display_cls(unsigned int x,unsigned int y,unsigned int color);
//void OLED_DrawPoint(unsigned int x,unsigned int y,unsigned int color);
//void OLED_DrawVerticalLine(unsigned int start_y,unsigned int end_y,unsigned int x,unsigned int color);
//void OLED_DrawLevelLine(unsigned int start_x,unsigned int end_x,unsigned int y,unsigned int color);
//void OLED_DrawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2,unsigned int color);
//void Fill_Block(unsigned int start_x, unsigned int end_x,unsigned int start_y,  unsigned int end_y, unsigned int color);
//void OLED_FillGram(u16 start_x, u16 end_x, u16 start_y, u16 end_y, u16 color);

//==============================================
void WriteCommand(unsigned int Data);
void WriteData(unsigned int Data);
void OLED_CursorXY(uint16_t xn,uint16_t yn)	;
void OLED_DisplayON(uint8_t choice)	;
void OLED_Clear(void)	;
void OLED_WriteRAMOneChar(uint16_t xn,uint16_t yn,uint8_t width,uint8_t height,uint8_t* ramdata,uint16_t color);  
void OLED_DisplayChar(uint16_t xn,uint16_t yn,uint8_t c,uint16_t color);
void OLED_DisplayString(uint16_t xn,uint16_t yn,uint8_t *pstr,uint16_t color);
void OLED_XYLine(uint8_t x0, int y0, uint8_t x1, int y1,uint16_t color);
void OLED_DrawLine(uint8_t x0,uint16_t y0,uint8_t x1,uint16_t y1,uint16_t color);
void OLED_DrawCircle(uint8_t x0,uint16_t y0,uint8_t r,uint16_t color);
void OLED_ColorBar(void);
void OLED_FillBlock(uint16_t x0,uint16_t x1,uint16_t y0,uint16_t y1,uint16_t color)	;
void ftoa(double a,char *p);
void itoa(unsigned a,char *p);	  //???????????
void OLED_DisplayColorPicture(uint16_t xn,uint16_t yn,uint8_t width,uint16_t height,uint8_t *pic);
void OLED_DisplayPicture(uint16_t xn,uint16_t yn,uint8_t width,uint16_t height,uint8_t *pic,uint16_t color);
void OLED_DrawVerticalLine(unsigned int start_y,unsigned int end_y,unsigned int x,unsigned int color);
void OLED_DrawLevelLine(unsigned int start_x,unsigned int end_x,unsigned int y,unsigned int color);
void Show_Word(unsigned int start_x,unsigned int start_y,unsigned int color,unsigned int b_color);
void HEAD_WORD(void);

void Display_Environmental_Parameter(void);
void OLED_WriteRAMOneChar_BGC(uint16_t xn,uint16_t yn,uint8_t width,uint8_t height,uint8_t* ramdata,uint16_t color,uint16_t backgroundcolor);
void OLED_DisplayChar_BGC(uint16_t xn,uint16_t yn,uint8_t c,uint16_t color,uint16_t backgroundcolor);

#endif  
	 



