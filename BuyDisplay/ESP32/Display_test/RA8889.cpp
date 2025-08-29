#include "Arduino.h"
#include "RA8889.h"
#include <SPI.h>

#define  LCD_RESET  16
#define  LCD_CS     5

// ------------------------------------------------------------ SPI Drive --------------------------------------------------------------------
#if Arduino_SPI
void ER_TFTBasic::SPIInit()
{
	pinMode(LCD_CS, OUTPUT);
	SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));
	SPI.begin();
}
void ER_TFTBasic::SPISetCs(int cs)
{
	if(cs)
		digitalWrite(LCD_CS,HIGH);
	else
	  digitalWrite(LCD_CS,LOW);
}
unsigned char ER_TFTBasic::SPIRwByte(unsigned char value)
{
	unsigned char rec;
	rec = SPI.transfer(value);
	return rec;
}
void ER_TFTBasic::SPI_CmdWrite(int cmd)
{
  ER_TFT.SPISetCs(0);    //SS_RESET;
  ER_TFT.SPIRwByte(0x00);
  ER_TFT.SPIRwByte(cmd);
  ER_TFT.SPISetCs(1);    //SS_SET;
}
void ER_TFTBasic::SPI_DataWrite(int data)
{
  ER_TFT.SPISetCs(0);    //SS_RESET;
  ER_TFT.SPIRwByte(0x80);
  ER_TFT.SPIRwByte(data);
  ER_TFT.SPISetCs(1);    //SS_SET;
}
void ER_TFTBasic::SPI_DataWrite_Pixel(int data)
{
  ER_TFT.SPISetCs(0);    //SS_RESET;
  ER_TFT.SPIRwByte(0x80);
  ER_TFT.SPIRwByte(data);
  ER_TFT.SPISetCs(1);    //SS_SET;
  
  ER_TFT.SPISetCs(0);    //SS_RESET;
  ER_TFT.SPIRwByte(0x80);
  ER_TFT.SPIRwByte(data>>8);
  ER_TFT.SPISetCs(1);    //SS_SET;
}

//uint8_t StatusRead(void);
int ER_TFTBasic::SPI_StatusRead(void)
{
  int temp = 0;
  ER_TFT.SPISetCs(0);    //SS_RESET;
  ER_TFT.SPIRwByte(0x40);
  temp = ER_TFT.SPIRwByte(0x00);
  ER_TFT.SPISetCs(1);    //SS_SET;
  return temp;
}

int ER_TFTBasic::SPI_DataRead(void)
{
  int temp = 0;
  ER_TFT.SPISetCs(0);    //SS_RESET;
  ER_TFT.SPIRwByte(0xc0);
  temp = ER_TFT.SPIRwByte(0x00);
  ER_TFT.SPISetCs(1);    //SS_SET;
  return temp;
}
#endif

//-----------------------------------------------------------------------------------------------------------------------------------

void ER_TFTBasic::Parallel_Init(void)
{
	#if Arduino_SPI
	ER_TFT.SPIInit();
	#endif
	
	#if Arduino_IIC
	ER_TFT.IICInit();
	#endif
}
void ER_TFTBasic::LCD_CmdWrite(unsigned char cmd)
{
	#if Arduino_SPI
	ER_TFT.SPI_CmdWrite(cmd);
	#endif
	
	#if Arduino_IIC
	ER_TFT.IIC_CmdWrite(cmd);
	#endif
}

//void SPI_DataWrite(int data);
void ER_TFTBasic::LCD_DataWrite(unsigned char data)
{
	#if Arduino_SPI
	ER_TFT.SPI_DataWrite(data);
	#endif
	
	#if Arduino_IIC
	ER_TFT.IIC_DataWrite(data);
	#endif
}

//void SPI_DataWrite_Pixel(int data);
void ER_TFTBasic::LCD_DataWrite_Pixel(unsigned int data)
{
	#if Arduino_SPI
	ER_TFT.SPI_DataWrite_Pixel(data);
	#endif
	
	#if Arduino_IIC
	ER_TFT.IIC_DataWrite_Pixel(data);
	#endif
}

//funcao removida do meu projeto
unsigned char ER_TFTBasic::LCD_StatusRead(void)
{
	unsigned char temp = 0;
	
	#if Arduino_SPI
	temp = ER_TFT.SPI_StatusRead();
	#endif
	
	#if Arduino_IIC
	temp = ER_TFT.IIC_StatusRead();
	#endif
	
	return temp;
}

unsigned int ER_TFTBasic::LCD_DataRead(void)
{
	unsigned int temp = 0;

	#if Arduino_SPI
	temp = ER_TFT.SPI_DataRead();
	#endif
	
	#if Arduino_IIC
	temp = ER_TFT.IIC_DataRead();
	#endif
	
	return temp;
}
void ER_TFTBasic::LCD_RegisterWrite(unsigned char Cmd,unsigned char Data)
{
	ER_TFT.LCD_CmdWrite(Cmd);
	ER_TFT.LCD_DataWrite(Data);
}  
//---------------------//
unsigned char ER_TFTBasic::LCD_RegisterRead(unsigned char Cmd)
{
	unsigned char temp;
	
	ER_TFT.LCD_CmdWrite(Cmd);
	temp=ER_TFT.LCD_DataRead();
	return temp;
}

//void SDRAM_WaitReady(void);
void ER_TFTBasic::Check_SDRAM_Ready(void)
{
/*  0: SDRAM is not ready for access
  1: SDRAM is ready for access    */  
  unsigned char temp;
  do
  {
    temp=ER_TFT.LCD_StatusRead();
  }
  while( (temp&0x04) == 0x00 );
}


//void TFT_SetInterface(TFTInterface mode)
//void TFT_16bit(void)
void ER_TFTBasic::TFT_16bit(void)
{
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0x01);
  temp = ER_TFT.LCD_DataRead();
  temp |= cSetb4;
    temp &= cClrb3;
  ER_TFT.LCD_DataWrite(temp);  
}

//void HostDataBus_Select_16bit(void)
void ER_TFTBasic::Host_Bus_16bit(void)
{
/*  Parallel Host Data Bus Width Selection
    0: 8-bit Parallel Host Data Bus.
    1: 16-bit Parallel Host Data Bus.*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0x01);
  temp = ER_TFT.LCD_DataRead();
  temp |= cSetb0;
  ER_TFT.LCD_DataWrite(temp);
}

//void HostColorDepthFormat(uint_t type)
void ER_TFTBasic::RGB_16b_16bpp(void)
{
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0x02);
  temp = ER_TFT.LCD_DataRead();
  temp &= cClrb7;
  temp |= cSetb6;
  ER_TFT.LCD_DataWrite(temp);
}

//void HostReadMemoryDirection(MemoryDirection direction)
void ER_TFTBasic::MemRead_Left_Right_Top_Down(void)
{
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0x02);
  temp = ER_TFT.LCD_DataRead();
  temp &= cClrb5;
  temp &= cClrb4;
  ER_TFT.LCD_DataWrite(temp);
}

//void GraphicMode(void);
void ER_TFTBasic::Graphic_Mode(void)
{
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0x03);
  temp = ER_TFT.LCD_DataRead();
    temp &= cClrb2;
  ER_TFT.LCD_DataWrite(temp);
}

//void Memory_Select_SDRAM(void)
void ER_TFTBasic::Memory_Select_SDRAM(void)
{
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0x03);
  temp = ER_TFT.LCD_DataRead();
    temp &= cClrb1;
    temp &= cClrb0; // B
  ER_TFT.LCD_DataWrite(temp);
}

void ER_TFTBasic::HSCAN_L_to_R(void)
{
/*  
Horizontal Scan Direction
0 : From Left to Right
1 : From Right to Left
PIP window will be disabled when HDIR set as 1.
*/
  unsigned char temp;
  
  ER_TFT.LCD_CmdWrite(0x12);
  temp = ER_TFT.LCD_DataRead();
  temp &= cClrb4;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::VSCAN_T_to_B(void)
{
/*  
Vertical Scan direction
0 : From Top to Bottom
1 : From bottom to Top
PIP window will be disabled when VDIR set as 1.
*/
  unsigned char temp;
  
  ER_TFT.LCD_CmdWrite(0x12);
  temp = ER_TFT.LCD_DataRead();
  temp &= cClrb3;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::PDATA_Set_RGB(void)
{
/*  
parallel PDATA[23:0] Output Sequence
000b : RGB.
001b : RBG.
010b : GRB.
011b : GBR.
100b : BRG.
101b : BGR.
*/
  unsigned char temp;
  
  ER_TFT.LCD_CmdWrite(0x12);
  temp = ER_TFT.LCD_DataRead();
    temp &=0xf8;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::PCLK_Rising(void)   
{
/*
PCLK Inversion
0: PDAT, DE, HSYNC etc. Drive(/ change) at PCLK falling edge.
1: PDAT, DE, HSYNC etc. Drive(/ change) at PCLK rising edge.
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0x12);
  temp = ER_TFT.LCD_DataRead();
    temp &= cClrb7;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::PCLK_Falling(void)
{
/*
PCLK Inversion
0: PDAT, DE, HSYNC etc. Drive(/ change) at PCLK falling edge.
1: PDAT, DE, HSYNC etc. Drive(/ change) at PCLK rising edge.
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0x12);
  temp = ER_TFT.LCD_DataRead();
    temp |= cSetb7;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::HSYNC_Low_Active(void)
{
/*  
HSYNC Polarity
0 : Low active.
1 : High active.
*/
  unsigned char temp;
  
  ER_TFT.LCD_CmdWrite(0x13);
  temp = ER_TFT.LCD_DataRead();
  temp &= cClrb7;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::HSYNC_High_Active(void)
{
/*  
HSYNC Polarity
0 : Low active.
1 : High active.
*/
  unsigned char temp;
  
  ER_TFT.LCD_CmdWrite(0x13);
  temp = ER_TFT.LCD_DataRead();   
  temp |= cSetb7;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::VSYNC_Low_Active(void)
{
/*  
VSYNC Polarity
0 : Low active.
1 : High active.
*/
  unsigned char temp;
  
  ER_TFT.LCD_CmdWrite(0x13);
  temp = ER_TFT.LCD_DataRead();
  temp &= cClrb6; 
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::VSYNC_High_Active(void)
{
/*  
VSYNC Polarity
0 : Low active.
1 : High active.
*/
  unsigned char temp;
  
  ER_TFT.LCD_CmdWrite(0x13);
  temp = ER_TFT.LCD_DataRead();
  temp |= cSetb6;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::DE_Low_Active(void)
{
/*  
DE Polarity
0 : High active.
1 : Low active.
*/
  unsigned char temp;
  
  ER_TFT.LCD_CmdWrite(0x13);
  temp = ER_TFT.LCD_DataRead();
    temp |= cSetb5;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::DE_High_Active(void)
{
/*  
DE Polarity
0 : High active.
1 : Low active.
*/
  unsigned char temp;
  
  ER_TFT.LCD_CmdWrite(0x13);
  temp = ER_TFT.LCD_DataRead();
  temp &= cClrb5;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Set_PCLK(unsigned char val)
{
  if(val == 1)  ER_TFT.PCLK_Falling();
  else      ER_TFT.PCLK_Rising();
}

void ER_TFTBasic::Set_HSYNC_Active(unsigned char val)
{
  if(val == 1)  ER_TFT.HSYNC_High_Active();
  else      ER_TFT.HSYNC_Low_Active();
}

void ER_TFTBasic::Set_VSYNC_Active(unsigned char val)
{
  if(val == 1)  ER_TFT.VSYNC_High_Active();
  else      ER_TFT.VSYNC_Low_Active();
}

void ER_TFTBasic::Set_DE_Active(unsigned char val)
{
  if(val == 1)  ER_TFT.DE_High_Active();
  else      ER_TFT.DE_Low_Active();
}
void ER_TFTBasic::LCD_HorizontalWidth_VerticalHeight(unsigned short WX,unsigned short HY)
{
  unsigned char temp;
  if(WX<8)
    {
  ER_TFT.LCD_CmdWrite(0x14);
  ER_TFT.LCD_DataWrite(0x00);
    
  ER_TFT.LCD_CmdWrite(0x15);
  ER_TFT.LCD_DataWrite(WX);
    
    temp=HY-1;
  ER_TFT.LCD_CmdWrite(0x1A);
  ER_TFT.LCD_DataWrite(temp);
      
  temp=(HY-1)>>8;
  ER_TFT.LCD_CmdWrite(0x1B);
  ER_TFT.LCD_DataWrite(temp);
  }
  else
  {
    temp=(WX/8)-1;
  ER_TFT.LCD_CmdWrite(0x14);
  ER_TFT.LCD_DataWrite(temp);
    
    temp=WX%8;
  ER_TFT.LCD_CmdWrite(0x15);
  ER_TFT.LCD_DataWrite(temp);
    
    temp=HY-1;
  ER_TFT.LCD_CmdWrite(0x1A);
  ER_TFT.LCD_DataWrite(temp);
      
  temp=(HY-1)>>8;
  ER_TFT.LCD_CmdWrite(0x1B);
  ER_TFT.LCD_DataWrite(temp);
  }
}
//[16h][17h]=========================================================================
void ER_TFTBasic::LCD_Horizontal_Non_Display(unsigned short WX)
{
  unsigned char temp;
  if(WX<8)
  {
  ER_TFT.LCD_CmdWrite(0x16);
  ER_TFT.LCD_DataWrite(0x00);
    
  ER_TFT.LCD_CmdWrite(0x17);
  ER_TFT.LCD_DataWrite(WX);
  }
  else
  {
    temp=(WX/8)-1;
  ER_TFT.LCD_CmdWrite(0x16);
  ER_TFT.LCD_DataWrite(temp);
    
    temp=WX%8;
  ER_TFT.LCD_CmdWrite(0x17);
  ER_TFT.LCD_DataWrite(temp);
  } 
}
//[18h]=========================================================================
void ER_TFTBasic::LCD_HSYNC_Start_Position(unsigned short WX)
{
  unsigned char temp;
  if(WX<8)
  {
  ER_TFT.LCD_CmdWrite(0x18);
  ER_TFT.LCD_DataWrite(0x00);
  }
  else
  {
    temp=(WX/8)-1;
  ER_TFT.LCD_CmdWrite(0x18);
  ER_TFT.LCD_DataWrite(temp);  
  }
}
//[19h]=========================================================================
void ER_TFTBasic::LCD_HSYNC_Pulse_Width(unsigned short WX)
{
  unsigned char temp;
  if(WX<8)
  {
  ER_TFT.LCD_CmdWrite(0x19);
  ER_TFT.LCD_DataWrite(0x00);
  }
  else
  {
    temp=(WX/8)-1;
  ER_TFT.LCD_CmdWrite(0x19);
  ER_TFT.LCD_DataWrite(temp);  
  }
}
//[1Ch][1Dh]=========================================================================
void ER_TFTBasic::LCD_Vertical_Non_Display(unsigned short HY)
{
  unsigned char temp;
    temp=HY-1;
  ER_TFT.LCD_CmdWrite(0x1C);
  ER_TFT.LCD_DataWrite(temp);

  ER_TFT.LCD_CmdWrite(0x1D);
  ER_TFT.LCD_DataWrite(temp>>8);
}
//[1Eh]=========================================================================
void ER_TFTBasic::LCD_VSYNC_Start_Position(unsigned short HY)
{
  unsigned char temp;
    temp=HY-1;
  ER_TFT.LCD_CmdWrite(0x1E);
  ER_TFT.LCD_DataWrite(temp);
}
//[1Fh]=========================================================================
void ER_TFTBasic::LCD_VSYNC_Pulse_Width(unsigned short HY)
{
  unsigned char temp;
    temp=HY-1;
  ER_TFT.LCD_CmdWrite(0x1F);
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Memory_XY_Mode(void) 
{
  unsigned char temp;

  ER_TFT.LCD_CmdWrite(0x5E);
  temp = ER_TFT.LCD_DataRead();
  temp &= cClrb2;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Memory_16bpp_Mode(void)  
{
  unsigned char temp;

  ER_TFT.LCD_CmdWrite(0x5E);
  temp = ER_TFT.LCD_DataRead();
  temp &= cClrb1;
  temp |= cSetb0;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Select_Main_Window_16bpp(void)
{
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0x10);
  temp = ER_TFT.LCD_DataRead();
    temp &= cClrb3;
    temp |= cSetb2;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Main_Image_Start_Address(unsigned long Addr) 
{
/*
[20h] Main Image Start Address[7:2]
[21h] Main Image Start Address[15:8]
[22h] Main Image Start Address [23:16]
[23h] Main Image Start Address [31:24]
*/
  ER_TFT.LCD_RegisterWrite(0x20,Addr);
  ER_TFT.LCD_RegisterWrite(0x21,Addr>>8);
  ER_TFT.LCD_RegisterWrite(0x22,Addr>>16);
  ER_TFT.LCD_RegisterWrite(0x23,Addr>>24);
}
void ER_TFTBasic::Main_Image_Width(unsigned short WX)  
{
/*
[24h] Main Image Width [7:0]
[25h] Main Image Width [12:8]
Unit: Pixel.
It must be divisible by 4. MIW Bit [1:0] tie to ��0�� internally.
The value is physical pixel number. Maximum value is 8188 pixels
*/
  ER_TFT.LCD_RegisterWrite(0x24,WX);
  ER_TFT.LCD_RegisterWrite(0x25,WX>>8);
}
//[26h][27h][28h][29h]=========================================================================
void ER_TFTBasic::Main_Window_Start_XY(unsigned short WX,unsigned short HY)  
{
/*
[26h] Main Window Upper-Left corner X-coordination [7:0]
[27h] Main Window Upper-Left corner X-coordination [12:8]
Reference Main Image coordination.
Unit: Pixel
It must be divisible by 4. MWULX Bit [1:0] tie to ��0�� internally.
X-axis coordination plus Horizontal display width cannot large than 8188.

[28h] Main Window Upper-Left corner Y-coordination [7:0]
[29h] Main Window Upper-Left corner Y-coordination [12:8]
Reference Main Image coordination.
Unit: Pixel
Range is between 0 and 8191.
*/
  ER_TFT.LCD_RegisterWrite(0x26,WX);
  ER_TFT.LCD_RegisterWrite(0x27,WX>>8);

  ER_TFT.LCD_RegisterWrite(0x28,HY);
  ER_TFT.LCD_RegisterWrite(0x29,HY>>8);
}
void ER_TFTBasic::Canvas_Image_Start_address(unsigned long Addr) 
{
/*
[50h] Start address of Canvas [7:0]
[51h] Start address of Canvas [15:8]
[52h] Start address of Canvas [23:16]
[53h] Start address of Canvas [31:24]
*/
  ER_TFT.LCD_RegisterWrite(0x50,Addr);
  ER_TFT.LCD_RegisterWrite(0x51,Addr>>8);
  ER_TFT.LCD_RegisterWrite(0x52,Addr>>16);
  ER_TFT.LCD_RegisterWrite(0x53,Addr>>24);
}
//[54h][55h]=========================================================================
void ER_TFTBasic::Canvas_image_width(unsigned short WX)  
{
/*
[54h] Canvas image width [7:2]
[55h] Canvas image width [12:8]
*/
  ER_TFT.LCD_RegisterWrite(0x54,WX);
  ER_TFT.LCD_RegisterWrite(0x55,WX>>8);
}
//[56h][57h][58h][59h]=========================================================================
void ER_TFTBasic::Active_Window_XY(unsigned short WX,unsigned short HY)  
{
/*
[56h] Active Window Upper-Left corner X-coordination [7:0]
[57h] Active Window Upper-Left corner X-coordination [12:8]
[58h] Active Window Upper-Left corner Y-coordination [7:0]
[59h] Active Window Upper-Left corner Y-coordination [12:8]
*/
  ER_TFT.LCD_RegisterWrite(0x56,WX);
  ER_TFT.LCD_RegisterWrite(0x57,WX>>8);
  
  ER_TFT.LCD_RegisterWrite(0x58,HY);
  ER_TFT.LCD_RegisterWrite(0x59,HY>>8);
}
//[5Ah][5Bh][5Ch][5Dh]=========================================================================
void ER_TFTBasic::Active_Window_WH(unsigned short WX,unsigned short HY)  
{
/*
[5Ah] Width of Active Window [7:0]
[5Bh] Width of Active Window [12:8]
[5Ch] Height of Active Window [7:0]
[5Dh] Height of Active Window [12:8]
*/
  ER_TFT.LCD_RegisterWrite(0x5A,WX);
  ER_TFT.LCD_RegisterWrite(0x5B,WX>>8);
 
  ER_TFT.LCD_RegisterWrite(0x5C,HY);
  ER_TFT.LCD_RegisterWrite(0x5D,HY>>8);
}
void ER_TFTBasic::Foreground_color_65k(unsigned short temp)
{
    ER_TFT.LCD_CmdWrite(0xD2);
  ER_TFT.LCD_DataWrite(temp>>8);
 
    ER_TFT.LCD_CmdWrite(0xD3);
  ER_TFT.LCD_DataWrite(temp>>3);
  
    ER_TFT.LCD_CmdWrite(0xD4);
  ER_TFT.LCD_DataWrite(temp<<3);
}

//Input data format:R5G6B6
void ER_TFTBasic::Background_color_65k(unsigned short temp)
{
    ER_TFT.LCD_CmdWrite(0xD5);
  ER_TFT.LCD_DataWrite(temp>>8);
  
    ER_TFT.LCD_CmdWrite(0xD6);
  ER_TFT.LCD_DataWrite(temp>>3);
   
    ER_TFT.LCD_CmdWrite(0xD7);
  ER_TFT.LCD_DataWrite(temp<<3);
}



void ER_TFTBasic::Check_Busy_Draw(void)
{
  unsigned char temp;
  do
  {
    temp=ER_TFT.LCD_StatusRead();
  }
  while(temp&0x08);

}

void ER_TFTBasic::Check_2D_Busy(void)
{
  do
  {
    
  }
  while( ER_TFT.LCD_StatusRead()&0x08 );
}


void ER_TFTBasic::Check_Mem_WR_FIFO_not_Full(void)
{
/*  0: Memory Write FIFO is not full.
  1: Memory Write FIFO is full.   */
  do
  {
    
  }
  while( ER_TFT.LCD_StatusRead()&0x80 );
}
void ER_TFTBasic::Check_Mem_WR_FIFO_Empty(void)
{
/*  0: Memory Write FIFO is not empty.
  1: Memory Write FIFO is empty.    */  
  do
  {
    
  }
  while( (ER_TFT.LCD_StatusRead()&0x40) == 0x00 );
}
void ER_TFTBasic::Check_Mem_RD_FIFO_not_Full(void)
{
/*  0: Memory Read FIFO is not full.
  1: Memory Read FIFO is full.    */
  do
  {
    
  }
  while( ER_TFT.LCD_StatusRead()&0x20 );
}
void ER_TFTBasic::Check_Mem_RD_FIFO_not_Empty(void)
{
/*  0: Memory Read FIFO is not empty.
  1: Memory Read FIFO is empty.
    */
  do
  {
    
  }
  while( ER_TFT.LCD_StatusRead()&0x10 );
}


void ER_TFTBasic::DrawSquare_Fill
(
 unsigned short X1                
,unsigned short Y1              
,unsigned short X2                
,unsigned short Y2              
,unsigned long ForegroundColor   
)
{
  ER_TFT.Foreground_color_65k(ForegroundColor);
  ER_TFT.Square_Start_XY(X1,Y1);
  ER_TFT.Square_End_XY(X2,Y2);
  ER_TFT.Start_Square_Fill();
  ER_TFT.Check_2D_Busy();
}

void ER_TFTBasic::DrawCircle_Fill
(
 unsigned short X1                
,unsigned short Y1              
,unsigned short R                            
,unsigned long ForegroundColor 
)

{
  ER_TFT.Foreground_color_65k(ForegroundColor);
  ER_TFT.Circle_Center_XY(X1,Y1);
  ER_TFT.Circle_Radius_R(R);
  ER_TFT.Start_Circle_or_Ellipse_Fill();
  ER_TFT.Check_2D_Busy();
}


void ER_TFTBasic::Enable_SFlash_SPI(void)
{
/*  Serial Flash SPI Interface Enable/Disable
    0: Disable
    1: Enable*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0x01);
  temp = ER_TFT.LCD_DataRead();
  temp |= cSetb1;
  ER_TFT.LCD_DataWrite(temp);     
}

void ER_TFTBasic::Goto_Pixel_XY(unsigned short WX,unsigned short HY) 
{
/*
[Write]: Set Graphic Read/Write position
[Read]: Current Graphic Read/Write position
Read back is Read position or Write position depends on
REG[5Eh] bit3, Select to read back Graphic Read/Write position.
When DPRAM Linear mode:Graphic Read/Write Position [31:24][23:16][15:8][7:0]
When DPRAM Active window mode:Graphic Read/Write 
Horizontal Position [12:8][7:0], 
Vertical Position [12:8][7:0].
Reference Canvas image coordination. Unit: Pixel
*/
  ER_TFT.LCD_RegisterWrite(0x5F,WX);
  ER_TFT.LCD_RegisterWrite(0x60,WX>>8);
  
  ER_TFT.LCD_RegisterWrite(0x61,HY);
  ER_TFT.LCD_RegisterWrite(0x62,HY>>8);
}

void ER_TFTBasic::Goto_Text_XY(unsigned short WX,unsigned short HY)  
{
/*
Write: Set Text Write position
Read: Current Text Write position
Text Write X-coordination [12:8][7:0]
Text Write Y-coordination [12:8][7:0]
Reference Canvas image coordination.
Unit: Pixel
*/
  ER_TFT.LCD_RegisterWrite(0x63,WX);
  ER_TFT.LCD_RegisterWrite(0x64,WX>>8);
  
  ER_TFT.LCD_RegisterWrite(0x65,HY);
  ER_TFT.LCD_RegisterWrite(0x66,HY>>8);
}


//[67h]=========================================================================
/*
[bit7]Draw Line / Triangle Start Signal
Write Function
0 : Stop the drawing function.
1 : Start the drawing function.
Read Function
0 : Drawing function complete.
1 : Drawing function is processing.
[bit5]Fill function for Triangle Signal
0 : Non fill.
1 : Fill.
[bit1]Draw Triangle or Line Select Signal
0 : Draw Line
1 : Draw Triangle
*/
void ER_TFTBasic::Start_Line(void)
{
  ER_TFT.LCD_CmdWrite(0x67);
  ER_TFT.LCD_DataWrite(0x80);
  Check_Busy_Draw();
}
void ER_TFTBasic::Start_Triangle(void)
{
  ER_TFT.LCD_CmdWrite(0x67);
  ER_TFT.LCD_DataWrite(0x82);//B1000_0010
  Check_Busy_Draw();
}
void ER_TFTBasic::Start_Triangle_Fill(void)
{

  ER_TFT.LCD_CmdWrite(0x67);
  ER_TFT.LCD_DataWrite(0xA2);//B1010_0010
  Check_Busy_Draw();
}
//[68h][69h][6Ah][6Bh]=========================================================================
//�����
void ER_TFTBasic::Line_Start_XY(unsigned short WX,unsigned short HY)
{
/*
[68h] Draw Line/Square/Triangle Start X-coordination [7:0]
[69h] Draw Line/Square/Triangle Start X-coordination [12:8]
[6Ah] Draw Line/Square/Triangle Start Y-coordination [7:0]
[6Bh] Draw Line/Square/Triangle Start Y-coordination [12:8]
*/
  ER_TFT.LCD_CmdWrite(0x68);
  ER_TFT.LCD_DataWrite(WX);

  ER_TFT.LCD_CmdWrite(0x69);
  ER_TFT.LCD_DataWrite(WX>>8);

  ER_TFT.LCD_CmdWrite(0x6A);
  ER_TFT.LCD_DataWrite(HY);

  ER_TFT.LCD_CmdWrite(0x6B);
  ER_TFT.LCD_DataWrite(HY>>8);
}
//[6Ch][6Dh][6Eh][6Fh]=========================================================================
//���յ�
void ER_TFTBasic::Line_End_XY(unsigned short WX,unsigned short HY)
{
/*
[6Ch] Draw Line/Square/Triangle End X-coordination [7:0]
[6Dh] Draw Line/Square/Triangle End X-coordination [12:8]
[6Eh] Draw Line/Square/Triangle End Y-coordination [7:0]
[6Fh] Draw Line/Square/Triangle End Y-coordination [12:8]
*/
  ER_TFT.LCD_CmdWrite(0x6C);
  ER_TFT.LCD_DataWrite(WX);

  ER_TFT.LCD_CmdWrite(0x6D);
  ER_TFT.LCD_DataWrite(WX>>8);

  ER_TFT.LCD_CmdWrite(0x6E);
  ER_TFT.LCD_DataWrite(HY);

  ER_TFT.LCD_CmdWrite(0x6F);
  ER_TFT.LCD_DataWrite(HY>>8);
}
//[68h]~[73h]=========================================================================
//�T��-�I1
void ER_TFTBasic::Triangle_Point1_XY(unsigned short WX,unsigned short HY)
{
/*
[68h] Draw Line/Square/Triangle Start X-coordination [7:0]
[69h] Draw Line/Square/Triangle Start X-coordination [12:8]
[6Ah] Draw Line/Square/Triangle Start Y-coordination [7:0]
[6Bh] Draw Line/Square/Triangle Start Y-coordination [12:8]
*/
  ER_TFT.LCD_CmdWrite(0x68);
  ER_TFT.LCD_DataWrite(WX);

  ER_TFT.LCD_CmdWrite(0x69);
  ER_TFT.LCD_DataWrite(WX>>8);

  ER_TFT.LCD_CmdWrite(0x6A);
  ER_TFT.LCD_DataWrite(HY);

  ER_TFT.LCD_CmdWrite(0x6B);
  ER_TFT.LCD_DataWrite(HY>>8);
}
//�T��-�I2
void ER_TFTBasic::Triangle_Point2_XY(unsigned short WX,unsigned short HY)
{
/*
[6Ch] Draw Line/Square/Triangle End X-coordination [7:0]
[6Dh] Draw Line/Square/Triangle End X-coordination [12:8]
[6Eh] Draw Line/Square/Triangle End Y-coordination [7:0]
[6Fh] Draw Line/Square/Triangle End Y-coordination [12:8]
*/
  ER_TFT.LCD_CmdWrite(0x6C);
  ER_TFT.LCD_DataWrite(WX);

  ER_TFT.LCD_CmdWrite(0x6D);
  ER_TFT.LCD_DataWrite(WX>>8);

  ER_TFT.LCD_CmdWrite(0x6E);
  ER_TFT.LCD_DataWrite(HY);

  ER_TFT.LCD_CmdWrite(0x6F);
  ER_TFT.LCD_DataWrite(HY>>8);
}
//�T��-�I3
void ER_TFTBasic::Triangle_Point3_XY (unsigned short WX,unsigned short HY)
{
/*
[70h] Draw Triangle Point 3 X-coordination [7:0]
[71h] Draw Triangle Point 3 X-coordination [12:8]
[72h] Draw Triangle Point 3 Y-coordination [7:0]
[73h] Draw Triangle Point 3 Y-coordination [12:8]
*/
  ER_TFT.LCD_CmdWrite(0x70);
  ER_TFT.LCD_DataWrite(WX);

  ER_TFT.LCD_CmdWrite(0x71);
  ER_TFT.LCD_DataWrite(WX>>8);

  ER_TFT.LCD_CmdWrite(0x72);
  ER_TFT.LCD_DataWrite(HY);

  ER_TFT.LCD_CmdWrite(0x73);
  ER_TFT.LCD_DataWrite(HY>>8);
}

void ER_TFTBasic::Square_Start_XY(unsigned short WX,unsigned short HY)
{
/*
[68h] Draw Line/Square/Triangle Start X-coordination [7:0]
[69h] Draw Line/Square/Triangle Start X-coordination [12:8]
[6Ah] Draw Line/Square/Triangle Start Y-coordination [7:0]
[6Bh] Draw Line/Square/Triangle Start Y-coordination [12:8]
*/
  ER_TFT.LCD_CmdWrite(0x68);
  ER_TFT.LCD_DataWrite(WX);

  ER_TFT.LCD_CmdWrite(0x69);
  ER_TFT.LCD_DataWrite(WX>>8);

  ER_TFT.LCD_CmdWrite(0x6A);
  ER_TFT.LCD_DataWrite(HY);

  ER_TFT.LCD_CmdWrite(0x6B);
  ER_TFT.LCD_DataWrite(HY>>8);
}

void ER_TFTBasic::Square_End_XY(unsigned short WX,unsigned short HY)
{
/*
[6Ch] Draw Line/Square/Triangle End X-coordination [7:0]
[6Dh] Draw Line/Square/Triangle End X-coordination [12:8]
[6Eh] Draw Line/Square/Triangle End Y-coordination [7:0]
[6Fh] Draw Line/Square/Triangle End Y-coordination [12:8]
*/
  ER_TFT.LCD_CmdWrite(0x6C);
  ER_TFT.LCD_DataWrite(WX);

  ER_TFT.LCD_CmdWrite(0x6D);
  ER_TFT.LCD_DataWrite(WX>>8);

  ER_TFT.LCD_CmdWrite(0x6E);
  ER_TFT.LCD_DataWrite(HY);

  ER_TFT.LCD_CmdWrite(0x6F);
  ER_TFT.LCD_DataWrite(HY>>8);
}
//[76h]=========================================================================
/*
[bit7]
Draw Circle / Ellipse / Square /Circle Square Start Signal 
Write Function
0 : Stop the drawing function.
1 : Start the drawing function.
Read Function
0 : Drawing function complete.
1 : Drawing function is processing.
[bit6]
Fill the Circle / Ellipse / Square / Circle Square Signal
0 : Non fill.
1 : fill.
[bit5 bit4]
Draw Circle / Ellipse / Square / Ellipse Curve / Circle Square Select
00 : Draw Circle / Ellipse
01 : Draw Circle / Ellipse Curve
10 : Draw Square.
11 : Draw Circle Square.
[bit1 bit0]
Draw Circle / Ellipse Curve Part Select
00 : 
01 : 
10 : 
11 : 
*/
void ER_TFTBasic::Start_Circle_or_Ellipse(void)
{
  ER_TFT.LCD_CmdWrite(0x76);
  ER_TFT.LCD_DataWrite(0x80);//B1000_XXXX
  Check_Busy_Draw();  
}
void ER_TFTBasic::Start_Circle_or_Ellipse_Fill(void)
{
  ER_TFT.LCD_CmdWrite(0x76);
  ER_TFT.LCD_DataWrite(0xC0);//B1100_XXXX
  Check_Busy_Draw();  
}
//
void ER_TFTBasic::Start_Left_Down_Curve(void)
{
  ER_TFT.LCD_CmdWrite(0x76);
  ER_TFT.LCD_DataWrite(0x90);//B1001_XX00
  Check_Busy_Draw();  
}
void ER_TFTBasic::Start_Left_Up_Curve(void)
{
  ER_TFT.LCD_CmdWrite(0x76);
  ER_TFT.LCD_DataWrite(0x91);//B1001_XX01
  Check_Busy_Draw();  
}
void ER_TFTBasic::Start_Right_Up_Curve(void)
{
  ER_TFT.LCD_CmdWrite(0x76);
  ER_TFT.LCD_DataWrite(0x92);//B1001_XX10
  Check_Busy_Draw();  
}
void ER_TFTBasic::Start_Right_Down_Curve(void)
{
  ER_TFT.LCD_CmdWrite(0x76);
  ER_TFT.LCD_DataWrite(0x93);//B1001_XX11
  Check_Busy_Draw();  
}
//
void ER_TFTBasic::Start_Left_Down_Curve_Fill(void)
{
  ER_TFT.LCD_CmdWrite(0x76);
  ER_TFT.LCD_DataWrite(0xD0);//B1101_XX00
  Check_Busy_Draw();
}
void ER_TFTBasic::Start_Left_Up_Curve_Fill(void)
{
  ER_TFT.LCD_CmdWrite(0x76);
  ER_TFT.LCD_DataWrite(0xD1);//B1101_XX01
  Check_Busy_Draw();
}
void ER_TFTBasic::Start_Right_Up_Curve_Fill(void)
{
  ER_TFT.LCD_CmdWrite(0x76);
  ER_TFT.LCD_DataWrite(0xD2);//B1101_XX10
  Check_Busy_Draw();
}
void ER_TFTBasic::Start_Right_Down_Curve_Fill(void)
{
  ER_TFT.LCD_CmdWrite(0x76);
  ER_TFT.LCD_DataWrite(0xD3);//B1101_XX11
  Check_Busy_Draw();
}
//
void ER_TFTBasic::Start_Square(void)
{
  ER_TFT.LCD_CmdWrite(0x76);
  ER_TFT.LCD_DataWrite(0xA0);//B1010_XXXX
  Check_Busy_Draw();
}
void ER_TFTBasic::Start_Square_Fill(void)
{
  ER_TFT.LCD_CmdWrite(0x76);
  ER_TFT.LCD_DataWrite(0xE0);//B1110_XXXX
  Check_Busy_Draw();
}
void ER_TFTBasic::Start_Circle_Square(void)
{
  ER_TFT.LCD_CmdWrite(0x76);
  ER_TFT.LCD_DataWrite(0xB0);//B1011_XXXX
  Check_Busy_Draw();  
}
void ER_TFTBasic::Start_Circle_Square_Fill(void)
{
  ER_TFT.LCD_CmdWrite(0x76);
  ER_TFT.LCD_DataWrite(0xF0);//B1111_XXXX
  Check_Busy_Draw();  
}
//[77h]~[7Eh]=========================================================================

void ER_TFTBasic::Circle_Center_XY(unsigned short WX,unsigned short HY)
{
/*
[7Bh] Draw Circle/Ellipse/Circle Square Center X-coordination [7:0]
[7Ch] Draw Circle/Ellipse/Circle Square Center X-coordination [12:8]
[7Dh] Draw Circle/Ellipse/Circle Square Center Y-coordination [7:0]
[7Eh] Draw Circle/Ellipse/Circle Square Center Y-coordination [12:8]
*/
  ER_TFT.LCD_CmdWrite(0x7B);
  ER_TFT.LCD_DataWrite(WX);

  ER_TFT.LCD_CmdWrite(0x7C);
  ER_TFT.LCD_DataWrite(WX>>8);

  ER_TFT.LCD_CmdWrite(0x7D);
  ER_TFT.LCD_DataWrite(HY);

  ER_TFT.LCD_CmdWrite(0x7E);
  ER_TFT.LCD_DataWrite(HY>>8);
}

void ER_TFTBasic::Ellipse_Center_XY(unsigned short WX,unsigned short HY)
{
/*
[7Bh] Draw Circle/Ellipse/Circle Square Center X-coordination [7:0]
[7Ch] Draw Circle/Ellipse/Circle Square Center X-coordination [12:8]
[7Dh] Draw Circle/Ellipse/Circle Square Center Y-coordination [7:0]
[7Eh] Draw Circle/Ellipse/Circle Square Center Y-coordination [12:8]
*/
  ER_TFT.LCD_CmdWrite(0x7B);
  ER_TFT.LCD_DataWrite(WX);

  ER_TFT.LCD_CmdWrite(0x7C);
  ER_TFT.LCD_DataWrite(WX>>8);

  ER_TFT.LCD_CmdWrite(0x7D);
  ER_TFT.LCD_DataWrite(HY);

  ER_TFT.LCD_CmdWrite(0x7E);
  ER_TFT.LCD_DataWrite(HY>>8);
}

void ER_TFTBasic::Circle_Radius_R(unsigned short WX)
{
/*
[77h] Draw Circle/Ellipse/Circle Square Major radius [7:0]
[78h] Draw Circle/Ellipse/Circle Square Major radius [12:8]
[79h] Draw Circle/Ellipse/Circle Square Minor radius [7:0]
[7Ah] Draw Circle/Ellipse/Circle Square Minor radius [12:8]
*/
  ER_TFT.LCD_CmdWrite(0x77);
  ER_TFT.LCD_DataWrite(WX);

  ER_TFT.LCD_CmdWrite(0x78);
  ER_TFT.LCD_DataWrite(WX>>8);

  ER_TFT.LCD_CmdWrite(0x79);
  ER_TFT.LCD_DataWrite(WX);

  ER_TFT.LCD_CmdWrite(0x7A);
  ER_TFT.LCD_DataWrite(WX>>8);
}


void ER_TFTBasic::Ellipse_Radius_RxRy(unsigned short WX,unsigned short HY)
{
/*
[77h] Draw Circle/Ellipse/Circle Square Major radius [7:0]
[78h] Draw Circle/Ellipse/Circle Square Major radius [12:8]
[79h] Draw Circle/Ellipse/Circle Square Minor radius [7:0]
[7Ah] Draw Circle/Ellipse/Circle Square Minor radius [12:8]
*/
  ER_TFT.LCD_CmdWrite(0x77);
  ER_TFT.LCD_DataWrite(WX);

  ER_TFT.LCD_CmdWrite(0x78);
  ER_TFT.LCD_DataWrite(WX>>8);

  ER_TFT.LCD_CmdWrite(0x79);
  ER_TFT.LCD_DataWrite(HY);

  ER_TFT.LCD_CmdWrite(0x7A);
  ER_TFT.LCD_DataWrite(HY>>8);
}


void ER_TFTBasic::Circle_Square_Radius_RxRy(unsigned short WX,unsigned short HY)
{
/*
[77h] Draw Circle/Ellipse/Circle Square Major radius [7:0]
[78h] Draw Circle/Ellipse/Circle Square Major radius [12:8]
[79h] Draw Circle/Ellipse/Circle Square Minor radius [7:0]
[7Ah] Draw Circle/Ellipse/Circle Square Minor radius [12:8]
*/
  ER_TFT.LCD_CmdWrite(0x77);
  ER_TFT.LCD_DataWrite(WX);

  ER_TFT.LCD_CmdWrite(0x78);
  ER_TFT.LCD_DataWrite(WX>>8);

  ER_TFT.LCD_CmdWrite(0x79);
  ER_TFT.LCD_DataWrite(HY);

  ER_TFT.LCD_CmdWrite(0x7A);
  ER_TFT.LCD_DataWrite(HY>>8);
}

//[84h]=========================================================================
void ER_TFTBasic::Set_PWM_Prescaler_1_to_256(unsigned short WX)
{
/*
PWM Prescaler Register
These 8 bits determine prescaler value for Timer 0 and 1.
Time base is ��Core_Freq / (Prescaler + 1)��
*/
  WX=WX-1;
  ER_TFT.LCD_CmdWrite(0x84);
  ER_TFT.LCD_DataWrite(WX);
}
//[85h]=========================================================================
void ER_TFTBasic::Select_PWM1_Clock_Divided_By_1(void)
{
/*
Select MUX input for PWM Timer 1.
00 = 1; 01 = 1/2; 10 = 1/4 ; 11 = 1/8;
*/
  unsigned char temp;
  
  ER_TFT.LCD_CmdWrite(0x85);
  temp = ER_TFT.LCD_DataRead();
  temp &= cClrb7;
  temp &= cClrb6;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Select_PWM1_Clock_Divided_By_2(void)
{
/*
Select MUX input for PWM Timer 1.
00 = 1; 01 = 1/2; 10 = 1/4 ; 11 = 1/8;
*/
  unsigned char temp;
  
  ER_TFT.LCD_CmdWrite(0x85);
  temp = ER_TFT.LCD_DataRead();
  temp &= cClrb7;
  temp |= cSetb6;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Select_PWM1_Clock_Divided_By_4(void)
{
/*
Select MUX input for PWM Timer 1.
00 = 1; 01 = 1/2; 10 = 1/4 ; 11 = 1/8;
*/
  unsigned char temp;
  
  ER_TFT.LCD_CmdWrite(0x85);
  temp = ER_TFT.LCD_DataRead();
  temp |= cSetb7;
  temp &= cClrb6;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Select_PWM1_Clock_Divided_By_8(void)
{
/*
Select MUX input for PWM Timer 1.
00 = 1; 01 = 1/2; 10 = 1/4 ; 11 = 1/8;
*/
  unsigned char temp;
  
  ER_TFT.LCD_CmdWrite(0x85);
  temp = ER_TFT.LCD_DataRead();
  temp |= cSetb7;
  temp |= cSetb6;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Select_PWM0_Clock_Divided_By_1(void)
{
/*
Select MUX input for PWM Timer 0.
00 = 1; 01 = 1/2; 10 = 1/4 ; 11 = 1/8;
*/
  unsigned char temp;
  
  ER_TFT.LCD_CmdWrite(0x85);
  temp = ER_TFT.LCD_DataRead();
  temp &= cClrb5;
  temp &= cClrb4;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Select_PWM0_Clock_Divided_By_2(void)
{
/*
Select MUX input for PWM Timer 0.
00 = 1; 01 = 1/2; 10 = 1/4 ; 11 = 1/8;
*/
  unsigned char temp;
  
  ER_TFT.LCD_CmdWrite(0x85);
  temp = ER_TFT.LCD_DataRead();
  temp &= cClrb5;
  temp |= cSetb4;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Select_PWM0_Clock_Divided_By_4(void)
{
/*
Select MUX input for PWM Timer 0.
00 = 1; 01 = 1/2; 10 = 1/4 ; 11 = 1/8;
*/
  unsigned char temp;
  
  ER_TFT.LCD_CmdWrite(0x85);
  temp = ER_TFT.LCD_DataRead();
  temp |= cSetb5;
  temp &= cClrb4;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Select_PWM0_Clock_Divided_By_8(void)
{
/*
Select MUX input for PWM Timer 0.
00 = 1; 01 = 1/2; 10 = 1/4 ; 11 = 1/8;
*/
  unsigned char temp;
  
  ER_TFT.LCD_CmdWrite(0x85);
  temp = ER_TFT.LCD_DataRead();
  temp |= cSetb5;
  temp |= cSetb4;
  ER_TFT.LCD_DataWrite(temp);
}
//[85h].[bit3][bit2]=========================================================================
/*
XPWM[1] pin function control
0X: XPWM[1] output system error flag (REG[00h] bit[1:0], Scan bandwidth insufficient + Memory access out of range)
10: XPWM[1] enabled and controlled by PWM timer 1
11: XPWM[1] output oscillator clock
//If XTEST[0] set high, then XPWM[1] will become panel scan clock input.
*/
void ER_TFTBasic::Select_PWM1_is_ErrorFlag(void)
{
  unsigned char temp;
  
  ER_TFT.LCD_CmdWrite(0x85);
  temp = ER_TFT.LCD_DataRead();
  temp &= cClrb3;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Select_PWM1(void)
{
  unsigned char temp;
  
  ER_TFT.LCD_CmdWrite(0x85);
  temp = ER_TFT.LCD_DataRead();
  temp |= cSetb3;
  temp &= cClrb2;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Select_PWM1_is_Osc_Clock(void)
{
  unsigned char temp;
  
  ER_TFT.LCD_CmdWrite(0x85);
  temp = ER_TFT.LCD_DataRead();
  temp |= cSetb3;
  temp |= cSetb2;
  ER_TFT.LCD_DataWrite(temp);
}
//[85h].[bit1][bit0]=========================================================================
/*
XPWM[0] pin function control
0X: XPWM[0] becomes GPIO-C[7]
10: XPWM[0] enabled and controlled by PWM timer 0
11: XPWM[0] output core clock
*/
void ER_TFTBasic::Select_PWM0_is_GPIO_C7(void)
{
  unsigned char temp;
  
  ER_TFT.LCD_CmdWrite(0x85);
  temp = ER_TFT.LCD_DataRead();
  temp &= cClrb1;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Select_PWM0(void)
{
  unsigned char temp;
  
  ER_TFT.LCD_CmdWrite(0x85);
  temp = ER_TFT.LCD_DataRead();
  temp |= cSetb1;
  temp &= cClrb0;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Select_PWM0_is_Core_Clock(void)
{
  unsigned char temp;
  
  ER_TFT.LCD_CmdWrite(0x85);
  temp = ER_TFT.LCD_DataRead();
  temp |= cSetb1;
  temp |= cSetb0;
  ER_TFT.LCD_DataWrite(temp);
}
//[86h]=========================================================================
//[86h]PWM1
void ER_TFTBasic::Enable_PWM1_Inverter(void)
{
/*
PWM Timer 1 output inverter on/off.
Determine the output inverter on/off for Timer 1. 
0 = Inverter off 
1 = Inverter on for PWM1
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0x86);
  temp = ER_TFT.LCD_DataRead();
  temp |= cSetb6;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Disable_PWM1_Inverter(void)
{
/*
PWM Timer 1 output inverter on/off.
Determine the output inverter on/off for Timer 1. 
0 = Inverter off 
1 = Inverter on for PWM1
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0x86);
  temp = ER_TFT.LCD_DataRead();
  temp &= cClrb6;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Auto_Reload_PWM1(void)
{
/*
PWM Timer 1 auto reload on/off
Determine auto reload on/off for Timer 1. 
0 = One-shot 
1 = Interval mode(auto reload)
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0x86);
  temp = ER_TFT.LCD_DataRead();
  temp |= cSetb5;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::One_Shot_PWM1(void)
{
/*
PWM Timer 1 auto reload on/off
Determine auto reload on/off for Timer 1. 
0 = One-shot 
1 = Interval mode(auto reload)
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0x86);
  temp = ER_TFT.LCD_DataRead();
  temp &= cClrb5;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Start_PWM1(void)
{
/*
PWM Timer 1 start/stop
Determine start/stop for Timer 1. 
0 = Stop 
1 = Start for Timer 1
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0x86);
  temp = ER_TFT.LCD_DataRead();
  temp |= cSetb4;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Stop_PWM1(void)
{
/*
PWM Timer 1 start/stop
Determine start/stop for Timer 1. 
0 = Stop 
1 = Start for Timer 1
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0x86);
  temp = ER_TFT.LCD_DataRead();
  temp &= cClrb4;
  ER_TFT.LCD_DataWrite(temp);
}
//[86h]PWM0
void ER_TFTBasic::Enable_PWM0_Dead_Zone(void)
{
/*
PWM Timer 0 Dead zone enable
Determine the dead zone operation. 0 = Disable. 1 = Enable.
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0x86);
  temp = ER_TFT.LCD_DataRead();
  temp |= cSetb3;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Disable_PWM0_Dead_Zone(void)
{
/*
PWM Timer 0 Dead zone enable
Determine the dead zone operation. 0 = Disable. 1 = Enable.
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0x86);
  temp = ER_TFT.LCD_DataRead();
  temp &= cClrb3;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Enable_PWM0_Inverter(void)
{
/*
PWM Timer 0 output inverter on/off
Determine the output inverter on/off for Timer 0. 
0 = Inverter off 
1 = Inverter on for PWM0
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0x86);
  temp = ER_TFT.LCD_DataRead();
  temp |= cSetb2;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Disable_PWM0_Inverter(void)
{
/*
PWM Timer 0 output inverter on/off
Determine the output inverter on/off for Timer 0. 
0 = Inverter off 
1 = Inverter on for PWM0
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0x86);
  temp = ER_TFT.LCD_DataRead();
  temp &= cClrb2;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Auto_Reload_PWM0(void)
{
/*
PWM Timer 0 auto reload on/off
Determine auto reload on/off for Timer 0. 
0 = One-shot 
1 = Interval mode(auto reload)
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0x86);
  temp = ER_TFT.LCD_DataRead();
  temp |= cSetb1;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::One_Shot_PWM0(void)
{
/*
PWM Timer 1 auto reload on/off
Determine auto reload on/off for Timer 1. 
0 = One-shot 
1 = Interval mode(auto reload)
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0x86);
  temp = ER_TFT.LCD_DataRead();
  temp &= cClrb1;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Start_PWM0(void)
{
/*
PWM Timer 0 start/stop
Determine start/stop for Timer 0. 
0 = Stop 
1 = Start for Timer 0
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0x86);
  temp = ER_TFT.LCD_DataRead();
  temp |= cSetb0;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Stop_PWM0(void)
{
/*
PWM Timer 0 start/stop
Determine start/stop for Timer 0. 
0 = Stop 
1 = Start for Timer 0
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0x86);
  temp = ER_TFT.LCD_DataRead();
  temp &= cClrb0;
  ER_TFT.LCD_DataWrite(temp);
}
//[87h]=========================================================================
void ER_TFTBasic::Set_Timer0_Dead_Zone_Length(unsigned char temp)
{
/*
Timer 0 Dead zone length register
These 8 bits determine the dead zone length. The 1 unit time of
the dead zone length is equal to that of timer 0.
*/
  ER_TFT.LCD_CmdWrite(0x87);
  ER_TFT.LCD_DataWrite(temp);
}
//[88h][89h]=========================================================================
void ER_TFTBasic::Set_Timer0_Compare_Buffer(unsigned short WX)
{
/*
Timer 0 compare buffer register
Compare buffer register total has 16 bits.
When timer counter equal or less than compare buffer register will cause PWM out
high level if inv_on bit is off.
*/
  ER_TFT.LCD_CmdWrite(0x88);
  ER_TFT.LCD_DataWrite(WX);
  ER_TFT.LCD_CmdWrite(0x89);
  ER_TFT.LCD_DataWrite(WX>>8);
}
//[8Ah][8Bh]=========================================================================
void ER_TFTBasic::Set_Timer0_Count_Buffer(unsigned short WX)
{
/*
Timer 0 count buffer register
Count buffer register total has 16 bits.
When timer counter equal to 0 will cause PWM timer reload Count buffer register if reload_en bit set as enable.
It may read back timer counter��s real time value when PWM timer start.
*/
  ER_TFT.LCD_CmdWrite(0x8A);
  ER_TFT.LCD_DataWrite(WX);
  ER_TFT.LCD_CmdWrite(0x8B);
  ER_TFT.LCD_DataWrite(WX>>8);
}
//[8Ch][8Dh]=========================================================================
void ER_TFTBasic::Set_Timer1_Compare_Buffer(unsigned short WX)
{
/*
Timer 0 compare buffer register
Compare buffer register total has 16 bits.
When timer counter equal or less than compare buffer register will cause PWM out
high level if inv_on bit is off.
*/
  ER_TFT.LCD_CmdWrite(0x8C);
  ER_TFT.LCD_DataWrite(WX);
  ER_TFT.LCD_CmdWrite(0x8D);
  ER_TFT.LCD_DataWrite(WX>>8);
}
//[8Eh][8Fh]=========================================================================
void ER_TFTBasic::Set_Timer1_Count_Buffer(unsigned short WX)
{
/*
Timer 0 count buffer register
Count buffer register total has 16 bits.
When timer counter equal to 0 will cause PWM timer reload Count buffer register if reload_en bit set as enable.
It may read back timer counter��s real time value when PWM timer start.
*/
  ER_TFT.LCD_CmdWrite(0x8E);
  ER_TFT.LCD_DataWrite(WX);
  ER_TFT.LCD_CmdWrite(0x8F);
  ER_TFT.LCD_DataWrite(WX>>8);
}


//[90h]~[B5h]=========================================================================

//[90h]=========================================================================
void ER_TFTBasic::BTE_Enable(void)
{ 
/*
BTE Function Enable
0 : BTE Function disable.
1 : BTE Function enable.
*/
    unsigned char temp;
    ER_TFT.LCD_CmdWrite(0x90);
    temp = ER_TFT.LCD_DataRead();
    temp |= cSetb4 ;
  ER_TFT.LCD_DataWrite(temp);  
}

//[90h]=========================================================================
void ER_TFTBasic::BTE_Disable(void)
{ 
/*
BTE Function Enable
0 : BTE Function disable.
1 : BTE Function enable.
*/
    unsigned char temp;
    ER_TFT.LCD_CmdWrite(0x90);
    temp = ER_TFT.LCD_DataRead();
    temp &= cClrb4 ;
  ER_TFT.LCD_DataWrite(temp);  
}

//[90h]=========================================================================
void ER_TFTBasic::Check_BTE_Busy(void)
{ 
/*
BTE Function Status
0 : BTE Function is idle.
1 : BTE Function is busy.
*/
  unsigned char temp;   
  do
  {
    temp=ER_TFT.LCD_StatusRead();
  }while(temp&0x08);

}
//[90h]=========================================================================
void ER_TFTBasic::Pattern_Format_8X8(void)
{ 
/*
Pattern Format
0 : 8X8
1 : 16X16
*/
    unsigned char temp;
    ER_TFT.LCD_CmdWrite(0x90);
    temp = ER_TFT.LCD_DataRead();
    temp &= cClrb0 ;
  ER_TFT.LCD_DataWrite(temp);
} 
//[90h]=========================================================================
void ER_TFTBasic::Pattern_Format_16X16(void)
{ 
/*
Pattern Format
0 : 8X8
1 : 16X16
*/
    unsigned char temp;
    ER_TFT.LCD_CmdWrite(0x90);
    temp = ER_TFT.LCD_DataRead();
    temp |= cSetb0 ;
    ER_TFT.LCD_DataWrite(temp);
} 

//[91h]=========================================================================
void ER_TFTBasic::BTE_ROP_Code(unsigned char setx)
{ 
/*
BTE ROP Code[Bit7:4]
  
0000 : 0(Blackness)
0001 : ~S0.~S1 or ~ ( S0+S1 )
0010 : ~S0.S1
0011 : ~S0
0100 : S0.~S1
0101 : ~S1
0110 : S0^S1
0111 : ~S0+~S1 or ~ ( S0.S1 )
1000 : S0.S1
1001 : ~ ( S0^S1 )
1010 : S1
1011 : ~S0+S1
1100 : S0
1101 : S0+~S1
1110 : S0+S1
1111 : 1 ( Whiteness )
*/
    unsigned char temp;
    ER_TFT.LCD_CmdWrite(0x91);
    temp = ER_TFT.LCD_DataRead();
    temp &= 0x0f ;
    temp |= (setx<<4);
    ER_TFT.LCD_DataWrite(temp);
}
  
//[91h]=========================================================================
void ER_TFTBasic::BTE_Operation_Code(unsigned char setx)
{ 
/*
BTE Operation Code[Bit3:0]
  
0000 : MPU Write BTE with ROP.
0001 : MPU Read BTE w/o ROP.
0010 : Memory copy (move) BTE in positive direction with ROP.
0011 : Memory copy (move) BTE in negative direction with ROP.
0100 : MPU Transparent Write BTE. (w/o ROP.)
0101 : Transparent Memory copy (move) BTE in positive direction (w/o ROP.)
0110 : Pattern Fill with ROP.
0111 : Pattern Fill with key-chroma
1000 : Color Expansion
1001 : Color Expansion with transparency
1010 : Move BTE in positive direction with Alpha blending
1011 : MPU Write BTE with Alpha blending
1100 : Solid Fill
1101 : Reserved
1110 : Reserved
1111 : Reserved
*/
    unsigned char temp;
    ER_TFT.LCD_CmdWrite(0x91);
    temp = ER_TFT.LCD_DataRead();
    temp &= 0xf0 ;
    temp |= setx ;
    ER_TFT.LCD_DataWrite(temp);

}
//[92h]=========================================================================
void ER_TFTBasic::BTE_S0_Color_8bpp(void)
{ 
/*
S0 Color Depth
00 : 256 Color
01 : 64k Color
1x : 16M Color
*/  
    unsigned char temp;
    ER_TFT.LCD_CmdWrite(0x92);
    temp = ER_TFT.LCD_DataRead();
    temp &= cClrb6 ;
    temp &= cClrb5 ;
    ER_TFT.LCD_DataWrite(temp);
} 
//[92h]=========================================================================
void ER_TFTBasic::BTE_S0_Color_16bpp(void)
{ 
/*
S0 Color Depth
00 : 256 Color
01 : 64k Color
1x : 16M Color
*/  
    unsigned char temp;
    ER_TFT.LCD_CmdWrite(0x92);
    temp = ER_TFT.LCD_DataRead();
    temp &= cClrb6 ;
    temp |= cSetb5 ;
    ER_TFT.LCD_DataWrite(temp);

} 
//[92h]=========================================================================
void ER_TFTBasic::BTE_S0_Color_24bpp(void)
{ 
/*
S0 Color Depth
00 : 256 Color
01 : 64k Color
1x : 16M Color
*/  
    unsigned char temp;
    ER_TFT.LCD_CmdWrite(0x92);
    temp = ER_TFT.LCD_DataRead();
    temp |= cSetb6 ;
    //temp |= cSetb5 ;
    ER_TFT.LCD_DataWrite(temp);
}
//[92h]=========================================================================
void ER_TFTBasic::BTE_S1_Color_8bpp(void)
{ 
/*
S1 Color Depth
000 : 256 Color
001 : 64k Color
010 : 16M Color
011 : Constant Color
100 : 8 bit pixel alpha blending
101 : 16 bit pixel alpha blending
*/  
    unsigned char temp;
    ER_TFT.LCD_CmdWrite(0x92);
    temp = ER_TFT.LCD_DataRead();
    temp &= cClrb4 ;
    temp &= cClrb3 ;
    temp &= cClrb2 ;
    ER_TFT.LCD_DataWrite(temp);
} 
//[92h]=========================================================================
void ER_TFTBasic::BTE_S1_Color_16bpp(void)
{ 
/*
S1 Color Depth
000 : 256 Color
001 : 64k Color
010 : 16M Color
011 : Constant Color
100 : 8 bit pixel alpha blending
101 : 16 bit pixel alpha blending
*/  
    unsigned char temp;
    ER_TFT.LCD_CmdWrite(0x92);
    temp = ER_TFT.LCD_DataRead();
    temp &= cClrb4 ;
    temp &= cClrb3 ;
    temp |= cSetb2 ;
    ER_TFT.LCD_DataWrite(temp);

}
//[92h]=========================================================================
void ER_TFTBasic::BTE_S1_Color_24bpp(void)
{ 
/*
S1 Color Depth
000 : 256 Color
001 : 64k Color
010 : 16M Color
011 : Constant Color
100 : 8 bit pixel alpha blending
101 : 16 bit pixel alpha blending
*/  
    unsigned char temp;
    ER_TFT.LCD_CmdWrite(0x92);
    temp = ER_TFT.LCD_DataRead();
    temp &= cClrb4 ;
    temp |= cSetb3 ;
    temp &= cClrb2 ;
    ER_TFT.LCD_DataWrite(temp);
}

//[92h]=========================================================================
void ER_TFTBasic::BTE_S1_Color_Constant(void)
{ 
/*
S1 Color Depth
000 : 256 Color
001 : 64k Color
010 : 16M Color
011 : Constant Color
100 : 8 bit pixel alpha blending
101 : 16 bit pixel alpha blending
*/  
    unsigned char temp;
    ER_TFT.LCD_CmdWrite(0x92);
    temp = ER_TFT.LCD_DataRead();
    temp &= cClrb4 ;
    temp |= cSetb3 ;
    temp |= cSetb2 ;
    ER_TFT.LCD_DataWrite(temp);
}



//[92h]=========================================================================
void ER_TFTBasic::BTE_S1_Color_8bit_Alpha(void)
{ 
/*
S1 Color Depth
000 : 256 Color
001 : 64k Color
010 : 16M Color
011 : Constant Color
100 : 8 bit pixel alpha blending
101 : 16 bit pixel alpha blending
*/  
    unsigned char temp;
    ER_TFT.LCD_CmdWrite(0x92);
    temp = ER_TFT.LCD_DataRead();
    temp |= cSetb4 ;
    temp &= cClrb3 ;
    temp &= cClrb2 ;
    ER_TFT.LCD_DataWrite(temp);
}

//[92h]=========================================================================
void ER_TFTBasic::BTE_S1_Color_16bit_Alpha(void)
{ 
/*
S1 Color Depth
000 : 256 Color
001 : 64k Color
010 : 16M Color
011 : Constant Color
100 : 8 bit pixel alpha blending
101 : 16 bit pixel alpha blending
*/  
    unsigned char temp;
    ER_TFT.LCD_CmdWrite(0x92);
    temp = ER_TFT.LCD_DataRead();
    temp |= cSetb4 ;
    temp &= cClrb3 ;
    temp |= cSetb2 ;
    ER_TFT.LCD_DataWrite(temp);
}

//[92h]=========================================================================
void ER_TFTBasic::BTE_Destination_Color_8bpp(void)
{ 
/*
Destination Color Depth
00 : 256 Color
01 : 64k Color
1x : 16M Color
*/  
    unsigned char temp;
    ER_TFT.LCD_CmdWrite(0x92);
    temp = ER_TFT.LCD_DataRead();
    temp &= cClrb1 ;
    temp &= cClrb0 ;
    ER_TFT.LCD_DataWrite(temp);
} 
//[92h]=========================================================================
void ER_TFTBasic::BTE_Destination_Color_16bpp(void)
{ 
/*
Destination Color Depth
00 : 256 Color
01 : 64k Color
1x : 16M Color
*/  
    unsigned char temp;
    ER_TFT.LCD_CmdWrite(0x92);
    temp = ER_TFT.LCD_DataRead();
    temp &= cClrb1 ;
    temp |= cSetb0 ;
    ER_TFT.LCD_DataWrite(temp);

} 
//[92h]=========================================================================
void ER_TFTBasic::BTE_Destination_Color_24bpp(void)
{ 
/*
Destination Color Depth
00 : 256 Color
10 : 64k Color
1x : 16M Color
*/  
    unsigned char temp;
    ER_TFT.LCD_CmdWrite(0x92);
    temp = ER_TFT.LCD_DataRead();
    temp |= cSetb1 ;
    //temp |= cSetb0 ;
    ER_TFT.LCD_DataWrite(temp);
}


//[93h][94h][95h][96h]=========================================================================
void ER_TFTBasic::BTE_S0_Memory_Start_Address(unsigned long Addr)  
{
/*
[93h] BTE S0 Memory Start Address [7:0]
[94h] BTE S0 Memory Start Address [15:8]
[95h] BTE S0 Memory Start Address [23:16]
[96h] BTE S0 Memory Start Address [31:24]
Bit [1:0] tie to ��0�� internally.
*/
  ER_TFT.LCD_RegisterWrite(0x93,Addr);
  ER_TFT.LCD_RegisterWrite(0x94,Addr>>8);
  ER_TFT.LCD_RegisterWrite(0x95,Addr>>16);
  ER_TFT.LCD_RegisterWrite(0x96,Addr>>24);
}


//[97h][98h]=========================================================================
void ER_TFTBasic::BTE_S0_Image_Width(unsigned short WX)  
{
/*
[97h] BTE S0 Image Width [7:0]
[98h] BTE S0 Image Width [12:8]
Unit: Pixel.
Bit [1:0] tie to ��0�� internally.
*/
  ER_TFT.LCD_RegisterWrite(0x97,WX);
  ER_TFT.LCD_RegisterWrite(0x98,WX>>8);
}


//[99h][9Ah][9Bh][9Ch]=========================================================================
void ER_TFTBasic::BTE_S0_Window_Start_XY(unsigned short WX,unsigned short HY)  
{
/*
[99h] BTE S0 Window Upper-Left corner X-coordination [7:0]
[9Ah] BTE S0 Window Upper-Left corner X-coordination [12:8]
[9Bh] BTE S0 Window Upper-Left corner Y-coordination [7:0]
[9Ch] BTE S0 Window Upper-Left corner Y-coordination [12:8]
*/
  ER_TFT.LCD_RegisterWrite(0x99,WX);
  ER_TFT.LCD_RegisterWrite(0x9A,WX>>8);

  ER_TFT.LCD_RegisterWrite(0x9B,HY);
  ER_TFT.LCD_RegisterWrite(0x9C,HY>>8);
}




//[9Dh][9Eh][9Fh][A0h]=========================================================================
void ER_TFTBasic::BTE_S1_Memory_Start_Address(unsigned long Addr)  
{
/*
[9Dh] BTE S1 Memory Start Address [7:0]
[9Eh] BTE S1 Memory Start Address [15:8]
[9Fh] BTE S1 Memory Start Address [23:16]
[A0h] BTE S1 Memory Start Address [31:24]
Bit [1:0] tie to ��0�� internally.
*/
  ER_TFT.LCD_RegisterWrite(0x9D,Addr);
  ER_TFT.LCD_RegisterWrite(0x9E,Addr>>8);
  ER_TFT.LCD_RegisterWrite(0x9F,Addr>>16);
  ER_TFT.LCD_RegisterWrite(0xA0,Addr>>24);
}


//Input data format:R3G3B2
void ER_TFTBasic::S1_Constant_color_256(unsigned char temp)
{
    ER_TFT.LCD_CmdWrite(0x9D);
    ER_TFT.LCD_DataWrite(temp);

    ER_TFT.LCD_CmdWrite(0x9E);
    ER_TFT.LCD_DataWrite(temp<<3);

    ER_TFT.LCD_CmdWrite(0x9F);
    ER_TFT.LCD_DataWrite(temp<<6);
}

//Input data format:R5G6B6
void ER_TFTBasic::S1_Constant_color_65k(unsigned short temp)
{
    ER_TFT.LCD_CmdWrite(0x9D);
    ER_TFT.LCD_DataWrite(temp>>8);

    ER_TFT.LCD_CmdWrite(0x9E);
    ER_TFT.LCD_DataWrite(temp>>3);

    ER_TFT.LCD_CmdWrite(0x9F);
    ER_TFT.LCD_DataWrite(temp<<3);
}

//Input data format:R8G8B8
void ER_TFTBasic::S1_Constant_color_16M(unsigned long temp)
{
    ER_TFT.LCD_CmdWrite(0x9D);
    ER_TFT.LCD_DataWrite(temp>>16);

    ER_TFT.LCD_CmdWrite(0x9E);
    ER_TFT.LCD_DataWrite(temp>>8);

    ER_TFT.LCD_CmdWrite(0x9F);
    ER_TFT.LCD_DataWrite(temp);
}




//[A1h][A2h]=========================================================================
void ER_TFTBasic::BTE_S1_Image_Width(unsigned short WX)  
{
/*
[A1h] BTE S1 Image Width [7:0]
[A2h] BTE S1 Image Width [12:8]
Unit: Pixel.
Bit [1:0] tie to ��0�� internally.
*/
  ER_TFT.LCD_RegisterWrite(0xA1,WX);
  ER_TFT.LCD_RegisterWrite(0xA2,WX>>8);
}


//[A3h][A4h][A5h][A6h]=========================================================================
void ER_TFTBasic::BTE_S1_Window_Start_XY(unsigned short WX,unsigned short HY)  
{
/*
[A3h] BTE S1 Window Upper-Left corner X-coordination [7:0]
[A4h] BTE S1 Window Upper-Left corner X-coordination [12:8]
[A5h] BTE S1 Window Upper-Left corner Y-coordination [7:0]
[A6h] BTE S1 Window Upper-Left corner Y-coordination [12:8]
*/
  ER_TFT.LCD_RegisterWrite(0xA3,WX);
  ER_TFT.LCD_RegisterWrite(0xA4,WX>>8);

  ER_TFT.LCD_RegisterWrite(0xA5,HY);
  ER_TFT.LCD_RegisterWrite(0xA6,HY>>8);
}




//[A7h][A8h][A9h][AAh]=========================================================================
void ER_TFTBasic::BTE_Destination_Memory_Start_Address(unsigned long Addr) 
{
/*
[A7h] BTE Destination Memory Start Address [7:0]
[A8h] BTE Destination Memory Start Address [15:8]
[A9h] BTE Destination Memory Start Address [23:16]
[AAh] BTE Destination Memory Start Address [31:24]
Bit [1:0] tie to ��0�� internally.
*/
  ER_TFT.LCD_RegisterWrite(0xA7,Addr);
  ER_TFT.LCD_RegisterWrite(0xA8,Addr>>8);
  ER_TFT.LCD_RegisterWrite(0xA9,Addr>>16);
  ER_TFT.LCD_RegisterWrite(0xAA,Addr>>24);
}


//[ABh][ACh]=========================================================================
void ER_TFTBasic::BTE_Destination_Image_Width(unsigned short WX) 
{
/*
[ABh] BTE Destination Image Width [7:0]
[ACh] BTE Destination Image Width [12:8]
Unit: Pixel.
Bit [1:0] tie to ��0�� internally.
*/
  ER_TFT.LCD_RegisterWrite(0xAB,WX);
  ER_TFT.LCD_RegisterWrite(0xAC,WX>>8);
}


//[ADh][AEh][AFh][B0h]=========================================================================
void ER_TFTBasic::BTE_Destination_Window_Start_XY(unsigned short WX,unsigned short HY) 
{
/*
[ADh] BTE Destination Window Upper-Left corner X-coordination [7:0]
[AEh] BTE Destination Window Upper-Left corner X-coordination [12:8]
[AFh] BTE Destination Window Upper-Left corner Y-coordination [7:0]
[B0h] BTE Destination Window Upper-Left corner Y-coordination [12:8]
*/
  ER_TFT.LCD_RegisterWrite(0xAD,WX);
  ER_TFT.LCD_RegisterWrite(0xAE,WX>>8);

  ER_TFT.LCD_RegisterWrite(0xAF,HY);
  ER_TFT.LCD_RegisterWrite(0xB0,HY>>8);
}


//[B1h][B2h][B3h][B4h]===============================================================

void ER_TFTBasic::BTE_Window_Size(unsigned short WX, unsigned short WY)

{
/*
[B1h] BTE Window Width [7:0]
[B2h] BTE Window Width [12:8]

[B3h] BTE Window Height [7:0]
[B4h] BTE Window Height [12:8]
*/
        ER_TFT.LCD_RegisterWrite(0xB1,WX);
        ER_TFT.LCD_RegisterWrite(0xB2,WX>>8);
  
      ER_TFT.LCD_RegisterWrite(0xB3,WY);
        ER_TFT.LCD_RegisterWrite(0xB4,WY>>8);
}

//[B5h]=========================================================================
void ER_TFTBasic::BTE_Alpha_Blending_Effect(unsigned char temp)
{ 
/*
Window Alpha Blending effect for S0 & S1
The value of alpha in the color code ranges from 0.0 to 1.0,
where 0.0 represents a fully transparent color, and 1.0
represents a fully opaque color.
00h: 0
01h: 1/32
02h: 2/32
:
1Eh: 30/32
1Fh: 31/32
2Xh: 1
Output Effect = (S0 image x (1 - alpha setting value)) + (S1 image x alpha setting value)
*/
    ER_TFT.LCD_CmdWrite(0xB5);
  ER_TFT.LCD_DataWrite(temp);  
}


//[B6h]=========================================================================
void ER_TFTBasic::Start_SFI_DMA(void)
{
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xB6);
  temp = ER_TFT.LCD_DataRead();
    temp |= cSetb0;
  ER_TFT.LCD_DataWrite(temp);
}

void ER_TFTBasic::Check_Busy_SFI_DMA(void)
{
  ER_TFT.LCD_CmdWrite(0xB6);
  do
  {   
  }while((ER_TFT.LCD_DataRead()&0x01)==0x01);
}


//[B7h]=========================================================================
void ER_TFTBasic::Select_SFI_0(void)
{
/*[bit7]
Serial Flash/ROM I/F # Select
0: Serial Flash/ROM 0 I/F is selected.
1: Serial Flash/ROM 1 I/F is selected.
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xB7);
  temp = ER_TFT.LCD_DataRead();
    temp &= cClrb7;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Select_SFI_1(void)
{
/*[bit7]
Serial Flash/ROM I/F # Select
0: Serial Flash/ROM 0 I/F is selected.
1: Serial Flash/ROM 1 I/F is selected.
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xB7);
  temp = ER_TFT.LCD_DataRead();
    temp |= cSetb7;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Select_SFI_Font_Mode(void)
{
/*[bit6]
Serial Flash /ROM Access Mode
0: Font mode �V for external cgrom
1: DMA mode �V for cgram , pattern , bootstart image or osd
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xB7);
  temp = ER_TFT.LCD_DataRead();
    temp &= cClrb6;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Select_SFI_DMA_Mode(void)
{
/*[bit6]
Serial Flash /ROM Access Mode
0: Font mode �V for external cgrom
1: DMA mode �V for cgram , pattern , bootstart image or osd
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xB7);
  temp = ER_TFT.LCD_DataRead();
    temp |= cSetb6;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Select_SFI_24bit_Address(void)
{
/*[bit5]
Serial Flash/ROM Address Mode
0: 24 bits address mode
1: 32 bits address mode
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xB7);
  temp = ER_TFT.LCD_DataRead();
    temp &= cClrb5;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Select_SFI_32bit_Address(void)
{
/*[bit5]
Serial Flash/ROM Address Mode
0: 24 bits address mode
1: 32 bits address mode
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xB7);
  temp = ER_TFT.LCD_DataRead();
    temp |= cSetb5;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Select_SFI_Waveform_Mode_0(void)
{
/*[bit4]
Serial Flash/ROM Waveform Mode
Mode 0.
Mode 3.
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xB7);
  temp = ER_TFT.LCD_DataRead();
    temp &= cClrb4;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Select_SFI_Waveform_Mode_3(void)
{
/*[bit4]
Serial Flash/ROM Waveform Mode
Mode 0.
Mode 3.
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xB7);
  temp = ER_TFT.LCD_DataRead();
    temp |= cSetb4;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Select_SFI_0_DummyRead(void)
{
/*[bit3][bit2]
Serial Flash /ROM Read Cycle 0 RW
00b: no dummy cycle mode
01b: 1 dummy cycle mode
10b: 2 dummy cycle mode
11b: 4 dummy cycle mode
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xB7);
  temp = ER_TFT.LCD_DataRead();
  temp &= 0xF3;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Select_SFI_8_DummyRead(void)
{
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xB7);
  temp = ER_TFT.LCD_DataRead();
  temp &= 0xF3;
    temp |= cSetb2;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Select_SFI_16_DummyRead(void)
{

  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xB7);
  temp = ER_TFT.LCD_DataRead();
  temp &= 0xF3;
    temp |= cSetb3;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Select_SFI_24_DummyRead(void)
{
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xB7);
  temp = ER_TFT.LCD_DataRead();
    temp |= 0x0c;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Select_SFI_Single_Mode(void)
{
/*[bit1][bit0]
Serial Flash /ROM I/F Data Latch Mode Select
0Xb: Single Mode
10b: Dual Mode 0.
11b: Dual Mode 1.
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xB7);
  temp = ER_TFT.LCD_DataRead();
  temp &= 0xFC;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Select_SFI_Dual_Mode0(void)
{
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xB7);
  temp = ER_TFT.LCD_DataRead();
  temp &= 0xFC;
    temp |= cSetb1;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Select_SFI_Dual_Mode1(void)
{
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xB7);
  temp = ER_TFT.LCD_DataRead();
    temp |= 0x03;
  ER_TFT.LCD_DataWrite(temp);
}

//REG[B8h] SPI master Tx /Rx FIFO Data Register (SPIDR) 
unsigned char ER_TFTBasic::SPI_Master_FIFO_Data_Put(unsigned char Data)
{
    unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xB8);
  ER_TFT.LCD_DataWrite(Data);
  while(Tx_FIFO_Empty_Flag()==0); 
  temp = SPI_Master_FIFO_Data_Get();
  return temp;
}

unsigned char ER_TFTBasic::SPI_Master_FIFO_Data_Get(void)
{
   unsigned char temp;

  while(Rx_FIFO_Empty_Flag()==1);
  ER_TFT.LCD_CmdWrite(0xB8);
  temp=ER_TFT.LCD_DataRead();
  //while(Rx_FIFO_full_flag());
   return temp;
}

//REG[B9h] SPI master Control Register (SPIMCR2) 
void ER_TFTBasic::Mask_SPI_Master_Interrupt_Flag(void)
{
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xB9);
  temp = ER_TFT.LCD_DataRead();
  temp |= cSetb6;
  ER_TFT.LCD_DataWrite(temp);

} 

void ER_TFTBasic::Select_nSS_drive_on_xnsfcs0(void)
{
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xB9);
  temp = ER_TFT.LCD_DataRead();
  temp &= cClrb5;
  ER_TFT.LCD_DataWrite(temp);

}

void ER_TFTBasic::Select_nSS_drive_on_xnsfcs1(void)
{
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xB9);
  temp = ER_TFT.LCD_DataRead();
  temp |= cSetb5;
  ER_TFT.LCD_DataWrite(temp);
}

//0: inactive (nSS port will goes high) 
void ER_TFTBasic::nSS_Inactive(void)
{
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xB9);
  temp = ER_TFT.LCD_DataRead();
  temp &= cClrb4;
  ER_TFT.LCD_DataWrite(temp);
}
//1: active (nSS port will goes low) 
void ER_TFTBasic::nSS_Active(void)
{
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xB9);
  temp = ER_TFT.LCD_DataRead();
  temp |= cSetb4;
  ER_TFT.LCD_DataWrite(temp);
}

//Interrupt enable for FIFO overflow error [OVFIRQEN] 
void ER_TFTBasic::OVFIRQEN_Enable(void)
{
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xB9);
  temp = ER_TFT.LCD_DataRead();
  temp |= cSetb3;
  ER_TFT.LCD_DataWrite(temp);
}
//Interrupt enable for while Tx FIFO empty & SPI engine/FSM idle
void ER_TFTBasic::EMTIRQEN_Enable(void)
{
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xB9);
  temp = ER_TFT.LCD_DataRead();
  temp |= cSetb2;
  ER_TFT.LCD_DataWrite(temp);
}


//At CPOL=0 the base value of the clock is zero   
//o  For CPHA=0, data are read on the clock's rising edge (low->high transition) and 
//data are changed on a falling edge (high->low clock transition). 
//o  For CPHA=1, data are read on the clock's falling edge and data are changed on a 
//rising edge. 

//At CPOL=1 the base value of the clock is one (inversion of CPOL=0)   
//o  For CPHA=0, data are read on clock's falling edge and data are changed on a 
//rising edge. 
//o  For CPHA=1, data are read on clock's rising edge and data are changed on a 
//falling edge.

void ER_TFTBasic::Reset_CPOL(void)
{
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xB9);
  temp = ER_TFT.LCD_DataRead();
  temp &= cClrb1;
  ER_TFT.LCD_DataWrite(temp);
}

void ER_TFTBasic::Set_CPOL(void)
{
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xB9);
  temp = ER_TFT.LCD_DataRead();
  temp |= cSetb1;
  ER_TFT.LCD_DataWrite(temp);
}


void ER_TFTBasic::Reset_CPHA(void)
{
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xB9);
  temp = ER_TFT.LCD_DataRead();
  temp &= cClrb0;
  ER_TFT.LCD_DataWrite(temp);
}

void ER_TFTBasic::Set_CPHA(void)
{
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xB9);
  temp = ER_TFT.LCD_DataRead();
  temp |= cSetb0;
  ER_TFT.LCD_DataWrite(temp);
}


//REG[BAh] SPI master Status Register (SPIMSR) 
unsigned char ER_TFTBasic::Tx_FIFO_Empty_Flag(void)
{
  ER_TFT.LCD_CmdWrite(0xBA);
  if((ER_TFT.LCD_DataRead()&0x80)==0x80)
  return 1;
  else
  return 0;
}

unsigned char ER_TFTBasic::Tx_FIFO_Full_Flag(void)
{
  ER_TFT.LCD_CmdWrite(0xBA);
  if((ER_TFT.LCD_DataRead()&0x40)==0x40)
  return 1;
  else
  return 0;
} 

unsigned char ER_TFTBasic::Rx_FIFO_Empty_Flag(void)
{
  ER_TFT.LCD_CmdWrite(0xBA);
  if((ER_TFT.LCD_DataRead()&0x20)==0x20)
  return 1;
  else
  return 0;
} 

unsigned char ER_TFTBasic::Rx_FIFO_full_flag(void)
{
   ER_TFT.LCD_CmdWrite(0xBA);
   if((ER_TFT.LCD_DataRead()&0x10)==0x10)
   return 1;
   else
   return 0;
} 

unsigned char ER_TFTBasic::OVFI_Flag(void)
{
   ER_TFT.LCD_CmdWrite(0xBA);
   if((ER_TFT.LCD_DataRead()&0x08)==0x08)
   return 1;
   else
   return 0;
}

void ER_TFTBasic::Clear_OVFI_Flag(void)
{
   unsigned char temp;
   ER_TFT.LCD_CmdWrite(0xBA);
   temp = ER_TFT.LCD_DataRead();
   temp |= cSetb3;
   ER_TFT.LCD_DataWrite(temp);
}

unsigned char ER_TFTBasic::EMTI_Flag(void)
{
   ER_TFT.LCD_CmdWrite(0xBA);
   if((ER_TFT.LCD_DataRead()&0x04)==0x04)
   return 1;
   else
   return 0;
}

void ER_TFTBasic::Clear_EMTI_Flag(void)
{
   unsigned char temp;
   ER_TFT.LCD_CmdWrite(0xBA);
   temp = ER_TFT.LCD_DataRead();
   temp |= cSetb2;
   ER_TFT.LCD_DataWrite(temp);
}


//REG[BB] SPI Clock period (SPIDIV) 
void ER_TFTBasic::SPI_Clock_Period(unsigned char temp)
{
   ER_TFT.LCD_CmdWrite(0xBB);
   ER_TFT.LCD_DataWrite(temp);
} 

//[BCh][BDh][BEh][BFh]=========================================================================
void ER_TFTBasic::SFI_DMA_Source_Start_Address(unsigned long Addr)
{
/*
DMA Source START ADDRESS
This bits index serial flash address [7:0][15:8][23:16][31:24]
*/
  ER_TFT.LCD_CmdWrite(0xBC);
  ER_TFT.LCD_DataWrite(Addr);
  ER_TFT.LCD_CmdWrite(0xBD);
  ER_TFT.LCD_DataWrite(Addr>>8);
  ER_TFT.LCD_CmdWrite(0xBE);
  ER_TFT.LCD_DataWrite(Addr>>16);
  ER_TFT.LCD_CmdWrite(0xBF);
  ER_TFT.LCD_DataWrite(Addr>>24);
}
//[C0h][C1h][C2h][C3h]=========================================================================
void ER_TFTBasic::SFI_DMA_Destination_Start_Address(unsigned long Addr)
{
/*
DMA Destination START ADDRESS 
[1:0]Fix at 0
This bits index SDRAM address [7:0][15:8][23:16][31:24]
*/
  ER_TFT.LCD_CmdWrite(0xC0);
  ER_TFT.LCD_DataWrite(Addr);
  ER_TFT.LCD_CmdWrite(0xC1);
  ER_TFT.LCD_DataWrite(Addr>>8);
  ER_TFT.LCD_CmdWrite(0xC2);
  ER_TFT.LCD_DataWrite(Addr>>16);
  ER_TFT.LCD_CmdWrite(0xC3);
  ER_TFT.LCD_DataWrite(Addr>>24);
}
//[C0h][C1h][C2h][C3h]=========================================================================
void ER_TFTBasic::SFI_DMA_Destination_Upper_Left_Corner(unsigned short WX,unsigned short HY)
{
/*
C0h
This register defines DMA Destination Window Upper-Left corner 
X-coordination [7:0] on Canvas area. 
When REG DMACR bit 1 = 1 (Block Mode) 
This register defines Destination address [7:2] in SDRAM. 
C1h
When REG DMACR bit 1 = 0 (Linear Mode) 
This register defines DMA Destination Window Upper-Left corner 
X-coordination [12:8] on Canvas area. 
When REG DMACR bit 1 = 1 (Block Mode) 
This register defines Destination address [15:8] in SDRAM.
C2h
When REG DMACR bit 1 = 0 (Linear Mode) 
This register defines DMA Destination Window Upper-Left corner
Y-coordination [7:0] on Canvas area. 
When REG DMACR bit 1 = 1 (Block Mode) 
This register defines Destination address [23:16] in SDRAM. 
C3h
When REG DMACR bit 1 = 0 (Linear Mode) 
This register defines DMA Destination Window Upper-Left corner 
Y-coordination [12:8] on Canvas area. 
When REG DMACR bit 1 = 1 (Block Mode) 
This register defines Destination address [31:24] in SDRAM. 
*/
 
  ER_TFT.LCD_CmdWrite(0xC0);
  ER_TFT.LCD_DataWrite(WX);
  ER_TFT.LCD_CmdWrite(0xC1);
  ER_TFT.LCD_DataWrite(WX>>8);
 
  ER_TFT.LCD_CmdWrite(0xC2);
  ER_TFT.LCD_DataWrite(HY);
  ER_TFT.LCD_CmdWrite(0xC3);
  ER_TFT.LCD_DataWrite(HY>>8);
}



//[C6h][C7h][C8h][C9h]=========================================================================
void ER_TFTBasic::SFI_DMA_Transfer_Number(unsigned long Addr)
{
/*
Unit : Pixel
When REG DMACR bit 1 = 0 (Linear Mode)
DMA Transfer Number [7:0][15:8][23:16][31:24]

When REG DMACR bit 1 = 1 (Block Mode)
DMA Block Width [7:0][15:8]
DMA Block HIGH[7:0][15:8]
*/
  ER_TFT.LCD_CmdWrite(0xC6);
  ER_TFT.LCD_DataWrite(Addr);
  ER_TFT.LCD_CmdWrite(0xC7);
  ER_TFT.LCD_DataWrite(Addr>>8);
  ER_TFT.LCD_CmdWrite(0xC8);
  ER_TFT.LCD_DataWrite(Addr>>16);
  ER_TFT.LCD_CmdWrite(0xC9);
  ER_TFT.LCD_DataWrite(Addr>>24);
}
void ER_TFTBasic::SFI_DMA_Transfer_Width_Height(unsigned short WX,unsigned short HY)
{
/*
When REG DMACR bit 1 = 0 (Linear Mode)
DMA Transfer Number [7:0][15:8][23:16][31:24]

When REG DMACR bit 1 = 1 (Block Mode)
DMA Block Width [7:0][15:8]
DMA Block HIGH[7:0][15:8]
*/
  ER_TFT.LCD_CmdWrite(0xC6);
  ER_TFT.LCD_DataWrite(WX);
  ER_TFT.LCD_CmdWrite(0xC7);
  ER_TFT.LCD_DataWrite(WX>>8);

  ER_TFT.LCD_CmdWrite(0xC8);
  ER_TFT.LCD_DataWrite(HY);
  ER_TFT.LCD_CmdWrite(0xC9);
  ER_TFT.LCD_DataWrite(HY>>8);
}
//[CAh][CBh]=========================================================================
void ER_TFTBasic::SFI_DMA_Source_Width(unsigned short WX)
{
/*
DMA Source Picture Width [7:0][12:8]
Unit: pixel
*/
  ER_TFT.LCD_CmdWrite(0xCA);
  ER_TFT.LCD_DataWrite(WX);
  ER_TFT.LCD_CmdWrite(0xCB);
  ER_TFT.LCD_DataWrite(WX>>8);
}

//[CCh]=========================================================================

void ER_TFTBasic::Font_Select_UserDefine_Mode(void)
{
/*[bit7-6]
User-defined Font /CGROM Font Selection Bit in Text Mode
00 : Internal CGROM
01 : Genitop serial flash
10 : User-defined Font
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xCC);
  temp = ER_TFT.LCD_DataRead();
    temp |= cSetb7;
  temp &= cClrb6;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::CGROM_Select_Internal_CGROM(void)
{
/*[bit7-6]
User-defined Font /CGROM Font Selection Bit in Text Mode
00 : Internal CGROM
01 : Genitop serial flash
10 : User-defined Font
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xCC);
  temp = ER_TFT.LCD_DataRead();
  temp &= cClrb7;
    temp &= cClrb6;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::CGROM_Select_Genitop_FontROM(void)
{
/*[bit7-6]
User-defined Font /CGROM Font Selection Bit in Text Mode
00 : Internal CGROM
01 : Genitop serial flash
10 : User-defined Font
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xCC);
  temp = ER_TFT.LCD_DataRead();
  temp &= cClrb7;
    temp |= cSetb6;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Font_Select_8x16_16x16(void)
{
/*[bit5-4]
Font Height Setting
00b : 8x16 / 16x16.
01b : 12x24 / 24x24.
10b : 16x32 / 32x32.
*** User-defined Font width is decided by font code. Genitop
serial flash��s font width is decided by font code or GT Font ROM
control register.
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xCC);
  temp = ER_TFT.LCD_DataRead();
    temp &= cClrb5;
    temp &= cClrb4;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Font_Select_12x24_24x24(void)
{
/*[bit5-4]
Font Height Setting
00b : 8x16 / 16x16.
01b : 12x24 / 24x24.
10b : 16x32 / 32x32.
*** User-defined Font width is decided by font code. Genitop
serial flash��s font width is decided by font code or GT Font ROM
control register.
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xCC);
  temp = ER_TFT.LCD_DataRead();
    temp &= cClrb5;
    temp |= cSetb4;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Font_Select_16x32_32x32(void)
{
/*[bit5-4]
Font Height Setting
00b : 8x16 / 16x16.
01b : 12x24 / 24x24.
10b : 16x32 / 32x32.
*** User-defined Font width is decided by font code. Genitop
serial flash��s font width is decided by font code or GT Font ROM
control register.
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xCC);
  temp = ER_TFT.LCD_DataRead();
    temp |= cSetb5;
    temp &= cClrb4;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Internal_CGROM_Select_ISOIEC8859_1(void)
{
/*
Font Selection for internal CGROM
When FNCR0 B7 = 0 and B5 = 0, Internal CGROM supports the
8x16 character sets with the standard coding of ISO/IEC 8859-1~4, 
which supports English and most of European country languages.
00b : ISO/IEC 8859-1.
01b : ISO/IEC 8859-2.
10b : ISO/IEC 8859-3.
11b : ISO/IEC 8859-4.
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xCC);
  temp = ER_TFT.LCD_DataRead();
    temp &= cClrb1;
    temp &= cClrb0;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Internal_CGROM_Select_ISOIEC8859_2(void)
{
/*
Font Selection for internal CGROM
When FNCR0 B7 = 0 and B5 = 0, Internal CGROM supports the
8x16 character sets with the standard coding of ISO/IEC 8859-1~4, 
which supports English and most of European country languages.
00b : ISO/IEC 8859-1.
01b : ISO/IEC 8859-2.
10b : ISO/IEC 8859-3.
11b : ISO/IEC 8859-4.
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xCC);
  temp = ER_TFT.LCD_DataRead();
    temp &= cClrb1;
    temp |= cSetb0;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Internal_CGROM_Select_ISOIEC8859_3(void)
{
/*
Font Selection for internal CGROM
When FNCR0 B7 = 0 and B5 = 0, Internal CGROM supports the
8x16 character sets with the standard coding of ISO/IEC 8859-1~4, 
which supports English and most of European country languages.
00b : ISO/IEC 8859-1.
01b : ISO/IEC 8859-2.
10b : ISO/IEC 8859-3.
11b : ISO/IEC 8859-4.
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xCC);
  temp = ER_TFT.LCD_DataRead();
    temp |= cSetb1;
    temp &= cClrb0;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Internal_CGROM_Select_ISOIEC8859_4(void)
{
/*
Font Selection for internal CGROM
When FNCR0 B7 = 0 and B5 = 0, Internal CGROM supports the
8x16 character sets with the standard coding of ISO/IEC 8859-1~4, 
which supports English and most of European country languages.
00b : ISO/IEC 8859-1.
01b : ISO/IEC 8859-2.
10b : ISO/IEC 8859-3.
11b : ISO/IEC 8859-4.
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xCC);
  temp = ER_TFT.LCD_DataRead();
    temp |= cSetb1;
    temp |= cSetb0;
  ER_TFT.LCD_DataWrite(temp);
}
//[CDh]=========================================================================
void ER_TFTBasic::Enable_Font_Alignment(void)
{
/*
Full Alignment Selection Bit
0 : Full alignment disable.
1 : Full alignment enable.
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xCD);
  temp = ER_TFT.LCD_DataRead();
    temp |= cSetb7;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Disable_Font_Alignment(void)
{
/*
Full Alignment Selection Bit
0 : Full alignment disable.
1 : Full alignment enable.
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xCD);
  temp = ER_TFT.LCD_DataRead();
    temp &= cClrb7;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Font_Background_select_Transparency(void)
{
/*
Font Transparency
0 : Font with background color.
1 : Font with background transparency.
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xCD);
  temp = ER_TFT.LCD_DataRead();
    temp |= cSetb6;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Font_Background_select_Color(void)
{
/*
Font Transparency
0 : Font with background color.
1 : Font with background transparency.
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xCD);
  temp = ER_TFT.LCD_DataRead();
    temp &= cClrb6;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Font_0_degree(void)
{
/*
Font Rotation
0 : Normal
Text direction from left to right then from top to bottom
1 : Counterclockwise 90 degree & horizontal flip
Text direction from top to bottom then from left to right
(it should accommodate with set VDIR as 1)
This attribute can be changed only when previous font write
finished (core_busy = 0)
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xCD);
  temp = ER_TFT.LCD_DataRead();
    temp &= cClrb4;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Font_90_degree(void)
{
/*
Font Rotation
0 : Normal
Text direction from left to right then from top to bottom
1 : Counterclockwise 90 degree & horizontal flip
Text direction from top to bottom then from left to right
(it should accommodate with set VDIR as 1)
This attribute can be changed only when previous font write
finished (core_busy = 0)
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xCD);
  temp = ER_TFT.LCD_DataRead();
    temp |= cSetb4;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Font_Width_X1(void)
{
/*
Horizontal Font Enlargement
00b : X1.
01b : X2.
10b : X3.
11b : X4.
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xCD);
  temp = ER_TFT.LCD_DataRead();
    temp &= cClrb3;
    temp &= cClrb2;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Font_Width_X2(void)
{
/*
Horizontal Font Enlargement
00b : X1.
01b : X2.
10b : X3.
11b : X4.
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xCD);
  temp = ER_TFT.LCD_DataRead();
    temp &= cClrb3;
    temp |= cSetb2;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Font_Width_X3(void)
{
/*
Horizontal Font Enlargement
00b : X1.
01b : X2.
10b : X3.
11b : X4.
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xCD);
  temp = ER_TFT.LCD_DataRead();
    temp |= cSetb3;
    temp &= cClrb2;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Font_Width_X4(void)
{
/*
Horizontal Font Enlargement
00b : X1.
01b : X2.
10b : X3.
11b : X4.
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xCD);
  temp = ER_TFT.LCD_DataRead();
    temp |= cSetb3;
    temp |= cSetb2;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Font_Height_X1(void)
{
/*
Vertical Font Enlargement
00b : X1.
01b : X2.
10b : X3.
11b : X4.
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xCD);
  temp = ER_TFT.LCD_DataRead();
    temp &= cClrb1;
    temp &= cClrb0;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Font_Height_X2(void)
{
/*
Vertical Font Enlargement
00b : X1.
01b : X2.
10b : X3.
11b : X4.
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xCD);
  temp = ER_TFT.LCD_DataRead();
    temp &= cClrb1;
    temp |= cSetb0;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Font_Height_X3(void)
{
/*
Vertical Font Enlargement
00b : X1.
01b : X2.
10b : X3.
11b : X4.
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xCD);
  temp = ER_TFT.LCD_DataRead();
    temp |= cSetb1;
    temp &= cClrb0;
  ER_TFT.LCD_DataWrite(temp);
}
void ER_TFTBasic::Font_Height_X4(void)
{
/*
Vertical Font Enlargement
00b : X1.
01b : X2.
10b : X3.
11b : X4.
*/
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0xCD);
  temp = ER_TFT.LCD_DataRead();
    temp |= cSetb1;
    temp |= cSetb0;
  ER_TFT.LCD_DataWrite(temp);
}



//[D0h]=========================================================================
void ER_TFTBasic::Font_Line_Distance(unsigned char temp)
{
/*[bit4-0]
Font Line Distance Setting
Setting the font character line distance when setting memory font
write cursor auto move. (Unit: pixel)
*/
  ER_TFT.LCD_CmdWrite(0xD0);
  ER_TFT.LCD_DataWrite(temp);
}
//[D1h]=========================================================================
void ER_TFTBasic::Set_Font_to_Font_Width(unsigned char temp)
{
/*[bit5-0]
Font to Font Width Setting (Unit: pixel)
*/
  ER_TFT.LCD_CmdWrite(0xD1);
  ER_TFT.LCD_DataWrite(temp);
}



void ER_TFTBasic::MemWrite_Left_Right_Top_Down(void)
{
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0x02);
  temp = ER_TFT.LCD_DataRead();
  temp &= cClrb2;
  temp &= cClrb1;
  ER_TFT.LCD_DataWrite(temp);
}

//void PLL_WaitReady(void);
void ER_TFTBasic::System_Check_Temp(void)
{
  unsigned char i=0,j=0;
  unsigned char temp=0;
  unsigned char system_ok=0;
  do
  {
    j = ER_TFT.LCD_StatusRead();
    if((j&0x02)==0x00)    
    {
      delay(2);                  //MCU too fast, necessary
      ER_TFT.LCD_CmdWrite(0x01);
      delay(2);                  //MCU too fast, necessary
      temp = ER_TFT.LCD_DataRead();
      if((temp & 0x80) == 0x80)       //Check CCR register's PLL is ready or not
      {
        system_ok=1;
        i=0;
      }
      else
      {
        delay(2); //MCU too fast, necessary
        ER_TFT.LCD_CmdWrite(0x01);
        delay(2); //MCU too fast, necessary
        ER_TFT.LCD_DataWrite(0x80);
      }
    }
    else
    {
      system_ok=0;
      i++;
    }
    if(system_ok==0 && i==5)
    {
      ER_TFT.HW_Reset(); //note1
      i=0;
    }
  }while(system_ok==0);
}

//void PLL_ConfigClocks(void);
void ER_TFTBasic::PLL_Initial(void) 
{


 // Set pixel clock
  if(SCAN_FREQ>=63)        //&&(SCAN_FREQ<=100))
  {
	ER_TFT.LCD_CmdWrite(0x05);    //PLL Divided by 4
	ER_TFT.LCD_DataWrite(0x04);
	ER_TFT.LCD_CmdWrite(0x06);
	ER_TFT.LCD_DataWrite((SCAN_FREQ*4/OSC_FREQ)-1);
  }
  if((SCAN_FREQ>=32)&&(SCAN_FREQ<=62))
  {           
	ER_TFT.LCD_CmdWrite(0x05);    //PLL Divided by 8
	ER_TFT.LCD_DataWrite(0x06);
	ER_TFT.LCD_CmdWrite(0x06);
	ER_TFT.LCD_DataWrite((SCAN_FREQ*8/OSC_FREQ)-1);
  }
  if((SCAN_FREQ>=16)&&(SCAN_FREQ<=31))
  {           
	ER_TFT.LCD_CmdWrite(0x05);    //PLL Divided by 16
	ER_TFT.LCD_DataWrite(0x16);
	ER_TFT.LCD_CmdWrite(0x06);
	ER_TFT.LCD_DataWrite((SCAN_FREQ*16/OSC_FREQ)-1);
  }
  if((SCAN_FREQ>=8)&&(SCAN_FREQ<=15))
  {
	ER_TFT.LCD_CmdWrite(0x05);    //PLL Divided by 32
	ER_TFT.LCD_DataWrite(0x26);
	ER_TFT.LCD_CmdWrite(0x06);
	ER_TFT.LCD_DataWrite((SCAN_FREQ*32/OSC_FREQ)-1);
  }
  if((SCAN_FREQ>0)&&(SCAN_FREQ<=7))
  {
	ER_TFT.LCD_CmdWrite(0x05);    //PLL Divided by 64
	ER_TFT.LCD_DataWrite(0x36);
	ER_TFT.LCD_CmdWrite(0x06);
	ER_TFT.LCD_DataWrite((SCAN_FREQ*64/OSC_FREQ)-1);
  }            
 
  
  // Set SDRAM clock
  if(DRAM_FREQ>=125)        //&&(DRAM_FREQ<=166))
  {
	ER_TFT.LCD_CmdWrite(0x07);    //PLL Divided by 2
	ER_TFT.LCD_DataWrite(0x02);
	ER_TFT.LCD_CmdWrite(0x08);
	ER_TFT.LCD_DataWrite((DRAM_FREQ*2/OSC_FREQ)-1);
  }
  if((DRAM_FREQ>=63)&&(DRAM_FREQ<=124))   //&&(DRAM_FREQ<=166)
  {
	ER_TFT.LCD_CmdWrite(0x07);    //PLL Divided by 4
	ER_TFT.LCD_DataWrite(0x04);
	ER_TFT.LCD_CmdWrite(0x08);
	ER_TFT.LCD_DataWrite((DRAM_FREQ*4/OSC_FREQ)-1);
  }
  if((DRAM_FREQ>=31)&&(DRAM_FREQ<=62))
  {           
	ER_TFT.LCD_CmdWrite(0x07);    //PLL Divided by 8
	ER_TFT.LCD_DataWrite(0x06);
	ER_TFT.LCD_CmdWrite(0x08);
	ER_TFT.LCD_DataWrite((DRAM_FREQ*8/OSC_FREQ)-1);
  }
  if(DRAM_FREQ<=30)
  {
	ER_TFT.LCD_CmdWrite(0x07);    //PLL Divided by 8
	ER_TFT.LCD_DataWrite(0x06);
	ER_TFT.LCD_CmdWrite(0x08); //
	ER_TFT.LCD_DataWrite((30*8/OSC_FREQ)-1);
  }
 

  // Set Core clock
  if(CORE_FREQ>=125)
  {
	ER_TFT.LCD_CmdWrite(0x09);    //PLL Divided by 2
	ER_TFT.LCD_DataWrite(0x02);
	ER_TFT.LCD_CmdWrite(0x0A);
	ER_TFT.LCD_DataWrite((CORE_FREQ*2/OSC_FREQ)-1);
  }
  if((CORE_FREQ>=63)&&(CORE_FREQ<=124))     
  {
	ER_TFT.LCD_CmdWrite(0x09);    //PLL Divided by 4
	ER_TFT.LCD_DataWrite(0x04);
	ER_TFT.LCD_CmdWrite(0x0A);
	ER_TFT.LCD_DataWrite((CORE_FREQ*4/OSC_FREQ)-1);
  }
  if((CORE_FREQ>=31)&&(CORE_FREQ<=62))
  {           
	ER_TFT.LCD_CmdWrite(0x09);    //PLL Divided by 8
	ER_TFT.LCD_DataWrite(0x06);
	ER_TFT.LCD_CmdWrite(0x0A);
	ER_TFT.LCD_DataWrite((CORE_FREQ*8/OSC_FREQ)-1);
  }
  if(CORE_FREQ<=30)
  {
	ER_TFT.LCD_CmdWrite(0x09);    //PLL Divided by 8
	ER_TFT.LCD_DataWrite(0x06);
	ER_TFT.LCD_CmdWrite(0x0A); // 
	ER_TFT.LCD_DataWrite((30*8/OSC_FREQ)-1);
  }

	ER_TFT.LCD_CmdWrite(0x01);
	ER_TFT.LCD_CmdWrite(0x00);
	delay(1);
	ER_TFT.LCD_CmdWrite(0x80);
	//Enable_PLL();

	delay(1);	//
  

}


void ER_TFTBasic::SDRAM_initail(void)
{
  unsigned short sdram_itv;
  
  ER_TFT.LCD_RegisterWrite(0xe0,0x29);      
  ER_TFT.LCD_RegisterWrite(0xe1,0x03); //CAS:2=0x02�ACAS:3=0x03
  sdram_itv = (64000000 / 8192) / (1000/60) ;
  sdram_itv-=2;

  ER_TFT.LCD_RegisterWrite(0xe2,sdram_itv);
  ER_TFT.LCD_RegisterWrite(0xe3,sdram_itv >>8);
  ER_TFT.LCD_RegisterWrite(0xe4,0x01);
  ER_TFT.Check_SDRAM_Ready();
  delay(1);
}


//void HardwareReset(void);
void ER_TFTBasic::HW_Reset(void)
{
  pinMode(LCD_RESET, OUTPUT);
  digitalWrite(LCD_RESET, LOW);
  delay(500);
  digitalWrite(LCD_RESET, HIGH);
  delay(500);
}


//sera desfragmentado esta funcao
void ER_TFTBasic::initial(void)
{

    ER_TFT.PLL_Initial();
    
    ER_TFT.SDRAM_initail();

//**[01h]**//
    ER_TFT.TFT_16bit();
  ER_TFT.Host_Bus_16bit(); //Host bus 16bit
      
//**[02h]**//
  ER_TFT.RGB_16b_16bpp();
  ER_TFT.MemWrite_Left_Right_Top_Down(); 
      
//**[03h]**//
  ER_TFT.Graphic_Mode();
  ER_TFT.Memory_Select_SDRAM();

  ER_TFT.HSCAN_L_to_R();     //REG[12h]:from left to right
  ER_TFT.VSCAN_T_to_B();       //REG[12h]:from top to bottom
  ER_TFT.PDATA_Set_RGB();        //REG[12h]:Select RGB output

  ER_TFT.Set_PCLK(LCD_PCLK_Falling_Rising);   //LCD_PCLK_Falling_Rising
  ER_TFT.Set_HSYNC_Active(LCD_HSYNC_Active_Polarity);
  ER_TFT.Set_VSYNC_Active(LCD_VSYNC_Active_Polarity);
  ER_TFT.Set_DE_Active(LCD_DE_Active_Polarity);
 
  ER_TFT.LCD_HorizontalWidth_VerticalHeight(LCD_XSIZE_TFT ,LCD_YSIZE_TFT);
  ER_TFT.LCD_Horizontal_Non_Display(LCD_HBPD);                          
  ER_TFT.LCD_HSYNC_Start_Position(LCD_HFPD);                              
  ER_TFT.LCD_HSYNC_Pulse_Width(LCD_HSPW);                              
  ER_TFT.LCD_Vertical_Non_Display(LCD_VBPD);                               
  ER_TFT.LCD_VSYNC_Start_Position(LCD_VFPD);                               
  ER_TFT.LCD_VSYNC_Pulse_Width(LCD_VSPW);                              
      
  ER_TFT.Select_Main_Window_16bpp();

  ER_TFT.Memory_XY_Mode(); //Block mode (X-Y coordination addressing)
  ER_TFT.Memory_16bpp_Mode();
  ER_TFT.Select_Main_Window_16bpp();
}

void ER_TFTBasic::Display_ON(void)
{
/*  
Display ON/OFF
0b: Display Off.
1b: Display On.
*/
  unsigned char temp;
  
  ER_TFT.LCD_CmdWrite(0x12);
  temp = ER_TFT.LCD_DataRead();
  temp |= cSetb6;
  ER_TFT.LCD_DataWrite(temp);
}

void ER_TFTBasic::DMA_24bit_Block
(
 unsigned char SCS         // Select SPI : SCS��0       SCS��1
,unsigned char Clk         // SPI Clock = System Clock /{(Clk+1)*2}
,unsigned short X1         // Transfer to SDRAM address:X1
,unsigned short Y1         // Transfer to SDRAM address:Y1
,unsigned short X_W        // DMA data width
,unsigned short Y_H        // DMA data height
,unsigned short P_W        // Picture's width
,unsigned long Addr        // Flash address
)
{
  ER_TFT.Enable_SFlash_SPI();                            
  if(SCS == 0)  ER_TFT.Select_SFI_0();                   // Select SPI0
  if(SCS == 1)  ER_TFT.Select_SFI_1();                   // Select SPI1

  ER_TFT.Select_SFI_Dual_Mode0();	
  ER_TFT.Memory_XY_Mode();                     
  ER_TFT.Select_SFI_DMA_Mode();                          // Select SPI DMA mode
  ER_TFT.SPI_Clock_Period(Clk);                          // Select SPI clock

  ER_TFT.Goto_Pixel_XY(X1,Y1);                           // Setting the location of memory in the graphic mode
  ER_TFT.SFI_DMA_Destination_Upper_Left_Corner(X1,Y1);   // DMA destination(SDRAM address)
  ER_TFT.SFI_DMA_Transfer_Width_Height(X_W,Y_H);         // Setting Block data: width&height
  ER_TFT.SFI_DMA_Source_Width(P_W);                      // Setting the width of the source data
  ER_TFT.SFI_DMA_Source_Start_Address(Addr);             // Setting the FLASH address of the source data

  ER_TFT.Start_SFI_DMA();                                
  ER_TFT.Check_Busy_SFI_DMA();                        
}

//void TextMode(void);
void ER_TFTBasic::Text_Mode(void)
{
  unsigned char temp;
  ER_TFT.LCD_CmdWrite(0x03);
  temp = ER_TFT.LCD_DataRead();
    temp |= cSetb2;
  ER_TFT.LCD_DataWrite(temp);
}

void ER_TFTBasic::Show_String(char *str)
{   
    Text_Mode();     
    ER_TFT.LCD_CmdWrite(0x04);
    while(*str != '\0')
    {
      ER_TFT.LCD_DataWrite(*str);
      Check_Mem_WR_FIFO_not_Full();
      ++str;   
    }
    Check_2D_Busy();

    Graphic_Mode(); //back to graphic mode;
}

void ER_TFTBasic::DrawPixel(unsigned short x,unsigned short y,unsigned short color)
{  
 //   ER_TFT.Goto_Pixel_XY(x,y);
    ER_TFT.LCD_CmdWrite(0x04); 
    ER_TFT.LCD_DataWrite(color);
    Check_Mem_WR_FIFO_not_Full();
    ER_TFT.LCD_DataWrite(color>>8);
    Check_Mem_WR_FIFO_not_Full();  
}  


void ER_TFTBasic::Show_picture(unsigned long numbers,const unsigned char *datap)
{   
  unsigned long i;

  ER_TFT.LCD_CmdWrite(0x04);  
  for(i=0;i<numbers*2;i+=2)
  {
    ER_TFT.LCD_DataWrite(pgm_read_byte(&datap[i+1]));
    Check_Mem_WR_FIFO_not_Full();
    ER_TFT.LCD_DataWrite(pgm_read_byte(&datap[i]));
    Check_Mem_WR_FIFO_not_Full();
  }

}

ER_TFTBasic ER_TFT=ER_TFTBasic();

