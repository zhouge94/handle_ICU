/**
  ******************************************************************************
  * File Name          : main.h
  * Description        : This file contains the common defines of the application
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H
  /* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define OLED_D13_Pin GPIO_PIN_13
#define OLED_D13_GPIO_Port GPIOC
#define OLED_D14_Pin GPIO_PIN_14
#define OLED_D14_GPIO_Port GPIOC
#define OLED_D15_Pin GPIO_PIN_15
#define OLED_D15_GPIO_Port GPIOC
#define OLED_D0_Pin GPIO_PIN_0
#define OLED_D0_GPIO_Port GPIOC
#define OLED_D1_Pin GPIO_PIN_1
#define OLED_D1_GPIO_Port GPIOC
#define OLED_D2_Pin GPIO_PIN_2
#define OLED_D2_GPIO_Port GPIOC
#define OLED_D3_Pin GPIO_PIN_3
#define OLED_D3_GPIO_Port GPIOC
#define VPP_EN_Pin GPIO_PIN_4
#define VPP_EN_GPIO_Port GPIOA
#define E_RDB_Pin GPIO_PIN_5
#define E_RDB_GPIO_Port GPIOA
#define Vibration_motor_Pin GPIO_PIN_7
#define Vibration_motor_GPIO_Port GPIOA
#define OLED_D4_Pin GPIO_PIN_4
#define OLED_D4_GPIO_Port GPIOC
#define OLED_D5_Pin GPIO_PIN_5
#define OLED_D5_GPIO_Port GPIOC
#define OLED_CS_Pin GPIO_PIN_0
#define OLED_CS_GPIO_Port GPIOB
#define OLED_RST_Pin GPIO_PIN_1
#define OLED_RST_GPIO_Port GPIOB
#define Submit_Button_Pin GPIO_PIN_10
#define Submit_Button_GPIO_Port GPIOB
#define Exit_Button_Pin GPIO_PIN_12
#define Exit_Button_GPIO_Port GPIOB
#define Up_Button_Pin GPIO_PIN_13
#define Up_Button_GPIO_Port GPIOB
#define Down_Button_Pin GPIO_PIN_14
#define Down_Button_GPIO_Port GPIOB
#define OLED_D6_Pin GPIO_PIN_6
#define OLED_D6_GPIO_Port GPIOC
#define OLED_D7_Pin GPIO_PIN_7
#define OLED_D7_GPIO_Port GPIOC
#define OLED_D8_Pin GPIO_PIN_8
#define OLED_D8_GPIO_Port GPIOC
#define OLED_D9_Pin GPIO_PIN_9
#define OLED_D9_GPIO_Port GPIOC
#define OLED_D10_Pin GPIO_PIN_10
#define OLED_D10_GPIO_Port GPIOC
#define OLED_D11_Pin GPIO_PIN_11
#define OLED_D11_GPIO_Port GPIOC
#define OLED_D12_Pin GPIO_PIN_12
#define OLED_D12_GPIO_Port GPIOC
#define OLED_WRD_Pin GPIO_PIN_3
#define OLED_WRD_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)

/**
  * @}
  */ 

/**
  * @}
*/ 

#endif /* __MAIN_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
