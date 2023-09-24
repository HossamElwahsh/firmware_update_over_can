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

/* USER CODE BEGIN Private defines */

/* APP LEDs/Peripherals */
#define APP_LED_TX_CAN_ARGS		GPIOA, GPIO_PIN_1
#define APP_LED_RX_CAN_ARGS		GPIOA, GPIO_PIN_2
#define APP_LED_STATUS_ARGS		GPIOC, GPIO_PIN_13
#define APP_UPDATE_BTN_ARGS		GPIOB, GPIO_PIN_0
#define APP_BUZZER_ARGS			GPIOB, GPIO_PIN_4

/* Helping Macros */
#define APP_UPDATE_STATUS_LED(value) HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, value)
#define APP_TOGGLE_STATUS_LED() HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13)
#define APP_BUZZ()	HAL_GPIO_WritePin(APP_BUZZER_ARGS, GPIO_PIN_SET);\
					HAL_Delay(APP_BUZZ_DELAY_MS);\
					HAL_GPIO_WritePin(APP_BUZZER_ARGS, GPIO_PIN_RESET)

/* APP Delays */
#define APP_SEND_DELAY_MS  200
#define APP_BTN_DEBOUNCE_DELAY_MS 50
#define APP_BUZZ_DELAY_MS 100

/* APP CAN Configs */
#define APP_CAN_TX_MSG_ID		0x101
#define APP_CAN_RX_MSG_ID		0x103

#define APP_TX_DATA_LENGTH	8
#define APP_RX_DATA_LENGTH 	8

/* Flashing Config */
#define APP_UPDATE_START_ADDRESS 	0x08001C00UL
#define APP_UPDATE_MAX_SIZE_BYTES	7000UL

/* APP CAN Commands/Messages */
#define APP_CAN_CMD_CHECK_FOR_UPDATE 	"CHECKUPD"
#define APP_CAN_CMD_GET_UPDATE_SIZE		"UPDTSIZE"
#define APP_CAN_CMD_START_UPDATE		"STARTUPD"

#define APP_CAN_RESP_OK_UPDATE			"OKUPDATE"
#define APP_CAN_RESP_NO_UPDATE			"NOUPDATE"

//#define APP_CHECK_RX_WORDS_COUNT 580UL

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
