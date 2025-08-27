//=========================================//
/*
File Name : Ra8889_Lite_IDEC                                 
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

//#define IDEC_DEMO_24BIT_ADDR
#define IDEC_DEMO_32BIT_ADDR


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

#ifdef IDEC_DEMO_24BIT_ADDR   
  /*IDEC demo 24bit address*/ 
  
  //set serial flash Quad mode for IDEC function
  //only needs set one times after power on
   
  ra8889lite.spimSetSerialFlashQuadMode(1,3,0,0x00,0x00);
  
  while(1)
  {
  //clear page1
  ra8889lite.canvasImageStartAddress(PAGE1_START_ADDR);
  ra8889lite.canvasImageWidth(SCREEN_WIDTH);
  ra8889lite.activeWindowXY(0,0);
  ra8889lite.activeWindowWH(SCREEN_WIDTH,SCREEN_HEIGHT); 
  

  #ifdef COLOR_DEPTH_24BPP 
  ra8889lite.drawSquareFill(0, 0, 799, 599, COLOR16M_BLUE); 
  ra8889lite.idec_24bitAddressQuadMode6B_24bpp_JPEG(1,3,5,10,BINARY_INFO[bird].start_addr,BINARY_INFO[bird].img_size,SCREEN_WIDTH,PAGE1_START_ADDR);
  delay(1000);
  ra8889lite.idec_24bitAddressQuadMode6B_24bpp_JPEG(1,3,330,10,BINARY_INFO[cat].start_addr,BINARY_INFO[cat].img_size,SCREEN_WIDTH,PAGE1_START_ADDR);
  delay(1000);
  ra8889lite.idec_24bitAddressQuadMode6B_24bpp_JPEG(1,3,5,260,BINARY_INFO[fish].start_addr,BINARY_INFO[fish].img_size,SCREEN_WIDTH,PAGE1_START_ADDR);
  delay(1000);
  ra8889lite.idec_24bitAddressQuadMode6B_24bpp_AVI(1,3,330,260,BINARY_INFO[demo_video320240].start_addr,BINARY_INFO[demo_video320240].img_size,
                                                 320,240, PAGE2_START_ADDR,PAGE3_START_ADDR,SCREEN_WIDTH);
  ra8889lite.aviWindowOn(1);                                               
  while( ra8889lite.getMediaDecodeBusyFlag());  //check busy flag until media decode not busy, user can run font/dma function with different sfi bus when idec function run avi decoding,      
  ra8889lite.aviWindowOn(0); 
  
  #endif


  #ifdef COLOR_DEPTH_16BPP
  ra8889lite.drawSquareFill(0, 0, 799, 599, COLOR65K_BLUE); 
  
  //demo 24bit address serial flash IDEC function

  ra8889lite.idec_24bitAddressQuadMode6B_16bpp_JPEG(1,3,5,10,BINARY_INFO[bird].start_addr,BINARY_INFO[bird].img_size,SCREEN_WIDTH,PAGE1_START_ADDR);
  delay(1000);

  ra8889lite.idec_24bitAddressQuadMode6B_16bpp_JPEG(1,3,330,10,BINARY_INFO[cat].start_addr,BINARY_INFO[cat].img_size,SCREEN_WIDTH,PAGE1_START_ADDR);
  delay(1000);

  ra8889lite.idec_24bitAddressQuadMode6B_16bpp_JPEG(1,3,5,260,BINARY_INFO[fish].start_addr,BINARY_INFO[fish].img_size,SCREEN_WIDTH,PAGE1_START_ADDR);
  delay(1000);

  ra8889lite.idec_24bitAddressQuadMode6B_16bpp_AVI(1,3,330,260,BINARY_INFO[demo_video320240].start_addr,BINARY_INFO[demo_video320240].img_size,
                                                 320,240, PAGE2_START_ADDR,PAGE3_START_ADDR,SCREEN_WIDTH);
  ra8889lite.aviWindowOn(1);
  while( ra8889lite.getMediaDecodeBusyFlag());  //check busy flag until media decode not busy, user can run font/dma function with different sfi bus when idec function run avi decoding  
  ra8889lite.aviWindowOn(0);

  
  #endif
  //while(1);
  }
  
#endif   


#ifdef IDEC_DEMO_32BIT_ADDR  
  /*IDEC demo 32bit address*/ 
  //when using the 32bit address serial flash, must be setting serial flash to 4Bytes mode and Quad mode for IDEC function
  //only needs set one times after power on

  ra8889lite.spimSetSerialFlashQuadMode(1,2,1,0x00,0x00);
  ra8889lite.spimSetSerialFlash4BytesMode(1,2); 
  
  while(1)
  {
  ra8889lite.canvasImageStartAddress(PAGE1_START_ADDR);
  ra8889lite.canvasImageWidth(SCREEN_WIDTH);
  ra8889lite.activeWindowXY(0,0);
  ra8889lite.activeWindowWH(SCREEN_WIDTH,SCREEN_HEIGHT); 
  
  #ifdef COLOR_DEPTH_24BPP
  ra8889lite.drawSquareFill(0, 0, 799, 599, COLOR16M_LIGHTCYAN);
  
  ra8889lite.idec_32bitAddressQuadMode6B_24bpp_JPEG(1,2,5,10,BINARY_INFO[bird].start_addr,BINARY_INFO[bird].img_size,SCREEN_WIDTH,PAGE1_START_ADDR);
  delay(1000);

  ra8889lite.idec_32bitAddressQuadMode6B_24bpp_JPEG(1,2,330,10,BINARY_INFO[cat].start_addr,BINARY_INFO[cat].img_size,SCREEN_WIDTH,PAGE1_START_ADDR);
  delay(1000);

  ra8889lite.idec_32bitAddressQuadMode6B_24bpp_JPEG(1,2,5,260,BINARY_INFO[fish].start_addr,BINARY_INFO[fish].img_size,SCREEN_WIDTH,PAGE1_START_ADDR);
  delay(1000);
  
  ra8889lite.idec_32bitAddressQuadMode6B_24bpp_AVI(1,2,330,260,BINARY_INFO[demo_video320240].start_addr,BINARY_INFO[demo_video320240].img_size,
                                                 320,240, PAGE2_START_ADDR,PAGE3_START_ADDR,SCREEN_WIDTH);                                                
  ra8889lite.aviWindowOn(1);
  while( ra8889lite.getMediaDecodeBusyFlag());  //check busy flag until media decode not busy, user can run font/dma function with different sfi bus when idec function run avi decoding,     
  ra8889lite.aviWindowOn(0);
  #endif


   #ifdef COLOR_DEPTH_16BPP
  ra8889lite.drawSquareFill(0, 0, 799, 599, COLOR65K_LIGHTCYAN); 
  
  ra8889lite.idec_32bitAddressQuadMode6B_16bpp_JPEG(1,2,5,10,BINARY_INFO[bird].start_addr,BINARY_INFO[bird].img_size,SCREEN_WIDTH,PAGE1_START_ADDR);
  delay(1000);

  ra8889lite.idec_32bitAddressQuadMode6B_16bpp_JPEG(1,2,330,10,BINARY_INFO[cat].start_addr,BINARY_INFO[cat].img_size,SCREEN_WIDTH,PAGE1_START_ADDR);
  delay(1000);

  ra8889lite.idec_32bitAddressQuadMode6B_16bpp_JPEG(1,2,5,260,BINARY_INFO[fish].start_addr,BINARY_INFO[fish].img_size,SCREEN_WIDTH,PAGE1_START_ADDR);
  delay(1000);
  
  ra8889lite.idec_32bitAddressQuadMode6B_16bpp_AVI(1,2,330,260,BINARY_INFO[demo_video320240].start_addr,BINARY_INFO[demo_video320240].img_size,
                                                 320,240, PAGE2_START_ADDR,PAGE3_START_ADDR,SCREEN_WIDTH);
  ra8889lite.aviWindowOn(1);
  while( ra8889lite.getMediaDecodeBusyFlag()); //check busy flag until media decode not busy, user can run font/dma function with different sfi bus when idec function run avi decoding,                                                
  ra8889lite.aviWindowOn(0);                                               
  #endif

  
 //while(1);
  }
  //while(1);
 #endif    
 
 
 
 
}
