#ifndef DELAY_H_
#define DELAY_H_
#include "stm32f4xx.h"

void SysTick_Init(void);
void TimeTick_Decrement(void);
void delay_nus(u32 n);
void delay_1ms(void);
void delay_nms(u32 n);

#endif
