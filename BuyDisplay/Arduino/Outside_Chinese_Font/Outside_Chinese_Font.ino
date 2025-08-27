/***************************************************
//Web: http://www.buydisplay.com
EastRising Technology Co.,LTD
Examples for ER-PCBA6105RA8889+ER-TFT070-2  Outside Chinese Font
Display is Hardward SPI 4-Wire SPI Interface and 5V Power Supply
Tested and worked with:
Works with Arduino 1.6.0 IDE  
Test ok:  Arduino Due,Arduino UNO,Arduino MEGA2560
****************************************************/

#include <SPI.h>
#include <Wire.h>
#include "RA8889.h"

void setup() {
  
  pinMode(5,   OUTPUT);
  digitalWrite(5, HIGH);//Disable  SD 
  pinMode(2,   OUTPUT);
  digitalWrite(2, HIGH);//Disable  RTP    
  
  ER5517.Parallel_Init();
  ER5517.HW_Reset();
  ER5517.System_Check_Temp();
  delay(100);
  while(ER5517.LCD_StatusRead()&0x02);
  ER5517.initial();
  ER5517.Display_ON();

}



void LCD_Select_Outside_Font_Init
(
 unsigned char SCS           // 选择外挂的SPI   : SCS：0       SCS：1
,unsigned char Clk           // SPI时钟分频参数 : SPI Clock = System Clock /{(Clk+1)*2}
,unsigned long FlashAddr     // 源地址(Flash)
,unsigned long MemoryAddr    // 目的地址(SDRAM)
,unsigned long Num           // 字库的数据量大小
,unsigned char Size          // 设置字体大小  16：16*16     24:24*24    32:32*32
,unsigned char XxN           // 字体的宽度放大倍数：1~4
,unsigned char YxN           // 字体的高度放大倍数：1~4
,unsigned char ChromaKey     // 0：字体背景色透明    1：可以设置字体的背景色
,unsigned char Alignment     // 0：不字体不对齐      1：字体对齐
)
{
	if(Size==16)	ER5517.Font_Select_8x16_16x16();
	if(Size==24)	ER5517.Font_Select_12x24_24x24();
	if(Size==32)	ER5517.Font_Select_16x32_32x32();

	//(*)
	if(XxN==1)	ER5517.Font_Width_X1();
	if(XxN==2)	ER5517.Font_Width_X2();
	if(XxN==3)	ER5517.Font_Width_X3();
	if(XxN==4)	ER5517.Font_Width_X4();

	//(*)	
	if(YxN==1)	ER5517.Font_Height_X1();
	if(YxN==2)	ER5517.Font_Height_X2();
	if(YxN==3)	ER5517.Font_Height_X3();
	if(YxN==4)	ER5517.Font_Height_X4();

	//(*)
	if(ChromaKey==0)	ER5517.Font_Background_select_Color();	
	if(ChromaKey==1)	ER5517.Font_Background_select_Transparency();	

	//(*)
	if(Alignment==0)	ER5517.Disable_Font_Alignment();
	if(Alignment==1)	ER5517.Enable_Font_Alignment();	
	
	ER5517.DMA_24bit_Linear(SCS,Clk,FlashAddr,MemoryAddr,Num);
	ER5517.CGRAM_Start_address(MemoryAddr);        
}



void LCD_Print_Outside_Font_String
(
 unsigned short x               
,unsigned short y               
,unsigned long FontColor       
,unsigned long BackGroundColor 
,unsigned char *c               
)
{
	unsigned short temp_H = 0;
	unsigned short temp_L = 0;
	unsigned short temp = 0;
	unsigned int i = 0;
	
  ER5517.Text_Mode();
  ER5517.Font_Select_UserDefine_Mode();
  ER5517.Foreground_color_65k(FontColor);
  ER5517.Background_color_65k(BackGroundColor);
  ER5517.Goto_Text_XY(x,y);
	
	while(c[i] != '\0')
  { 
		if(c[i] < 0xa1)
		{
		  ER5517.CGROM_Select_Internal_CGROM();   // 内部CGROM为字符来源
		  ER5517.LCD_CmdWrite(0x04);
		  ER5517.LCD_DataWrite(c[i]);
		  ER5517.Check_Mem_WR_FIFO_not_Full();  
			i += 1;
		}
		else
		{
		ER5517.Font_Select_UserDefine_Mode();   // 自定义字库
		ER5517.LCD_CmdWrite(0x04);
			temp_H = ((c[i] - 0xa1) & 0x00ff) * 94;
			temp_L = c[i+1] - 0xa1;
			temp = temp_H + temp_L + 0x8000;
		ER5517.LCD_DataWrite((temp>>8)&0xff);
		ER5517.Check_Mem_WR_FIFO_not_Full();
		ER5517.LCD_DataWrite(temp&0xff);
		ER5517.Check_Mem_WR_FIFO_not_Full();
			i += 2;		
		}
	}
	
ER5517.Check_2D_Busy();

ER5517.Graphic_Mode(); //back to graphic mode;图形模式
}

#define FLASH_ADDR_16	  6030408
#define FLASH_ADDR_24	  6297672
#define FLASH_ADDR_32	  6899016

#define SIZE_16_NUM		267264
#define SIZE_24_NUM     601344
#define SIZE_32_NUM     1069056

#define MEMORY_ADDR_16   layer11_start_addr
#define MEMORY_ADDR_24	(MEMORY_ADDR_16+SIZE_16_NUM)
#define MEMORY_ADDR_32	(MEMORY_ADDR_24+SIZE_24_NUM)

void loop() {
  ER5517.Select_Main_Window_16bpp();
  ER5517.Main_Image_Start_Address(layer1_start_addr);        
  ER5517.Main_Image_Width(LCD_XSIZE_TFT);
  ER5517.Main_Window_Start_XY(0,0);
  ER5517.Canvas_Image_Start_address(0);
  ER5517.Canvas_image_width(LCD_XSIZE_TFT);
  ER5517.Active_Window_XY(0,0);
  ER5517.Active_Window_WH(LCD_XSIZE_TFT,LCD_YSIZE_TFT); 

 unsigned char f []={0xD0,0xF1,0xC8,0xD5,0xB6,0xAB,0xB7,0xBD,0xBF,0xC6,0xBC,0xBC,0x00};  //GBK 
 
 	while(1)
	{
		ER5517.DrawSquare_Fill(0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,Black);
		LCD_Select_Outside_Font_Init(1,0,FLASH_ADDR_16,MEMORY_ADDR_16,SIZE_16_NUM,16,1,1,0,0);
		LCD_Print_Outside_Font_String(20,50,Red,White, f );

		ER5517.Font_Width_X2();
		ER5517.Font_Height_X2();
		LCD_Print_Outside_Font_String(20,75,LIGHTRED,Black,f);
	
		ER5517.Font_Width_X3();
		ER5517.Font_Height_X3();
		LCD_Print_Outside_Font_String(20,120,Blue,White,f);
	
		ER5517.Font_Width_X4();
		ER5517.Font_Height_X4();
		LCD_Print_Outside_Font_String(20,178,Purple,Black,f);


	        delay(3000);



		LCD_Select_Outside_Font_Init(1,0,FLASH_ADDR_24,MEMORY_ADDR_24,SIZE_24_NUM,24,1,1,0,0);

                ER5517.DrawSquare_Fill(0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,Blue);     
		LCD_Print_Outside_Font_String(20,20,Red,White,f);
	
		ER5517.Font_Width_X2();
		ER5517.Font_Height_X2();
		LCD_Print_Outside_Font_String(20,55,Green,Black,f);

		ER5517.Font_Width_X3();
		ER5517.Font_Height_X3();
		LCD_Print_Outside_Font_String(20,115,Cyan,White,f);
	
		ER5517.Font_Width_X4();
		ER5517.Font_Height_X4();
		LCD_Print_Outside_Font_String(20,195,Yellow,Black,f);

	    delay(3000);

		LCD_Select_Outside_Font_Init(1,0,FLASH_ADDR_32,MEMORY_ADDR_32,SIZE_32_NUM,32,1,1,0,0);
		
		ER5517.DrawSquare_Fill(0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,White);
		LCD_Print_Outside_Font_String(20,55,Red,White,f);
	
		ER5517.Font_Width_X2();
		ER5517.Font_Height_X2();
		LCD_Print_Outside_Font_String(20,115,Green,Black,f);

		ER5517.Font_Width_X3();
		ER5517.Font_Height_X3();
		LCD_Print_Outside_Font_String(20,200,Blue,White,f);
	
		ER5517.Font_Width_X4();
		ER5517.Font_Height_X4();
		LCD_Print_Outside_Font_String(20,310,Yellow,Black,f);
		
		    delay(3000);	
	}
  
    
}
