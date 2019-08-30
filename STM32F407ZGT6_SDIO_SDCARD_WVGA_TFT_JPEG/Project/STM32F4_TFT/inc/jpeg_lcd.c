#include "jpeg_lcd.h"

//��¼ͼƬ�߶ȵ�ȫ�ֱ���
UINT Height_Of_Jpeg = 0;

BYTE Buff[8192] __attribute__ ((aligned(4)));	//����ȫ�������������Ϊ����Ļ�������ǿ��4�ֽڶ���

/*********************************************************************************************
  ��������:	STM32_in_func
  ��������:	�û��Զ�������������ļ����ݵĹ��ܺ���
  ��ڲ���:	������ͷ
  ���ڲ���:	��ȡ����ɾ����������
  ȫ�ֱ���:	   ��
  ��ע˵��:	�������ڽ���׼�����������ڶ�ȡ�ļ�ͷ��Ϣ
*********************************************************************************************/
UINT STM32_in_func(
	JDEC* jd,	/*���������Ķ�����Ϣ�Ľṹ�� */
	BYTE* buff,	/* �������ݻ����� (NULL:ɾ������) */
	UINT nd		/*��Ҫ����������������/ɾ����������*/
){
	UINT rb;
	FIL * dev = (FIL *)jd->device;	/* ��������ļ�����Ϣ��ʹ��FATFS�е�FIL�ṹ���ͽ��ж��� */  
	
	/*��ȡ������Ч����ʼ��ȡ���� */
	if (buff){
		f_read(dev, buff, nd, &rb);	//����FATFS��f_read���������ڰ�jpeg�ļ������ݶ�ȡ����
		return rb;					/* ���ض�ȡ�����ֽ���Ŀ*/
	}else{
		return (f_lseek(dev, f_tell(dev) + nd) == FR_OK) ? nd : 0;/* ���¶�λ���ݵ㣬�൱��ɾ��֮ǰ��n�ֽ����� */
	}
}

/*********************************************************************************************
  ��������:	STM32_out_func
  ��������:	�û��Զ�����������RGBλͼ���ݵĹ��ܺ���
  ��ڲ���:	������ͷ
  ���ڲ���:	1������뺯������ִ��
  ȫ�ֱ���:	   ��
  ��ע˵��:	��
*********************************************************************************************/
UINT STM32_out_func(
	
	JDEC* jd,		/*���������Ķ�����Ϣ�Ľṹ��*/
	void* bitmap,	/* ָ��ȴ������RGBλͼ���� ��ָ��*/
	JRECT* rect		/* �ȴ�����ľ���ͼ��Ĳ��� */
){
	jd = jd;		/* ˵�������������JDEC�ṹ��û���õ� */
	
	STM32_Display(rect->left, rect->right, rect->top, rect->bottom, (uint16_t*)bitmap);
	
	return 1;		/*����1��ʹ���빤������ִ�� */
}

/*********************************************************************************************
  ��������:	STM32_Display
  ��������:	��TFTLCD��Ļ����ʾͼƬ
  ��ڲ���:	������ͷ
  ���ڲ���:	��
  ȫ�ֱ���:	   ��
  ��ע˵��:	��
*********************************************************************************************/
void STM32_Display(
	int left,							/*ͼƬ����ʼ�㣬��һ�е���ʼ�� */
	int right,						   /*ͼƬ�ҷ��Ľ����㣬��һ�еĽ�����*/
	int top,							 /* ͼƬ�Ϸ�����ʼ�㣬��һ�е���ʼ�� */
	int bottom,						  /*ͼ���·��Ľ����㣬��һ�еĽ����� */
	const uint16_t * RGB_Data_Pointer	/* ����ʾ��ͼ�����ݣ�RGB��ʽ*/
){
	int height_jpeg=0,width_jpeg=0,x_width=0,lines_change=0;	
	uint32_t width_clip=0;  //����һ��Ҫ��ʼ������Ȼ���������벻�����鷳
	
	//��ʼ����󣬲�ִ����ʾ����
	if (left > right || top > bottom){
		return;
	}
	
	// ͼ�񳬳���Ļ����ʾ��Χ����ִ����ʾ����
	if (left > Mask_Right || right < Mask_Left  || top > Mask_Bottom || bottom < Mask_Top){
		return;
	}
	
	//��ʼ��ͼƬ�Ŀ�Ⱥ͸߶Ƚ��вü����޸�  
	height_jpeg = bottom - top  + 1;	// ����ͼ��ĸ߶� 
	width_jpeg = right  - left + 1;		//����ͼ��Ŀ��
	
	// �ü�����ͼƬ����������ʾ��Χ�Ĳ�������
	if (top < Mask_Top){
		//�������Ҫɾ����������
		RGB_Data_Pointer += width_jpeg * (Mask_Top- top);
		height_jpeg-= Mask_Top- top;
		top = Mask_Top;
	}
	
	// �ü�����ͼƬ�ײ�������ʾ��Χ�Ĳ�������
	if (bottom > Mask_Bottom){
		height_jpeg-= bottom - Mask_Bottom;
		bottom = Mask_Bottom;
	}
	
	// �ü�����ͼƬ��߳�����ʾ��Χ�Ĳ�������
	if (left < Mask_Left){
		RGB_Data_Pointer += Mask_Left - left; //ˮƽ���������ָ������
		width_jpeg -= Mask_Left - left;  //ˮƽ��ȼ�С
		width_clip+= Mask_Left - left;//��¼�ü����Ŀ��
		left = Mask_Left;
	}
	
	// �ü�����ͼƬ�ұ߳�����ʾ��Χ�Ĳ�������
	if (right > Mask_Right){
		width_jpeg -= right - Mask_Right;
		width_clip+=right - Mask_Right;
		right = Mask_Right;
	}
	
	//��ͼƬ�ĸ߶�С��LCD��ʾ���ĸ߶�ʱ��ʹͼƬ�ܹ�������ʾ
	if(Height_Of_Jpeg<TFTLCD_HEIGHT)
		//����ͼƬ�߶�
		lines_change=(TFTLCD_HEIGHT-Height_Of_Jpeg)/2;
	
	//����TFTLCD��ʾ������ʾ����
	//�ڴ˴���������TFTLCD��Ļ��ʾͼƬ�Ĵ��ڵĹ��ܺ��������þ�����ʾ����
	LCD_SetCursor(left,right,top+lines_change,bottom+lines_change);
	
	//LCD_WR_REG(R32,left);
	//�ڴ˴���������TFTLCD���λ�õĹ��ܺ����������ص㶨λ����ʼ�㣨���Ͻǣ�
	//LCD_WR_REG(R33,top+lines_change);
	
	//��TFTLCD��ʾ��������д������
	//�ڴ˴����뿪ʼд��TFTLCD��GRAM���ܵĺ�����
	//LCD_WR_ADD(R34);
	
	//����TFTLCD�Ŀ����������ԣ��������������������������úõľ�����������
	//������ʾ����ĸ߶�
	while(height_jpeg--){
		
		x_width = width_jpeg;
		
		//�ڴ˴�������TFTLCDд�����ݵĹ��ܺ�������ˮƽ���������������һ�е�����
		while(x_width--)
			LCD->LCD_RAM = *RGB_Data_Pointer++;
		
		//ͼƬ��ˮƽ����ĵ�����ȥ���ü�������������
		RGB_Data_Pointer += width_clip; 			
	}
}

/*********************************************************************************************
  ��������:	STM32_JPEG_DISPLAY
  ��������:	���������е��õ�JPEGͼƬ��ʾ����
  ��ڲ���:	f_Jpeg��ָ��JPEGͼƬ�ļ���ָ�룬��ֵ����
			   filename��JPEGͼƬ�ļ�������
  ���ڲ���:	0������ִ�гɹ�
			   ����������ִ��ʧ��
  ȫ�ֱ���:	   ��
  ��ע˵��:	��
*********************************************************************************************/
uint32_t  STM32_JPEG_DISPLAY(FIL* f_Jpeg,const char* filename){
	
	FRESULT	Res_FatFs;		//FATFSϵͳ��������ֵ
	JDEC jd;				/* ������Ķ������Ϣ��¼�� */
	JRESULT	Res_TjpgDec;	// TjpgDecϵͳ��������ֵ
	BYTE scale;				//ͼ���������
	
	//ʹ��FATFS��f_open�����Զ���ʽ���ļ�
	Res_FatFs = f_open(f_Jpeg,filename,FA_READ);
	
	//���ļ�ʧ�ܣ�����
	if(Res_FatFs!= FR_OK){
		delay_1ms();
		f_close(f_Jpeg);
		return Res_FatFs;
	}
	
	//ִ�н����׼������������TjpgDecģ���jd_prepare����
	Res_TjpgDec = jd_prepare(&jd,STM32_in_func,Buff,sizeof(Buff),f_Jpeg);
	//����ͼƬ�ĸ߶�����
	Height_Of_Jpeg=jd.height;
	
	if (Res_TjpgDec == JDR_OK) {
		
		//ȷ�����ͼ��ı�������
		for (scale = 0; scale < 3; scale++){
			if ((jd.width >> scale) <= 800 && (jd.height >> scale) <= 480){
				break;
			}
		}
		
		//�����׼240��ȵ�ͼƬʱ�����ڸ߶ȹ��ߵ�������н���һ�������Ĵ���
		if(scale)
			scale--;
		
		//ִ�н��빤��������TjpgDecģ���jd_decomp����
		Res_TjpgDec = jd_decomp(&jd, STM32_out_func, scale);
	
	//�����׼������ʧ�ܣ��ر��ļ�������
	}else{
		delay_1ms();
		f_close(f_Jpeg);
		return Res_TjpgDec;
	}
	
	//���빤��ִ�гɹ�������0
	f_close(f_Jpeg);
	
	return 0;
}

