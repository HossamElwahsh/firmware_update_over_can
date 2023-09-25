/**
   ----------------------------------------------------------------------
    Copyright (C) Guilherme Amorim, <guilherme.vini65@gmail.com>, 2020
    Copyright (C) Renan Guedes, <rbguedes1998@gmail.com>, 2020
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.
     
    API: ssd1306

	University: UFMG

	Version: 1

	Files:
	- ssd1306.c
	- ssd1306.h
	- fonts.c
	- fonts.h

	Hardware and software requirements: STM32fxxx

    Note1: This API was developed as a work in the discipline of Embedded
    Systems Programming at UFMG - Prof. Ricardo de Oliveira Duarte -
    Department of Electronic Engineering
    
    Note2: This API is an Update to <github.com/SL-RU/stm32libs> API

	Default pinout:

    SSD1306    |STM32Fxxx    |DESCRIPTION
	VCC        |3.3V - 5V    |
	GND        |GND          |
	SCL        |PB6 - PB8    |Serial clock line
	SDA        |PB7 - PB9    |Serial data line

	Configuration:

	1) Copy ssd1306.h and fonts.h to your Inc project folder
	2) Copy ssd1306.c and fonts.c to your Src project folder
	3) Select your i2c struct pointer in ssd1306.c (extern I2C_HandleTypeDef hi2c1)
	4) Include ssd1306.h where this API will be used
	5) include your HAL lib project in ssd1306.h and fonts.h

   ----------------------------------------------------------------------
*/

#ifndef FONTS_H
#define FONTS_H 120

/* C++ detection */
#ifdef __cplusplus
extern C {
#endif

/**
 *
 * Default fonts library. It is used in all LCD based libraries.
 *
 * \par Supported fonts
 * 
 * Currently, these fonts are supported:
 *  - 7 x 10 pixels
 *  - 11 x 18 pixels
 *  - 16 x 26 pixels
 */

/* stm32fxxx_hal.h */
#include "stm32f1xx_hal.h"
#include "string.h"

/**
 * @defgroup LIB_Typedefs
 * @brief    Library Typedefs
 * @{
 */

/**
 * @brief  Font structure used on my LCD libraries
 */
typedef struct {
	uint8_t FontWidth;    /*!< Font width in pixels */
	uint8_t FontHeight;   /*!< Font height in pixels */
	const uint16_t *data; /*!< Pointer to data font data array */
} FontDef_t;

/** 
 * @brief  String length and height 
 */
typedef struct {
	uint16_t Length;      /*!< String length in units of pixels */
	uint16_t Height;      /*!< String height in units of pixels */
} FONTS_SIZE_t;

/**
 * @}
 */

/**
 * @defgroup FONTS_FontVariables
 * @brief    Library font variables
 * @{
 */

/**
 * @brief  7 x 10 pixels font size structure 
 */
extern FontDef_t Font_7x10;

/**
 * @brief  11 x 18 pixels font size structure 
 */
extern FontDef_t Font_11x18;

/**
 * @brief  16 x 26 pixels font size structure 
 */
extern FontDef_t Font_16x26;

/**
 * @}
 */
 
/**
 * @defgroup FONTS_Functions
 * @brief    Library functions
 * @{
 */

/**
 * @brief  Calculates string length and height in units of pixels depending on string and font used
 * @param  *str: String to be checked for length and height
 * @param  *SizeStruct: Pointer to empty @ref FONTS_SIZE_t structure where informations will be saved
 * @param  *Font: Pointer to @ref FontDef_t font used for calculations
 * @retval Pointer to string used for length and height
 */
char* FONTS_GetStringSize(char* str, FONTS_SIZE_t* SizeStruct, FontDef_t* Font);

/**
 * @}
 */
 
/**
 * @}
 */
 
/**
 * @}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

 
#endif
