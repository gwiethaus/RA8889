
//=========================================//
/*
File Name : Ra8889_Lite_BTE                                  
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


#ifdef COLOR_DEPTH_24BPP 
#include "pic24bpp_1.h"
#include "pic24bpp_2.h"
#endif

#ifdef COLOR_DEPTH_16BPP 
#include "pic16bpp_byte.h"
#include "pic16bpp_word.h"
#endif

#include "bw.h"


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
     
 #ifdef   COLOR_DEPTH_24BPP
  /***bte memory copy demo***/
  //clear page1
  ra8889lite.canvasImageStartAddress(PAGE1_START_ADDR);
  ra8889lite.canvasImageWidth(SCREEN_WIDTH);
  
  ra8889lite.activeWindowXY(0,0);
  ra8889lite.activeWindowWH(SCREEN_WIDTH,SCREEN_HEIGHT); 
  ra8889lite.drawSquareFill(0, 0, SCREEN_WIDTH-1, SCREEN_HEIGHT-1, COLOR16M_BLUE);
  
  //clear page2
  ra8889lite.canvasImageStartAddress(PAGE2_START_ADDR);
  ra8889lite.drawSquareFill(0, 0, SCREEN_WIDTH-1, SCREEN_HEIGHT-1, COLOR16M_RED);
  
  //write picture to page2
  ra8889lite.putPicture_24bpp(50,50,128,128,pic24bpp_1);
  //while(1);
  //write string to page1
  ra8889lite.canvasImageStartAddress(PAGE1_START_ADDR);
  ra8889lite.textColor(COLOR16M_WHITE,COLOR16M_BLACK);
  ra8889lite.setTextParameter1(RA8889_SELECT_INTERNAL_CGROM,RA8889_CHAR_HEIGHT_24,RA8889_SELECT_8859_1);//cch
  ra8889lite.setTextParameter2(RA8889_TEXT_FULL_ALIGN_ENABLE, RA8889_TEXT_CHROMA_KEY_ENABLE,RA8889_TEXT_WIDTH_ENLARGEMENT_X1,RA8889_TEXT_HEIGHT_ENLARGEMENT_X1);
  ra8889lite.putString(0,0,"BTE memory copy,copy page2 picture to page1 display");
  //copy page2 picture to page1 
  ra8889lite.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,50,50,PAGE1_START_ADDR,SCREEN_WIDTH, 50,50,128,128); 
  ra8889lite.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,50,50,PAGE1_START_ADDR,SCREEN_WIDTH, (50+128),50,128,128);
  ra8889lite.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,50,50,PAGE1_START_ADDR,SCREEN_WIDTH, (50+128+128),50,128,128);
  //ra8889lite.mainImageStartAddress(PAGE2_START_ADDR);
  //delay(3000);
  //while(1);
  /***bte memory copy with rop demo***/ 
  //write string to page1
  ra8889lite.canvasImageStartAddress(PAGE1_START_ADDR);
  ra8889lite.putString(0,178,"BTE memory copy with ROP, copy page2 picture to page1 display"); 
  ra8889lite.bteMemoryCopyWithROP(PAGE2_START_ADDR,SCREEN_WIDTH,50,50,PAGE1_START_ADDR,SCREEN_WIDTH,50,228,
                            PAGE1_START_ADDR,SCREEN_WIDTH, 50,228,128,128,RA8889_BTE_ROP_CODE_1); 
  ra8889lite.bteMemoryCopyWithROP(PAGE2_START_ADDR,SCREEN_WIDTH,50,50,PAGE1_START_ADDR,SCREEN_WIDTH,(50+128),228,
                            PAGE1_START_ADDR,SCREEN_WIDTH, (50+128),228,128,128,RA8889_BTE_ROP_CODE_2);
  ra8889lite.bteMemoryCopyWithROP(PAGE2_START_ADDR,SCREEN_WIDTH,50,50,PAGE1_START_ADDR,SCREEN_WIDTH,(50+128+128),228,
                            PAGE1_START_ADDR,SCREEN_WIDTH, (50+128+128),228,128,128,RA8889_BTE_ROP_CODE_3);  

  //while(1);
  /***bte memory copy with chromakey demo***/ 
  ra8889lite.putString(0,356,"BTE memory copy with ChromaKey, copy page2 picture to page1 display"); 
  ra8889lite.bteMemoryCopyWithChromaKey(PAGE2_START_ADDR,SCREEN_WIDTH,50,50, PAGE1_START_ADDR,SCREEN_WIDTH,50,406,128,128,0xff0000);
  ra8889lite.bteMemoryCopyWithChromaKey(PAGE2_START_ADDR,SCREEN_WIDTH,50,50, PAGE1_START_ADDR,SCREEN_WIDTH,50+128,406,128,128,0xff0000);
  ra8889lite.bteMemoryCopyWithChromaKey(PAGE2_START_ADDR,SCREEN_WIDTH,50,50, PAGE1_START_ADDR,SCREEN_WIDTH,50+128+128,406,128,128,0xff0000);
  //delay(3000); 
  //while(1);
  
  //clear page1
  ra8889lite.canvasImageStartAddress(PAGE1_START_ADDR);
  ra8889lite.canvasImageWidth(SCREEN_WIDTH);
  ra8889lite.activeWindowXY(0,0);
  ra8889lite.activeWindowWH(SCREEN_WIDTH,SCREEN_HEIGHT); 
  ra8889lite.drawSquareFill(0, 0, 799, 599, COLOR16M_BLUE); 
  
  /***bte MPU write with ROP demo***/
  ra8889lite.textColor(COLOR16M_WHITE,COLOR16M_BLACK);
  ra8889lite.setTextParameter1(RA8889_SELECT_INTERNAL_CGROM,RA8889_CHAR_HEIGHT_24,RA8889_SELECT_8859_1);//cch
  ra8889lite.setTextParameter2(RA8889_TEXT_FULL_ALIGN_ENABLE, RA8889_TEXT_CHROMA_KEY_ENABLE,RA8889_TEXT_WIDTH_ENLARGEMENT_X1,RA8889_TEXT_HEIGHT_ENLARGEMENT_X1);
  ra8889lite.putString(0,0,"BTE MPU write with ROP, write picture to page1, format byte");
  ra8889lite.bteMpuWriteWithROP(PAGE1_START_ADDR,SCREEN_WIDTH,50,50,PAGE1_START_ADDR,SCREEN_WIDTH,
                                50,50,128,128,RA8889_BTE_ROP_CODE_4,pic24bpp_2);                           
  ra8889lite.bteMpuWriteWithROP(PAGE1_START_ADDR,SCREEN_WIDTH,50+128,50,PAGE1_START_ADDR,SCREEN_WIDTH,
                                50+128,50,128,128,RA8889_BTE_ROP_CODE_5,pic24bpp_2);                         
  ra8889lite.bteMpuWriteWithROP(PAGE1_START_ADDR,SCREEN_WIDTH,50+128+128,50,PAGE1_START_ADDR,SCREEN_WIDTH,
                                50+128+128,50,128,128,RA8889_BTE_ROP_CODE_6,pic24bpp_2); 
  //delay(3000);
  //while(1);                             
  ra8889lite.putString(0,356,"BTE MPU write with Chroma Key, write picture to page1, format byte"); 
  ra8889lite.bteMpuWriteWithChromaKey(PAGE1_START_ADDR,SCREEN_WIDTH, 50,406,128,128,0xff0000,pic24bpp_1);

  //delay(3000);
 
  //while(1);
  //clear page1
  ra8889lite.canvasImageStartAddress(PAGE1_START_ADDR);
  ra8889lite.canvasImageWidth(SCREEN_WIDTH);
  ra8889lite.activeWindowXY(0,0);
  ra8889lite.activeWindowWH(SCREEN_WIDTH,SCREEN_HEIGHT); 
  ra8889lite.drawSquareFill(0, 0, 799, 599, COLOR16M_BLUE); 
  /***bte MPU write color expansion demo***/
  ra8889lite.textColor(COLOR16M_WHITE,COLOR16M_BLACK);
  ra8889lite.setTextParameter1(RA8889_SELECT_INTERNAL_CGROM,RA8889_CHAR_HEIGHT_24,RA8889_SELECT_8859_1);//cch
  ra8889lite.setTextParameter2(RA8889_TEXT_FULL_ALIGN_ENABLE, RA8889_TEXT_CHROMA_KEY_ENABLE,RA8889_TEXT_WIDTH_ENLARGEMENT_X1,RA8889_TEXT_HEIGHT_ENLARGEMENT_X1);
  ra8889lite.putString(0,0,"BTE MPU write with color expansion, write black and white picture data to page1");
  ra8889lite.bteMpuWriteColorExpansion(PAGE1_START_ADDR,SCREEN_WIDTH,50,50,128,128,COLOR16M_BLACK,COLOR16M_WHITE,bw);
  ra8889lite.bteMpuWriteColorExpansion(PAGE1_START_ADDR,SCREEN_WIDTH,50+128,50,128,128,COLOR16M_WHITE,COLOR16M_BLACK,bw);
  ra8889lite.bteMpuWriteColorExpansion(PAGE1_START_ADDR,SCREEN_WIDTH,50+128+128,50,128,128,COLOR16M_YELLOW,COLOR16M_CYAN,bw);
  ra8889lite.textColor(COLOR16M_WHITE,COLOR16M_BLACK);
  ra8889lite.putString(0,178,"BTE MPU write with color expansion with chroma key, write black and white picture data to page1");
  
  ra8889lite.bteMpuWriteColorExpansionWithChromaKey(PAGE1_START_ADDR,SCREEN_WIDTH,50,228,128,128,COLOR16M_BLACK,COLOR16M_WHITE,bw);
  ra8889lite.bteMpuWriteColorExpansionWithChromaKey(PAGE1_START_ADDR,SCREEN_WIDTH,50+128,228,128,128,COLOR16M_WHITE,COLOR16M_BLACK,bw);
  ra8889lite.bteMpuWriteColorExpansionWithChromaKey(PAGE1_START_ADDR,SCREEN_WIDTH,50+128+128,228,128,128,COLOR16M_YELLOW,COLOR16M_BLACK,bw);
  //delay(3000);
  //while(1);


  //demo BTE memory with ARGB after DMA function 
  //clear page1
  ra8889lite.canvasImageStartAddress(PAGE1_START_ADDR);
  ra8889lite.canvasImageWidth(SCREEN_WIDTH);
  ra8889lite.activeWindowXY(0,0);
  ra8889lite.activeWindowWH(SCREEN_WIDTH,SCREEN_HEIGHT); 
  ra8889lite.drawSquareFill(0, 0, 799, 599, COLOR16M_BLUE); 

  ra8889lite.spimSetSerialFlash4BytesMode(0,1);//
  ra8889lite.dma_32bitAddressLinearMode(0,1,2,BINARY_INFO[Aircraft_PNG].start_addr,PAGE9_START_ADDR,BINARY_INFO[Aircraft_PNG].img_size);
  
  ra8889lite.bteMemoryCopyWith_ARGB8888(PAGE9_START_ADDR,BINARY_INFO[Aircraft_PNG].img_width,PAGE1_START_ADDR,SCREEN_WIDTH,10,20,BINARY_INFO[Aircraft_PNG].img_width,
                                      BINARY_INFO[Aircraft_PNG].img_height);


  ra8889lite.dma_32bitAddressLinearMode(0,1,2,BINARY_INFO[Car_PNG].start_addr,PAGE9_START_ADDR,BINARY_INFO[Car_PNG].img_size);
  
  ra8889lite.bteMemoryCopyWith_ARGB8888(PAGE9_START_ADDR,BINARY_INFO[Car_PNG].img_width,PAGE1_START_ADDR,SCREEN_WIDTH,100,20,BINARY_INFO[Car_PNG].img_width,
                                      BINARY_INFO[Car_PNG].img_height);


  ra8889lite.dma_32bitAddressLinearMode(0,1,2,BINARY_INFO[Lion_PNG].start_addr,PAGE9_START_ADDR,BINARY_INFO[Lion_PNG].img_size);
  ra8889lite.bteMemoryCopyWith_ARGB8888(PAGE9_START_ADDR,BINARY_INFO[Lion_PNG].img_width,PAGE1_START_ADDR,SCREEN_WIDTH,150,100,BINARY_INFO[Lion_PNG].img_width,
                                      BINARY_INFO[Lion_PNG].img_height); 
                                                                         
 while(1);
#endif

#ifdef   COLOR_DEPTH_16BPP
  /***bte memory copy demo***/
  //clear page1
  ra8889lite.canvasImageStartAddress(PAGE1_START_ADDR);
  ra8889lite.canvasImageWidth(SCREEN_WIDTH);
  
  ra8889lite.activeWindowXY(0,0);
  ra8889lite.activeWindowWH(SCREEN_WIDTH,SCREEN_HEIGHT); 
  ra8889lite.drawSquareFill(0, 0, SCREEN_WIDTH-1, SCREEN_HEIGHT-1, COLOR65K_BLUE);
  
  //clear page2
  ra8889lite.canvasImageStartAddress(PAGE2_START_ADDR);
  ra8889lite.drawSquareFill(0, 0, SCREEN_WIDTH-1, SCREEN_HEIGHT-1, COLOR65K_RED);
  
  //write picture to page2
  ra8889lite.putPicture_16bpp(50,50,128,128,pic16bpp_word);
  
  //write string to page1
  ra8889lite.canvasImageStartAddress(PAGE1_START_ADDR);
  ra8889lite.textColor(COLOR65K_WHITE,COLOR65K_BLACK);
  ra8889lite.setTextParameter1(RA8889_SELECT_INTERNAL_CGROM,RA8889_CHAR_HEIGHT_24,RA8889_SELECT_8859_1);//cch
  ra8889lite.setTextParameter2(RA8889_TEXT_FULL_ALIGN_ENABLE, RA8889_TEXT_CHROMA_KEY_ENABLE,RA8889_TEXT_WIDTH_ENLARGEMENT_X1,RA8889_TEXT_HEIGHT_ENLARGEMENT_X1);
  ra8889lite.putString(0,0,"BTE memory copy,copy page2 picture to page1 display");
  //copy page2 picture to page1 
  ra8889lite.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,50,50,PAGE1_START_ADDR,SCREEN_WIDTH, 50,50,128,128); 
  ra8889lite.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,50,50,PAGE1_START_ADDR,SCREEN_WIDTH, (50+128),50,128,128);
  ra8889lite.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,50,50,PAGE1_START_ADDR,SCREEN_WIDTH, (50+128+128),50,128,128);
  //ra8889lite.mainImageStartAddress(PAGE2_START_ADDR);
  delay(3000);
  //while(1);
  /***bte memory copy with rop demo***/ 
  //write string to page1
  ra8889lite.canvasImageStartAddress(PAGE1_START_ADDR);
  ra8889lite.putString(0,178,"BTE memory copy with ROP, copy page2 picture to page1 display"); 
  ra8889lite.bteMemoryCopyWithROP(PAGE2_START_ADDR,SCREEN_WIDTH,50,50,PAGE1_START_ADDR,SCREEN_WIDTH,50,228,
                            PAGE1_START_ADDR,SCREEN_WIDTH, 50,228,128,128,RA8889_BTE_ROP_CODE_1); 
  ra8889lite.bteMemoryCopyWithROP(PAGE2_START_ADDR,SCREEN_WIDTH,50,50,PAGE1_START_ADDR,SCREEN_WIDTH,(50+128),228,
                            PAGE1_START_ADDR,SCREEN_WIDTH, (50+128),228,128,128,RA8889_BTE_ROP_CODE_2);
  ra8889lite.bteMemoryCopyWithROP(PAGE2_START_ADDR,SCREEN_WIDTH,50,50,PAGE1_START_ADDR,SCREEN_WIDTH,(50+128+128),228,
                            PAGE1_START_ADDR,SCREEN_WIDTH, (50+128+128),228,128,128,RA8889_BTE_ROP_CODE_3);  
  /***bte memory copy with chromakey demo***/ 
  ra8889lite.putString(0,356,"BTE memory copy with ChromaKey, copy page2 picture to page1 display"); 
  ra8889lite.bteMemoryCopyWithChromaKey(PAGE2_START_ADDR,SCREEN_WIDTH,50,50, PAGE1_START_ADDR,SCREEN_WIDTH,50,406,128,128,0xf800);
  ra8889lite.bteMemoryCopyWithChromaKey(PAGE2_START_ADDR,SCREEN_WIDTH,50,50, PAGE1_START_ADDR,SCREEN_WIDTH,50+128,406,128,128,0xf800);
  ra8889lite.bteMemoryCopyWithChromaKey(PAGE2_START_ADDR,SCREEN_WIDTH,50,50, PAGE1_START_ADDR,SCREEN_WIDTH,50+128+128,406,128,128,0xf800);
  delay(3000); 
  //while(1);
  //clear page1
  ra8889lite.canvasImageStartAddress(PAGE1_START_ADDR);
  ra8889lite.canvasImageWidth(SCREEN_WIDTH);
  ra8889lite.activeWindowXY(0,0);
  ra8889lite.activeWindowWH(SCREEN_WIDTH,SCREEN_HEIGHT); 
  ra8889lite.drawSquareFill(0, 0, 799, 599, COLOR65K_BLUE); 
  
  /***bte MPU write with ROP demo***/
  ra8889lite.textColor(COLOR65K_WHITE,COLOR65K_BLACK);
  ra8889lite.setTextParameter1(RA8889_SELECT_INTERNAL_CGROM,RA8889_CHAR_HEIGHT_24,RA8889_SELECT_8859_1);//cch
  ra8889lite.setTextParameter2(RA8889_TEXT_FULL_ALIGN_ENABLE, RA8889_TEXT_CHROMA_KEY_ENABLE,RA8889_TEXT_WIDTH_ENLARGEMENT_X1,RA8889_TEXT_HEIGHT_ENLARGEMENT_X1);
  ra8889lite.putString(0,0,"BTE MPU write with ROP, write picture to page1, format byte");
  ra8889lite.bteMpuWriteWithROP(PAGE1_START_ADDR,SCREEN_WIDTH,50,50,PAGE1_START_ADDR,SCREEN_WIDTH,
                                50,50,128,128,RA8889_BTE_ROP_CODE_4,pic16bpp_byte);
  ra8889lite.bteMpuWriteWithROP(PAGE1_START_ADDR,SCREEN_WIDTH,50+128,50,PAGE1_START_ADDR,SCREEN_WIDTH,
                                50+128,50,128,128,RA8889_BTE_ROP_CODE_5,pic16bpp_byte);                         
  ra8889lite.bteMpuWriteWithROP(PAGE1_START_ADDR,SCREEN_WIDTH,50+128+128,50,PAGE1_START_ADDR,SCREEN_WIDTH,
                                50+128+128,50,128,128,RA8889_BTE_ROP_CODE_6,pic16bpp_byte); 
  ra8889lite.putString(0,178,"BTE MPU write with ROP, write picture to page1, format word");                              
  ra8889lite.bteMpuWriteWithROP(PAGE1_START_ADDR,SCREEN_WIDTH,50,228,PAGE1_START_ADDR,SCREEN_WIDTH,
                                50,228,128,128,RA8889_BTE_ROP_CODE_7,pic16bpp_word);
  ra8889lite.bteMpuWriteWithROP(PAGE1_START_ADDR,SCREEN_WIDTH,50+128,228,PAGE1_START_ADDR,SCREEN_WIDTH,
                                50+128,228,128,128,RA8889_BTE_ROP_CODE_8,pic16bpp_word);                         
  ra8889lite.bteMpuWriteWithROP(PAGE1_START_ADDR,SCREEN_WIDTH,50+128+128,228,PAGE1_START_ADDR,SCREEN_WIDTH,
                                50+128+128,228,128,128,RA8889_BTE_ROP_CODE_9,pic16bpp_word); 
  ra8889lite.putString(0,356,"BTE MPU write with Chroma Key, write picture to page1, format byte,word"); 
  ra8889lite.bteMpuWriteWithChromaKey(PAGE1_START_ADDR,SCREEN_WIDTH, 50,406,128,128,0xf800,pic16bpp_byte);
  ra8889lite.bteMpuWriteWithChromaKey(PAGE1_START_ADDR,SCREEN_WIDTH, 50+128,406,128,128,0xf800,pic16bpp_word);
  delay(3000);
  //while(1);
  //clear page1
  ra8889lite.canvasImageStartAddress(PAGE1_START_ADDR);
  ra8889lite.canvasImageWidth(SCREEN_WIDTH);
  ra8889lite.activeWindowXY(0,0);
  ra8889lite.activeWindowWH(SCREEN_WIDTH,SCREEN_HEIGHT); 
  ra8889lite.drawSquareFill(0, 0, 799, 599, COLOR65K_BLUE); 
  /***bte MPU write color expansion demo***/
  ra8889lite.textColor(COLOR65K_WHITE,COLOR65K_BLACK);
  ra8889lite.setTextParameter1(RA8889_SELECT_INTERNAL_CGROM,RA8889_CHAR_HEIGHT_24,RA8889_SELECT_8859_1);//cch
  ra8889lite.setTextParameter2(RA8889_TEXT_FULL_ALIGN_ENABLE, RA8889_TEXT_CHROMA_KEY_ENABLE,RA8889_TEXT_WIDTH_ENLARGEMENT_X1,RA8889_TEXT_HEIGHT_ENLARGEMENT_X1);
  ra8889lite.putString(0,0,"BTE MPU write with color expansion, write black and white picture data to page1");
  ra8889lite.bteMpuWriteColorExpansion(PAGE1_START_ADDR,SCREEN_WIDTH,50,50,128,128,COLOR65K_BLACK,COLOR65K_WHITE,bw);
  ra8889lite.bteMpuWriteColorExpansion(PAGE1_START_ADDR,SCREEN_WIDTH,50+128,50,128,128,COLOR65K_WHITE,COLOR65K_BLACK,bw);
  ra8889lite.bteMpuWriteColorExpansion(PAGE1_START_ADDR,SCREEN_WIDTH,50+128+128,50,128,128,COLOR65K_YELLOW,COLOR65K_CYAN,bw);
  ra8889lite.textColor(COLOR65K_WHITE,COLOR65K_BLACK);
  ra8889lite.putString(0,178,"BTE MPU write with color expansion with chroma key, write black and white picture data to page1");
  
  ra8889lite.bteMpuWriteColorExpansionWithChromaKey(PAGE1_START_ADDR,SCREEN_WIDTH,50,228,128,128,COLOR65K_BLACK,COLOR65K_WHITE,bw);
  ra8889lite.bteMpuWriteColorExpansionWithChromaKey(PAGE1_START_ADDR,SCREEN_WIDTH,50+128,228,128,128,COLOR65K_WHITE,COLOR65K_BLACK,bw);
  ra8889lite.bteMpuWriteColorExpansionWithChromaKey(PAGE1_START_ADDR,SCREEN_WIDTH,50+128+128,228,128,128,COLOR65K_YELLOW,COLOR65K_BLACK,bw);
  delay(3000);

 #endif
    
 // while(1);
}
