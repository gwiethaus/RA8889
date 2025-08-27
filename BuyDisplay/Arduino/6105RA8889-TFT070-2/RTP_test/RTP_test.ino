/***************************************************
//Web: http://www.buydisplay.com
EastRising Technology Co.,LTD
Examples for ER-PCBA6105RA8889+ER-ER-TFT080-1  Resistive touch screen test
Display is Hardward SPI 4-Wire SPI Interface and 5V Power Supply,RTP is Hardward  SPI 4-Wire SPI interface.
Tested and worked with:
Works with Arduino 1.6.0 IDE  
Test ok:  Arduino UNO,Arduino MEGA,Arduino DUE
****************************************************/
#include <SPI.h>
#include "RA8889.h"
#include "RTouch.h"


uint8_t buf[10];
uint16_t TouchLocation_x;
uint16_t TouchLocation_y;

UTouch  myTouch(1);

void setup() {
 
  ER5517.Parallel_Init();
  ER5517.HW_Reset();
  ER5517.System_Check_Temp();
  delay(100);
  while(ER5517.LCD_StatusRead()&0x02);
  ER5517.initial();
  ER5517.Display_ON();

     pinMode(5,   OUTPUT);
    digitalWrite(5, HIGH);//Disable  SD
    
  myTouch.InitTouch();
  myTouch.setPrecision(PREC_EXTREME);

}
void loop() 
{

  ER5517.Select_Main_Window_16bpp();
  ER5517.Main_Image_Start_Address(layer1_start_addr);        
  ER5517.Main_Image_Width(LCD_XSIZE_TFT);
  ER5517.Main_Window_Start_XY(0,0);
  ER5517.Canvas_Image_Start_address(0);
  ER5517.Canvas_image_width(LCD_XSIZE_TFT);
  ER5517.Active_Window_XY(0,0);
  ER5517.Active_Window_WH(LCD_XSIZE_TFT,LCD_YSIZE_TFT); 
  
  ER5517.Foreground_color_65k(Black);
  ER5517.Line_Start_XY(0,0);
  ER5517.Line_End_XY(LCD_XSIZE_TFT-1,LCD_YSIZE_TFT-1);
  ER5517.Start_Square_Fill(); 
  
   ER5517.Foreground_color_65k(White);
  ER5517.Background_color_65k(Black);
  ER5517.CGROM_Select_Internal_CGROM();  
  ER5517.Font_Select_12x24_24x24();
  ER5517.Goto_Text_XY(0,0); 
  ER5517.Show_String( "www.buydisplay.com");   
  ER5517.Goto_Text_XY(0,24); 
  ER5517.Show_String( "Resistive touch screen tese"); 
  ER5517.Goto_Text_XY(5,LCD_YSIZE_TFT-25); 
  ER5517.Show_String( "Clear");       
  
 while(1)
  { 

   while(  myTouch.dataAvailable() == 0) 
    {
      myTouch.read();
    TouchLocation_x =   myTouch.getX();
    TouchLocation_y=   myTouch.getY();
    if ((TouchLocation_x!=-1) and (TouchLocation_y!=-1))       
      
            
            if ((TouchLocation_x<50) &&(TouchLocation_y>LCD_YSIZE_TFT-30))
              {    ER5517.Foreground_color_65k(Black);
                  ER5517.Line_Start_XY(0,0);
                  ER5517.Line_End_XY(LCD_XSIZE_TFT-1,LCD_YSIZE_TFT-1);
                  ER5517.Start_Square_Fill(); 
                  
                   ER5517.Foreground_color_65k(White);
                  ER5517.Background_color_65k(Black);
                  ER5517.CGROM_Select_Internal_CGROM();  
                  ER5517.Font_Select_12x24_24x24();
                  ER5517.Goto_Text_XY(0,0); 
                  ER5517.Show_String( "www.buydisplay.com");   
                  ER5517.Goto_Text_XY(0,24); 
                  ER5517.Show_String( "Resistive touch screen test"); 
                  ER5517.Goto_Text_XY(5,LCD_YSIZE_TFT-25); 
                  ER5517.Show_String( "Clear");                  
              }  
              
          else{                                       
              snprintf((char*)buf,sizeof(buf),"(%3d,%3d)",TouchLocation_x,TouchLocation_y); 
            const  char *str=(const char *)buf;
             ER5517.Foreground_color_65k(Red);  
            ER5517.Text_Mode();
            ER5517.Goto_Text_XY(50,80);
            ER5517.LCD_CmdWrite(0x04);
            while(*str != '\0')
            {
            ER5517.LCD_DataWrite(*str);
            ER5517.Check_Mem_WR_FIFO_not_Full();      
            ++str; 
            } 
            ER5517.Check_2D_Busy();
            ER5517.Graphic_Mode(); //back to graphic mode;
           
            ER5517.DrawCircle_Fill(TouchLocation_x,TouchLocation_y, 3, Red);  
   
          }
    }     
  }   
    
 
}
