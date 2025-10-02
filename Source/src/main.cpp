#include <Arduino.h>
#include <SPI.h>
#include "Panel_RA8889.hpp"
#include "FT5316.hpp"
#include "userconf.hpp"

//ESP32
//#define  PIN_RESET  16
//#define  PIN_CS     5

//ARDUINO
#define  PIN_RESET  9
#define  PIN_CS     10
#define  PIN_INT    00

uint16_t myColors[30] = {
    0xF800,
    0xF800,
    0xF800,
    0xF800,
    0xF800,
    0xF800,
    0xF800,
    0xF800,
    0xF800,
    0xF800,
    0xF800,
    clYellow,
    clYellow,
    clYellow,
    clYellow,
    clYellow,
    clYellow,
    clYellow,
    clYellow,
    clYellow,
    clYellow,
    0xF800,
    0xF800,
    0xF800,
    0xF800,
    0xF800,
    0xF800,
    0xF800,
    0xF800,
    0xF800
};

Panel_RA8889 gfx(PIN_CS, PIN_RESET);

void setup() {

  Serial.begin(9600);

  //Para Arduino, placa de desenvolvimento shield, ER5517
  //Somente arduino
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);                       //Disable  SD 
  pinMode(2, OUTPUT);                        
  digitalWrite(2, HIGH);                       //Disable  RTP
  
  bool b = gfx.Begin();
  gfx.DisplayOn(true);                         //esta funcao nao seria necessaria, pois init() já inicializa o display no modo grafico
  //gfx.DisplayTestBar(true);

  Serial.println("Iniciou Display");
  Serial.print("Width: ");
  Serial.println(gfx.Width());
  Serial.print("Heigth: ");
  Serial.println(gfx.Height());
  Serial.print("Color Deth (bpp): ");
  Serial.println(gfx.getColorDepth());
  delay(3000);
}


void loop() {

  gfx.GraphicMode();

  Serial.print ("Layer Start Address : "); Serial.println (gfx.LayerStartAddr(0));
  Serial.print ("Width : "); Serial.println (gfx.Width());
  Serial.print ("Heigth : "); Serial.println (gfx.Height());

  gfx.setWindow(0,0, gfx.Width(), gfx.Height());

  gfx.DrawSquare(0,0,gfx.Width()-1,gfx.Height()-1, clBlack, true);

  uint16_t w =  100 ;
  uint16_t h =  10 ; 
  for (uint16_t i = 0; i < 100; i++)
      gfx.DrawPixel(i, i+1,clRed);
  gfx.DrawPixel(0, 10,clRedFox);
  gfx.DrawPixel(20, 45,clBakersBrown);
  gfx.DrawPixel(200, 300,clYellowOrange);
  gfx.DrawPixel(100, 400,clRoseDust);

  gfx.DrawPixels(200,100,30,myColors);
  gfx.DrawPixels(200,200,30,myColors);
  gfx.DrawPixels(200,300,30,myColors);

  delay (5000);

  gfx.DrawSquare(0,0,gfx.Width()-1,gfx.Height()-1, clRed, true);
  delay(500);
  gfx.DrawSquare(0,0,gfx.Width()-1,gfx.Height()-1, clGreen, true);
  delay(500);
  gfx.DrawSquare(0,0,gfx.Width()-1,gfx.Height()-1, clBlue, true);
  delay(500);
  gfx.DrawSquare(0,0,gfx.Width()-1,gfx.Height()-1, clCyan, true);
  delay(500);
  gfx.DrawSquare(0,0,gfx.Width()-1,gfx.Height()-1, clYellow, true);
  delay(500); 
  gfx.DrawSquare(0,0,gfx.Width()-1,gfx.Height()-1, clPurple, true);
  delay(500);
  gfx.DrawSquare(0, 0, gfx.Width()-1, gfx.Height()-1, clPink, true);
  delay(500);
  gfx.DrawSquare(0, 0, gfx.Width()-1, gfx.Height()-1, clAmber, true);
  delay(500);

#ifdef ACTIVE6
  gfx.DrawSquare(0,0,gfx.Width()-1,gfx.Height()-1, clBlack, true);
  delay(500); 
  gfx.DrawSquare(0,0,gfx.Width()-1,gfx.Height()-1, clWhite, true);
  delay(500);

  gfx.DrawSquare(300,300,600,400, clSandyBrown, true);
  delay(500);
  gfx.DrawSquare(120,120,240,260, clRed);
  delay(500);
  gfx.DrawSquare(10,10,600,400, clGreen);
  delay(500);
  gfx.DrawSquare(100,100,400,200, clOlive);
  delay(500);
  gfx.DrawSquare(0,0,50,80, clBlue, true);

  delay(1000);
#endif

#ifdef ACTIVE7
  ////////////////Square
  
  uint8_t i = 0;
  gfx.FillScreen(clBlack);

  for(i=0;i<=gfx.Height()/2-10;i+=8)
  {
    gfx.DrawSquare(0+i, 0+i, gfx.Width()-1-i, gfx.Height()-1-i, clRed);
	  delay(100);
  }

  for(i=0;i<=gfx.Height()/2-10;i+=8) {
	  gfx.DrawSquare(0+i, 0+i, gfx.Width()-1-i, gfx.Height()-1-i, clBlack);
	  delay(100);
  }
  delay(100);

  ////////////////Square

  gfx.FillScreen(clBlack);

  for(i=0;i<=gfx.Height()/2-10;i+=8) {
    gfx.DrawCircleSquare(0+i, 0+i, gfx.Width()-1-i, gfx.Height()-1-i, 10, 10, clGreen);
	delay(10);
  }

  for(i=0;i<=gfx.Height()/2-10;i+=8) {
   	gfx.DrawCircleSquare(0+i, 0+i, gfx.Width()-1-i, gfx.Height()-1-i, 10, 10, clBlack);
    delay(10);
  }
  delay(100);
#endif

#ifdef ACTIVE8
  ///////////////////////////Circle
  
  gfx.GraphicMode();
  gfx.FillScreen(clBlack);

  for(i=0;i<=gfx.Height()/2-10;i+=8)
  {
	gfx.DrawCircle(gfx.Width()/2, gfx.Height()/2, i, clBlue);
    delay(10);
  }

  for(i=0;i<=gfx.Height()/2-10;i+=8) {
	gfx.DrawCircle(gfx.Width()/2, gfx.Height()/2, i, clBlack);
    delay(10);
  }
  delay(100);

  ///////////////////////////Ellipse

  gfx.FillScreen(clBlack);

  for(i=0;i<=gfx.Height()/2-10;i+=8) {
	gfx.DrawEllipse(gfx.Width()/2, gfx.Height()/2, i+100, i, clWhite);
    delay(10);
  }

  for(i=0;i<=gfx.Height()/2-10;i+=8) {
    gfx.DrawEllipse(gfx.Width()/2, gfx.Height()/2, i+100, i, clBlack);
	delay(10);
  }
  delay(100);
#endif
  
#ifdef ACTIVE9
 ////////////////////////////Triangle
  gfx.GraphicMode();
  gfx.FillScreen(clBlack);

  for(uint16_t i=0;i<=gfx.Height()/2-10;i+=8) {
    gfx.DrawTriangle(gfx.Width()/2, i, i, gfx.Height()-1-i, gfx.Width()-1-i, gfx.Height()-1-i, clYellow);
    delay(10);
  }

  for(uint16_t i=0;i<=gfx.Height()/2-10;i+=8) {
	  gfx.DrawTriangle(gfx.Width()/2, i, i, gfx.Height()-1-i, gfx.Width()-1-i, gfx.Height()-1-i, clBlack);
  }
  delay(100);
#endif

#ifdef ACTIVE10
 ////////////////////////////line
  gfx.GraphicMode();
  gfx.DrawLine(0, 0, gfx.Width()-1, gfx.Height()-1, clBlack);

  for(uint16_t i=0;i<gfx.Width();i+=8) {
	  gfx.DrawLine(i, 0, gfx.Width()-1-i, gfx.Height()-1, clRed);
	  delay(10);
  }
  
  for(uint16_t i=0;i<gfx.Height();i+=8) {
	  gfx.DrawLine(0, gfx.Height()-1-i, gfx.Width()-1, i, clRed);
	  delay(10);
  }

  for(uint16_t i=0;i<gfx.Width();i+=8) {
	  gfx.DrawLine(i, 0, gfx.Width()-1-i, gfx.Height()-1, clBlack);
	  delay(10);
  }

  for(uint16_t i=0;i<gfx.Height();i+=8) {
    gfx.DrawLine(0, gfx.Height()-1-i, gfx.Width()-1, i, clBlack);
	  delay(10);
  }

  delay(100);  
#endif

#ifdef ACTIVE11
/////////////Text  
  gfx.GraphicMode();
  gfx.setWindow(0, 0, gfx.Width()-1, gfx.Height()-1);
  gfx.DrawSquare(0, 0, gfx.Width()-1, gfx.Height()-1, clBlack, true);
  
  gfx.Text(0,26,"buydisplay.com",clGreen, clBlack);

  //colocar isso numa API
  //que define a lardura e espaçamento da fonte
  //gfx.Font_WidthEnlargFactor(FontEnlargFactor::X4); 
  //gfx.Font_HeightEnlargFactor(FontEnlargFactor::X4);

  gfx.Text(0,90,"buydisplay.com",clRed, clBlack);
  delay(2000); 
  
  //retorna ao modo normal
  //gfx.Font_WidthEnlargFactor(FontEnlargFactor::X1); 
  //gfx.Font_HeightEnlargFactor(FontEnlargFactor::X1);
#endif
  
#ifdef ACTIVE12
  gfx.GraphicMode();
  unsigned int temp;
  uint32_t im = 1;
  gfx.Select_MainWindow_16bpp();
  gfx.MainImage_StartAddress( gfx.LayerStartAddr(0) );  
  gfx.MainImage_Width( gfx.Width() );							
  gfx.MainWindow_StartXY(0,0);

  gfx.CanvasImage_StartAddr( gfx.LayerStartAddr(0) );
  gfx.CanvasImage_Width(gfx.Width());//
  gfx.ActiveWindow_XY(0,0);
  gfx.ActiveWindow_WidhtHeight( gfx.Width(), gfx.Height() );

  gfx.DrawSquare(0, 0, gfx.Width()-1, gfx.Height()-25, clBlack, true);

  gfx.DrawSquare(0, gfx.Height()-24, gfx.Width()-1, gfx.Height()-1, clBlue, true);
  
  gfx.ForegroundColor16bpp(clWhite);
  gfx.BackgroundColor16bpp(clBlue);
  gfx.Font_UseInternalCGROM();
  gfx.Font_SetHeight_24();
  gfx.GotoText_XY(0,gfx.Height()-24);
  gfx.ShowText("  Demo BTE Compare");
  gfx.ForegroundColor16bpp(clBlack);
  gfx.BackgroundColor16bpp(clWhite);
 
  gfx.GotoText_XY(0,gfx.Height()-48);
  gfx.ShowText("Execute Logic 'OR' 0xf000");

  gfx.DrawPicture(20, 40, 80, 80, pic_80x80);
  
  gfx.DrawPicture(20+80+20, 40, 80, 80, pic_80x80);
  
  gfx.DrawPicture(20+80+20+80+20, 40, 80, 80, pic_80x80);
  
  gfx.ActiveWindow_XY(0,0);
  gfx.ActiveWindow_WidhtHeight( gfx.Width(), gfx.Height() );
  gfx.ForegroundColor16bpp(clBlack);
  gfx.BackgroundColor16bpp(clWhite);
  gfx.Font_UseInternalCGROM();
 
  gfx.GotoText_XY(20,130 );
  gfx.ShowText("NO BTE");
  gfx.GotoText_XY(120,130 );
  gfx.ShowText("BTE ");
  gfx.GotoText_XY(120,154 );
  gfx.ShowText("ROP");
  gfx.GotoText_XY(220,130 );
  gfx.ShowText("BTE Move");
  gfx.GotoText_XY(220,154 );
  gfx.ShowText("ROP");
  delay(1000);
  gfx.ActiveWindow_XY(20,40);
  gfx.ActiveWindow_WidhtHeight(80,80); 
  gfx.GotoPixel_XY(20,40);
  gfx.SPI_CmdWrite(0x04);
  temp =  gfx.SPI_DataRead();
  gfx.Wait_ReadFIFO_NotEmpty();  //dummy
  for(i=0; i<80*80;i++)
  {				
    temp =  gfx.SPI_DataRead();		   
    temp = temp | (gfx.SPI_DataRead() << 8);
    gfx.Wait_ReadFIFO_NotEmpty();
    temp |= 0xf000; 
    gfx.SPI_DataWrite(temp);
    gfx.SPI_DataWrite(temp>>8);
    gfx.Wait_WriteFIFO_NotFull();
  }
  gfx.ActiveWindow_XY(0,0);
  gfx.ActiveWindow_WidhtHeight( gfx.Width(), gfx.Height() );
  delay(1000);
   //second block, MCU write with BTE ROP 
  gfx.BTE_S0_ColorDeph(eColorDepthBPP::bpp16);
  gfx.BTE_S1_ColorDeph_16bpp();
  gfx.BTE_S1_MemoryStartAddress(0);
  gfx.BTE_S1_ImageWidth(gfx.Width());
  gfx.BTE_S1_WindowStart_XY(120, 40);

  gfx.BTE_Destination_ColorDeph(eColorDepthBPP::bpp16);
  gfx.BTE_Destination_MemoryStartAddress(0);
  gfx.BTE_Destination_ImageWidth(gfx.Width());
  gfx.BTE_Destination_WindowStart_XY(120,40);  
  gfx.BTE_WindowSize(80,80);

  gfx.BTE_ROPCode(eROPCode::S0_Or_S1);
  gfx.BTE_OperationCode(eBTEOpCode::MPU_WROP); //BTE write
  gfx.BTE_Enable(true);

  gfx.SPI_CmdWrite(0x04);
  for(i=0; i<80*80;i++) {				
    gfx.SPI_DataWrite(0xf000);
    gfx.SPI_DataWrite(0xf000>>8);
    gfx.Wait_WriteFIFO_NotFull();
  }
  gfx.Wait_WriteFIFO_Empty();//糶Ч浪琩
  gfx.BTE_WaitReady();

  delay(1000);   
  
  //third block, BTE MOVE with ROP
  gfx.CanvasImage_StartAddr( gfx.LayerStartAddr(1) );
  gfx.CanvasImage_Width(gfx.Width());
  gfx.ActiveWindow_XY(0,0);
  gfx.ActiveWindow_WidhtHeight( gfx.Width(), gfx.Height() );

  gfx.BackgroundColor16bpp(clBlack);
  gfx.DrawSquare(0, 40, 80, 120, 0xf000, true);
  
  gfx.BTE_S0_ColorDeph(eColorDepthBPP::bpp16);
  gfx.BTE_S0_MemoryStartAddress( gfx.LayerStartAddr(1) );
  gfx.BTE_S0_ImageWidth(gfx.Width());
  gfx.BTE_S0_WindowStart_XY(0,40);

  gfx.BTE_S1_ColorDeph_16bpp();
  gfx.BTE_S1_MemoryStartAddress( gfx.LayerStartAddr(0) );
  gfx.BTE_S1_ImageWidth(gfx.Width());
  gfx.BTE_S1_WindowStart_XY(220,40);

  gfx.BTE_Destination_ColorDeph(eColorDepthBPP::bpp16);
  gfx.BTE_Destination_MemoryStartAddress( gfx.LayerStartAddr(0) );
  gfx.BTE_Destination_ImageWidth(gfx.Width());
  gfx.BTE_Destination_WindowStart_XY(220,40);  
  gfx.BTE_WindowSize(80, 80);

  gfx.BTE_ROPCode(eROPCode::S0_Or_S1);
  gfx.BTE_OperationCode(eBTEOpCode::MEM_COPY_POS_ROP); //BTE write
  gfx.BTE_Enable(true);
  gfx.BTE_WaitReady();
  delay(1000);  
  gfx.BTE_Enable(false);
  
 ///////////////////////// DMA
  gfx.Select_MainWindow_16bpp();
  gfx.MainImage_StartAddress( gfx.LayerStartAddr(0) );				
  gfx.MainImage_Width( gfx.Width() );
  gfx.MainWindow_StartXY(0,0);
  gfx.CanvasImage_StartAddr( gfx.LayerStartAddr(0) );
  gfx.CanvasImage_Width(gfx.Width());
  gfx.ActiveWindow_XY(0,0);
  gfx.ActiveWindow_WidhtHeight( gfx.Width(), gfx.Height() );	
  
  gfx.DrawSquare(0, 0, gfx.Width()-1, gfx.Height()-1, clBlack, true);  

 
  gfx.DMA_24bit_Block(1,1,0,0,gfx.Width(),gfx.Height(),gfx.Width(),0);
// Select SPI : SCS��0       SCS��1
// SPI Clock = System Clock /{(Clk+1)*2}
// Transfer to SDRAM address:X1
 // Transfer to SDRAM address:Y1
// DMA data width
// DMA data height
// Picture's width
// Flash address
  delay(2000);
  gfx.DMA_24bit_Block(1,1,0,0,gfx.Width(),gfx.Height(),gfx.Width(),768000);
  delay(2000);  
  gfx.DMA_24bit_Block(1,1,0,0,gfx.Width(),gfx.Height(),gfx.Width(),1536000);
  delay(2000); 
  
#endif    
}
