/********************COPYRIGHT (C)  2013 Creator Lab of embedded systems *******************************
 * 文件名  ：main.c
 * 描述    ：
 * 实验平台：MINI开发板
 * 库版本  ：ST3.5.0
 * 调试方法：
********************************************************************************************************/
#include <stdio.h>
#include "stm32f4xx.h"
#include "ff.h"
#include "tjpgd.h"
#include "WKS43WV001.h"
#include "delay.h"

typedef struct {
    FIL *fp;      /*指向输入文件的指针（JPEG文件）*/  // 
    BYTE *fbuf;    /*指向输出缓冲区的指针*/// 
    UINT wfbuf;    /*输出缓冲区的宽度[pix] */// 
} IODEV;

//有关TFTLCD的尺寸的定义
#define TFTLCD_WIDTH     800
#define TFTLCD_HEIGHT    480

static int Mask_Left  = 0;                  //TFTLCD最左边的起始点
static int Mask_Right = TFTLCD_WIDTH - 1;   //TFTLCD最右边的结束点
static int Mask_Top   = 0;                  //TFTLCD最上方的起始点
static int Mask_Bottom= TFTLCD_HEIGHT - 1;  //TFTLCD最下方的结束点


//函数声明
UINT      STM32_in_func ( JDEC* jd,BYTE* buff,UINT nd) ;
UINT      STM32_out_func (JDEC* jd,void* bitmap,JRECT* rect); 
void      STM32_Display( int left,int right,int top,int bottom,const uint16_t * RGB_Data_Pointer); 
uint32_t  STM32_JPEG_DISPLAY(FIL* f_Jpeg,const char* filename);


/********************  COPYRIGHT (C) 2013 Creator Lab of embedded systems *******************************/




