//=========================================//
/*
File Name : Ra8889_Lite_Graphic                                  
Author    : Mountain                            
Edit Date : 04/21/2020
Version   : v1.0  
*/
//=========================================//
//========== COPYRIGHT 2020 RAiO ==========//
//=========================================//
#include <SPI.h>
#include "Arduino.h"
#include "Print.h"
#include "Ra8889_Lite.h"
#include "pic16bpp_byte.h"
#include "pic16bpp_word.h"
#include "pic24bpp_1.h"
#include "pic24bpp_2.h"

#define DEMO_ASCII_8X12
#define DEMO_ASCII_16X24
#define DEMO_ASCII_32X48


#ifdef DEMO_ASCII_8X12
#include "ascii_table_8x12.h"
void lcdPutChar8x12(unsigned short x,unsigned short y,unsigned long fgcolor,unsigned long bgcolor,boolean bg_transparent,unsigned char code);
void lcdPutString8x12(unsigned short x,unsigned short y, unsigned long fgcolor, unsigned long bgcolor,boolean bg_transparent,char *ptr);
#endif

#ifdef DEMO_ASCII_16X24
#include "ascii_table_16x24.h"
void lcdPutChar16x24(unsigned short x,unsigned short y,unsigned long fgcolor,unsigned long bgcolor,boolean bg_transparent,unsigned char code);
void lcdPutString16x24(unsigned short x,unsigned short y, unsigned long fgcolor, unsigned long bgcolor,boolean bg_transparent,char *ptr);

#endif

#ifdef DEMO_ASCII_32X48
#include "ascii_table_32x48.h"
void lcdPutChar32x48(unsigned short x,unsigned short y,unsigned long fgcolor,unsigned long bgcolor,boolean bg_transparent, unsigned char code);
void lcdPutString32x48(unsigned short x,unsigned short y, unsigned long fgcolor, unsigned long bgcolor,boolean bg_transparent, char *ptr);
#endif





const int RA8889_XNSCS = 52;
const int RA8889_XNRESET = 51;

Ra8889_Lite ra8889lite(RA8889_XNSCS, RA8889_XNRESET);  
   
void setup() {
   Serial.begin(9600);
   Serial.println("RA8889 Lite");
   
   delay(100);
   if (!ra8889lite.begin()) 
   {
   Serial.println("RA8889 Fail!");
   while (1);
   }
   Serial.println("RA8889 Pass!");
   
   ra8889lite.displayOn(true);
 
}

void loop() {
   unsigned long i;

  #ifdef COLOR_DEPTH_24BPP 
   
  //clear page1
  ra8889lite.canvasImageStartAddress(PAGE1_START_ADDR);
  ra8889lite.canvasImageWidth(SCREEN_WIDTH);
  ra8889lite.activeWindowXY(0,0);
  ra8889lite.activeWindowWH(SCREEN_WIDTH,SCREEN_HEIGHT); 
  ra8889lite.drawSquareFill(0, 0, 799, 599, COLOR16M_BLUE);
  
  //write a pixel dot to page1
  ra8889lite.setPixelCursor(20,20);
  ra8889lite.ramAccessPrepare();
  ra8889lite.lcdDataWrite(0x00);//RGB888 Blue data
  ra8889lite.lcdDataWrite(0x00);//RGB888 Green data
  ra8889lite.lcdDataWrite(0xff);//RGB888 Red data
  
  //write a pixel dot to page1
  ra8889lite.putPixel_24bpp(40,40,COLOR16M_MAGENTA);
  delay(3000);
  
 //while(1);
  //clear page1
  ra8889lite.canvasImageStartAddress(PAGE1_START_ADDR);
  ra8889lite.canvasImageWidth(SCREEN_WIDTH);
  ra8889lite.activeWindowXY(0,0);
  ra8889lite.activeWindowWH(SCREEN_WIDTH,SCREEN_HEIGHT); 
  ra8889lite.drawSquareFill(0, 0, 799, 599, COLOR16M_BLUE);
  

  ra8889lite.putPicture_24bpp(50,50,128,128);
  for(i=0;i<16384;i++)
  {
   ra8889lite.lcdDataWrite(COLOR16M_YELLOW);//RGB888 blue data
   ra8889lite.lcdDataWrite(COLOR16M_YELLOW>>8);//RGB888 blue data
   ra8889lite.lcdDataWrite(COLOR16M_YELLOW>>16);//RGB888 blue data
  }

  ra8889lite.putPicture_24bpp(50+128,50+128,128,128,pic24bpp_1);


  delay(3000);
  //while(1);
  //clear page1
  ra8889lite.canvasImageStartAddress(PAGE1_START_ADDR);
  ra8889lite.canvasImageWidth(SCREEN_WIDTH);
  ra8889lite.activeWindowXY(0,0);
  ra8889lite.activeWindowWH(SCREEN_WIDTH,SCREEN_HEIGHT); 
  ra8889lite.drawSquareFill(0, 0, 799, 599, COLOR16M_BLUE);

  #ifdef DEMO_ASCII_8X12
  lcdPutString8x12(0,0,0xFFFFFF,0x000000,true," !\"#$%&'()*+,-./012345678");
  lcdPutString8x12(0,12,0xFFFFFF,0x000000,true,"9:;<=>?@ABCDEFGHIJKLMNOPQ");
  lcdPutString8x12(0,24,0xFFFFFF,0x000000,true,"RSTUVWXYZ[\\]^_`abcdefghij");
  lcdPutString8x12(0,36,0xFFFFFF,0x000000,true,"klmnopqrstuvwxyz{|}~");
  #endif
  
  #ifdef DEMO_ASCII_16X24
  lcdPutString16x24(0,48,0xFFFFFF,0x000000,true," !\"#$%&'()*+,-./012345678");
  lcdPutString16x24(0,72,0xFFFFFF,0x000000,true,"9:;<=>?@ABCDEFGHIJKLMNOPQ");
  lcdPutString16x24(0,96,0xFFFFFF,0x000000,true,"RSTUVWXYZ[\\]^_`abcdefghij");
  lcdPutString16x24(0,120,0xFFFFFF,0x000000,true,"klmnopqrstuvwxyz{|}~");
  #endif
  
  #ifdef DEMO_ASCII_32X48
  lcdPutString32x48(0,144,0xFFFFFF,0x000000,false," !\"#$%&'()*+,-./012345678");
  lcdPutString32x48(0,192,0xFFFFFF,0x000000,false,"9:;<=>?@ABCDEFGHIJKLMNOPQ");
  lcdPutString32x48(0,240,0xFFFFFF,0x000000,false,"RSTUVWXYZ[\\]^_`abcdefghij");
  lcdPutString32x48(0,288,0xFFFFFF,0x000000,false,"klmnopqrstuvwxyz{|}~");
  #endif

  delay(3000);
  
  #endif


  #ifdef COLOR_DEPTH_16BPP 
   
  //clear page1
  ra8889lite.canvasImageStartAddress(PAGE1_START_ADDR);
  ra8889lite.canvasImageWidth(SCREEN_WIDTH);
  ra8889lite.activeWindowXY(0,0);
  ra8889lite.activeWindowWH(SCREEN_WIDTH,SCREEN_HEIGHT); 
  ra8889lite.drawSquareFill(0, 0, 799, 599, COLOR65K_BLUE);
  
  //write a pixel dot to page1
  ra8889lite.setPixelCursor(20,20);
  ra8889lite.ramAccessPrepare();
  ra8889lite.lcdDataWrite(0x00);//RGB565 LSB data
  ra8889lite.lcdDataWrite(0xf8);//RGB565 MSB data
  
  //write a pixel dot to page1
  ra8889lite.setPixelCursor(30,30);
  ra8889lite.ramAccessPrepare();
  ra8889lite.lcdDataWrite16bbp(COLOR65K_WHITE);//RGB565 16bpp data
  
  //write a pixel dot to page1
  ra8889lite.putPixel_16bpp(40,40,COLOR65K_MAGENTA);
  delay(3000);
 //while(1);
  //clear page1
  ra8889lite.canvasImageStartAddress(PAGE1_START_ADDR);
  ra8889lite.canvasImageWidth(SCREEN_WIDTH);
  ra8889lite.activeWindowXY(0,0);
  ra8889lite.activeWindowWH(SCREEN_WIDTH,SCREEN_HEIGHT); 
  ra8889lite.drawSquareFill(0, 0, 799, 599, COLOR65K_BLUE);
  

  ra8889lite.putPicture_16bpp(50,50,128,128);
  for(i=0;i<16384;i++)
  {
   ra8889lite.lcdDataWrite16bbp(COLOR65K_YELLOW);//RGB565 16bpp data
  }

  ra8889lite.putPicture_16bpp(50+128,50+128,128,128,pic16bpp_byte);

  ra8889lite.putPicture_16bpp(50+128+128,50+128+128,128,128,pic16bpp_word);
  
  delay(3000);
  //while(1);
  //clear page1
  ra8889lite.canvasImageStartAddress(PAGE1_START_ADDR);
  ra8889lite.canvasImageWidth(SCREEN_WIDTH);
  ra8889lite.activeWindowXY(0,0);
  ra8889lite.activeWindowWH(SCREEN_WIDTH,SCREEN_HEIGHT); 
  ra8889lite.drawSquareFill(0, 0, 799, 599, COLOR65K_BLUE);

  #ifdef DEMO_ASCII_8X12
  lcdPutString8x12(0,0,0xFFFF,0x0000,true," !\"#$%&'()*+,-./012345678");
  lcdPutString8x12(0,12,0xFFFF,0x0000,true,"9:;<=>?@ABCDEFGHIJKLMNOPQ");
  lcdPutString8x12(0,24,0xFFFF,0x0000,true,"RSTUVWXYZ[\\]^_`abcdefghij");
  lcdPutString8x12(0,36,0xFFFF,0x0000,true,"klmnopqrstuvwxyz{|}~");
  #endif
  
  #ifdef DEMO_ASCII_16X24
  lcdPutString16x24(0,48,0xFFFF,0x0000,true," !\"#$%&'()*+,-./012345678");
  lcdPutString16x24(0,72,0xFFFF,0x0000,true,"9:;<=>?@ABCDEFGHIJKLMNOPQ");
  lcdPutString16x24(0,96,0xFFFF,0x0000,true,"RSTUVWXYZ[\\]^_`abcdefghij");
  lcdPutString16x24(0,120,0xFFFF,0x0000,true,"klmnopqrstuvwxyz{|}~");
  #endif
  
  #ifdef DEMO_ASCII_32X48
  lcdPutString32x48(0,144,0xFFFF,0x0000,false," !\"#$%&'()*+,-./012345678");
  lcdPutString32x48(0,192,0xFFFF,0x0000,false,"9:;<=>?@ABCDEFGHIJKLMNOPQ");
  lcdPutString32x48(0,240,0xFFFF,0x0000,false,"RSTUVWXYZ[\\]^_`abcdefghij");
  lcdPutString32x48(0,288,0xFFFF,0x0000,false,"klmnopqrstuvwxyz{|}~");
  #endif

  delay(3000);
  
  #endif
  
}


void lcdPutChar8x12(unsigned short x,unsigned short y,unsigned long fgcolor,unsigned long bgcolor,boolean bg_transparent,unsigned char code)
{ unsigned short i=0;
  unsigned short j=0;
  unsigned char tmp_char=0;

  for (i=0;i<12;i++)
  {
    tmp_char = ascii_table_8x12[((code-0x20)*12)+i];//minus 32 offset, because this table from ascii table "space" 
   for (j=0;j<8;j++)
   {
    if ( (tmp_char >>7-j) & 0x01 == 0x01)
         { 
          #ifdef COLOR_DEPTH_16BPP
          ra8889lite.putPixel_16bpp(x+j,y+i,fgcolor); //   
          #endif
          #ifdef COLOR_DEPTH_24BPP
          ra8889lite.putPixel_24bpp(x+j,y+i,fgcolor); //   
          #endif
         }    
    else
    {   
        if(!bg_transparent)
        {
         #ifdef COLOR_DEPTH_16BPP 
         ra8889lite.putPixel_16bpp(x+j,y+i,bgcolor); //
         #endif

         #ifdef COLOR_DEPTH_24BPP 
         ra8889lite.putPixel_24bpp(x+j,y+i,bgcolor); //
         #endif
        }
    } 
   }
  }
}

void lcdPutString8x12(unsigned short x,unsigned short y, unsigned long fgcolor, unsigned long bgcolor,boolean bg_transparent,char *ptr)
{unsigned short i = 0;
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

void lcdPutChar16x24(unsigned short x,unsigned short y,unsigned long fgcolor,unsigned long bgcolor,boolean bg_transparent,unsigned char code)
{ unsigned short i=0;
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
         { 
          #ifdef COLOR_DEPTH_16BPP
          ra8889lite.putPixel_16bpp(x+j,y+i,fgcolor); //   
          #endif
          #ifdef COLOR_DEPTH_24BPP
          ra8889lite.putPixel_24bpp(x+j,y+i,fgcolor); //   
          #endif
         }
    else
       {
        if(!bg_transparent)
         {
         #ifdef COLOR_DEPTH_16BPP 
         ra8889lite.putPixel_16bpp(x+j,y+i,bgcolor); //
         #endif

         #ifdef COLOR_DEPTH_24BPP 
         ra8889lite.putPixel_24bpp(x+j,y+i,bgcolor); //
         #endif
        }
       }
   }
  }
}

void lcdPutString16x24(unsigned short x,unsigned short y, unsigned long fgcolor, unsigned long bgcolor,boolean bg_transparent,char *ptr)
{unsigned short i = 0;
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

void lcdPutChar32x48(unsigned short x,unsigned short y,unsigned long fgcolor,unsigned long bgcolor,boolean bg_transparent,unsigned char code)
{ unsigned short i=0;
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
          {  
          #ifdef COLOR_DEPTH_16BPP
          ra8889lite.putPixel_16bpp(x+j,y+i,fgcolor); //   
          #endif
          #ifdef COLOR_DEPTH_24BPP
          ra8889lite.putPixel_24bpp(x+j,y+i,fgcolor); //   
          #endif
         }
     else
         {
         if(!bg_transparent)
          {
           #ifdef COLOR_DEPTH_16BPP 
           ra8889lite.putPixel_16bpp(x+j,y+i,bgcolor); //
           #endif

           #ifdef COLOR_DEPTH_24BPP 
           ra8889lite.putPixel_24bpp(x+j,y+i,bgcolor); //
           #endif
          } 
         }
      } 
  }
}

void lcdPutString32x48(unsigned short x,unsigned short y, unsigned long fgcolor, unsigned long bgcolor,boolean bg_transparent,char *ptr)
{unsigned short i = 0;
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
