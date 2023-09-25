/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "std.h"
#include "FLASH_PAGE_F1.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define APP1_ADDRESS (0x08004C00UL)
#define APP2_ADDRESS (0x08001C00UL)

#define APP_BTN1_ARGS GPIOB, GPIO_PIN_11
#define APP_BTN2_ARGS GPIOB, GPIO_PIN_10
#define APP_RED_LED_ARGS GPIOA, GPIO_PIN_2

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

// ARM Compiler-6
#define EnablePrivilegedMode() __asm("SVC #0")

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
static void BootJump(uint32_t * ptr_uint32_address);
__attribute__( ( naked, noreturn ) ) void BootJumpASM( uint32_t SP, uint32_t RH );
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
    BOOLEAN new_version_installed = FALSE;
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
  /* USER CODE BEGIN 2 */

  // button states
  GPIO_PinState btn1_state;
  GPIO_PinState btn2_state;

  // Enable Red LED
  HAL_GPIO_WritePin(APP_RED_LED_ARGS, GPIO_PIN_SET);
  HAL_Delay(500);
  HAL_GPIO_WritePin(APP_RED_LED_ARGS, GPIO_PIN_RESET);
  HAL_Delay(500);

    /* Check for new version installation to switch to */
    /* Buffer for flash data */
    uint32_t dataBuffer[1] = {0};

    /* Read flash data at new version installation address */
    Flash_Read_Data(APP2_ADDRESS, dataBuffer, 0);

    /* Check if there's actually new version installed in that address */
    if(dataBuffer[0] != UINT32_MAX_VAL)
    {
        /* Update new version flag */
        new_version_installed = TRUE;
    }
    else
    {
        /* Update new version flag */
        new_version_installed = FALSE;
    }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  HAL_GPIO_WritePin(APP_RED_LED_ARGS, GPIO_PIN_SET);

	  btn1_state = HAL_GPIO_ReadPin(APP_BTN1_ARGS);
	  btn2_state = HAL_GPIO_ReadPin(APP_BTN2_ARGS);

	  if(GPIO_PIN_RESET == btn1_state)
	  {
		  // De-init HAL
		  HAL_DeInit();

		  // Goto application 1
		  BootJump(( uint32_t * )APP1_ADDRESS);
	  }
	  else if(GPIO_PIN_RESET == btn2_state)
	  {
		  // De-init HAL
		  HAL_DeInit();

		  // Goto application 2
		  BootJump(( uint32_t * )APP2_ADDRESS);
	  }
      else
      {
          if(TRUE == new_version_installed)
          {
              // Goto new application version
              BootJump(( uint32_t * )APP2_ADDRESS);
          }
          else
          {
              // Goto Base Application
              BootJump(( uint32_t * )APP1_ADDRESS);
          }
      }
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB10 PB11 */
  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
static void BootJump(uint32_t * ptr_uint32_address)
{
	// Make sure CPU is in priviledge mode
	  if( CONTROL_nPRIV_Msk & __get_CONTROL( ) )
	  {  // not in privileged mode
	    EnablePrivilegedMode( ) ;
	  }

	  // Disable all NVIC interrupts
	  NVIC->ICER[ 0 ] = 0xFFFFFFFF ;
	  NVIC->ICER[ 1 ] = 0xFFFFFFFF ;
	  NVIC->ICER[ 2 ] = 0xFFFFFFFF ;
	  NVIC->ICER[ 3 ] = 0xFFFFFFFF ;
	  NVIC->ICER[ 4 ] = 0xFFFFFFFF ;
	  NVIC->ICER[ 5 ] = 0xFFFFFFFF ;
	  NVIC->ICER[ 6 ] = 0xFFFFFFFF ;
	  NVIC->ICER[ 7 ] = 0xFFFFFFFF ;

	  // Clear all pending NVIC interrupts
	  NVIC->ICPR[ 0 ] = 0xFFFFFFFF ;
	  NVIC->ICPR[ 1 ] = 0xFFFFFFFF ;
	  NVIC->ICPR[ 2 ] = 0xFFFFFFFF ;
	  NVIC->ICPR[ 3 ] = 0xFFFFFFFF ;
	  NVIC->ICPR[ 4 ] = 0xFFFFFFFF ;
	  NVIC->ICPR[ 5 ] = 0xFFFFFFFF ;
	  NVIC->ICPR[ 6 ] = 0xFFFFFFFF ;
	  NVIC->ICPR[ 7 ] = 0xFFFFFFFF ;

	  // Disable Systick
	  SysTick->CTRL = 0 ;
	  SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk ;

	  // Disable fault handlers
	  SCB->SHCSR &= ~( SCB_SHCSR_USGFAULTENA_Msk | \
	                   SCB_SHCSR_BUSFAULTENA_Msk | \
	                   SCB_SHCSR_MEMFAULTENA_Msk ) ;

	  // Activate MSP
	  if( CONTROL_SPSEL_Msk & __get_CONTROL( ) )
	  {  /* MSP is not active */
	    __set_MSP( __get_PSP( ) ) ;
	    __set_CONTROL( __get_CONTROL( ) & ~CONTROL_SPSEL_Msk ) ;
	  }

	  // Load vector table
	  SCB->VTOR = ( uint32_t )ptr_uint32_address ;

	  // Jump MSP/SP
	  BootJumpASM( ptr_uint32_address[ 0 ], ptr_uint32_address[ 1 ] ) ;

}

__attribute__( ( naked, noreturn ) ) void BootJumpASM( uint32_t SP, uint32_t RH )
{
  __asm("MSR      MSP,r0");
  __asm("BX       r1");
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
