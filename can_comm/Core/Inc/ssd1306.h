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

#ifndef SSD1306_H
#define SSD1306_H 100

/* C++ detection */
#ifdef __cplusplus
extern C {
#endif

/* stm32fxxx_hal.h */
#include "stm32f1xx_hal.h"

#include "fonts.h"

#include "stdlib.h"
#include "string.h"
#include <stdarg.h>


/* I2C address */
#ifndef SSD1306_I2C_ADDR
#define SSD1306_I2C_ADDR         0x78
#endif

/* SSD1306 settings */
/* SSD1306 width in pixels */
#ifndef SSD1306_WIDTH
#define SSD1306_WIDTH            128
#endif
/* SSD1306 LCD height in pixels */
#ifndef SSD1306_HEIGHT
#define SSD1306_HEIGHT           64
#endif

/**
 * @brief  SSD1306 color enumeration
 */
typedef enum {
	SSD1306_COLOR_BLACK = 0x00, /*!< Black color, no pixel */
	SSD1306_COLOR_WHITE = 0x01  /*!< Pixel is set. Color depends on LCD */
} SSD1306_COLOR_t;

/**
 * @brief  Initializes SSD1306 LCD
 * @param  None
 * @retval Initialization status:
 *           - 0: LCD was not detected on I2C port
 *           - 1: LCD initialized OK and ready to use
 */
uint8_t SSD1306_Init(void);

/** 
 * @brief  Updates buffer from internal RAM to LCD
 * @note   This function must be called each time you do some changes to LCD, to update buffer from RAM to LCD
 * @param  None
 * @retval None
 */
void SSD1306_UpdateScreen(void);

/**
 * @brief  Toggles pixels invertion inside internal RAM
 * @note   @ref SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  None
 * @retval None
 */
void SSD1306_ToggleInvert(void);

/** 
 * @brief  Fills entire LCD with desired color
 * @note   @ref SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  Color: Color to be used for screen fill. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
 * @retval None
 */
void SSD1306_Fill(SSD1306_COLOR_t Color);

/**
 * @brief  Draws pixel at desired location
 * @note   @ref SSD1306_UpdateScreen() must called after that in order to see updated LCD screen
 * @param  x: X location. This parameter can be a value between 0 and SSD1306_WIDTH - 1
 * @param  y: Y location. This parameter can be a value between 0 and SSD1306_HEIGHT - 1
 * @param  color: Color to be used for screen fill. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
 * @retval None
 */
void SSD1306_DrawPixel(uint16_t x, uint16_t y, SSD1306_COLOR_t color);

/**
 * @brief  Sets cursor pointer to desired location for strings
 * @param  x: X location. This parameter can be a value between 0 and SSD1306_WIDTH - 1
 * @param  y: Y location. This parameter can be a value between 0 and SSD1306_HEIGHT - 1
 * @retval None
 */
void SSD1306_GotoXY(uint16_t x, uint16_t y);

/**
 * @brief  Puts character to internal RAM
 * @note   @ref SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  ch: Character to be written
 * @param  *Font: Pointer to @ref FontDef_t structure with used font
 * @param  color: Color used for drawing. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
 * @retval Character written
 */
char SSD1306_Putc(char ch, FontDef_t* Font, SSD1306_COLOR_t color);

/**
 * @brief  Puts string to internal RAM
 * @note   @ref SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  *str: String to be written
 * @param  *Font: Pointer to @ref FontDef_t structure with used font
 * @param  color: Color used for drawing. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
 * @retval Zero on success or character value when function failed
 */
char SSD1306_Puts(char* str, FontDef_t* Font, SSD1306_COLOR_t color);

/**
 * @brief  Draws line on LCD
 * @note   @ref SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  x0: Line X start point. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y0: Line Y start point. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  x1: Line X end point. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y1: Line Y end point. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  c: Color to be used. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
 * @retval None
 */
void SSD1306_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, SSD1306_COLOR_t c);

/**
 * @brief  Draws rectangle on LCD
 * @note   @ref SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  x: Top left X start point. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y: Top left Y start point. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  w: Rectangle width in units of pixels
 * @param  h: Rectangle height in units of pixels
 * @param  c: Color to be used. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
 * @retval None
 */
void SSD1306_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SSD1306_COLOR_t c);

/**
 * @brief  Draws filled rectangle on LCD
 * @note   @ref SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  x: Top left X start point. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y: Top left Y start point. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  w: Rectangle width in units of pixels
 * @param  h: Rectangle height in units of pixels
 * @param  c: Color to be used. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
 * @retval None
 */
void SSD1306_DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SSD1306_COLOR_t c);

/**
 * @brief  Draws triangle on LCD
 * @note   @ref SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  x1: First coordinate X location. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y1: First coordinate Y location. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  x2: Second coordinate X location. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y2: Second coordinate Y location. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  x3: Third coordinate X location. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y3: Third coordinate Y location. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  c: Color to be used. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
 * @retval None
 */
void SSD1306_DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, SSD1306_COLOR_t color);

/**
 * @brief  Draws circle to STM buffer
 * @note   @ref SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  x: X location for center of circle. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y: Y location for center of circle. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  r: Circle radius in units of pixels
 * @param  c: Color to be used. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
 * @retval None
 */
void SSD1306_DrawCircle(int16_t x0, int16_t y0, int16_t r, SSD1306_COLOR_t c);

/**
 * @brief  Draws filled circle to STM buffer
 * @note   @ref SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  x: X location for center of circle. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y: Y location for center of circle. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  r: Circle radius in units of pixels
 * @param  c: Color to be used. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
 * @retval None
 */
void SSD1306_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, SSD1306_COLOR_t c);

/**
 * @brief  Scroll the Scream pixels to right
 * @note   Scroll the screen for fixed rows
 * @param  start_row: Starting row to Scroll to right
 * @param  end_row: Ending row to Scroll to right
 * @retval None
 */
void SSD1306_ScrollRight(uint8_t start_row, uint8_t end_row);

/**
 * @brief  Scroll the Scream pixels to left
 * @note   Scroll the screen for fixed rows
 * @param  start_row: Starting row to Scroll to left
 * @param  end_row: Ending row to Scroll to left
 * @retval None
 */
void SSD1306_ScrollLeft(uint8_t start_row, uint8_t end_row);

/**
 * @brief  Scroll the Scream pixels to diagonal right
 * @note   Scroll the screen for fixed rows
 * @param  start_row: Starting row to Scroll to diagonal right
 * @param  end_row: Ending row to Scroll to diagonal right
 * @retval None
 */
void SSD1306_Scrolldiagright(uint8_t start_row, uint8_t end_row);

/**
 * @brief  Scroll the Scream pixels to diagonal left
 * @note   Scroll the screen for fixed rows
 * @param  start_row: Starting row to Scroll to diagonal left
 * @param  end_row: Ending row to Scroll to diagonal left
 * @retval None
 */
void SSD1306_Scrolldiagleft(uint8_t start_row, uint8_t end_row);

/**
 * @brief  Stop scrolling the Scream
 * @note   None
 * @retval None
 */
void SSD1306_Stopscroll(void);

/**
 * @brief  Inverts the display colors
 * @note   i = 1->inverted, i = 0->normal
 * @retval None
 */
void SSD1306_InvertDisplay (int i);

/**
 * @brief  Clear the Display
 * @note   Fill display with SSD1306_COLOR_BLACK
 * @retval None
 */
void SSD1306_Clear (void);

/**
 * @brief  Draws the Bitmap
 * @param  X:  X location to start the Drawing
 * @param  Y:  Y location to start the Drawing
 * @param  *bitmap : Pointer to the bitmap
 * @param  W : width of the image
 * @param  H : Height of the image
 * @param  color : 1-> white/blue, 0-> black
 */
void SSD1306_DrawBitmap(int16_t x, int16_t y, const unsigned char* bitmap, int16_t w, int16_t h, uint16_t color);

/**
 * @brief  Imports a Bitmap and shows it on display
 * @note   Receives a pointer to const unsigned char
 * @param  bitmap: Pointer to Bitmap
 * @retval None
 */
void SSD1306_ShowBitmap(const unsigned char bitmap[]);

/**
 * @brief  Shows a GIF on display
 * @note   The number of frames is unlimited
 * @param  n_frames: How many frames will be used
 * @param  ... : Unlimited pointers to const unsigned char
 * @retval None
 */
void SSD1306_ShowGif(uint8_t n_frames, ...);

/**
 * @brief  Shows a increasing count on display
 * @note   None
 * @param  seconds: Counting time
 * @retval None
 */
void SSD1306_Counter(uint8_t seconds);

/**
 * @brief  Sends formatted output to display
 * @note   It's similar to int printf(const char *format, ...) in C
 * @note   https://www.tutorialspoint.com/c_standard_library/c_function_printf.htm
 * @note   The user can print a maximum of 11 characters.
 * @note   The user can print a maximum of 3 lines. In the fourth line, this function will clean the scream to print it.
 * @retval None
 *
 * @param  format:
 * 		This is the string that contains the text to be written to stdout.
 *      It can optionally contain embedded format tags that are replaced by the values specified
 * 	    in subsequent additional arguments and formatted as requested.
 *		Format tags prototype is %[flags][width][.precision][length]specifier
 *
 * @example
 *
 * 	for (uint8_t i = 0; i < 10; i++)
 *	{
 *		SSD1306_Println("var1 = %i", i);
 *		SSD1306_Println("var2 = %d", i*3);
 *		SSD1306_Println("var3 = %i", i*4);
 *		HAL_Delay(1000);
 *	}
 */
void SSD1306_Println(char* format, ...);

/* I2C Functions */

#ifndef ssd1306_I2C_TIMEOUT
#define ssd1306_I2C_TIMEOUT					20000
#endif

/**
 * @brief  Initializes I2C
 * @param  None
 * @retval None
 */
void ssd1306_I2C_Init();

/**
 * @brief  Writes single byte to slave
 * @param  *I2Cx: I2C used
 * @param  address: 7 bit slave address, left aligned, bits 7:1 are used, LSB bit is not used
 * @param  reg: register to write to
 * @param  data: data to be written
 * @retval None
 */
void ssd1306_I2C_Write(uint8_t address, uint8_t reg, uint8_t data);

/**
 * @brief  Writes multi bytes to slave
 * @param  *I2Cx: I2C used
 * @param  address: 7 bit slave address, left aligned, bits 7:1 are used, LSB bit is not used
 * @param  reg: register to write to
 * @param  *data: pointer to data array to write it to slave
 * @param  count: how many bytes will be written
 * @retval None
 */
void ssd1306_I2C_WriteMulti(uint8_t address, uint8_t reg, uint8_t *data, uint16_t count);

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif
