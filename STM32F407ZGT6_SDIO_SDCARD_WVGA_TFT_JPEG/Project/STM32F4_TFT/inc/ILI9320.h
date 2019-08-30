/*
 * ILI9320.h
 *
 * Created: 16/10/2014 0:20:27
 *  Author: brian
 */ 

#ifndef _ILI9320_H
#define _ILI9320_H

#include "stm32f4xx.h"
#include "delay.h"
#include "stdint.h"

typedef struct 
{
  int16_t X;
  int16_t Y;
} Point, * pPoint;

typedef struct
{
  u16  LCD_REG;
  u16  LCD_RAM;
} LCD_TypeDef;

#define LCD_BASE	(uint32_t) (0x6C000000 | 0x0000007E)
#define LCD			((LCD_TypeDef *) LCD_BASE)

#define LCD_DIR_HORIZONTAL       0x0000
#define LCD_DIR_VERTICAL         0x0001

#define LCD_PIXEL_WIDTH          0x0140
#define LCD_PIXEL_HEIGHT         0x00F0

#define ASSEMBLE_RGB(R ,G, B)    ((((R)& 0xF8) << 8) | (((G) & 0xFC) << 3) | (((B) & 0xF8) >> 3)) 


void LCD_CtrlLinesConfig(void);
void LCD_FSMCConfig(void);
void TIM_Config(void);

void lcd_ini(void);
void LCD_WriteCommand(uint16_t LCD_RegValue);
void LCD_WriteReg(uint8_t LCD_Reg,uint16_t LCD_RegValue);
uint16_t LCD_ReadReg(uint8_t LCD_Reg);

void LCD_SetCursor(uint16_t x,uint16_t y);
void LCD_Set_Window(uint8_t left, uint16_t top, uint8_t right, uint16_t bottom);

void LCD_Clear(uint16_t Color);

#endif

/* EOF */
