/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#define TFT_BL_Pin GPIO_PIN_12
#define TFT_BL_GPIO_Port GPIOE
#define TFT_CS_Pin GPIO_PIN_13
#define TFT_CS_GPIO_Port GPIOE
#define TFT_DC_Pin GPIO_PIN_14
#define TFT_DC_GPIO_Port GPIOE
#define TFT_RES_Pin GPIO_PIN_15
#define TFT_RES_GPIO_Port GPIOE
#define TDC_SCK_Pin GPIO_PIN_10
#define TDC_SCK_GPIO_Port GPIOD
#define TDC_SSN_Pin GPIO_PIN_11
#define TDC_SSN_GPIO_Port GPIOD
#define TDC_INT_Pin GPIO_PIN_12
#define TDC_INT_GPIO_Port GPIOD
#define TDC_RTN_Pin GPIO_PIN_13
#define TDC_RTN_GPIO_Port GPIOD
#define TDC_SI_Pin GPIO_PIN_14
#define TDC_SI_GPIO_Port GPIOD
#define TDC_SO_Pin GPIO_PIN_15
#define TDC_SO_GPIO_Port GPIOD
#define PULSE_Pin GPIO_PIN_10
#define PULSE_GPIO_Port GPIOA
#define AD_SCK_Pin GPIO_PIN_10
#define AD_SCK_GPIO_Port GPIOC
#define AD_MISO_Pin GPIO_PIN_11
#define AD_MISO_GPIO_Port GPIOC

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
