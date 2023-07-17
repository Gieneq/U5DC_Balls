/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//#include "bsp.h"
#include "stm32_lcd.h"
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
//ALIGN_32BYTES (uint32_t   PhysFrameBuff[184320]);
#define FRAMEBUFER_SIZE ((uint32_t)(480 * 481 * 1))
extern uint32_t lcd_framebuffer[FRAMEBUFER_SIZE];
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define VFP 50
#define PIXEL_PERLINE 768
#define LCD_WIDTH 480
#define VBP 12
#define LCD_FRAME_BUFFER GFXMMU_VIRTUAL_BUFFER0_BASE
#define HACT 480
#define VSYNC 1
#define HFP 1
#define VACT 481
#define HBP 1
#define LCD_HEIGHT 481
#define HSYNC 2
#define LCD_FRAME_BUFFER_ADDRESS ((uint32_t)(lcd_framebuffer))
#define DSI_NRES_Pin GPIO_PIN_5
#define DSI_NRES_GPIO_Port GPIOD
#define DSI_BL_CTRL_Pin GPIO_PIN_6
#define DSI_BL_CTRL_GPIO_Port GPIOI
#define BLUE_BUTTON_Pin GPIO_PIN_13
#define BLUE_BUTTON_GPIO_Port GPIOC
#define BLUE_BUTTON_EXTI_IRQn EXTI13_IRQn
#define TEMPT_INTRN_Pin GPIO_PIN_2
#define TEMPT_INTRN_GPIO_Port GPIOF
#define TEMPT_INTRN_EXTI_IRQn EXTI2_IRQn

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
