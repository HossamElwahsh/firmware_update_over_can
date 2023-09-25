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
#include "std.h"
#include "FLASH_PAGE_F1.h"

#include "fonts.h"
#include "ssd1306.h"
#include "bitmap.h"
#include "test.h"
#include "horse_anim.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

typedef enum
{
	APP_STATE_NORMAL 	=	 0		,
	APP_STATE_CHECK_FOR_UPDATES		,
	APP_STATE_NO_UPDATE_AV			,
	APP_STATE_GET_UPDATE_SIZE		,
	APP_STATE_GET_UPDATE_SIZE_AGAIN	,
	APP_STATE_INVALID_UPDATE_SIZE	,
	APP_STATE_START_UPDATE			,
	APP_STATE_RECEIVING_UPDATE		,
	APP_STATE_UPDATE_RECEIVED		,
	APP_STATE_FLASHING				,
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

/* Flag indicating whether a new app version is installed and ready to be switched to or not */
static BOOLEAN bool_gs_new_version_available = FALSE;
/* application current state */
static en_app_state_t en_gs_app_state = APP_STATE_NORMAL;

CAN_TxHeaderTypeDef TxHeader; //structure for transmitting messages
CAN_RxHeaderTypeDef RxHeader; //structure for message reception
CAN_FilterTypeDef FilterConfig; //declare CAN filter structure

uint32_t TxMailbox;

/* Converter union for RxData */
union
{
	uint8_t RxData[APP_RX_DATA_LENGTH];	// Receive buffer
	uint32_t u32_Rx_Number;
}un_gs_RxConv;


uint8_t TxData[APP_TX_DATA_LENGTH];	// Transmit buffer
uint8_t button_pressed;

uint8_t update_data_arr[APP_UPDATE_MAX_SIZE_BYTES] = {ZERO};

/* Counter to keep track of amount of update data received */
STATIC uint32_t u32_gs_rec_count = ZERO;

/* Holder for update size to be received */
STATIC uint32_t u32_gs_update_size = ZERO;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CAN_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

static void app_switch_state(en_app_state_t en_a_app_new_state);
static void app_fill_array_with_str(uint8_t * u8ptr_array, uint8_t * u8ptr_a_str);
static void app_tx_over_can(uint8_t * msg);
static void app_write_on_screen(uint8_t * string, uint8_t Col, uint8_t Row, boolean clearScreen);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	/* loop counter for received update data */
	int i = ZERO;

	/* Read Message into local buffer [RxData] */
	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, un_gs_RxConv.RxData);

	// Toggle Receive LED indicator
	HAL_GPIO_TogglePin(APP_LED_RX_CAN_ARGS);


	/* Switch over APP states to take appropriate actions */
	switch(en_gs_app_state)
	{
		case APP_STATE_NORMAL:
		{
		 /* Do Nothing */
		 break;
		}
		case APP_STATE_CHECK_FOR_UPDATES:
		{
			/* Check for response */
			if(ZERO == strcmp((char *)un_gs_RxConv.RxData, (char *)APP_CAN_RESP_OK_UPDATE))
			{
				/* Update Available - Request update size */
                app_switch_state(APP_STATE_GET_UPDATE_SIZE);
			}
			else if(ZERO == strcmp((char *)un_gs_RxConv.RxData, (char *)APP_CAN_RESP_NO_UPDATE))
			{
				/* No Updates - Cancel */
                app_switch_state(APP_STATE_NO_UPDATE_AV);
            }
			else
			{
				/* Drop */
			}
			break;
		}
		case APP_STATE_GET_UPDATE_SIZE:
		{
			/* Store size */
			u32_gs_update_size = un_gs_RxConv.u32_Rx_Number;

			/* Fetch again */
            app_switch_state(APP_STATE_GET_UPDATE_SIZE_AGAIN);
			break;
		}
		case APP_STATE_GET_UPDATE_SIZE_AGAIN:
		{
			/* Verify size */
			if(
					(u32_gs_update_size == un_gs_RxConv.u32_Rx_Number) &&
					(u32_gs_update_size < APP_UPDATE_MAX_SIZE_BYTES)
			)
			{
				/* Sizes Matches and Valid - Proceed with update */
                app_switch_state(APP_STATE_START_UPDATE);
			}
			else
			{
				/* Fail */
                app_switch_state(APP_STATE_INVALID_UPDATE_SIZE);
			}

			break;
		}
		case APP_STATE_RECEIVING_UPDATE:
		{
			/* Parse and Store Update Bytes */

			  for(i = 0; i < APP_RX_DATA_LENGTH; i++)
			  {
				  update_data_arr[i + u32_gs_rec_count] = un_gs_RxConv.RxData[i];
			  }

			  u32_gs_rec_count += APP_RX_DATA_LENGTH;


			  // Check if total received count is correct
			  if(u32_gs_rec_count >= u32_gs_update_size)
			  {
				  /* Update APP state */
                  app_switch_state(APP_STATE_UPDATE_RECEIVED);
			  }
			  else
			  {
				  /* Do Nothing */
			  }

		  break;
		}
        case APP_STATE_NO_UPDATE_AV:
        {
            /* Do Nothing */
            break;
        }
        case APP_STATE_INVALID_UPDATE_SIZE:
        {
            /* Do Nothing */
            break;
        }
        case APP_STATE_START_UPDATE:
        {
            /* Do Nothing */
            break;
        }
        case APP_STATE_UPDATE_RECEIVED:
        {
            /* Do Nothing */
            break;
        }
        case APP_STATE_FLASHING:
        {
            /* Do Nothing */
            break;
        }
        case APP_STATE_TOTAL:
        {
            /* Do Nothing */
            break;
        }
        default:
		{
		  /* Do Nothing */
		  break;
		}

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
	/* Update-Btn State, GPIO_PIN_SET: not pressed, GPIO_PIN_RESET: pressed */
	GPIO_PinState GPIO_Loc_UpdateBtnState;
	GPIO_PinState GPIO_Loc_ResetBtnState;


  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
	GPIO_Loc_UpdateBtnState = GPIO_PIN_SET;
	GPIO_Loc_ResetBtnState = GPIO_PIN_SET;


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

  SSD1306_Init (); // initialize the display

  SSD1306_Clear();

  /* Systick Config */
  /* Update SystemCoreClock variable according to Clock Register Values */
	SystemCoreClockUpdate();

	/* Generates interrupt every 2 seconds
	 * Handler inside stm32f1xx_it.c -> SysTick_Handler
	 * */
	SysTick_Config(SystemCoreClock*2);

	SysTick->CTRL = 0;
	SysTick->VAL = 0;
	SysTick->CTRL =
	  (
			  SysTick_CTRL_TICKINT_Msk
	  | SysTick_CTRL_ENABLE_Msk
	  | SysTick_CTRL_CLKSOURCE_Msk);


	/* Init CAN Tx Variables */
	TxData[0] = 0x00;

	TxHeader.DLC=1; //give message size of 1 byte
	TxHeader.IDE=CAN_ID_STD; //set identifier to standard
	TxHeader.RTR=CAN_RTR_DATA; //RTR bit is set to data
	TxHeader.TransmitGlobalTime = DISABLE;

   /* Configure CAN Receiving Filter */
   /* set FIFO assignment */
   FilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;
   /* 0x245<<5; the ID that the filter looks for: Zero to pass all IDs */
   FilterConfig.FilterIdHigh = 0;
   FilterConfig.FilterIdLow = 0;
   FilterConfig.FilterMaskIdHigh = 0;
   FilterConfig.FilterMaskIdLow = 0;

   /* Set Filter Scale */
   FilterConfig.FilterScale = CAN_FILTERSCALE_32BIT; //set filter scale

   /* Enable Filter */
   FilterConfig.FilterActivation = ENABLE;

   /* Configure CAN Filter */
   HAL_CAN_ConfigFilter(&hcan, &FilterConfig); //configure CAN filter

   /* Start CAN */
   HAL_CAN_Start(&hcan);

   /* Enable Rx FIFO0 Interrupt */
   HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING);

    /* Switch app state to normal */
    app_switch_state(APP_STATE_NORMAL);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	  /* switch on current app state */
	  switch(en_gs_app_state)
	  {
		  case APP_STATE_NORMAL:
		  {

			  /* Check if update button is pressed */
			  GPIO_Loc_UpdateBtnState = HAL_GPIO_ReadPin(APP_UPDATE_BTN_ARGS);
			  GPIO_Loc_ResetBtnState = HAL_GPIO_ReadPin(APP_RESET_BTN_ARGS);

			  if(GPIO_PIN_RESET == GPIO_Loc_ResetBtnState)
			  {
				  /* Clear Screen */
                  SSD1306_Clear();

                  /* Reset system - bootloader will handle the switching */
                  NVIC_SystemReset();
			  }
			  else if(GPIO_PIN_RESET == GPIO_Loc_UpdateBtnState)
			  {
				  /* Btn is pressed
				   * Check for updates */
                  app_switch_state(APP_STATE_CHECK_FOR_UPDATES);
			  }
			  else
			  {
				  /* Do Nothing */
			  }


			  break;
		  }
		  case APP_STATE_CHECK_FOR_UPDATES:
		  {
              /* Do Nothing */
              break;
		  }
		  case APP_STATE_GET_UPDATE_SIZE:
		  case APP_STATE_GET_UPDATE_SIZE_AGAIN:
		  {
              /* Do Nothing */
              break;
		  }
		  case APP_STATE_START_UPDATE:
		  {
              /* Do Nothing */
              break;
		  }
		  case APP_STATE_RECEIVING_UPDATE:
		  {
			  /* IDLE - Receiving Data */
			  break;
		  }
		  case APP_STATE_INVALID_UPDATE_SIZE:
		  {
              APP_OLED_CLEAR_WRITE(APP_OLED_INVALID_UPDATE_SIZE, 0, 0);
              HAL_Delay(APP_OLED_MSG_TIMEOUT_TICKS);
              /* Reset state back to normal */
              app_switch_state(APP_STATE_NORMAL);
              break;
		  }

		  case APP_STATE_NO_UPDATE_AV:
		  {
              APP_OLED_CLEAR_WRITE(APP_OLED_NO_UPDATES, 0, 0);
              HAL_Delay(APP_OLED_MSG_TIMEOUT_TICKS);
              /* Reset state back to normal */
              app_switch_state(APP_STATE_NORMAL);
              break;
		  }
		  case APP_STATE_UPDATE_RECEIVED:
		  {

			  /* Add Padding If needed */
			  uint8_t mod_result = MOD(u32_gs_update_size, APP_TX_DATA_LENGTH);

			  if(ZERO == mod_result)
			  {
				  /* No Padding Needed */
			  }
			  else
			  {
				  /* Add Padding */
				  uint8_t padding_count = APP_TX_DATA_LENGTH - mod_result;

				  /* Add 0xFF padding */
				  for (int var = 0; var < padding_count; ++var)
				  {
					  update_data_arr[u32_gs_rec_count + var] = BYTE_MAX_VAL;
				  }

				  /* Update size to be flashed */
				  u32_gs_update_size += padding_count;
			  }

              app_switch_state(APP_STATE_FLASHING);
			  break;
		  }
          case APP_STATE_FLASHING:
          {
              /* Flash Update
               * Division by two because we flash 16 bits (2-bytes) per write
               *  */
              Flash_Write_Data(APP_UPDATE_START_ADDRESS,
                               ((uint32_t *)update_data_arr), u32_gs_update_size / 2);

              /* Turn on status LED indicator to indicate flash is done */
              APP_UPDATE_STATUS_LED(GPIO_PIN_SET);

              APP_OLED_WRITE(APP_OLED_UPDATED_INSTALLED, 0, 4);

              HAL_Delay(APP_OLED_MSG_TIMEOUT_TICKS);

              app_switch_state(APP_STATE_NORMAL);
              break;
          }
          case APP_STATE_TOTAL:
          {
              /* Do Nothing */
              break;
          }
		  default:
		  {
			  /* Do Nothing */
			  break;
		  }

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

  /*Configure GPIO pin : PB0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

static void app_switch_state(en_app_state_t en_a_app_new_state)
{
    switch(en_a_app_new_state)
    {
        case APP_STATE_NORMAL:
        {
            /* Print welcome message */
            APP_PRINT_MAIN_WELCOME_SCREEN();

            /* Check for new version installation to switch to */
            /* Buffer for flash data */
            uint32_t dataBuffer[1] = {0};

            /* Read flash data at new version installation address */
            Flash_Read_Data(APP_UPDATE_START_ADDRESS, dataBuffer, 0);

            /* Check if there's actually new version installed in that address */
            if(dataBuffer[0] != UINT32_MAX_VAL)
            {
                /* New Version Available */
                APP_PRINT_NEW_VERSION_AVAILABLE();

                /* Update Global Flag */
                bool_gs_new_version_available = TRUE;
            }
            else
            {
                /* Update Global Flag */
                bool_gs_new_version_available = FALSE;
            }

            break;
        }
        case APP_STATE_CHECK_FOR_UPDATES:
        {
            /* Update OLED */
            APP_PRINT_CHECKING_FOR_UPDATES();

            /* Send Check for Updates Command */
            app_tx_over_can(APP_CAN_CMD_CHECK_FOR_UPDATE);
            break;
        }
        case APP_STATE_GET_UPDATE_SIZE:
        {
            /* Send Get Update Size Command */
            app_tx_over_can(APP_CAN_CMD_GET_UPDATE_SIZE);

            /* Update OLED */
            APP_OLED_WRITE(APP_OLED_GETTING_UPDATE_SIZE, 0, 4);
            break;
        }
        case APP_STATE_GET_UPDATE_SIZE_AGAIN:
        {
            /* Send Get Update Size Command */
            app_tx_over_can(APP_CAN_CMD_GET_UPDATE_SIZE);

            /* Update OLED */
            APP_OLED_WRITE(APP_OLED_VERIFYING_UPDATE_SIZE, 0, 5);
            break;
        }
        case APP_STATE_START_UPDATE:
        {
            /* Update OLED */
            APP_OLED_CLEAR_WRITE(APP_OLED_STARTING_UPDATE, 0, 0);

            /* Immediately Switch to Receiving update state to prevent data loss */
            en_gs_app_state = APP_STATE_RECEIVING_UPDATE;
            en_a_app_new_state = APP_STATE_RECEIVING_UPDATE;

            /* Send Start Update Command */
            app_tx_over_can(APP_CAN_CMD_START_UPDATE);
            /*break;*/

            /* No Break to process the Receiving update section */
        }
        case APP_STATE_RECEIVING_UPDATE:
        {
            /* Update OLED */
            APP_OLED_WRITE(APP_OLED_UPDATE_IN_PROGRESS, 0, 1);
            break;
        }
        case APP_STATE_INVALID_UPDATE_SIZE:
        {
            /* Do Nothing */
            break;
        }

        case APP_STATE_NO_UPDATE_AV:
        {
            /* Do Nothing */
            break;
        }
        case APP_STATE_UPDATE_RECEIVED:
        {
            /* Update OLED */
            APP_OLED_WRITE(APP_OLED_UPDATED_DATA_PROCESS, 0, 2);
            break;
        }
        case APP_STATE_FLASHING:
        {
            /* Do Nothing */
            APP_OLED_WRITE(APP_OLED_UPDATED_INSTALLING, 0, 3);
            break;
        }
        case APP_STATE_TOTAL:
        {
            /* Do Nothing */
            break;
        }
        default:
        {
            /* Do Nothing */
            break;
        }

    }

    /* Update global app state */
    en_gs_app_state = en_a_app_new_state;

}

/* Custom Function to write on OLED Screen
 * According to Font 7x10 and Screen Size 128x64
 * Max Col, Rows are 17, 5 respectively
 * */
static void app_write_on_screen(uint8_t * string, uint8_t Col, uint8_t Row, boolean clearScreen)
{
	if((Col > 17) || (Row > 5))
	{
		/* Cancel */
	}
	else
	{
		if(TRUE == clearScreen)
		{
			SSD1306_Clear();
		}
		else
		{
			/* Do Nothing */
		}

	/* Goto Requested Position */
	  SSD1306_GotoXY (Col * 7, Row * 10);

	  /* Write String */
	  SSD1306_Puts ((char *) string, &Font_7x10, SSD1306_COLOR_WHITE);

	  /* Update Screen */
	  SSD1306_UpdateScreen(); //display

	}
}


/* Fills an array with APP_TX_DATA_LENGTH bytes with an APP_TX_DATA_LENGTH byte string */
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
	app_fill_array_with_str(TxData, msg);

	/* Add Message to CAN Tx */
	HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox);

	/* Toggle TX LED indicator */
	HAL_GPIO_TogglePin(APP_LED_TX_CAN_ARGS);
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
