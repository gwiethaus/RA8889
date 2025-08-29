
/************** RAIO Technology Inc. / Lcdvision Co., Ltd.***************
 * file			: main.c
 * author		: LCDVISION FAE TEAM
 * version	: V1.3
 * date			: 2024/07/01
 * brief		:	STC8H8K64U + RA8889 ��ʾ����������MCUͨ�Žӿڲ���8080 
************************* bbs.lcdvision.com.cn **************************/

#include "stc8h.h"       //������ͷ�ļ��󣬲���Ҫ�ٰ���"reg51.h"ͷ�ļ�
#include <intrins.h>

/* ��Ƭ����������غ��� */
#include "MCU_Dev_Board.h"

/* RA8889ͷ�ļ� */
#include "RA8889_51.h"
#include "RA8889_API_51.h"
#include "RA8889_MCU_IF.h"
#include "UserDef.h"
#include "delay.h"

/* FLASHͼƬ��ͷ�ļ� */
#include "flash.h"


/**************************** ȫ�ֱ������� ******************************/




/*************************************************************************
*                             ��������
*************************************************************************/
void Introduction (void);				//���ֽ���
void Geometry_Demo(void);				//���λ�ͼ��ʾ
void Show_BMP(void);						//����FLASH��BMPͼƬ��ʾ
void Show_JPG(void);						//����FLASH��JPGͼƬ��ʾ
void Fade_in_Fade_out(void);		//ͼƬ���뵭����ʾ
void Show_AVI(void);						//����FLASH��AVIӰƬ����
void Show_PNG(void);						//����FLASH��PNGͼƬ��ʾ


/*************************************************************************
  * @brief  ������
  * @param  ��
  * @retval ��
  * @note   SPIͨ�Žӿڣ�Һ������ʾɫ���Ϊ24bpp������ʾ
  ************************************************************************/
void main()
{
	P2M0 = 0x00;  			//����P2Ϊ˫���ģʽ  Data_Bus  8bit
  P2M1 = 0x00; 

	P0M0 = 0xFF;				//����P0Ϊ�������ģʽ
	P0M1 = 0x00;
	
	P1M0 = 0xFF;  			//����P1Ϊ�������ģʽ
  P1M1 = 0x00;
	

	/* ��ʼ����˸LED */
	LED2_FLASH();
	
	/* RA8889Ӳ����λ */
	RA8889_RST = 1;
	delay_ms(10);
	RA8889_RST = 0;
	delay_ms(10);
	RA8889_RST = 1;
	
	/* ���SPIͨ���Ƿ���������������˸LED2 */
	//	while(1) Comm_Test();

	/* RA8889��ʼ�� */
	RA8889_Initial();
	
	/* ѭ��������ʾ */
	while(1)
	{
		Introduction();				//���ֽ���
		Geometry_Demo();			//���λ�ͼ��ʾ
		Show_BMP();						//����FLASH��BMPͼƬ��ʾ
		Show_JPG();						//����FLASH��JPGͼƬ��ʾ
		Fade_in_Fade_out();		//ͼƬ���뵭����ʾ
		Show_AVI();						//����FLASH��AVIӰƬ����
		Show_PNG();						//����FLASH��PNGͼƬ��ʾ
	}
	
}




/*****************************************************************
  * @brief  ���ֽ���
  * @param  ��
  * @retval ��
  * @note   
  ****************************************************************/
void Introduction (void)
{	
	unsigned char i=3;

	/* ��ʼ��ַ�������趨 */
	Main_Image_Start_Address(0);
	Main_Image_Width(canvas_image_width);							
	Canvas_Image_Start_address(0);
	Canvas_image_width(canvas_image_width);
	
	/* ��ʼ���ꡢ��ǰ���ڿ���趨 */
	Active_Window_XY(0,0);
	Active_Window_WH(LCD_Width,LCD_Height);
	
	BTE_Solid_Fill(0,canvas_image_width,0,0,color16M_black,LCD_Width,LCD_Height);
	Draw_Square(color16M_green,0,0,LCD_Width-1,LCD_Height-1);
	
	Select_Font_Height_WxN_HxN_ChromaKey_Alignment(24,1,1,1,0);			//�趨���ָ�ʽ
	Foreground_color_16M(color16M_white);
	Goto_Text_XY(124,30);
	Show_String("Solution: MCU 8051 + RA8889.");
	Goto_Text_XY(124,60);
	Show_String("RA8889 is an efficient TFT LCD CONTROLLER.");
	Goto_Text_XY(124,90);
	Show_String("It has an embedded decoder for JPG and AVI.");
	
	Draw_Square_Fill(color16M_red,124,180,124+150,180+200);
	Draw_Square_Fill(color16M_green,124+200,180,124+150+200,180+200);
	Draw_Square_Fill(color16M_blue,124+200+200,180,124+150+200+200,180+200);
	delay_ms(1000);

	BTE_Solid_Fill(0,canvas_image_width,0,0,color16M_black,LCD_Width,LCD_Height);
	
	Draw_Square_Fill(color16M_red,10,10,40,60);
	Draw_Square_Fill(color16M_green,50,10,80,60);
	Draw_Square_Fill(color16M_blue,90,10,120,60);

	Foreground_color_16M(color16M_white);
	
	/* ��Щ�򿪿��ܻᳬ����Ƭ����RAM��������ʾ���룬��ʱ���� */
	Goto_Text_XY(10,24*i++);
	Show_String("RA8889 is a low power TFT controller with powerful display");
	Goto_Text_XY(10,24*i++);
	Show_String("functions and build-in internal SDRAM memory. In order to quickly");
	Goto_Text_XY(10,24*i++);
	Show_String("refresh the screen content with the display memory. RA8889");
	Goto_Text_XY(10,24*i++);
	Show_String("provides not only parallel 8080/6800 and 8/16-bit MCU interface,");
	Goto_Text_XY(10,24*i++);
	Show_String("but also 3/4 wire SPI and IIC serial interface. Plenty powerful");
	Goto_Text_XY(10,24*i++);
	Show_String("functions are provides from RA8889, such as multiple display");
	Goto_Text_XY(10,24*i++);
	Show_String("buffers, Picture-in-Picture,transparency control,display with");
	Goto_Text_XY(10,24*i++);
	Show_String("rotation & mirror, and build-in JPEG and AVI decoder.");


	Select_Font_Height_WxN_HxN_ChromaKey_Alignment(24,2,2,1,0);			//�趨���ָ�ʽ���Ŵ�����

	i+=3;
	Goto_Text_XY(10,300);
	Foreground_color_16M(color16M_yellow);
	Show_String("Contact us:");
	Goto_Text_XY(10,350);
	Show_String("Wechat/QQ: 598076690");
	Goto_Text_XY(10,400);
	Show_String("BBS: bbs.lcdvision.com.cn");

	delay_ms(1000);
}


/*****************************************************************
  * @brief  ���Ƽ���ͼ����ʾ
  * @param  ��
  * @retval ��
  * @note   
  ****************************************************************/
void Geometry_Demo(void)
{
	BTE_Solid_Fill(0,canvas_image_width,0,0,color16M_black,LCD_Width,LCD_Height);
	Foreground_color_16M(color16M_purple);
	Select_Font_Height_WxN_HxN_ChromaKey_Alignment(24,2,2,1,0);			//�趨���ָ�ʽ���Ŵ�����
	Goto_Text_XY(10,10);
	Show_String("Geometry drawing.");
	delay_ms(500);
	
	Foreground_color_65k(0x0);
	Square_Start_XY(0,0);
	Square_End_XY(LCD_Width-1,LCD_Height-1);
	Start_Square_Fill();
	
	Draw_Line(color16M_purple,10,10,799-10,479-10);
	Draw_Line(color16M_purple,11,11,799-11,479-11);

	Draw_Triangle(color16M_red,50,0,50,100,150,100);
	Draw_Triangle_Fill(color16M_green,200,0,200,100,300,100);

	Draw_Circle(color16M_blue,400,50,50);
	Draw_Circle_Fill(color16M_white,550,50,50);
	
	Draw_Ellipse(color16M_yellow,100,200,100,50);	
	Draw_Ellipse_Fill(color16M_purple,350,200,100,50);
	
	Draw_Left_Up_Curve(color16M_blue,550,190,50,50);
	Draw_Right_Down_Curve(color16M_white,560,200,50,50);
	Draw_Right_Up_Curve(color16M_red,560,190,50,50);
	Draw_Left_Down_Curve(color16M_yellow,550,200,50,50);

	Draw_Left_Up_Curve_Fill(color16M_blue,700,190,50,50);
	Draw_Right_Down_Curve_Fill(color16M_white,710,200,50,50);
	Draw_Right_Up_Curve_Fill(color16M_red,710,190,50,50);
	Draw_Left_Down_Curve_Fill(color16M_yellow,700,200,50,50);
	
	Draw_Square(color16M_red,50,300,150,400);			    
	Draw_Square_Fill(color16M_red,200,300,300,400);
	
	Draw_Circle_Square(color16M_purple,450,300,550,400,10,10);
	Draw_Circle_Square_Fill(color16M_green,600,300,700,400,10,10);
	
	delay_ms(1000);

}	


/*****************************************************************
  * @brief  ����FLASH��BMPͼƬ��ʾ
  * @param  ��
  * @retval ��
  * @note   ѭ����ʾBMPͼƬ
  ****************************************************************/
void Show_BMP (void)
{
	unsigned char n;
	
	BTE_Solid_Fill(0,canvas_image_width,0,0,color16M_black,LCD_Width,LCD_Height);
	Foreground_color_16M(color16M_red);
	Select_Font_Height_WxN_HxN_ChromaKey_Alignment(24,2,2,1,0);			//�趨���ָ�ʽ���Ŵ�����
	Goto_Text_XY(10,10);
	Show_String("Showing picture of BMP format.");
	Goto_Text_XY(10,60);
	Show_String("DMA BMP: FLASH->DISPLAY RAM");
	delay_ms(1000);
	
	SPI_NOR_initial_DMA (3,BUS0,CS1,1,0);				//ƬѡBUS0,CS1 ��ע�⣺��ʼ������BMP��JPG�Ĳ�ͬ��

	//��1~3����BMPͼƬ
	for(n=0;n<3;n++)
	{		
		DMA_24bit(2,0,0,800,480,BINARY_INFO[n].img_width,BINARY_INFO[n].start_addr);
		delay_ms(100);
	}
	
	delay_ms(1000);
}


/*****************************************************************
  * @brief  ����FLASH��JPGͼƬ��ʾ
  * @param  ��
  * @retval ��
  * @note   ѭ����ʾJPGͼƬ����ʾ�ٶȳ�����
  ****************************************************************/
void Show_JPG (void)
{
	unsigned char n;
	
	BTE_Solid_Fill(0,canvas_image_width,0,0,color16M_black,LCD_Width,LCD_Height);
	Foreground_color_16M(color16M_green);
	Select_Font_Height_WxN_HxN_ChromaKey_Alignment(24,2,2,1,0);			//�趨���ָ�ʽ���Ŵ�����
	Goto_Text_XY(10,10);
	Show_String("Showing picture of JPG format.");
	Goto_Text_XY(10,60);
	Show_String("DMA JPG: FLASH->DISPLAY RAM");
	delay_ms(1000);
	
	SPI_NOR_initial_JPG_AVI(1,0,BUS0,CS1,1);					//ƬѡBUS0,CS1
	
	//��4~8����JPGͼƬ
	for(n=3;n<8;n++)
	{
		JPG_NOR(BINARY_INFO[n].start_addr,BINARY_INFO[n].img_size,canvas_image_width,0,0);
		delay_ms(300);
	}
	
	delay_ms(1000);
}


/*****************************************************************
  * @brief  ͼƬ���뵭����ʾ
  * @param  ��
  * @retval ��
  * @note   ����2��ͼƬ�����(0,480)��(800,480)��������ͼƬ������ʾ
  ****************************************************************/
void Fade_in_Fade_out (void)
{
	unsigned char n,a;
	
	BTE_Solid_Fill(0,canvas_image_width,0,0,color16M_black,LCD_Width,LCD_Height);
	Foreground_color_16M(color16M_green);
	Select_Font_Height_WxN_HxN_ChromaKey_Alignment(24,2,2,1,0);			//�趨���ָ�ʽ���Ŵ�����
	Goto_Text_XY(10,10);
	Show_String("Fade in & fade out.");
	delay_ms(1000);
	
	SPI_NOR_initial_JPG_AVI(1,0,BUS0,CS1,1);					//ƬѡBUS0,CS1

	//��4~8����JPGͼƬ��ѭ������2�Ų����й����л���ʾ
	for(n=3;n<7;n++)
	{
		JPG_NOR(BINARY_INFO[n].start_addr,BINARY_INFO[n].img_size,canvas_image_width,0,480);
		JPG_NOR(BINARY_INFO[n+1].start_addr,BINARY_INFO[n+1].img_size,canvas_image_width,800,480);
		
		for(a=0;a<=32;a=a+2)
		{
			BTE_Alpha_Blending_Picture_Mode(0,canvas_image_width,0,480,0,canvas_image_width,800,480,0,canvas_image_width, 0,0,800,480,a);
		}
	}
	
	delay_ms(1000);
}


/*****************************************************************
  * @brief  ����FLASH��AVIӰƬ����
  * @param  ��
  * @retval ��
  * @note   ����һ��AVIӰƬ���ߴ�600x250��������ʾ
  ****************************************************************/
void Show_AVI (void)
{
	unsigned char temp;
	
	BTE_Solid_Fill(0,canvas_image_width,0,0,color16M_black,LCD_Width,LCD_Height);
	Foreground_color_16M(color16M_white);
	Select_Font_Height_WxN_HxN_ChromaKey_Alignment(24,2,2,1,0);			//�趨���ָ�ʽ���Ŵ�����
	Goto_Text_XY(10,10);
	Show_String("Playing AVI video.");

	SPI_NOR_initial_JPG_AVI(1,0,BUS0,CS1,1);					//ƬѡBUS0,CS1
	
	AVI_NOR(BINARY_INFO[10].start_addr,BINARY_INFO[10].img_size,shadow_buff,shadow_buff+2400,(LCD_Width-600)/2,(LCD_Height-250)/2,250,600,canvas_image_width);
	AVI_window(1);
	
	do{	
			temp = Read_Media_Decode_Busy();	//read busy flag to know media decode busy or idle
	}while(temp&0x40);
	AVI_window(0);
}


/*****************************************************************
  * @brief  ����FLASH��PNGͼƬ��ʾ
  * @param  ��
  * @retval ��
  * @note   ��ʾPNGͼƬ��ͬʱ����PNG
  ****************************************************************/
void Show_PNG (void)
{
	unsigned short n,x,y,dir_x,dir_y;
	
	BTE_Solid_Fill(0,canvas_image_width,0,0,color16M_black,LCD_Width,LCD_Height);
	Foreground_color_16M(color16M_green);
	Select_Font_Height_WxN_HxN_ChromaKey_Alignment(24,2,2,1,0);			//�趨���ָ�ʽ���Ŵ�����
	Goto_Text_XY(10,10);
	Show_String("Showing picture of PNG format.");
	delay_ms(1000);

	SPI_NOR_initial_JPG_AVI(1,0,BUS0,CS1,1);					//ƬѡBUS0,CS1
	JPG_NOR(BINARY_INFO[3].start_addr,BINARY_INFO[3].img_size,canvas_image_width,0,0);			//Layer0 ������
	JPG_NOR(BINARY_INFO[3].start_addr,BINARY_INFO[3].img_size,canvas_image_width,0,480);		//Layer1 ������������
	JPG_NOR(BINARY_INFO[3].start_addr,BINARY_INFO[3].img_size,canvas_image_width,800,480);	//Layer1 ����������Ҳ�

	SPI_NOR_initial_DMA (4,BUS0,CS1,1,0);				//ƬѡBUS0,CS1 ��ע�⣺��ʼ������BMP��JPG�Ĳ�ͬ��
	SPI_NOR_DMA_png (BINARY_INFO[8].start_addr,Layer4,0,BINARY_INFO[8].img_width,BINARY_INFO[8].img_height);		//����8�ţ�PNGͼƬ������DMA�ŵ�Layer4����
	
	x=123;
	y=218;			
	dir_x=1;	//PNG�ƶ�����0:�ݼ���1���ݽ�
	dir_y=1;	//PNG�ƶ�����0:�ݼ���1���ݽ�
	
	for(n=0;n<300;n++)
	{
		//��Դ0=JPG����ͼƬ   ��Դ1=PNG����
		BTE_alpha_blending_32bit_Pixel_mode(BINARY_INFO[8].img_width,BINARY_INFO[8].img_width,BINARY_INFO[8].img_height,x,y,Layer1,canvas_image_width,x,y,Layer1,canvas_image_width,Layer4);	//(S0X,S0Y)<--��ͬ����-->(desX,desY)��PNGչ������Layer1(x,y)
		BTE_Memory_Copy(Layer1,canvas_image_width,0,0,0,canvas_image_width,0,0,Layer0,canvas_image_width,0,0,0x0C,800,480);				//��Layer1(0,0) -> Layer0(0,0) �����������PNGͼƬ��copy
		BTE_Memory_Copy(Layer1,canvas_image_width,800,0,0,canvas_image_width,0,0,Layer1,canvas_image_width,0,0,0x0C,800,480);			//��Layer1(800,0) -> Layer1(0,0) �ָ���ͼ

		//x������͵ݼ�
		if(dir_x==1 && x<800-400)
		{
			x++;
			if(x==800-400-1)	dir_x=0;
		}
		else if(dir_x==0 && x>0)
		{
			x--;
			if(x==0)	dir_x=1;
		}		
		
		//y������͵ݼ�
		if(dir_y==1 && y<480-100)
		{
			y++;
			if(y==480-100-1)	dir_y=0;
		}
		else if(dir_y==0 && y>0)
		{
			y--;
			if(y==0)	dir_y=1;
		}
	}	
	
}

