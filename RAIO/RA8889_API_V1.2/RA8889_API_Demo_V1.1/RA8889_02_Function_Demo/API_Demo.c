
  /*************** RAIO Technology Inc. ***************
  * file		: API_Demo.c
  * author		: RAIO Application Team 
  * version		: V1.0
  * date		: 2020/04/21 
  * brief		: 	
	* RAIO copyright
  ****************************************************/	

#include "stdio.h"
#include "stdlib.h"
//#include "stm32f10x_tim.h"
#include "stm32f10x.h"
#include "delay.h" 

#include "UserDef.h"
#include "RA8889_MCU_IF.h"
#include "RA8889.h"
#include "RA8889_API.h"

#include "API_Demo.h"
#include "pic24.h"
#include "pic8.h"
#include "pic16.h"
#include "pic168.h"
#include "pic1616.h"
#include "pic16241.h"
#include "pic1624.h"
#include "8bit_test.h"
#include "16bit_test.h"
#include "8bit_8bpp_icon.h"
#include "8bit_16bpp_icon.h"
#include "8bit_24bpp_icon.h"
#include "16bit_16bpp_icon.h"
#include "16bit_24bpp1_icon.h"
#include "16bit_24bpp2_icon.h"
#include "RA8889_Application_Using_STM32.h"
#include "NAND_PIC.h"


void API_Demo(void)
{
//clear (0,0) to (1366,768)
#ifdef MCU_8bit_ColorDepth_8bpp	
BTE_Solid_Fill(0,canvas_image_width,0,0,0xff,LCD_width,LCD_legth);
#endif
#ifdef MCU_8bit_ColorDepth_16bpp	
BTE_Solid_Fill(0,canvas_image_width,0,0,0xffff,LCD_width,LCD_legth);
#endif
#ifdef MCU_8bit_ColorDepth_24bpp	
BTE_Solid_Fill(0,canvas_image_width,0,0,0xffffff,LCD_width,LCD_legth);
#endif
#ifdef MCU_16bit_ColorDepth_16bpp	
BTE_Solid_Fill(0,canvas_image_width,0,0,0xffff,LCD_width,LCD_legth);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
BTE_Solid_Fill(0,canvas_image_width,0,0,0xffffff,LCD_width,LCD_legth);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
BTE_Solid_Fill(0,canvas_image_width,0,0,0xffffff,LCD_width,LCD_legth);
#endif
	

////*********************NAND Flash
//NAND_JPG_Demo();
//NAND_AVI_Demo();
//NAND_DMA_Demo();
//BTE_alpha_32bit_blending_pixel_mode_NAND_Demo();
////*********************

////***************NOR Flash.
//API_DMA_24bit_Demo();
//API_DMA_32bit_Demo();		
//NOR_JPG_Demo();
//NOR_AVI_Demo();
//BTE_alpha_32bit_blending_pixel_mode_NOR_Demo();
////***************

//API_MPU8_8bpp_Memory_Write_Demo();
//API_MPU8_16bpp_Memory_Write_Demo();
//API_MPU8_24bpp_Memory_Write_Demo();
//API_MPU16_16bpp_Memory_Write_Demo();
//API_MPU16_24bpp_Mode1_Memory_Write_Demo();
//API_MPU16_24bpp_Mode2_Memory_Write_Demo();
//API_Draw_Line_Demo();
//API_Draw_Triangle_Demo();
//API_Draw_Circle_Demo();
//API_Draw_Ellipse_Demo();
//API_Draw_Curve_Demo();
//API_Draw_Square_Demo();
//API_Draw_Circle_Square_Demo();

//API_PIP_Demo();

//	 
//API_BTE_Pattern_Fill_Demo();
//API_BTE_Pattern_Fill_with_Chroma_key_Demo();
//API_BTE_Solid_Fill_Demo();
//API_BTE_Memory_Copy_Demo();
//API_BTE_Memory_Copy_Chroma_key_Demo();
//API_BTE_MCU_Write_MCU_8bit_Demo();				  //
//API_BTE_MCU_Write_MCU_16bit_Demo();
//API_BTE_MCU_Write_Chroma_key_MCU_8bit_Demo();
//API_BTE_MCU_Write_Chroma_key_MCU_16bit_Demo();
//API_BTE_MCU_Write_ColorExpansion_MCU_8bit_Demo();
//API_BTE_MCU_Write_ColorExpansion_Chroma_key_MCU_8bit_Demo();
//API_BTE_MCU_Write_ColorExpansion_MCU_16bit_Demo();	  
//API_BTE_MCU_Write_ColorExpansion_Chroma_key_MCU_16bit_Demo();	 
//API_BTE_Alpha_Blending_Picture_mode_Demo();

API_Print_Internal_Font_String_Demo();
//API_Print_BIG5String_Demo();
//API_Print_GB2312String_Demo();
//API_Print_GB12345String_Demo();
//API_Print_UnicodeString_Demo();


//PWM_Demo();

//RAIO_define_font();
//Print_key_code();	  //example code : print key code on LCD 


}							   

void API_MPU8_8bpp_Memory_Write_Demo(void)
{

	MPU8_8bpp_Memory_Write(0,0,128,128 ,gImage_8);
	MPU8_8bpp_Memory_Write(200,0,128,128 ,gImage_8);
	MPU8_8bpp_Memory_Write(400,0,128,128 ,gImage_8);
}

void API_MPU8_16bpp_Memory_Write_Demo(void)
{
	MPU8_16bpp_Memory_Write (0,0,128,128,gImage_16);
	MPU8_16bpp_Memory_Write (200,0,128,128,gImage_16);
	MPU8_16bpp_Memory_Write (400,0,128,128,gImage_16);
}

void API_MPU8_24bpp_Memory_Write_Demo(void)
{
	MPU8_24bpp_Memory_Write (0,0,128,128 ,gImage_24);
	MPU8_24bpp_Memory_Write (200,0,128,128,gImage_24);
	MPU8_24bpp_Memory_Write (400,0,128,128,gImage_24);
}

void API_MPU16_16bpp_Memory_Write_Demo(void)
{
	MPU16_16bpp_Memory_Write (0,0,128,128,pic1616);
	MPU16_16bpp_Memory_Write (200,0,128,128,pic1616);
	MPU16_16bpp_Memory_Write (400,0,128,128,pic1616);
}

void API_MPU16_24bpp_Mode1_Memory_Write_Demo(void)
{

	MPU16_24bpp_Mode1_Memory_Write(0,0,128,128,pic16241);
	MPU16_24bpp_Mode1_Memory_Write(200,0,128,128,pic16241);
	MPU16_24bpp_Mode1_Memory_Write(400,0,128,128,pic16241);
}

void API_MPU16_24bpp_Mode2_Memory_Write_Demo(void)
{
	MPU16_24bpp_Mode2_Memory_Write(0,0,128,128,pic1624);
	MPU16_24bpp_Mode2_Memory_Write(200,0,128,128,pic1624);
	MPU16_24bpp_Mode2_Memory_Write(400,0,128,128,pic1624);
}


void API_Draw_Line_Demo(void)
{
    Active_Window_XY(0,0);
	Active_Window_WH(1366,768);

	#ifdef MCU_8bit_ColorDepth_8bpp	
	Draw_Line(0xe0,10,10,800,700);
	#endif
	#ifdef MCU_8bit_ColorDepth_16bpp	
	Draw_Line(0xf800,10,10,800,700);
	#endif
	#ifdef MCU_8bit_ColorDepth_24bpp	
	Draw_Line(0xff0000,10,10,800,700);
	#endif
	#ifdef MCU_16bit_ColorDepth_16bpp	
	Draw_Line(0xf800,10,10,800,700);
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
	Draw_Line(0xff0000,10,10,800,700);
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
	Draw_Line(0xff0000,10,10,800,700);
	#endif
	
}

void API_Draw_Triangle_Demo(void)
{
    Active_Window_XY(0,0);
	Active_Window_WH(1366,768);

	#ifdef MCU_8bit_ColorDepth_8bpp	
	Draw_Triangle(0x07,150,0,150,100,250,100);
	Draw_Triangle_Fill(0x03,300,0,300,100,400,100);
	#endif
	#ifdef MCU_8bit_ColorDepth_16bpp	
	Draw_Triangle(0x001f,150,0,150,100,250,100);
	Draw_Triangle_Fill(0x001f,300,0,300,100,400,100);
	#endif
	#ifdef MCU_8bit_ColorDepth_24bpp	
	Draw_Triangle(0x0000ff,150,0,150,100,250,100);
	Draw_Triangle_Fill(0x0000ff,300,0,300,100,400,100);
	#endif
	#ifdef MCU_16bit_ColorDepth_16bpp	
	Draw_Triangle(0x001f,150,0,150,100,250,100);
	Draw_Triangle_Fill(0x001f,300,0,300,100,400,100);
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
	Draw_Triangle(0x0000ff,150,0,150,100,250,100);
	Draw_Triangle_Fill(0x0000ff,300,0,300,100,400,100);
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
	Draw_Triangle(0x0000ff,150,0,150,100,250,100);
	Draw_Triangle_Fill(0x0000ff,300,0,300,100,400,100);
	#endif
}

void API_Draw_Circle_Demo(void)
{
    Active_Window_XY(0,0);
	Active_Window_WH(1366,768);
	#ifdef MCU_8bit_ColorDepth_8bpp	
	Draw_Circle(0xfc,500,50,50);
	Draw_Circle_Fill(0xfc,650,50,50);
	#endif
	#ifdef MCU_8bit_ColorDepth_16bpp	
	Draw_Circle(0xffe0,500,50,50);
	Draw_Circle_Fill(0xffe0,650,50,50);
	#endif
	#ifdef MCU_8bit_ColorDepth_24bpp	
	Draw_Circle(0xffff00,500,50,50);
	Draw_Circle_Fill(0xffff00,650,50,50);
	#endif
	#ifdef MCU_16bit_ColorDepth_16bpp	
	Draw_Circle(0xffe0,500,50,50);
	Draw_Circle_Fill(0xffe0,650,50,50);
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
	Draw_Circle(0xffff00,500,50,50);
	Draw_Circle_Fill(0xffff00,650,50,50);
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
	Draw_Circle(0xffff00,500,50,50);
	Draw_Circle_Fill(0xffff00,650,50,50);
	#endif
}


void API_Draw_Ellipse_Demo(void)
{	

	#ifdef MCU_8bit_ColorDepth_8bpp	
	Draw_Ellipse(0x1f,100,200,100,50);
	Draw_Ellipse_Fill(0x1f,350,200,100,50);
	#endif
	#ifdef MCU_8bit_ColorDepth_16bpp	
	Draw_Ellipse(0x07ff,100,200,100,50);	   //setting in UserDef.h
	Draw_Ellipse_Fill(0x07ff,350,200,100,50);
	#endif
	#ifdef MCU_8bit_ColorDepth_24bpp	
	Draw_Ellipse(0x00ffff,100,200,100,50);	   //setting in UserDef.h
	Draw_Ellipse_Fill(0x00ffff,350,200,100,50);
	#endif
	#ifdef MCU_16bit_ColorDepth_16bpp	
	Draw_Ellipse(0x07ff,100,200,100,50);	   //setting in UserDef.h
	Draw_Ellipse_Fill(0x07ff,350,200,100,50);
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
	Draw_Ellipse(0x00ffff,100,200,100,50);	   //setting in UserDef.h
	Draw_Ellipse_Fill(0x00ffff,350,200,100,50);
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
	Draw_Ellipse(0x00ffff,100,200,100,50);	   //setting in UserDef.h
	Draw_Ellipse_Fill(0x00ffff,350,200,100,50);
	#endif
}


void API_Draw_Curve_Demo(void)
{
    Active_Window_XY(0,0);
	Active_Window_WH(1366,768); 		

	#ifdef MCU_8bit_ColorDepth_8bpp	
	Draw_Left_Up_Curve(0xe3,550,190,50,50);
	Draw_Right_Down_Curve(0xff,560,200,50,50);
	Draw_Right_Up_Curve(0xff,560,190,50,50);
	Draw_Left_Down_Curve(0x1c,550,200,50,50);

	Draw_Left_Up_Curve_Fill(0xe3,700,190,50,50);
	Draw_Right_Down_Curve_Fill(0xff,710,200,50,50);
	Draw_Right_Up_Curve_Fill(0xff,710,190,50,50);
	Draw_Left_Down_Curve_Fill(0x1c,700,200,50,50);
	#endif
	#ifdef MCU_8bit_ColorDepth_16bpp	
	Draw_Left_Up_Curve(0xf11f,550,190,50,50);
	Draw_Right_Down_Curve(0xffff,560,200,50,50);
	Draw_Right_Up_Curve(0xffff,560,190,50,50);
	Draw_Left_Down_Curve(0x07e0,550,200,50,50);

	Draw_Left_Up_Curve_Fill(0xf11f,700,190,50,50);
	Draw_Right_Down_Curve_Fill(0xffff,710,200,50,50);
	Draw_Right_Up_Curve_Fill(0xffff,710,190,50,50);
	Draw_Left_Down_Curve_Fill(0x07e0,700,200,50,50);
	#endif
	#ifdef MCU_8bit_ColorDepth_24bpp	
	Draw_Left_Up_Curve(0xff00ff,550,190,50,50);
	Draw_Right_Down_Curve(0xffffff,560,200,50,50);
	Draw_Right_Up_Curve(0xffffff,560,190,50,50);
	Draw_Left_Down_Curve(0x00ff00,550,200,50,50);

	Draw_Left_Up_Curve_Fill(0xff00ff,700,190,50,50);
	Draw_Right_Down_Curve_Fill(0xffffff,710,200,50,50);
	Draw_Right_Up_Curve_Fill(0xffffff,710,190,50,50);
	Draw_Left_Down_Curve_Fill(0x00ff00,700,200,50,50);
	#endif
	#ifdef MCU_16bit_ColorDepth_16bpp	
	Draw_Left_Up_Curve(0xf11f,550,190,50,50);
	Draw_Right_Down_Curve(0xffff,560,200,50,50);
	Draw_Right_Up_Curve(0xffff,560,190,50,50);
	Draw_Left_Down_Curve(0x07e0,550,200,50,50);

	Draw_Left_Up_Curve_Fill(0xf11f,700,190,50,50);
	Draw_Right_Down_Curve_Fill(0xffff,710,200,50,50);
	Draw_Right_Up_Curve_Fill(0xffff,710,190,50,50);
	Draw_Left_Down_Curve_Fill(0x07e0,700,200,50,50);
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
	Draw_Left_Up_Curve(0xff00ff,550,190,50,50);
	Draw_Right_Down_Curve(0x000000,560,200,50,50);
	Draw_Right_Up_Curve(0x000000,560,190,50,50);
	Draw_Left_Down_Curve(0x00ff00,550,200,50,50);

	Draw_Left_Up_Curve_Fill(0xff00ff,700,190,50,50);
	Draw_Right_Down_Curve_Fill(0x000000,710,200,50,50);
	Draw_Right_Up_Curve_Fill(0x000000,710,190,50,50);
	Draw_Left_Down_Curve_Fill(0x00ff00,700,200,50,50);
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
	Draw_Left_Up_Curve(0xff00ff,550,190,50,50);
	Draw_Right_Down_Curve(0xffffff,560,200,50,50);
	Draw_Right_Up_Curve(0xffffff,560,190,50,50);
	Draw_Left_Down_Curve(0x00ff00,550,200,50,50);

	Draw_Left_Up_Curve_Fill(0xff00ff,700,190,50,50);
	Draw_Right_Down_Curve_Fill(0xffffff,710,200,50,50);
	Draw_Right_Up_Curve_Fill(0xffffff,710,190,50,50);
	Draw_Left_Down_Curve_Fill(0x00ff00,700,200,50,50);
	#endif
}


void API_Draw_Square_Demo(void)
{
    Active_Window_XY(0,0);
	Active_Window_WH(1366,768); 
	#ifdef MCU_8bit_ColorDepth_8bpp	
	Draw_Square(0xe0,50,300,150,400);
	Draw_Square_Fill(0xe0,200,300,300,400);
	#endif
	#ifdef MCU_8bit_ColorDepth_16bpp	
	Draw_Square(0xf800,50,300,150,400);					    
	Draw_Square_Fill(0xf800,200,300,300,400);
	#endif
	#ifdef MCU_8bit_ColorDepth_24bpp	
	Draw_Square(0xff0000,50,300,150,400);
	Draw_Square_Fill(0xff0000,200,300,300,400);
	#endif
	#ifdef MCU_16bit_ColorDepth_16bpp	
	Draw_Square(0xf800,50,300,150,400);					    
	Draw_Square_Fill(0xf800,200,300,300,400);
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
	Draw_Square(0xff0000,50,300,150,400);
	Draw_Square_Fill(0xff0000,200,300,300,400);
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
	Draw_Square(0xff0000,50,300,150,400);
	Draw_Square_Fill(0xff0000,200,300,300,400);
	#endif
}

void API_Draw_Circle_Square_Demo(void)
{
    Active_Window_XY(0,0);
	Active_Window_WH(1366,768); 


	#ifdef MCU_8bit_ColorDepth_8bpp	
	Draw_Circle_Square(0xe0,450,300,550,400,20,30);
	Draw_Circle_Square_Fill(0xe0,600,300,700,400,20,30);
	#endif
	#ifdef MCU_8bit_ColorDepth_16bpp	
	Draw_Circle_Square(0xf800,450,300,550,400,20,30);
	Draw_Circle_Square_Fill(0xf800,600,300,700,400,20,30);
	#endif
	#ifdef MCU_8bit_ColorDepth_24bpp	
	Draw_Circle_Square(0xff0000,450,300,550,400,20,30);
	Draw_Circle_Square_Fill(0xff0000,600,300,700,400,20,30);
	#endif
	#ifdef MCU_16bit_ColorDepth_16bpp	
	Draw_Circle_Square(0xf800,450,300,550,400,20,30);
	Draw_Circle_Square_Fill(0xf800,600,300,700,400,20,30);
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
	Draw_Circle_Square(0xff0000,450,300,550,400,20,30);
	Draw_Circle_Square_Fill(0xff0000,600,300,700,400,20,30);
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
	Draw_Circle_Square(0xff0000,450,300,550,400,20,30);
	Draw_Circle_Square_Fill(0xff0000,600,300,700,400,20,30);
	#endif
}

void API_PIP_Demo(void)
{


	SPI_NOR_initial_DMA (3,0,1,1,0);

	#ifdef MCU_8bit_ColorDepth_8bpp					   //setting in UserDef.h
	DMA_24bit(2,800,0,800,480,800,15443088);
	#endif
	#ifdef MCU_8bit_ColorDepth_16bpp				   //setting in UserDef.h
	DMA_24bit(2,800,0,800,480,800,14675088);
	#endif
	#ifdef MCU_8bit_ColorDepth_24bpp				   //setting in UserDef.h
	DMA_24bit(2,800,0,800,480,800,0);
	#endif
	#ifdef MCU_16bit_ColorDepth_16bpp				   //setting in UserDef.h
	DMA_24bit(2,800,0,800,480,800,14675088);
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1		   //setting in UserDef.h
	DMA_24bit(2,800,0,800,480,800,0);
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2		   //setting in UserDef.h
	DMA_24bit(2,800,0,800,480,800,0);
	#endif


	PIP(1,2,0,800,0,canvas_image_width,200,200,200,200);




}

//Serial Flash size <= 128Mbits
void API_DMA_24bit_Demo(void)				  
{
	SPI_NOR_initial_DMA (3,0,1,1,0);
	#ifdef MCU_8bit_ColorDepth_8bpp					   //setting in UserDef.h
	DMA_24bit(2,0,0,800,480,800,15443088);
	#endif
	#ifdef MCU_8bit_ColorDepth_16bpp				   //setting in UserDef.h
	DMA_24bit(2,0,0,800,480,800,14675088);
	#endif
	#ifdef MCU_8bit_ColorDepth_24bpp				   //setting in UserDef.h
	DMA_24bit(2,0,0,800,480,800,0);
	#endif
	#ifdef MCU_16bit_ColorDepth_16bpp				   //setting in UserDef.h
	DMA_24bit(2,0,0,800,480,800,14675088);
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1		   //setting in UserDef.h
	DMA_24bit(2,0,0,800,480,800,0);
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2		   //setting in UserDef.h
	DMA_24bit(2,0,0,800,480,800,0);
	#endif
}

//Serial Flash size > 128Mbits
void API_DMA_32bit_Demo(void)				  
{
	SPI_NOR_initial_DMA (3,0,1,1,1);
	#ifdef MCU_8bit_ColorDepth_8bpp					   //setting in UserDef.h
	DMA_32bit(2,0,0,800,480,800,15443088);
	#endif
	#ifdef MCU_8bit_ColorDepth_16bpp				   //setting in UserDef.h
	DMA_32bit(2,0,0,800,480,800,14675088);
	#endif
	#ifdef MCU_8bit_ColorDepth_24bpp				   //setting in UserDef.h
	DMA_32bit(2,0,0,800,480,800,0);
	#endif
	#ifdef MCU_16bit_ColorDepth_16bpp				   //setting in UserDef.h
	DMA_32bit(2,0,0,800,480,800,14675088);
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1		   //setting in UserDef.h
	DMA_32bit(2,0,0,800,480,800,0);
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2		   //setting in UserDef.h
	DMA_32bit(2,0,0,800,480,800,0);
	#endif

}
void API_BTE_Solid_Fill_Demo(void)
{

	#ifdef MCU_8bit_ColorDepth_8bpp	
	BTE_Solid_Fill(0,canvas_image_width,0,0,0xe0,200,200);
	#endif
	#ifdef MCU_8bit_ColorDepth_16bpp	
	BTE_Solid_Fill(0,canvas_image_width,0,0,0xf800,200,200);
	#endif
	#ifdef MCU_8bit_ColorDepth_24bpp	
	BTE_Solid_Fill(0,canvas_image_width,0,0,0xFF0000,200,200);
	#endif
	#ifdef MCU_16bit_ColorDepth_16bpp	
	BTE_Solid_Fill(0,canvas_image_width,0,0,0xf800,200,200);
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
	BTE_Solid_Fill(0,canvas_image_width,0,0,0xFF0000,200,200);
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
	BTE_Solid_Fill(0,canvas_image_width,0,0,0xFF0000,200,200);
	#endif
}

void API_BTE_Pattern_Fill_Demo(void)
{	

	SPI_NOR_initial_DMA (3,0,1,1,0);
	#ifdef MCU_8bit_ColorDepth_8bpp	
	DMA_24bit(2,0,0,800,480,800,15443088);
	MPU8_8bpp_Memory_Write(0,0,16,16,Icon_8bit_8bpp);
	#endif
	#ifdef MCU_8bit_ColorDepth_16bpp	
	DMA_24bit(2,0,0,800,480,800,14675088);
	MPU8_16bpp_Memory_Write(0,0,16,16,Icon_8bit_16bpp);
	#endif
	#ifdef MCU_8bit_ColorDepth_24bpp	
	DMA_24bit(2,0,0,800,480,800,0);
	MPU8_24bpp_Memory_Write(0,0,16,16,Icon_8bit_24bpp);
	#endif
	#ifdef MCU_16bit_ColorDepth_16bpp	
	DMA_24bit(2,0,0,800,480,800,14675088);
	MPU16_16bpp_Memory_Write(0,0,16,16,Icon_16bit_16bpp);
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
	DMA_24bit(2,0,0,800,480,800,0);
	MPU16_24bpp_Mode1_Memory_Write(0,0,16,16,Icon_16bit_24bpp_mode1);
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
	DMA_24bit(2,0,0,800,480,800,0);
	MPU16_24bpp_Mode2_Memory_Write(0,0,16,16,Icon_16bit_24bpp_mode2);
	#endif

 	BTE_Pattern_Fill(1,0,canvas_image_width,0,0,0,canvas_image_width,0,0,0,canvas_image_width,500,200,12,100,100);
}

void API_BTE_Pattern_Fill_with_Chroma_key_Demo(void)
{
	SPI_NOR_initial_DMA (3,0,1,1,0);
	#ifdef MCU_8bit_ColorDepth_8bpp	
	DMA_24bit(2,0,0,800,480,800,15443088);
	MPU8_8bpp_Memory_Write(0,0,16,16,Icon_8bit_8bpp);
	BTE_Pattern_Fill_With_Chroma_key(1,0,canvas_image_width,0,0,0,canvas_image_width,0,0,0,canvas_image_width,500,200,12,0x1f,100,100);
	#endif
	#ifdef MCU_8bit_ColorDepth_16bpp	
	DMA_24bit(2,0,0,800,480,800,14675088);
	MPU8_16bpp_Memory_Write(0,0,16,16,Icon_8bit_16bpp);
	BTE_Pattern_Fill_With_Chroma_key(1,0,canvas_image_width,0,0,0,canvas_image_width,0,0,0,canvas_image_width,500,200,12,0x07ff,100,100);
	#endif
	#ifdef MCU_8bit_ColorDepth_24bpp	
	DMA_24bit(2,0,0,800,480,800,0);
	MPU8_24bpp_Memory_Write(0,0,16,16,Icon_8bit_24bpp);
	BTE_Pattern_Fill_With_Chroma_key(1,0,canvas_image_width,0,0,0,canvas_image_width,0,0,0,canvas_image_width,500,200,12,0x00ffff,100,100);
	#endif
	#ifdef MCU_16bit_ColorDepth_16bpp	
	DMA_24bit(2,0,0,800,480,800,14675088);
	MPU16_16bpp_Memory_Write(0,0,16,16,Icon_16bit_16bpp);
	BTE_Pattern_Fill_With_Chroma_key(1,0,canvas_image_width,0,0,0,canvas_image_width,0,0,0,canvas_image_width,500,200,12,0x07ff,100,100);
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
	DMA_24bit(2,0,0,800,480,800,0);
	MPU16_24bpp_Mode1_Memory_Write(0,0,16,16,Icon_16bit_24bpp_mode1);
	BTE_Pattern_Fill_With_Chroma_key(1,0,canvas_image_width,0,0,0,canvas_image_width,0,0,0,canvas_image_width,500,200,12,0x00ffff,100,100);
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
	DMA_24bit(2,0,0,800,480,800,0);
	MPU16_24bpp_Mode2_Memory_Write(0,0,16,16,Icon_16bit_24bpp_mode2);
	BTE_Pattern_Fill_With_Chroma_key(1,0,canvas_image_width,0,0,0,canvas_image_width,0,0,0,canvas_image_width,500,200,12,0x00ffff,100,100);
	#endif
}

void API_BTE_Memory_Copy_Demo(void)
{
	#ifdef MCU_8bit_ColorDepth_8bpp	
	BTE_Solid_Fill(0,canvas_image_width,0,0,0xe0,200,200);
	Draw_Circle_Fill(0xffff00,100,100,50);
	#endif
	#ifdef MCU_8bit_ColorDepth_16bpp	
	BTE_Solid_Fill(0,canvas_image_width,0,0,0xf800,200,200);
	Draw_Circle_Fill(0xffff00,100,100,50);
	#endif
	#ifdef MCU_8bit_ColorDepth_24bpp	
	BTE_Solid_Fill(0,canvas_image_width,0,0,0xFF0000,200,200);
	Draw_Circle_Fill(0xffff00,100,100,50);
	#endif
	#ifdef MCU_16bit_ColorDepth_16bpp	
	BTE_Solid_Fill(0,canvas_image_width,0,0,0xf800,200,200);
	Draw_Circle_Fill(0xffff00,100,100,50);
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
	BTE_Solid_Fill(0,canvas_image_width,0,0,0xFF0000,200,200);
	Draw_Circle_Fill(0xffff00,100,100,50);
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
	BTE_Solid_Fill(0,canvas_image_width,0,0,0xFF0000,200,200);
	Draw_Circle_Fill(0xffff00,100,100,50);
	#endif
	BTE_Memory_Copy(0,canvas_image_width,0,0,0,canvas_image_width,0,0,0,canvas_image_width,500,200,12,200,200);
}

void API_BTE_Memory_Copy_Chroma_key_Demo(void)
{
	#ifdef MCU_8bit_ColorDepth_8bpp	
	BTE_Solid_Fill(0,canvas_image_width,0,0,0xe0,200,200);
	Draw_Circle_Fill(0xffff00,100,100,50);
	BTE_Memory_Copy_Chroma_key(0,canvas_image_width,0,0,0,canvas_image_width,500,200,0xe0,200,200);	
	#endif
	#ifdef MCU_8bit_ColorDepth_16bpp	
	BTE_Solid_Fill(0,canvas_image_width,0,0,0xf800,200,200);
	Draw_Circle_Fill(0xffff00,100,100,50);
	BTE_Memory_Copy_Chroma_key(0,canvas_image_width,0,0,0,canvas_image_width,500,200,0xf800,200,200);	
	#endif
	#ifdef MCU_8bit_ColorDepth_24bpp	
	BTE_Solid_Fill(0,canvas_image_width,0,0,0xFF0000,200,200);
	Draw_Circle_Fill(0xffff00,100,100,50);
	BTE_Memory_Copy_Chroma_key(0,canvas_image_width,0,0,0,canvas_image_width,500,200,0xff0000,200,200);	
	#endif
	#ifdef MCU_16bit_ColorDepth_16bpp	
	BTE_Solid_Fill(0,canvas_image_width,0,0,0xf800,200,200);
	Draw_Circle_Fill(0xffff00,100,100,50);
	BTE_Memory_Copy_Chroma_key(0,canvas_image_width,0,0,0,canvas_image_width,500,200,0xf800,200,200);	
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
	BTE_Solid_Fill(0,canvas_image_width,0,0,0xFF0000,200,200);
	Draw_Circle_Fill(0xffff00,100,100,50);
	BTE_Memory_Copy_Chroma_key(0,canvas_image_width,0,0,0,canvas_image_width,500,200,0xff0000,200,200);	
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
	BTE_Solid_Fill(0,canvas_image_width,0,0,0xFF0000,200,200);
	Draw_Circle_Fill(0xffff00,100,100,50);
	BTE_Memory_Copy_Chroma_key(0,canvas_image_width,0,0,0,canvas_image_width,500,200,0xff0000,200,200);	
	#endif


		
}

void API_BTE_MCU_Write_MCU_8bit_Demo(void)
{

	#ifdef MCU_8bit_ColorDepth_8bpp				   //setting in UserDef.h
	BTE_MCU_Write_MCU_8bit(0,canvas_image_width,0,0,0,canvas_image_width,100,100,12,128,128,gImage_8);
	#endif
	#ifdef MCU_8bit_ColorDepth_16bpp			  //setting in UserDef.h
	BTE_MCU_Write_MCU_8bit(0,canvas_image_width,0,0,0,canvas_image_width,100,100,12,128,128,gImage_16);
	#endif
	#ifdef MCU_8bit_ColorDepth_24bpp			   //setting in UserDef.h
	BTE_MCU_Write_MCU_8bit(0,canvas_image_width,0,0,0,canvas_image_width,100,100,12,128,128,gImage_24);
	#endif

}

void API_BTE_MCU_Write_MCU_16bit_Demo(void)
{

	#ifdef MCU_16bit_ColorDepth_16bpp	
	BTE_MCU_Write_MCU_16bit(0,canvas_image_width,0,0,0,canvas_image_width,100,100,12,128,128,pic1616);
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
	BTE_MCU_Write_MCU_16bit(0,canvas_image_width,0,0,0,canvas_image_width,100,100,12,128,128,pic16241);
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
	BTE_MCU_Write_MCU_16bit(0,canvas_image_width,0,0,0,canvas_image_width,100,100,12,128,128,pic1624);
	#endif
}

void API_BTE_MCU_Write_Chroma_key_MCU_8bit_Demo(void)
{

	#ifdef MCU_8bit_ColorDepth_8bpp				   //setting in UserDef.h
	BTE_MCU_Write_Chroma_key_MCU_8bit(0,canvas_image_width,100,100,0xe0,128,128,gImage_8);
	#endif
	#ifdef MCU_8bit_ColorDepth_16bpp			  //setting in UserDef.h
	BTE_MCU_Write_Chroma_key_MCU_8bit(0,canvas_image_width,100,100,0xf800,128,128,gImage_16);
	#endif
	#ifdef MCU_8bit_ColorDepth_24bpp			   //setting in UserDef.h
	BTE_MCU_Write_Chroma_key_MCU_8bit(0,canvas_image_width,100,100,0xff0000,128,128,gImage_24);
	#endif

}
void API_BTE_MCU_Write_Chroma_key_MCU_16bit_Demo(void)
{
	#ifdef MCU_16bit_ColorDepth_16bpp				//setting in UserDef.h
	BTE_MCU_Write_Chroma_key_MCU_16bit(0,canvas_image_width,100,100,0xf800,128,128,pic1616);
	#endif	
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1		//setting in UserDef.h
	BTE_MCU_Write_Chroma_key_MCU_16bit(0,canvas_image_width,100,100,0xff0000,128,128,pic16241);
	#endif	
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2		//setting in UserDef.h
	BTE_MCU_Write_Chroma_key_MCU_16bit(0,canvas_image_width,100,100,0xff0000,128,128,pic1624);
	#endif

}

void API_BTE_MCU_Write_ColorExpansion_MCU_8bit_Demo(void)
{
	#ifdef MCU_8bit_ColorDepth_8bpp						//setting in UserDef.h
	BTE_MCU_Write_ColorExpansion_MCU_8bit(0,canvas_image_width,0,0,128,128,0x03,0x1c,gImage_1);
	#endif	
	#ifdef MCU_8bit_ColorDepth_16bpp					//setting in UserDef.h
	BTE_MCU_Write_ColorExpansion_MCU_8bit(0,canvas_image_width,0,0,128,128,0x001f,0x07e0,gImage_1);
	#endif	
	#ifdef MCU_8bit_ColorDepth_24bpp					//setting in UserDef.h
	BTE_MCU_Write_ColorExpansion_MCU_8bit(0,canvas_image_width,0,0,128,128,0x0000ff,0x00ff00,gImage_1);
	#endif
}

void API_BTE_MCU_Write_ColorExpansion_Chroma_key_MCU_8bit_Demo(void)
{
	#ifdef MCU_8bit_ColorDepth_8bpp						 //setting in UserDef.h
	BTE_MCU_Write_ColorExpansion_Chroma_key_MCU_8bit(0,canvas_image_width,0,0,128,128,0xe0,gImage_1);
	#endif	
	#ifdef MCU_8bit_ColorDepth_16bpp						 //setting in UserDef.h
	BTE_MCU_Write_ColorExpansion_Chroma_key_MCU_8bit(0,canvas_image_width,0,0,128,128,0xf800,gImage_1);
	#endif	
	#ifdef MCU_8bit_ColorDepth_24bpp						 //setting in UserDef.h
	BTE_MCU_Write_ColorExpansion_Chroma_key_MCU_8bit(0,canvas_image_width,0,0,128,128,0xff0000,gImage_1);
	#endif
}

void API_BTE_MCU_Write_ColorExpansion_MCU_16bit_Demo(void)
{
	#ifdef MCU_16bit_ColorDepth_16bpp						//setting in UserDef.h
	BTE_MCU_Write_ColorExpansion_MCU_16bit(0,canvas_image_width,0,0,128,128,0x001f,0x07e0,Test);
	#endif	
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1				//setting in UserDef.h
	BTE_MCU_Write_ColorExpansion_MCU_16bit(0,canvas_image_width,0,0,128,128,0x0000ff,0x00ff00,Test);
	#endif	
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2				//setting in UserDef.h
	BTE_MCU_Write_ColorExpansion_MCU_16bit(0,canvas_image_width,0,0,128,128,0x0000ff,0x00ff00,Test);
	#endif
}

void API_BTE_MCU_Write_ColorExpansion_Chroma_key_MCU_16bit_Demo(void)
{
	#ifdef MCU_16bit_ColorDepth_16bpp						//setting in UserDef.h
	BTE_MCU_Write_ColorExpansion_Chroma_key_MCU_16bit(0,canvas_image_width,0,0,128,128,0xf800,Test);
	#endif	
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1				//setting in UserDef.h
	BTE_MCU_Write_ColorExpansion_Chroma_key_MCU_16bit(0,canvas_image_width,0,0,128,128,0xff0000,Test);
	#endif	
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2				//setting in UserDef.h
	BTE_MCU_Write_ColorExpansion_Chroma_key_MCU_16bit(0,canvas_image_width,0,0,128,128,0xff0000,Test);
	#endif
}

void API_BTE_Alpha_Blending_Picture_mode_Demo(void)
{
	SPI_NOR_initial_DMA (3,0,1,1,0);
  #ifdef MCU_8bit_ColorDepth_8bpp					   //setting in UserDef.h
	DMA_24bit(2,0,0,800,480,800,15443088);
	BTE_Solid_Fill(0,canvas_image_width,800,0,0x1c,200,200);
	#endif
	#ifdef MCU_8bit_ColorDepth_16bpp				   //setting in UserDef.h
	DMA_24bit(2,0,0,800,480,800,14675088);
	BTE_Solid_Fill(0,canvas_image_width,800,0,0x07e0,200,200);
	#endif
	#ifdef MCU_8bit_ColorDepth_24bpp				   //setting in UserDef.h
	DMA_24bit(2,0,0,800,480,800,0);
	BTE_Solid_Fill(0,canvas_image_width,800,0,0x00FF00,200,200);
	#endif
	#ifdef MCU_16bit_ColorDepth_16bpp				   //setting in UserDef.h
	DMA_24bit(2,0,0,800,480,800,14675088);
	BTE_Solid_Fill(0,canvas_image_width,800,0,0x07e0,200,200);
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1		   //setting in UserDef.h
	DMA_24bit(2,0,0,800,480,800,0);
	BTE_Solid_Fill(0,canvas_image_width,800,0,0x00FF00,200,200);
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2		   //setting in UserDef.h
	DMA_24bit(2,0,0,800,480,800,0);
	BTE_Solid_Fill(0,canvas_image_width,800,0,0x00FF00,200,200);
	#endif


//	BTE_Alpha_Blending(0,canvas_image_width,500,300,0,canvas_image_width,800,0,0,canvas_image_width,500,300,200,200,0);
//	BTE_Alpha_Blending(0,canvas_image_width,500,300,0,canvas_image_width,800,0,0,canvas_image_width,500,300,200,200,8);
	BTE_Alpha_Blending_Picture_Mode(0,canvas_image_width,500,300,0,canvas_image_width,800,0,0,canvas_image_width,500,300,200,200,16);
//	BTE_Alpha_Blending(0,canvas_image_width,500,300,0,canvas_image_width,800,0,0,canvas_image_width,500,300,200,200,24);
//	BTE_Alpha_Blending(0,canvas_image_width,500,300,0,canvas_image_width,800,0,0,canvas_image_width,500,300,200,200,32);
}



void API_Print_Internal_Font_String_Demo(void) //Internal font only for 12x24 size
{
	
	Select_Font_Height_WxN_HxN_ChromaKey_Alignment(24,4,4,0,0);
	#ifdef MCU_8bit_ColorDepth_8bpp	
	Print_Internal_Font_String(0,0,800,480,0xe0,0x1c, "adfsdfdgfhhgfh");
	#endif
	#ifdef MCU_8bit_ColorDepth_16bpp	
	Print_Internal_Font_String(0,0,800,480,0xf800,0x07e0, "adfsdfdgfhhgfh");
	#endif
	#ifdef MCU_8bit_ColorDepth_24bpp	
	Print_Internal_Font_String(0,0,800,480,0xff0000,0x00ff00, "adfsdfdgfhhgfh");
	#endif
	#ifdef MCU_16bit_ColorDepth_16bpp	
	Print_Internal_Font_String(0,0,800,480,0xf800,0x07e0, "adfsdfdgfhhgfh");
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
	Print_Internal_Font_String(0,0,800,480,0xff0000,0x00ff00, "adfsdfdgfhhgfh");
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
	Print_Internal_Font_String(0,0,800,480,0xff0000,0x00ff00, "adfsdfdgfhhgfh");
	#endif
}

void API_Print_BIG5String_Demo(void)
{
	Select_Font_Height_WxN_HxN_ChromaKey_Alignment(24,4,4,0,0);
	#ifdef MCU_8bit_ColorDepth_8bpp	
	Print_BIG5String(3,0,1,0,0,800,480,0xe0,0x1c,"风м123456");
	#endif
	#ifdef MCU_8bit_ColorDepth_16bpp	
	Print_BIG5String(3,0,1,0,0,800,480,0xf800,0x07e0,"风м123456");
	#endif
	#ifdef MCU_8bit_ColorDepth_24bpp	
	Print_BIG5String(3,0,1,0,0,800,480,0xff0000,0x00ff00,"风м123456");
	#endif
	#ifdef MCU_16bit_ColorDepth_16bpp	
	Print_BIG5String(3,0,1,0,0,800,480,0xf800,0x07e0,"风м123456");
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
	Print_BIG5String(3,0,1,0,0,800,480,0xff0000,0x00ff00,"风м123456");
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
	Print_BIG5String(3,0,1,0,0,800,480,0xff0000,0x00ff00,"风м123456");
	#endif
}
void API_Print_GB2312String_Demo(void)
{
	Select_Font_Height_WxN_HxN_ChromaKey_Alignment(24,4,4,0,1);
	#ifdef MCU_8bit_ColorDepth_8bpp	
	Print_GB2312String(3,0,1,0,0,800,480,0xe0,0x03,"瑞佑科技123456");
	#endif
	#ifdef MCU_8bit_ColorDepth_16bpp	
	Print_GB2312String(3,0,1,0,0,800,480,0xf800,0x001f,"瑞佑科技123456");
	#endif
	#ifdef MCU_8bit_ColorDepth_24bpp	
	Print_GB2312String(3,0,1,0,0,800,480,0xff0000,0x0000ff,"瑞佑科技123456");
	#endif
	#ifdef MCU_16bit_ColorDepth_16bpp	
	Print_GB2312String(3,0,1,0,0,800,480,0xf800,0x001f,"瑞佑科技123456");
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
	Print_GB2312String(3,0,1,0,0,800,480,0xff0000,0x0000ff,"瑞佑科技123456");
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
	Print_GB2312String(3,0,1,0,0,800,480,0xff0000,0x0000ff,"瑞佑科技123456");
	#endif
}

void API_Print_GB12345String_Demo(void)
{
	Select_Font_Height_WxN_HxN_ChromaKey_Alignment(24,4,4,0,1);
	#ifdef MCU_8bit_ColorDepth_8bpp	
	Print_GB12345String(3,0,1,0,0,800,480,0xe0,0x03,"瑞佑科技123456");
	#endif
	#ifdef MCU_8bit_ColorDepth_16bpp	
	Print_GB12345String(3,0,1,0,0,800,480,0xf800,0x001f,"瑞佑科技123456");
	#endif
	#ifdef MCU_8bit_ColorDepth_24bpp	
	Print_GB12345String(3,0,1,0,0,800,480,0xff0000,0x0000ff,"瑞佑科技123456");
	#endif
	#ifdef MCU_16bit_ColorDepth_16bpp	
	Print_GB12345String(3,0,1,0,0,800,480,0xf800,0x001f,"瑞佑科技123456");
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
	Print_GB12345String(3,0,1,0,0,800,480,0xff0000,0x0000ff,"瑞佑科技123456");
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
	Print_GB12345String(3,0,1,0,0,800,480,0xff0000,0x0000ff,"瑞佑科技123456");
	#endif
}

void API_Print_UnicodeString_Demo(void)
{
	Select_Font_Height_WxN_HxN_ChromaKey_Alignment(24,4,4,0,1);
	#ifdef MCU_8bit_ColorDepth_8bpp	
	Print_UnicodeString(3,0,1,0,0,800,480,0xe0,0x1c,L"穝λ郡λカじ刁8腹6加ぇ5");
	#endif
	#ifdef MCU_8bit_ColorDepth_16bpp	
	Print_UnicodeString(3,0,1,0,0,800,480,0xf800,0x07e0,L"穝λ郡λカじ刁8腹6加ぇ5");
	#endif
	#ifdef MCU_8bit_ColorDepth_24bpp	
	Print_UnicodeString(3,0,1,0,0,800,480,0xff0000,0x00ff00,L"穝λ郡λカじ刁8腹6加ぇ5");
	#endif
	#ifdef MCU_16bit_ColorDepth_16bpp	
	Print_UnicodeString(3,0,1,0,0,800,480,0xf800,0x07e0,L"穝λ郡λカじ刁8腹6加ぇ5");
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
	Print_UnicodeString(3,0,1,0,0,800,480,0xff0000,0x00ff00,L"穝λ郡λカじ刁8腹6加ぇ5");
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
	Print_UnicodeString(3,0,1,0,0,800,480,0xff0000,0x00ff00,L"穝λ郡λカじ刁8腹6加ぇ5");
	#endif
}

void PWM_Demo (void)
{
PWM0(1,3,100,1,0);   	  
PWM1(1,3,100,4,3);   
}

void RAIO_define_font(void)
{
Active_Window_XY(0,0);
Active_Window_WH(1366,768); 					
#ifdef MCU_8bit_ColorDepth_8bpp	
lcdPutString8x12(0,0,0xe0,0x00,0,"sdfs6+55");
lcdPutString16x24(0,100,0x1c,0x00, 0,"sijsojfe565");
lcdPutString32x48(0,200,0x03,0x00,1,"sdjlfw5464ewr");
#endif

#ifdef MCU_8bit_ColorDepth_16bpp	
lcdPutString8x12(0,0,0xf800,0x0000,0,"sdfs6+55");
lcdPutString16x24(0,100,0x07e0,0x0000, 0,"sijsojfe565");
lcdPutString32x48(0,200,0x001f,0x0000,1,"sdjlfw5464ewr");
#endif

#ifdef MCU_16bit_ColorDepth_16bpp	
lcdPutString8x12(0,0,0xf800,0x0000,0,"sdfs6+55");
lcdPutString16x24(0,100,0x07e0,0x0000, 0,"sijsojfe565");
lcdPutString32x48(0,200,0x001f,0x0000,1,"sdjlfw5464ewr");
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
lcdPutString8x12(0,0,0xff0000,0x000000,0,"sdfs6+55");
lcdPutString16x24(0,100,0x00ff00,0x000000, 0,"sijsojfe565");
lcdPutString32x48(0,200,0x0000ff,0x000000,1,"sdjlfw5464ewr");
#endif

}

void Print_key_code(void)
{ 
 unsigned char temp,temp1,temp2;   

 
  TFT_16bit();							//RA8889 need set TFT output 16bit to use key function,

///******************set internal font parameter
  CGROM_Select_Internal_CGROM();
  Internal_CGROM_Select_ISOIEC8859_1();
  Font_Select_8x16_16x16();
  Font_Background_select_Color();
  Background_RGB(0xFF,0xFF,0xFF);
  Background_RGB(0x00,0x00,0x00);
  Font_Width_X1();
  Font_Height_X1();
////**************************

  Text_Mode();
  LCD_CmdWrite(0x0F);		 //PDAT for PIO/Key Function Select Register,0xff = set XPDAT[2:0],XPDAT[9:8],XPDAT[18:16] PIN = KOUT[4:1],KIN[4:1]
  LCD_DataWrite(0xff);
 // Long_Key_enable();	 
  Key_Scan_Enable();
  //Disable_LongKey();								   
  Enable_KeyScan_Interrupt();
 // Disable_KeyScan_Interrupt();
  LCD_CmdWrite(0xfb);					//bit[6] = 1 :enable long key. bit[5:4]:set Short Key de-bounce Times. bit[3] = 0 :Repeatable Key disable. bit[2:0] : Row Scan Time setting.
  LCD_DataWrite(0x40);
  LCD_CmdWrite(0xfc);					//bit[7] = 1 : Key-Scan Wakeup Function Enable. bit[6] = 0: Without interrupt event when all key released. bit[4:2] : Long Key Recognition Factor.
  LCD_DataWrite(0x08);			   

  Main_Window_Start_XY(0,0);
  Active_Window_XY(0,0);
  Active_Window_WH(800,480); 					
  Goto_Text_XY(0,0);
  delay_ms(10);

  while(1)										     
  {	 

  if(Chk_Key_Press())
	 {	
	    Clear_KeyScan_Interrupt_Flag();

		delay_ms(10);
		temp = Read_Key_Strobe_Data_0();		//Key Strobe Data0 The corresponding key code 2 that is pressed.It will auto return to FFh if w/o any keys are pressed for a debounce time.
		temp1 = Read_Key_Strobe_Data_1();		//Key Strobe Data1 The corresponding key code 2 that is pressed.It will auto return to FFh if w/o any keys are pressed for a debounce time.
		temp2 = Read_Key_Strobe_Data_2();		//Key Strobe Data2 The corresponding key code 2 that is pressed.It will auto return to FFh if w/o any keys are pressed for a debounce time.
		delay_ms(100);
		Goto_Text_XY(0,0);
		LCD_CmdWrite(0x04);
		Print_Hex(temp);		//RA8889_Application_Using
		Check_Mem_WR_FIFO_Empty();
		Print_Hex(temp1);
		Check_Mem_WR_FIFO_Empty();
		Print_Hex(temp2);
		Check_Mem_WR_FIFO_Empty();
		delay_ms(100);	

	 }


  }
}


/*
NAND flash only support W25N01GVZEIG
*/
void NAND_DMA_Demo(void)
{
 SPI_NAND_initial_DMA (0 ,0);
 SPI_NAND_DMA (BINARY_INFO[0].start_addr,0,0,canvas_image_width,800,480,nand_buff,0,0,0x000000);
}

/*
NAND flash only support W25N01GVZEIG
*/
void NAND_JPG_Demo(void)
{
	
SPI_NAND_initial_JPG_AVI (1,3,1);
JPG_NAND (BINARY_INFO[3].start_addr,BINARY_INFO[1].img_size,canvas_image_width,0,0);
}

/*
NAND flash only support W25N01GVZEIG
*/
void NAND_AVI_Demo(void)
{
	unsigned char temp;

SPI_NAND_initial_JPG_AVI (1,3,1);

AVI_NAND(BINARY_INFO[5].start_addr,BINARY_INFO[5].img_size,shadow_buff,shadow_buff+2400,0,0,322,548,canvas_image_width);
AVI_window(1);
do{	
		temp = Read_Media_Decode_Busy();	//read busy flag to know media decode busy or idle
	}while(temp&0x40);
AVI_window(0);
}

void BTE_alpha_32bit_blending_pixel_mode_NAND_Demo(void)
{
SPI_NAND_initial_DMA (0 ,0);
SPI_NAND_DMA_png (BINARY_INFO[7].start_addr,nand_buff,0,80,80);
SPI_NAND_DMA_png (BINARY_INFO[6].start_addr,nand_buff+25600,0,80,80);
	
SPI_NAND_initial_JPG_AVI (1,3,1);
JPG_NAND (BINARY_INFO[1].start_addr,BINARY_INFO[1].img_size,canvas_image_width,0,0);

BTE_alpha_blending_32bit_Pixel_mode(80,80,80,600,400,0,canvas_image_width,600,400,0,canvas_image_width,nand_buff);
BTE_alpha_blending_32bit_Pixel_mode(80,80,80,700,400,0,canvas_image_width,700,400,0,canvas_image_width,nand_buff+25600);



}	



void NOR_JPG_Demo(void)
{
//SPI_NOR_initial_JPG_AVI (1,0,1,2,1);
SPI_NOR_initial_JPG_AVI (1,0,0,1,1);
JPG_NOR (1152000,42237,canvas_image_width,0,0);
}


void NOR_AVI_Demo(void)
{
	unsigned char temp;
SPI_NOR_initial_JPG_AVI (1,0,1,2,1);
AVI_NOR(1296674,13327214,shadow_buff,shadow_buff+2400,0,0,322,548,canvas_image_width);
AVI_window(1);
	
do{	
		temp = Read_Media_Decode_Busy();	//read busy flag to know media decode busy or idle
	  
	}while(temp&0x40);
AVI_window(0);

}


void BTE_alpha_32bit_blending_pixel_mode_NOR_Demo(void)
{
SPI_NOR_initial_DMA (3,0,1,1,0);
SPI_NOR_DMA_png (14623888,nand_buff,0,80,80);
SPI_NOR_DMA_png (14649488,nand_buff+25600,0,80,80);
	
SPI_NOR_initial_JPG_AVI (1,0,1,2,1);
JPG_NOR (1152000,42237,canvas_image_width,0,0);


BTE_alpha_blending_32bit_Pixel_mode(80,80,80,600,400,0,canvas_image_width,600,400,0,canvas_image_width,nand_buff);
BTE_alpha_blending_32bit_Pixel_mode(80,80,80,700,400,0,canvas_image_width,700,400,0,canvas_image_width,nand_buff+25600);

}