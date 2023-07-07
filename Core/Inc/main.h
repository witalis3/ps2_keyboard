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
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define K1_BUTTON_Pin GPIO_PIN_3
#define K1_BUTTON_GPIO_Port GPIOE
#define PE7_Pin GPIO_PIN_7
#define PE7_GPIO_Port GPIOE
#define PE9_Pin GPIO_PIN_9
#define PE9_GPIO_Port GPIOE
#define PE11_Pin GPIO_PIN_11
#define PE11_GPIO_Port GPIOE
#define PE13_Pin GPIO_PIN_13
#define PE13_GPIO_Port GPIOE
#define CLK_Pin GPIO_PIN_8
#define CLK_GPIO_Port GPIOD
#define CLK_EXTI_IRQn EXTI9_5_IRQn
#define DIN_Pin GPIO_PIN_9
#define DIN_GPIO_Port GPIOD
#define DEBUG2_Pin GPIO_PIN_6
#define DEBUG2_GPIO_Port GPIOD
#define DEBUG1_Pin GPIO_PIN_1
#define DEBUG1_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
