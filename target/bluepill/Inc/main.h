/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
#include "stm32f1xx_hal.h"

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
#define VOUT_Pin GPIO_PIN_0
#define VOUT_GPIO_Port GPIOA
#define IOUT_Pin GPIO_PIN_1
#define IOUT_GPIO_Port GPIOA
#define ILOAD_Pin GPIO_PIN_2
#define ILOAD_GPIO_Port GPIOA
#define VLOAD_Pin GPIO_PIN_3
#define VLOAD_GPIO_Port GPIOA
#define DBG_Pin GPIO_PIN_13
#define DBG_GPIO_Port GPIOC
#define T2_Pin GPIO_PIN_13
#define T2_GPIO_Port GPIOB
#define TT_Pin GPIO_PIN_14
#define TT_GPIO_Port GPIOB
#define T1_Pin GPIO_PIN_15
#define T1_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
void App(void);
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
