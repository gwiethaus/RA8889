//=========================================//
/*
File Name : Ra8889_Lite_PWM                                  
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
  ra8889lite.drawSquareFill(0, 0, 799, 599, COLOR16M_BLUE);
  
  /*pwm demo please measure by scope*/
  ra8889lite.pwm_Prescaler(RA8889_PRESCALER); //if core_freq = 120MHz, pwm base clock = 120/(3+1) = 30MHz
  ra8889lite.pwm_ClockMuxReg(RA8889_PWM_TIMER_DIV4, RA8889_PWM_TIMER_DIV4, RA8889_XPWM1_OUTPUT_PWM_TIMER1,RA8889_XPWM0_OUTPUT_PWM_TIMER0);
                               //pwm timer clock = 30/4 = 7.5MHz
                 
  ra8889lite.pwm0_ClocksPerPeriod(1024); // pwm0 = 7.5MHz/1024 = 7.3KHz
  ra8889lite.pwm0_Duty(10);//pwm0 set 10/1024 duty
               
  ra8889lite.pwm1_ClocksPerPeriod(256);  // pwm1 = 7.5MHz/256 = 29.2KHz
  ra8889lite.pwm1_Duty(5); //pwm1 set 5/256 duty
 
  ra8889lite.pwm_Configuration(RA8889_PWM_TIMER1_INVERTER_ON,RA8889_PWM_TIMER1_AUTO_RELOAD,RA8889_PWM_TIMER1_START,RA8889_PWM_TIMER0_DEAD_ZONE_DISABLE 
                      , RA8889_PWM_TIMER0_INVERTER_ON, RA8889_PWM_TIMER0_AUTO_RELOAD,RA8889_PWM_TIMER0_START);                     
  
  #endif
  
 while(1);
  
}
