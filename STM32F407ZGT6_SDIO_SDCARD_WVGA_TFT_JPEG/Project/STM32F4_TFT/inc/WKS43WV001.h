/*
 * WKS43WV001.h
 *
 * Created: 16/10/2018 0:20:27
 *  Author: brian
 */ 

#ifndef _WKS43WV001_H
#define _WKS43WV001_H

#include "stm32f4xx.h"
#include "delay.h"
#include "stdint.h"


typedef struct
{
  vu16  LCD_REG;
  vu16  LCD_RAM;
} LCD_TypeDef;

#define LCD_BASE	(uint32_t) (0x6C000000 | 0x0000007E)
#define LCD			((LCD_TypeDef *) LCD_BASE)

void LCD_CtrlLinesConfig(void);
void LCD_FSMCConfig(void);

void lcd_ini(void);

void LCD_WR_REG(uint16_t LCD_Reg);
void LCD_WR_DATA(uint16_t LCD_RegValue);
void LCD_WriteReg(uint16_t LCD_Reg, uint16_t LCD_RegValue);

uint16_t LCD_ReadReg(uint16_t LCD_Reg);
void LCD_WriteRAM_Prepare(void);

void LCD_SetCursor(uint16_t x0, uint16_t x1, uint16_t y0, uint16_t y1);
void LCD_Clear(uint16_t Color);

#endif

/* EOF */
