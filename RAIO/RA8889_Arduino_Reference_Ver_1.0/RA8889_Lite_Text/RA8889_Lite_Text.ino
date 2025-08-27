//=========================================//
/*
File Name : Ra8889_Lite_Text                                 
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

const int RA8889_XNSCS = 52;
const int RA8889_XNRESET = 51;

 char stringEnd ='\0';
 char string1[] = {0xB7,0xE7,0xA6,0xF6,0xAC,0xEC,0xA7,0xDE,stringEnd};//瑞佑科技  //BIG5
 char string2[] = {0xBC,0xF2,0xCC,0xE5,0xD6,0xD0,0xCE,0xC4,stringEnd};//潠极笢恅  //BG2312

Ra8889_Lite ra8889lite(RA8889_XNSCS, RA8889_XNRESET);  
   
void setup() {
   Serial.begin(9600);
   Serial.println("RA8889 Lite");
   
     delay(100);
   if (!ra8889lite.begin()) 
   {
   Serial.println("RA8889 or RA8877 Fail");
   while (1);
   }
   Serial.println("RA8889 or RA8877 Pass!");
   
   ra8889lite.displayOn(true);
 
}

void loop() {
   unsigned int i;
   double float_data;
   //char char_buffer[20];

  #ifdef COLOR_DEPTH_24BPP
  
  //clear page1
  ra8889lite.canvasImageStartAddress(PAGE1_START_ADDR);
  ra8889lite.canvasImageWidth(SCREEN_WIDTH);
  ra8889lite.activeWindowXY(0,0);
  ra8889lite.activeWindowWH(SCREEN_WIDTH,SCREEN_HEIGHT); 
  ra8889lite.drawSquareFill(0, 0, 799, 599, COLOR16M_BLUE);

  ra8889lite.setTextParameter1(RA8889_SELECT_INTERNAL_CGROM,RA8889_CHAR_HEIGHT_24,RA8889_SELECT_8859_1);//cch
  ra8889lite.setTextParameter2(RA8889_TEXT_FULL_ALIGN_DISABLE, RA8889_TEXT_CHROMA_KEY_DISABLE,RA8889_TEXT_WIDTH_ENLARGEMENT_X1,RA8889_TEXT_HEIGHT_ENLARGEMENT_X1);
  ra8889lite.textColor(COLOR16M_BLUE,COLOR16M_MAGENTA);
  ra8889lite.putString(10,10,"Show internal font 12x24");
  
  ra8889lite.setTextParameter1(RA8889_SELECT_INTERNAL_CGROM,RA8889_CHAR_HEIGHT_24,RA8889_SELECT_8859_1);//cch
  ra8889lite.setTextParameter2(RA8889_TEXT_FULL_ALIGN_DISABLE, RA8889_TEXT_CHROMA_KEY_ENABLE,RA8889_TEXT_WIDTH_ENLARGEMENT_X2,RA8889_TEXT_HEIGHT_ENLARGEMENT_X2);
  ra8889lite.textColor(COLOR16M_WHITE,COLOR16M_RED);
  ra8889lite.putString(10,44,"font enlarge x2");
  
  ra8889lite.setTextParameter1(RA8889_SELECT_INTERNAL_CGROM,RA8889_CHAR_HEIGHT_24,RA8889_SELECT_8859_1);//cch
  ra8889lite.setTextParameter2(RA8889_TEXT_FULL_ALIGN_DISABLE, RA8889_TEXT_CHROMA_KEY_DISABLE,RA8889_TEXT_WIDTH_ENLARGEMENT_X3,RA8889_TEXT_HEIGHT_ENLARGEMENT_X3);
  ra8889lite.textColor(COLOR16M_WHITE,COLOR16M_RED);
  ra8889lite.putString(10,102,"font enlarge x3");
  
  ra8889lite.setTextParameter1(RA8889_SELECT_INTERNAL_CGROM,RA8889_CHAR_HEIGHT_24,RA8889_SELECT_8859_1);//cch
  ra8889lite.setTextParameter2(RA8889_TEXT_FULL_ALIGN_DISABLE, RA8889_TEXT_CHROMA_KEY_DISABLE,RA8889_TEXT_WIDTH_ENLARGEMENT_X4,RA8889_TEXT_HEIGHT_ENLARGEMENT_X4);
  ra8889lite.textColor(COLOR16M_WHITE,COLOR16M_LIGHTCYAN);
  ra8889lite.putString(10,184,"font enlarge x4");
  
  //used genitop rom 
  
  ra8889lite.setTextParameter1(RA8889_SELECT_EXTERNAL_CGROM,RA8889_CHAR_HEIGHT_16,RA8889_SELECT_8859_1);//cch
  ra8889lite.setTextParameter2(RA8889_TEXT_FULL_ALIGN_DISABLE, RA8889_TEXT_CHROMA_KEY_ENABLE,RA8889_TEXT_WIDTH_ENLARGEMENT_X1,RA8889_TEXT_HEIGHT_ENLARGEMENT_X1);
  ra8889lite.genitopCharacterRomParameter(RA8889_SERIAL_FLASH_SELECT0,RA8889_SPI_DIV4,RA8889_GT30L24T3Y,RA8889_BIG5,RA8889_GT_FIXED_WIDTH);
  ra8889lite.textColor(COLOR16M_BLACK,COLOR16M_RED);
  ra8889lite.putString(10,290,"show external GT font 16x16");
  
  ra8889lite.setTextParameter1(RA8889_SELECT_EXTERNAL_CGROM,RA8889_CHAR_HEIGHT_24,RA8889_SELECT_8859_1);//cch
  ra8889lite.setTextParameter2(RA8889_TEXT_FULL_ALIGN_DISABLE, RA8889_TEXT_CHROMA_KEY_ENABLE,RA8889_TEXT_WIDTH_ENLARGEMENT_X1,RA8889_TEXT_HEIGHT_ENLARGEMENT_X1);
  ra8889lite.genitopCharacterRomParameter(RA8889_SERIAL_FLASH_SELECT0,RA8889_SPI_DIV4,RA8889_GT30L24T3Y,RA8889_BIG5,RA8889_GT_VARIABLE_WIDTH_ARIAL);
  ra8889lite.putString(10,316,"show external GT font 24x24 with Arial font");

  ra8889lite.setTextParameter2(RA8889_TEXT_FULL_ALIGN_DISABLE, RA8889_TEXT_CHROMA_KEY_ENABLE,RA8889_TEXT_WIDTH_ENLARGEMENT_X2,RA8889_TEXT_HEIGHT_ENLARGEMENT_X2);
  ra8889lite.putString(10,350,string1);
  ra8889lite.setTextParameter1(RA8889_SELECT_EXTERNAL_CGROM,RA8889_CHAR_HEIGHT_24,RA8889_SELECT_8859_1);//cch
  ra8889lite.setTextParameter2(RA8889_TEXT_FULL_ALIGN_DISABLE, RA8889_TEXT_CHROMA_KEY_ENABLE,RA8889_TEXT_WIDTH_ENLARGEMENT_X1,RA8889_TEXT_HEIGHT_ENLARGEMENT_X1);
  ra8889lite.genitopCharacterRomParameter(RA8889_SERIAL_FLASH_SELECT0,RA8889_SPI_DIV4,RA8889_GT30L24T3Y,RA8889_GB2312,RA8889_GT_FIXED_WIDTH);
  ra8889lite.putString(10,408,string2);
  delay(3000);
//while(1);  

 //demo display decimals
  ra8889lite.canvasImageStartAddress(PAGE1_START_ADDR);
  ra8889lite.canvasImageWidth(SCREEN_WIDTH);
  ra8889lite.activeWindowXY(0,0);
  ra8889lite.activeWindowWH(SCREEN_WIDTH,SCREEN_HEIGHT); 
  ra8889lite.drawSquareFill(0, 0, 799, 599, COLOR16M_BLUE);

  ra8889lite.setTextParameter1(RA8889_SELECT_INTERNAL_CGROM,RA8889_CHAR_HEIGHT_24,RA8889_SELECT_8859_1);//cch
  ra8889lite.setTextParameter2(RA8889_TEXT_FULL_ALIGN_DISABLE, RA8889_TEXT_CHROMA_KEY_DISABLE,RA8889_TEXT_WIDTH_ENLARGEMENT_X1,RA8889_TEXT_HEIGHT_ENLARGEMENT_X1);
  ra8889lite.textColor(COLOR16M_WHITE,COLOR16M_BLACK);
  
  ra8889lite.putDec(10,10,1,2,"n");
  ra8889lite.putDec(10,44,2147483647,11,"n");
  ra8889lite.putDec(10,78,-12345,10,"n");
  ra8889lite.putDec(10,112,-2147483648,11,"n");
  
  ra8889lite.putDec(10,146,1,2,"-");
  ra8889lite.putDec(10,180,2147483647,11,"-");
  ra8889lite.putDec(10,214,-12345,10,"-");
  ra8889lite.putDec(10,248,-2147483648,11,"-");
  
  ra8889lite.putDec(10,282,1,2,"+");
  ra8889lite.putDec(10,316,2147483647,11,"+");
  ra8889lite.putDec(10,350,-12345,10,"+");
  ra8889lite.putDec(10,384,-2147483648,11,"+");
  
  ra8889lite.putDec(10,418,1,2,"0");
  ra8889lite.putDec(10,452,2147483647,11,"0");
  ra8889lite.putDec(10,486,-12345,10,"0");
  ra8889lite.putDec(10,520,-2147483648,11,"0");
  
  delay(3000);
  //while(1);
  
  //demo display float
  ra8889lite.canvasImageStartAddress(PAGE1_START_ADDR);
  ra8889lite.canvasImageWidth(SCREEN_WIDTH);
  ra8889lite.activeWindowXY(0,0);
  ra8889lite.activeWindowWH(SCREEN_WIDTH,SCREEN_HEIGHT); 
  ra8889lite.drawSquareFill(0, 0, 799, 599, COLOR16M_BLUE);

  ra8889lite.setTextParameter1(RA8889_SELECT_INTERNAL_CGROM,RA8889_CHAR_HEIGHT_24,RA8889_SELECT_8859_1);//cch
  ra8889lite.setTextParameter2(RA8889_TEXT_FULL_ALIGN_DISABLE, RA8889_TEXT_CHROMA_KEY_DISABLE,RA8889_TEXT_WIDTH_ENLARGEMENT_X1,RA8889_TEXT_HEIGHT_ENLARGEMENT_X1);
  ra8889lite.textColor(COLOR16M_WHITE,COLOR16M_BLACK);
 
  ra8889lite.putFloat(10,10,1.1,7,1,"n");
  ra8889lite.putFloat(10,44,483647.12,11,2,"n");
  ra8889lite.putFloat(10,78,-12345.123,11,3,"n");
  ra8889lite.putFloat(10,112,-123456.1234,11,4,"n");
  
  ra8889lite.putFloat(10,146,1.1234,7,1,"-");
  ra8889lite.putFloat(10,180,483647.12,11,2,"-");
  ra8889lite.putFloat(10,214,-12345.123,11,3,"-");
  ra8889lite.putFloat(10,248,-123456.1234,11,4,"-");
  
  ra8889lite.putFloat(10,282,1.1,7,1,"+");
  ra8889lite.putFloat(10,316,483647.12,11,2,"+");
  ra8889lite.putFloat(10,350,-12345.123,11,3,"+");
  ra8889lite.putFloat(10,384,-123456.1234,11,4,"+");
  
  ra8889lite.putFloat(10,418,1.1,7,1,"0");
  ra8889lite.putFloat(10,452,483647.12,11,2,"0");
  ra8889lite.putFloat(10,486,-12345.123,11,3,"0");
  ra8889lite.putFloat(10,520,-123456.1234,11,4,"0");
  delay(3000);
  //while(1);
//demo display Hex
  ra8889lite.canvasImageStartAddress(PAGE1_START_ADDR);
  ra8889lite.canvasImageWidth(SCREEN_WIDTH);
  ra8889lite.activeWindowXY(0,0);
  ra8889lite.activeWindowWH(SCREEN_WIDTH,SCREEN_HEIGHT); 
  ra8889lite.drawSquareFill(0, 0, 799, 599, COLOR16M_BLUE);

  ra8889lite.setTextParameter1(RA8889_SELECT_INTERNAL_CGROM,RA8889_CHAR_HEIGHT_24,RA8889_SELECT_8859_1);//cch
  ra8889lite.setTextParameter2(RA8889_TEXT_FULL_ALIGN_DISABLE, RA8889_TEXT_CHROMA_KEY_DISABLE,RA8889_TEXT_WIDTH_ENLARGEMENT_X1,RA8889_TEXT_HEIGHT_ENLARGEMENT_X1);
  ra8889lite.textColor(COLOR16M_WHITE,COLOR16M_BLACK);
  
  ra8889lite.putHex(10,10,1,4,"n");
  ra8889lite.putHex(10,44,255,6,"n");
  ra8889lite.putHex(10,78,0xa7c8,6,"n");
  ra8889lite.putHex(10,112,0xdd11ff55,10,"n");
  
  ra8889lite.putHex(10,146,1,4,"0");
  ra8889lite.putHex(10,180,255,6,"0");
  ra8889lite.putHex(10,214,0xa7c8,6,"0");
  ra8889lite.putHex(10,248,0xdd11ff55,10,"0");
  
  ra8889lite.putHex(10,282,1,4,"#");
  ra8889lite.putHex(10,316,255,6,"#");
  ra8889lite.putHex(10,350,0xa7c8,6,"#");
  ra8889lite.putHex(10,384,0xdd11ff55,10,"#");
  
  ra8889lite.putHex(10,418,1,4,"x");
  ra8889lite.putHex(10,452,255,6,"x");
  ra8889lite.putHex(10,486,0xa7c8,6,"x");
  ra8889lite.putHex(10,520,0xdd11ff55,10,"x");
  delay(3000);

  #endif

  
 // while(1);
}
