#include "jpeg_lcd.h"

//记录图片高度的全局变量
UINT Height_Of_Jpeg = 0;

BYTE Buff[8192] __attribute__ ((aligned(4)));	//定义全局数组变量，作为输入的缓冲区，强制4字节对齐

/*********************************************************************************************
  函数名称:	STM32_in_func
  函数功能:	用户自定义的用于输入文件数据的功能函数
  入口参数:	见函数头
  出口参数:	读取或者删除的数据量
  全局变量:	   无
  备注说明:	本函数在解码准备工作中用于读取文件头信息
*********************************************************************************************/
UINT STM32_in_func(
	JDEC* jd,	/*储存待解码的对象信息的结构体 */
	BYTE* buff,	/* 输入数据缓冲区 (NULL:删除数据) */
	UINT nd		/*需要从输入数据流读出/删除的数据量*/
){
	UINT rb;
	FIL * dev = (FIL *)jd->device;	/* 待解码的文件的信息，使用FATFS中的FIL结构类型进行定义 */  
	
	/*读取数据有效，开始读取数据 */
	if (buff){
		f_read(dev, buff, nd, &rb);	//调用FATFS的f_read函数，用于把jpeg文件的数据读取出来
		return rb;					/* 返回读取到的字节数目*/
	}else{
		return (f_lseek(dev, f_tell(dev) + nd) == FR_OK) ? nd : 0;/* 重新定位数据点，相当于删除之前的n字节数据 */
	}
}

/*********************************************************************************************
  函数名称:	STM32_out_func
  函数功能:	用户自定义的用于输出RGB位图数据的功能函数
  入口参数:	见函数头
  出口参数:	1：令解码函数继续执行
  全局变量:	   无
  备注说明:	无
*********************************************************************************************/
UINT STM32_out_func(
	
	JDEC* jd,		/*储存待解码的对象信息的结构体*/
	void* bitmap,	/* 指向等待输出的RGB位图数据 的指针*/
	JRECT* rect		/* 等待输出的矩形图像的参数 */
){
	jd = jd;		/* 说明：输出函数中JDEC结构体没有用到 */
	
	STM32_Display(rect->left, rect->right, rect->top, rect->bottom, (uint16_t*)bitmap);
	
	return 1;		/*返回1，使解码工作继续执行 */
}

/*********************************************************************************************
  函数名称:	STM32_Display
  函数功能:	在TFTLCD屏幕上显示图片
  入口参数:	见函数头
  出口参数:	无
  全局变量:	   无
  备注说明:	无
*********************************************************************************************/
void STM32_Display(
	int left,							/*图片左方起始点，即一行的起始点 */
	int right,						   /*图片右方的结束点，即一行的结束点*/
	int top,							 /* 图片上方的起始点，即一列的起始点 */
	int bottom,						  /*图像下方的结束点，即一列的结束点 */
	const uint16_t * RGB_Data_Pointer	/* 待显示的图像数据，RGB格式*/
){
	int height_jpeg=0,width_jpeg=0,x_width=0,lines_change=0;	
	uint32_t width_clip=0;  //变量一定要初始化，不然会遇到意想不到的麻烦
	
	//起始点错误，不执行显示功能
	if (left > right || top > bottom){
		return;
	}
	
	// 图像超出屏幕的显示范围，则不执行显示功能
	if (left > Mask_Right || right < Mask_Left  || top > Mask_Bottom || bottom < Mask_Top){
		return;
	}
	
	//开始对图片的宽度和高度进行裁剪和修改  
	height_jpeg = bottom - top  + 1;	// 计算图像的高度 
	width_jpeg = right  - left + 1;		//计算图像的宽度
	
	// 裁减掉在图片顶部超出显示范围的部分数据
	if (top < Mask_Top){
		//计算出需要删除的数据量
		RGB_Data_Pointer += width_jpeg * (Mask_Top- top);
		height_jpeg-= Mask_Top- top;
		top = Mask_Top;
	}
	
	// 裁减掉在图片底部超出显示范围的部分数据
	if (bottom > Mask_Bottom){
		height_jpeg-= bottom - Mask_Bottom;
		bottom = Mask_Bottom;
	}
	
	// 裁减掉在图片左边超出显示范围的部分数据
	if (left < Mask_Left){
		RGB_Data_Pointer += Mask_Left - left; //水平方向的数据指针右移
		width_jpeg -= Mask_Left - left;  //水平宽度减小
		width_clip+= Mask_Left - left;//记录裁减掉的宽度
		left = Mask_Left;
	}
	
	// 裁减掉在图片右边超出显示范围的部分数据
	if (right > Mask_Right){
		width_jpeg -= right - Mask_Right;
		width_clip+=right - Mask_Right;
		right = Mask_Right;
	}
	
	//当图片的高度小于LCD显示屏的高度时，使图片能够居中显示
	if(Height_Of_Jpeg<TFTLCD_HEIGHT)
		//调整图片高度
		lines_change=(TFTLCD_HEIGHT-Height_Of_Jpeg)/2;
	
	//设置TFTLCD显示屏的显示参数
	//在此处加入设置TFTLCD屏幕显示图片的窗口的功能函数，设置矩形显示区域
	LCD_SetCursor(left,right,top+lines_change,bottom+lines_change);
	
	//LCD_WR_REG(R32,left);
	//在此处加入设置TFTLCD光标位置的功能函数，把像素点定位到起始点（左上角）
	//LCD_WR_REG(R33,top+lines_change);
	
	//向TFTLCD显示屏控制器写入数据
	//在此处加入开始写入TFTLCD的GRAM功能的函数，
	//LCD_WR_ADD(R34);
	
	//根据TFTLCD的控制器的特性，接下来可以连续地向上面设置好的矩形区域丢数据
	//控制显示区域的高度
	while(height_jpeg--){
		
		x_width = width_jpeg;
		
		//在此处加入向TFTLCD写入数据的功能函数，在水平方向输出矩形区域一行的数据
		while(x_width--)
			LCD->LCD_RAM = *RGB_Data_Pointer++;
		
		//图片的水平方向的调整，去除裁减掉的像素数据
		RGB_Data_Pointer += width_clip; 			
	}
}

/*********************************************************************************************
  函数名称:	STM32_JPEG_DISPLAY
  函数功能:	在主函数中调用的JPEG图片显示函数
  入口参数:	f_Jpeg：指向JPEG图片文件的指针，其值不定
			   filename：JPEG图片文件的名称
  出口参数:	0：函数执行成功
			   其他：函数执行失败
  全局变量:	   无
  备注说明:	无
*********************************************************************************************/
uint32_t  STM32_JPEG_DISPLAY(FIL* f_Jpeg,const char* filename){
	
	FRESULT	Res_FatFs;		//FATFS系统函数返回值
	JDEC jd;				/* 待解码的对象的信息记录表 */
	JRESULT	Res_TjpgDec;	// TjpgDec系统函数返回值
	BYTE scale;				//图像输出比例
	
	//使用FATFS的f_open函数以读方式打开文件
	Res_FatFs = f_open(f_Jpeg,filename,FA_READ);
	
	//打开文件失败，返回
	if(Res_FatFs!= FR_OK){
		delay_1ms();
		f_close(f_Jpeg);
		return Res_FatFs;
	}
	
	//执行解码的准备工作，调用TjpgDec模块的jd_prepare函数
	Res_TjpgDec = jd_prepare(&jd,STM32_in_func,Buff,sizeof(Buff),f_Jpeg);
	//保存图片的高度数据
	Height_Of_Jpeg=jd.height;
	
	if (Res_TjpgDec == JDR_OK) {
		
		//确定输出图像的比例因子
		for (scale = 0; scale < 3; scale++){
			if ((jd.width >> scale) <= 800 && (jd.height >> scale) <= 480){
				break;
			}
		}
		
		//处理标准240宽度的图片时，对于高度过高的情况进行降低一级比例的处理
		if(scale)
			scale--;
		
		//执行解码工作，调用TjpgDec模块的jd_decomp函数
		Res_TjpgDec = jd_decomp(&jd, STM32_out_func, scale);
	
	//解码的准备工作失败，关闭文件，返回
	}else{
		delay_1ms();
		f_close(f_Jpeg);
		return Res_TjpgDec;
	}
	
	//解码工作执行成功，返回0
	f_close(f_Jpeg);
	
	return 0;
}

