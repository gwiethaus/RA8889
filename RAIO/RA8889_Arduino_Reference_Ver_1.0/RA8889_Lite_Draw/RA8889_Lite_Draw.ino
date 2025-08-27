//=========================================//
/*
File Name : Ra8889_Lite_Draw                                  
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

  #ifdef COLOR_DEPTH_24BPP
  //clear page1
  ra8889lite.canvasImageStartAddress(PAGE1_START_ADDR);
  ra8889lite.canvasImageWidth(SCREEN_WIDTH);
  ra8889lite.activeWindowXY(0,0);
  ra8889lite.activeWindowWH(SCREEN_WIDTH,SCREEN_HEIGHT); 
  ra8889lite.drawSquareFill(0, 0, 799, 599, COLOR16M_BLACK);
  
  //draw on page1 
  ra8889lite.drawLine(40,40,159,159,COLOR16M_RED);
  ra8889lite.drawLine(40,159,159,40,COLOR16M_LIGHTRED);
  
  ra8889lite.drawSquare(200+30, 50, 399-30, 199-50, COLOR16M_GRAYSCALE13);
  ra8889lite.drawSquareFill(420, 20, 579, 179, COLOR16M_GREEN);
  
  ra8889lite.drawCircleSquare(600+30,0+50, 799-30, 199-50, 20, 20, COLOR16M_BLUE2);
  ra8889lite.drawCircleSquareFill(50,200, 149, 399, 10, 10, COLOR16M_BLUE);
  
  ra8889lite.drawTriangle(220,250,360,360,250,380,COLOR16M_MAGENTA);
  ra8889lite.drawTriangleFill(500,220,580,380,420,380,COLOR16M_LIGHTMAGENTA);

  ra8889lite.drawCircle(700,300,80,COLOR16M_YELLOW);
  ra8889lite.drawCircleFill(100,500,60,COLOR16M_LIGHTYELLOW);
  
  ra8889lite.drawEllipse(300,500,50,80,COLOR16M_CYAN);
  ra8889lite.drawEllipseFill(500,500,80,50,COLOR16M_LIGHTCYAN);

  #endif
  
  #ifdef COLOR_DEPTH_16BPP
  //clear page1
  ra8889lite.canvasImageStartAddress(PAGE1_START_ADDR);
  ra8889lite.canvasImageWidth(SCREEN_WIDTH);
  ra8889lite.activeWindowXY(0,0);
  ra8889lite.activeWindowWH(SCREEN_WIDTH,SCREEN_HEIGHT); 
  ra8889lite.drawSquareFill(0, 0, 799, 599, COLOR65K_BLACK);
  
  //draw on page1 
  ra8889lite.drawLine(40,40,159,159,COLOR65K_RED);
  ra8889lite.drawLine(40,159,159,40,COLOR65K_LIGHTRED);
  
  ra8889lite.drawSquare(200+30, 50, 399-30, 199-50, COLOR65K_GRAYSCALE23);
  ra8889lite.drawSquareFill(420, 20, 579, 179, COLOR65K_GREEN);
  
  ra8889lite.drawCircleSquare(600+30,0+50, 799-30, 199-50, 20, 20, COLOR65K_BLUE2);
  ra8889lite.drawCircleSquareFill(50,200, 149, 399, 10, 10, COLOR65K_BLUE);
  
  ra8889lite.drawTriangle(220,250,360,360,250,380,COLOR65K_MAGENTA);
  ra8889lite.drawTriangleFill(500,220,580,380,420,380,COLOR65K_LIGHTMAGENTA);

  ra8889lite.drawCircle(700,300,80,COLOR65K_YELLOW);
  ra8889lite.drawCircleFill(100,500,60,COLOR65K_LIGHTYELLOW);
  
  ra8889lite.drawEllipse(300,500,50,80,COLOR65K_CYAN);
  ra8889lite.drawEllipseFill(500,500,80,50,COLOR65K_LIGHTCYAN);

  #endif

  while(1);
}
