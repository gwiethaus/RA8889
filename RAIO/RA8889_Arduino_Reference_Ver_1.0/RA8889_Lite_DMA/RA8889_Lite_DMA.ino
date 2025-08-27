//=========================================//
/*
File Name : Ra8889_Lite_DMA                                  
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
#include "All_8889_Pic_NOR.h"


//#define DMA_DEMO_24BIT_ADDR   //select for Flash size <=128Mbit
#define DMA_DEMO_32BIT_ADDR     //select for Flash size >128Mbit


const int RA8889_XNSCS = 52;
const int RA8889_XNRESET = 51;

Ra8889_Lite ra8889lite(RA8889_XNSCS, RA8889_XNRESET);  
   
void setup() {
   Serial.begin(9600);
   Serial.println("RA8889 Lite");
   
   delay(100);
   if (!ra8889lite.begin()) 
   {
   Serial.println("RA8889 Fail");
   while (1);
   }
   Serial.println("RA8889 Pass!");
   
   ra8889lite.displayOn(true);
}

void loop() {
   unsigned short i;
 //this demo dma bitmap data only 24bpp RGB888 format be stored in norflash, do not define COLOR_DEPTH_16BPP in Ra8889_Lite.h.
 #ifdef DMA_DEMO_24BIT_ADDR   
  /***Demo function demo***/
  /*DMA demo 24bit address*/ 
  while(1)
  {
  //clear page1
  ra8889lite.canvasImageStartAddress(PAGE1_START_ADDR);
  ra8889lite.canvasImageWidth(SCREEN_WIDTH);
  ra8889lite.activeWindowXY(0,0);
  ra8889lite.activeWindowWH(SCREEN_WIDTH,SCREEN_HEIGHT); 
  ra8889lite.drawSquareFill(0, 0, 799, 599, COLOR16M_BROWN); 

  //demo 24bit address serial flash DMA function
  //dma function suggested run at BUS0 scs0 or scs1
  ra8889lite.dma_24bitAddressBlockMode(0,1,RA8889_SPI_DIV2,0,0,BINARY_INFO[p1].img_width,BINARY_INFO[p1].img_height,BINARY_INFO[p1].img_width,BINARY_INFO[p1].start_addr);
   //while(1);
  delay(1000);
  ra8889lite.dma_24bitAddressBlockMode(0,1,RA8889_SPI_DIV2,0,0,BINARY_INFO[p2].img_width,BINARY_INFO[p2].img_height,BINARY_INFO[p2].img_width,BINARY_INFO[p2].start_addr);
  //while(1);
  delay(1000);
  ra8889lite.dma_24bitAddressBlockMode(0,1,RA8889_SPI_DIV2,0,0,BINARY_INFO[p3].img_width,BINARY_INFO[p3].img_height,BINARY_INFO[p3].img_width,BINARY_INFO[p3].start_addr);
   //while(1);
  delay(1000);

  while(1);
  }
#endif   

#ifdef DMA_DEMO_32BIT_ADDR  
  /*DMA demo 32bit address*/ 
  //when using the 32bit address serial flash, must be setting serial flash to 4Bytes mode 
  //only needs set one times after power on
  ra8889lite.spimSetSerialFlash4BytesMode(0,1);  
  while(1)
  {
  ra8889lite.canvasImageStartAddress(PAGE1_START_ADDR);
  ra8889lite.canvasImageWidth(SCREEN_WIDTH);
  ra8889lite.activeWindowXY(0,0);
  ra8889lite.activeWindowWH(SCREEN_WIDTH,SCREEN_HEIGHT); 
  ra8889lite.drawSquareFill(0, 0, 799, 599, COLOR16M_GRAYSCALE3); 

  //demo 32bit address serial flash DMA function
  ra8889lite.dma_32bitAddressBlockMode(0,1,RA8889_SPI_DIV2,30,20,BINARY_INFO[p1].img_width,BINARY_INFO[p1].img_height,BINARY_INFO[p1].img_width,BINARY_INFO[p1].start_addr);
  delay(1000);
  //while(1);
  
  ra8889lite.dma_32bitAddressBlockMode(0,1,RA8889_SPI_DIV2,30,20,BINARY_INFO[p2].img_width,BINARY_INFO[p2].img_height,BINARY_INFO[p2].img_width,BINARY_INFO[p2].start_addr);
  delay(1000);
  //while(1);
  
  ra8889lite.dma_32bitAddressBlockMode(0,1,RA8889_SPI_DIV2,30,20,BINARY_INFO[p3].img_width,BINARY_INFO[p3].img_height,BINARY_INFO[p3].img_width,BINARY_INFO[p3].start_addr);
  delay(1000);
  //while(1);
  }
  //while(1);
 #endif  

 
}
