 
  /*************** RAIO Technology Inc. ***************
  * file		: RA8889_API.c
  * author		: RAIO Application Team ^v^ 
  * version		: V1.0  
  * date		: 2020/04/21 
  * brief		: 	
	* RAIO copyright
  ****************************************************/		   


#include <stdio.h>
#include <stdlib.h>
#include "stm32f10x.h"
#include "delay.h"

#include "UserDef.h"
#include "RA8889_MCU_IF.h"
#include "RA8889.h"
#include "RA8889_API.h"
#include "ascii_table_8x12.h"
#include "ascii_table_16x24.h"
#include "ascii_table_32x48.h"
#include "W25N01GV.h"
char tmp1[];		 



void MPU8_8bpp_Memory_Write
(
unsigned short x //x of coordinate 
,unsigned short y // y of coordinate 
,unsigned short w //width
,unsigned short h //height
,const unsigned char *data //8bit data
)
{														  
	unsigned short i,j;
	Graphic_Mode();
    Active_Window_XY(x,y);
	Active_Window_WH(w,h); 					
	Goto_Pixel_XY(x,y);
	LCD_CmdWrite(0x04);	
for(i=0;i< h;i++)
{	
	for(j=0;j< w;j++)
 	{	   
	 Check_Mem_WR_FIFO_not_Full();
	 LCD_DataWrite(*data);
	 data++;
	}
}
	Check_Mem_WR_FIFO_Empty();
}	 

			
void MPU8_16bpp_Memory_Write
(
unsigned short x // x of coordinate 
,unsigned short y // y of coordinate 
,unsigned short w // width
,unsigned short h // height
,const unsigned char *data // 8bit data
)
{
	unsigned short i,j;
	Graphic_Mode();
    Active_Window_XY(x,y);
	Active_Window_WH(w,h); 					
	Goto_Pixel_XY(x,y);
	LCD_CmdWrite(0x04);
for(i=0;i< h;i++)
{	
	for(j=0;j< w;j++)
 	{
	 Check_Mem_WR_FIFO_not_Full();
	 LCD_DataWrite(*data);
	 data++;
	 Check_Mem_WR_FIFO_not_Full();
	 LCD_DataWrite(*data);
	 data++;
	}
}
	Check_Mem_WR_FIFO_Empty();
}		 


void MPU8_24bpp_Memory_Write 
(
unsigned short x // x of coordinate
,unsigned short y // y of coordinate 
,unsigned short w // width
,unsigned short h // height
,const unsigned char *data // 8bit data
)

{
	unsigned short i,j;
	Graphic_Mode();
    Active_Window_XY(x,y);
	Active_Window_WH(w,h); 					
	Goto_Pixel_XY(x,y);
	LCD_CmdWrite(0x04);
for(i=0;i< h;i++)
{	
	for(j=0;j< w;j++)
 	{
	 Check_Mem_WR_FIFO_not_Full();
	 LCD_DataWrite(*data);
	 data++;
	 Check_Mem_WR_FIFO_not_Full();
	 LCD_DataWrite(*data);
	 data++;
	 Check_Mem_WR_FIFO_not_Full();
	 LCD_DataWrite(*data);
	 data++;
	}
}
	Check_Mem_WR_FIFO_Empty();
}



void MPU16_16bpp_Memory_Write 
(
unsigned short x //x of coordinate
,unsigned short y //y of coordinate 
,unsigned short w //width
,unsigned short h //height
,const unsigned short *data //16bit data
)			
{
	unsigned short i,j;
	Graphic_Mode();
    Active_Window_XY(x,y);
	Active_Window_WH(w,h); 					
	Goto_Pixel_XY(x,y);
	LCD_CmdWrite(0x04);
for(i=0;i< h;i++)
{	
	for(j=0;j< w;j++)
 	{
	 Check_Mem_WR_FIFO_not_Full();
	 LCD_DataWrite(*data);
	 data++;
	}
}
	Check_Mem_WR_FIFO_Empty();
}




void MPU16_24bpp_Mode1_Memory_Write 
(
unsigned short x //x of coordinate
,unsigned short y //y of coordinate
,unsigned short w //width
,unsigned short h //height
,const unsigned short *data //16bit data
)	
{
	unsigned short i,j;
	Graphic_Mode();
    Active_Window_XY(x,y);
	Active_Window_WH(w,h); 					
	Goto_Pixel_XY(x,y);
	LCD_CmdWrite(0x04);
for(i=0;i< h;i++)
{	
	for(j=0;j< w/2;j++)
 	{
	 LCD_DataWrite(*data);
	 Check_Mem_WR_FIFO_not_Full();
	 data++;
	 LCD_DataWrite(*data);
	 Check_Mem_WR_FIFO_not_Full();
	 data++;
	 LCD_DataWrite(*data);
	 Check_Mem_WR_FIFO_not_Full();
	 data++;
	}
}
	Check_Mem_WR_FIFO_Empty();
}


void MPU16_24bpp_Mode2_Memory_Write
(
unsigned short x //x of coordinate
,unsigned short y //y of coordinate
,unsigned short w //width
,unsigned short h //height
,const unsigned short *data //16bit data
)	
{
	unsigned short i,j;
	Graphic_Mode();
    Active_Window_XY(x,y);
	Active_Window_WH(w,h); 					
	Goto_Pixel_XY(x,y);
	LCD_CmdWrite(0x04);
for(i=0;i< h;i++)
{	
	for(j=0;j< w;j++)
 	{
	 Check_Mem_WR_FIFO_not_Full();
	 LCD_DataWrite(*data);
	 data++;
	 Check_Mem_WR_FIFO_not_Full();
	 LCD_DataWrite(*data);
	 data++;
	}
}
	Check_Mem_WR_FIFO_Empty();
}




void PIP
(
unsigned char On_Off // 0 : disable PIP, 1 : enable PIP, 2 : To maintain the original state
,unsigned char Select_PIP // 1 : use PIP1 , 2 : use PIP2
,unsigned long PAddr //start address of PIP
,unsigned short XP //coordinate X of PIP Window, It must be divided by 4.
,unsigned short YP //coordinate Y of PIP Window, It must be divided by 4.
,unsigned long ImageWidth //Image Width of PIP (recommend = canvas image width)
,unsigned short X_Dis //coordinate X of Display Window
,unsigned short Y_Dis //coordinate Y of Display Window
,unsigned short X_W //width of PIP and Display Window, It must be divided by 4.
,unsigned short Y_H //height of PIP and Display Window , It must be divided by 4.
)
{
	if(Select_PIP == 1 )  
	{
	Select_PIP1_Parameter();
	}
	if(Select_PIP == 2 )  
	{
	Select_PIP2_Parameter();
	}
	PIP_Display_Start_XY(X_Dis,Y_Dis);
	PIP_Image_Start_Address(PAddr);
	PIP_Image_Width(ImageWidth);
	PIP_Window_Image_Start_XY(XP,YP);
	PIP_Window_Width_Height(X_W,Y_H);


	if(On_Off == 0)
    {
  		if(Select_PIP == 1 )  
		{ 
		Disable_PIP1();	
		}
		if(Select_PIP == 2 )  
		{
		Disable_PIP2();
		}
    }

    if(On_Off == 1)
    {
  		if(Select_PIP == 1 )  
		{ 
		Enable_PIP1();	
		}
		if(Select_PIP == 2 )  
		{
		Enable_PIP2();
		}
    }
   
}

 


/*internal font
x : Print font start coordinate of X
y : Print font start coordinate of Y
X_W : active window width
Y_H : active window height
FontColor : Set Font Color
BackGroundColor : Set Font BackGround Color 
Font Color and BackGround Color dataformat : 
ColorDepth_8bpp : R3G3B2
ColorDepth_16bpp : R5G6B5
ColorDepth_24bpp : R8G8B8
tmp2 : Hex variable you want print (range : 0 ~ 32bit)
*/
void Print_Internal_Font_Hexvariable(unsigned short x,unsigned short y,unsigned short X_W,unsigned short Y_H,unsigned long FontColor,unsigned long BackGroundColor,  unsigned int tmp2)
{	
   	Text_Mode();
	CGROM_Select_Internal_CGROM();
#ifdef MCU_8bit_ColorDepth_8bpp
    Foreground_color_256(FontColor);
	Background_color_256(BackGroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_16bpp
    Foreground_color_65k(FontColor);
	Background_color_65k(BackGroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_24bpp
    Foreground_color_16M(FontColor);
	Background_color_16M(BackGroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_16bpp	
    Foreground_color_65k(FontColor);
	Background_color_65k(BackGroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
    Foreground_color_16M(FontColor);
	Background_color_16M(BackGroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
    Foreground_color_16M(FontColor);
	Background_color_16M(BackGroundColor);
#endif
	Active_Window_XY(x,y);
    Active_Window_WH(X_W,Y_H);
	Goto_Text_XY(x,y);
	sprintf(tmp1 ,"%X", tmp2); 
	Show_String(tmp1);
}



/*internal font
x : Print font start coordinate of X
y : Print font start coordinate of Y
X_W : active window width
Y_H : active window height
FontColor : Set Font Color
BackGroundColor : Set Font BackGround Color 
Font Color and BackGround Color dataformat :
ColorDepth_8bpp : R3G3B2
ColorDepth_16bpp : R5G6B5
ColorDepth_24bpp : R8G8B8
tmp2 : Decimalvariable you want print (range : 0 ~ 32bit)
*/
void Print_Internal_Font_Decimalvariable(unsigned short x,unsigned short y,unsigned short X_W,unsigned short Y_H,unsigned long FontColor,unsigned long BackGroundColor,unsigned int tmp2)
{	

	Text_Mode();
	CGROM_Select_Internal_CGROM();
#ifdef MCU_8bit_ColorDepth_8bpp
    Foreground_color_256(FontColor);
	Background_color_256(BackGroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_16bpp
    Foreground_color_65k(FontColor);
	Background_color_65k(BackGroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_24bpp
    Foreground_color_16M(FontColor);
	Background_color_16M(BackGroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_16bpp	
    Foreground_color_65k(FontColor);
	Background_color_65k(BackGroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
    Foreground_color_16M(FontColor);
	Background_color_16M(BackGroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
    Foreground_color_16M(FontColor);
	Background_color_16M(BackGroundColor);
#endif
    Active_Window_XY(x,y);
    Active_Window_WH(X_W,Y_H);
	Goto_Text_XY(x,y);
	sprintf(tmp1 ,"%d", tmp2); 
	Show_String(tmp1);
}





void Print_Internal_Font_String
(
unsigned short x //coordinate x for print string
,unsigned short y //coordinate x for print string
,unsigned short X_W //active window width
,unsigned short Y_H //active window height
,unsigned long FontColor //FontColor : Set Font Color
,unsigned long BackGroundColor 
/*BackGroundColor : Set Font BackGround Color.Font Color and BackGround Color dataformat :
ColorDepth_8bpp : R3G3B2
ColorDepth_16bpp : R5G6B5
ColorDepth_24bpp : R8G8B8*/
,char tmp2[] //tmp2 : Font String which you want print on LCD
)
{

  	Text_Mode();
	CGROM_Select_Internal_CGROM();
	#ifdef MCU_8bit_ColorDepth_8bpp	
    Foreground_color_256(FontColor);
	Background_color_256(BackGroundColor);
	#endif
	#ifdef MCU_8bit_ColorDepth_16bpp	
    Foreground_color_65k(FontColor);
	Background_color_65k(BackGroundColor);
	#endif
	#ifdef MCU_8bit_ColorDepth_24bpp	
    Foreground_color_16M(FontColor);
	Background_color_16M(BackGroundColor);
	#endif
	#ifdef MCU_16bit_ColorDepth_16bpp	
    Foreground_color_65k(FontColor);
	Background_color_65k(BackGroundColor);
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
    Foreground_color_16M(FontColor);
	Background_color_16M(BackGroundColor);
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
    Foreground_color_16M(FontColor);
	Background_color_16M(BackGroundColor);
	#endif
	Active_Window_XY(x,y);							 
	Active_Window_WH(X_W,Y_H);
	Goto_Text_XY(x,y);
//	sprintf(tmp1,"%s",tmp2); 
//	Show_String(tmp1);
	Show_String(tmp2);

}





void Print_BIG5String
(
unsigned char Clk //SPI CLK = System Clock / 2*(Clk+1)
,unsigned char BUS	// 0 : Bus0, 1:Bus1
,unsigned char SCS //0 : use CS0 , 1 : use CS1 , 2 : use CS2, 3 :use CS3 
,unsigned short x //coordinate x for print string
,unsigned short y //coordinate y for print string
,unsigned short X_W //active window width
,unsigned short Y_H //active window height
,unsigned long FontColor //Set Font Color
,unsigned long BackGroundColor //Set Font BackGround Color 
/*Font Color and BackGround Color dataformat :
ColorDepth_8bpp : R3G3B2
ColorDepth_16bpp : R5G6B5
ColorDepth_24bpp : R8G8B8*/
,char *tmp2 //tmp2 : BIG5 Font String which you want print on LCD
)
{

  Select_SFI_Font_Mode();
  CGROM_Select_Genitop_FontROM();
  SPI_Clock_Period(Clk);
  Set_GTFont_Decoder(0x11);  
	
	Select_SFI_24bit_Address();
	Select_RA8875_SPI_Mode0_and_Mode3();
	Select_SFI_Single_Mode_Dummy_0T_03h();

switch(SCS)	
{
	case 0:
  Select_SFI_0();
	break;
	case 1:
  Select_SFI_1();
	break;
	case 2:
  Select_SFI_2();
	break;
	case 3:
  Select_SFI_3();
	break;
}


switch(BUS)
{
	case 0:
	FONT_DMA_Select_Bus_0();
	break;
	case 1:
	FONT_DMA_Select_Bus_1();
	break;
}


#ifdef MCU_8bit_ColorDepth_8bpp	
  Foreground_color_256(FontColor);
  Background_color_256(BackGroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_16bpp	
  Foreground_color_65k(FontColor);
  Background_color_65k(BackGroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_24bpp	
  Foreground_color_16M(FontColor);
  Background_color_16M(BackGroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_16bpp	
  Foreground_color_65k(FontColor);					                               
  Background_color_65k(BackGroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
  Foreground_color_16M(FontColor);
  Background_color_16M(BackGroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
  Foreground_color_16M(FontColor);
  Background_color_16M(BackGroundColor);
#endif
  Text_Mode();
  Active_Window_XY(x,y);
  Active_Window_WH(X_W,Y_H);
  Goto_Text_XY(x,y);
//  sprintf(tmp1,"%s",tmp2); 
//  Show_String(tmp1);
  Show_String(tmp2);

}




void Print_GB2312String
(
unsigned char Clk //Clk : SPI CLK = System Clock / 2*(Clk+1)
,unsigned char BUS	// 0 : Bus0, 1:Bus1
,unsigned char SCS //0 : use CS0 , 1 : use CS1 , 2 : use CS2, 3 :use CS3 
,unsigned short x //coordinate x for print string
,unsigned short y //coordinate y for print string
,unsigned short X_W //active window width
,unsigned short Y_H //active window height
,unsigned long FontColor //Set Font Color
,unsigned long BackGroundColor //Set Font BackGround Color 
/*Font Color and BackGround Color dataformat :
ColorDepth_8bpp : R3G3B2
ColorDepth_16bpp : R5G6B5
ColorDepth_24bpp : R8G8B8*/
,char tmp2[] //tmp2 : GB2312 Font String which you want print on LCD
)
{

  Select_SFI_Font_Mode();
  CGROM_Select_Genitop_FontROM();
  SPI_Clock_Period(Clk);
  Set_GTFont_Decoder(0x01);      

	Select_SFI_24bit_Address();
	Select_RA8875_SPI_Mode0_and_Mode3();
	Select_SFI_Single_Mode_Dummy_0T_03h();

switch(SCS)	
{
	case 0:
  Select_SFI_0();
	break;
	case 1:
  Select_SFI_1();
	break;
	case 2:
  Select_SFI_2();
	break;
	case 3:
  Select_SFI_3();
	break;
}


switch(BUS)
{
	case 0:
	FONT_DMA_Select_Bus_0();
	break;
	case 1:
	FONT_DMA_Select_Bus_1();
	break;
}


#ifdef MCU_8bit_ColorDepth_8bpp	
  Foreground_color_256(FontColor);
  Background_color_256(BackGroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_16bpp	
  Foreground_color_65k(FontColor);
  Background_color_65k(BackGroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_24bpp	
  Foreground_color_16M(FontColor);
  Background_color_16M(BackGroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_16bpp	
  Foreground_color_65k(FontColor);
  Background_color_65k(BackGroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
  Foreground_color_16M(FontColor);
  Background_color_16M(BackGroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
  Foreground_color_16M(FontColor);
  Background_color_16M(BackGroundColor);
#endif
  Text_Mode();
  Active_Window_XY(x,y);
  Active_Window_WH(X_W,Y_H);
  Goto_Text_XY(x,y);
//  sprintf(tmp1,"%s",tmp2); 
//  Show_String(tmp1);
  Show_String(tmp2);

}


void Print_GB12345String
(
unsigned char Clk //Clk : SPI CLK = System Clock / 2*(Clk+1)
,unsigned char BUS	// 0 : Bus0, 1:Bus1
,unsigned char SCS //0 : use CS0 , 1 : use CS1 , 2 : use CS2, 3 :use CS3 
,unsigned short x //coordinate x for print string
,unsigned short y ////coordinate y for print string
,unsigned short X_W //active window width
,unsigned short Y_H //active window height
,unsigned long FontColor //Set Font Color
,unsigned long BackGroundColor //Set Font BackGround Color 
/*Font Color and BackGround Color dataformat :
ColorDepth_8bpp : R3G3B2
ColorDepth_16bpp : R5G6B5
ColorDepth_24bpp : R8G8B8*/
,char *tmp2 //tmp2 : GB12345 Font String which you want print on LCD
)
{  
  Select_SFI_Font_Mode();
  CGROM_Select_Genitop_FontROM();
  SPI_Clock_Period(Clk);
  Set_GTFont_Decoder(0x05);      

	Select_SFI_24bit_Address();
	Select_RA8875_SPI_Mode0_and_Mode3();
	Select_SFI_Single_Mode_Dummy_0T_03h();

switch(SCS)	
{
	case 0:
  Select_SFI_0();
	break;
	case 1:
  Select_SFI_1();
	break;
	case 2:
  Select_SFI_2();
	break;
	case 3:
  Select_SFI_3();
	break;
}


switch(BUS)
{
	case 0:
	FONT_DMA_Select_Bus_0();
	break;
	case 1:
	FONT_DMA_Select_Bus_1();
	break;
}
	

#ifdef MCU_8bit_ColorDepth_8bpp	
  Foreground_color_256(FontColor);
  Background_color_256(BackGroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_16bpp	
  Foreground_color_65k(FontColor);
  Background_color_65k(BackGroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_24bpp	
  Foreground_color_16M(FontColor);
  Background_color_16M(BackGroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_16bpp	
  Foreground_color_65k(FontColor);
  Background_color_65k(BackGroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
  Foreground_color_16M(FontColor);
  Background_color_16M(BackGroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
  Foreground_color_16M(FontColor);
  Background_color_16M(BackGroundColor);
#endif
  Text_Mode();
  Active_Window_XY(x,y);
  Active_Window_WH(X_W,Y_H);
  Goto_Text_XY(x,y);
//  sprintf(tmp1,"%s",tmp2); 
//  Show_String(tmp1);
  Show_String(tmp2);
}




 
void Print_UnicodeString
(
unsigned char Clk //SPI CLK = System Clock / 2*(Clk+1)
,unsigned char BUS	// 0 : Bus0, 1:Bus1
,unsigned char SCS //0 : use CS0 , 1 : use CS1 , 2 : use CS2, 3 :use CS3 
,unsigned short x //Print font start coordinate of X
,unsigned short y //Print font start coordinate of Y
,unsigned short X_W //active window width
,unsigned short Y_H //active window height
,unsigned long FontColor //Set Font Color
,unsigned long BackGroundColor //Set Font BackGround Color 
/*Font Color and BackGround Color dataformat :
ColorDepth_8bpp : R3G3B2
ColorDepth_16bpp : R5G6B5
ColorDepth_24bpp : R8G8B8*/
,unsigned short *tmp2 /*tmp2 : Unicode Font String which you want print on LCD (L"string" in keil c is Unicode string)*/
)
{
  Select_SFI_Font_Mode();
  CGROM_Select_Genitop_FontROM();
  SPI_Clock_Period(Clk);

	Select_SFI_24bit_Address();
	Select_RA8875_SPI_Mode0_and_Mode3();
	Select_SFI_Single_Mode_Dummy_0T_03h();

switch(SCS)	
{
	case 0:
  Select_SFI_0();
	break;
	case 1:
  Select_SFI_1();
	break;
	case 2:
  Select_SFI_2();
	break;
	case 3:
  Select_SFI_3();
	break;
}


switch(BUS)
{
	case 0:
	FONT_DMA_Select_Bus_0();
	break;
	case 1:
	FONT_DMA_Select_Bus_1();
	break;
}

#ifdef MCU_8bit_ColorDepth_8bpp	
  Foreground_color_256(FontColor);
  Background_color_256(BackGroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_16bpp	
  Foreground_color_65k(FontColor);
  Background_color_65k(BackGroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_24bpp	
  Foreground_color_16M(FontColor);
  Background_color_16M(BackGroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_16bpp	
  Foreground_color_65k(FontColor);
  Background_color_65k(BackGroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
  Foreground_color_16M(FontColor);
  Background_color_16M(BackGroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
  Foreground_color_16M(FontColor);
  Background_color_16M(BackGroundColor);
#endif
  Text_Mode();
  Active_Window_XY(x,y);
  Active_Window_WH(X_W,Y_H);
  Goto_Text_XY(x,y);
  while(*tmp2 != '\0')
  {
    if((*tmp2)>0x0020 && (*tmp2)<0x0080)
		{
			/* ASCII Code*/
			Set_GTFont_Decoder(0x21); 
			LCD_CmdWrite(0x04);
			LCD_DataWrite(*tmp2);
			Check_2D_Busy();
		}
    else
		{
			/* Unicode */
	        Set_GTFont_Decoder(0x18);
			LCD_CmdWrite(0x04);
			LCD_DataWrite((*tmp2)>>8); 
            LCD_DataWrite(*tmp2);
            Check_2D_Busy();
      
		}
 	        ++tmp2;
  }
  
  Graphic_Mode(); //back to graphic mode
}





void Select_Font_Height_WxN_HxN_ChromaKey_Alignment
(
unsigned char Font_Height 
/*Font_Height:
16 : Font = 8x16 、16x16
24 : Font = 12x24、24x24  
32 : Font = 16x32、32x32 */
,unsigned char XxN //XxN :Font Width x 1~4
,unsigned char YxN //YxN :Font Height x 1~4
,unsigned char ChromaKey 
/*ChromaKey :
0 : Font Background color not transparency
1 : Set Font Background color transparency*/
,unsigned char Alignment // 0 : no alignment, 1 : Set font alignment
)
{
	if(Font_Height==16)	Font_Select_8x16_16x16();
	if(Font_Height==24)	Font_Select_12x24_24x24();
	if(Font_Height==32)	Font_Select_16x32_32x32();

	//(*)
	if(XxN==1)	Font_Width_X1();
	if(XxN==2)	Font_Width_X2();
	if(XxN==3)	Font_Width_X3();
	if(XxN==4)	Font_Width_X4();

	//(*)	
	if(YxN==1)	Font_Height_X1();
	if(YxN==2)	Font_Height_X2();
	if(YxN==3)	Font_Height_X3();
	if(YxN==4)	Font_Height_X4();

	//(*)
	if(ChromaKey==0)
	{	Font_Background_select_Color();	}

	if(ChromaKey==1)
	{	Font_Background_select_Original_Canvas();	}

	//(*)
	if(Alignment==0)
	{	Disable_Font_Alignment();	}

	if(Alignment==1)
	{	Enable_Font_Alignment();	}

} 




void Show_String(char *str)
{  
	Check_Mem_WR_FIFO_Empty();
	
	Text_Mode();
	
	LCD_CmdWrite(0x04);
	while(*str != '\0')
	{
		Check_Mem_WR_FIFO_not_Full();  
		LCD_DataWrite(*str);
		++str; 
	} 
	Check_2D_Busy();
	Graphic_Mode(); //back to graphic mode
}



void Draw_Line
(
unsigned long LineColor 
/*LineColor : Set Draw Line color. Line Color dataformat :
ColorDepth_8bpp : R3G3B2、ColorDepth_16bpp : R5G6B5、ColorDepth_24bpp : R8G8B8*/
,unsigned short X1 //X of point1 coordinate
,unsigned short Y1 //Y of point1 coordinate
,unsigned short X2 //X of point2 coordinate
,unsigned short Y2 // Y of point2 coordinate
)
{

#ifdef MCU_8bit_ColorDepth_8bpp	
  Foreground_color_256(LineColor);
#endif
#ifdef MCU_8bit_ColorDepth_16bpp	
  Foreground_color_65k(LineColor);
#endif
#ifdef MCU_8bit_ColorDepth_24bpp	
  Foreground_color_16M(LineColor);
#endif
#ifdef MCU_16bit_ColorDepth_16bpp	
  Foreground_color_65k(LineColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
  Foreground_color_16M(LineColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
  Foreground_color_16M(LineColor);
#endif
 Line_Start_XY(X1,Y1);
 Line_End_XY(X2,Y2);
 Start_Line();
 Check_2D_Busy(); 
}



void Draw_Triangle
(
unsigned long ForegroundColor 
/*ForegroundColor: Set Draw Triangle color. ForegroundColor Color dataformat :
ColorDepth_8bpp : R3G3B2、ColorDepth_16bpp : R5G6B5、ColorDepth_24bpp : R8G8B8*/
,unsigned short X1 //X of point1 coordinate
,unsigned short Y1 //Y of point1 coordinate
,unsigned short X2 //X of point2 coordinate
,unsigned short Y2 //Y of point2 coordinate
,unsigned short X3 //X of point3 coordinate
,unsigned short Y3 //Y of point3 coordinate
)
{ 
#ifdef MCU_8bit_ColorDepth_8bpp	
  Foreground_color_256(ForegroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_16bpp	
  Foreground_color_65k(ForegroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_24bpp	
  Foreground_color_16M(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_16bpp	
  Foreground_color_65k(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
  Foreground_color_16M(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
  Foreground_color_16M(ForegroundColor);
#endif
  Triangle_Point1_XY(X1,Y1);
  Triangle_Point2_XY(X2,Y2);
  Triangle_Point3_XY(X3,Y3);
  Start_Triangle();
  Check_2D_Busy(); 
}


void Draw_Triangle_Fill
(
unsigned long ForegroundColor 
/*ForegroundColor: Set Draw Triangle color. ForegroundColor Color dataformat :
ColorDepth_8bpp : R3G3B2、ColorDepth_16bpp : R5G6B5、ColorDepth_24bpp : R8G8B8*/
,unsigned short X1 //X of point1 coordinate
,unsigned short Y1 //Y of point1 coordinate
,unsigned short X2 //X of point2 coordinate
,unsigned short Y2 //Y of point2 coordinate
,unsigned short X3 //X of point3 coordinate
,unsigned short Y3 //Y of point3 coordinate
)
{
#ifdef MCU_8bit_ColorDepth_8bpp	
  Foreground_color_256(ForegroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_16bpp	
  Foreground_color_65k(ForegroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_24bpp	
  Foreground_color_16M(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_16bpp	
  Foreground_color_65k(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
  Foreground_color_16M(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
  Foreground_color_16M(ForegroundColor);
#endif
  Triangle_Point1_XY(X1,Y1);
  Triangle_Point2_XY(X2,Y2);
  Triangle_Point3_XY(X3,Y3);
  Start_Triangle_Fill();
  Check_2D_Busy(); 
}



void Draw_Circle
(
unsigned long ForegroundColor //ForegroundColor: Set Draw Circle or Circle Fill color
/*ForegroundColor Color dataformat :
ColorDepth_8bpp : R3G3B2、ColorDepth_16bpp : R5G6B5、ColorDepth_24bpp : R8G8B8*/
,unsigned short XCenter //coordinate X of Center
,unsigned short YCenter //coordinate Y of Center
,unsigned short R //Circle Radius
)
{
#ifdef MCU_8bit_ColorDepth_8bpp	
  Foreground_color_256(ForegroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_16bpp	
  Foreground_color_65k(ForegroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_24bpp	
  Foreground_color_16M(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_16bpp	
  Foreground_color_65k(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
  Foreground_color_16M(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
  Foreground_color_16M(ForegroundColor);
#endif
  Circle_Center_XY(XCenter,YCenter);
  Circle_Radius_R(R);
  Start_Circle_or_Ellipse();
  Check_2D_Busy(); 
}


void Draw_Circle_Fill
(
unsigned long ForegroundColor 
/*ForegroundColor: Set Draw Circle or Circle Fill color 
ForegroundColor Color dataformat :
ColorDepth_8bpp : R3G3B2、ColorDepth_16bpp : R5G6B5、ColorDepth_24bpp : R8G8B8*/
,unsigned short XCenter //coordinate X of Center
,unsigned short YCenter //coordinate Y of Center
,unsigned short R //Circle Radius
)
{
#ifdef MCU_8bit_ColorDepth_8bpp	
  Foreground_color_256(ForegroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_16bpp	
  Foreground_color_65k(ForegroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_24bpp	
  Foreground_color_16M(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_16bpp	
  Foreground_color_65k(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
  Foreground_color_16M(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
  Foreground_color_16M(ForegroundColor);
#endif
  Circle_Center_XY(XCenter,YCenter);
  Circle_Radius_R(R);
  Start_Circle_or_Ellipse_Fill();
  Check_2D_Busy(); 
}



void Draw_Ellipse
(
unsigned long ForegroundColor //ForegroundColor : Set Draw Ellipse or Ellipse Fill color
/*ForegroundColor Color dataformat :
ColorDepth_8bpp : R3G3B2、ColorDepth_16bpp : R5G6B5、ColorDepth_24bpp : R8G8B8*/
,unsigned short XCenter //coordinate X of Center
,unsigned short YCenter //coordinate Y of Center
,unsigned short X_R // Radius Width of Ellipse
,unsigned short Y_R // Radius Length of Ellipse
)
{
#ifdef MCU_8bit_ColorDepth_8bpp	
  Foreground_color_256(ForegroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_16bpp	
  Foreground_color_65k(ForegroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_24bpp	
  Foreground_color_16M(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_16bpp	
  Foreground_color_65k(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
  Foreground_color_16M(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
  Foreground_color_16M(ForegroundColor);
#endif
  Ellipse_Center_XY(XCenter,YCenter);
  Ellipse_Radius_RxRy(X_R,Y_R);
  Start_Circle_or_Ellipse();
  Check_2D_Busy(); 
}



void Draw_Ellipse_Fill
(
unsigned long ForegroundColor //ForegroundColor : Set Draw Ellipse or Ellipse Fill color
/*ForegroundColor Color dataformat :
ColorDepth_8bpp : R3G3B2、ColorDepth_16bpp : R5G6B5、ColorDepth_24bpp : R8G8B8*/
,unsigned short XCenter //coordinate X of Center
,unsigned short YCenter //coordinate Y of Center
,unsigned short X_R // Radius Width of Ellipse
,unsigned short Y_R // Radius Length of Ellipse
)				
{
#ifdef MCU_8bit_ColorDepth_8bpp	
  Foreground_color_256(ForegroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_16bpp	
  Foreground_color_65k(ForegroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_24bpp	
  Foreground_color_16M(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_16bpp	
  Foreground_color_65k(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
  Foreground_color_16M(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
  Foreground_color_16M(ForegroundColor);
#endif
  Ellipse_Center_XY(XCenter,YCenter);
  Ellipse_Radius_RxRy(X_R,Y_R);
  Start_Circle_or_Ellipse_Fill();
  Check_2D_Busy(); 
}




void Draw_Left_Up_Curve
(
unsigned long ForegroundColor //ForegroundColor: Set Curve or Curve Fill color 
/*ForegroundColor Color dataformat :
ColorDepth_8bpp : R3G3B2、ColorDepth_16bpp : R5G6B5、ColorDepth_24bpp : R8G8B8*/
,unsigned short XCenter //coordinate X of Center
,unsigned short YCenter //coordinate Y of Center
,unsigned short X_R // Radius Width of Curve
,unsigned short Y_R // Radius Length of Curve
)
{
#ifdef MCU_8bit_ColorDepth_8bpp	
  Foreground_color_256(ForegroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_16bpp	
  Foreground_color_65k(ForegroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_24bpp	
  Foreground_color_16M(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_16bpp	
  Foreground_color_65k(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
  Foreground_color_16M(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
  Foreground_color_16M(ForegroundColor);
#endif
 Ellipse_Center_XY(XCenter,YCenter);
 Ellipse_Radius_RxRy(X_R,Y_R);
 Start_Left_Up_Curve();
 Check_2D_Busy(); 
}



void Draw_Left_Up_Curve_Fill
(
unsigned long ForegroundColor //ForegroundColor: Set Curve or Curve Fill color 
/*ForegroundColor Color dataformat :
ColorDepth_8bpp : R3G3B2、ColorDepth_16bpp : R5G6B5、ColorDepth_24bpp : R8G8B8*/
,unsigned short XCenter //coordinate X of Center
,unsigned short YCenter //coordinate Y of Center
,unsigned short X_R // Radius Width of Curve
,unsigned short Y_R // Radius Length of Curve
)
{
#ifdef MCU_8bit_ColorDepth_8bpp	
  Foreground_color_256(ForegroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_16bpp	
  Foreground_color_65k(ForegroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_24bpp	
  Foreground_color_16M(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_16bpp	
  Foreground_color_65k(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
  Foreground_color_16M(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
  Foreground_color_16M(ForegroundColor);
#endif
  Ellipse_Center_XY(XCenter,YCenter);
  Ellipse_Radius_RxRy(X_R,Y_R);
  Start_Left_Up_Curve_Fill();
  Check_2D_Busy(); 
}




void Draw_Right_Down_Curve
(
unsigned long ForegroundColor //ForegroundColor: Set Curve or Curve Fill color 
/*ForegroundColor Color dataformat :
ColorDepth_8bpp : R3G3B2、ColorDepth_16bpp : R5G6B5、ColorDepth_24bpp : R8G8B8*/
,unsigned short XCenter //coordinate X of Center
,unsigned short YCenter //coordinate Y of Center
,unsigned short X_R // Radius Width of Curve
,unsigned short Y_R // Radius Length of Curve
) 
{
#ifdef MCU_8bit_ColorDepth_8bpp	
  Foreground_color_256(ForegroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_16bpp	
  Foreground_color_65k(ForegroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_24bpp	
  Foreground_color_16M(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_16bpp	
  Foreground_color_65k(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
  Foreground_color_16M(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
  Foreground_color_16M(ForegroundColor);
#endif
  Ellipse_Center_XY(XCenter,YCenter);
  Ellipse_Radius_RxRy(X_R,Y_R);
  Start_Right_Down_Curve();
  Check_2D_Busy(); 
}



void Draw_Right_Down_Curve_Fill
(
unsigned long ForegroundColor //ForegroundColor: Set Curve or Curve Fill color 
/*ForegroundColor Color dataformat :
ColorDepth_8bpp : R3G3B2、ColorDepth_16bpp : R5G6B5、ColorDepth_24bpp : R8G8B8*/
,unsigned short XCenter //coordinate X of Center
,unsigned short YCenter //coordinate Y of Center
,unsigned short X_R // Radius Width of Curve
,unsigned short Y_R // Radius Length of Curve
)
{
#ifdef MCU_8bit_ColorDepth_8bpp	
  Foreground_color_256(ForegroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_16bpp	
  Foreground_color_65k(ForegroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_24bpp	
  Foreground_color_16M(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_16bpp	
  Foreground_color_65k(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
  Foreground_color_16M(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
  Foreground_color_16M(ForegroundColor);
#endif
  Ellipse_Center_XY(XCenter,YCenter);
  Ellipse_Radius_RxRy(X_R,Y_R);
  Start_Right_Down_Curve_Fill();
  Check_2D_Busy(); 
}



void Draw_Right_Up_Curve
(
unsigned long ForegroundColor //ForegroundColor: Set Curve or Curve Fill color 
/*ForegroundColor Color dataformat :
ColorDepth_8bpp : R3G3B2、ColorDepth_16bpp : R5G6B5、ColorDepth_24bpp : R8G8B8*/
,unsigned short XCenter //coordinate X of Center
,unsigned short YCenter //coordinate Y of Center
,unsigned short X_R // Radius Width of Curve
,unsigned short Y_R // Radius Length of Curve
)
{
#ifdef MCU_8bit_ColorDepth_8bpp	
  Foreground_color_256(ForegroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_16bpp	
  Foreground_color_65k(ForegroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_24bpp	
  Foreground_color_16M(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_16bpp	
  Foreground_color_65k(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
  Foreground_color_16M(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
  Foreground_color_16M(ForegroundColor);
#endif
  Ellipse_Center_XY(XCenter,YCenter);
  Ellipse_Radius_RxRy(X_R,Y_R);
  Start_Right_Up_Curve();
  Check_2D_Busy(); 
}



void Draw_Right_Up_Curve_Fill
(
unsigned long ForegroundColor //ForegroundColor: Set Curve or Curve Fill color 
/*ForegroundColor Color dataformat :
ColorDepth_8bpp : R3G3B2、ColorDepth_16bpp : R5G6B5、ColorDepth_24bpp : R8G8B8*/
,unsigned short XCenter //coordinate X of Center
,unsigned short YCenter //coordinate Y of Center
,unsigned short X_R // Radius Width of Curve
,unsigned short Y_R // Radius Length of Curve
)
{
#ifdef MCU_8bit_ColorDepth_8bpp	
  Foreground_color_256(ForegroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_16bpp	
  Foreground_color_65k(ForegroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_24bpp	
  Foreground_color_16M(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_16bpp	
  Foreground_color_65k(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
  Foreground_color_16M(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
  Foreground_color_16M(ForegroundColor);
#endif
  Ellipse_Center_XY(XCenter,YCenter);
  Ellipse_Radius_RxRy(X_R,Y_R);
  Start_Right_Up_Curve_Fill();
  Check_2D_Busy();
}



void Draw_Left_Down_Curve
(
unsigned long ForegroundColor //ForegroundColor: Set Curve or Curve Fill color 
/*ForegroundColor Color dataformat :
ColorDepth_8bpp : R3G3B2、ColorDepth_16bpp : R5G6B5、ColorDepth_24bpp : R8G8B8*/
,unsigned short XCenter //coordinate X of Center
,unsigned short YCenter //coordinate Y of Center
,unsigned short X_R // Radius Width of Curve
,unsigned short Y_R // Radius Length of Curve
)
{
#ifdef MCU_8bit_ColorDepth_8bpp	
  Foreground_color_256(ForegroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_16bpp	
  Foreground_color_65k(ForegroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_24bpp	
  Foreground_color_16M(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_16bpp	
  Foreground_color_65k(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
  Foreground_color_16M(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
  Foreground_color_16M(ForegroundColor);
#endif
  Ellipse_Center_XY(XCenter,YCenter);
  Ellipse_Radius_RxRy(X_R,Y_R);
  Start_Left_Down_Curve();
  Check_2D_Busy(); 
}



void Draw_Left_Down_Curve_Fill
(
unsigned long ForegroundColor //ForegroundColor: Set Curve or Curve Fill color 
/*ForegroundColor Color dataformat :
ColorDepth_8bpp : R3G3B2、ColorDepth_16bpp : R5G6B5、ColorDepth_24bpp : R8G8B8*/
,unsigned short XCenter //coordinate X of Center
,unsigned short YCenter //coordinate Y of Center
,unsigned short X_R // Radius Width of Curve
,unsigned short Y_R // Radius Length of Curve
)
{
#ifdef MCU_8bit_ColorDepth_8bpp	
  Foreground_color_256(ForegroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_16bpp	
  Foreground_color_65k(ForegroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_24bpp	
  Foreground_color_16M(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_16bpp	
  Foreground_color_65k(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
  Foreground_color_16M(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
  Foreground_color_16M(ForegroundColor);
#endif
  Ellipse_Center_XY(XCenter,YCenter);
  Ellipse_Radius_RxRy(X_R,Y_R);
  Start_Left_Down_Curve_Fill();
  Check_2D_Busy(); 
}


void Draw_Square
(
unsigned long ForegroundColor 
/*ForegroundColor: Set Curve or Curve Fill color. ForegroundColor Color dataformat :
ColorDepth_8bpp : R3G3B2、ColorDepth_16bpp : R5G6B5、ColorDepth_24bpp : R8G8B8*/
,unsigned short X1 //X of point1 coordinate
,unsigned short Y1 //Y of point1 coordinate
,unsigned short X2 //X of point2 coordinate
,unsigned short Y2 //Y of point2 coordinate
)
{
#ifdef MCU_8bit_ColorDepth_8bpp	
  Foreground_color_256(ForegroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_16bpp	
  Foreground_color_65k(ForegroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_24bpp	
  Foreground_color_16M(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_16bpp	
  Foreground_color_65k(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
  Foreground_color_16M(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
  Foreground_color_16M(ForegroundColor);
#endif
  Square_Start_XY(X1,Y1);
  Square_End_XY(X2,Y2);
  Start_Square();
  Check_2D_Busy(); 
}


void Draw_Square_Fill
(
unsigned long ForegroundColor 
/*ForegroundColor: Set Curve or Curve Fill color. ForegroundColor Color dataformat :
ColorDepth_8bpp : R3G3B2、ColorDepth_16bpp : R5G6B5、ColorDepth_24bpp : R8G8B8*/
,unsigned short X1 //X of point1 coordinate
,unsigned short Y1 //Y of point1 coordinate
,unsigned short X2 //X of point2 coordinate
,unsigned short Y2 //Y of point2 coordinate
)
{
#ifdef MCU_8bit_ColorDepth_8bpp	
  Foreground_color_256(ForegroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_16bpp	
  Foreground_color_65k(ForegroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_24bpp	
  Foreground_color_16M(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_16bpp	
  Foreground_color_65k(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
  Foreground_color_16M(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
  Foreground_color_16M(ForegroundColor);
#endif
  Square_Start_XY(X1,Y1);
  Square_End_XY(X2,Y2);
  Start_Square_Fill();
  Check_2D_Busy(); 
}



void Draw_Circle_Square
(
unsigned long ForegroundColor 
/*ForegroundColor : Set Circle Square color. ForegroundColor Color dataformat :
ColorDepth_8bpp : R3G3B2、ColorDepth_16bpp : R5G6B5、ColorDepth_24bpp : R8G8B8*/
,unsigned short X1 //X of point1 coordinate
,unsigned short Y1 //Y of point1 coordinate
,unsigned short X2 //X of point2 coordinate
,unsigned short Y2 //Y of point2 coordinate
,unsigned short X_R //Radius Width of Circle Square
,unsigned short Y_R //Radius Length of Circle Square
)
{
#ifdef MCU_8bit_ColorDepth_8bpp	
  Foreground_color_256(ForegroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_16bpp	
  Foreground_color_65k(ForegroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_24bpp	
  Foreground_color_16M(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_16bpp	
  Foreground_color_65k(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
  Foreground_color_16M(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
  Foreground_color_16M(ForegroundColor);
#endif 
  Square_Start_XY(X1,Y1);
  Square_End_XY(X2,Y2); 
  Circle_Square_Radius_RxRy(X_R,Y_R);
  Start_Circle_Square();
  Check_2D_Busy(); 
}




void Draw_Circle_Square_Fill
(
unsigned long ForegroundColor 
/*ForegroundColor : Set Circle Square color. ForegroundColor Color dataformat :
ColorDepth_8bpp : R3G3B2、ColorDepth_16bpp : R5G6B5、ColorDepth_24bpp : R8G8B8*/
,unsigned short X1 //X of point1 coordinate
,unsigned short Y1 //Y of point1 coordinate
,unsigned short X2 //X of point2 coordinate
,unsigned short Y2 //Y of point2 coordinate
,unsigned short X_R //Radius Width of Circle Square
,unsigned short Y_R //Radius Length of Circle Square
)
{

#ifdef MCU_8bit_ColorDepth_8bpp	
  Foreground_color_256(ForegroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_16bpp	
  Foreground_color_65k(ForegroundColor);
#endif
#ifdef MCU_8bit_ColorDepth_24bpp	
  Foreground_color_16M(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_16bpp	
  Foreground_color_65k(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
  Foreground_color_16M(ForegroundColor);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
  Foreground_color_16M(ForegroundColor);
#endif
  Square_Start_XY(X1,Y1);
  Square_End_XY(X2,Y2); 
  Circle_Square_Radius_RxRy(X_R,Y_R);
  Start_Circle_Square_Fill();
  Check_2D_Busy(); 
}



void BTE_Pattern_Fill
(
unsigned char P_8x8_or_16x16 //0 : use 8x8 Icon , 1 : use 16x16 Icon.
,unsigned long S0_Addr //Start address of Source 0
,unsigned short S0_W //image width of Source 0 (recommend = canvas image width)
,unsigned short XS0 // coordinate X of Source 0
,unsigned short YS0 // coordinate Y of Source 0
,unsigned long S1_Addr //Start address of Source 1
,unsigned short S1_W //image width of Source 1 (recommend = canvas image width)
,unsigned short XS1 //coordinate X of Source 1
,unsigned short YS1 //coordinate Y of Source 1
,unsigned long Des_Addr // start address of Destination
,unsigned short Des_W //image width of Destination (recommend = canvas image width)
, unsigned short XDes //coordinate X of Destination
,unsigned short YDes //coordinate Y of Destination
,unsigned int ROP_Code 
/*ROP_Code :
   0000b		0(Blackness)
   0001b		~S0!E~S1 or ~(S0+S1)
   0010b		~S0!ES1
   0011b		~S0
   0100b		S0!E~S1
   0101b		~S1
   0110b		S0^S1
   0111b		~S0 + ~S1 or ~(S0 + S1)
   1000b		S0!ES1
   1001b		~(S0^S1)
   1010b		S1
   1011b		~S0+S1
   1100b		S0
   1101b		S0+~S1
   1110b		S0+S1
   1111b		1(whiteness)*/
,unsigned short X_W //Width of BTE Winodw
,unsigned short Y_H //Length of BTE Winodw
)
{
  if(P_8x8_or_16x16 == 0)
  {
  Pattern_Format_8X8();
   }
  if(P_8x8_or_16x16 == 1)
  {														    
  Pattern_Format_16X16();
  }	  
  BTE_S0_Memory_Start_Address(S0_Addr);
  BTE_S0_Image_Width(S0_W);
  BTE_S0_Window_Start_XY(XS0,YS0);

  BTE_S1_Memory_Start_Address(S1_Addr);
  BTE_S1_Image_Width(S1_W); 
  BTE_S1_Window_Start_XY(XS1,YS1);

  BTE_Destination_Memory_Start_Address(Des_Addr);
  BTE_Destination_Image_Width(Des_W);
  BTE_Destination_Window_Start_XY(XDes,YDes);	
   
  BTE_ROP_Code(ROP_Code);	
  BTE_Operation_Code(0x06); //BTE Operation: Pattern Fill with ROP.
  BTE_Window_Size(X_W,Y_H); 
  BTE_Enable();
  Check_BTE_Busy();
}

void BTE_Pattern_Fill_With_Chroma_key
(
unsigned char P_8x8_or_16x16 //0 : use 8x8 Icon , 1 : use 16x16 Icon.
,unsigned long S0_Addr //Start address of Source 0
,unsigned short S0_W //image width of Source 0 (recommend = canvas image width)
,unsigned short XS0 //coordinate X of Source 0
,unsigned short YS0 //coordinate Y of Source 0
,unsigned long S1_Addr //Start address of Source 1
,unsigned short S1_W //image width of Source 1 (recommend = canvas image width)
,unsigned short XS1 //coordinate X of Source 1
,unsigned short YS1 //coordinate Y of Source 1
,unsigned long Des_Addr //Des_Addr : start address of Destination
,unsigned short Des_W //Des_W : image width of Destination (recommend = canvas image width)
,unsigned short XDes //coordinate X of Destination
,unsigned short YDes //coordinate Y of Destination
,unsigned int ROP_Code
/*ROP_Code :
   0000b		0(Blackness)
   0001b		~S0!E~S1 or ~(S0+S1)
   0010b		~S0!ES1
   0011b		~S0
   0100b		S0!E~S1
   0101b		~S1
   0110b		S0^S1
   0111b		~S0 + ~S1 or ~(S0 + S1)
   1000b		S0!ES1
   1001b		~(S0^S1)
   1010b		S1
   1011b		~S0+S1
   1100b		S0
   1101b		S0+~S1
   1110b		S0+S1
   1111b		1(whiteness)*/
,unsigned long Background_color //Transparent color
,unsigned short X_W //Width of BTE Window
,unsigned short Y_H //Length of BTE Window
)
{
#ifdef MCU_8bit_ColorDepth_8bpp	
  Background_color_256(Background_color); 
#endif
#ifdef MCU_8bit_ColorDepth_16bpp	
  Background_color_65k(Background_color); 
#endif
#ifdef MCU_8bit_ColorDepth_24bpp	
  Background_color_16M(Background_color); 
#endif
#ifdef MCU_16bit_ColorDepth_16bpp	
  Background_color_65k(Background_color); 
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
  Background_color_16M(Background_color); 
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
  Background_color_16M(Background_color); 
#endif
  if(P_8x8_or_16x16 == 0)
  {
  Pattern_Format_8X8();
   }
  if(P_8x8_or_16x16 == 1)
  {														    
  Pattern_Format_16X16();
  }	  
  BTE_S0_Memory_Start_Address(S0_Addr);
  BTE_S0_Image_Width(S0_W);
  BTE_S0_Window_Start_XY(XS0,YS0);

  BTE_S1_Memory_Start_Address(S1_Addr);
  BTE_S1_Image_Width(S1_W); 
  BTE_S1_Window_Start_XY(XS1,YS1);

  BTE_Destination_Memory_Start_Address(Des_Addr);
  BTE_Destination_Image_Width(Des_W);
  BTE_Destination_Window_Start_XY(XDes,YDes);	
   
  BTE_ROP_Code(ROP_Code);	
  BTE_Operation_Code(0x07); //BTE Operation: Pattern Fill with Chroma key.
  BTE_Window_Size(X_W,Y_H); 
  BTE_Enable();
  Check_BTE_Busy();
}

void BTE_Memory_Copy
(
unsigned long S0_Addr //Start address of Source 0
,unsigned short S0_W //image width of Source 0 (recommend = canvas image width)
,unsigned short XS0 //coordinate X of Source 0
,unsigned short YS0 //coordinate Y of Source 0
,unsigned long S1_Addr //Start address of Source 1
,unsigned short S1_W //image width of Source 1 (recommend = canvas image width)
,unsigned short XS1 //coordinate X of Source 1
,unsigned short YS1 //coordinate Y of Source 1
,unsigned long Des_Addr //start address of Destination
,unsigned short Des_W //image width of Destination (recommend = canvas image width)
,unsigned short XDes //coordinate X of Destination
,unsigned short YDes //coordinate Y of Destination
,unsigned int ROP_Code
/*ROP_Code :
   0000b		0(Blackness)
   0001b		~S0!E~S1 or ~(S0+S1)
   0010b		~S0!ES1
   0011b		~S0
   0100b		S0!E~S1
   0101b		~S1
   0110b		S0^S1
   0111b		~S0 + ~S1 or ~(S0 + S1)
   1000b		S0!ES1
   1001b		~(S0^S1)
   1010b		S1
   1011b		~S0+S1
   1100b		S0
   1101b		S0+~S1
   1110b		S0+S1
   1111b		1(whiteness)*/
,unsigned short X_W //X_W : Width of BTE Window
,unsigned short Y_H //Y_H : Length of BTE Window
)
{
  BTE_S0_Memory_Start_Address(S0_Addr);
  BTE_S0_Image_Width(S0_W);
  BTE_S0_Window_Start_XY(XS0,YS0);

  BTE_S1_Memory_Start_Address(S1_Addr);
  BTE_S1_Image_Width(S1_W); 
  BTE_S1_Window_Start_XY(XS1,YS1);

  BTE_Destination_Memory_Start_Address(Des_Addr);
  BTE_Destination_Image_Width(Des_W);
  BTE_Destination_Window_Start_XY(XDes,YDes);	
   
  BTE_ROP_Code(ROP_Code);	
  BTE_Operation_Code(0x02); //BTE Operation: Memory copy (move) with ROP.
  BTE_Window_Size(X_W,Y_H); 
  BTE_Enable();
  Check_BTE_Busy();
}






void BTE_Memory_Copy_Chroma_key
(
unsigned long S0_Addr //Start address of Source 0
,unsigned short S0_W //image width of Source 0 (recommend = canvas image width) 
,unsigned short XS0 //coordinate X of Source 0
,unsigned short YS0 //coordinate Y of Source 0
,unsigned long Des_Addr //start address of Destination
,unsigned short Des_W //image width of Destination (recommend = canvas image width) 
,unsigned short XDes //coordinate X of Destination
,unsigned short YDes //coordinate Y of Destination
,unsigned long Background_color // transparent color
,unsigned short X_W //Width of BTE Window
,unsigned short Y_H //Length of BTE Window
)
{

#ifdef MCU_8bit_ColorDepth_8bpp	
  Background_color_256(Background_color); 
#endif
#ifdef MCU_8bit_ColorDepth_16bpp	
  Background_color_65k(Background_color); 
#endif
#ifdef MCU_8bit_ColorDepth_24bpp	
  Background_color_16M(Background_color); 
#endif
#ifdef MCU_16bit_ColorDepth_16bpp	
  Background_color_65k(Background_color); 
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
  Background_color_16M(Background_color); 
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
  Background_color_16M(Background_color); 
#endif
  BTE_S0_Memory_Start_Address(S0_Addr);
  BTE_S0_Image_Width(S0_W);
  BTE_S0_Window_Start_XY(XS0,YS0);	

  BTE_Destination_Memory_Start_Address(Des_Addr);
  BTE_Destination_Image_Width(Des_W);
  BTE_Destination_Window_Start_XY(XDes,YDes);
   
  BTE_Operation_Code(0x05);	//BTE Operation: Memory copy (move) with chroma keying (w/o ROP)
  BTE_Window_Size(X_W,Y_H); 
  BTE_Enable();
  Check_BTE_Busy();
}


void BTE_MCU_Write_MCU_8bit
(
unsigned long S1_Addr //Start address of Source 1
,unsigned short S1_W //image width of Source 1 (recommend = canvas image width)
,unsigned short XS1 //coordinate X of Source 1
,unsigned short YS1 //coordinate Y of Source 1
,unsigned long Des_Addr //start address of Destination
,unsigned short Des_W //image width of Destination (recommend = canvas image width)
,unsigned short XDes //coordinate X of Destination
,unsigned short YDes //coordinate Y of Destination
,unsigned int ROP_Code
/*ROP_Code :
   0000b		0(Blackness)
   0001b		~S0!E~S1 or ~(S0+S1)
   0010b		~S0!ES1
   0011b		~S0
   0100b		S0!E~S1
   0101b		~S1
   0110b		S0^S1
   0111b		~S0 + ~S1 or ~(S0 + S1)
   1000b		S0!ES1
   1001b		~(S0^S1)
   1010b		S1
   1011b		~S0+S1
   1100b		S0
   1101b		S0+~S1
   1110b		S0+S1
   1111b		1(whiteness)*/
,unsigned short X_W // Width of BTE Window
,unsigned short Y_H // Length of BTE Window
,const unsigned char *data // 8-bit data
)
{	
  unsigned short i,j;

  BTE_S1_Memory_Start_Address(S1_Addr);
  BTE_S1_Image_Width(S1_W); 
  BTE_S1_Window_Start_XY(XS1,YS1);

  BTE_Destination_Memory_Start_Address(Des_Addr);
  BTE_Destination_Image_Width(Des_W);
  BTE_Destination_Window_Start_XY(XDes,YDes);
  
  BTE_Window_Size(X_W,Y_H);
  BTE_ROP_Code(ROP_Code);
  BTE_Operation_Code(0x00);		//BTE Operation: MPU Write with ROP.
  BTE_Enable();
  LCD_CmdWrite(0x04);			//Memory Data Read/Write Port
#ifdef MCU_8bit_ColorDepth_8bpp	
    			 		
	for(i=0;i< Y_H;i++)
    {	
	  for(j=0;j< (X_W);j++)
 	  {
	   Check_Mem_WR_FIFO_not_Full();
	   LCD_DataWrite(*data);
	   data++;
	  }
    }
  Check_Mem_WR_FIFO_Empty();
#endif

#ifdef MCU_8bit_ColorDepth_16bpp	
	for(i=0;i< Y_H;i++)
    {	
	  for(j=0;j< (X_W*2);j++)
 	  {
	   Check_Mem_WR_FIFO_not_Full();
	   LCD_DataWrite(*data);
	   data++;
	  }
    }
  Check_Mem_WR_FIFO_Empty();
#endif

#ifdef MCU_8bit_ColorDepth_24bpp	
	for(i=0;i< Y_H;i++)
    {	
	  for(j=0;j< (X_W*3);j++)
 	  {
	   Check_Mem_WR_FIFO_not_Full();
	   LCD_DataWrite(*data);
	   data++;
	  }
    }
  Check_Mem_WR_FIFO_Empty();
#endif

}


void BTE_MCU_Write_MCU_16bit
(
unsigned long S1_Addr //Start address of Source 1
,unsigned short S1_W //image width of Source 1 (recommend = canvas image width)
,unsigned short XS1 //coordinate X of Source 1
,unsigned short YS1 //coordinate Y of Source 1
,unsigned long Des_Addr //start address of Destination
,unsigned short Des_W //image width of Destination (recommend = canvas image width)
,unsigned short XDes //coordinate X of Destination
,unsigned short YDes //coordinate Y of Destination
,unsigned int ROP_Code
/*ROP_Code :
   0000b		0(Blackness)
   0001b		~S0!E~S1 or ~(S0+S1)
   0010b		~S0!ES1
   0011b		~S0
   0100b		S0!E~S1
   0101b		~S1
   0110b		S0^S1
   0111b		~S0 + ~S1 or ~(S0 + S1)
   1000b		S0!ES1
   1001b		~(S0^S1)
   1010b		S1
   1011b		~S0+S1
   1100b		S0
   1101b		S0+~S1
   1110b		S0+S1
   1111b		1(whiteness)*/
,unsigned short X_W // Width of BTE Window
,unsigned short Y_H // Length of BTE Window
,const unsigned short *data // 16-bit data 
)
{	
  unsigned short i,j;

  BTE_S1_Memory_Start_Address(S1_Addr);
  BTE_S1_Image_Width(S1_W); 
  BTE_S1_Window_Start_XY(XS1,YS1);

  BTE_Destination_Memory_Start_Address(Des_Addr);
  BTE_Destination_Image_Width(Des_W);
  BTE_Destination_Window_Start_XY(XDes,YDes);
  
  BTE_Window_Size(X_W,Y_H);
  BTE_ROP_Code(ROP_Code);
  BTE_Operation_Code(0x00);		//BTE Operation: MPU Write with ROP.
  BTE_Enable();
  LCD_CmdWrite(0x04);				 		//Memory Data Read/Write Port
#ifdef MCU_16bit_ColorDepth_16bpp	
	for(i=0;i< Y_H;i++)
    {	
	  for(j=0;j< (X_W);j++)
 	  {
	   Check_Mem_WR_FIFO_not_Full();
	   LCD_DataWrite((*data));
	   data++;
	  }
    }
  Check_Mem_WR_FIFO_Empty();
#endif

#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
	for(i=0;i< Y_H;i++)
    {	
	  for(j=0;j< (X_W/2);j++)
 	  {
	    Check_Mem_WR_FIFO_not_Full();
	    LCD_DataWrite(*data);
	    data++;
	    Check_Mem_WR_FIFO_not_Full();
	    LCD_DataWrite(*data);
	    data++;
	    Check_Mem_WR_FIFO_not_Full();
	    LCD_DataWrite(*data);
	    data++;
	  }
    }
  Check_Mem_WR_FIFO_Empty();
#endif
  

#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
	for(i=0;i< Y_H;i++)
    {	
	  for(j=0;j< (X_W);j++)
 	  {
	    Check_Mem_WR_FIFO_not_Full();
	    LCD_DataWrite(*data);
	    data++;
	    Check_Mem_WR_FIFO_not_Full();
	    LCD_DataWrite(*data);
	    data++;
	  }
    }
  Check_Mem_WR_FIFO_Empty();
#endif

  Check_BTE_Busy();
	
}






void BTE_MCU_Write_Chroma_key_MCU_8bit
(
unsigned long Des_Addr //start address of Destination
,unsigned short Des_W //image width of Destination (recommend = canvas image width)
,unsigned short XDes //coordinate X of Destination
,unsigned short YDes //coordinate Y of Destination
,unsigned long Background_color //transparency color
,unsigned short X_W //Width of BTE Window
,unsigned short Y_H //Length of BTE Window
,const unsigned char *data // 8-bit data
)
{
  unsigned short i,j;
	
#ifdef MCU_8bit_ColorDepth_8bpp	
  Background_color_256(Background_color);
#endif

#ifdef MCU_8bit_ColorDepth_16bpp	
  Background_color_65k(Background_color);
#endif

#ifdef MCU_8bit_ColorDepth_24bpp	
  Background_color_16M(Background_color);
#endif
  BTE_Destination_Memory_Start_Address(Des_Addr);
  BTE_Destination_Image_Width(Des_W);
  BTE_Destination_Window_Start_XY(XDes,YDes);
  
  BTE_Window_Size(X_W,Y_H);
  BTE_Operation_Code(0x04);		//BTE Operation: MPU Write w/ chroma keying (w/o ROP)
  BTE_Enable();
  
  LCD_CmdWrite(0x04);				 		//Memory Data Read/Write Port

#ifdef MCU_8bit_ColorDepth_8bpp	
    			 		
	for(i=0;i< Y_H;i++)
    {	
	  for(j=0;j< (X_W);j++)
 	  {
	   Check_Mem_WR_FIFO_not_Full();
	   LCD_DataWrite(*data);
	   data++;
	  }
    }
  Check_Mem_WR_FIFO_Empty();
#endif

#ifdef MCU_8bit_ColorDepth_16bpp	
	for(i=0;i< Y_H;i++)
    {	
	  for(j=0;j< (X_W*2);j++)
 	  {
	   Check_Mem_WR_FIFO_not_Full();
	   LCD_DataWrite(*data);
	   data++;
	  }
    }
  Check_Mem_WR_FIFO_Empty();
#endif

#ifdef MCU_8bit_ColorDepth_24bpp	
	for(i=0;i< Y_H;i++)
    {	
	  for(j=0;j< (X_W*3);j++)
 	  {
	   Check_Mem_WR_FIFO_not_Full();
	   LCD_DataWrite(*data);
	   data++;
	  }
    }
  Check_Mem_WR_FIFO_Empty();
#endif
	Check_BTE_Busy();
}



void BTE_MCU_Write_Chroma_key_MCU_16bit
(
unsigned long Des_Addr //start address of Destination
,unsigned short Des_W //image width of Destination (recommend = canvas image width)
,unsigned short XDes //coordinate X of Destination
,unsigned short YDes //coordinate Y of Destination
,unsigned long Background_color //transparency color
,unsigned short X_W //Width of BTE Window
,unsigned short Y_H //Length of BTE Window
,const unsigned short *data // 16-bit data
)
{	
  unsigned int i,j;

  
#ifdef MCU_16bit_ColorDepth_16bpp				//setting in UserDef.h
  Background_color_65k(Background_color);
#endif	
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1		//setting in UserDef.h
  Background_color_16M(Background_color);
#endif	
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2		//setting in UserDef.h
  Background_color_16M(Background_color);
#endif
  BTE_Destination_Memory_Start_Address(Des_Addr);
  BTE_Destination_Image_Width(Des_W);
  BTE_Destination_Window_Start_XY(XDes,YDes);
  
  BTE_Window_Size(X_W,Y_H);
  BTE_Operation_Code(0x04);		//BTE Operation: MPU Write with chroma keying (w/o ROP)
  BTE_Enable();
  LCD_CmdWrite(0x04);			//Memory Data Read/Write Port
#ifdef MCU_16bit_ColorDepth_16bpp				//setting in UserDef.h
  				 		
	for(i=0;i< Y_H;i++)
    {	
	  for(j=0;j< X_W;j++)
 	  {
	    Check_Mem_WR_FIFO_not_Full();
		  LCD_DataWrite(*data);  
	    data++;
	  }
    }
  Check_Mem_WR_FIFO_Empty();
#endif	

#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1		//setting in UserDef.h
	for(i=0;i< Y_H;i++)
    {	
	  for(j=0;j< (X_W/2);j++)
 	  {
	    Check_Mem_WR_FIFO_not_Full();
		  LCD_DataWrite(*data);  
	    data++;
		  Check_Mem_WR_FIFO_not_Full();
		  LCD_DataWrite(*data);  
	    data++;
		  Check_Mem_WR_FIFO_not_Full();
		  LCD_DataWrite(*data);  
	    data++;
	  }
    }
  Check_Mem_WR_FIFO_Empty(); 
#endif	
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2		//setting in UserDef.h
	for(i=0;i< Y_H;i++)
    {	
	  for(j=0;j< (X_W);j++)
 	  {
	    Check_Mem_WR_FIFO_not_Full();
		  LCD_DataWrite(*data);  
	    data++;
		  Check_Mem_WR_FIFO_not_Full();
		  LCD_DataWrite(*data);  
	    data++;

	  }
    }
  Check_Mem_WR_FIFO_Empty();
#endif

	Check_BTE_Busy();

}



/*
No support 24bpp color depth 
S0_Addr : Source 0 Start address
S0_W : Source 0 image width(recommend = canvas image width) 
XS0 : Source 0 coordinate of X
YS0 : Source 0 coordinate of Y
Des_Addr : Destination start address
Des_W :	 Destination image width(recommend = canvas image width) 
XDes : Destination coordinate of X
YDes : Destination coordinate of Y
X_W : BTE Window Size of X
Y_H : BTE Window Size of Y
Foreground_color : The source (1bit map picture) map data 1 translate to Foreground color by color expansion
Background_color : The source (1bit map picture) map data 0 translate to background color by color expansion
*/
void BTE_Memory_Copy_ColorExpansion(unsigned long S0_Addr,unsigned short S0_W,unsigned short XS0,unsigned short YS0,unsigned long Des_Addr,unsigned short Des_W, unsigned short XDes,unsigned short YDes,unsigned short X_W,unsigned short Y_H,unsigned long Foreground_color,unsigned long Background_color)
{

#ifdef MCU_8bit_ColorDepth_8bpp
    Foreground_color_256(Foreground_color);
    Background_color_256(Background_color);
	BTE_ROP_Code(7);	//MCU data bus width: 8bit, ROP Code=7
#endif
#ifdef MCU_8bit_ColorDepth_16bpp
    Foreground_color_65k(Foreground_color);
    Background_color_65k(Background_color);
	BTE_ROP_Code(15);	//MCU data bus width: 8bit, ROP Code=7
#endif
#ifdef MCU_16bit_ColorDepth_16bpp
    Foreground_color_65k(Foreground_color);
    Background_color_65k(Background_color);
	BTE_ROP_Code(15);	//MCU data bus width: 16bit, ROP Code=7
#endif


  BTE_S0_Memory_Start_Address(S0_Addr);
  BTE_S0_Image_Width(S0_W);
  BTE_S0_Window_Start_XY(XS0,YS0);
  
  BTE_Destination_Memory_Start_Address(Des_Addr);
  BTE_Destination_Image_Width(Des_W);
  BTE_Destination_Window_Start_XY(XDes,YDes);
   
  BTE_Window_Size(X_W,Y_H);
  BTE_Operation_Code(0xE);		//BTE Operation: Memory write with Color Expansion (w/o ROP)

#ifdef MCU_8bit_ColorDepth_8bpp
  BTE_Enable();
  Check_BTE_Busy();
#endif
#ifdef MCU_8bit_ColorDepth_16bpp
  BTE_Enable();
  Check_BTE_Busy();
#endif
#ifdef MCU_16bit_ColorDepth_16bpp
  BTE_Enable();
  Check_BTE_Busy();
#endif	

}


/*
No support 24bpp color depth
S0_Addr : Source 0 Start address
S0_W : Source 0 image width(recommend = canvas image width) 
XS0 : Source 0 coordinate of X
YS0 : Source 0 coordinate of Y
Des_Addr : Destination start address
Des_W :	 Destination image width(recommend = canvas image width) 
XDes : Destination coordinate of X
YDes : Destination coordinate of Y
X_W : BTE Window Size of X
Y_H : BTE Window Size of Y
Foreground_color : The source (1bit map picture) map data 1 translate to Foreground color by color expansion
*/
void BTE_Memory_Copy_ColorExpansion_Chroma_key(unsigned long S0_Addr,unsigned short S0_W,unsigned short XS0,unsigned short YS0,unsigned long Des_Addr,unsigned short Des_W, unsigned short XDes,unsigned short YDes,unsigned short X_W,unsigned short Y_H,unsigned long Foreground_color)
{
#ifdef MCU_8bit_ColorDepth_8bpp
    Foreground_color_256(Foreground_color);
	BTE_ROP_Code(7);	//MCU data bus width: 8bit, ROP Code=7
#endif
#ifdef MCU_8bit_ColorDepth_16bpp
    Foreground_color_65k(Foreground_color);
	BTE_ROP_Code(15);	//MCU data bus width: 8bit, ROP Code=7
#endif
#ifdef MCU_16bit_ColorDepth_16bpp
    Foreground_color_65k(Foreground_color);
	BTE_ROP_Code(15);	//MCU data bus width: 16bit, ROP Code=7
#endif

  BTE_S0_Memory_Start_Address(S0_Addr);
  BTE_S0_Image_Width(S0_W);
  BTE_S0_Window_Start_XY(XS0,YS0);

  BTE_Destination_Memory_Start_Address(Des_Addr);
  BTE_Destination_Image_Width(Des_W);
  BTE_Destination_Window_Start_XY(XDes,YDes);
   
  BTE_Window_Size(X_W,Y_H);
  BTE_Operation_Code(0xF);		//BTE Operation: Memory write with Color Expansion and chroma keying (w/o ROP)
	
#ifdef MCU_8bit_ColorDepth_8bpp
  BTE_Enable();
  Check_BTE_Busy();
#endif
#ifdef MCU_8bit_ColorDepth_16bpp
  BTE_Enable();
  Check_BTE_Busy();
#endif
#ifdef MCU_16bit_ColorDepth_16bpp
  BTE_Enable();
  Check_BTE_Busy();
#endif	

}


void BTE_MCU_Write_ColorExpansion_MCU_8bit
(
unsigned long Des_Addr //start address of Destination
,unsigned short Des_W //image width of Destination (recommend = canvas image width)
,unsigned short XDes //coordinate X of Destination
,unsigned short YDes //coordinate Y of Destination
,unsigned short X_W //Width of BTE Window
,unsigned short Y_H //Length of BTE Window
,unsigned long Foreground_color 
/*Foreground_color : The source (1bit map picture) map data 1 translate to Foreground color by color expansion*/
,unsigned long Background_color 
/*Background_color : The source (1bit map picture) map data 0 translate to Foreground color by color expansion*/
,const unsigned char *data // 8-bit data
)
{    
    unsigned short i,j;
	
	/* set BTE Parameters and Run */
#ifdef MCU_8bit_ColorDepth_8bpp
    Foreground_color_256(Foreground_color);
    Background_color_256(Background_color);
#endif
#ifdef MCU_8bit_ColorDepth_16bpp
    Foreground_color_65k(Foreground_color);
    Background_color_65k(Background_color);
#endif
#ifdef MCU_8bit_ColorDepth_24bpp
    Foreground_color_16M(Foreground_color);
    Background_color_16M(Background_color);
#endif 

	BTE_Destination_Memory_Start_Address(Des_Addr);
	BTE_Destination_Image_Width(Des_W);
	BTE_Destination_Window_Start_XY(XDes,YDes);

	BTE_ROP_Code(7);	//MCU data bus width: 8bit, ROP Code=7
	BTE_Window_Size(X_W,Y_H);
	BTE_Operation_Code(0x8);		//BTE Operation: MPU Write with Color Expansion (w/o ROP)

	BTE_Enable();		
		
  LCD_CmdWrite(0x04);				 		//Memory Data Read/Write Port
    
	for(i=0;i< Y_H;i++)
    {	
	  for(j=0;j< X_W/8;j++)
 	  {
	    Check_Mem_WR_FIFO_not_Full();
		  LCD_DataWrite(*data);  
	    data++;
	  }
    }
	Check_Mem_WR_FIFO_Empty();
	  
	Check_BTE_Busy();


}

void BTE_MCU_Write_ColorExpansion_MCU_16bit
(
unsigned long Des_Addr //start address of Destination
,unsigned short Des_W //image width of Destination (recommend = canvas image width)
,unsigned short XDes //coordinate X of Destination
,unsigned short YDes //coordinate Y of Destination
,unsigned short X_W //Width of BTE Window
,unsigned short Y_H //Length of BTE Window
,unsigned long Foreground_color 
/*Foreground_color : The source (1bit map picture) map data 1 translate to Foreground color by color expansion*/
,unsigned long Background_color 
/*Background_color : The source (1bit map picture) map data 0 translate to Background color by color expansion*/
,const unsigned short *data //16-bit data　
)
{ 
  unsigned short i,j;


#ifdef MCU_16bit_ColorDepth_16bpp
	Data_Format_16b_16bpp();
    Foreground_color_65k(Foreground_color);
    Background_color_65k(Background_color);
	BTE_ROP_Code(15);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1
  Data_Format_16b_8bpp();
  Foreground_color_16M(Foreground_color);
  Background_color_16M(Background_color);
  BTE_ROP_Code(7);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2
  Data_Format_16b_8bpp();
  Foreground_color_16M(Foreground_color);
  Background_color_16M(Background_color);
  BTE_ROP_Code(7);
#endif

  BTE_Destination_Memory_Start_Address(Des_Addr);
  BTE_Destination_Image_Width(Des_W);
  BTE_Destination_Window_Start_XY(XDes,YDes);

  BTE_Window_Size(X_W,Y_H);
  BTE_Operation_Code(0x8);		//BTE Operation: MPU Write with Color Expansion (w/o ROP)
  BTE_Enable();
	

#ifdef MCU_16bit_ColorDepth_16bpp
  LCD_CmdWrite(0x04);				 		//Memory Data Read/Write Port  
	for(i=0;i< Y_H;i++)
    {	
	  for(j=0;j< X_W/16;j++)
 	  {
	    Check_Mem_WR_FIFO_not_Full();
		  LCD_DataWrite(*data);  
	    data++;
	  }
    }
  Check_Mem_WR_FIFO_Empty();
#endif

#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1
	LCD_CmdWrite(0x04);				 		//Memory Data Read/Write Port 
	for(i=0;i< Y_H;i++)
    {	
	  for(j=0;j< X_W/16;j++)
 	  {
	    Check_Mem_WR_FIFO_not_Full();
			LCD_DataWrite(*data>>8);  

	    Check_Mem_WR_FIFO_not_Full();
			LCD_DataWrite(*data);  
	    data++;
	  }
    }
	Check_Mem_WR_FIFO_Empty();
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2
	LCD_CmdWrite(0x04);				 		//Memory Data Read/Write Port 
	for(i=0;i< Y_H;i++)
    {	
	  for(j=0;j< X_W/16;j++)
 	  {
	    Check_Mem_WR_FIFO_not_Full();
			LCD_DataWrite(*data>>8);  

	    Check_Mem_WR_FIFO_not_Full();
			LCD_DataWrite(*data);  
	    data++;
	  }
    }
	Check_Mem_WR_FIFO_Empty();
#endif	

    Check_BTE_Busy();			

#ifdef MCU_16bit_ColorDepth_16bpp	
	Data_Format_16b_16bpp();
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
	Data_Format_16b_24bpp_mode1();
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
	Data_Format_16b_24bpp_mode2();
#endif
}




void BTE_MCU_Write_ColorExpansion_Chroma_key_MCU_8bit
(
unsigned long Des_Addr //start address of Destination
,unsigned short Des_W //image width of Destination (recommend = canvas image width) 
,unsigned short XDes //coordinate X of Destination
,unsigned short YDes //coordinate Y of Destination
,unsigned short X_W //Width of BTE Window
,unsigned short Y_H //Length of BTE Window
,unsigned long Foreground_color 
/*Foreground_color : The source (1bit map picture) map data 1 translate to Foreground color by color expansion*/
,const unsigned char *data //8-bit data
)
{
  unsigned short i,j;

//  Data_Format_8b_8bpp();
	
	/* set BTE Parameters and Run */
#ifdef MCU_8bit_ColorDepth_8bpp
  Foreground_color_256(Foreground_color);
#endif
#ifdef MCU_8bit_ColorDepth_16bpp
  Foreground_color_65k(Foreground_color);
#endif
#ifdef MCU_8bit_ColorDepth_24bpp
  Foreground_color_16M(Foreground_color);
#endif 

	BTE_Destination_Memory_Start_Address(Des_Addr);
	BTE_Destination_Image_Width(Des_W);
	BTE_Destination_Window_Start_XY(XDes,YDes);

	BTE_ROP_Code(7);	//MCU data bus width: 8bit, ROP Code=7
	BTE_Window_Size(X_W,Y_H);
	BTE_Operation_Code(0x9);		//BTE Operation: MPU Write with Color Expansion and chroma keying (w/o ROP)

	BTE_Enable();		
		
  LCD_CmdWrite(0x04);				 		//Memory Data Read/Write Port
    
	for(i=0;i< Y_H;i++)
  {	
	  for(j=0;j< X_W/8;j++)
 	  {
	    Check_Mem_WR_FIFO_not_Full();
		  LCD_DataWrite(*data);  
	    data++;
	  }
  }
	Check_Mem_WR_FIFO_Empty();
	  
	Check_BTE_Busy();


}



void BTE_MCU_Write_ColorExpansion_Chroma_key_MCU_16bit
(
unsigned long Des_Addr //start address of Destination
,unsigned short Des_W //image width of Destination (recommend = canvas image width)
,unsigned short XDes //coordinate X of Destination
,unsigned short YDes //coordinate Y of Destination
,unsigned short X_W //Width of BTE Window
,unsigned short Y_H //Length of BTE Window
,unsigned long Foreground_color
/*Foreground_color : The source (1bit map picture) map data 1 translate to Foreground color by color expansion*/
,const unsigned short *data //16-bit data
)
{
  unsigned short i,j;

#ifdef MCU_16bit_ColorDepth_16bpp
	Data_Format_16b_16bpp();
    Foreground_color_65k(Foreground_color);
	BTE_ROP_Code(15);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1
	Data_Format_16b_8bpp();
	Foreground_color_16M(Foreground_color);
	BTE_ROP_Code(7);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2
	Data_Format_16b_8bpp();
	Foreground_color_16M(Foreground_color);
	BTE_ROP_Code(7);
#endif

	BTE_Destination_Memory_Start_Address(Des_Addr);
	BTE_Destination_Image_Width(Des_W);
	BTE_Destination_Window_Start_XY(XDes,YDes);


	BTE_Window_Size(X_W,Y_H);
	BTE_Operation_Code(0x9);		//BTE Operation: MPU Write with Color Expansion and chroma keying (w/o ROP)
	BTE_Enable();	
 	LCD_CmdWrite(0x04);				 		//Memory Data Read/Write Port

#ifdef MCU_16bit_ColorDepth_16bpp						//setting in UserDef.h
	for(i=0;i< Y_H;i++)
  {	
	  for(j=0;j< X_W/16;j++)
 	  {
	    Check_Mem_WR_FIFO_not_Full();
		  LCD_DataWrite(*data);  
	    data++;
	  }
  }
	Check_Mem_WR_FIFO_Empty();
#endif	

#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1				//setting in UserDef.h
	for(i=0;i< Y_H;i++)
  {	
	  for(j=0;j< X_W/16;j++)
 	  {
	    Check_Mem_WR_FIFO_not_Full();
			LCD_DataWrite(*data>>8);  

	    Check_Mem_WR_FIFO_not_Full();
			LCD_DataWrite(*data);  
	    data++;
	  }
  }
	Check_Mem_WR_FIFO_Empty();
#endif	

#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2				//setting in UserDef.h
	for(i=0;i< Y_H;i++)
  {	
	  for(j=0;j< X_W/16;j++)
 	  {
	    Check_Mem_WR_FIFO_not_Full();
			LCD_DataWrite(*data>>8);  

	    Check_Mem_WR_FIFO_not_Full();
			LCD_DataWrite(*data);  
	    data++;
	  }
  }
	Check_Mem_WR_FIFO_Empty();
#endif
	Check_BTE_Busy();			


}



void BTE_Solid_Fill
(
unsigned long Des_Addr //start address of destination 
,unsigned short Des_W // image width of destination (recommend = canvas image width) 
, unsigned short XDes //coordinate X of destination 
,unsigned short YDes //coordinate Y of destination 
,unsigned long Foreground_color //Solid Fill color
,unsigned short X_W //Width of BTE Window
,unsigned short Y_H //Length of BTE Window 
)
{

#ifdef MCU_8bit_ColorDepth_8bpp	
  Foreground_color_256(Foreground_color);
#endif
#ifdef MCU_8bit_ColorDepth_16bpp	
  Foreground_color_65k(Foreground_color);
#endif
#ifdef MCU_8bit_ColorDepth_24bpp	
  Foreground_color_16M(Foreground_color);
#endif

#ifdef MCU_16bit_ColorDepth_16bpp	
  Foreground_color_65k(Foreground_color);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
  Foreground_color_16M(Foreground_color);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
  Foreground_color_16M(Foreground_color);
#endif
  BTE_Destination_Memory_Start_Address(Des_Addr);
  BTE_Destination_Image_Width(Des_W);
  BTE_Destination_Window_Start_XY(XDes,YDes);
  BTE_Window_Size(X_W,Y_H);
  BTE_Operation_Code(0x0c);		//BTE Operation: Solid Fill (w/o ROP)
  BTE_Enable();
  Check_BTE_Busy();  
}


/*
S0_Addr : Source 0 Start address
S0_W : Source 0 image width(recommend = canvas image width) 
XS0 : Source 0 coordinate of X
YS0 : Source 0 coordinate of Y
S1_Addr : Source 1 Start address
S1_W : Source 1 image width(recommend = canvas image width) 
XS1 : Source 1 coordinate of X
YS1 : Source 1 coordinate of Y
Des_Addr : Destination start address
Des_W :	 Destination image width(recommend = canvas image width) 
XDes : Destination coordinate of X
YDes : Destination coordinate of Y
X_W : BTE Window Size of X
Y_H : BTE Window Size of Y
alpha : Alpha Blending effect 0 ~ 32, Destination data = (Source 0 * (1- alpha)) + (Source 1 * alpha)
*/
void BTE_Alpha_Blending_Picture_Mode(unsigned long S0_Addr,unsigned short S0_W,unsigned short XS0,unsigned short YS0,unsigned long S1_Addr,unsigned short S1_W,unsigned short XS1,unsigned short YS1,unsigned long Des_Addr,unsigned short Des_W, unsigned short XDes,unsigned short YDes,unsigned short X_W,unsigned short Y_H,unsigned char alpha)
{
  BTE_S0_Memory_Start_Address(S0_Addr);
  BTE_S0_Image_Width(S0_W);
  BTE_S0_Window_Start_XY(XS0,YS0);

  BTE_S1_Memory_Start_Address(S1_Addr);
  BTE_S1_Image_Width(S1_W); 
  BTE_S1_Window_Start_XY(XS1,YS1);

  BTE_Destination_Memory_Start_Address(Des_Addr);
  BTE_Destination_Image_Width(Des_W);
  BTE_Destination_Window_Start_XY(XDes,YDes);

  BTE_Window_Size(X_W,Y_H);
  BTE_Operation_Code(0x0A);		//BTE Operation: Memory write with opacity (w/o ROP)
  BTE_Alpha_Blending_Effect(alpha);
  BTE_Enable();
  Check_BTE_Busy();
}






void DMA_24bit
(
unsigned char Clk //Clk : SPI Clock = System Clock /{(Clk)*2} , SPI CLK recommend <=90MHz
,unsigned short X1 //X of DMA Coordinate
,unsigned short Y1 //Y of DMA Coordinate 
,unsigned short X_W //DMA Block width
,unsigned short Y_H //DMA Block height
,unsigned short P_W //DMA Picture width
,unsigned long Addr //DMA Source Start address
)
{  
  SPIM_Clock_Divided_1();                                   //SPI Clock = System Clock /{(Clk)*2}
  SPI_Master_LatchEdge_Select_Falling();           //SPIM latch

  Select_SFI_DMA_Mode();								   //Set Serial Flash DMA Mode
  SPI_Clock_Period(Clk);
  /////////////////////////////////////////****************DMA 
  Goto_Pixel_XY(X1,Y1);									  //set Memory coordinate in Graphic Mode
  SFI_DMA_Destination_Upper_Left_Corner(X1,Y1);			  //DMA Destination position(x,y)
  SFI_DMA_Transfer_Width_Height(X_W,Y_H);				  //Set DMA Block (Height , Width)
  SFI_DMA_Source_Width(P_W);							  //Set DMA Source Picture Width
  SFI_DMA_Source_Start_Address(Addr); 						  //Set Serial Flash DMA Source Starting Address

  Start_SFI_DMA();									  //Start DMA
  Check_Busy_SFI_DMA();								  //DMA Busy Check
}


void DMA_32bit
(
unsigned char Clk //Clk : SPI Clock = System Clock /{(Clk+1)*2}
,unsigned short X1 //X of DMA Coordinate
,unsigned short Y1 //Y of DMA Coordinate 
,unsigned short X_W //DMA Block width
,unsigned short Y_H //DMA Block height
,unsigned short P_W //DMA Picture width
,unsigned long Addr //DMA Source Start address
)
{  

//  Enable_SFlash_SPI();									   //Enable Serial Flash
  Select_SFI_DMA_Mode();								   //Set Serial Flash DMA Mode
  SPI_Clock_Period(Clk);

  Select_SFI_32bit_Address();							   //Set Serial Flash/ROM 32bits Address
  /////////////////////////////////////////****************DMA 
  Goto_Pixel_XY(X1,Y1);									  //set Memory coordinate in Graphic Mode
  SFI_DMA_Destination_Upper_Left_Corner(X1,Y1);			  //DMA Destination position(x,y)
  SFI_DMA_Transfer_Width_Height(X_W,Y_H);				  //Set DMA Block (Height , Width)
  SFI_DMA_Source_Width(P_W);							  //Set DMA Source Picture Width
  SFI_DMA_Source_Start_Address(Addr); 						  //Set Serial Flash DMA Source Starting Address

  Start_SFI_DMA();									  //Start DMA
  Check_Busy_SFI_DMA();								  //DMA Busy Check
  Select_SFI_24bit_Address();
}


void switch_24bits_to_32bits(unsigned char BUS,unsigned char SCS)
{
	Enable_SFlash_SPI();
	SPI_Master_LatchEdge_Select_Falling();
  SPIM_Clock_Divided_1();
	SPI_Clock_Period(2);
	
	switch(BUS)
  {	
	  case 0:
      SPIM_Select_Bus_0();	
	  break;
		
	  case 1:
      SPIM_Select_Bus_1();	
	  break;
	}
	
  if(SCS == 0)
  {
   Select_nSS_drive_on_xnsfcs0();
  }
  if(SCS == 1)
  {														    
   Select_nSS_drive_on_xnsfcs1();
  }	
  if(SCS == 2)
  {														    
   Select_nSS_drive_on_xnsfcs2();
  }	
  if(SCS == 3)
  {														    
   Select_nSS_drive_on_xnsfcs3();
  }		
//**************************//data are read on the clock's rising edge(low!鑐igh transition)
//**************************//and data are changed on a falling edge (high!鑇ow clock transition) 
  Reset_CPOL();						   
  //Set_CPOL();
  Reset_CPHA();
  //Set_CPHA();
//**************************
  //Enter 4-byte mode
  								   
  nSS_Active();					   //nSS port will goes low
  SPI_Master_FIFO_Data_Put(0xB7);  

  delay_ms(1); 
  nSS_Inactive();				   //nSS port will goes high
}



void PWM0
(
unsigned char on_off //on_off = 1 ,enable PWM, on_off = 0 , disable PWM.
,unsigned char Clock_Divided // divided PWM clock, only 0~3(1,1/2,1/4,1/8)
,unsigned char Prescalar //Prescaler : only 1~256
,unsigned short Count_Buffer //Count_Buffer : set PWM output period time
,unsigned short Compare_Buffer //Compare_Buffer : set PWM output high level time(Duty cycle)
/*Such as the following formula :
PWM CLK = (Core CLK / Prescalar ) /2^ divided clock 
PWM output period = (Count Buffer + 1) x PWM CLK time
PWM output high level time = (Compare Buffer + 1) x PWM CLK time */
)  
{
   Select_PWM0();
   Set_PWM_Prescaler_1_to_256(Prescalar);

if(Clock_Divided ==0)
{
   Select_PWM0_Clock_Divided_By_1();
}
if(Clock_Divided ==1)
{
   Select_PWM0_Clock_Divided_By_2();
}
if(Clock_Divided ==2)
{
   Select_PWM0_Clock_Divided_By_4();
}
if(Clock_Divided ==3)
{
   Select_PWM0_Clock_Divided_By_8();
}

	Set_Timer0_Count_Buffer(Count_Buffer);  //設定階數
    Set_Timer0_Compare_Buffer(Compare_Buffer);	//DUTY
	
if (on_off == 1)
{
	Start_PWM0();
}	 
if (on_off == 0)
{
	Stop_PWM0();
}
}


void PWM1
(
unsigned char on_off //on_off = 1 ,enable PWM, on_off = 0 , disable PWM.
,unsigned char Clock_Divided // divided PWM clock, only 0~3(1,1/2,1/4,1/8)
,unsigned char Prescalar //Prescaler : only 1~256
,unsigned short Count_Buffer //Count_Buffer : set PWM output period time
,unsigned short Compare_Buffer //Compare_Buffer : set PWM output high level time(Duty cycle)
/*Such as the following formula :
PWM CLK = (Core CLK / Prescalar ) /2^ divided clock 
PWM output period = (Count Buffer + 1) x PWM CLK time
PWM output high level time = (Compare Buffer + 1) x PWM CLK time */
)   
{
 Select_PWM1();
 Set_PWM_Prescaler_1_to_256(Prescalar);
 
if(Clock_Divided ==0)
{
   Select_PWM1_Clock_Divided_By_1();
}
if(Clock_Divided ==1)
{
   Select_PWM1_Clock_Divided_By_2();
}
if(Clock_Divided ==2)
{
   Select_PWM1_Clock_Divided_By_4();
}
if(Clock_Divided ==3)
{
   Select_PWM1_Clock_Divided_By_8();
}

   Set_Timer1_Count_Buffer(Count_Buffer); //設定階數
   Set_Timer1_Compare_Buffer(Compare_Buffer); //DUTY

 if (on_off == 1)
{
	Start_PWM1();
}	 
if (on_off == 0)
{
	Stop_PWM1();
}

}



// Note. this API does not support the case that MCU=16bit, 24bpp and mode1
void putPixel
(
unsigned short x //x of coordinate
,unsigned short y //y of coordinate
,unsigned long color 
/*color : 8bpp:R3G3B2
16bpp:R5G6B5
24bpp:R8G8B8 */
)

{

	Goto_Pixel_XY(x,y);
	LCD_CmdWrite(0x04);	
  	Check_Mem_WR_FIFO_not_Full();

	#ifdef MCU_8bit_ColorDepth_8bpp	
	LCD_DataWrite(color);
	#endif
	#ifdef MCU_8bit_ColorDepth_16bpp	
	LCD_DataWrite(color);
	Check_Mem_WR_FIFO_not_Full();
	LCD_DataWrite(color>>8);
	#endif
	#ifdef MCU_8bit_ColorDepth_24bpp	
	LCD_DataWrite(color);
	Check_Mem_WR_FIFO_not_Full();
	LCD_DataWrite(color>>8);
	Check_Mem_WR_FIFO_not_Full();
	LCD_DataWrite(color>>16);
	#endif
	#ifdef MCU_16bit_ColorDepth_16bpp	
	LCD_DataWrite(color);
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
	LCD_DataWrite(color);
	Check_Mem_WR_FIFO_not_Full();
	LCD_DataWrite(color>>16);
	#endif
}

 
// Note. this API does not support the case that MCU=16bit, 24bpp and mode1
void lcdPutChar8x12
(
unsigned short x // x of coordinate
,unsigned short y // y of coordinate
,unsigned long fgcolor //fgcolor : foreground color(font color)
,unsigned long bgcolor //bgcolor : background color
/*8bpp:R3G3B2
16bpp:R5G6B5
24bpp:R8G8B8*/
, unsigned char bg_transparent 
/*bg_transparent = 0, background color with no transparent
bg_transparent = 1, background color with transparent*/
,unsigned char code //code : font char
)
{ 
  unsigned short i=0;
  unsigned short j=0;
  unsigned char tmp_char=0,LB;
  unsigned long temp1,temp2;

  for (i=0;i<12;i++)
  {
   tmp_char = ascii_table_8x12[((code-0x20)*12)+i];//minus 32 offset, because this table from ascii table "space" 

   for (j=0;j<8;j++)
   {

    if ( (tmp_char >>7-j) & 0x01 == 0x01)

        putPixel(x+j,y+i,fgcolor); //

    else
	{   

        if(!bg_transparent)

        putPixel(x+j,y+i,bgcolor); //

    } 


   }

  }

}


// Note. this API does not support the case that MCU=16bit, 24bpp and mode1
void lcdPutString8x12
(
unsigned short x //x of coordinate 
,unsigned short y //y of coordinate 
, unsigned long fgcolor //fgcolor : foreground color(font color)
,unsigned long bgcolor //bgcolor : background color
/*8bpp:R3G3B2
16bpp:R5G6B5
24bpp:R8G8B8*/
, unsigned char bg_transparent 
/*bg_transparent = 0, background color with no transparent
bg_transparent = 1, background color with transparent*/
,char *ptr //*ptr: font string
)
{
  unsigned short i = 0;
  //screen width = 800,  800/8 = 100 
  //if string more then 100 fonts, no show
  while ((*ptr != 0) & (i < 100))
  {
    lcdPutChar8x12(x, y, fgcolor, bgcolor,bg_transparent, *ptr);
    x += 8;
    ptr++;
    i++;  
  }

}

 
// Note. this API does not support the case that MCU=16bit, 24bpp and mode1
void lcdPutChar16x24
(
unsigned short x //x of coordinate
,unsigned short y //y of coordinate
,unsigned long fgcolor //fgcolor : foreground color(font color)
,unsigned long bgcolor //bgcolor : background color
/*8bpp:R3G3B2
16bpp:R5G6B5
24bpp:R8G8B8*/
, unsigned char bg_transparent
/*bg_transparent = 0, background color with no transparent
bg_transparent = 1, background color with transparent*/
,unsigned char code //code : font char
)
{ 
  unsigned short i=0;
  unsigned short j=0;
  unsigned long array_addr =0;
  unsigned int tmp_char=0;

  for (i=0;i<24;i++)
  {
    //minus 32 offset, because this table from ascii table "space"  
    array_addr = ((code-0x20)*2*24)+(i*2); 
    tmp_char = ascii_table_16x24[array_addr]<<8|ascii_table_16x24[array_addr+1];

   for (j=0;j<16;j++)
   {

    if ( (tmp_char >>15-j) & 0x01 == 0x01)

        putPixel(x+j,y+i,fgcolor); //

    else
	   {
	    if(!bg_transparent)

        putPixel(x+j,y+i,bgcolor); // 

       }
   }
  }
}

 
// Note. this API does not support the case that MCU=16bit, 24bpp and mode1
void lcdPutString16x24
(
unsigned short x //x of coordinate
,unsigned short y //y of coordinate
, unsigned long fgcolor //fgcolor : foreground color(font color)
, unsigned long bgcolor //bgcolor : background color
/*8bpp:R3G3B2
16bpp:R5G6B5
24bpp:R8G8B8*/
, unsigned char bg_transparent 
/*bg_transparent = 0, background color with no transparent
bg_transparent = 1, background color with transparent*/
,char *ptr //*ptr : font string
) 
{
  unsigned short i = 0;
  //screen width = 800,  800/16 = 50 
  //if string more then 50 fonts, no show
  while ((*ptr != 0) & (i < 50))
  {
    lcdPutChar16x24(x, y, fgcolor, bgcolor,bg_transparent, *ptr);
    x += 16;
    ptr++;
    i++;  
  }

}

 
// Note. this API does not support the case that MCU=16bit, 24bpp and mode1
void lcdPutChar32x48
(
unsigned short x //x of coordinate
,unsigned short y //y of coordinate
,unsigned long fgcolor //fgcolor : foreground color(font color)
,unsigned long bgcolor //bgcolor : background color
/*8bpp:R3G3B2
16bpp:R5G6B5
24bpp:R8G8B8*/
, unsigned char bg_transparent 
/*bg_transparent = 0, background color with no transparent 
bg_transparent = 1, background color with transparent*/
,unsigned char code //code : font char
)
{ 
  unsigned short i=0;
  unsigned short j=0;
  unsigned long array_addr =0;
  unsigned long tmp_char=0;

  for (i=0;i<48;i++)
  {

    //minus 32 offset, because this table from ascii table "space"  
	array_addr = ((code-0x20)*4*48)+(i*4); 
	tmp_char = ascii_table_32x48[array_addr]<<24|ascii_table_32x48[array_addr+1]<<16|ascii_table_32x48[array_addr+2]<<8|ascii_table_32x48[array_addr+3];
	     
     for (j=0;j<32;j++)
	 {

     if ( (tmp_char >> (31-j)) & 0x01 == 0x01)

       putPixel(x+j,y+i,fgcolor); //

     else

         {
		 if(!bg_transparent)
		    putPixel(x+j,y+i,bgcolor); // 
		 }
	 } 

  }

}

 
// Note. this API does not support the case that MCU=16bit, 24bpp and mode1
void lcdPutString32x48
(
unsigned short x //x of coordinate
,unsigned short y //y of coordinate
, unsigned long fgcolor //fgcolor : foreground color(font color)
, unsigned long bgcolor //bgcolor : background color
/*8bpp:R3G3B2
16bpp:R5G6B5
24bpp:R8G8B8*/
, unsigned char bg_transparent, 
/*bg_transparent = 0, background color with no transparent
bg_transparent = 1, background color with transparent*/
char *ptr //*ptr: font string
)
{
  unsigned short i = 0;
  //screen width = 800,  800/32 = 25 
  //if string more then 25 fonts, no show
  while ((*ptr != 0) & (i < 25))
  {
    lcdPutChar32x48(x, y, fgcolor, bgcolor,bg_transparent, *ptr);
    x += 32;
    ptr++;
    i++;  
  }

}


/////********************************
/*
BUS : select SPI Master bus 0 or 1
SCS : select SPI Master bus 0~3
buffer : AVI Shadow buffer
SCK_Divide : Media CLK = CORE CLK / (2^SCK_Divide) , Media CLK <= 60MHz (range : 0~2)
*/
/////********************************
void SPI_NAND_initial_JPG_AVI 
(
unsigned char BUS//BUS : 0 = Use BUS0, 1 = Use BUS1
,unsigned char SCS//SCS : 0 = Use SCS0, 1 = Use SCS1 ,2 = Use SCS2 ,3 = Use SCS3
,unsigned char SCK_Divide//media decode divide : IDEC Clock = CORE CLK/2^SCK_Divide ,range:0~3, recommend <= 60MHz
)
{
	Enable_SFlash_SPI();
	SPI_Master_LatchEdge_Select_Falling();
  SPIM_Clock_Divided_1();
	IDEC_SPI_Clock_Divide(SCK_Divide);
	SPI_Clock_Period(2);
	
		
  
	switch(BUS)
  {	
	  case 0:
      IDEC_Select_Bus_0();                     //MEDIA BUS 0
      SPIM_Select_Bus_0();	
	  break;
		
	  case 1:
      IDEC_Select_Bus_1();                     //MEDIA BUS 1
      SPIM_Select_Bus_1();	
	  break;
  }		

  switch(SCS)
  {	
	  case 0:
			IDEC_Select_SFI_0();                               //MEDIA CS0
      Select_nSS_drive_on_xnsfcs0();
	  break;
	
	  case 1:
		  IDEC_Select_SFI_1();                               //MEDIA CS1
      Select_nSS_drive_on_xnsfcs1();
	  break;

	  case 2:
			IDEC_Select_SFI_2();                               //MEDIA CS2
      Select_nSS_drive_on_xnsfcs2();
	  break;
		
	  case 3:
		  IDEC_Select_SFI_3();                               //MEDIA CS3
      Select_nSS_drive_on_xnsfcs3();
	  break;
  }	


  Reset_CPOL();						   
	//Set_CPOL();
	Reset_CPHA();
  //Set_CPHA();


	W25N01_Device_Reset();
	W25N01_Clear_BUF();
	W25N01_Set_ECC();
	
  IDEC_Select_SFI_24bit_Address();          
  IDEC_Select_RA8875_SPI_Mode0_and_Mode3();         
  IDEC_Select_SFI_Quad_Mode_Dummy_8T_6Bh();       //MEDIA QUAD MODE
  //IDEC_Select_SFI_Quad_Mode_Dummy_4T_EBh();

  IDEC_AVI_Decoding_to_PIP1_and_Shadow();        
}

/////********************************
/*
BUS : select SPI Master bus 0 or 1
SCS : select SPI Master bus 0~3
SCK_Divide : Media CLK = CORE CLK / (2^SCK_Divide) , Media CLK <= 60MHz
*/
/////********************************
void SPI_NAND_initial_DMA 
(
unsigned char SCS  //SCS : 0 = Use SCS0, 1 = Use SCS1 ,2 = Use SCS2 ,3 = Use SCS3
,unsigned char BUS//BUS : 0 = Use BUS0, 1 = Use BUS1
)
{
	Enable_SFlash_SPI();
	SPI_Master_LatchEdge_Select_Falling();
  SPIM_Clock_Divided_1();
	SPI_Clock_Period(2);

  switch(BUS)
  {	
	  case 0:
	    SPIM_Select_Bus_0();		
	    FONT_DMA_Select_Bus_0();
	  break;
		
	  case 1:
	    SPIM_Select_Bus_1();		
	    FONT_DMA_Select_Bus_1();
	  break;
  }		


  switch(SCS)
  {	
	  case 0:
 	    Select_nSS_drive_on_xnsfcs0();
	    Select_SFI_0();
	  break;
	
	  case 1:
 	    Select_nSS_drive_on_xnsfcs1();
	    Select_SFI_1();
	  break;
		
	  case 2:
 	    Select_nSS_drive_on_xnsfcs2();
	    Select_SFI_2();
	  break;
	  
		case 3:
 	  Select_nSS_drive_on_xnsfcs3();
	  Select_SFI_3();
	  break;
  }
	
  Reset_CPOL();						   
  //Set_CPOL();
	Reset_CPHA();
  //Set_CPHA();
	
	W25N01_Device_Reset();
	W25N01_Clear_BUF();
	W25N01_Set_ECC();
	
	
	//Select_standard_SPI_Mode0_or_Mode3();
	Select_RA8875_SPI_Mode0_and_Mode3();
	
	//Select_SFI_Single_Mode_Dummy_0T_03h();
	//Select_SFI_Single_Mode_Dummy_8T_0Bh();
	//Select_SFI_Single_Mode_Dummy_16T_1Bh();
  //Select_SFI_Dual_Mode_Dummy_8T_3Bh();
	//Select_SFI_Dual_Mode_Dummy_4T_BBh();
	Select_SFI_Quad_Mode_Dummy_8T_6Bh();

	Select_SFI_DMA_Mode();
	Select_SFI_24bit_Address();

}




/////********************************
/*
addr : Pic addr in flash
JPGsize : Pic size
x : write pic coordinate of x
y : write pic coordinate of y
*/
/////********************************
void JPG_NAND 
(
unsigned long addr//Pic addr in flash
,unsigned long JPGsize//Pic size
,unsigned long IDEC_canvas_width //recommend = canvas_image_width
,unsigned short x//write pic coordinate of x
,unsigned short y//write pic coordinate of y
)
{
   IDEC_Destination_Image_Width(IDEC_canvas_width);
   IDEC_Destination_Upper_Left_Corner(x,y);

   nSS_Active();
   SPI_Master_FIFO_Data_Put(0x13);      //page data read(13h)
   SPI_Master_FIFO_Data_Put(0x00);      //8Dummy clock
   SPI_Master_FIFO_Data_Put(addr>>8);   //page address MSB
   SPI_Master_FIFO_Data_Put(addr);   //page address LSB
   nSS_Inactive();
   delay_us(500); //delay for buffer load

	 IDEC_Source_Start_Address(addr);    //JPG起cl|i址
   IDEC_Transfer_Number(JPGsize);             //JPG SIZE
   IDEC_Starts_Decoding();        //?}cl?NJPG
   Check_Busy_Media_Decode(); 
}


/////********************************
/*
addr : vedio addr in flash
vediosize : vedio size
x : show vedio coordinate of x
y : show vedio coordinate of y
height : vedio height
width : vedio width
*/
/////********************************
void AVI_NAND
(
unsigned long addr//vedio addr in flash
,unsigned long videosize//vedio size
,unsigned long shadow_buffer_addr //shadow buffer addr
,unsigned long PIP_buffer_addr//PIP buffer addr
,unsigned long x //show vedio coordinate of x
,unsigned long y //show vedio coordinate of y
,unsigned long height//vedio height
,unsigned long width//vedio width
,unsigned long PIP_canvas_Width//recommend = canvas_image_width
)
{	
  nSS_Active();
  SPI_Master_FIFO_Data_Put(0x13);      //page data read(13h)
  SPI_Master_FIFO_Data_Put(0x00);      //8Dummy clock
  SPI_Master_FIFO_Data_Put(addr>>8);   //page address MSB
  SPI_Master_FIFO_Data_Put(addr);   //page address LSB
  nSS_Inactive();
  delay_us(500); //delay for buffer load

	IDEC_Source_Start_Address(addr);
	IDEC_Transfer_Number(videosize);//AVI File Size
  AVI_Shadow_PIP_Start_Address(shadow_buffer_addr);
	
	Select_PIP1_Parameter();	
	PIP_Image_Start_Address(PIP_buffer_addr);
	PIP_Display_Start_XY(x,y);
	PIP_Image_Width(PIP_canvas_Width);
	PIP_Window_Image_Start_XY(0,0);
	PIP_Window_Width_Height(width,height);		
//	Enable_PIP1();
	
	
	
	//Check_Vsync_finished();
	IDEC_Starts_Decoding();
  //Check_Busy_Media_Decode();	

}

void AVI_window
(
unsigned char ON_OFF//0 : turn off AVI window, 1 :turn on AVI window
)
{
if (ON_OFF == 0)	
{
	Disable_PIP1();
}
if (ON_OFF == 1)	
{
	Enable_PIP1();
}

}

/*
PNG PIC : A(8)R(8)G(8)B(8) = 32bits
For BTE_alpha_blending_32bit_Pixel_mode
dma_page_addr : pic addr
pic_buffer_Layer : read pic buffer in sdram
Show_pic_Layer : show pic layer
*/
void SPI_NAND_DMA_png 
(
unsigned long dma_page_addr //dma pic addr in flash
,unsigned long pic_buffer_Layer//pic_buffer_Layer : read pic buffer in sdram
,unsigned long Show_pic_Layer//Show_pic_Layer : write pic into sdram addr
,unsigned int picture_Width
,unsigned int picture_Height
)
{

	nSS_Active();
	SPI_Master_FIFO_Data_Put(0x13);	 //page data read(13h)
	SPI_Master_FIFO_Data_Put(0x00);	 //8Dummy clock
	SPI_Master_FIFO_Data_Put((dma_page_addr)>>8);   //page address MSB
	SPI_Master_FIFO_Data_Put(dma_page_addr);   //page address LSB
	nSS_Inactive();
	delay_us(500); //delay for buffer load
	    	  
	Memory_Linear_Mode();

	SFI_DMA_Transfer_Number(picture_Width*picture_Height*4);//picture Width x Height x Color depth(32bit = 4byte)
	SFI_DMA_Destination_Start_Address(pic_buffer_Layer);//

	Start_SFI_DMA();
	Check_2D_Busy();
	Memory_XY_Mode();

}

void SPI_NOR_DMA_png 
(
unsigned long dma_page_addr//dma pic addr in flash
,unsigned long pic_buffer_Layer //pic_buffer_Layer : read pic buffer in sdram
,unsigned long Show_pic_Layer //Show_pic_Layer : write pic into sdram addr
,unsigned int picture_Width
,unsigned int picture_Height
)
{
	  	  
	Memory_Linear_Mode();
  SFI_DMA_Source_Start_Address(dma_page_addr);
	SFI_DMA_Transfer_Number(picture_Width*picture_Height*4);//picture Width x Height x Color depth(32bit = 4byte)
	SFI_DMA_Destination_Start_Address(pic_buffer_Layer);//

  	
	Start_SFI_DMA();
  Check_2D_Busy();
	Memory_XY_Mode();

}


/*
dma_page_addr : pic addr
pic_buffer_Layer : read pic buffer in sdram
Show_pic_Layer : show pic layer
X_coordinate : write to sdram coordinate of x
Y_coordinate : write to sdram coordinate of y
chorma : 0 = no transparent ,1 : Specify a color as transparent
Background_color : transparent color
*/
void SPI_NAND_DMA 
(
unsigned long dma_page_addr//dma pic addr in flash
,unsigned long X_coordinate//pic write to sdram coordinate of x
,unsigned long Y_coordinate//pic write to sdram coordinate of y
,unsigned long des_canvas_width//recommend = canvas_image_width
,unsigned int picture_Width
,unsigned int picture_Height
,unsigned long pic_buffer_Layer//pic_buffer_Layer : read pic buffer in sdram
,unsigned long Show_pic_Layer//Show_pic_Layer : write pic into sdram addr
,unsigned char chorma//0: no transparent,1:Specify a color as transparent
,unsigned long Background_color//transparent color
)
{

	#ifdef MCU_8bit_ColorDepth_8bpp	
		Background_color_256(Background_color); 
	#endif
	#ifdef MCU_8bit_ColorDepth_16bpp	
		Background_color_65k(Background_color); 
	#endif
	#ifdef MCU_8bit_ColorDepth_24bpp	
		Background_color_16M(Background_color); 
	#endif
	#ifdef MCU_16bit_ColorDepth_16bpp	
		Background_color_65k(Background_color); 
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_1	
		Background_color_16M(Background_color); 
	#endif
	#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2	
		Background_color_16M(Background_color); 
	#endif

	nSS_Active();
	SPI_Master_FIFO_Data_Put(0x13);	 //page data read(13h)
	SPI_Master_FIFO_Data_Put(0x00);	 //8Dummy clock
	SPI_Master_FIFO_Data_Put((dma_page_addr)>>8);   //page address MSB
	SPI_Master_FIFO_Data_Put(dma_page_addr);   //page address LSB
	nSS_Inactive();
	delay_us(500); //delay for buffer load
	  
	  	  
	Memory_Linear_Mode();
	SFI_DMA_Source_Start_Address(dma_page_addr);//24bit address

	#if defined  MCU_8bit_ColorDepth_8bpp
	SFI_DMA_Transfer_Number(picture_Width*picture_Height);//picture Width x Height x Color depth(8/8=1)
	#endif
	#if defined  MCU_8bit_ColorDepth_16bpp 
	SFI_DMA_Transfer_Number(picture_Width*picture_Height*2);//picture Width x Height x Color depth(16/8=2)
	#endif
	#if defined  MCU_16bit_ColorDepth_16bpp
	SFI_DMA_Transfer_Number(picture_Width*picture_Height*2);//picture Width x Height x Color depth(16/8=2)
	#endif
	#if defined  MCU_8bit_ColorDepth_24bpp 
	SFI_DMA_Transfer_Number(picture_Width*picture_Height*3);//picture Width x Height x Color depth(24/8=3)
	#endif
	#if defined  MCU_16bit_ColorDepth_24bpp_Mode_1
	SFI_DMA_Transfer_Number(picture_Width*picture_Height*3);//picture Width x Height x Color depth(24/8=3)
	#endif	
	#if defined  MCU_16bit_ColorDepth_24bpp_Mode_2
	SFI_DMA_Transfer_Number(picture_Width*picture_Height*3);//picture Width x Height x Color depth(24/8=3)
	#endif	
	
	SFI_DMA_Destination_Start_Address(pic_buffer_Layer);//DMA to Memory Address 1280*480*3
	
	Start_SFI_DMA();
  //Check_Busy_SFI_DMA();
	Check_2D_Busy();

  BTE_S0_Color_24bpp();
  BTE_S0_Memory_Start_Address(pic_buffer_Layer);	//S0 Address 1280*480*3
  BTE_S0_Image_Width(picture_Width);
  BTE_S0_Window_Start_XY(0,0);



  BTE_Destination_Color_24bpp();  
  BTE_Destination_Memory_Start_Address(Show_pic_Layer);
  BTE_Destination_Image_Width(des_canvas_width);
  BTE_Destination_Window_Start_XY(X_coordinate,Y_coordinate);
  
  if(chorma==0)
  {
    BTE_ROP_Code(12);
    BTE_Operation_Code(0x02);
  }
  if(chorma==1)
  {
    BTE_Operation_Code(0x05);
  }
  
	BTE_Window_Size(picture_Width,picture_Height);

  BTE_Enable();

  Check_BTE_Busy();
  
  Memory_XY_Mode();
  
}


/*
use for linear mode ,nand flash,
BTE_X : BTE window Width
BTE_Y : BTE window Height
S0X : source 0 coordinate of x
S0Y : source 0 coordinate of y
desX :destination coordinate of x
desY :destination coordinate of y
pic_buffer_Layer : read pic buffer addr in sdram
*/
void BTE_alpha_blending_32bit_Pixel_mode
(
unsigned int picture_Width //pic width
,unsigned int BTE_X //BTE window Width
,unsigned int BTE_Y//BTE window Height
,unsigned long S0X //source 0 coordinate of x
,unsigned long S0Y//source 0 coordinate of y
,unsigned long S0_Start_Addr //source 0 start addr
,unsigned long S0_canvas_width //recomamnd = canvas_image_width
,unsigned long desX//Destination coordinate of x
,unsigned long desY//Destination coordinate of y
,unsigned long DES_Start_Addr//Destination start addr
,unsigned long DES_canvas_width//recomamnd = canvas_image_width
,unsigned long pic_buffer_Layer//source 1 pic addr
)
{
   BTE_S0_Color_24bpp();
  BTE_S0_Memory_Start_Address(S0_Start_Addr);
  BTE_S0_Image_Width(S0_canvas_width);
  

  BTE_S1_Color_32bit_Alpha();
  BTE_S1_Memory_Start_Address(pic_buffer_Layer);
  BTE_S1_Image_Width(picture_Width); //ARGB


  BTE_Destination_Color_24bpp();  
  BTE_Destination_Memory_Start_Address(DES_Start_Addr);
  BTE_Destination_Image_Width(DES_canvas_width);


  BTE_S1_Window_Start_XY(0,0);
  BTE_S0_Window_Start_XY(S0X,S0Y);
  BTE_Destination_Window_Start_XY(desX,desY);
  BTE_Window_Size(BTE_X,BTE_Y);
  BTE_ROP_Code(12);
  BTE_Operation_Code(0x0A);
  BTE_Enable();
  Check_BTE_Busy();


  BTE_S1_Color_24bpp();

}



void SPI_NOR_Flash_switch_QUAD_MODE
(
unsigned char BUS /*select SPI Bus 0 or 1*/
,unsigned char SCS /*select SPI CS 0~3*/
,unsigned char flash /*select flash: 0 =MXIC , 1=WINBOND*/
)

{
	unsigned char temp;


	Enable_SFlash_SPI();
	SPI_Master_LatchEdge_Select_Falling();
  SPIM_Clock_Divided_1();
	SPI_Clock_Period(2);

  switch(BUS)
  {	
	  case 0:
      SPIM_Select_Bus_0();	
	  break;
	  case 1:
      SPIM_Select_Bus_1();	
	  break;
  }		

  switch(SCS)
  {	
	  case 0:
      Select_nSS_drive_on_xnsfcs0();
	  break;
	
	  case 1:
      Select_nSS_drive_on_xnsfcs1();
	  break;

	  case 2:
      Select_nSS_drive_on_xnsfcs2();
	  break;
		
	  case 3:
      Select_nSS_drive_on_xnsfcs3();
	  break;
  }	
	
  Reset_CPOL();						   
  //Set_CPOL();
	Reset_CPHA();
  //Set_CPHA();	
	
  //WREN : write enable
  nSS_Active();
  SPI_Master_FIFO_Data_Put(0x06);  
  nSS_Inactive();

  //RDSR : check writable

  do{
    nSS_Active();
    SPI_Master_FIFO_Data_Put(0x05);  //
    temp = SPI_Master_FIFO_Data_Put(0xff);
    nSS_Inactive();
    delay_us(100);
    }while((temp&0x02)==0x00);  //WEL=1 ? WRITABLE : UNWRITABLE

  switch(flash)
  {
	  case 0:
    //**********************************MXIC Flash Quad mode
    //WRITE STATUS REG                      
      nSS_Active();
      SPI_Master_FIFO_Data_Put(0x01);  
      SPI_Master_FIFO_Data_Put(0x40);
      nSS_Inactive();                        
    ///*************************************	
	  break;
	
		case 1:

    //******************************Winbond Flash Quad mode
    //WRITE STATUS REG
      nSS_Active();
      SPI_Master_FIFO_Data_Put(0x31);  // Status Register-2
      SPI_Master_FIFO_Data_Put(0x02);    // QE=1, Quad Mode
      nSS_Inactive();
    ///*****************************		
	  break;
  }	

  //check flash busy 
  do                
  {                           
    //RDSR
    nSS_Active();
    SPI_Master_FIFO_Data_Put(0x05);  //
    temp = SPI_Master_FIFO_Data_Put(0xff);
    nSS_Inactive();
  }while((temp & 0x01)==0x01);   //WIP=1 ? BUSY:IDLE

}

void SPI_NOR_initial_DMA 
(
unsigned char mode//SPI mode : 0:Single_03h,1:Single_0Bh,2:Single_1Bh,3:Dual_3Bh,4:Dual_BBh,5:Quad_6Bh,6:Quad_EBh
,unsigned char BUS //BUS : 0 = Use BUS0, 1 = Use BUS1
,unsigned char SCS //SCS : 0 = Use SCS0, 1 = Use SCS1 ,2 = Use SCS2 ,3 = Use SCS3
,unsigned char flash//0 : MXIC , 1 : Winbond
,unsigned char addr_24b_32b //flash 24bit or 32bit addr
)
{		
	unsigned char temp;
  
	Enable_SFlash_SPI();
	SPI_Master_LatchEdge_Select_Falling();
  SPIM_Clock_Divided_1();
	SPI_Clock_Period(2);
	
  switch(BUS)
  {	
	  case 0:
	    FONT_DMA_Select_Bus_0();                   
      SPIM_Select_Bus_0();	
	  break;
		
	  case 1:
	    FONT_DMA_Select_Bus_1();              
      SPIM_Select_Bus_1();	
	  break;
  }		

  switch(SCS)
  {	
	  case 0:
		  Select_SFI_0();
      Select_nSS_drive_on_xnsfcs0();
	  break;
	
	  case 1:
		  Select_SFI_1();
      Select_nSS_drive_on_xnsfcs1();
	  break;

	  case 2:
		  Select_SFI_2();
      Select_nSS_drive_on_xnsfcs2();
	  break;
	  
		case 3:
		  Select_SFI_3();
      Select_nSS_drive_on_xnsfcs3();
	  break;
  }	

	Select_RA8875_SPI_Mode0_and_Mode3();

  switch(mode)
  {
	  case 0:
	    Select_SFI_Single_Mode_Dummy_0T_03h();	//single mode
	  break;
		
	  case 1:
	    Select_SFI_Single_Mode_Dummy_8T_0Bh();	//single mode
	  break;
	  
		case 2:
	    Select_SFI_Single_Mode_Dummy_16T_1Bh();	//single mode
	  break;
	  
		case 3:
	    Select_SFI_Dual_Mode_Dummy_8T_3Bh();	//dual mode
	  break;
		
		case 4:
			SPI_NOR_Flash_switch_QUAD_MODE(BUS,SCS,flash); //quad mode
		  Select_SFI_Quad_Mode_Dummy_8T_6Bh();
		break;
		
		case 5:
			SPI_NOR_Flash_switch_QUAD_MODE(BUS,SCS,flash); //quad mode
		  Select_SFI_Quad_Mode_Dummy_4T_EBh();
	}
	
	Select_SFI_DMA_Mode();
	
	if(addr_24b_32b==0)
	  Select_SFI_24bit_Address();
 	else
	{
		switch_24bits_to_32bits(BUS,SCS);
		Select_SFI_32bit_Address();
	}
	Memory_XY_Mode();
  delay_ms(30);

}



void SPI_NOR_initial_JPG_AVI
(
unsigned char flash//0 : MXIC , 1 : Winbond
,unsigned char addr_24b_32b//flash addr : 0:24bit addr, 1:32bit addr
,unsigned char BUS//BUS : 0 = Use BUS0, 1 = Use BUS1
,unsigned char SCS//SCS : 0 = Use SCS0, 1 = Use SCS1 ,2 = Use SCS2 ,3 = Use SCS3
,unsigned char SCK_Divide//media decode divide : IDEC Clock = CORE CLK/2^SCK_Divide ,range:0~3, recommend <= 60MHz
)
{
	unsigned char temp;

	Enable_SFlash_SPI();
	SPI_Master_LatchEdge_Select_Falling();
  SPIM_Clock_Divided_1();
	IDEC_SPI_Clock_Divide(SCK_Divide);
	SPI_Clock_Period(2);
                                            

  switch(BUS)
  {	
	  case 0:
      IDEC_Select_Bus_0();                     //MEDIA ?I?IBUS 0
      SPIM_Select_Bus_0();	
	  break;
	  case 1:
      IDEC_Select_Bus_1();                     //MEDIA ?I?IBUS 1
      SPIM_Select_Bus_1();	
	  break;
  }		

  switch(SCS)
  {	
	  case 0:
			IDEC_Select_SFI_0();                               //MEDIA?I?ICS0
			Select_nSS_drive_on_xnsfcs0();
		break;
		
		case 1:
			IDEC_Select_SFI_1();                               //MEDIA?I?ICS1
			Select_nSS_drive_on_xnsfcs1();
		break;

		case 2:
			IDEC_Select_SFI_2();                               //MEDIA?I?ICS2
			Select_nSS_drive_on_xnsfcs2();
		break;
		
		case 3:
			IDEC_Select_SFI_3();                               //MEDIA?I?ICS3
			Select_nSS_drive_on_xnsfcs3();
		break;
	}
  
	SPI_NOR_Flash_switch_QUAD_MODE(BUS,SCS,flash);		//Into Quad Mode

	if(addr_24b_32b==0)
	  IDEC_Select_SFI_24bit_Address();
 	else
	{
		switch_24bits_to_32bits(BUS,SCS);
		IDEC_Select_SFI_32bit_Address();
	}
	         
  IDEC_Select_RA8875_SPI_Mode0_and_Mode3();         
  IDEC_Select_SFI_Quad_Mode_Dummy_8T_6Bh();       //MEDIA QUAD MODE
  //IDEC_Select_SFI_Quad_Mode_Dummy_4T_EBh();

  IDEC_AVI_Decoding_to_PIP1_and_Shadow();        
}


void JPG_NOR 
(
unsigned long addr// JPG pic addr in flash
,unsigned long JPGsize //JPG pic size
,unsigned long IDEC_canvas_width //recommend = canvas_image_width
,unsigned short x//JPG pic write to sdram coordinate of x
,unsigned short y//JPG pic write to sdram coordinate of y
)
{
	
  IDEC_Destination_Image_Width(IDEC_canvas_width);
	IDEC_Destination_Upper_Left_Corner(x,y);

	IDEC_Source_Start_Address(addr);    //JPG起cl|i址
	IDEC_Transfer_Number(JPGsize);             //JPG SIZE
	IDEC_Starts_Decoding();        //?}cl?NJPG
	Check_Busy_Media_Decode(); 

}

void AVI_NOR
(
unsigned long addr// AVI addr in flash
,unsigned long videosize//video size
,unsigned long shadow_buffer_addr//shadow buffer addr
,unsigned long PIP_buffer_addr//PIP buffer addr
,unsigned long x//show AVI to coordinate of x
,unsigned long y//show AVI to coordinate of y
,unsigned long height//vedio height
,unsigned long width//vedio width
,unsigned long PIP_width// PIP Image width, recommend = canvas_image_width
)
{

	IDEC_Source_Start_Address(addr);
	IDEC_Transfer_Number(videosize);	//AVI File Size

	AVI_Shadow_PIP_Start_Address(shadow_buffer_addr);
	
	Select_PIP1_Parameter();
	
	PIP_Image_Start_Address(PIP_buffer_addr);
	PIP_Display_Start_XY(x,y);
	PIP_Image_Width(PIP_width);
	PIP_Window_Image_Start_XY(0,0);
	PIP_Window_Width_Height(width,height);		
//	Enable_PIP1();
	//Check_Vsync_finished();
	IDEC_Starts_Decoding();
	//Check_Busy_Media_Decode();

}


void Print_Hex(unsigned char buf)
{
	unsigned char temp;
	temp=buf;
	temp = (temp >>4) & 0x0F;
	Text_Mode();
	LCD_CmdWrite(0x04);

	if(temp < 0x0A)
	{
		temp |= 0x30;
	}
	else temp = temp + 0x37;
	LCD_DataWrite(temp);
	Check_Mem_WR_FIFO_not_Full();
//	delay_ms(1);

	temp=buf;
	temp = temp & 0x0F;
	if(temp < 0x0A)
	{
		temp |= 0x30;
	}
	else temp = temp + 0x37;
	LCD_DataWrite(temp);
//	delay_ms(1);
	Check_Mem_WR_FIFO_not_Full();
}