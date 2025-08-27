//=========================================//
/*
File Name : Ra8889_Lite.cpp                                   
Author    : Mountain                            
Edit Date : 04/21/2020
Version   : v1.0  
*/
//=========================================//
//========== COPYRIGHT 2020 RAiO ==========//
//=========================================//
#include <SPI.h>
#include "Ra8889_Lite.h"

#define mydebug
//**************************************************************//
 Ra8889_Lite::Ra8889_Lite(int xnscs,int xnreset)
{
  _xnscs = xnscs;
  _xnreset = xnreset;
}
//**************************************************************//
//**************************************************************//
boolean Ra8889_Lite::begin(void) 
{ 
  ru8 temp;
  //initialize the bus for Due
  SPI.begin(_xnscs);
  //On the Due, the system clock can be divided by values from 1 to 255. 
  //The default value is 21, which sets the clock to 4 MHz like other Arduino boards
  SPI.setClockDivider(_xnscs,21);
  SPI.setDataMode(_xnscs,SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  
  //ra8889 hardware reset
  pinMode(_xnreset, OUTPUT); 
  digitalWrite(_xnreset, HIGH);
  delay(1);
  digitalWrite(_xnreset, LOW);
  delay(1);
  digitalWrite(_xnreset, HIGH);
  delay(10);
  if(!checkIcReady())
  {return false;}
  
  #ifdef mydebug
  Serial.println("mydebug"); 
  #endif
  
  //read ID code must disable pll, 01h bit7 set 0
  //lcdRegDataWrite(0x01,0x08);
  //delay(1);
  
  if (readIdCode() != 0x89)
  {Serial.println("RA8889 not found!");
    return false;}
  else
  {Serial.println("RA8889 connect pass!");}
  
  if(!ra8889Initialize())
  {Serial.println("ra8889 initial fail!");
   return false;}
  else
  {Serial.println("RA8889 initial Pass!");}
  
  SPI.setClockDivider(_xnscs,2);
  return true;
}
//**************************************************************//

boolean Ra8889_Lite::ra8889Initialize(void)
{ ru8 temp;
   
  if(!ra8889PllInitial(DRAM_FREQ,CORE_FREQ,SCAN_FREQ))
  {Serial.println("PLL initial fail!");
  return false;}
  
  if(!ra8889SdramInitial())
  {Serial.println("SDRAM initial fail!");
  return false;}
  
  lcdRegWrite(RA8889_CCR);//01h
  lcdDataWrite(RA8889_PLL_ENABLE<<7|RA8889_WAIT_NO_MASK<<6|RA8889_KEY_SCAN_DISABLE<<5|RA8889_TFT_OUTPUT24<<3
  |RA8889_I2C_MASTER_DISABLE<<2|RA8889_SERIAL_IF_ENABLE<<1|RA8889_HOST_DATA_BUS_SERIAL);

  lcdRegWrite(RA8889_MACR);//02h
  lcdDataWrite(RA8889_DIRECT_WRITE<<6|RA8889_READ_MEMORY_LRTB<<4|RA8889_WRITE_MEMORY_LRTB<<1);

  lcdRegWrite(RA8889_ICR);//03h
  lcdDataWrite(RA8889_GRAPHIC_MODE<<2|RA8889_MEMORY_SELECT_IMAGE);

  #ifdef COLOR_DEPTH_16BPP
  lcdRegWrite(RA8889_MPWCTR);//10h
  lcdDataWrite(RA8889_PIP1_WINDOW_DISABLE<<7|RA8889_PIP2_WINDOW_DISABLE<<6|RA8889_SELECT_CONFIG_PIP1<<4
  |RA8889_IMAGE_COLOCR_DEPTH_16BPP<<2|TFT_MODE);

  lcdRegWrite(RA8889_PIPCDEP);//11h
  lcdDataWrite(RA8889_PIP1_COLOR_DEPTH_16BPP<<2|RA8889_PIP2_COLOR_DEPTH_16BPP);
  
  lcdRegWrite(RA8889_AW_COLOR);//5Eh
  lcdDataWrite(RA8889_CANVAS_BLOCK_MODE<<2|RA8889_CANVAS_COLOR_DEPTH_16BPP);
  lcdRegDataWrite(RA8889_BTE_COLR,RA8889_S0_COLOR_DEPTH_16BPP<<5|RA8889_S1_COLOR_DEPTH_16BPP<<2|RA8889_S0_COLOR_DEPTH_16BPP);//92h
  #endif
  
   #ifdef COLOR_DEPTH_24BPP
  lcdRegWrite(RA8889_MPWCTR);//10h
  lcdDataWrite(RA8889_PIP1_WINDOW_DISABLE<<7|RA8889_PIP2_WINDOW_DISABLE<<6|RA8889_SELECT_CONFIG_PIP1<<4
  |RA8889_IMAGE_COLOCR_DEPTH_24BPP<<2|TFT_MODE);

  lcdRegWrite(RA8889_PIPCDEP);//11h
  lcdDataWrite(RA8889_PIP1_COLOR_DEPTH_24BPP<<2|RA8889_PIP2_COLOR_DEPTH_24BPP);
  
  lcdRegWrite(RA8889_AW_COLOR);//5Eh
  lcdDataWrite(RA8889_CANVAS_BLOCK_MODE<<2|RA8889_CANVAS_COLOR_DEPTH_24BPP);
  lcdRegDataWrite(RA8889_BTE_COLR,RA8889_S0_COLOR_DEPTH_24BPP<<5|RA8889_S1_COLOR_DEPTH_24BPP<<2|RA8889_S0_COLOR_DEPTH_24BPP);//92h
  #endif

  
  /*TFT timing configure*/
  lcdRegWrite(RA8889_DPCR);//12h
  lcdDataWrite(XPCLK_INV<<7|RA8889_DISPLAY_OFF<<6|RA8889_OUTPUT_RGB);
 
  lcdRegWrite(RA8889_PCSR);//13h
  lcdDataWrite(XHSYNC_INV<<7|XVSYNC_INV<<6|XDE_INV<<5);
    
  lcdHorizontalWidthVerticalHeight(HDW,VDH);
  lcdHorizontalNonDisplay(HND);
  lcdHsyncStartPosition(HST);
  lcdHsyncPulseWidth(HPW);
  lcdVerticalNonDisplay(VND);
  lcdVsyncStartPosition(VST);
  lcdVsyncPulseWidth(VPW);
  
  /*image buffer configure*/
  displayImageStartAddress(PAGE1_START_ADDR);
  displayImageWidth(SCREEN_WIDTH);
  displayWindowStartXY(0,0);
  canvasImageStartAddress(PAGE1_START_ADDR);
  canvasImageWidth(SCREEN_WIDTH);
  activeWindowXY(0,0);
  activeWindowWH(SCREEN_WIDTH,SCREEN_HEIGHT);
  return true;
}
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::lcdRegWrite(ru8 reg) 
{
  SPI.transfer(_xnscs,RA8889_SPI_CMDWRITE,SPI_CONTINUE);
  SPI.transfer(_xnscs,reg);
}
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::lcdDataWrite(ru8 data) 
{
  SPI.transfer(_xnscs,RA8889_SPI_DATAWRITE,SPI_CONTINUE);
  SPI.transfer(_xnscs,data);
}
//**************************************************************//
//**************************************************************//
ru8 Ra8889_Lite::lcdDataRead(void) 
{
  SPI.transfer(_xnscs,RA8889_SPI_DATAREAD,SPI_CONTINUE);
  ru8 data = SPI.transfer(_xnscs,0xff);
  return data;
}
//**************************************************************//
//**************************************************************//
ru8 Ra8889_Lite::lcdStatusRead(void) 
{
  SPI.transfer(_xnscs,RA8889_SPI_STATUSREAD,SPI_CONTINUE);
  ru8 data= SPI.transfer(_xnscs,0xff);
  return data;
}
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::lcdRegDataWrite(ru8 reg,ru8 data)
{
  lcdRegWrite(reg);
  lcdDataWrite(data);
}
//**************************************************************//
//**************************************************************//
ru8 Ra8889_Lite::lcdRegDataRead(ru8 reg)
{
  lcdRegWrite(reg);
  return lcdDataRead();
}

//**************************************************************//
// support SPI interface to write 16bpp data after Regwrite 04h
//**************************************************************//
void Ra8889_Lite::lcdDataWrite16bbp(ru16 data) 
{
  SPI.transfer(_xnscs,RA8889_SPI_DATAWRITE,SPI_CONTINUE);
  SPI.transfer(_xnscs,data,SPI_CONTINUE);
  SPI.transfer(_xnscs,data>>8);
}

//**************************************************************//
//RA8889 register 
//**************************************************************//
/*[Status Register] bit7  Host Memory Write FIFO full
0: Memory Write FIFO is not full.
1: Memory Write FIFO is full.
Only when Memory Write FIFO is not full, MPU may write another one pixel.*/ 
//**************************************************************//
void Ra8889_Lite::checkWriteFifoNotFull(void)
{  ru16 i;  
   for(i=0;i<10000;i++) //Please according to your usage to modify i value.
   {
    if( (lcdStatusRead()&0x80)==0 ){break;}
   }
}

//**************************************************************//
/*[Status Register] bit6  Host Memory Write FIFO empty
0: Memory Write FIFO is not empty.
1: Memory Write FIFO is empty.
When Memory Write FIFO is empty, MPU may write 8bpp data 64
pixels, or 16bpp data 32 pixels, 24bpp data 16 pixels directly.*/
//**************************************************************//
void Ra8889_Lite::checkWriteFifoEmpty(void)
{ ru16 i;
   for(i=0;i<10000;i++)   //Please according to your usage to modify i value.
   {
    if( (lcdStatusRead()&0x40)==0x40 ){break;}
   }
}

//**************************************************************//
/*[Status Register] bit5  Host Memory Read FIFO full
0: Memory Read FIFO is not full.
1: Memory Read FIFO is full.
When Memory Read FIFO is full, MPU may read 8bpp data 32
pixels, or 16bpp data 16 pixels, 24bpp data 8 pixels directly.*/
//**************************************************************//
void Ra8889_Lite::checkReadFifoNotFull(void)
{ ru16 i;
  for(i=0;i<10000;i++)  //Please according to your usage to modify i value.
  {if( (lcdStatusRead()&0x20)==0x00 ){break;}}
}

//**************************************************************//
/*[Status Register] bit4   Host Memory Read FIFO empty
0: Memory Read FIFO is not empty.
1: Memory Read FIFO is empty.*/
//**************************************************************//
void Ra8889_Lite::checkReadFifoNotEmpty(void)
{ ru16 i;
  for(i=0;i<10000;i++)// //Please according to your usage to modify i value. 
  {if( (lcdStatusRead()&0x10)==0x00 ){break;}}
}

//**************************************************************//
/*[Status Register] bit3   Core task is busy
Following task is running:
BTE, Geometry engine, Serial flash DMA, Text write or Graphic write
0: task is done or idle.   1: task is busy*/
//**************************************************************//
void Ra8889_Lite::check2dBusy(void)
{  ru32 i;
   for(i=0;i<1000000;i++)   //Please according to your usage to modify i value.
   {
    delayMicroseconds(1);
    if( (lcdStatusRead()&0x08)==0x00 )
    {break;}
   }
}  

//**************************************************************//
/*[Status Register] bit2   SDRAM ready for access
0: SDRAM is not ready for access   1: SDRAM is ready for access*/	
//**************************************************************//
boolean Ra8889_Lite::checkSdramReady(void)
{ru32 i;
 for(i=0;i<1000000;i++) //Please according to your usage to modify i value.
 { 
   delayMicroseconds(1);
   if( (lcdStatusRead()&0x04)==0x04 )
    {return true;}
    
 }
 return false;
}

//**************************************************************//
/*[Status Register] bit1  Operation mode status
0: Normal operation state  1: Inhibit operation state
Inhibit operation state means internal reset event keep running or
initial display still running or chip enter power saving state.	*/
//**************************************************************//
boolean Ra8889_Lite::checkIcReady(void)
{ru32 i;
  for(i=0;i<1000000;i++)  //Please according to your usage to modify i value.
   {
     delayMicroseconds(1);
     if( (lcdStatusRead()&0x02)==0x00 )
     {return true;}     
   }
   return false;
}
//**************************************************************//
//**************************************************************//
//[05h] [06h] [07h] [08h] [09h] [0Ah]
//------------------------------------//----------------------------------*/
boolean Ra8889_Lite::ra8889PllInitial(ru8 dram_clk,ru8 core_clk,ru8 scan_clk) 
{
/*(1) OSC_FREQ = 10MHz 
  (2) 10MHz <= (OSC_FREQ/PLLDIVM) <= 40MHz
  (3) 250MHz <= [OSC_FREQ/(PLLDIVM+1)]x(PLLDIVN+1) <= 500MHz
PLLDIVM:0
PLLDIVN:1~63
PLLDIVK:CPLL & MPLL = 1/2/4/8.SPLL = 1/2/4/8/16/32/64/128.
ex:
 OSC_FREQ = 10MHz
 Set X_DIVK=2
 Set X_DIVM=0
 => (X_DIVN+1)=(XPLLx4)/10*/


		// Set internal Sdram Ram clock
		if((dram_clk>=125)&&(dram_clk<=166))					
		{
			lcdRegDataWrite(0x07,0x02);				//PLL Divided by 2
			lcdRegDataWrite(0x08,(dram_clk*2/10)-1);
		}
		else if((dram_clk>=63)&&(dram_clk<=124))					
		{
			lcdRegDataWrite(0x07,0x04);				//PLL Divided by 4
			lcdRegDataWrite(0x08,(dram_clk*4/10)-1);
		}
		else if((dram_clk>=32)&&(dram_clk<=62))
		{								  	
			lcdRegDataWrite(0x07,0x06);				//PLL Divided by 8
			lcdRegDataWrite(0x08,(dram_clk*8/10)-1);
		}
		else
		{
			lcdRegDataWrite(0x07,0x06);				//PLL Divided by 8
			lcdRegDataWrite(0x08,(32*8/10)-1);	//set to 32MHz if out of range
		}

// Set Core clock
		if((core_clk>=125)&&(core_clk<=133))
		{
			lcdRegDataWrite(0x09,0x02);				//PLL Divided by 2
			lcdRegDataWrite(0x0A,(CORE_FREQ*2/10)-1);
		}

		else if((core_clk>=63)&&(core_clk<=124))					
		{
			lcdRegDataWrite(0x09,0x04);				//PLL Divided by 4
			lcdRegDataWrite(0x0A,(core_clk*4/10)-1);
		}

		else if((core_clk>=32)&&(core_clk<=62))
		{								  	
			lcdRegDataWrite(0x09,0x06);				//PLL Divided by 8
			lcdRegDataWrite(0x0A,(core_clk*8/10)-1);
		}
		else
		{
			lcdRegDataWrite(0x09,0x06);				//PLL Divided by 8
			lcdRegDataWrite(0x0A,(32*8/10)-1);	//set to 32MHz if out of range
		}


	// Set tft output pixel clock
		if((scan_clk>=63)&&(scan_clk<=100))						
		{
			lcdRegDataWrite(0x05,0x04);				//PLL Divided by 4
			lcdRegDataWrite(0x06,(scan_clk*4/10)-1);
		}
		else if((scan_clk>=32)&&(scan_clk<=62))
		{								  	
			lcdRegDataWrite(0x05,0x06);				//PLL Divided by 8
			lcdRegDataWrite(0x06,(scan_clk*8/10)-1);
		}
		else if((scan_clk>=16)&&(scan_clk<=31))
		{								  	
			lcdRegDataWrite(0x05,0x16);				//PLL Divided by 16
			lcdRegDataWrite(0x06,(scan_clk*16/10)-1);
		}
		else if((scan_clk>=8)&&(scan_clk<=15))
		{
			lcdRegDataWrite(0x05,0x26);				//PLL Divided by 32
			lcdRegDataWrite(0x06,(scan_clk*32/10)-1);
		}
		else if((scan_clk>0)&&(scan_clk<=7))
		{
			lcdRegDataWrite(0x05,0x36);				//PLL Divided by 64
			lcdRegDataWrite(0x06,(scan_clk*64/10)-1);
		}
                else								    
                {
                       	lcdRegDataWrite(0x05,0x06);				
			lcdRegDataWrite(0x06,(32*8/10)-1);          //set to 32MHz if out of range
                }
			
  delay(1);
  lcdRegWrite(0x01);
  lcdDataWrite(0x80);
  delay(2);//wait for pll stable
  if((lcdDataRead()&0x80)==0x80)
  return true;
  else
  return false; 
}

//**************************************************************//
boolean Ra8889_Lite::ra8889SdramInitial(void)
{
ru8	CAS_Latency;
ru16	Auto_Refresh;
			
  CAS_Latency=3;

  Auto_Refresh=(64*DRAM_FREQ*1000)/(4096);
  Auto_Refresh=Auto_Refresh-2;  // Start [refresh] in advance to avoid just reaching the limits.
  lcdRegDataWrite(0xe0,0x29);        
  lcdRegDataWrite(0xe1,CAS_Latency);      //CAS:2=0x02，CAS:3=0x03
  lcdRegDataWrite(0xe2,Auto_Refresh);
  lcdRegDataWrite(0xe3,Auto_Refresh>>8);
  lcdRegDataWrite(0xe4,0x01);
  
  checkSdramReady();
}
 //**************************************************************//
//**************************************************************//
 void Ra8889_Lite::displayOn(boolean on)
 {
  if(on)
   lcdRegDataWrite(RA8889_DPCR, XPCLK_INV<<7|RA8889_DISPLAY_ON<<6|RA8889_OUTPUT_RGB);
  else
   lcdRegDataWrite(RA8889_DPCR, XPCLK_INV<<7|RA8889_DISPLAY_OFF<<6|RA8889_OUTPUT_RGB);
   
  delay(20);
 }
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::lcdHorizontalWidthVerticalHeight(ru16 width,ru16 height)
{unsigned char temp;
   temp=(width/8)-1;
   lcdRegDataWrite(RA8889_HDWR,temp);
   temp=width%8;
   lcdRegDataWrite(RA8889_HDWFTR,temp);
   temp=height-1;
   lcdRegDataWrite(RA8889_VDHR0,temp);
   temp=(height-1)>>8;
   lcdRegDataWrite(RA8889_VDHR1,temp);
}
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::lcdHorizontalNonDisplay(ru16 numbers)
{ru8 temp;
 if(numbers<8)
  {
   lcdRegDataWrite(RA8889_HNDR,0x00);
   lcdRegDataWrite(RA8889_HNDFTR,numbers);
  }
 else
  {
  temp=(numbers/8)-1;
  lcdRegDataWrite(RA8889_HNDR,temp);
  temp=numbers%8;
  lcdRegDataWrite(RA8889_HNDFTR,temp);
  }	
}
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::lcdHsyncStartPosition(ru16 numbers)
{ru8 temp;
 if(numbers<8)
  {
   lcdRegDataWrite(RA8889_HSTR,0x00);
  }
  else
  {
   temp=(numbers/8)-1;
   lcdRegDataWrite(RA8889_HSTR,temp);
  }
}
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::lcdHsyncPulseWidth(ru16 numbers)
{ru8 temp;
 if(numbers<8)
  {
   lcdRegDataWrite(RA8889_HPWR,0x00);
  }
  else
  {
   temp=(numbers/8)-1;
   lcdRegDataWrite(RA8889_HPWR,temp);
  }
}
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::lcdVerticalNonDisplay(ru16 numbers)
{ru8 temp;
  temp=numbers-1;
  lcdRegDataWrite(RA8889_VNDR0,temp);
  lcdRegDataWrite(RA8889_VNDR1,temp>>8);
}
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::lcdVsyncStartPosition(ru16 numbers)
{ru8 temp;
  temp=numbers-1;
  lcdRegDataWrite(RA8889_VSTR,temp);
}
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::lcdVsyncPulseWidth(ru16 numbers)
{ru8 temp;
  temp=numbers-1;
  lcdRegDataWrite(RA8889_VPWR,temp);
}
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::displayImageStartAddress(ru32 addr)	
{
  lcdRegDataWrite(RA8889_MISA0,addr);//20h
  lcdRegDataWrite(RA8889_MISA1,addr>>8);//21h 
  lcdRegDataWrite(RA8889_MISA2,addr>>16);//22h  
  lcdRegDataWrite(RA8889_MISA3,addr>>24);//23h 
}
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::displayImageWidth(ru16 width)	
{
  lcdRegDataWrite(RA8889_MIW0,width); //24h
  lcdRegDataWrite(RA8889_MIW1,width>>8); //25h 
}
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::displayWindowStartXY(ru16 x0,ru16 y0)	
{
  lcdRegDataWrite(RA8889_MWULX0,x0);//26h
  lcdRegDataWrite(RA8889_MWULX1,x0>>8);//27h
  lcdRegDataWrite(RA8889_MWULY0,y0);//28h
  lcdRegDataWrite(RA8889_MWULY1,y0>>8);//29h
}
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::canvasImageStartAddress(ru32 addr)	
{
  lcdRegDataWrite(RA8889_CVSSA0,addr);//50h
  lcdRegDataWrite(RA8889_CVSSA1,addr>>8);//51h
  lcdRegDataWrite(RA8889_CVSSA2,addr>>16);//52h
  lcdRegDataWrite(RA8889_CVSSA3,addr>>24);//53h  
}
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::canvasImageWidth(ru16 width)	
{
  lcdRegDataWrite(RA8889_CVS_IMWTH0,width);//54h
  lcdRegDataWrite(RA8889_CVS_IMWTH1,width>>8); //55h
}
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::activeWindowXY(ru16 x0,ru16 y0)	
{
  lcdRegDataWrite(RA8889_AWUL_X0,x0);//56h
  lcdRegDataWrite(RA8889_AWUL_X1,x0>>8);//57h 
  lcdRegDataWrite(RA8889_AWUL_Y0,y0);//58h
  lcdRegDataWrite(RA8889_AWUL_Y1,y0>>8);//59h 
}
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::activeWindowWH(ru16 width,ru16 height)	
{
  lcdRegDataWrite(RA8889_AW_WTH0,width);//5ah
  lcdRegDataWrite(RA8889_AW_WTH1,width>>8);//5bh
  lcdRegDataWrite(RA8889_AW_HT0,height);//5ch
  lcdRegDataWrite(RA8889_AW_HT1,height>>8);//5dh  
}
//**************************************************************//
//**************************************************************//
void Ra8889_Lite:: setPixelCursor(ru16 x,ru16 y)
{
  lcdRegDataWrite(RA8889_CURH0,x); //5fh
  lcdRegDataWrite(RA8889_CURH1,x>>8);//60h
  lcdRegDataWrite(RA8889_CURV0,y);//61h
  lcdRegDataWrite(RA8889_CURV1,y>>8);//62h
}
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::bte_Source0_MemoryStartAddr(ru32 addr)	
{
  lcdRegDataWrite(RA8889_S0_STR0,addr);//93h
  lcdRegDataWrite(RA8889_S0_STR1,addr>>8);//94h
  lcdRegDataWrite(RA8889_S0_STR2,addr>>16);//95h
  lcdRegDataWrite(RA8889_S0_STR3,addr>>24);////96h
}
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::bte_Source0_ImageWidth(ru16 width)	
{
  lcdRegDataWrite(RA8889_S0_WTH0,width);//97h
  lcdRegDataWrite(RA8889_S0_WTH1,width>>8);//98h
}
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::bte_Source0_WindowStartXY(ru16 x0,ru16 y0)	
{
  lcdRegDataWrite(RA8889_S0_X0,x0);//99h
  lcdRegDataWrite(RA8889_S0_X1,x0>>8);//9ah
  lcdRegDataWrite(RA8889_S0_Y0,y0);//9bh
  lcdRegDataWrite(RA8889_S0_Y1,y0>>8);//9ch
}
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::bte_Source1_MemoryStartAddr(ru32 addr)	
{
  lcdRegDataWrite(RA8889_S1_STR0,addr);//9dh
  lcdRegDataWrite(RA8889_S1_STR1,addr>>8);//9eh
  lcdRegDataWrite(RA8889_S1_STR2,addr>>16);//9fh
  lcdRegDataWrite(RA8889_S1_STR3,addr>>24);//a0h
}
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::bte_Source1_ImageWidth(ru16 width)	
{
  lcdRegDataWrite(RA8889_S1_WTH0,width);//a1h
  lcdRegDataWrite(RA8889_S1_WTH1,width>>8);//a2h
}
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::bte_Source1_WindowStartXY(ru16 x0,ru16 y0)	
{
  lcdRegDataWrite(RA8889_S1_X0,x0);//a3h
  lcdRegDataWrite(RA8889_S1_X1,x0>>8);//a4h
  lcdRegDataWrite(RA8889_S1_Y0,y0);//a5h
  lcdRegDataWrite(RA8889_S1_Y1,y0>>8);//a6h
}
//**************************************************************//
//**************************************************************//
void  Ra8889_Lite::bte_DestinationMemoryStartAddr(ru32 addr)	
{
  lcdRegDataWrite(RA8889_DT_STR0,addr);//a7h
  lcdRegDataWrite(RA8889_DT_STR1,addr>>8);//a8h
  lcdRegDataWrite(RA8889_DT_STR2,addr>>16);//a9h
  lcdRegDataWrite(RA8889_DT_STR3,addr>>24);//aah
}
//**************************************************************//
//**************************************************************//
void  Ra8889_Lite::bte_DestinationImageWidth(ru16 width)	
{
  lcdRegDataWrite(RA8889_DT_WTH0,width);//abh
  lcdRegDataWrite(RA8889_DT_WTH1,width>>8);//ach
}
//**************************************************************//
//**************************************************************//
void  Ra8889_Lite::bte_DestinationWindowStartXY(ru16 x0,ru16 y0)	
{
  lcdRegDataWrite(RA8889_DT_X0,x0);//adh
  lcdRegDataWrite(RA8889_DT_X1,x0>>8);//aeh
  lcdRegDataWrite(RA8889_DT_Y0,y0);//afh
  lcdRegDataWrite(RA8889_DT_Y1,y0>>8);//b0h
}
//**************************************************************//
//**************************************************************//
void  Ra8889_Lite::bte_WindowSize(ru16 width, ru16 height)
{
  lcdRegDataWrite(RA8889_BTE_WTH0,width);//b1h
  lcdRegDataWrite(RA8889_BTE_WTH1,width>>8);//b2h
  lcdRegDataWrite(RA8889_BTE_HIG0,height);//b3h
  lcdRegDataWrite(RA8889_BTE_HIG1,height>>8);//b4h
}

//**************************************************************//
/*These 8 bits determine prescaler value for Timer 0 and 1.*/
/*Time base is “Core_Freq / (Prescaler + 1)”*/
//**************************************************************//
void Ra8889_Lite::pwm_Prescaler(ru8 prescaler)
{
  lcdRegDataWrite(RA8889_PSCLR,prescaler);//84h
}
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::pwm_ClockMuxReg(ru8 pwm1_clk_div, ru8 pwm0_clk_div, ru8 xpwm1_ctrl, ru8 xpwm0_ctrl)
{
  lcdRegDataWrite(RA8889_PMUXR,pwm1_clk_div<<6|pwm0_clk_div<<4|xpwm1_ctrl<<2|xpwm0_ctrl);//85h
}
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::pwm_Configuration(ru8 pwm1_inverter,ru8 pwm1_auto_reload,ru8 pwm1_start,ru8 
                       pwm0_dead_zone, ru8 pwm0_inverter, ru8 pwm0_auto_reload,ru8 pwm0_start)
 {
  lcdRegDataWrite(RA8889_PCFGR,pwm1_inverter<<6|pwm1_auto_reload<<5|pwm1_start<<4|pwm0_dead_zone<<3|
                  pwm0_inverter<<2|pwm0_auto_reload<<1|pwm0_start);//86h                
 }   
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::pwm0_Duty(ru16 duty)
{
  lcdRegDataWrite(RA8889_TCMPB0L,duty);//88h 
  lcdRegDataWrite(RA8889_TCMPB0H,duty>>8);//89h 
}
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::pwm0_ClocksPerPeriod(ru16 clocks_per_period)
{
  lcdRegDataWrite(RA8889_TCNTB0L,clocks_per_period);//8ah
  lcdRegDataWrite(RA8889_TCNTB0H,clocks_per_period>>8);//8bh
}
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::pwm1_Duty(ru16 duty)
{
  lcdRegDataWrite(RA8889_TCMPB1L,duty);//8ch 
  lcdRegDataWrite(RA8889_TCMPB1H,duty>>8);//8dh
}
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::pwm1_ClocksPerPeriod(ru16 clocks_per_period)
{
  lcdRegDataWrite(RA8889_TCNTB1L,clocks_per_period);//8eh
  lcdRegDataWrite(RA8889_TCNTB1F,clocks_per_period>>8);//8fh
}
//**************************************************************//
//**************************************************************//
void Ra8889_Lite:: ramAccessPrepare(void)
{
  lcdRegWrite(RA8889_MRWDP); //04h
}
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::foreGroundColor16bpp(ru16 color)
{
  lcdRegDataWrite(RA8889_FGCR,color>>8);//d2h
  lcdRegDataWrite(RA8889_FGCG,color>>3);//d3h
  lcdRegDataWrite(RA8889_FGCB,color<<3);//d4h
}
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::backGroundColor16bpp(ru16 color)
{
  lcdRegDataWrite(RA8889_BGCR,color>>8);//d5h
  lcdRegDataWrite(RA8889_BGCG,color>>3);//d6h
  lcdRegDataWrite(RA8889_BGCB,color<<3);//d7h
}

//**************************************************************//
//**************************************************************//
void Ra8889_Lite::foreGroundColor24bpp(ru32 color)
{
  lcdRegDataWrite(RA8889_FGCR,color>>16);//d2h
  lcdRegDataWrite(RA8889_FGCG,color>>8);//d3h
  lcdRegDataWrite(RA8889_FGCB,color);//d4h
}
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::backGroundColor24bpp(ru32 color)
{
  lcdRegDataWrite(RA8889_BGCR,color>>16);//d5h
  lcdRegDataWrite(RA8889_BGCG,color>>8);//d6h
  lcdRegDataWrite(RA8889_BGCB,color);//d7h
}

 /*graphic function*/
//**************************************************************//
/* If use ra8889 graphic mode function turns on */
//**************************************************************//
 void Ra8889_Lite::graphicMode(boolean on)
 {
  if(on)
   lcdRegDataWrite(RA8889_ICR,RA8889_GRAPHIC_MODE<<2|RA8889_MEMORY_SELECT_IMAGE);//03h  //switch to graphic mode
  else
   lcdRegDataWrite(RA8889_ICR,RA8889_TEXT_MODE<<2|RA8889_MEMORY_SELECT_IMAGE);//03h  //switch back to text mode
 }

 #ifdef COLOR_DEPTH_16BPP 
//**************************************************************//
/*support serial IF to write 16bpp pixel*/
//**************************************************************//
void Ra8889_Lite:: putPixel_16bpp(ru16 x,ru16 y,ru16 color)
{
 setPixelCursor(x,y);
 ramAccessPrepare();
 //checkWriteFifoNotFull();//if high speed mcu and without Xnwait check
 //lcdDataWrite(color);
 //lcdDataWrite(color>>8);
 lcdDataWrite16bbp(color);
 //checkWriteFifoEmpty();//if high speed mcu and without Xnwait check
}
//**************************************************************//
/*support ra8889 serial IF to write 16bpp(RGB565) picture data for user operation */
//**************************************************************//
void Ra8889_Lite:: putPicture_16bpp(ru16 x,ru16 y,ru16 width, ru16 height)
{ru16 i,j;
 activeWindowXY(x,y);
 activeWindowWH(width,height);
 setPixelCursor(x,y);
 ramAccessPrepare();
}
//**************************************************************//
/*support serial IF to write 16bpp(RGB565) picture data byte format from data pointer*/
//**************************************************************//
void Ra8889_Lite:: putPicture_16bpp(ru16 x,ru16 y,ru16 width, ru16 height, const unsigned char *data)
{ru16 i,j;

 activeWindowXY(x,y);
 activeWindowWH(width,height);
 setPixelCursor(x,y);
 ramAccessPrepare();
 for(j=0;j<height;j++)
 {
  for(i=0;i<width;i++)
  {
   //checkWriteFifoNotFull();//if high speed mcu and without Xnwait check
   lcdDataWrite(*data);
   data++;
   //checkWriteFifoNotFull();//if high speed mcu and without Xnwait check
   lcdDataWrite(*data);
   data++;
  }
 } 
 checkWriteFifoEmpty();//if high speed mcu and without Xnwait check
 activeWindowXY(0,0);
 activeWindowWH(SCREEN_WIDTH,SCREEN_HEIGHT);
}
//**************************************************************//
/*support serial IF to write 16bpp(RGB565) picture data word format from data pointer*/
//**************************************************************//
void Ra8889_Lite:: putPicture_16bpp(ru16 x,ru16 y,ru16 width, ru16 height, const unsigned short *data)
{ru16 i,j;
 activeWindowXY(x,y);
 activeWindowWH(width,height);
 setPixelCursor(x,y);
 ramAccessPrepare();
 for(j=0;j<height;j++)
 {
  for(i=0;i<width;i++)
  {
   //checkWriteFifoNotFull();//if high speed mcu and without Xnwait check
   lcdDataWrite16bbp(*data);
   data++;
   //checkWriteFifoEmpty();//if high speed mcu and without Xnwait check
  }
 } 
 checkWriteFifoEmpty();//if high speed mcu and without Xnwait check
 activeWindowXY(0,0);
 activeWindowWH(SCREEN_WIDTH,SCREEN_HEIGHT);
}

#endif

#ifdef COLOR_DEPTH_24BPP 

//**************************************************************//
/*support serial IF to write 24bpp pixel*/
//**************************************************************//
void Ra8889_Lite:: putPixel_24bpp(ru16 x,ru16 y,ru32 color)
{
 setPixelCursor(x,y);
 ramAccessPrepare();
 //checkWriteFifoNotFull();//if high speed mcu and without Xnwait check
 
 lcdDataWrite(color);
 lcdDataWrite(color>>8);
 lcdDataWrite(color>>16);
 //checkWriteFifoEmpty();//if high speed mcu and without Xnwait check
}
//**************************************************************//
/*support serial IF to write 24bpp(RGB888) picture data byte format from data pointer*/
//**************************************************************//
void Ra8889_Lite:: putPicture_24bpp(ru16 x,ru16 y,ru16 width, ru16 height)
{ru16 i,j;
 activeWindowXY(x,y);
 activeWindowWH(width,height);
 setPixelCursor(x,y);
 ramAccessPrepare();
}

//**************************************************************//
/*support serial IF to write 24bpp(RGB888) picture data byte format from data pointer*/
//**************************************************************//
void Ra8889_Lite:: putPicture_24bpp(ru16 x,ru16 y,ru16 width, ru16 height, const unsigned char *data)
{ru16 i,j;
 activeWindowXY(x,y);
 activeWindowWH(width,height);
 setPixelCursor(x,y);
 ramAccessPrepare();
 for(j=0;j<height;j++)
 {
  for(i=0;i<width;i++)
  {
   checkWriteFifoNotFull();//if high speed mcu and without Xnwait check
   lcdDataWrite(*data);
   data++;
   checkWriteFifoNotFull();//if high speed mcu and without Xnwait check
   lcdDataWrite(*data);
   data++;
   checkWriteFifoNotFull();//if high speed mcu and without Xnwait check
   lcdDataWrite(*data);
   data++;
  }
 } 
 checkWriteFifoEmpty();//if high speed mcu and without Xnwait check
 activeWindowXY(0,0);
 activeWindowWH(SCREEN_WIDTH,SCREEN_HEIGHT);
}

#endif


/*text function*/
//**************************************************************//
/* If use ra8889 text mode function turns on */
//**************************************************************//
void Ra8889_Lite::textMode(boolean on)
 {
  if(on)
   lcdRegDataWrite(RA8889_ICR,RA8889_TEXT_MODE<<2|RA8889_MEMORY_SELECT_IMAGE);//03h  //switch to text mode
  else
   lcdRegDataWrite(RA8889_ICR,RA8889_GRAPHIC_MODE<<2|RA8889_MEMORY_SELECT_IMAGE);//03h  //switch back to graphic mode

 }
 //**************************************************************//
 //**************************************************************//
void Ra8889_Lite::textColor(ru32 foreground_color,ru32 background_color)
 {
  #ifdef COLOR_DEPTH_16BPP 
   foreGroundColor16bpp(foreground_color);
   backGroundColor16bpp(background_color);
  #endif
  #ifdef COLOR_DEPTH_24BPP 
   foreGroundColor24bpp(foreground_color);
   backGroundColor24bpp(background_color);
  #endif
  
 }
 //**************************************************************//
 //**************************************************************//
void Ra8889_Lite:: setTextCursor(ru16 x,ru16 y)
{
  lcdRegDataWrite(RA8889_F_CURX0,x); //63h
  lcdRegDataWrite(RA8889_F_CURX1,x>>8);//64h
  lcdRegDataWrite(RA8889_F_CURY0,y);//65h
  lcdRegDataWrite(RA8889_F_CURY1,y>>8);//66h
}
 //**************************************************************//
 /* source_select = 0 : internal CGROM,  source_select = 1: external CGROM, source_select = 2: user-define*/
 /* size_select = 0 : 8*16/16*16, size_select = 1 : 12*24/24*24, size_select = 2 : 16*32/32*32  */
 /* iso_select = 0 : iso8859-1, iso_select = 1 : iso8859-2, iso_select = 2 : iso8859-4, iso_select = 3 : iso8859-5*/
 //**************************************************************//
 void Ra8889_Lite::setTextParameter1(ru8 source_select,ru8 size_select,ru8 iso_select)//cch
 {
   lcdRegDataWrite(RA8889_CCR0,source_select<<6|size_select<<4|iso_select);//cch
 }
 //**************************************************************//
 /*align = 0 : full alignment disable, align = 1 : full alignment enable*/
 /*chroma_key = 0 : text with chroma key disable, chroma_key = 1 : text with chroma key enable*/
 /* width_enlarge and height_enlarge can be set 0~3, (00b: X1) (01b : X2)  (10b : X3)  (11b : X4)*/
 //**************************************************************//
void Ra8889_Lite::setTextParameter2(ru8 align, ru8 chroma_key, ru8 width_enlarge, ru8 height_enlarge)
{
  lcdRegDataWrite(RA8889_CCR1,align<<7|chroma_key<<6|width_enlarge<<2|height_enlarge);//cdh
}
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::genitopCharacterRomParameter(ru8 scs_select, ru8 clk_div, ru8 rom_select, ru8 character_select, ru8 gt_width)
{ 
  if(scs_select==0)
  lcdRegDataWrite(RA8889_SFL_CTRL,RA8889_SERIAL_FLASH_SELECT0<<7|RA8889_SERIAL_FLASH_FONT_MODE<<6|RA8889_SERIAL_FLASH_ADDR_24BIT<<5|RA8889_FOLLOW_RA8875_MODE<<4|RA8889_SPI_FAST_READ_8DUMMY);//b7h
  if(scs_select==1)
  lcdRegDataWrite(RA8889_SFL_CTRL,RA8889_SERIAL_FLASH_SELECT1<<7|RA8889_SERIAL_FLASH_FONT_MODE<<6|RA8889_SERIAL_FLASH_ADDR_24BIT<<5|RA8889_FOLLOW_RA8875_MODE<<4|RA8889_SPI_FAST_READ_8DUMMY);//b7h
  
  lcdRegDataWrite(RA8889_SPI_DIVSOR,clk_div);//bbh 
  
  lcdRegDataWrite(RA8889_GTFNT_SEL,rom_select<<5);//ceh
  lcdRegDataWrite(RA8889_GTFNT_CR,character_select<<3|gt_width);//cfh
}
//**************************************************************//
//support ra8889 internal font and external string font code write from data pointer
//**************************************************************//
void Ra8889_Lite:: putString(ru16 x0,ru16 y0, char *str)
{
  textMode(true);
  setTextCursor(x0,y0);
  ramAccessPrepare();
  while(*str != '\0')
  {
  checkWriteFifoNotFull();  
  lcdDataWrite(*str);
  ++str; 
  } 
  check2dBusy();
  textMode(false);
}
/*put value,base on sprintf*/
//**************************************************************//
//vaule: -2147483648(-2^31) ~ 2147483647(2^31-1)
//len: 1~11 minimum output length
/*
  [flag] 
    n:靠右　
    -:靠左
    +:輸出正負號
    (space):當不輸出正負號時，就輸出空白
    0:在開頭處(左側) 補 0，而非補空白。 
 */
//**************************************************************//
void Ra8889_Lite:: putDec(ru16 x0,ru16 y0,rs32 vaule,ru8 len,const char *flag)
{
  char char_buffer[12];
  switch(len)
  {
    case 1:
           if(flag=="n")
           {sprintf(char_buffer ,"%1d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="-")
           {sprintf(char_buffer ,"%-1d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="+")
           {sprintf(char_buffer ,"%+1d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="0")
           {sprintf(char_buffer ,"%01d", vaule); putString(x0,y0,char_buffer);}
           break;
    case 2:
           if(flag=="n")
           {sprintf(char_buffer ,"%2d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="-")
           { sprintf(char_buffer ,"%-2d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="+")
           { sprintf(char_buffer ,"%+2d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="0")
           { sprintf(char_buffer ,"%02d", vaule); putString(x0,y0,char_buffer);}
           break; 
    case 3: 
           if(flag=="n")
           {sprintf(char_buffer ,"%3d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="-")
           {sprintf(char_buffer ,"%-3d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="+")
           {sprintf(char_buffer ,"%+3d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="0")
           {sprintf(char_buffer ,"%03d", vaule); putString(x0,y0,char_buffer);}
           break;
    case 4: 
           if(flag=="n")
           {sprintf(char_buffer ,"%4d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="-")
           { sprintf(char_buffer ,"%-4d", vaule);  putString(x0,y0,char_buffer);}
           else if(flag=="+")
           { sprintf(char_buffer ,"%+4d", vaule);  putString(x0,y0,char_buffer);}
           else if(flag=="0")
           { sprintf(char_buffer ,"%04d", vaule);  putString(x0,y0,char_buffer);}
           break;
    case 5: 
           if(flag=="n")
           {sprintf(char_buffer ,"%5d", vaule); putString(x0,y0,char_buffer);}    
           else if(flag=="-")
           {sprintf(char_buffer ,"%-5d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="+")
           {sprintf(char_buffer ,"%+5d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="0")
           {sprintf(char_buffer ,"%05d", vaule); putString(x0,y0,char_buffer);}
           break;
    case 6: 
           if(flag=="n")
           {sprintf(char_buffer ,"%6d", vaule); putString(x0,y0,char_buffer);}    
           else if(flag=="-")
           {sprintf(char_buffer ,"%-6d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="+")
           {sprintf(char_buffer ,"%+6d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="0")
           {sprintf(char_buffer ,"%06d", vaule); putString(x0,y0,char_buffer);}
           break; 
    case 7: 
           if(flag=="n")
           {sprintf(char_buffer ,"%7d", vaule); putString(x0,y0,char_buffer);} 
           else if(flag=="-")
           {sprintf(char_buffer ,"%-7d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="+")
           {sprintf(char_buffer ,"%+7d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="0")
           {sprintf(char_buffer ,"%07d", vaule); putString(x0,y0,char_buffer);}
           break;
    case 8: 
           if(flag=="n")
           {sprintf(char_buffer ,"%8d", vaule); putString(x0,y0,char_buffer);}     
           else if(flag=="-")
           {sprintf(char_buffer ,"%-8d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="+")
           {sprintf(char_buffer ,"%+8d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="0")
           {sprintf(char_buffer ,"%08d", vaule); putString(x0,y0,char_buffer);}
           break;
    case 9:
           if(flag=="n")
           {sprintf(char_buffer ,"%9d", vaule); putString(x0,y0,char_buffer);}  
           else if(flag=="-")
           {sprintf(char_buffer ,"%-9d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="+")
           {sprintf(char_buffer ,"%+9d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="0")
           {sprintf(char_buffer ,"%09d", vaule); putString(x0,y0,char_buffer);}
           break;
    case 10:
           if(flag=="n")
           {sprintf(char_buffer ,"%10d", vaule); putString(x0,y0,char_buffer);}  
           else if(flag=="-")
           {sprintf(char_buffer ,"%-10d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="+")
           {sprintf(char_buffer ,"%+10d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="0")
           {sprintf(char_buffer ,"%010d", vaule); putString(x0,y0,char_buffer);}
           break;
    case 11: 
           if(flag=="n")
           {sprintf(char_buffer ,"%11d", vaule); putString(x0,y0,char_buffer);} 
           else if(flag=="-")
           {sprintf(char_buffer ,"%-11d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="+")
           {sprintf(char_buffer ,"%+11d", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="0")
           {sprintf(char_buffer ,"%011d", vaule); putString(x0,y0,char_buffer);}
           break;       
    default:   
           break;    
  }
}

//**************************************************************//
//vaule: (3.4E-38) ~ (3.4E38)
//len: 1~11 minimum output length
//precision: right side of point numbers 1~4 
/*
  [flag] 
    n:靠右　
    -:靠左
    +:輸出正負號
    (space):當不輸出正負號時，就輸出空白
    #: 強制輸出小數點
    0:在開頭處(左側) 補 0，而非補空白。
    
 */
 //arduino Floats have only 6-7 decimal digits of precision. That means the total number of digits, not the number to the right of the decimal point. 
 //Unlike other platforms, where you can get more precision by using a double (e.g. up to 15 digits), on the Arduino, double is the same size as float.
//**************************************************************//
void Ra8889_Lite:: putFloat(ru16 x0,ru16 y0,double vaule,ru8 len,ru8 precision, const char *flag)
{
  char char_buffer[20];
  switch(len)
  {
    case 1:
           if(flag=="n")
           {
             if(precision==1)
             sprintf(char_buffer ,"%1.1f", vaule); putString(x0,y0,char_buffer);
             if(precision==2)
             sprintf(char_buffer ,"%1.2f", vaule); putString(x0,y0,char_buffer);
             if(precision==3)
             sprintf(char_buffer ,"%1.3f", vaule); putString(x0,y0,char_buffer);
             if(precision==4)
             sprintf(char_buffer ,"%1.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="-")
           {
            if(precision==1)
            sprintf(char_buffer ,"%-1.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%-1.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%-1.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%-1.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="+")
           {
            if(precision==1)
            sprintf(char_buffer ,"%+1.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%+1.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%+1.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%+1.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="0")
           {
            if(precision==1)
            sprintf(char_buffer ,"%01.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%01.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%01.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%01.4f", vaule); putString(x0,y0,char_buffer);
           }
           break;
    case 2:
           if(flag=="n")
           {
           if(precision==1)  
           sprintf(char_buffer ,"%2.1f", vaule); putString(x0,y0,char_buffer);
           if(precision==2)  
           sprintf(char_buffer ,"%2.2f", vaule); putString(x0,y0,char_buffer);
           if(precision==3)  
           sprintf(char_buffer ,"%2.3f", vaule); putString(x0,y0,char_buffer);
           if(precision==4)  
           sprintf(char_buffer ,"%2.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="-")
           { 
           if(precision==1) 
           sprintf(char_buffer ,"%-2.1f", vaule); putString(x0,y0,char_buffer);
           if(precision==2) 
           sprintf(char_buffer ,"%-2.2f", vaule); putString(x0,y0,char_buffer);
           if(precision==3) 
           sprintf(char_buffer ,"%-2.3f", vaule); putString(x0,y0,char_buffer);
           if(precision==4) 
           sprintf(char_buffer ,"%-2.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="+")
           { 
           if(precision==1)
           sprintf(char_buffer ,"%+2.1f", vaule); putString(x0,y0,char_buffer);
           if(precision==2)
           sprintf(char_buffer ,"%+2.2f", vaule); putString(x0,y0,char_buffer);
           if(precision==3)
           sprintf(char_buffer ,"%+2.3f", vaule); putString(x0,y0,char_buffer);
           if(precision==4)
           sprintf(char_buffer ,"%+2.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="0")
           { 
           if(precision==1)
           sprintf(char_buffer ,"%02.1f", vaule); putString(x0,y0,char_buffer);
           if(precision==2)
           sprintf(char_buffer ,"%02.2f", vaule); putString(x0,y0,char_buffer);
           if(precision==3)
           sprintf(char_buffer ,"%02.3f", vaule); putString(x0,y0,char_buffer);
           if(precision==4)
           sprintf(char_buffer ,"%02.4f", vaule); putString(x0,y0,char_buffer);
           }
           break; 
    case 3: 
           if(flag=="n")
           {
            if(precision==1)
            sprintf(char_buffer ,"%3.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%3.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%3.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%3.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="-")
           {
           if(precision==1)
           sprintf(char_buffer ,"%-3.1f", vaule); putString(x0,y0,char_buffer);
           if(precision==2)
           sprintf(char_buffer ,"%-3.2f", vaule); putString(x0,y0,char_buffer);
           if(precision==3)
           sprintf(char_buffer ,"%-3.3f", vaule); putString(x0,y0,char_buffer);
           if(precision==4)
           sprintf(char_buffer ,"%-3.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="+")
           {
           if(precision==1)
           sprintf(char_buffer ,"%+3.1f", vaule); putString(x0,y0,char_buffer);
           if(precision==2)
           sprintf(char_buffer ,"%+3.2f", vaule); putString(x0,y0,char_buffer);
           if(precision==3)
           sprintf(char_buffer ,"%+3.3f", vaule); putString(x0,y0,char_buffer);
           if(precision==4)
           sprintf(char_buffer ,"%+3.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="0")
           {
           if(precision==1)
           sprintf(char_buffer ,"%03.1f", vaule); putString(x0,y0,char_buffer);
           if(precision==2)
           sprintf(char_buffer ,"%03.2f", vaule); putString(x0,y0,char_buffer);
           if(precision==3)
           sprintf(char_buffer ,"%03.3f", vaule); putString(x0,y0,char_buffer);
           if(precision==4)
           sprintf(char_buffer ,"%03.4f", vaule); putString(x0,y0,char_buffer);
           }
           break;
    case 4: 
           if(flag=="n")
           {
            if(precision==1)
            sprintf(char_buffer ,"%4.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%4.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%4.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%4.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="-")
           { 
            if(precision==1)
            sprintf(char_buffer ,"%-4.1f", vaule);  putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%-4.2f", vaule);  putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%-4.3f", vaule);  putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%-4.4f", vaule);  putString(x0,y0,char_buffer);
           }
           else if(flag=="+")
           {   
            if(precision==1)
            sprintf(char_buffer ,"%+4.1f", vaule);  putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%+4.2f", vaule);  putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%+4.3f", vaule);  putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%+4.4f", vaule);  putString(x0,y0,char_buffer);
           }
           else if(flag=="0")
           { 
            if(precision==1)
            sprintf(char_buffer ,"%04.1f", vaule);  putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%04.2f", vaule);  putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%04.3f", vaule);  putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%04.4f", vaule);  putString(x0,y0,char_buffer);
           }
           break;
    case 5: 
           if(flag=="n")
           {
            if(precision==1)
            sprintf(char_buffer ,"%5.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%5.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%5.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%5.4f", vaule); putString(x0,y0,char_buffer);
           }    
           else if(flag=="-")
           {
            if(precision==1)
            sprintf(char_buffer ,"%-5.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%-5.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%-5.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%-5.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="+")
           {
            if(precision==1)
            sprintf(char_buffer ,"%+5.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%+5.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%+5.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%+5.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="0")
           {
            if(precision==1)
            sprintf(char_buffer ,"%05.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%05.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%05.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%05.4f", vaule); putString(x0,y0,char_buffer);
           }
           break;
    case 6: 
           if(flag=="n")
           {
            if(precision==1)
            sprintf(char_buffer ,"%6.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%6.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%6.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%6.4f", vaule); putString(x0,y0,char_buffer);
           }    
           else if(flag=="-")
           {
            if(precision==1)
            sprintf(char_buffer ,"%-6.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%-6.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%-6.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%-6.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="+")
           {
            if(precision==1)
            sprintf(char_buffer ,"%+6.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%+6.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%+6.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%+6.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="0")
           {
            if(precision==1)
            sprintf(char_buffer ,"%06.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%06.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%06.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%06.4f", vaule); putString(x0,y0,char_buffer);
           }
           break; 
    case 7: 
           if(flag=="n")
           {
            if(precision==1)
            sprintf(char_buffer ,"%7.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%7.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%7.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%7.4f", vaule); putString(x0,y0,char_buffer);
           } 
           else if(flag=="-")
           {
            if(precision==1)
            sprintf(char_buffer ,"%-7.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%-7.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%-7.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%-7.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="+")
           {
            if(precision==1)
            sprintf(char_buffer ,"%+7.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%+7.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%+7.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%+7.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="0")
           {
            if(precision==1)
            sprintf(char_buffer ,"%07.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%07.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%07.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%07.4f", vaule); putString(x0,y0,char_buffer);
           }
           break;
    case 8: 
           if(flag=="n")
           {
            if(precision==1)
            sprintf(char_buffer ,"%8.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%8.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%8.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%8.4f", vaule); putString(x0,y0,char_buffer);
           }     
           else if(flag=="-")
           {
            if(precision==1)
            sprintf(char_buffer ,"%-8.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%-8.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%-8.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%-8.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="+")
           {
            if(precision==1)
            sprintf(char_buffer ,"%+8.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%+8.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%+8.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%+8.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="0")
           {
            if(precision==1)
            sprintf(char_buffer ,"%08.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%08.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%08.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%08.4f", vaule); putString(x0,y0,char_buffer);
           }
           break;
    case 9:
           if(flag=="n")
           {
            if(precision==1)
            sprintf(char_buffer ,"%9.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%9.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%9.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%9.4f", vaule); putString(x0,y0,char_buffer);
           }  
           else if(flag=="-")
           {
            if(precision==1)
            sprintf(char_buffer ,"%-9.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%-9.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%-9.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%-9.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="+")
           {
            if(precision==1)
            sprintf(char_buffer ,"%+9.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%+9.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%+9.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%+9.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="0")
           {
            if(precision==1)
            sprintf(char_buffer ,"%09.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%09.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%09.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%09.4f", vaule); putString(x0,y0,char_buffer);
           }
           break;
    case 10:
           if(flag=="n")
           {
            if(precision==1)
            sprintf(char_buffer ,"%10.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%10.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%10.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%10.4f", vaule); putString(x0,y0,char_buffer);
           }  
           else if(flag=="-")
           {
            if(precision==1)
            sprintf(char_buffer ,"%-10.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%-10.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%-10.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%-10.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="+")
           {
            if(precision==1)
            sprintf(char_buffer ,"%+10.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%+10.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%+10.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%+10.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="0")
           {
            if(precision==1)
            sprintf(char_buffer ,"%010.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%010.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%010.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%010.4f", vaule); putString(x0,y0,char_buffer);
           }
           break;
    case 11: 
           if(flag=="n")
           {
            if(precision==1)
            sprintf(char_buffer ,"%11.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%11.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%11.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%11.4f", vaule); putString(x0,y0,char_buffer);
           } 
           else if(flag=="-")
           {
            if(precision==1)
            sprintf(char_buffer ,"%-11.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%-11.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%-11.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%-11.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="+")
           {
            if(precision==1)
            sprintf(char_buffer ,"%+11.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%+11.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%+11.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%+11.4f", vaule); putString(x0,y0,char_buffer);
           }
           else if(flag=="0")
           {
            if(precision==1)
            sprintf(char_buffer ,"%011.1f", vaule); putString(x0,y0,char_buffer);
            if(precision==2)
            sprintf(char_buffer ,"%011.2f", vaule); putString(x0,y0,char_buffer);
            if(precision==3)
            sprintf(char_buffer ,"%011.3f", vaule); putString(x0,y0,char_buffer);
            if(precision==4)
            sprintf(char_buffer ,"%011.4f", vaule); putString(x0,y0,char_buffer);
           }
           break;       
    default:   
           break;    
  }
}
//**************************************************************//
//vaule: 0x00000000 ~ 0xffffffff
//len: 1~11 minimum output length
/*
  [flag] 
    n:靠右,補空白　
    #:強制輸出 0x 作為開頭.
    0:在開頭處(左側) 補 0，而非補空白。
    x:強制輸出 0x 作為開頭，補 0。 
 */
//**************************************************************//
void Ra8889_Lite:: putHex(ru16 x0,ru16 y0,ru32 vaule,ru8 len,const char *flag)
{
  char char_buffer[12];
  switch(len)
  {
    case 1:
           if(flag=="n")
           {sprintf(char_buffer ,"%1x", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="0")
           {sprintf(char_buffer ,"%01x", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="#")
           {sprintf(char_buffer ,"%#1x", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="x")
           {sprintf(char_buffer ,"%#01x", vaule); putString(x0,y0,char_buffer);}
           break;
    case 2:
           if(flag=="n")
           {sprintf(char_buffer ,"%2x", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="0")
           { sprintf(char_buffer ,"%02x", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="#")
           { sprintf(char_buffer ,"%#2x", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="x")
           { sprintf(char_buffer ,"%#02x", vaule); putString(x0,y0,char_buffer);}
           break; 
    case 3: 
           if(flag=="n")
           {sprintf(char_buffer ,"%3x", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="0")
           {sprintf(char_buffer ,"%03x", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="#")
           {sprintf(char_buffer ,"%#3x", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="x")
           {sprintf(char_buffer ,"%#03x", vaule); putString(x0,y0,char_buffer);}
           break;
    case 4: 
           if(flag=="n")
           {sprintf(char_buffer ,"%4x", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="0")
           { sprintf(char_buffer ,"%04x", vaule);  putString(x0,y0,char_buffer);}
           else if(flag=="#")
           { sprintf(char_buffer ,"%#4x", vaule);  putString(x0,y0,char_buffer);}
           else if(flag=="x")
           { sprintf(char_buffer ,"%#04x", vaule);  putString(x0,y0,char_buffer);}
           break;
    case 5: 
           if(flag=="n")
           {sprintf(char_buffer ,"%5x", vaule); putString(x0,y0,char_buffer);}    
           else if(flag=="0")
           {sprintf(char_buffer ,"%05x", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="#")
           {sprintf(char_buffer ,"%#5x", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="x")
           {sprintf(char_buffer ,"%#05x", vaule); putString(x0,y0,char_buffer);}
           break;
    case 6: 
           if(flag=="n")
           {sprintf(char_buffer ,"%6x", vaule); putString(x0,y0,char_buffer);}    
           else if(flag=="0")
           {sprintf(char_buffer ,"%06x", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="#")
           {sprintf(char_buffer ,"%#6x", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="x")
           {sprintf(char_buffer ,"%#06x", vaule); putString(x0,y0,char_buffer);}
           break; 
    case 7: 
           if(flag=="n")
           {sprintf(char_buffer ,"%7x", vaule); putString(x0,y0,char_buffer);} 
           else if(flag=="0")
           {sprintf(char_buffer ,"%07x", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="#")
           {sprintf(char_buffer ,"%#7x", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="x")
           {sprintf(char_buffer ,"%#07x", vaule); putString(x0,y0,char_buffer);}
           break;
    case 8: 
           if(flag=="n")
           {sprintf(char_buffer ,"%8x", vaule); putString(x0,y0,char_buffer);}     
           else if(flag=="0")
           {sprintf(char_buffer ,"%08x", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="#")
           {sprintf(char_buffer ,"%#8x", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="x")
           {sprintf(char_buffer ,"%#08x", vaule); putString(x0,y0,char_buffer);}
           break;
case 9: 
           if(flag=="n")
           {sprintf(char_buffer ,"%9x", vaule); putString(x0,y0,char_buffer);} 
           else if(flag=="0")
           {sprintf(char_buffer ,"%09x", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="#")
           {sprintf(char_buffer ,"%#9x", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="x")
           {sprintf(char_buffer ,"%#09x", vaule); putString(x0,y0,char_buffer);}
           break;
case 10: 
           if(flag=="n")
           {sprintf(char_buffer ,"%10x", vaule); putString(x0,y0,char_buffer);}     
           else if(flag=="0")
           {sprintf(char_buffer ,"%010x", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="#")
           {sprintf(char_buffer ,"%#10x", vaule); putString(x0,y0,char_buffer);}
           else if(flag=="x")
           {sprintf(char_buffer ,"%#010x", vaule); putString(x0,y0,char_buffer);}
           break;
      
    default:   
           break;    
  }
}
 /*draw function*/
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::drawLine(ru16 x0, ru16 y0, ru16 x1, ru16 y1, ru32 color)
{
  #ifdef COLOR_DEPTH_16BPP
  foreGroundColor16bpp(color);
  #endif
  #ifdef COLOR_DEPTH_24BPP
  foreGroundColor24bpp(color);
  #endif
  lcdRegDataWrite(RA8889_DLHSR0,x0);//68h
  lcdRegDataWrite(RA8889_DLHSR1,x0>>8);//69h
  lcdRegDataWrite(RA8889_DLVSR0,y0);//6ah
  lcdRegDataWrite(RA8889_DLVSR1,y0>>8);//6bh
  lcdRegDataWrite(RA8889_DLHER0,x1);//6ch
  lcdRegDataWrite(RA8889_DLHER1,x1>>8);//6dh
  lcdRegDataWrite(RA8889_DLVER0,y1);//6eh
  lcdRegDataWrite(RA8889_DLVER1,y1>>8);//6fh        
  lcdRegDataWrite(RA8889_DCR0,RA8889_DRAW_LINE);//67h,0x80
  check2dBusy();
}
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::drawSquare(ru16 x0, ru16 y0, ru16 x1, ru16 y1, ru32 color)
{
  #ifdef COLOR_DEPTH_16BPP
  foreGroundColor16bpp(color);
  #endif
  #ifdef COLOR_DEPTH_24BPP
  foreGroundColor24bpp(color);
  #endif
  lcdRegDataWrite(RA8889_DLHSR0,x0);//68h
  lcdRegDataWrite(RA8889_DLHSR1,x0>>8);//69h
  lcdRegDataWrite(RA8889_DLVSR0,y0);//6ah
  lcdRegDataWrite(RA8889_DLVSR1,y0>>8);//6bh
  lcdRegDataWrite(RA8889_DLHER0,x1);//6ch
  lcdRegDataWrite(RA8889_DLHER1,x1>>8);//6dh
  lcdRegDataWrite(RA8889_DLVER0,y1);//6eh
  lcdRegDataWrite(RA8889_DLVER1,y1>>8);//6fh        
  lcdRegDataWrite(RA8889_DCR1,RA8889_DRAW_SQUARE);//89h,0xa0
  check2dBusy();
}
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::drawSquareFill(ru16 x0, ru16 y0, ru16 x1, ru16 y1, ru32 color)
{
  #ifdef COLOR_DEPTH_16BPP
  foreGroundColor16bpp(color);
  #endif
  #ifdef COLOR_DEPTH_24BPP
  foreGroundColor24bpp(color);
  #endif
  lcdRegDataWrite(RA8889_DLHSR0,x0);//68h
  lcdRegDataWrite(RA8889_DLHSR1,x0>>8);//69h
  lcdRegDataWrite(RA8889_DLVSR0,y0);//6ah
  lcdRegDataWrite(RA8889_DLVSR1,y0>>8);//6bh
  lcdRegDataWrite(RA8889_DLHER0,x1);//6ch
  lcdRegDataWrite(RA8889_DLHER1,x1>>8);//6dh
  lcdRegDataWrite(RA8889_DLVER0,y1);//6eh
  lcdRegDataWrite(RA8889_DLVER1,y1>>8);//6fh        
  lcdRegDataWrite(RA8889_DCR1,RA8889_DRAW_SQUARE_FILL);//89h,0xa0
  check2dBusy();
}
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::drawCircleSquare(ru16 x0, ru16 y0, ru16 x1, ru16 y1, ru16 xr, ru16 yr, ru32 color)
{
  #ifdef COLOR_DEPTH_16BPP
  foreGroundColor16bpp(color);
  #endif
  #ifdef COLOR_DEPTH_24BPP
  foreGroundColor24bpp(color);
  #endif
  lcdRegDataWrite(RA8889_DLHSR0,x0);//68h
  lcdRegDataWrite(RA8889_DLHSR1,x0>>8);//69h
  lcdRegDataWrite(RA8889_DLVSR0,y0);//6ah
  lcdRegDataWrite(RA8889_DLVSR1,y0>>8);//6bh
  lcdRegDataWrite(RA8889_DLHER0,x1);//6ch
  lcdRegDataWrite(RA8889_DLHER1,x1>>8);//6dh
  lcdRegDataWrite(RA8889_DLVER0,y1);//6eh
  lcdRegDataWrite(RA8889_DLVER1,y1>>8);//6fh    
  lcdRegDataWrite(RA8889_ELL_A0,xr);//77h    
  lcdRegDataWrite(RA8889_ELL_A1,xr>>8);//79h 
  lcdRegDataWrite(RA8889_ELL_B0,yr);//7ah    
  lcdRegDataWrite(RA8889_ELL_B1,yr>>8);//7bh
  lcdRegDataWrite(RA8889_DCR1,RA8889_DRAW_CIRCLE_SQUARE);//89h,0xb0
  check2dBusy();
}
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::drawCircleSquareFill(ru16 x0, ru16 y0, ru16 x1, ru16 y1, ru16 xr, ru16 yr, ru32 color)
{
  #ifdef COLOR_DEPTH_16BPP
  foreGroundColor16bpp(color);
  #endif
  #ifdef COLOR_DEPTH_24BPP
  foreGroundColor24bpp(color);
  #endif
  lcdRegDataWrite(RA8889_DLHSR0,x0);//68h
  lcdRegDataWrite(RA8889_DLHSR1,x0>>8);//69h
  lcdRegDataWrite(RA8889_DLVSR0,y0);//6ah
  lcdRegDataWrite(RA8889_DLVSR1,y0>>8);//6bh
  lcdRegDataWrite(RA8889_DLHER0,x1);//6ch
  lcdRegDataWrite(RA8889_DLHER1,x1>>8);//6dh
  lcdRegDataWrite(RA8889_DLVER0,y1);//6eh
  lcdRegDataWrite(RA8889_DLVER1,y1>>8);//6fh    
  lcdRegDataWrite(RA8889_ELL_A0,xr);//77h    
  lcdRegDataWrite(RA8889_ELL_A1,xr>>8);//78h 
  lcdRegDataWrite(RA8889_ELL_B0,yr);//79h    
  lcdRegDataWrite(RA8889_ELL_B1,yr>>8);//7ah
  lcdRegDataWrite(RA8889_DCR1,RA8889_DRAW_CIRCLE_SQUARE_FILL);//89h,0xf0
  check2dBusy();
}
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::drawTriangle(ru16 x0,ru16 y0,ru16 x1,ru16 y1,ru16 x2,ru16 y2,ru32 color)
{
  #ifdef COLOR_DEPTH_16BPP
  foreGroundColor16bpp(color);
  #endif
  #ifdef COLOR_DEPTH_24BPP
  foreGroundColor24bpp(color);
  #endif
  lcdRegDataWrite(RA8889_DLHSR0,x0);//68h
  lcdRegDataWrite(RA8889_DLHSR1,x0>>8);//69h
  lcdRegDataWrite(RA8889_DLVSR0,y0);//6ah
  lcdRegDataWrite(RA8889_DLVSR1,y0>>8);//6bh
  lcdRegDataWrite(RA8889_DLHER0,x1);//6ch
  lcdRegDataWrite(RA8889_DLHER1,x1>>8);//6dh
  lcdRegDataWrite(RA8889_DLVER0,y1);//6eh
  lcdRegDataWrite(RA8889_DLVER1,y1>>8);//6fh  
  lcdRegDataWrite(RA8889_DTPH0,x2);//70h
  lcdRegDataWrite(RA8889_DTPH1,x2>>8);//71h
  lcdRegDataWrite(RA8889_DTPV0,y2);//72h
  lcdRegDataWrite(RA8889_DTPV1,y2>>8);//73h  
  lcdRegDataWrite(RA8889_DCR0,RA8889_DRAW_TRIANGLE);//67h,0x82
  check2dBusy();
}
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::drawTriangleFill(ru16 x0,ru16 y0,ru16 x1,ru16 y1,ru16 x2,ru16 y2,ru32 color)
{
  #ifdef COLOR_DEPTH_16BPP
  foreGroundColor16bpp(color);
  #endif
  #ifdef COLOR_DEPTH_24BPP
  foreGroundColor24bpp(color);
  #endif
  lcdRegDataWrite(RA8889_DLHSR0,x0);//68h
  lcdRegDataWrite(RA8889_DLHSR1,x0>>8);//69h
  lcdRegDataWrite(RA8889_DLVSR0,y0);//6ah
  lcdRegDataWrite(RA8889_DLVSR1,y0>>8);//6bh
  lcdRegDataWrite(RA8889_DLHER0,x1);//6ch
  lcdRegDataWrite(RA8889_DLHER1,x1>>8);//6dh
  lcdRegDataWrite(RA8889_DLVER0,y1);//6eh
  lcdRegDataWrite(RA8889_DLVER1,y1>>8);//6fh  
  lcdRegDataWrite(RA8889_DTPH0,x2);//70h
  lcdRegDataWrite(RA8889_DTPH1,x2>>8);//71h
  lcdRegDataWrite(RA8889_DTPV0,y2);//72h
  lcdRegDataWrite(RA8889_DTPV1,y2>>8);//73h  
  lcdRegDataWrite(RA8889_DCR0,RA8889_DRAW_TRIANGLE_FILL);//67h,0xa2
  check2dBusy();
}
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::drawCircle(ru16 x0,ru16 y0,ru16 r,ru32 color)
{
  #ifdef COLOR_DEPTH_16BPP
  foreGroundColor16bpp(color);
  #endif
  #ifdef COLOR_DEPTH_24BPP
  foreGroundColor24bpp(color);
  #endif
  lcdRegDataWrite(RA8889_DEHR0,x0);//7bh
  lcdRegDataWrite(RA8889_DEHR1,x0>>8);//7ch
  lcdRegDataWrite(RA8889_DEVR0,y0);//7dh
  lcdRegDataWrite(RA8889_DEVR1,y0>>8);//7eh
  lcdRegDataWrite(RA8889_ELL_A0,r);//77h    
  lcdRegDataWrite(RA8889_ELL_A1,r>>8);//78h 
  lcdRegDataWrite(RA8889_ELL_B0,r);//79h    
  lcdRegDataWrite(RA8889_ELL_B1,r>>8);//7ah
  lcdRegDataWrite(RA8889_DCR1,RA8889_DRAW_CIRCLE);//89h,0x80
  check2dBusy();
}
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::drawCircleFill(ru16 x0,ru16 y0,ru16 r,ru32 color)
{
  #ifdef COLOR_DEPTH_16BPP
  foreGroundColor16bpp(color);
  #endif
  #ifdef COLOR_DEPTH_24BPP
  foreGroundColor24bpp(color);
  #endif
  lcdRegDataWrite(RA8889_DEHR0,x0);//7bh
  lcdRegDataWrite(RA8889_DEHR1,x0>>8);//7ch
  lcdRegDataWrite(RA8889_DEVR0,y0);//7dh
  lcdRegDataWrite(RA8889_DEVR1,y0>>8);//7eh
  lcdRegDataWrite(RA8889_ELL_A0,r);//77h    
  lcdRegDataWrite(RA8889_ELL_A1,r>>8);//78h 
  lcdRegDataWrite(RA8889_ELL_B0,r);//79h    
  lcdRegDataWrite(RA8889_ELL_B1,r>>8);//7ah
  lcdRegDataWrite(RA8889_DCR1,RA8889_DRAW_CIRCLE_FILL);//89h,0xc0
  check2dBusy();
}
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::drawEllipse(ru16 x0,ru16 y0,ru16 xr,ru16 yr,ru32 color)
{
  #ifdef COLOR_DEPTH_16BPP
  foreGroundColor16bpp(color);
  #endif
  #ifdef COLOR_DEPTH_24BPP
  foreGroundColor24bpp(color);
  #endif
  lcdRegDataWrite(RA8889_DEHR0,x0);//7bh
  lcdRegDataWrite(RA8889_DEHR1,x0>>8);//7ch
  lcdRegDataWrite(RA8889_DEVR0,y0);//7dh
  lcdRegDataWrite(RA8889_DEVR1,y0>>8);//7eh
  lcdRegDataWrite(RA8889_ELL_A0,xr);//77h    
  lcdRegDataWrite(RA8889_ELL_A1,xr>>8);//78h 
  lcdRegDataWrite(RA8889_ELL_B0,yr);//79h    
  lcdRegDataWrite(RA8889_ELL_B1,yr>>8);//7ah
  lcdRegDataWrite(RA8889_DCR1,RA8889_DRAW_ELLIPSE);//89h,0x80
  check2dBusy();
}
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::drawEllipseFill(ru16 x0,ru16 y0,ru16 xr,ru16 yr,ru32 color)
{
  #ifdef COLOR_DEPTH_16BPP
  foreGroundColor16bpp(color);
  #endif
  #ifdef COLOR_DEPTH_24BPP
  foreGroundColor24bpp(color);
  #endif
  lcdRegDataWrite(RA8889_DEHR0,x0);//7bh
  lcdRegDataWrite(RA8889_DEHR1,x0>>8);//7ch
  lcdRegDataWrite(RA8889_DEVR0,y0);//7dh
  lcdRegDataWrite(RA8889_DEVR1,y0>>8);//7eh
  lcdRegDataWrite(RA8889_ELL_A0,xr);//77h    
  lcdRegDataWrite(RA8889_ELL_A1,xr>>8);//78h 
  lcdRegDataWrite(RA8889_ELL_B0,yr);//79h    
  lcdRegDataWrite(RA8889_ELL_B1,yr>>8);//7ah
  lcdRegDataWrite(RA8889_DCR1,RA8889_DRAW_ELLIPSE_FILL);//89h,0xc0
  check2dBusy();
}

/*BTE function*/
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::bteMemoryCopy(ru32 s0_addr,ru16 s0_image_width,ru16 s0_x,ru16 s0_y,ru32 des_addr,ru16 des_image_width, 
                                ru16 des_x,ru16 des_y,ru16 copy_width,ru16 copy_height)
{
  bte_Source0_MemoryStartAddr(s0_addr);
  bte_Source0_ImageWidth(s0_image_width);
  bte_Source0_WindowStartXY(s0_x,s0_y);
  //bte_Source1_MemoryStartAddr(des_addr);
  //bte_Source1_ImageWidth(des_image_width);
  //bte_Source1_WindowStartXY(des_x,des_y);
  bte_DestinationMemoryStartAddr(des_addr);
  bte_DestinationImageWidth(des_image_width);
  bte_DestinationWindowStartXY(des_x,des_y);
  
  bte_WindowSize(copy_width,copy_height); 
  lcdRegDataWrite(RA8889_BTE_CTRL1,RA8889_BTE_ROP_CODE_12<<4|RA8889_BTE_MEMORY_COPY_WITH_ROP);//91h
  lcdRegDataWrite(RA8889_BTE_CTRL0,RA8889_BTE_ENABLE<<4);//90h
  check2dBusy();
} 
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::bteMemoryCopyWithROP(ru32 s0_addr,ru16 s0_image_width,ru16 s0_x,ru16 s0_y,ru32 s1_addr,ru16 s1_image_width,ru16 s1_x,ru16 s1_y,
                                       ru32 des_addr,ru16 des_image_width, ru16 des_x,ru16 des_y,ru16 copy_width,ru16 copy_height,ru8 rop_code)
{
  bte_Source0_MemoryStartAddr(s0_addr);
  bte_Source0_ImageWidth(s0_image_width);
  bte_Source0_WindowStartXY(s0_x,s0_y);
  bte_Source1_MemoryStartAddr(s1_addr);
  bte_Source1_ImageWidth(s1_image_width);
  bte_Source1_WindowStartXY(s1_x,s1_y);
  bte_DestinationMemoryStartAddr(des_addr);
  bte_DestinationImageWidth(des_image_width);
  bte_DestinationWindowStartXY(des_x,des_y);
  bte_WindowSize(copy_width,copy_height);
  lcdRegDataWrite(RA8889_BTE_CTRL1,rop_code<<4|RA8889_BTE_MEMORY_COPY_WITH_ROP);//91h
  lcdRegDataWrite(RA8889_BTE_CTRL0,RA8889_BTE_ENABLE<<4);//90h
  check2dBusy();
} 
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::bteMemoryCopyWithChromaKey(ru32 s0_addr,ru16 s0_image_width,ru16 s0_x,ru16 s0_y,
ru32 des_addr,ru16 des_image_width, ru16 des_x,ru16 des_y,ru16 copy_width,ru16 copy_height, ru32 chromakey_color)
{
  bte_Source0_MemoryStartAddr(s0_addr);
  bte_Source0_ImageWidth(s0_image_width);
  bte_Source0_WindowStartXY(s0_x,s0_y);
  bte_DestinationMemoryStartAddr(des_addr);
  bte_DestinationImageWidth(des_image_width);
  bte_DestinationWindowStartXY(des_x,des_y);
  bte_WindowSize(copy_width,copy_height);
  
  #ifdef COLOR_DEPTH_16BPP
  backGroundColor16bpp(chromakey_color);
  #endif
  #ifdef COLOR_DEPTH_24BPP
  backGroundColor24bpp(chromakey_color);
  #endif
  
  lcdRegDataWrite(RA8889_BTE_CTRL1,RA8889_BTE_MEMORY_COPY_WITH_CHROMA);//91h
  lcdRegDataWrite(RA8889_BTE_CTRL0,RA8889_BTE_ENABLE<<4);//90h
  check2dBusy();
}



//**************************************************************//
//**************************************************************//
void Ra8889_Lite::bteMpuWriteWithROP(ru32 s1_addr,ru16 s1_image_width,ru16 s1_x,ru16 s1_y,ru32 des_addr,ru16 des_image_width,
ru16 des_x,ru16 des_y,ru16 width,ru16 height,ru8 rop_code,const unsigned char *data)
{
   ru16 i,j;
  bte_Source1_MemoryStartAddr(s1_addr);
  bte_Source1_ImageWidth(s1_image_width);
  bte_Source1_WindowStartXY(s1_x,s1_y);
  bte_DestinationMemoryStartAddr(des_addr);
  bte_DestinationImageWidth(des_image_width);
  bte_DestinationWindowStartXY(des_x,des_y);
  bte_WindowSize(width,height);
  lcdRegDataWrite(RA8889_BTE_CTRL1,rop_code<<4|RA8889_BTE_MPU_WRITE_WITH_ROP);//91h
  lcdRegDataWrite(RA8889_BTE_CTRL0,RA8889_BTE_ENABLE<<4);//90h
  ramAccessPrepare();

  #ifdef COLOR_DEPTH_16BPP 
  for(i=0;i< height;i++)
  {	
   for(j=0;j< (width*2);j++)
   {
    checkWriteFifoNotFull();
    lcdDataWrite(*data);
    data++;
    }
   }

  #endif

  #ifdef COLOR_DEPTH_24BPP 
  for(i=0;i< height;i++)
  {  
   for(j=0;j< (width);j++)
   {
    //checkWriteFifoNotFull();
    lcdDataWrite(*data);
    data++;
    //checkWriteFifoNotFull();
    lcdDataWrite(*data);
    data++;
    //checkWriteFifoNotFull();
    lcdDataWrite(*data);
    data++;
    }
   }
  #endif 
   
  checkWriteFifoEmpty();
}

//**************************************************************//
//**************************************************************//
void Ra8889_Lite::bteMpuWriteWithROP(ru32 s1_addr,ru16 s1_image_width,ru16 s1_x,ru16 s1_y,ru32 des_addr,ru16 des_image_width,
ru16 des_x,ru16 des_y,ru16 width,ru16 height,ru8 rop_code,const unsigned short *data)
{
   ru16 i,j;
  bte_Source1_MemoryStartAddr(s1_addr);
  bte_Source1_ImageWidth(s1_image_width);
  bte_Source1_WindowStartXY(s1_x,s1_y);
  bte_DestinationMemoryStartAddr(des_addr);
  bte_DestinationImageWidth(des_image_width);
  bte_DestinationWindowStartXY(des_x,des_y);
  bte_WindowSize(width,height);
  lcdRegDataWrite(RA8889_BTE_CTRL1,rop_code<<4|RA8889_BTE_MPU_WRITE_WITH_ROP);//91h
  lcdRegDataWrite(RA8889_BTE_CTRL0,RA8889_BTE_ENABLE<<4);//90h
  ramAccessPrepare();
  
 for(j=0;j<height;j++)
 {
  for(i=0;i<width;i++)
  {
   checkWriteFifoNotFull();//if high speed mcu and without Xnwait check
   lcdDataWrite16bbp(*data);
   data++;
   //checkWriteFifoEmpty();//if high speed mcu and without Xnwait check
  }
 } 
  checkWriteFifoEmpty();
}
//**************************************************************//
//write data after setting
//**************************************************************//
void Ra8889_Lite::bteMpuWriteWithROP(ru32 s1_addr,ru16 s1_image_width,ru16 s1_x,ru16 s1_y,ru32 des_addr,ru16 des_image_width,
ru16 des_x,ru16 des_y,ru16 width,ru16 height,ru8 rop_code)
{
   ru16 i,j;
  bte_Source1_MemoryStartAddr(s1_addr);
  bte_Source1_ImageWidth(s1_image_width);
  bte_Source1_WindowStartXY(s1_x,s1_y);
  bte_DestinationMemoryStartAddr(des_addr);
  bte_DestinationImageWidth(des_image_width);
  bte_DestinationWindowStartXY(des_x,des_y);
  bte_WindowSize(width,height);
  lcdRegDataWrite(RA8889_BTE_CTRL1,rop_code<<4|RA8889_BTE_MPU_WRITE_WITH_ROP);//91h
  lcdRegDataWrite(RA8889_BTE_CTRL0,RA8889_BTE_ENABLE<<4);//90h
  ramAccessPrepare();
}
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::bteMpuWriteWithChromaKey(ru32 des_addr,ru16 des_image_width, ru16 des_x,ru16 des_y,ru16 width,ru16 height,ru32 chromakey_color,const unsigned char *data)
{
  ru16 i,j;
  bte_DestinationMemoryStartAddr(des_addr);
  bte_DestinationImageWidth(des_image_width);
  bte_DestinationWindowStartXY(des_x,des_y);
  bte_WindowSize(width,height);

  #ifdef COLOR_DEPTH_16BPP 
  backGroundColor16bpp(chromakey_color);
  #endif

  #ifdef COLOR_DEPTH_24BPP 
  backGroundColor24bpp(chromakey_color);
  #endif
  
  lcdRegDataWrite(RA8889_BTE_CTRL1,RA8889_BTE_MPU_WRITE_WITH_CHROMA);//91h
  lcdRegDataWrite(RA8889_BTE_CTRL0,RA8889_BTE_ENABLE<<4);//90h
  ramAccessPrepare();
  
  #ifdef COLOR_DEPTH_16BPP 
  for(i=0;i< height;i++)
  {  
   for(j=0;j< (width*2);j++)
   {
    checkWriteFifoNotFull();
    lcdDataWrite(*data);
    data++;
    }
   }

  #endif

  #ifdef COLOR_DEPTH_24BPP 
  for(i=0;i< height;i++)
  {  
   for(j=0;j< (width);j++)
   {
    //checkWriteFifoNotFull();
    lcdDataWrite(*data);
    data++;
    //checkWriteFifoNotFull();
    lcdDataWrite(*data);
    data++;
    //checkWriteFifoNotFull();
    lcdDataWrite(*data);
    data++;
    }
   }
  #endif 
  
  checkWriteFifoEmpty();
}

//**************************************************************//
//**************************************************************//
void Ra8889_Lite::bteMpuWriteWithChromaKey(ru32 des_addr,ru16 des_image_width, ru16 des_x,ru16 des_y,ru16 width,ru16 height,ru32 chromakey_color,const unsigned short *data)
{
  ru16 i,j;
  bte_DestinationMemoryStartAddr(des_addr);
  bte_DestinationImageWidth(des_image_width);
  bte_DestinationWindowStartXY(des_x,des_y);
  bte_WindowSize(width,height);

  #ifdef COLOR_DEPTH_16BPP 
  backGroundColor16bpp(chromakey_color);
  #endif

  #ifdef COLOR_DEPTH_24BPP 
  backGroundColor24bpp(chromakey_color);
  #endif
  
  lcdRegDataWrite(RA8889_BTE_CTRL1,RA8889_BTE_MPU_WRITE_WITH_CHROMA);//91h
  lcdRegDataWrite(RA8889_BTE_CTRL0,RA8889_BTE_ENABLE<<4);//90h
  ramAccessPrepare();
  
 for(j=0;j<height;j++)
 {
  for(i=0;i<width;i++)
  {
   checkWriteFifoNotFull();//if high speed mcu and without Xnwait check
   lcdDataWrite16bbp(*data);
   data++;
   //checkWriteFifoEmpty();//if high speed mcu and without Xnwait check
  }
 } 
  checkWriteFifoEmpty();
}
//**************************************************************//
//write data after setting
//**************************************************************//
void Ra8889_Lite::bteMpuWriteWithChromaKey(ru32 des_addr,ru16 des_image_width, ru16 des_x,ru16 des_y,ru16 width,ru16 height,ru32 chromakey_color)
{
  ru16 i,j;
  bte_DestinationMemoryStartAddr(des_addr);
  bte_DestinationImageWidth(des_image_width);
  bte_DestinationWindowStartXY(des_x,des_y);
  bte_WindowSize(width,height);
  
  #ifdef COLOR_DEPTH_16BPP 
  backGroundColor16bpp(chromakey_color);
  #endif

  #ifdef COLOR_DEPTH_24BPP 
  backGroundColor24bpp(chromakey_color);
  #endif
  
  lcdRegDataWrite(RA8889_BTE_CTRL1,RA8889_BTE_MPU_WRITE_WITH_CHROMA);//91h
  lcdRegDataWrite(RA8889_BTE_CTRL0,RA8889_BTE_ENABLE<<4);//90h
  ramAccessPrepare();
}
//**************************************************************//
//**************************************************************//
void Ra8889_Lite::bteMpuWriteColorExpansion(ru32 des_addr,ru16 des_image_width, ru16 des_x,ru16 des_y,ru16 width,ru16 height,ru32 foreground_color,ru32 background_color,const unsigned char *data)
{
  ru16 i,j;
  bte_DestinationMemoryStartAddr(des_addr);
  bte_DestinationImageWidth(des_image_width);
  bte_DestinationWindowStartXY(des_x,des_y);
  bte_WindowSize(width,height);
  
  #ifdef COLOR_DEPTH_16BPP 
  foreGroundColor16bpp(foreground_color);
  backGroundColor16bpp(background_color);
  #endif

  #ifdef COLOR_DEPTH_24BPP 
  foreGroundColor24bpp(foreground_color);
  backGroundColor24bpp(background_color);
  #endif
  
  lcdRegDataWrite(RA8889_BTE_CTRL1,RA8889_BTE_ROP_BUS_WIDTH8<<4|RA8889_BTE_MPU_WRITE_COLOR_EXPANSION);//91h
  lcdRegDataWrite(RA8889_BTE_CTRL0,RA8889_BTE_ENABLE<<4);//90h
  ramAccessPrepare();
  
  for(i=0;i< height;i++)
  {	
   for(j=0;j< (width/8);j++)
   {
    checkWriteFifoNotFull();
    lcdDataWrite(*data);
    data++;
    }
   }
  checkWriteFifoEmpty();
  check2dBusy();
}
//**************************************************************//
//write data after setting
//**************************************************************//
void Ra8889_Lite::bteMpuWriteColorExpansion(ru32 des_addr,ru16 des_image_width, ru16 des_x,ru16 des_y,ru16 width,ru16 height,ru32 foreground_color,ru32 background_color)
{
  ru16 i,j;
  bte_DestinationMemoryStartAddr(des_addr);
  bte_DestinationImageWidth(des_image_width);
  bte_DestinationWindowStartXY(des_x,des_y);
  bte_WindowSize(width,height); 

  #ifdef COLOR_DEPTH_16BPP 
  foreGroundColor16bpp(foreground_color);
  backGroundColor16bpp(background_color);
  #endif

  #ifdef COLOR_DEPTH_24BPP 
  foreGroundColor24bpp(foreground_color);
  backGroundColor24bpp(background_color);
  #endif
  
  lcdRegDataWrite(RA8889_BTE_CTRL1,RA8889_BTE_ROP_BUS_WIDTH8<<4|RA8889_BTE_MPU_WRITE_COLOR_EXPANSION);//91h
  lcdRegDataWrite(RA8889_BTE_CTRL0,RA8889_BTE_ENABLE<<4);//90h
  ramAccessPrepare();
}
//**************************************************************//
/*background_color do not set the same as foreground_color*/
//**************************************************************//
void Ra8889_Lite::bteMpuWriteColorExpansionWithChromaKey(ru32 des_addr,ru16 des_image_width, ru16 des_x,ru16 des_y,ru16 width,ru16 height,ru32 foreground_color,ru32 background_color, const unsigned char *data)
{
  ru16 i,j;
  bte_DestinationMemoryStartAddr(des_addr);
  bte_DestinationImageWidth(des_image_width);
  bte_DestinationWindowStartXY(des_x,des_y);
  bte_WindowSize(width,height);

  #ifdef COLOR_DEPTH_16BPP 
  foreGroundColor16bpp(foreground_color);
  backGroundColor16bpp(background_color);
  #endif

  #ifdef COLOR_DEPTH_24BPP 
  foreGroundColor24bpp(foreground_color);
  backGroundColor24bpp(background_color);
  #endif
  
  lcdRegDataWrite(RA8889_BTE_CTRL1,RA8889_BTE_ROP_BUS_WIDTH8<<4|RA8889_BTE_MPU_WRITE_COLOR_EXPANSION_WITH_CHROMA);//91h
  lcdRegDataWrite(RA8889_BTE_CTRL0,RA8889_BTE_ENABLE<<4);//90h
  ramAccessPrepare();
  
  for(i=0;i< height;i++)
  {	
   for(j=0;j< (width/8);j++)
   {
    checkWriteFifoNotFull();
    lcdDataWrite(*data);
    data++;
    }
   }
  checkWriteFifoEmpty();
  check2dBusy();
}
//**************************************************************//
/*background_color do not set the same as foreground_color*/
//write data after setting
//**************************************************************//
void Ra8889_Lite::bteMpuWriteColorExpansionWithChromaKey(ru32 des_addr,ru16 des_image_width, ru16 des_x,ru16 des_y,ru16 width,ru16 height,ru32 foreground_color,ru32 background_color)
{
  ru16 i,j;
  bte_DestinationMemoryStartAddr(des_addr);
  bte_DestinationImageWidth(des_image_width);
  bte_DestinationWindowStartXY(des_x,des_y);
  bte_WindowSize(width,height);
  #ifdef COLOR_DEPTH_16BPP 
  foreGroundColor16bpp(foreground_color);
  backGroundColor16bpp(background_color);
  #endif

  #ifdef COLOR_DEPTH_24BPP 
  foreGroundColor24bpp(foreground_color);
  backGroundColor24bpp(background_color);
  #endif
  lcdRegDataWrite(RA8889_BTE_CTRL1,RA8889_BTE_ROP_BUS_WIDTH8<<4|RA8889_BTE_MPU_WRITE_COLOR_EXPANSION_WITH_CHROMA);//91h
  lcdRegDataWrite(RA8889_BTE_CTRL0,RA8889_BTE_ENABLE<<4);//90h
  ramAccessPrepare();
}


//**************************************************************//
//**************************************************************//
void Ra8889_Lite::bteMemoryCopyWith_ARGB8888(ru32 s1_addr,ru16 s1_image_width,
ru32 des_addr,ru16 des_image_width, ru16 des_x,ru16 des_y,ru16 copy_width,ru16 copy_height)
{

  lcdRegDataWrite(RA8889_BTE_CTRL1,RA8889_BTE_MEMORY_COPY_WITH_OPACITY);//91h
  lcdRegDataWrite(RA8889_BTE_COLR,RA8889_S0_COLOR_DEPTH_24BPP<<5|RA8889_S1_32BIT_ARGB_ALPHA_BLENDING<<2|RA8889_DESTINATION_COLOR_DEPTH_24BPP);//92h
  
  bte_Source1_MemoryStartAddr(s1_addr);
  bte_Source1_ImageWidth(s1_image_width);
  bte_Source1_WindowStartXY(0,0);

  bte_Source0_MemoryStartAddr(des_addr);
  bte_Source0_ImageWidth(des_image_width);
  bte_Source0_WindowStartXY(des_x,des_y);
   
  bte_DestinationMemoryStartAddr(des_addr);
  bte_DestinationImageWidth(des_image_width);
  bte_DestinationWindowStartXY(des_x,des_y);
  
  bte_WindowSize(copy_width,copy_height);


  lcdRegDataWrite(RA8889_BTE_CTRL0,RA8889_BTE_ENABLE<<4);//90h
  check2dBusy();

  lcdRegDataWrite(RA8889_BTE_COLR,RA8889_S0_COLOR_DEPTH_24BPP<<5|RA8889_S1_COLOR_DEPTH_24BPP<<2|RA8889_DESTINATION_COLOR_DEPTH_24BPP);//92h
}

 /*DMA Function*/
 //**************************************************************//
/* 
bus_select = 0 : select bus0, 
bus_select = 1 : select bus1,

scs_select = 0 : select SFI0, scs_select = 1 : select SFI1 for bus0 
scs_select = 2 : select SFI2, scs_select = 3 : select SFI3 for bus1
*/
//**************************************************************//
 void Ra8889_Lite:: dma_24bitAddressBlockMode(ru8 bus_select,ru8 scs_select,ru8 clk_div,ru16 x0,ru16 y0,ru16 width,ru16 height,ru16 picture_width,ru32 addr)
 {
    if(bus_select==0)
   {fontDmaSelect_bus0();}
   else if(bus_select==1)
   {fontDmaSelect_bus1();}
   
    if(scs_select==0)//
     { fontDmaSelectSFI_0();  
      lcdRegDataWrite(RA8889_SFL_CTRL,RA8889_SERIAL_FLASH_SELECT0<<7|RA8889_SERIAL_FLASH_DMA_MODE<<6|RA8889_SERIAL_FLASH_ADDR_24BIT<<5|RA8889_FOLLOW_RA8875_MODE<<4|RA8889_SPI_FAST_READ_8DUMMY);
     }//b7h
    else if(scs_select==1)//
     {fontDmaSelectSFI_1(); 
      lcdRegDataWrite(RA8889_SFL_CTRL,RA8889_SERIAL_FLASH_SELECT1<<7|RA8889_SERIAL_FLASH_DMA_MODE<<6|RA8889_SERIAL_FLASH_ADDR_24BIT<<5|RA8889_FOLLOW_RA8875_MODE<<4|RA8889_SPI_FAST_READ_8DUMMY);
     }//b7h  
    else if(scs_select==2)//
      {fontDmaSelectSFI_2();
      lcdRegDataWrite(RA8889_SFL_CTRL,RA8889_SERIAL_FLASH_SELECT2<<7|RA8889_SERIAL_FLASH_DMA_MODE<<6|RA8889_SERIAL_FLASH_ADDR_24BIT<<5|RA8889_FOLLOW_RA8875_MODE<<4|RA8889_SPI_FAST_READ_8DUMMY);
      }//b7h
     else if(scs_select==3)//
      {fontDmaSelectSFI_3();
       lcdRegDataWrite(RA8889_SFL_CTRL,RA8889_SERIAL_FLASH_SELECT3<<7|RA8889_SERIAL_FLASH_DMA_MODE<<6|RA8889_SERIAL_FLASH_ADDR_24BIT<<5|RA8889_FOLLOW_RA8875_MODE<<4|RA8889_SPI_FAST_READ_8DUMMY);
      }//b7h 
   
    
  lcdRegDataWrite(RA8889_SPI_DIVSOR,clk_div);//bbh  
  
  lcdRegDataWrite(RA8889_DMA_DX0,x0);//c0h
  lcdRegDataWrite(RA8889_DMA_DX1,x0>>8);//c1h
  lcdRegDataWrite(RA8889_DMA_DY0,y0);//c2h
  lcdRegDataWrite(RA8889_DMA_DY1,y0>>8);//c3h 
  lcdRegDataWrite(RA8889_DMAW_WTH0,width);//c6h
  lcdRegDataWrite(RA8889_DMAW_WTH1,width>>8);//c7h
  lcdRegDataWrite(RA8889_DMAW_HIGH0,height);//c8h
  lcdRegDataWrite(RA8889_DMAW_HIGH1,height>>8);//c9h 
  lcdRegDataWrite(RA8889_DMA_SWTH0,picture_width);//cah
  lcdRegDataWrite(RA8889_DMA_SWTH1,picture_width>>8);//cbh 
  lcdRegDataWrite(RA8889_DMA_SSTR0,addr);//bch
  lcdRegDataWrite(RA8889_DMA_SSTR1,addr>>8);//bdh
  lcdRegDataWrite(RA8889_DMA_SSTR2,addr>>16);//beh
  lcdRegDataWrite(RA8889_DMA_SSTR3,addr>>24);//bfh 
  
  lcdRegDataWrite(RA8889_DMA_CTRL,RA8889_DMA_START);//b6h 
  check2dBusy(); 
 }
 //**************************************************************//
/* 
bus_select = 0 : select bus0, 
bus_select = 1 : select bus1,

scs_select = 0 : select SFI0, scs_select = 1 : select SFI1   //for bus0 
scs_select = 2 : select SFI2, scs_select = 3 : select SFI3   // for bus1
*/
//**************************************************************//
 void Ra8889_Lite:: dma_32bitAddressBlockMode(ru8 bus_select,ru8 scs_select,ru8 clk_div,ru16 x0,ru16 y0,ru16 width,ru16 height,ru16 picture_width,ru32 addr)
 {
    if(bus_select==0)
   {fontDmaSelect_bus0();}
   else if(bus_select==1)
   {fontDmaSelect_bus1();}
   
    if(scs_select==0)//
     { fontDmaSelectSFI_0();  
      lcdRegDataWrite(RA8889_SFL_CTRL,RA8889_SERIAL_FLASH_SELECT0<<7|RA8889_SERIAL_FLASH_DMA_MODE<<6|RA8889_SERIAL_FLASH_ADDR_32BIT<<5|RA8889_FOLLOW_RA8875_MODE<<4|RA8889_SPI_FAST_READ_8DUMMY);
     }//b7h
    else if(scs_select==1)//
    {fontDmaSelectSFI_1(); 
     lcdRegDataWrite(RA8889_SFL_CTRL,RA8889_SERIAL_FLASH_SELECT1<<7|RA8889_SERIAL_FLASH_DMA_MODE<<6|RA8889_SERIAL_FLASH_ADDR_32BIT<<5|RA8889_FOLLOW_RA8875_MODE<<4|RA8889_SPI_FAST_READ_8DUMMY);
    }//b7h  
    else if(scs_select==2)//
    {fontDmaSelectSFI_2();
     lcdRegDataWrite(RA8889_SFL_CTRL,RA8889_SERIAL_FLASH_SELECT2<<7|RA8889_SERIAL_FLASH_DMA_MODE<<6|RA8889_SERIAL_FLASH_ADDR_32BIT<<5|RA8889_FOLLOW_RA8875_MODE<<4|RA8889_SPI_FAST_READ_8DUMMY);
    }//b7h
    else if(scs_select==3)//
    {fontDmaSelectSFI_3();
     lcdRegDataWrite(RA8889_SFL_CTRL,RA8889_SERIAL_FLASH_SELECT3<<7|RA8889_SERIAL_FLASH_DMA_MODE<<6|RA8889_SERIAL_FLASH_ADDR_32BIT<<5|RA8889_FOLLOW_RA8875_MODE<<4|RA8889_SPI_FAST_READ_8DUMMY);
    }//b7h 
   
  
  lcdRegDataWrite(RA8889_SPI_DIVSOR,clk_div);//bbh 
  
  lcdRegDataWrite(RA8889_DMA_DX0,x0);//c0h
  lcdRegDataWrite(RA8889_DMA_DX1,x0>>8);//c1h
  lcdRegDataWrite(RA8889_DMA_DY0,y0);//c2h
  lcdRegDataWrite(RA8889_DMA_DY1,y0>>8);//c3h 
  lcdRegDataWrite(RA8889_DMAW_WTH0,width);//c6h
  lcdRegDataWrite(RA8889_DMAW_WTH1,width>>8);//c7h
  lcdRegDataWrite(RA8889_DMAW_HIGH0,height);//c8h
  lcdRegDataWrite(RA8889_DMAW_HIGH1,height>>8);//c9h 
  lcdRegDataWrite(RA8889_DMA_SWTH0,picture_width);//cah
  lcdRegDataWrite(RA8889_DMA_SWTH1,picture_width>>8);//cbh 
  lcdRegDataWrite(RA8889_DMA_SSTR0,addr);//bch
  lcdRegDataWrite(RA8889_DMA_SSTR1,addr>>8);//bdh
  lcdRegDataWrite(RA8889_DMA_SSTR2,addr>>16);//beh
  lcdRegDataWrite(RA8889_DMA_SSTR3,addr>>24);//bfh  
  
  lcdRegDataWrite(RA8889_DMA_CTRL,RA8889_DMA_START);//b6h 
  check2dBusy(); 
 }
 

//**************************************************************//
/* 
bus_select = 0 : select bus0, 
bus_select = 1 : select bus1,

scs_select = 0 : select SFI0, scs_select = 1 : select SFI1 for bus0 
scs_select = 2 : select SFI2, scs_select = 3 : select SFI3 for bus1
*/
//**************************************************************//
 void Ra8889_Lite:: dma_24bitAddressLinearMode(ru8 bus_select,ru8 scs_select,ru8 clk_div,ru32 source_addr,ru32 des_address,ru32 number)
 {

   //switch canvas to 8bpp format and canvas linear mode
   lcdRegWrite(RA8889_AW_COLOR);//5Eh
   lcdDataWrite(RA8889_CANVAS_LINEAR_MODE<<2|RA8889_CANVAS_COLOR_DEPTH_8BPP);

   
    if(bus_select==0)
   {fontDmaSelect_bus0();}
   else if(bus_select==1)
   {fontDmaSelect_bus1();}
   
    if(scs_select==0)//
     { fontDmaSelectSFI_0();  
      lcdRegDataWrite(RA8889_SFL_CTRL,RA8889_SERIAL_FLASH_SELECT0<<7|RA8889_SERIAL_FLASH_DMA_MODE<<6|RA8889_SERIAL_FLASH_ADDR_24BIT<<5|RA8889_FOLLOW_RA8875_MODE<<4|RA8889_SPI_FAST_READ_8DUMMY);
     }//b7h
    else if(scs_select==1)//
     {fontDmaSelectSFI_1(); 
      lcdRegDataWrite(RA8889_SFL_CTRL,RA8889_SERIAL_FLASH_SELECT1<<7|RA8889_SERIAL_FLASH_DMA_MODE<<6|RA8889_SERIAL_FLASH_ADDR_24BIT<<5|RA8889_FOLLOW_RA8875_MODE<<4|RA8889_SPI_FAST_READ_8DUMMY);
     }//b7h  
    else if(scs_select==2)//
      {fontDmaSelectSFI_2();
      lcdRegDataWrite(RA8889_SFL_CTRL,RA8889_SERIAL_FLASH_SELECT2<<7|RA8889_SERIAL_FLASH_DMA_MODE<<6|RA8889_SERIAL_FLASH_ADDR_24BIT<<5|RA8889_FOLLOW_RA8875_MODE<<4|RA8889_SPI_FAST_READ_8DUMMY);
      }//b7h
     else if(scs_select==3)//
      {fontDmaSelectSFI_3();
       lcdRegDataWrite(RA8889_SFL_CTRL,RA8889_SERIAL_FLASH_SELECT3<<7|RA8889_SERIAL_FLASH_DMA_MODE<<6|RA8889_SERIAL_FLASH_ADDR_24BIT<<5|RA8889_FOLLOW_RA8875_MODE<<4|RA8889_SPI_FAST_READ_8DUMMY);
      }//b7h 
   
    
  lcdRegDataWrite(RA8889_SPI_DIVSOR,clk_div);//bbh  
  
  lcdRegDataWrite(RA8889_DMA_DX0,des_address);//c0h
  lcdRegDataWrite(RA8889_DMA_DX1,des_address>>8);//c1h
  lcdRegDataWrite(RA8889_DMA_DY0,des_address>>16);//c2h
  lcdRegDataWrite(RA8889_DMA_DY1,des_address>>24);//c3h 

  lcdRegDataWrite(RA8889_DMAW_WTH0,number);//c6
  lcdRegDataWrite(RA8889_DMAW_WTH1,number>>8);//c7
  lcdRegDataWrite(RA8889_DMAW_HIGH0,number>>16);//c8
  lcdRegDataWrite(RA8889_DMAW_HIGH1,number>>24);//c9

  lcdRegDataWrite(RA8889_DMA_SSTR0,source_addr);//bch
  lcdRegDataWrite(RA8889_DMA_SSTR1,source_addr>>8);//bdh
  lcdRegDataWrite(RA8889_DMA_SSTR2,source_addr>>16);//beh
  lcdRegDataWrite(RA8889_DMA_SSTR3,source_addr>>24);//bfh 
  
  lcdRegDataWrite(RA8889_DMA_CTRL,RA8889_DMA_START);//b6h 
  check2dBusy(); 


   #ifdef COLOR_DEPTH_16BPP
   lcdRegWrite(RA8889_AW_COLOR);//5Eh
   lcdDataWrite(RA8889_CANVAS_BLOCK_MODE<<2|RA8889_CANVAS_COLOR_DEPTH_16BPP);
   lcdRegDataWrite(RA8889_BTE_COLR,RA8889_S0_COLOR_DEPTH_16BPP<<5|RA8889_S1_COLOR_DEPTH_16BPP<<2|RA8889_S0_COLOR_DEPTH_16BPP);//92h
   #endif
   
   #ifdef COLOR_DEPTH_24BPP
   lcdRegWrite(RA8889_AW_COLOR);//5Eh
   lcdDataWrite(RA8889_CANVAS_BLOCK_MODE<<2|RA8889_CANVAS_COLOR_DEPTH_24BPP);
   lcdRegDataWrite(RA8889_BTE_COLR,RA8889_S0_COLOR_DEPTH_24BPP<<5|RA8889_S1_COLOR_DEPTH_24BPP<<2|RA8889_S0_COLOR_DEPTH_24BPP);//92h
   #endif
   
 }
 //**************************************************************//
/* 
bus_select = 0 : select bus0, 
bus_select = 1 : select bus1,

scs_select = 0 : select SFI0, scs_select = 1 : select SFI1   //for bus0 
scs_select = 2 : select SFI2, scs_select = 3 : select SFI3   // for bus1
*/
//**************************************************************//
 void Ra8889_Lite:: dma_32bitAddressLinearMode(ru8 bus_select,ru8 scs_select,ru8 clk_div,ru32 source_addr,ru32 des_address,ru32 number)
 {

   //switch canvas to 8bpp format and canvas linear mode
   lcdRegWrite(RA8889_AW_COLOR);//5Eh
   lcdDataWrite(RA8889_CANVAS_LINEAR_MODE<<2|RA8889_CANVAS_COLOR_DEPTH_8BPP);
   
    if(bus_select==0)
   {fontDmaSelect_bus0();}
   else if(bus_select==1)
   {fontDmaSelect_bus1();}
   
    if(scs_select==0)//
     { fontDmaSelectSFI_0();  
      lcdRegDataWrite(RA8889_SFL_CTRL,RA8889_SERIAL_FLASH_SELECT0<<7|RA8889_SERIAL_FLASH_DMA_MODE<<6|RA8889_SERIAL_FLASH_ADDR_32BIT<<5|RA8889_FOLLOW_RA8875_MODE<<4|RA8889_SPI_FAST_READ_8DUMMY);
     }//b7h
    else if(scs_select==1)//
    {fontDmaSelectSFI_1(); 
     lcdRegDataWrite(RA8889_SFL_CTRL,RA8889_SERIAL_FLASH_SELECT1<<7|RA8889_SERIAL_FLASH_DMA_MODE<<6|RA8889_SERIAL_FLASH_ADDR_32BIT<<5|RA8889_FOLLOW_RA8875_MODE<<4|RA8889_SPI_FAST_READ_8DUMMY);
    }//b7h  
    else if(scs_select==2)//
    {fontDmaSelectSFI_2();
     lcdRegDataWrite(RA8889_SFL_CTRL,RA8889_SERIAL_FLASH_SELECT2<<7|RA8889_SERIAL_FLASH_DMA_MODE<<6|RA8889_SERIAL_FLASH_ADDR_32BIT<<5|RA8889_FOLLOW_RA8875_MODE<<4|RA8889_SPI_FAST_READ_8DUMMY);
    }//b7h
    else if(scs_select==3)//
    {fontDmaSelectSFI_3();
     lcdRegDataWrite(RA8889_SFL_CTRL,RA8889_SERIAL_FLASH_SELECT3<<7|RA8889_SERIAL_FLASH_DMA_MODE<<6|RA8889_SERIAL_FLASH_ADDR_32BIT<<5|RA8889_FOLLOW_RA8875_MODE<<4|RA8889_SPI_FAST_READ_8DUMMY);
    }//b7h 
   
  
  lcdRegDataWrite(RA8889_SPI_DIVSOR,clk_div);//bbh 
  
  lcdRegDataWrite(RA8889_DMA_DX0,des_address);//c0h
  lcdRegDataWrite(RA8889_DMA_DX1,des_address>>8);//c1h
  lcdRegDataWrite(RA8889_DMA_DY0,des_address>>16);//c2h
  lcdRegDataWrite(RA8889_DMA_DY1,des_address>>24);//c3h 

  lcdRegDataWrite(RA8889_DMAW_WTH0,number);//c6
  lcdRegDataWrite(RA8889_DMAW_WTH1,number>>8);//c7
  lcdRegDataWrite(RA8889_DMAW_HIGH0,number>>16);//c8
  lcdRegDataWrite(RA8889_DMAW_HIGH1,number>>24);//c9

  lcdRegDataWrite(RA8889_DMA_SSTR0,source_addr);//bch
  lcdRegDataWrite(RA8889_DMA_SSTR1,source_addr>>8);//bdh
  lcdRegDataWrite(RA8889_DMA_SSTR2,source_addr>>16);//beh
  lcdRegDataWrite(RA8889_DMA_SSTR3,source_addr>>24);//bfh 
  
  lcdRegDataWrite(RA8889_DMA_CTRL,RA8889_DMA_START);//b6h 
  check2dBusy(); 


   #ifdef COLOR_DEPTH_16BPP
   lcdRegWrite(RA8889_AW_COLOR);//5Eh
   lcdDataWrite(RA8889_CANVAS_BLOCK_MODE<<2|RA8889_CANVAS_COLOR_DEPTH_16BPP);
   lcdRegDataWrite(RA8889_BTE_COLR,RA8889_S0_COLOR_DEPTH_16BPP<<5|RA8889_S1_COLOR_DEPTH_16BPP<<2|RA8889_S0_COLOR_DEPTH_16BPP);//92h
   #endif
   
   #ifdef COLOR_DEPTH_24BPP
   lcdRegWrite(RA8889_AW_COLOR);//5Eh
   lcdDataWrite(RA8889_CANVAS_BLOCK_MODE<<2|RA8889_CANVAS_COLOR_DEPTH_24BPP);
   lcdRegDataWrite(RA8889_BTE_COLR,RA8889_S0_COLOR_DEPTH_24BPP<<5|RA8889_S1_COLOR_DEPTH_24BPP<<2|RA8889_S0_COLOR_DEPTH_24BPP);//92h
   #endif
   
 }

/*IDEC function*/

 #ifdef COLOR_DEPTH_16BPP
//**************************************************************//
/* 
bus_select = 0 : select bus0, 
bus_select = 1 : select bus1,

scs_select = 0 : select SFI0, scs_select = 1 : select SFI1   //for bus0 
scs_select = 2 : select SFI2, scs_select = 3 : select SFI3   //for bus1
*/
//**************************************************************//
 void Ra8889_Lite:: idec_24bitAddressQuadMode6B_16bpp_JPEG(ru8 bus_select,ru8 scs_select,ru16 x0,ru16 y0,ru32 addr,ru32 number,ru16 des_image_width,ru32 des_start_addr)
 {
   idecClockDivide(1);
   idecDestinationColor16bpp();
   
   if(bus_select==0)
    idecSelectBus0();
   else if(bus_select ==1)
    idecSelectBus1(); 
   
   if(scs_select==0)//
     idecSelectSfi_0();    
   else if(scs_select==1)// 
     idecSelectSfi_1();  
   else if(scs_select==2)//
     idecSelectSfi_2();    
   else if(scs_select==3)// 
     idecSelectSfi_3();    
       
    registerPageSwitch(1);
    lcdRegDataWrite(RA8889_IDEC_CTRL,RA8889_IDEC_SFI_24BIT<<5|RA8889_IDEC_FOLLOW_RA8875_MODE<<4|RA8889_IDEC_SPI_QUAD_MODE_6BH);//B7h
    registerPageSwitch(0);
    
    idecSourceStartAddress(addr);
    idecDestinationUpperLeftCorner(x0,y0);	
    idecTransferNumber(number);
    idecDestinationImageWidth(des_image_width); 
    idecDestinationStartAddress(des_start_addr);
    idecStartsDecoding();
    checkIdecBusy();
    checkMediaDecodeBusy();
 }


 //**************************************************************//
/* 
bus_select = 0 : select bus0, 
bus_select = 1 : select bus1,

scs_select = 0 : select SFI0, scs_select = 1 : select SFI1   //for bus0 
scs_select = 2 : select SFI2, scs_select = 3 : select SFI3   //for bus1
*/
//**************************************************************//
 void Ra8889_Lite:: idec_32bitAddressQuadMode6B_16bpp_JPEG(ru8 bus_select,ru8 scs_select,ru16 x0,ru16 y0,ru32 addr,ru32 number,ru16 des_image_width,ru32 des_start_addr)
 {
   idecClockDivide(1);
   idecDestinationColor16bpp();
   
   if(bus_select==0)
    idecSelectBus0();
   else if(bus_select ==1)
    idecSelectBus1(); 
   
   if(scs_select==0)//
     idecSelectSfi_0();    
   else if(scs_select==1)// 
     idecSelectSfi_1();  
   else if(scs_select==2)//
     idecSelectSfi_2();    
   else if(scs_select==3)// 
     idecSelectSfi_3();  
     
    registerPageSwitch(1);
    lcdRegDataWrite(RA8889_IDEC_CTRL,RA8889_IDEC_SFI_32BIT<<5|RA8889_IDEC_FOLLOW_RA8875_MODE<<4|RA8889_IDEC_SPI_QUAD_MODE_6BH);//B7h
    registerPageSwitch(0);
    
    idecSourceStartAddress(addr);
    idecDestinationUpperLeftCorner(x0,y0);	
    idecTransferNumber(number);
    idecDestinationImageWidth(des_image_width); 
    idecDestinationStartAddress(des_start_addr);
    
    idecStartsDecoding();
    checkIdecBusy();
    checkMediaDecodeBusy();
 }
 
 
 
 //**************************************************************//
/* 
bus_select = 0 : select bus0, 
bus_select = 1 : select bus1,

scs_select = 0 : select SFI0, scs_select = 1 : select SFI1   //for bus0 
scs_select = 2 : select SFI2, scs_select = 3 : select SFI3   //for bus1
*/
//**************************************************************//
 void Ra8889_Lite:: idec_24bitAddressQuadMode6B_16bpp_AVI(ru8 bus_select,ru8 scs_select,ru16 x0,ru16 y0,ru32 addr,ru32 number,ru16 width,ru16 height,ru32 shadow_buffer_addr,ru32 pip_image_addr,ru16 pip_image_width)
 {
   idecClockDivide(1);
   idecDestinationColor16bpp();
   idecAviDisplayBufferUsePipAndShadowPip(0);
    aviShadowPipStartAddress(shadow_buffer_addr);
   
   if(bus_select==0)
    idecSelectBus0();
   else if(bus_select ==1)
    idecSelectBus1(); 
   
   if(scs_select==0)//
     idecSelectSfi_0();    
   else if(scs_select==1)// 
     idecSelectSfi_1();  
   else if(scs_select==2)//
     idecSelectSfi_2();    
   else if(scs_select==3)// 
     idecSelectSfi_3();    
       
    registerPageSwitch(1);
    lcdRegDataWrite(RA8889_IDEC_CTRL,RA8889_IDEC_SFI_24BIT<<5|RA8889_IDEC_FOLLOW_RA8875_MODE<<4|RA8889_IDEC_SPI_QUAD_MODE_6BH);//B7h
    registerPageSwitch(0);
    
    idecSourceStartAddress(addr);
    idecTransferNumber(number);
    

    pip1_ParameterSelect();
    pip1_SelectWindow16bpp();
    pipImageStartAddress(pip_image_addr);
    pipWindowDisplay_XY(x0,y0);

    pipImageWidth(pip_image_width);
    pipWindowImageStartXY(0,0);
    pipWindowWidthHeight(width,height);
    //pip1_Enable();
    delay(20);
    idecStartsDecoding();
    //checkIdecBusy();
    //checkMediaDecodeBusy();
 }
 
 
  //**************************************************************//
/* 
bus_select = 0 : select bus0, 
bus_select = 1 : select bus1,

scs_select = 0 : select SFI0, scs_select = 1 : select SFI1   //for bus0 
scs_select = 2 : select SFI2, scs_select = 3 : select SFI3   //for bus1
*/
//**************************************************************//
 void Ra8889_Lite:: idec_32bitAddressQuadMode6B_16bpp_AVI(ru8 bus_select,ru8 scs_select,ru16 x0,ru16 y0,ru32 addr,ru32 number,ru16 width,ru16 height,ru32 shadow_buffer_addr,ru32 pip_image_addr,ru16 pip_image_width)
 {
   idecClockDivide(1);
   idecDestinationColor16bpp();
   idecAviDisplayBufferUsePipAndShadowPip(0);
    aviShadowPipStartAddress(shadow_buffer_addr);
   
   if(bus_select==0)
    idecSelectBus0();
   else if(bus_select ==1)
    idecSelectBus1(); 
   
   if(scs_select==0)//
     idecSelectSfi_0();    
   else if(scs_select==1)// 
     idecSelectSfi_1();  
   else if(scs_select==2)//
     idecSelectSfi_2();    
   else if(scs_select==3)// 
     idecSelectSfi_3();    
       
    registerPageSwitch(1);
    lcdRegDataWrite(RA8889_IDEC_CTRL,RA8889_IDEC_SFI_32BIT<<5|RA8889_IDEC_FOLLOW_RA8875_MODE<<4|RA8889_IDEC_SPI_QUAD_MODE_6BH);//B7h
    registerPageSwitch(0);
    
    idecSourceStartAddress(addr);
    idecTransferNumber(number);
    

    pip1_ParameterSelect();
    pip1_SelectWindow16bpp();
    pipImageStartAddress(pip_image_addr);
    pipWindowDisplay_XY(x0,y0);

    pipImageWidth(pip_image_width);
    pipWindowImageStartXY(0,0);
    pipWindowWidthHeight(width,height);
    //pip1_Enable();
    delay(20);
    idecStartsDecoding();
    //checkIdecBusy();
    //checkMediaDecodeBusy();
 }
 
#endif
 
 
#ifdef COLOR_DEPTH_24BPP
 //**************************************************************//
/* 
bus_select = 0 : select bus0, 
bus_select = 1 : select bus1,

scs_select = 0 : select SFI0, scs_select = 1 : select SFI1   //for bus0 
scs_select = 2 : select SFI2, scs_select = 3 : select SFI3   //for bus1
*/
//**************************************************************//
 void Ra8889_Lite:: idec_24bitAddressQuadMode6B_24bpp_JPEG(ru8 bus_select,ru8 scs_select,ru16 x0,ru16 y0,ru32 addr,ru32 number,ru16 des_image_width,ru32 des_start_addr)
 {
   idecClockDivide(2);
   idecDestinationColor24bpp();
   
   if(bus_select==0)
    idecSelectBus0();
   else if(bus_select ==1)
    idecSelectBus1(); 
   
   if(scs_select==0)//
     idecSelectSfi_0();    
   else if(scs_select==1)// 
     idecSelectSfi_1();  
   else if(scs_select==2)//
     idecSelectSfi_2();    
   else if(scs_select==3)// 
     idecSelectSfi_3();    
       
    registerPageSwitch(1);
    lcdRegDataWrite(RA8889_IDEC_CTRL,RA8889_IDEC_SFI_24BIT<<5|RA8889_IDEC_FOLLOW_RA8875_MODE<<4|RA8889_IDEC_SPI_QUAD_MODE_6BH);//B7h
    registerPageSwitch(0);
    
    idecSourceStartAddress(addr);
    idecDestinationUpperLeftCorner(x0,y0);	
    idecTransferNumber(number);
    idecDestinationImageWidth(des_image_width); 
    idecDestinationStartAddress(des_start_addr);
    idecStartsDecoding();     
    checkIdecBusy();  
    checkMediaDecodeBusy();
 }


 //**************************************************************//
/* 
bus_select = 0 : select bus0, 
bus_select = 1 : select bus1,

scs_select = 0 : select SFI0, scs_select = 1 : select SFI1   //for bus0 
scs_select = 2 : select SFI2, scs_select = 3 : select SFI3   //for bus1
*/
//**************************************************************//
 void Ra8889_Lite:: idec_32bitAddressQuadMode6B_24bpp_JPEG(ru8 bus_select,ru8 scs_select,ru16 x0,ru16 y0,ru32 addr,ru32 number,ru16 des_image_width,ru32 des_start_addr)
 {
   idecClockDivide(1);
   idecDestinationColor24bpp();
   
   if(bus_select==0)
    idecSelectBus0();
   else if(bus_select ==1)
    idecSelectBus1(); 
   
   if(scs_select==0)//
     idecSelectSfi_0();    
   else if(scs_select==1)// 
     idecSelectSfi_1();  
   else if(scs_select==2)//
     idecSelectSfi_2();    
   else if(scs_select==3)// 
     idecSelectSfi_3();  
     
    registerPageSwitch(1);
    lcdRegDataWrite(RA8889_IDEC_CTRL,RA8889_IDEC_SFI_32BIT<<5|RA8889_IDEC_FOLLOW_RA8875_MODE<<4|RA8889_IDEC_SPI_QUAD_MODE_6BH);//B7h
    registerPageSwitch(0);
    
    idecSourceStartAddress(addr);
    idecDestinationUpperLeftCorner(x0,y0);	
    idecTransferNumber(number);
    idecDestinationImageWidth(des_image_width); 
    idecDestinationStartAddress(des_start_addr);
    
    idecStartsDecoding();
    checkIdecBusy();
    checkMediaDecodeBusy();
 }
 
 
 
 //**************************************************************//
/* 
bus_select = 0 : select bus0, 
bus_select = 1 : select bus1,

scs_select = 0 : select SFI0, scs_select = 1 : select SFI1   //for bus0 
scs_select = 2 : select SFI2, scs_select = 3 : select SFI3   //for bus1
*/
//**************************************************************//
 void Ra8889_Lite:: idec_24bitAddressQuadMode6B_24bpp_AVI(ru8 bus_select,ru8 scs_select,ru16 x0,ru16 y0,ru32 addr,ru32 number,ru16 width,ru16 height,ru32 shadow_buffer_addr,ru32 pip_image_addr,ru16 pip_image_width)
 {
   idecClockDivide(1);
   idecDestinationColor24bpp();
   idecAviDisplayBufferUsePipAndShadowPip(0);
    aviShadowPipStartAddress(shadow_buffer_addr);
   
   if(bus_select==0)
    idecSelectBus0();
   else if(bus_select ==1)
    idecSelectBus1(); 
   
   if(scs_select==0)//
     idecSelectSfi_0();    
   else if(scs_select==1)// 
     idecSelectSfi_1();  
   else if(scs_select==2)//
     idecSelectSfi_2();    
   else if(scs_select==3)// 
     idecSelectSfi_3();    
       
    registerPageSwitch(1);
    lcdRegDataWrite(RA8889_IDEC_CTRL,RA8889_IDEC_SFI_24BIT<<5|RA8889_IDEC_FOLLOW_RA8875_MODE<<4|RA8889_IDEC_SPI_QUAD_MODE_6BH);//B7h
    registerPageSwitch(0);
    
    idecSourceStartAddress(addr);
    idecTransferNumber(number);
    

    pip1_ParameterSelect();
    pip1_SelectWindow24bpp();
    pipImageStartAddress(pip_image_addr);
    pipWindowDisplay_XY(x0,y0);

    pipImageWidth(pip_image_width);
    pipWindowImageStartXY(0,0);
    pipWindowWidthHeight(width,height);
    //pip1_Enable();
    delay(20);
    idecStartsDecoding();
    //checkIdecBusy();
    //checkMediaDecodeBusy();
 }
 
 
  //**************************************************************//
/* 
bus_select = 0 : select bus0, 
bus_select = 1 : select bus1,

scs_select = 0 : select SFI0, scs_select = 1 : select SFI1   //for bus0 
scs_select = 2 : select SFI2, scs_select = 3 : select SFI3   //for bus1
*/
//**************************************************************//
 void Ra8889_Lite:: idec_32bitAddressQuadMode6B_24bpp_AVI(ru8 bus_select,ru8 scs_select,ru16 x0,ru16 y0,ru32 addr,ru32 number,ru16 width,ru16 height,ru32 shadow_buffer_addr,ru32 pip_image_addr,ru16 pip_image_width)
 {
   idecClockDivide(1);
   idecDestinationColor24bpp();
   idecAviDisplayBufferUsePipAndShadowPip(0);
    aviShadowPipStartAddress(shadow_buffer_addr);
   
   if(bus_select==0)
    idecSelectBus0();
   else if(bus_select ==1)
    idecSelectBus1(); 
   
   if(scs_select==0)//
     idecSelectSfi_0();    
   else if(scs_select==1)// 
     idecSelectSfi_1();  
   else if(scs_select==2)//
     idecSelectSfi_2();    
   else if(scs_select==3)// 
     idecSelectSfi_3();    
       
    registerPageSwitch(1);
    lcdRegDataWrite(RA8889_IDEC_CTRL,RA8889_IDEC_SFI_32BIT<<5|RA8889_IDEC_FOLLOW_RA8875_MODE<<4|RA8889_IDEC_SPI_QUAD_MODE_6BH);//B7h
    registerPageSwitch(0);
    
    idecSourceStartAddress(addr);
    idecTransferNumber(number);
    

    pip1_ParameterSelect();
    pip1_SelectWindow24bpp();
    pipImageStartAddress(pip_image_addr);
    pipWindowDisplay_XY(x0,y0);

    pipImageWidth(pip_image_width);
    pipWindowImageStartXY(0,0);
    pipWindowWidthHeight(width,height);
    //pip1_Enable();
    delay(20);
    idecStartsDecoding();
    //checkIdecBusy();
    //checkMediaDecodeBusy();
 }
 #endif




 
 //**************************************************************//
 /*  page = 0 or 1                                               */
 //**************************************************************//
void Ra8889_Lite::registerPageSwitch(ru8 page)
{
  if(page == 0)
  lcdRegDataWrite(RA8889_CMC,RA8889_REGISTER_PAGE0);//46h
  else if(page == 1)
  lcdRegDataWrite(RA8889_CMC,RA8889_REGISTER_PAGE1);//46h
}

//**************************************************************//	
 //**************************************************************//
 ru8 Ra8889_Lite:: readIdCode(void)
 { ru8 temp;
    //read ID code must disable pll, 01h bit7 set 0
  lcdRegDataWrite(0x01,0x08);
  delay(1);
  registerPageSwitch(1);
  temp = lcdRegDataRead(0xff);
  registerPageSwitch(0);
  return temp;
 }
 
 
 //**************************************************************//	
 //**************************************************************//
 void Ra8889_Lite::fontDmaSelect_bus0(void)	
{ ru8 temp;

  registerPageSwitch(1);
  lcdRegWrite(0xB6);
  temp = lcdDataRead();
  temp &= 0xEE;
  lcdDataWrite(temp);
  registerPageSwitch(0);
}
 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::fontDmaSelect_bus1(void)	
{ ru8 temp;

  registerPageSwitch(1);
  lcdRegWrite(0xB6);
  temp = lcdDataRead();
  temp &= 0xEE;
  temp |= 0x10;
  lcdDataWrite(temp);
  registerPageSwitch(0);
}

 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::fontDmaSelectSFI_0(void)
{ ru8 temp;

  registerPageSwitch(1);
  lcdRegWrite(0xB7);
  temp=lcdDataRead();
  temp &= cClrb7;
  lcdDataWrite(temp);
  registerPageSwitch(0);
  lcdRegWrite(0xB7);
  temp = lcdDataRead();
  temp &= cClrb7;
  lcdDataWrite(temp);
}
 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::fontDmaSelectSFI_1(void)
{ ru8 temp;
	
  registerPageSwitch(1);
  lcdRegWrite(0xB7);
  temp=lcdDataRead();
  temp &= cClrb7;
  lcdDataWrite(temp);
  registerPageSwitch(0);

  lcdRegWrite(0xB7);
  temp = lcdDataRead();
  temp |= cSetb7;
  lcdDataWrite(temp);
}
 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::fontDmaSelectSFI_2(void)
{ ru8 temp;

  registerPageSwitch(1);
  lcdRegWrite(0xB7);
  temp=lcdDataRead();
  temp |= cSetb7;
  lcdDataWrite(temp);
  registerPageSwitch(0);

  lcdRegWrite(0xB7);
  temp = lcdDataRead();
  temp &= cClrb7;
  lcdDataWrite(temp);
}
 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::fontDmaSelectSFI_3(void)
{ ru8 temp;

  registerPageSwitch(1);
  lcdRegWrite(0xB7);
  temp=lcdDataRead();
  temp |= cSetb7;
  lcdDataWrite(temp);
  registerPageSwitch(0);

  lcdRegWrite(0xB7);
  temp = lcdDataRead();
  temp |= cSetb7;
  lcdDataWrite(temp);
} 




 //**************************************************************//
 /* return 1 : means unsupported image format or
    header format error*/	
 //**************************************************************//
boolean Ra8889_Lite::getMediaFileErrorFlag(void)
{ ru8 temp;

  registerPageSwitch(1);
  lcdRegWrite(0xA0);
  temp = lcdDataRead()&0x80;
  registerPageSwitch(0);
  
  if (temp==0x80)
  return true;
  else
  return false;
}
 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::checkMediaDecodeBusy(void)
{ ru8 temp;

  registerPageSwitch(1);
  lcdRegWrite(0xA0);
  do{
     temp=lcdDataRead();
   }while(temp&0x40);
  registerPageSwitch(0);
}

 //**************************************************************//
 /* return 1 : Media decoder is busy
    return 0 : Media decoder is free */	
 //**************************************************************//
boolean Ra8889_Lite::getMediaDecodeBusyFlag(void)
{ ru8 temp;

  registerPageSwitch(1);
  lcdRegWrite(0xA0);
  temp=lcdDataRead()&0x40;
  registerPageSwitch(0);
  
  if (temp==0x40)
  return true;
  else
  return false;
  
}

 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::resetIdec(void)
{ ru8 temp;

  registerPageSwitch(1);
  lcdRegWrite(0xA9);
  lcdDataWrite(0x02);
  registerPageSwitch(0);
}

 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::idecSelectSfi_0(void)	
{ ru8 temp;

  registerPageSwitch(1);
  lcdRegWrite(0xB6);
  temp = lcdDataRead();
  temp &= 0x3E;   //if bit0=1, IDEC will start .
  lcdDataWrite(temp);
  registerPageSwitch(0);
}

 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::idecSelectSfi_1(void)	
{ ru8 temp;

  registerPageSwitch(1);
  lcdRegWrite(0xB6);
  temp = lcdDataRead();
  temp &= 0x3E;   //if bit0=1, IDEC will start .
  temp |= 0x40;
  lcdDataWrite(temp);
  registerPageSwitch(0);
}
 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::idecSelectSfi_2(void)	
{ ru8 temp;

  registerPageSwitch(1);
  lcdRegWrite(0xB6);
  temp = lcdDataRead();
  temp &= 0x3E;   //if bit0=1, IDEC will start .
  temp |= 0x80;
  lcdDataWrite(temp);
  registerPageSwitch(0);
}
 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::idecSelectSfi_3(void)	
{ ru8 temp;

  registerPageSwitch(1);
  lcdRegWrite(0xB6);
  temp = lcdDataRead();
  temp &= 0x3E;
  temp |= 0xC0;
  lcdDataWrite(temp);
  registerPageSwitch(0);
}

 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::idecSelectBus0(void)	
{ ru8 temp;

  registerPageSwitch(1);
  lcdRegWrite(0xB6);
  temp = lcdDataRead();
  temp &= cClrb3;
  lcdDataWrite(temp);
  registerPageSwitch(0);
}
 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::idecSelectBus1(void)	
{ ru8 temp;

  registerPageSwitch(1);
  lcdRegWrite(0xB6);
  temp = lcdDataRead();
  temp |= cSetb3;
  lcdDataWrite(temp);
  registerPageSwitch(0);
}

 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::idecDestinationColor16bpp(void)
{ ru8 temp;

  registerPageSwitch(1);
  lcdRegWrite(0xB6);
  temp = lcdDataRead();
  temp &= 0xF8; //if bit0=1, IDEC will start .
  temp |= 0x02;
  lcdDataWrite(temp);
  registerPageSwitch(0);
}
 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::idecDestinationColor24bpp(void)
{ ru8 temp;

  registerPageSwitch(1);
  lcdRegWrite(0xB6);
  temp = lcdDataRead();
  temp &= 0xF8;   //if bit0=1, IDEC will start .
  temp |= 0x04;
  lcdDataWrite(temp);
  registerPageSwitch(0);
}

 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::idecStartsDecoding(void)
{ ru8 temp;

  registerPageSwitch(1);
  lcdRegWrite(0xB6);
  temp = lcdDataRead();
  temp |= cSetb0;
  lcdDataWrite(temp);
  registerPageSwitch(0); 
}
 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::checkIdecBusy(void)
{ ru8 temp;

  registerPageSwitch(1);

    lcdRegWrite(0xB6);
   do{
    temp = lcdDataRead();
   }while(temp&0x01);	

   registerPageSwitch(0);
}

 //**************************************************************//
 /* return 1 :	Busy
    return 0 :  Idle */
 //**************************************************************//
boolean Ra8889_Lite::getIdecBusyFlag(void)
{ ru8 temp;

  registerPageSwitch(1);
  lcdRegWrite(0xB6);
  temp = lcdDataRead()&0x01;
  registerPageSwitch(0);
   
  if (temp==0x01)
  return true;
  else
  return false; 
}

 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::idecSelectSfi24bitAddress(void)
{ ru8 temp;

  registerPageSwitch(1);
  lcdRegWrite(0xB7);
  temp = lcdDataRead();
  temp &= cClrb5;
  lcdDataWrite(temp);
  registerPageSwitch(0);
}
 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::idecSelectSfi32bitAddress(void)
{ ru8 temp;

  registerPageSwitch(1);
  lcdRegWrite(0xB7);
  temp = lcdDataRead();
  temp |= cSetb5;
  lcdDataWrite(temp);
  registerPageSwitch(0);
}
 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::idecSelectStandardSpiMode0AndMode3(void)
{ ru8 temp;

  registerPageSwitch(1);

  lcdRegWrite(0xB7);
  temp = lcdDataRead();
  temp &= cClrb4;
  lcdDataWrite(temp);
  registerPageSwitch(0);
}
 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::idecSelectRA8875SpiMode0AndMode3(void)
{ ru8 temp;

  registerPageSwitch(1);

  lcdRegWrite(0xB7);
  temp = lcdDataRead();
  temp |= cSetb4;
  lcdDataWrite(temp);
  registerPageSwitch(0);
}

 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::idecSelectSfiNormalRead_03h(void)
{ ru8 temp;

  registerPageSwitch(1);
  lcdRegWrite(0xB7);
  temp = lcdDataRead();
  temp &= 0xF0;
  lcdDataWrite(temp);
  registerPageSwitch(0);
}
 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::idecSelectSfiFastRead_0Bh(void)
{ ru8 temp;

  registerPageSwitch(1);
  lcdRegWrite(0xB7);
  temp = lcdDataRead();
  temp &= 0xF0;
  temp |= 0x02;
  lcdDataWrite(temp);
  registerPageSwitch(0);
}
 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::idecSelectSfiFastRead_1Bh(void)
{ ru8 temp;

  registerPageSwitch(1);
  lcdRegWrite(0xB7);
  temp = lcdDataRead();
  temp &= 0xF0;
  temp |= 0x04;
  lcdDataWrite(temp);
  registerPageSwitch(0);
}
 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::idecSelectSfiDualRead_3Bh(void)//
{ ru8 temp;

  registerPageSwitch(1);
  lcdRegWrite(0xB7);
  temp = lcdDataRead();
  temp &= 0xF0;
  temp |= 0x06;
  lcdDataWrite(temp);
  registerPageSwitch(0);
}
 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::idecSelectSfiDualRead_BBh(void)
{ ru8 temp;

  registerPageSwitch(1);
  lcdRegWrite(0xB7);
  temp = lcdDataRead();
  temp &= 0xF0;
  temp |= 0x08;
  lcdDataWrite(temp);
  registerPageSwitch(0);
}
 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::idecSelectSfiQuadRead_6Bh(void)
{ ru8 temp;

  registerPageSwitch(1);
  lcdRegWrite(0xB7);
  temp = lcdDataRead();
  temp &= 0xF0;
  temp |= 0x0A;
  lcdDataWrite(temp);
  registerPageSwitch(0);
}
 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::idecSelectSfiQuadRead_EBh(void)
{ ru8 temp;

  registerPageSwitch(1);
  lcdRegWrite(0xB7);
  temp = lcdDataRead();
  temp &= 0xF0;
  temp |= 0x0C;
  lcdDataWrite(temp);
  registerPageSwitch(0);
}

 //**************************************************************//
 /*  divide = 0 : idec clock = core clock 
     divide = 1 : idec clock = core clock/2
     divide = 2 : idec clock = core clock/4    */	
 //**************************************************************//
void Ra8889_Lite::idecClockDivide(ru8 divide)
{ ru8 temp;

  registerPageSwitch(1);
  lcdRegWrite(0xBB);
  lcdDataWrite(divide);
  registerPageSwitch(0);
}

 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::idecSourceStartAddress(ru32 Addr)
{
  registerPageSwitch(1);
  lcdRegWrite(0xBC);	lcdDataWrite(Addr);
  lcdRegWrite(0xBD);	lcdDataWrite(Addr>>8);
  lcdRegWrite(0xBE);	lcdDataWrite(Addr>>16);
  lcdRegWrite(0xBF);	lcdDataWrite(Addr>>24);
  registerPageSwitch(0);
}

 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::idecDestinationUpperLeftCorner(ru16 WX,ru16 HY)
{
  registerPageSwitch(1);
  lcdRegWrite(0xC0);
  lcdDataWrite(WX);
  lcdRegWrite(0xC1);
  lcdDataWrite(WX>>8);
	
  lcdRegWrite(0xC2);
  lcdDataWrite(HY);
  lcdRegWrite(0xC3);
  lcdDataWrite(HY>>8);
  registerPageSwitch(0);
}


 //**************************************************************//	
 /*pip_select 0: AVI display buffer use pip1 + shadow pip
   pip_select 1: AVI display buffer use pip2 + shadow pip*/
 //**************************************************************//
void Ra8889_Lite::idecAviDisplayBufferUsePipAndShadowPip(ru8 pip_select)
{ ru8 temp;

  registerPageSwitch(1);
  if(pip_select==0)
  lcdRegDataWrite(RA8889_IDEC_PIP,RA8889_AVI_DIS_BUFFER_USE_PIP1_AND_SHADOW);//page1 0xC5
  else if (pip_select==1)
  lcdRegDataWrite(RA8889_IDEC_PIP,RA8889_AVI_DIS_BUFFER_USE_PIP2_AND_SHADOW);//page1 0xC5
  registerPageSwitch(0);
}

 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::idecTransferNumber(ru32 Num)
{
  registerPageSwitch(1);
  lcdRegWrite(0xC6);
  lcdDataWrite(Num);
  lcdRegWrite(0xC7);
  lcdDataWrite(Num>>8);
  lcdRegWrite(0xC8);
  lcdDataWrite(Num>>16);
  lcdRegWrite(0xC9);
  lcdDataWrite(Num>>24);
  registerPageSwitch(0);
}

 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::idecDestinationStartAddress(ru32 Addr)//
{
  registerPageSwitch(1);
  lcdRegWrite(0xCA);	lcdDataWrite(Addr);
  lcdRegWrite(0xCB);	lcdDataWrite(Addr>>8);
  lcdRegWrite(0xCC);	lcdDataWrite(Addr>>16);
  lcdRegWrite(0xCD);	lcdDataWrite(Addr>>24);
  registerPageSwitch(0);
}

 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::idecDestinationImageWidth(ru16 WX)//
{
  registerPageSwitch(1);
  lcdRegWrite(0xCE);	lcdDataWrite(WX);
  lcdRegWrite(0xCF);	lcdDataWrite(WX>>8);
  registerPageSwitch(0);
}



 //**************************************************************//  
 //**************************************************************//
void Ra8889_Lite::aviShadowPipStartAddress(ru32 Addr)
{ ru8 temp;

  registerPageSwitch(1);
  lcdRegWrite(0x2E);  lcdDataWrite(Addr);
  lcdRegWrite(0x2F);  lcdDataWrite(Addr>>8);
  lcdRegWrite(0x30);  lcdDataWrite(Addr>>16);
  lcdRegWrite(0x31);  lcdDataWrite(Addr>>24);
  registerPageSwitch(0);
}


 //**************************************************************//
 /*Pause, the video will be paused when the bit is set*/	
 //**************************************************************//
void Ra8889_Lite::setAviPause(void)
{ ru8 temp;

  registerPageSwitch(1);
  lcdRegDataWrite(RA8889_AVI_PAUSE,RA8889_AVI_PAUSE_ENTER_EXIT);//page1 0xD3
  registerPageSwitch(0);
}

 //**************************************************************//
 /* return  : 1 – AVI pause
              0 – AVI display	*/
 //**************************************************************//
boolean Ra8889_Lite::getAviPauseFlag(void)
{ ru8 temp;

  registerPageSwitch(1);
  temp=lcdRegDataRead(RA8889_AVI_PAUSE);//page1 0xD3
  registerPageSwitch(0);
  
  if (temp == 0)
  return false;
  else
  return true;
}

 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::stopAvi(void)
{ ru8 temp;

  registerPageSwitch(1); 
  lcdRegDataWrite(RA8889_AVI_STOP,RA8889_AVI_STOP_ENABLE);//page1 0xD4
  registerPageSwitch(0);
}


 //**************************************************************//
 //avi window buffer use pip1
 //1: window on
 //0: window off
 //**************************************************************//
 void Ra8889_Lite::aviWindowOn(boolean enable)
 {
    if(enable==true)
    {pip1_Enable();}
    else
    {pip1_Disable();}
 }

/*spim function*/
 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::spimDataPut(ru8 data)
{
  ru8 temp;
  lcdRegWrite(0xB8);
  lcdDataWrite(data);
}
 //**************************************************************//	
 //**************************************************************//
ru8 Ra8889_Lite::getSpimTxFifoEmptyFlag()
{
  lcdRegWrite(0xBA);
  if((lcdDataRead()&0x80)==0x80)
  return 1;
  else
  return 0;
}
 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::spimSelectNssDriveOn_xnsfcs0(void)
{
  ru8 temp;
  lcdRegWrite(0xB9);
  temp = lcdDataRead();
  temp &= cClrb7;
  temp &= cClrb5;
  lcdDataWrite(temp);

}
 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::spimSelectNssDriveOn_xnsfcs1(void)
{
  ru8 temp;
  lcdRegWrite(0xB9);
  temp = lcdDataRead();
  temp &= cClrb7;
  temp |= cSetb5;
  lcdDataWrite(temp);

}
 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::spimSelectNssDriveOn_xnsfcs2(void)
{
  ru8 temp;
  lcdRegWrite(0xB9);
  temp = lcdDataRead();
  temp |= cSetb7;
  temp &= cClrb5;
  lcdDataWrite(temp);

}
 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::spimSelectNssDriveOn_xnsfcs3(void)
{
  ru8 temp;
  lcdRegWrite(0xB9);
  temp = lcdDataRead();
  temp |= cSetb7;
  temp |= cSetb5;
  lcdDataWrite(temp);
}

 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::spimNssHigh(void)
{
  ru8 temp;
  lcdRegWrite(0xB9);
  temp = lcdDataRead();
  temp &= cClrb4;
  lcdDataWrite(temp);
}

 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::spimNssLow(void)
{
  ru8 temp;
  lcdRegWrite(0xB9);
  temp = lcdDataRead();
  temp |= cSetb4;
  lcdDataWrite(temp);
}
 //**************************************************************//
 /* bus_select= 0 : Bus0 (xsck,xmosi,xmiso),
    bus_select= 1 : Bus1 (xspi1_sck, xspi1_msio0, xspi1_msio1)*/
 //**************************************************************//
void Ra8889_Lite::spimSelectBus(ru8 bus_select)	
{ ru8 temp;
  
  lcdRegDataWrite(RA8889_SPIMBS,bus_select<<7|SPIM_RX_LATCH_FALLING_EDGE<<5);//0xC5
 
}
 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::spimClockPeriod(ru8 divide)
 {
   lcdRegDataWrite(0xBB,divide);//
 }
 
  //**************************************************************//
 /*If used 32bit address serial flash through ra8889, must be set command B7h to serial flash to enter 4bytes mode first.
 only needs set one times after power on
 
 this function supported as the below
 bus_select = 0 --> Bus 0 (xsck, xmosi, xmiso)
 scs_select = 0 --> xnfscs[0]
 scs_select = 1 --> xnfscs[1]
 
 bus_select = 1 --> Bus 1 (xspi1_sck, xspi1_msio0, xspi1_msio1)
 scs_select = 2 --> xnfscs[2]
 scs_select = 3 --> xnfscs[3]
 */
 //**************************************************************//
 void Ra8889_Lite:: spimSetSerialFlash4BytesMode(ru8 bus_select,ru8 scs_select)
 {
  if(bus_select==0)
     spimSelectBus(0);
   else if(bus_select==1) 
     spimSelectBus(1);
     
  if(scs_select==0)
    spimSelectNssDriveOn_xnsfcs0();
  else if (scs_select==1) 
    spimSelectNssDriveOn_xnsfcs1();
  else if (scs_select==2) 
    spimSelectNssDriveOn_xnsfcs2();
  else if (scs_select==3)   
    spimSelectNssDriveOn_xnsfcs3(); 
  
   spimNssLow();
   spimDataPut(0xB7);
   delay(1);
   spimNssHigh();  
   delay(1);
 }
 
 //**************************************************************//
 // this function supported as the below
 /*
 bus_select = 0 --> Bus 0 (xsck, xmosi, xmiso)
 scs_select = 0 --> xnfscs[0]
 scs_select = 1 --> xnfscs[1]
 
 bus_select = 1 --> Bus 1 (xspi1_sck, xspi1_msio0, xspi1_msio1)
 scs_select = 2 --> xnfscs[2]
 scs_select = 3 --> xnfscs[3]
 
 flash_select = 0 --> set MXIC flash Quad mode, 01h,40h
 flash_select = 1 --> set winbond Quad mode, 31h,02h
 flash_select > 1 --> user set data1 and data2 for enter flash quad mode
 */
 //**************************************************************//
 void Ra8889_Lite:: spimSetSerialFlashQuadMode(ru8 bus_select,ru8 scs_select,ru8 flash_select,ru8 data1, ru8 data2)
 {
  if(bus_select==0)
     spimSelectBus(0);
   else if(bus_select==1) 
     spimSelectBus(1);
     
  if(scs_select==0)
    spimSelectNssDriveOn_xnsfcs0();
  else if (scs_select==1) 
    spimSelectNssDriveOn_xnsfcs1();
  else if (scs_select==2) 
    spimSelectNssDriveOn_xnsfcs2();
  else if (scs_select==3)   
    spimSelectNssDriveOn_xnsfcs3(); 
   
   spimNssLow();
   spimDataPut(0x06); //set flash write enable
   delay(1);
   spimNssHigh();
   delay(1);
   
   if(flash_select==0)// MXIC flash Quad mode
   {
   spimNssLow();
   spimDataPut(0x01);
   delay(1);
   spimDataPut(0x40);
   delay(1);
   spimNssHigh();  
   }
   else if (flash_select==1)   // winbond flash Quad mode
   {
   spimNssLow();
   spimDataPut(0x31);
   delay(1);
   spimDataPut(0x02);
   delay(1);
   spimNssHigh(); 
   }
   else                        // other flash Quad mode
   {
   spimNssLow();
   spimDataPut(data1);
   delay(1);
   spimDataPut(data2);
   delay(1);
   spimNssHigh(); 
   }
    
  delay(100); 

 }
  //**************************************************************//	
 //**************************************************************//
 
 
 /*pip function*/
  //**************************************************************//	
 //**************************************************************//
 void Ra8889_Lite::pip1_Enable(void)
{ ru8 temp;
  lcdRegWrite(0x10);
  temp = lcdDataRead();
  temp |= cSetb7;
  lcdDataWrite(temp);
}
 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::pip1_Disable(void)
{ ru8 temp;
  lcdRegWrite(0x10);
  temp = lcdDataRead();
  temp &= cClrb7;
  lcdDataWrite(temp);
}
 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::pip2_Enable(void)
{ ru8 temp;
  lcdRegWrite(0x10);
  temp = lcdDataRead();
  temp |= cSetb6;
  lcdDataWrite(temp);
}
 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::pip2_Disable(void)
{ ru8 temp;
  lcdRegWrite(0x10);
  temp = lcdDataRead();
  temp &= cClrb6;
  lcdDataWrite(temp);
}
 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::pip1_ParameterSelect(void)
{ ru8 temp;
  lcdRegWrite(0x10);
  temp = lcdDataRead();
  temp &= cClrb4;
  lcdDataWrite(temp);
}
 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::pip2_ParameterSelect(void)
{ ru8 temp;
  lcdRegWrite(0x10);
  temp = lcdDataRead();
  temp |= cSetb4;
  lcdDataWrite(temp);
}
 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::pip1_SelectWindow16bpp(void)
{ ru8 temp;
  lcdRegWrite(0x11);
  temp = lcdDataRead();
  temp &= cClrb3;
  temp |= cSetb2;
  lcdDataWrite(temp);
}
 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::pip1_SelectWindow24bpp(void)
{ ru8 temp;
  lcdRegWrite(0x11);
  temp = lcdDataRead();
  temp |= cSetb3;
  lcdDataWrite(temp);
}
 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::pip2_SelectWindow16bpp(void)
{ ru8 temp;
  lcdRegWrite(0x11);
  temp = lcdDataRead();
  temp &= cClrb1;
  temp |= cSetb0;
  lcdDataWrite(temp);
}
 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::pip2_SelectWindow24bpp(void)
{ ru8 temp;
  lcdRegWrite(0x11);
  temp = lcdDataRead();
  temp |= cSetb1;
  lcdDataWrite(temp);
} 
 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::pipWindowDisplay_XY(ru16 WX,ru16 HY)	
{
 lcdRegWrite(0x2A);	
 lcdDataWrite(WX);	
 lcdRegWrite(0x2B);	
 lcdDataWrite(WX>>8);	
 lcdRegWrite(0x2C);	
 lcdDataWrite(HY);
 lcdRegWrite(0x2D);	
 lcdDataWrite(HY>>8);
}
 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::pipImageStartAddress(ru32 Addr)	
{
 lcdRegWrite(0x2E);	
 lcdDataWrite(Addr);
 lcdRegWrite(0x2F);	
 lcdDataWrite(Addr>>8);
 lcdRegWrite(0x30);	
 lcdDataWrite(Addr>>16);
 lcdRegWrite(0x31);	
 lcdDataWrite(Addr>>24);
}
 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::pipImageWidth(ru16 WX)	
{
 lcdRegWrite(0x32);	
 lcdDataWrite(WX);
 lcdRegWrite(0x33);	
 lcdDataWrite(WX>>8);
}
 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::pipWindowImageStartXY(ru16 WX,ru16 HY)	
{
 lcdRegWrite(0x34);	
 lcdDataWrite(WX);
 lcdRegWrite(0x35);	
 lcdDataWrite(WX>>8);
 lcdRegWrite(0x36);	
 lcdDataWrite(HY);
 lcdRegWrite(0x37);	
 lcdDataWrite(HY>>8);
}
 //**************************************************************//	
 //**************************************************************//
void Ra8889_Lite::pipWindowWidthHeight(ru16 WX,ru16 HY)	
{
 lcdRegWrite(0x38);	
 lcdDataWrite(WX);
 lcdRegWrite(0x39);	
 lcdDataWrite(WX>>8);
 lcdRegWrite(0x3A);	
 lcdDataWrite(HY);
 lcdRegWrite(0x3B);	
 lcdDataWrite(HY>>8);
}
