/***************************************************
//Web: http://www.buydisplay.com
EastRising Technology Co.,LTD
Examples for ER-PCBA6105RA8889+ER-TFT070-2 Capacitive touch screen test
Display is Hardward SPI 4-Wire SPI Interface and 5V Power Supply,CTP is I2C interface.
Tested and worked with:
Works with Arduino 2.3.4 IDE  
NOTE: test OK:ESP32-WROOM-32 
****************************************************/

/*
  ==TFT Hardware SPI to ESP32  ==
    TFT   =>    ESP32
  *1,2. GND       ->    GND
  *3,4. VCC       ->    5V    3.3V OR 5V is optional, depending on the voltage of the module purchased 
  *5. CS        ->       5
  *6. MISO        ->    19  
  *7. MOSI       ->    23  
  *8. SCK       ->    18  
  *11. RES       ->    16   
  *33. CTPINT     ->    17
  *34. CTPSDA    ->    21
  *35. CTPSCL    ->    22 
  *36. CTPRST     ->    15
*/

#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include "RA8889.h"

uint8_t addr  = 0x38;  //CTP IIC ADDRESS

#define FT5316_RST 15
#define FT5316_INT   17  



enum {
  eNORMAL = 0,
  eTEST   = 0x04,
  eSYSTEM = 0x01
};


struct TouchLocation
{
  uint16_t x;
  uint16_t y;
};

TouchLocation touchLocations[5];

uint8_t readFT5316TouchRegister( uint8_t reg );
uint8_t readFT5316TouchLocation( TouchLocation * pLoc, uint8_t num );
uint8_t readFT5316TouchAddr( uint8_t regAddr, uint8_t * pBuf, uint8_t len );

uint32_t dist(const TouchLocation & loc);
uint32_t dist(const TouchLocation & loc1, const TouchLocation & loc2);

bool sameLoc( const TouchLocation & loc, const TouchLocation & loc2 );


uint8_t buf[30];

uint8_t readFT5316TouchRegister( uint8_t reg )
{
  Wire.beginTransmission(addr);
  Wire.write( reg );  // register 0
  uint8_t retVal = Wire.endTransmission();
  
  uint8_t returned = Wire.requestFrom(addr, uint8_t(1) );    // request 6 uint8_ts from slave device #2
  
  if (Wire.available())
  {
    retVal = Wire.read();
  }
  
  return retVal;
}

uint8_t readFT5316TouchAddr( uint8_t regAddr, uint8_t * pBuf, uint8_t len )
{
  Wire.beginTransmission(addr);
  Wire.write( regAddr );  // register 0
  uint8_t retVal = Wire.endTransmission();
  
  uint8_t returned = Wire.requestFrom(addr, len);    // request 1 bytes from slave device #2
  
  uint8_t i;
  for (i = 0; (i < len) && Wire.available(); i++)
  {
    pBuf[i] = Wire.read();
  }
  
  return i;
}

uint8_t readFT5316TouchLocation( TouchLocation * pLoc, uint8_t num )
{
  uint8_t retVal = 0;
  uint8_t i;
  uint8_t k;
  
  do
  {
    if (!pLoc) break; // must have a buffer
    if (!num)  break; // must be able to take at least one
    
    uint8_t status = readFT5316TouchRegister(2);
    
    static uint8_t tbuf[40];
    
    if ((status & 0x0f) == 0) break; // no points detected
    
    uint8_t hitPoints = status & 0x0f;
    
    Serial.print("number of hit points = ");
    Serial.println( hitPoints );
    
    readFT5316TouchAddr( 0x03, tbuf, hitPoints*6);
    
    for (k=0,i = 0; (i < hitPoints*6)&&(k < num); k++, i += 6)
    {
      pLoc[k].x = (tbuf[i+0] & 0x0f) << 8 | tbuf[i+1];
      pLoc[k].y = (tbuf[i+2] & 0x0f) << 8 | tbuf[i+3];
    }
    
    retVal = k;
    
  } while (0);
  
  return retVal;
}

void writeFT5316TouchRegister( uint8_t reg, uint8_t val)
{
  Wire.beginTransmission(addr);
  Wire.write( reg );  // register 0
  Wire.write( val );  // value
  
  uint8_t retVal = Wire.endTransmission();  
}

void readOriginValues(void)
{
  writeFT5316TouchRegister(0, eTEST);
  delay(1);
  //uint8_t originLength = readFT5316TouchAddr(0x08, buf, 8 );
  uint8_t originXH = readFT5316TouchRegister(0x08);
  uint8_t originXL = readFT5316TouchRegister(0x09);
  uint8_t originYH = readFT5316TouchRegister(0x0a);
  uint8_t originYL = readFT5316TouchRegister(0x0b);
  
  uint8_t widthXH  = readFT5316TouchRegister(0x0c);
  uint8_t widthXL  = readFT5316TouchRegister(0x0d);
  uint8_t widthYH  = readFT5316TouchRegister(0x0e);
  uint8_t widthYL  = readFT5316TouchRegister(0x0f);
  
  //if (originLength)
  {
    Serial.print("Origin X,Y = ");
    Serial.print( uint16_t((originXH<<8) | originXL) );
    Serial.print(", ");
    Serial.println( uint16_t((originYH<<8) | originYL) );
    
    Serial.print("Width X,Y = ");
    Serial.print( uint16_t((widthXH<<8) | widthXL) );
    Serial.print(", ");
    Serial.println( uint16_t((widthYH<<8) | widthYL) );
  }
  
}


uint32_t dist(const TouchLocation & loc)
{
  uint32_t retVal = 0;
  
  uint32_t x = loc.x;
  uint32_t y = loc.y;
  
  retVal = x*x + y*y;
  
  return retVal;
}

uint32_t dist(const TouchLocation & loc1, const TouchLocation & loc2)
{
  uint32_t retVal = 0;
  
  uint32_t x = loc1.x - loc2.x;
  uint32_t y = loc1.y - loc2.y;
  
  retVal = sqrt(x*x + y*y);
  
  return retVal;
}

bool sameLoc( const TouchLocation & loc, const TouchLocation & loc2 )
{
  return dist(loc,loc2) < 50;
}


void setup() {
  Serial.begin(9600);
  Wire.setClock(400000);    
  Wire.begin();        // join i2c bus (address optional for master)   
  pinMode     (FT5316_INT, INPUT);//CTP INT INPUT
  pinMode(FT5316_RST, OUTPUT);  //CTP RESET OUTPUT
  digitalWrite(FT5316_RST, HIGH);//on
  delay(10);
  digitalWrite(FT5316_RST, LOW);//on
  delay(10);
  digitalWrite(FT5316_RST, HIGH);//on
 
  
  ER_TFT.Parallel_Init();
  ER_TFT.HW_Reset();
  ER_TFT.System_Check_Temp();
  delay(100);
  while(ER_TFT.LCD_StatusRead()&0x02);
  ER_TFT.initial();
  ER_TFT.Display_ON();

}
void loop() {
   static uint16_t w = LCD_XSIZE_TFT;
  static uint16_t h = LCD_YSIZE_TFT; 
  uint8_t attention = digitalRead(FT5316_INT);
   unsigned int i;
   double float_data;  
  
  ER_TFT.Select_Main_Window_16bpp();
  ER_TFT.Main_Image_Start_Address(layer1_start_addr);        
  ER_TFT.Main_Image_Width(LCD_XSIZE_TFT);
  ER_TFT.Main_Window_Start_XY(0,0);
  ER_TFT.Canvas_Image_Start_address(0);
  ER_TFT.Canvas_image_width(LCD_XSIZE_TFT);
  ER_TFT.Active_Window_XY(0,0);
  ER_TFT.Active_Window_WH(LCD_XSIZE_TFT,LCD_YSIZE_TFT); 
  
  ER_TFT.Foreground_color_65k(Black);
  ER_TFT.Line_Start_XY(0,0);
  ER_TFT.Line_End_XY(LCD_XSIZE_TFT-1,LCD_YSIZE_TFT-1);
  ER_TFT.Start_Square_Fill(); 
  
   ER_TFT.Foreground_color_65k(White);
  ER_TFT.Background_color_65k(Black);
  ER_TFT.CGROM_Select_Internal_CGROM();  
  ER_TFT.Font_Select_12x24_24x24();
  ER_TFT.Goto_Text_XY(0,0); 
  ER_TFT.Show_String( "www.buydisplay.com");   
  ER_TFT.Goto_Text_XY(0,24); 
  ER_TFT.Show_String( "Capacitive touch screen tese(5 point touch)"); 
  ER_TFT.Goto_Text_XY(5,LCD_YSIZE_TFT-25); 
  ER_TFT.Show_String( "Clear");       
  
  while(1) 
  { attention = digitalRead(FT5316_INT);
     if (!attention) 
    {  
      Serial.println("Touch: ");
      
      uint8_t count = readFT5316TouchLocation( touchLocations, 5 );
                
        for (i = 0; i < count; i++)
        {
            
            if (((touchLocations[i].x)<50) &&((touchLocations[i].y)>LCD_YSIZE_TFT-30))
              {    ER_TFT.Foreground_color_65k(Black);
                  ER_TFT.Line_Start_XY(0,0);
                  ER_TFT.Line_End_XY(LCD_XSIZE_TFT-1,LCD_YSIZE_TFT-1);
                  ER_TFT.Start_Square_Fill(); 
                  
                   ER_TFT.Foreground_color_65k(White);
                  ER_TFT.Background_color_65k(Black);
                  ER_TFT.CGROM_Select_Internal_CGROM();  
                  ER_TFT.Font_Select_12x24_24x24();
                  ER_TFT.Goto_Text_XY(0,0); 
                  ER_TFT.Show_String( "www.buydisplay.com");   
                  ER_TFT.Goto_Text_XY(0,24); 
                  ER_TFT.Show_String( "Capacitive touch screen tese(5 point touch)");
                  ER_TFT.Goto_Text_XY(5,LCD_YSIZE_TFT-25); 
                  ER_TFT.Show_String( "Clear");                  
              }  
              
          else{                                       
              snprintf((char*)buf,sizeof(buf),"(%3d,%3d)",LCD_XSIZE_TFT-touchLocations[i].x,LCD_YSIZE_TFT-touchLocations[i].y); 
            const  char *str=(const char *)buf;
             ER_TFT.Foreground_color_65k(Red);  
            ER_TFT.Text_Mode();
            ER_TFT.Goto_Text_XY(50,80+24*i);
            ER_TFT.LCD_CmdWrite(0x04);
            while(*str != '\0')
            {
            ER_TFT.LCD_DataWrite(*str);
            ER_TFT.Check_Mem_WR_FIFO_not_Full();      
            ++str; 
            } 
            ER_TFT.Check_2D_Busy();
            ER_TFT.Graphic_Mode(); //back to graphic mode;
           
            if(i==0)  ER_TFT.DrawCircle_Fill(touchLocations[i].x,touchLocations[i].y, 3, Red);  
            else if(i==1)  ER_TFT.DrawCircle_Fill(touchLocations[i].x,touchLocations[i].y, 3, Green); 
            else if(i==2)  ER_TFT.DrawCircle_Fill(touchLocations[i].x,touchLocations[i].y, 3, Blue);        
            else if(i==3)  ER_TFT.DrawCircle_Fill(touchLocations[i].x,touchLocations[i].y, 3, Cyan); 
             else if(i==4)  ER_TFT.DrawCircle_Fill(touchLocations[i].x,touchLocations[i].y, 3, Yellow);     
          }
        }
     } 
    
  }
 
}



