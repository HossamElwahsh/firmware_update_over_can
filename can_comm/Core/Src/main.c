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


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

typedef enum
{
	APP_STATE_NORMAL 	=	 0		,
	APP_STATE_SENDING_UPDATE		,
	APP_STATE_TOTAL
}en_app_state_t;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CAN_HandleTypeDef hcan;

I2C_HandleTypeDef hi2c1;

/* USER CODE BEGIN PV */

/* application current state */
en_app_state_t en_gs_app_state = APP_STATE_NORMAL;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CAN_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */
static void app_fill_array_with_str(uint8_t * u8ptr_array, uint8_t * u8ptr_a_str);
static void app_tx_over_can(uint8_t * msg);
static void app_tx_number_over_can(uint32_t number);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

CAN_TxHeaderTypeDef TxHeader;
CAN_RxHeaderTypeDef RxHeader;
CAN_FilterTypeDef FilterConfig; //declare CAN filter structure

uint8_t RxData[8];

uint32_t TxMailbox;
uint8_t mailbox_free_level;
uint32_t rx_fifo_level;


/* Converter union for TxData */
union
{
	uint8_t TxData[APP_TX_DATA_LENGTH];	// Transmit buffer
	uint32_t u32_Tx_Number;
}un_gs_TxConv;

#define CAN_TX_CHUNK_SIZE_IN_WORDS 256 // Word = 32 bit


void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	// Toggle receive led status
	HAL_GPIO_TogglePin(APP_LED_RX_CAN_ARGS);

	// Receive Message
	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData);

	if(ZERO == strcmp(RxData, APP_CAN_RESP_CHECK_FOR_UPDATE))
	{
		/* Respond with OK update available */
		app_tx_over_can(APP_CAN_CMD_OK_UPDATE);
	}
	else if(ZERO == strcmp(RxData, APP_CAN_RESP_GET_UPDATE_SIZE))
	{
		/* Send Update Size */
		app_tx_number_over_can(APP_UPDATE_SIZE);
	}
	else if(ZERO == strcmp(RxData, APP_CAN_RESP_START_UPDATE))
	{
		/* Update app state to send update */
		en_gs_app_state = APP_STATE_SENDING_UPDATE;
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
//	int sent_flag = TRUE;
//	int flash_done = FALSE;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  // Init OLED
  SSD1306_Init();
  SSD1306_Clear();
  SSD1306_GotoXY(0, 0);

  SSD1306_Println("TEST EME");
//  SSD1306_Puts ("API!", &Font_11x18, 1);
  SSD1306_UpdateScreen();

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CAN_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

   TxHeader.DLC=8; //give message size of 8 byte
   TxHeader.IDE=CAN_ID_STD; //set identifier to standard
   TxHeader.RTR=CAN_RTR_DATA; //set data type to remote transmission request?
   TxHeader.StdId=APP_CAN_TX_MSG_ID; //define a standard identifier, used for message identification by filters (switch this for the other microcontroller)


   //filter one (stack light blink)
   FilterConfig.FilterFIFOAssignment=CAN_FILTER_FIFO0; //set fifo assignment
   //sFilterConfig.FilterIdHigh=0; //0x245<<5; //the ID that the filter looks for (switch this for the other microcontroller)
   //sFilterConfig.FilterIdLow=0;
   FilterConfig.FilterMaskIdHigh=0x0000;
   FilterConfig.FilterMaskIdLow=0x0000;
   FilterConfig.FilterScale=CAN_FILTERSCALE_32BIT; //set filter scale
   //FilterConfig.FilterActivation=CAN_FILTER_ENABLE;
   FilterConfig.FilterActivation = ENABLE;
   /*
    * Filter MUST be on in order to receive messages.  If disabled
    * no message can be received.
    * To receive all messages, set filters to 0;
    */

   FilterConfig.FilterBank = 0;
   FilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;


   HAL_CAN_ConfigFilter(&hcan, &FilterConfig); //configure CAN filter

   HAL_CAN_Start(&hcan); //start CAN
//   SSD1306_Println("Can Started");

//   SSD1306_UpdateScreen();

   HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING);


	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_2);
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

	uint32_t sent_data_count = ZERO;
	uint32_t next_send_index = ZERO;
	/* Number of 8-bytes chunks to be sent */
	uint32_t arr_chunks_count = ZERO;

	HAL_StatusTypeDef can_status_retval;
	unsigned int k = 0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	  if(APP_STATE_SENDING_UPDATE == en_gs_app_state)
	  {

		  // Check TX Mailbox free space
		  mailbox_free_level = HAL_CAN_GetTxMailboxesFreeLevel(&hcan);

		  // reset count
		  sent_data_count = 0;
		  arr_chunks_count = ARR_SIZE(update_data_array) / APP_TX_DATA_LENGTH;

		  while(sent_data_count < arr_chunks_count)
		  {
		//		  		  SSD1306_Println("sending chunk: %i", sent_data_count);
		//		  		   SSD1306_UpdateScreen();

			  // check mailbox
			  mailbox_free_level = HAL_CAN_GetTxMailboxesFreeLevel(&hcan);

			  // wait until a free CAN mailbox is available
			  while(ZERO == mailbox_free_level)
			  {
				  // re-check mailbox status
				  mailbox_free_level = HAL_CAN_GetTxMailboxesFreeLevel(&hcan);
			  }

			  HAL_GPIO_TogglePin(APP_LED_TX_CAN_ARGS);

			  // free space in mailbox, send next data
			  for (k = 0; k < 8; ++k)
			  {
				  next_send_index = (k + (sent_data_count * 8));

				  un_gs_TxConv.TxData[k] = update_data_array[next_send_index];
			  }

			  can_status_retval = HAL_CAN_AddTxMessage(&hcan, &TxHeader, un_gs_TxConv.TxData, &TxMailbox);

			  // Tx Check
			  if(HAL_OK == can_status_retval)
			  {
				  // send next
				  sent_data_count++;
			  }
			  else
			  {
				  // re-transmit
			  }
		  }

		  // Sending Done - Clear Flags
		  en_gs_app_state = APP_STATE_NORMAL;
		  sent_data_count = ZERO;
		  next_send_index = ZERO;
	  }
	  else
	  {
		  /* IDLE */
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
  hcan.Init.TransmitFifoPriority = ENABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN_Init 2 */

  CAN_FilterTypeDef canFilterConfig;

  canFilterConfig.FilterActivation = CAN_FILTER_ENABLE;
  canFilterConfig.FilterBank = 10;
  canFilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;
  canFilterConfig.FilterIdHigh = 0xc7 << 5;
  canFilterConfig.FilterIdLow = 0x0000;
  canFilterConfig.FilterMaskIdHigh = 0xc7 << 13;
  canFilterConfig.FilterMaskIdLow = 0x0000;
  canFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  canFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  canFilterConfig.SlaveStartFilterBank = 13;

  HAL_CAN_ConfigFilter(&hcan, &canFilterConfig);


  /* USER CODE END CAN_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

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
  __HAL_RCC_GPIOB_CLK_ENABLE();

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

static void app_tx_over_can(uint8_t * msg)
{
	/* CAN Mailbox free level */
	uint8_t mailbox_free_level;

	mailbox_free_level = HAL_CAN_GetTxMailboxesFreeLevel(&hcan);

	/* Block wait until there's a free mailbox */
	while(ZERO == mailbox_free_level)
	{
		/* Re-check mailbox free level */
		mailbox_free_level = HAL_CAN_GetTxMailboxesFreeLevel(&hcan);
	}

	TxHeader.StdId = APP_CAN_TX_MSG_ID;
	TxHeader.DLC = APP_TX_DATA_LENGTH;

	/* free space in mailbox */

	/* Fill CAN TxData buffer with CMD */
	app_fill_array_with_str(un_gs_TxConv.TxData, msg);

	/* Add Message to CAN Tx */
	HAL_CAN_AddTxMessage(&hcan, &TxHeader, un_gs_TxConv.TxData, &TxMailbox);

	/* Toggle TX LED indicator */
	HAL_GPIO_TogglePin(APP_LED_TX_CAN_ARGS);
}

static void app_tx_number_over_can(uint32_t number)
{
	/* CAN Mailbox free level */
	uint8_t mailbox_free_level;

	mailbox_free_level = HAL_CAN_GetTxMailboxesFreeLevel(&hcan);

	/* Block wait until there's a free mailbox */
	while(ZERO == mailbox_free_level)
	{
		/* Re-check mailbox free level */
		mailbox_free_level = HAL_CAN_GetTxMailboxesFreeLevel(&hcan);
	}

	un_gs_TxConv.u32_Tx_Number = number;

	TxHeader.StdId = APP_CAN_TX_MSG_ID;
	TxHeader.DLC = APP_TX_DATA_LENGTH;

	/* Add Message to CAN Tx */
	HAL_CAN_AddTxMessage(&hcan, &TxHeader, un_gs_TxConv.TxData, &TxMailbox);

	/* Toggle TX LED indicator */
	HAL_GPIO_TogglePin(APP_LED_TX_CAN_ARGS);
}

/* Fills an array of APP_TX_DATA_LENGTH bytes with an APP_TX_DATA_LENGTH byte string */
static void app_fill_array_with_str(uint8_t * u8ptr_array, uint8_t * u8ptr_a_str)
{
	uint8_t var;

	if(
			(NULL_PTR != u8ptr_array) &&
			(NULL_PTR != u8ptr_a_str)
		)
	{
		for (var = 0; var < APP_TX_DATA_LENGTH; ++var)
		{
			u8ptr_array[var] = u8ptr_a_str[var];
		}
	}
	else
	{
		/* Cancel */
	}
}
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
