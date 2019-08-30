/**
  ******************************************************************************
  * @file    SDIO/SDIO_uSDCard/main.c 
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    13-November-2013
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "delay.h"
#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include <stdio.h>
#include <string.h>
#include "WKS43WV001.h"
#include "delay.h"
#include "jpeg_lcd.h"

static void NVIC_Configuration(void);

uint8_t sd_ok_flag;

FATFS fatfs;
FIL fsrc;
DIR dir;
FILINFO fileInfo;
UINT bw;

FRESULT res;

BYTE work[FF_MAX_SS]; /* Work area (larger is better for processing time) */

uint8_t img_buff[240];
char fname_temp[32];

int main(void){
	
	uint16_t i;
	
	/* Initialize LEDs available on EVAL board */
	STM_EVAL_LEDInit(LED1);
	STM_EVAL_LEDInit(LED2);
	STM_EVAL_LEDInit(LED3);
	STM_EVAL_LEDInit(LED4);  

	/* NVIC Configuration */
	NVIC_Configuration();
	
	SysTick_Init();
	lcd_ini();
	LCD_Clear(Green);
	
	LCD_SetCursor(50,100,50,150);
	
	for(i = 0; i < 50*100; i++){
		LCD->LCD_RAM = Blue; 
	}
	
	delay_1ms();
	
	sd_ok_flag = 0x01;
	
	/*------------------------------ SD Init ---------------------------------- */
	if(disk_initialize(0) != RES_OK){
		STM_EVAL_LEDOn(LED1);
		sd_ok_flag = 0x00;
	}
	
	if(sd_ok_flag){
		
		res = f_mount(NULL, "0:",1);
		res = f_mount(&fatfs, "0:",1);
		
		STM_EVAL_LEDOn(LED2);
	}
	
	LCD_SetCursor(0,799,0,479);
	
	/* Infinite loop */
	while(1){
		
		res = f_opendir(&dir,"0:");
		
		if(res==FR_OK){
			while(1){
				res = f_readdir(&dir, &fileInfo);
				
				if(res != 0 || fileInfo.fname[0] == 0)
					break;
				
				if(fileInfo.fname[0] == '.')
					continue;
				
				strcpy(fname_temp,"0:/");
				strcat(fname_temp, fileInfo.fname);
				
				res = STM32_JPEG_DISPLAY(&fsrc, fname_temp);
				
				delay_nms(5000);
				STM_EVAL_LEDOn(LED3);
				delay_nms(5000);
				STM_EVAL_LEDOff(LED3);
			}
		}
	}
}


static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Configure the NVIC Preemption Priority Bits */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  NVIC_InitStructure.NVIC_IRQChannel = SD_SDIO_DMA_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_Init(&NVIC_InitStructure);  
}
