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
#define APP_RESET_BTN_ARGS		GPIOB, GPIO_PIN_1


/* APP CAN Commands/Messages */
#define APP_CAN_CMD_CHECK_FOR_UPDATE 	(uint8_t *)"CHECKUPD"
#define APP_CAN_CMD_GET_UPDATE_SIZE		(uint8_t *)"UPDTSIZE"
#define APP_CAN_CMD_START_UPDATE		(uint8_t *)"STARTUPD"

#define APP_CAN_RESP_OK_UPDATE			"OKUPDATE"
#define APP_CAN_RESP_NO_UPDATE			"NOUPDATE"

#define APP_OLED_WELCOME_L1				(uint8_t *)"EME"
#define APP_OLED_WELCOME_L2				(uint8_t *)"Blinking every 2s"
#define APP_OLED_WELCOME_L3				(uint8_t *)"Press 4 to update"
#define APP_OLED_NEW_VERSION_L4         (uint8_t *)"For New Version"
#define APP_OLED_NEW_VERSION_L5	    	(uint8_t *)"Press reset :)"
#define APP_OLED_CHECKING_FOR_UPDATE_L1	(uint8_t *)"Please wait"
#define APP_OLED_CHECKING_FOR_UPDATE_L2	(uint8_t *)"Checking for"
#define APP_OLED_CHECKING_FOR_UPDATE_L3	(uint8_t *)"updates..."
#define APP_OLED_NO_UPDATES             (uint8_t *)"No updates avail."
#define APP_OLED_GETTING_UPDATE_SIZE	(uint8_t *)"Retrieving size.."
#define APP_OLED_VERIFYING_UPDATE_SIZE	(uint8_t *)"Verifying size..."
#define APP_OLED_INVALID_UPDATE_SIZE    (uint8_t *)"Fail:Invalid Size"
#define APP_OLED_STARTING_UPDATE		(uint8_t *)"Starting update.."
#define APP_OLED_UPDATE_IN_PROGRESS		(uint8_t *)"Downloading data "
#define APP_OLED_UPDATED_DATA_PROCESS   (uint8_t *)"Unpacking data..."
#define APP_OLED_UPDATED_INSTALLING     (uint8_t *)"Installing update"
#define APP_OLED_UPDATED_INSTALLED      (uint8_t *)"Install Success !"

/* Helping Macros */
#define APP_UPDATE_STATUS_LED(value) HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, value)
#define APP_TOGGLE_STATUS_LED() HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13)

#define APP_OLED_WRITE(string, col, row) app_write_on_screen(string, col, row, FALSE)
#define APP_OLED_CLEAR_WRITE(string, col, row) app_write_on_screen(string, col, row, TRUE)

#define APP_PRINT_MAIN_WELCOME_SCREEN() APP_OLED_CLEAR_WRITE(APP_OLED_WELCOME_L1, 0, 0);\
		APP_OLED_WRITE(APP_OLED_WELCOME_L2, 0, 1);\
		APP_OLED_WRITE(APP_OLED_WELCOME_L3, 0, 2)

#define APP_PRINT_NEW_VERSION_AVAILABLE() APP_OLED_WRITE(APP_OLED_NEW_VERSION_L4, 0, 4);\
		APP_OLED_WRITE(APP_OLED_NEW_VERSION_L5, 0, 5)

#define APP_PRINT_CHECKING_FOR_UPDATES()  APP_OLED_CLEAR_WRITE(APP_OLED_CHECKING_FOR_UPDATE_L1, 0, 0);\
		APP_OLED_WRITE(APP_OLED_CHECKING_FOR_UPDATE_L2, 0, 2);\
		APP_OLED_WRITE(APP_OLED_CHECKING_FOR_UPDATE_L3, 6, 3)


/* APP Delays */
#define APP_OLED_MSG_TIMEOUT_TICKS 1

/* APP CAN Configs */
#define APP_CAN_TX_MSG_ID		0x101
#define APP_CAN_RX_MSG_ID		0x103

#define APP_TX_DATA_LENGTH	8
#define APP_RX_DATA_LENGTH 	8

/* Flashing Config */
#define APP_UPDATE_START_ADDRESS 	0x08001C00UL
#define APP_UPDATE_MAX_SIZE_BYTES	7000UL

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
