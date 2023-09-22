/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdio.h>
#include <string.h>
#include "FLASH_PAGE_F1.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define status(value) HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, value)

#define APP_SEND_DELAY_MS  200
#define APP_SEND_TOG_COUNT 10

#define APP_DATA_DELAY_INDEX	0
#define APP_DATA_REPEAT_INDEX	1

#define TRUE 	1
#define FALSE 	0
#define ZERO	0

#define APP_TX_MSG_ID		0x101
#define APP_RX_MSG_ID		0x103

#define APP_DATA_LENGTH		2
#define APP_REC_DATA_LENGTH 8

// Red LED RX interrupt indicator
#define APP_INT_LED_PORT		GPIOA
#define APP_INT_LED_PIN			GPIO_PIN_2

#define APP_DELAY_MS	conv.data[APP_DATA_DELAY_INDEX]
#define APP_BLINK_COUNT conv.data[APP_DATA_REPEAT_INDEX]

#define APP_2_START_ADDRESS 	0x08001C00UL

// APP LEDs
#define APP_LED_TX_CAN_ARGS		GPIOA, GPIO_PIN_1
#define APP_LED_RX_CAN_ARGS		GPIOA, GPIO_PIN_2
#define APP_LED_STATUS_ARGS		GPIOC, GPIO_PIN_13

#define APP_CHECK_RX_WORDS_COUNT 580UL


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CAN_HandleTypeDef hcan;

/* USER CODE BEGIN PV */

CAN_HandleTypeDef hcan;

/* USER CODE BEGIN PV */

CAN_TxHeaderTypeDef TxHeader; //structure for transmitting messages
CAN_RxHeaderTypeDef RxHeader; //structure for message reception
CAN_FilterTypeDef FilterConfig; //declare CAN filter structure

uint32_t TxMailbox;
uint8_t RxData[8];	//receive buffer
uint8_t txbuf[8];	//transmit buffer
uint8_t button_pressed;
uint8_t mailbox_free_level;

uint8_t to_flash_data[4640] = {0x00};

unsigned int rec_count = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CAN_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// flags
int dataCheck = FALSE;
int break_at = 16;

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	int i = 0;

		// receive led status toggle
	  HAL_GPIO_TogglePin(APP_LED_RX_CAN_ARGS);

	  HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData);		//copy message from FIFO into local variables

	  // copy data
	  //memcpy(to_flash_data + rec_count, RxData, APP_REC_DATA_LENGTH);


	  for(i = 0; i < 8; i++)
	  {
		  to_flash_data[i + rec_count] = RxData[i];
	  }

	  rec_count += APP_REC_DATA_LENGTH;


	  // check if total received count is correct
	  if(rec_count >= APP_CHECK_RX_WORDS_COUNT * APP_REC_DATA_LENGTH ) // 12584
	  {
		  dataCheck = TRUE;
	  }
	  else
	  {
		  dataCheck = FALSE;
	  }
}



/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CAN_Init();
  /* USER CODE BEGIN 2 */

  //HAL_TIM_Base_Start_IT(&htim2);

   txbuf[0] = 0x00;

   TxHeader.DLC=1; //give message size of 1 byte
   TxHeader.IDE=CAN_ID_STD; //set identifier to standard
   TxHeader.RTR=CAN_RTR_DATA; //RTR bit is set to data
   TxHeader.TransmitGlobalTime = DISABLE;

   FilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0; //set fifo assignment
   FilterConfig.FilterIdHigh = 0; //0x245<<5; //the ID that the filter looks for
   FilterConfig.FilterIdLow = 0;
   FilterConfig.FilterMaskIdHigh = 0;
   FilterConfig.FilterMaskIdLow = 0;
   FilterConfig.FilterScale = CAN_FILTERSCALE_32BIT; //set filter scale
   //FilterConfig.FilterActivation = ENABLE;
   FilterConfig.FilterActivation = ENABLE;

   HAL_CAN_ConfigFilter(&hcan, &FilterConfig); //configure CAN filter

   HAL_CAN_Start(&hcan); //start CAN

   HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING);

   int sent_flag = FALSE;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	  mailbox_free_level = HAL_CAN_GetTxMailboxesFreeLevel(&hcan);


	  TxHeader.StdId = APP_TX_MSG_ID;
	  TxHeader.DLC = APP_DATA_LENGTH;

	  if( (mailbox_free_level > 0) && (sent_flag == 0))
	  {
		  // free space in mailbox
		  txbuf[APP_DATA_DELAY_INDEX]	= APP_SEND_DELAY_MS	;
		  txbuf[APP_DATA_REPEAT_INDEX] 	= APP_SEND_TOG_COUNT;

		  HAL_CAN_AddTxMessage(&hcan, &TxHeader, txbuf, &TxMailbox);

		  HAL_GPIO_TogglePin(APP_LED_TX_CAN_ARGS);
		  sent_flag = TRUE;
	  }


	  if(TRUE == dataCheck)
	  {

	  // Flash received

		// Flash Yellow LED CODE
		Flash_Write_Data(APP_2_START_ADDRESS,
				((uint32_t *)to_flash_data), APP_CHECK_RX_WORDS_COUNT * 2); // 12584

		HAL_GPIO_TogglePin(APP_LED_STATUS_ARGS);

		dataCheck = FALSE;
	  }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CAN Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN_Init(void)
{

  /* USER CODE BEGIN CAN_Init 0 */

  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */

  /* USER CODE END CAN_Init 1 */
  hcan.Instance = CAN1;
  hcan.Init.Prescaler = 16;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_2TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = DISABLE;
  hcan.Init.AutoWakeUp = DISABLE;
  hcan.Init.AutoRetransmission = ENABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN_Init 2 */

  /* USER CODE END CAN_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1|GPIO_PIN_2, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA1 PA2 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
