/***************************************************
//Web: http://www.buydisplay.com
EastRising Technology Co.,LTD
Examples for ER-PCBA6105RA8889+ER-TFT070-2  Resistive touch screen Calibration
Display is Hardward SPI 4-Wire SPI Interface and 5V Power Supply,RTP is Hardward  SPI 4-Wire SPI interface.
Tested and worked with:
Works with Arduino 1.6.0 IDE  
Test ok:  Arduino UNO,Arduino MEGA,Arduino DUE
****************************************************/
#include <SPI.h>
#include "RA8889.h"
#include "RTouch.h"

// Declare which fonts we will be using
extern uint8_t SmallFont[];

uint32_t cx, cy;
uint32_t rx[8], ry[8];
uint32_t clx, crx, cty, cby;
float px, py;
int dispx, dispy, text_y_center,CENTER;
uint32_t calx, caly, cals;
char buf[13];
#define TOUCH_ORIENTATION LANDSCAPE //PORTRAIT
UTouch  myTouch(1);


void drawCrossHair(int x, int y,int colour)
{
   ER5517.DrawCircle_Fill(x,y,3,colour);
}

void readCoordinates()
{
  int iter = 5000;
  int failcount = 0;
  int cnt = 0;
  uint32_t tx=0;
  uint32_t ty=0;
  boolean OK = false;
  
  while (OK == false)
  {
      ER5517.Foreground_color_65k(White);
       ER5517.Goto_Text_XY(CENTER,text_y_center); 
      ER5517.Show_String( "*  PRESS  *");  
    while (myTouch.dataAvailable() != 0) {}
      ER5517.Goto_Text_XY(CENTER,text_y_center); 
      ER5517.Show_String( "*  HOLD!  *");   

    while ((  myTouch.dataAvailable() == 0) && (cnt<iter) && (failcount<10000))
    {
      myTouch.calibrateRead();
      if (!((myTouch.TP_X==65535) || (myTouch.TP_Y==65535)))
      {
        tx += myTouch.TP_X;
        ty += myTouch.TP_Y;
        cnt++;
      }
      else
        failcount++;
    }
    if (cnt>=iter)
    {
      OK = true;
    }
    else
    {
      tx = 0;
      ty = 0;
      cnt = 0;
    }
    if (failcount>=10000)
      fail();
  }

  cx = tx / iter;
  cy = ty / iter;
}

void calibrate(int x, int y, int i)
{
 
  drawCrossHair(x,y,Red);
  readCoordinates();
  ER5517.Foreground_color_65k(Green);
  ER5517.Goto_Text_XY(CENTER,text_y_center); 
  ER5517.Show_String( "* RELEASE *");  
  drawCrossHair(x,y,Blue);
  rx[i]=cx;
  ry[i]=cy;
  while (myTouch.dataAvailable() != 0) {}
}

void waitForTouch()
{
  while (myTouch.dataAvailable() != 0) {}
  while (myTouch.dataAvailable() == 0) {}
  while (myTouch.dataAvailable() != 0) {}
}

void toHex(uint32_t num)
{
  buf[0] = '0';
  buf[1] = 'x';
  buf[10] = 'U';
  buf[11] = 'L';
  buf[12] = 0;
  for (int zz=9; zz>1; zz--)
  {
    if ((num & 0xF) > 9)
      buf[zz] = (num & 0xF) + 55;
    else
      buf[zz] = (num & 0xF) + 48;
    num=num>>4;
  }
}

void startup()
{

   ER5517.Foreground_color_65k(White);
  ER5517.Goto_Text_XY(0, 1); 
  ER5517.Show_String( "UTouch Calibration");  

  if (dispx==220)
  {  
  ER5517.Goto_Text_XY(0, 30); 
  ER5517.Show_String( "Use a stylus or something");  
  ER5517.Goto_Text_XY(0, 54); 
  ER5517.Show_String( "similar to touch as close");  
  ER5517.Goto_Text_XY(0, 78); 
  ER5517.Show_String( "to the center of the");    
  ER5517.Goto_Text_XY(0, 102); 
  ER5517.Show_String( "highlighted crosshair as");
  ER5517.Goto_Text_XY(0, 126); 
  ER5517.Show_String( "possible. Keep as still as");  
  ER5517.Goto_Text_XY(0, 150); 
  ER5517.Show_String( "possible and keep holding");  
  ER5517.Goto_Text_XY(0, 174); 
  ER5517.Show_String( "until the highlight is");    
  ER5517.Goto_Text_XY(0, 198); 
  ER5517.Show_String( "removed. Repeat for all");  
  ER5517.Goto_Text_XY(0, 222); 
  ER5517.Show_String( "crosshairs in sequence.");  
  ER5517.Goto_Text_XY(0, 246); 
  ER5517.Show_String( "Touch screen to continue");    


  }
  else
  {
  ER5517.Goto_Text_XY(0, 30); 
  ER5517.Show_String( "INSTRUCTIONS");  
  ER5517.Goto_Text_XY(0, 54); 
  ER5517.Show_String( "Use a stylus or something similar to");  
  ER5517.Goto_Text_XY(0, 78); 
  ER5517.Show_String( "touch as close to the center of the");    
  ER5517.Goto_Text_XY(0, 102); 
  ER5517.Show_String( "highlighted crosshair as possible. Keep");
  ER5517.Goto_Text_XY(0, 126); 
  ER5517.Show_String( "as still as possible and keep holding");  
  ER5517.Goto_Text_XY(0, 150); 
  ER5517.Show_String( "until the highlight is removed. Repeat");  
  ER5517.Goto_Text_XY(0, 174); 
  ER5517.Show_String( "for all crosshairs in sequence");    
  ER5517.Goto_Text_XY(0, 198); 
  ER5517.Show_String( "Further instructions will be displayed");  
  ER5517.Goto_Text_XY(0, 222); 
  ER5517.Show_String( "Do NOT use your finger as a calibration");  
  ER5517.Goto_Text_XY(0, 246); 
  ER5517.Show_String( "stylus or the result WILL BE imprecise");    
  ER5517.Goto_Text_XY(0, 270); 
  ER5517.Show_String( "Touch screen to continue");      
    
  }

  waitForTouch();
  ER5517.DrawSquare_Fill(0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,Black);
}

void done()
{
 
  ER5517.Foreground_color_65k(White);
  ER5517.Goto_Text_XY(0, 1); 
  ER5517.Show_String( "UTouch Calibration");   
  
  
  if (dispx==220)
  {  
  ER5517.Goto_Text_XY(0, 30); 
  ER5517.Show_String( "To use the new calibration");  
  ER5517.Goto_Text_XY(0, 54); 
  ER5517.Show_String( "settings you must edit the");  
  ER5517.Foreground_color_65k(Red);
  ER5517.Goto_Text_XY(0, 78); 
  ER5517.Show_String( "UTouchCD.h");  
  ER5517.Foreground_color_65k(White);  
  ER5517.Goto_Text_XY(0, 102); 
  ER5517.Show_String( "file and change");
  ER5517.Goto_Text_XY(0, 126); 
  ER5517.Show_String( "the following values. The");  
  ER5517.Goto_Text_XY(0, 150); 
  ER5517.Show_String( "values are located right");  
  ER5517.Goto_Text_XY(0, 174); 
  ER5517.Show_String( "below the opening comment");    
  
  ER5517.Foreground_color_65k(Green);     
  ER5517.Goto_Text_XY(0, 198); 
  ER5517.Show_String( "CAL_X");  
  ER5517.Goto_Text_XY(0, 222); 
  ER5517.Show_String( "CAL_Y");  
  ER5517.Goto_Text_XY(CENTER, 246); 
  ER5517.Show_String( "CAL_S");      
 
   
  toHex(calx);
  ER5517.Goto_Text_XY(75, 198);  
  ER5517.Show_String(buf);  
  toHex(caly);
  ER5517.Goto_Text_XY(75, 222);  
  ER5517.Show_String(buf);     
  toHex(cals);
  ER5517.Goto_Text_XY(75, 246);  
  ER5517.Show_String(buf); 
  }
  else
  {  
  ER5517.Goto_Text_XY(0, 30); 
  ER5517.Show_String( "CALIBRATION COMPLETE");      
  ER5517.Goto_Text_XY(0, 54); 
  ER5517.Show_String( "To use the new calibration");  
  ER5517.Goto_Text_XY(0, 78); 
  ER5517.Show_String( "settings you must edit the");  
  ER5517.Foreground_color_65k(Red);
  ER5517.Goto_Text_XY(0, 102); 
  ER5517.Show_String( "UTouchCD.h");  
  ER5517.Foreground_color_65k(White);  
  ER5517.Goto_Text_XY(0, 126); 
  ER5517.Show_String( "file and change");
  ER5517.Goto_Text_XY(0, 150); 
  ER5517.Show_String( "the following values. The");  
  ER5517.Goto_Text_XY(0, 174); 
  ER5517.Show_String( "values are located right");  
  ER5517.Goto_Text_XY(0, 198); 
  ER5517.Show_String( "below the opening comment");    
  ER5517.Goto_Text_XY(0, 222); 
   ER5517.Show_String( "the file.");   
  ER5517.Foreground_color_65k(Green);   
  ER5517.Goto_Text_XY(0, 246);  
  ER5517.Show_String( "CAL_X");  
  ER5517.Goto_Text_XY(0, 270); 
  ER5517.Show_String( "CAL_Y");  
  ER5517.Goto_Text_XY(0, 294); 
  ER5517.Show_String( "CAL_S");         
    
  toHex(calx);
  ER5517.Goto_Text_XY(75, 246);  
  ER5517.Show_String(buf);  
  
  toHex(caly);
  ER5517.Goto_Text_XY(75, 270);  
  ER5517.Show_String(buf);     

  toHex(cals);
  ER5517.Goto_Text_XY(75, 294);  
  ER5517.Show_String(buf); 

  }
  
}

void fail()
{
  
  ER5517.Foreground_color_65k(Red);
  ER5517.Goto_Text_XY(0, 1); 
  ER5517.Show_String( "UTouch Calibration FAILED");   

  ER5517.Goto_Text_XY(0, 30); 
  ER5517.Show_String( "Unable to read the position");
  ER5517.Goto_Text_XY(0, 54); 
  ER5517.Show_String( "of the press. This is a");
  ER5517.Goto_Text_XY(0, 78); 
  ER5517.Show_String( "hardware issue and can");
  ER5517.Goto_Text_XY(0, 102); 
  ER5517.Show_String( "not be corrected in");
  ER5517.Goto_Text_XY(0, 126); 
  ER5517.Show_String( "software.");  
   
  while(true) {};
}


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

  dispx=LCD_XSIZE_TFT;
  dispy=LCD_YSIZE_TFT;
  text_y_center=(LCD_YSIZE_TFT/2);
  CENTER=(LCD_XSIZE_TFT/2);
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
  
  
   ER5517.Font_Select_12x24_24x24();

  
 startup();
 
  drawCrossHair(dispx-11,10,Blue); 
  drawCrossHair(dispx/2, 10,Blue);  
  drawCrossHair(10, 10,Blue);  
  drawCrossHair(dispx-11, dispy/2,Blue);   
  drawCrossHair(10, dispy/2,Blue);
  drawCrossHair(dispx-11, dispy-11,Blue);
  drawCrossHair(dispx/2, dispy-11,Blue);
  drawCrossHair(10, dispy-11,Blue);
  
   ER5517.Foreground_color_65k(White);
  ER5517.Background_color_65k(Black);
  ER5517.Goto_Text_XY(CENTER,text_y_center-16); 
  ER5517.Show_String( "***********");  
  ER5517.Goto_Text_XY(CENTER,text_y_center+16); 
  ER5517.Show_String( "***********");   


  calibrate(10, 10, 0);
  calibrate(10, dispy/2, 1);
  calibrate(10, dispy-11, 2);
  calibrate(dispx/2, 10, 3);
  calibrate(dispx/2, dispy-11, 4);
  calibrate(dispx-11, 10, 5);
  calibrate(dispx-11, dispy/2, 6);
  calibrate(dispx-11, dispy-11, 7);
  
  if (TOUCH_ORIENTATION == LANDSCAPE)
    cals=(long(dispx-1)<<12)+(dispy-1);
  else
    cals=(long(dispy-1)<<12)+(dispx-1);

  if (TOUCH_ORIENTATION == PORTRAIT)
    px = abs(((float(rx[2]+rx[4]+rx[7])/3)-(float(rx[0]+rx[3]+rx[5])/3))/(dispy-20));  // PORTRAIT
  else
    px = abs(((float(rx[5]+rx[6]+rx[7])/3)-(float(rx[0]+rx[1]+rx[2])/3))/(dispy-20));  // LANDSCAPE

  if (TOUCH_ORIENTATION == PORTRAIT)
  {
    clx = (((rx[0]+rx[3]+rx[5])/3));  // PORTRAIT
    crx = (((rx[2]+rx[4]+rx[7])/3));  // PORTRAIT
  }
  else
  {
    clx = (((rx[0]+rx[1]+rx[2])/3));  // LANDSCAPE
    crx = (((rx[5]+rx[6]+rx[7])/3));  // LANDSCAPE
  }
  if (clx<crx)
  {
    clx = clx - (px*10);
    crx = crx + (px*10);
  }
  else
  {
    clx = clx + (px*10);
    crx = crx - (px*10);
  }
  
  if (TOUCH_ORIENTATION == PORTRAIT)
    py = abs(((float(ry[5]+ry[6]+ry[7])/3)-(float(ry[0]+ry[1]+ry[2])/3))/(dispx-20));  // PORTRAIT
  else
    py = abs(((float(ry[0]+ry[3]+ry[5])/3)-(float(ry[2]+ry[4]+ry[7])/3))/(dispx-20));  // LANDSCAPE

  if (TOUCH_ORIENTATION == PORTRAIT)
  {
    cty = (((ry[5]+ry[6]+ry[7])/3));  // PORTRAIT
    cby = (((ry[0]+ry[1]+ry[2])/3));  // PORTRAIT
  }
  else
  {
    cty = (((ry[0]+ry[3]+ry[5])/3));  // LANDSCAPE
    cby = (((ry[2]+ry[4]+ry[7])/3));  // LANDSCAPE
  }
  if (cty<cby)
  {
    cty = cty - (py*10);
    cby = cby + (py*10);
  }
  else
  {
    cty = cty + (py*10);
    cby = cby - (py*10);
  }
  
  calx = (long(clx)<<14) + long(crx);
  caly = (long(cty)<<14) + long(cby);
  if (TOUCH_ORIENTATION == LANDSCAPE)
    cals = cals + (1L<<31);

  done();
  while(true) {}  
 
}
