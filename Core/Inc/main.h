/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* INCLUDES ------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

//Utilities
#include "peripherals.h"

#include "i2c.h"
#include "spi.h"
#include "i2s.h"
#include "rtc.h"
#include "usart.h"

#include "rtos.h"          //RTOS routines

#include "mem_map.h"

//#include "serialcom.h"

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

//RTOS Settings

//State Machine
static const int SM_RTOS_PRIORITY = 1;
#define SM_STACKSIZE            1500

//Bluetooth App
static const int BT_RTOS_PRIORITY = 2;
#define BTAPP_STACKSIZE         1280

//Serial processing
static const int SERIAL_RTOS_PRIORITY = 2;
#define SERIAL_STACKSIZE		1024

//POWER and UI
static const int PWRUI_RTOS_PRIORITY = 3;

//I2S
static const int I2S_RTOS_PRIORITY = 5;

//Watchdog
static const int WD_TIMEOUT_MS = 1000;

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void XTALClock_Config(void);
void XTALClk_TurnON(void);
void XTALClk_TurnOFF(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define MEM_CS1_Pin GPIO_PIN_4
#define MEM_CS1_GPIO_Port GPIOA
#define LSCNTRL_Pin GPIO_PIN_0
#define LSCNTRL_GPIO_Port GPIOB
#define PM_INT_Pin GPIO_PIN_1
#define PM_INT_GPIO_Port GPIOB
#define MEM_CS2_Pin GPIO_PIN_2
#define MEM_CS2_GPIO_Port GPIOB
#define LS_EN_Pin GPIO_PIN_10
#define LS_EN_GPIO_Port GPIOB
#define DSP_CS_Pin GPIO_PIN_12
#define DSP_CS_GPIO_Port GPIOB
#define BT_NSHUTDOWN_Pin GPIO_PIN_8
#define BT_NSHUTDOWN_GPIO_Port GPIOA
#define C_CD_Pin GPIO_PIN_8
#define C_CD_GPIO_Port GPIOB
#define EN_1V2_Pin GPIO_PIN_9
#define EN_1V2_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */



/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
