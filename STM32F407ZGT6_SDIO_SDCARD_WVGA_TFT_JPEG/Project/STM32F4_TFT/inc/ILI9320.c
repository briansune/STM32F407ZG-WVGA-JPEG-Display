/*
 * ILI9320.h
 *
 * Created: 16/10/2014 0:20:27
 *  Author: brian
 */ 

#include "ILI9320.h"

uint16_t TimerPeriod    = 0;
uint16_t Channel3Pulse  = 0;

TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;


void lcd_ini(void){
	
	LCD_CtrlLinesConfig();
	delay_nus(3000);
	LCD_FSMCConfig();
	delay_nus(3000);
	TIM_Config();


	Channel3Pulse =(uint16_t)(((uint32_t)100*(TimerPeriod-1))/100);
	TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
	TIM_OCInitStructure.TIM_Pulse = Channel3Pulse;
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);

	delay_nus(50);
	LCD_WriteReg(0x0000,0x0001);
	delay_nus(50);
	LCD_ReadReg(0x0000);
	delay_nus(50);
	
	LCD_WriteReg(0x00, 0x0001); // Start internal OSC.
	delay_nus(30);
	LCD_WriteReg(0x01, 0x793F); // Driver o 0011 1101 utput control, RL=0;REV=1;GD=1;BGR=0;SM=0;TB=1
	delay_nus(30);
	LCD_WriteReg(0x02, 0x0600); // set 1 line inversion
	delay_nus(30);
	
	//************* Power control setup ************/
	LCD_WriteReg(0x0C, 0x0007); // Adjust VCIX2 output voltage
	delay_nus(30);
	LCD_WriteReg(0x0D, 0x0006); // Set amplitude magnification of VLCD63
	delay_nus(30);
	LCD_WriteReg(0x0E, 0x3200); // Set alternating amplitude of VCOM
	delay_nus(30);
	LCD_WriteReg(0x1E, 0x00BB); // Set VcomH voltage
	delay_nus(30);
	LCD_WriteReg(0x03, 0x6A64); // Step-up factor/cycle setting
	delay_nus(30);

	//************ RAM position control **********/
	LCD_WriteReg(0x0F, 0x0000); // Gate scan position start at G0.
	delay_nus(30);
	LCD_WriteReg(0x44, 0xEF00); // Horizontal RAM address position
	delay_nus(30);
	LCD_WriteReg(0x45, 0x0000); // Vertical RAM address start position
	delay_nus(30);
	LCD_WriteReg(0x46, 0x013F); // Vertical RAM address end position
	delay_nus(30);
	
	// ----------- Adjust the Gamma Curve ----------//
	LCD_WriteReg(0x30, 0x0000);delay_nus(30);
	LCD_WriteReg(0x31, 0x0706);delay_nus(30);
	LCD_WriteReg(0x32, 0x0206);delay_nus(30);
	LCD_WriteReg(0x33, 0x0300);delay_nus(30);
	LCD_WriteReg(0x34, 0x0002);delay_nus(30);
	LCD_WriteReg(0x35, 0x0000);delay_nus(30);
	LCD_WriteReg(0x36, 0x0707);delay_nus(30);
	LCD_WriteReg(0x37, 0x0200);delay_nus(30);
	LCD_WriteReg(0x3A, 0x0908);delay_nus(30);
	LCD_WriteReg(0x3B, 0x0F0D);delay_nus(30);
	
	//************* Special command **************/
	LCD_WriteReg(0x28, 0x0006); // Enable test command
	delay_nus(30);
	LCD_WriteReg(0x2F, 0x12EB); // RAM speed tuning
	delay_nus(30);
	LCD_WriteReg(0x26, 0x7000); // Internal Bandgap strength
	delay_nus(30);
	LCD_WriteReg(0x20, 0xB0E3); // Internal Vcom strength
	delay_nus(30);
	LCD_WriteReg(0x27, 0x0044); // Internal Vcomh/VcomL timing
	delay_nus(30);
	LCD_WriteReg(0x2E, 0x7E45); // VCOM charge sharing time
	delay_nus(30);
	
	//************* Turn On display ******************/
	LCD_WriteReg(0x10, 0x0000); // Sleep mode off.
	delay_nus(80);
	LCD_WriteReg(0x11, 0x6870);// Entry mode setup. 262K type B, take care on the data bus with 16it only
	delay_nus(30);
	LCD_WriteReg(0x07, 0x0033); // Display ON
	delay_nus(30);
}


void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue){
	
	LCD->LCD_REG = LCD_Reg;
	LCD->LCD_RAM = LCD_RegValue;
}

void LCD_WriteCommand(uint16_t LCD_RegValue){

	LCD->LCD_RAM = LCD_RegValue;
}

uint16_t LCD_ReadReg(uint8_t LCD_Reg){
	
	LCD->LCD_REG = LCD_Reg;
	
	return LCD->LCD_RAM;
}


void LCD_CtrlLinesConfig(void){
	
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOG | 
						   RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF, 
						   ENABLE);
  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);

  GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);		// D2
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);		// D3
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);		// NOE -> RD
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);		// NWE -> WR
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);		// D13
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);		// D14
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);		// D15
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);		// D0
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);		// D1
	
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource12, GPIO_AF_FSMC);		// A6 -> RS

  GPIO_PinAFConfig(GPIOG, GPIO_PinSource12, GPIO_AF_FSMC);		// NE4 -> CS
  
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource7, GPIO_AF_FSMC);		// D4
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource8, GPIO_AF_FSMC);		// D5
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_FSMC);		// D6
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource10, GPIO_AF_FSMC);		// D7
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_FSMC);		// D8
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource12, GPIO_AF_FSMC);		// D9
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_FSMC);		// D10
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_FSMC);		// D11
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource15, GPIO_AF_FSMC);		// D12

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 |
								  GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
								  GPIO_Pin_14 | GPIO_Pin_15;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOG, &GPIO_InitStructure);

  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
                                  GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 |
                                  GPIO_Pin_15;
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

  GPIO_Init(GPIOE, &GPIO_InitStructure);
}


void LCD_FSMCConfig(void){
	
  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef FSMC_NORSRAMTimingInitStructure;
  FSMC_NORSRAMTimingInitStructure.FSMC_AddressSetupTime = 0;  //0
  FSMC_NORSRAMTimingInitStructure.FSMC_AddressHoldTime = 0;   //0   
  FSMC_NORSRAMTimingInitStructure.FSMC_DataSetupTime = 2;     //3   
  FSMC_NORSRAMTimingInitStructure.FSMC_BusTurnAroundDuration = 0;
  FSMC_NORSRAMTimingInitStructure.FSMC_CLKDivision = 1;//1
  FSMC_NORSRAMTimingInitStructure.FSMC_DataLatency = 0;
  FSMC_NORSRAMTimingInitStructure.FSMC_AccessMode = FSMC_AccessMode_A;
  
  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Enable;//disable
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &FSMC_NORSRAMTimingInitStructure;
  
  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);
  FSMC_NORSRAMTimingInitStructure.FSMC_AddressSetupTime = 0;    //0  
  FSMC_NORSRAMTimingInitStructure.FSMC_AddressHoldTime = 0;	//0   
  FSMC_NORSRAMTimingInitStructure.FSMC_DataSetupTime = 4;	//3   
  FSMC_NORSRAMTimingInitStructure.FSMC_BusTurnAroundDuration = 0;
  FSMC_NORSRAMTimingInitStructure.FSMC_CLKDivision = 1;//1
  FSMC_NORSRAMTimingInitStructure.FSMC_DataLatency = 0;
  FSMC_NORSRAMTimingInitStructure.FSMC_AccessMode = FSMC_AccessMode_A;	
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &FSMC_NORSRAMTimingInitStructure;
  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);

  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);
}

void TIM_Config(void){
	
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB  , ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOB, GPIO_Pin_15, GPIO_AF_TIM1);
	
	TimerPeriod = (SystemCoreClock / 17570 ) - 1;
	Channel3Pulse = (uint16_t) (((uint32_t) 99 * (TimerPeriod - 1)) / 100);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE);
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
	TIM_OCInitStructure.TIM_Pulse = Channel3Pulse;
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);
	TIM_Cmd(TIM1, ENABLE);
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
				
}

void LCD_SetCursor(uint16_t x,uint16_t y){
	
	LCD_WriteReg(0x4e,x);			// set y-axis
	LCD_WriteReg(0x4f,y);			// set x-axis
	LCD->LCD_REG = 0x22;
}

void LCD_Set_Window(uint8_t left,uint16_t top,uint8_t right,uint16_t bottom){
	
	LCD_WriteReg(0x44, (right << 8) | left);
	LCD_WriteReg(0x45, top);
	LCD_WriteReg(0x46, bottom);
	LCD->LCD_REG = 0x22;
}


void LCD_Clear(uint16_t Color){

	uint32_t index = 0;

	LCD_WriteReg(0x4e,0x00);			// set y-axis
	LCD_WriteReg(0x4f,0x00);			// set x-axis
	LCD->LCD_REG = 0x22;
	
	for(index = 0; index <76800; index++)
		LCD->LCD_RAM = Color;  
}


/* EOF */
