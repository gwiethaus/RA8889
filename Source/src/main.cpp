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

  DEBUG_BEGIN(115200);

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

void func(float x) {
  Serial.println(x,5);
}

void loop() {

  gfx.GraphicMode();

  Serial.print ("Layer Start Address : "); Serial.println (gfx.LayerStartAddr(0));
  Serial.print ("Width : "); Serial.println (gfx.Width());
  Serial.print ("Heigth : "); Serial.println (gfx.Height());

  gfx.setWindow(0,0, gfx.Width(), gfx.Height());

#ifdef ACTIVE1
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
#endif

#ifdef ACTIVE2
  gfx.DrawSquare(0,0,gfx.Width()-1,gfx.Height()-1, clRed, true);
  delay(500);
  gfx.DrawSquare(0,0,gfx.Width()-1,gfx.Height()-1, clGreen, true);
  delay(500);
  gfx.DrawSquare(0,0,gfx.Width()-1,gfx.Height()-1, clBlue, true);
  delay(500);
  gfx.DrawSquare(0,0,gfx.Width()-1,gfx.Height()-1, clCyan, true);
  delay(500);
#endif

#ifdef ACTIVE3
  gfx.DrawSquare(0,0,gfx.Width()-1,gfx.Height()-1, clYellow, true);
  delay(500); 
  gfx.DrawSquare(0,0,gfx.Width()-1,gfx.Height()-1, clPurple, true);
  delay(500);
  gfx.DrawSquare(0, 0, gfx.Width()-1, gfx.Height()-1, clPink, true);
  delay(500);
  gfx.DrawSquare(0, 0, gfx.Width()-1, gfx.Height()-1, clAmber, true);
  delay(500);
#endif

#ifdef ACTIVE5
  gfx.DrawSquare(0,0,gfx.Width()-1,gfx.Height()-1, clBlack, true);
  delay(250); 
  gfx.DrawSquare(0,0,gfx.Width()-1,gfx.Height()-1, clWhite, true);
  delay(250);

  gfx.DrawSquare(300,300,600,400, clSandyBrown, true);
  delay(250);
  gfx.DrawSquare(120,120,240,260, clRed);
  delay(250);
  gfx.DrawSquare(10,10,600,400, clGreen);
  delay(250);
  gfx.DrawSquare(100,100,400,200, clOlive);
  delay(250);
  gfx.DrawSquare(0,0,50,80, clBlue, true);

  delay(1000);
#endif

#ifdef ACTIVE6
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

#ifdef ACTIVE7
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
  
#ifdef ACTIVE8
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

#ifdef ACTIVE9
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

#ifdef ACTIVE10
/////////////Text  
  gfx.FillScreen(clBlack);
  gfx.TextMode();
    
  gfx.Text(0,26,"buydisplay.com",clGreen, clBlack);

  //colocar isso numa API
  //que define a lardura e espaçamento da fonte
  //gfx.Font_WidthEnlargFactor(FontEnlargFactor::X4); 
  //gfx.Font_HeightEnlargFactor(FontEnlargFactor::X4);

  gfx.Text(0,90,"buydisplay.com",clRed, clBlack);
  delay(5000); 
  
  //retorna ao modo normal
  //gfx.Font_WidthEnlargFactor(FontEnlargFactor::X1); 
  //gfx.Font_HeightEnlargFactor(FontEnlargFactor::X1);
#endif

#ifdef ACTIVE11
  gfx.FillScreen(clBlack);
  gfx.TextMode();

  uint8_t index_line = 0;
  uint8_t space = 4;
  
  FontInternalParam fntinter;
  
  //Testes de Fontes Interna
  
  fntinter.charset_select = eInternalCharSet::ISO8859_1;
  fntinter.full_align = false;
  fntinter.chroma_key = false;
  fntinter.width_enlarge = eFontEnlargFactor::X1;
  fntinter.height_enlarge = eFontEnlargFactor::X1;
  gfx.setFontInternal(fntinter, true);
  
  gfx.Text(0, (index_line*24)+space, "buydisplay.com",clBlue, clBlack);

  delay(2000);
#endif

#ifdef ACTIVE12  
  //Teste de caracteres externo
  
  FontExternalParam fntexter;
  
  fntexter.charset_select = eExternalCharSet::ISO8859_1;
  fntexter.gt_width = eExternalCharWidthSet::VariableArial;
  fntexter.scs_select = 0;
  gfx.setFontExternal(fntexter, false);
  gfx.setFontSource(eFontSource::ExternalCGROM);
  
  index_line++;
  gfx.Text(0, (index_line*24)+space,"raio.tw",clRed,clBlack);
  
  index_line++;
  gfx.PutHexa(0, (index_line*24)+space, 255, "%X");

  index_line++;
  gfx.PutDecimal(0, (index_line*24)+space, 255, "%-5d");

  index_line++;
  index_line++;
  gfx.Text(0, (index_line*24)+space,"Teste de Ponto flutuante",clYellow,clBlack);
  
  index_line++;
  gfx.PutFloat(0, (index_line*24)+space, 3.1416, "%6.3f"); //versão 2 esta funcionando bem este
  
  index_line++;
  gfx.PutFloat(0, (index_line*24)+space, 3.1415, "%06.4f"); 
  
  index_line++;
  gfx.PutFloat(0, (index_line*24)+space, -3.14, "%+05.2f");  
    
  index_line++;
  gfx.PutFloat(0, (index_line*24)+space, 0.000123, "%10.3e");

  index_line++;
  gfx.PutFloat(0, (index_line*24)+space, -3.14, "%+05.2f");

  index_line++;
  gfx.PutFloat(0, (index_line*24)+space, -3.14, "%5.2f");

  index_line++;
  gfx.PutFloat(0, (index_line*24)+space, -3.14,  "%+08.2f");

  index_line++;
  gfx.PutFloat(0, (index_line*24)+space, 12345678.9, "%+12.2g");

  //Teste de caracteres externo UNICODE (2 bytes)
  //Verificar se como são 2 bytes a funcao, nao precise fazer ajuste interno do Text()
  //para exibir corretamente a função
/*  
  fntexter.charset_select = eExternalCharSet::ISO8859_1;
  fntexter.gt_width = eExternalCharWidthSet::Fixed;
  fntexter.scs_select = 0;
  gfx.setFontExternal(fntexter);
  gfx.setFontSource(eFontSource::ExternalCGROM);
  
  index_line++;
  gfx.Text(0, (index_line*16)+space,"DelWie Labs",clPink, clBlack);
*/
  //teste de caracteres de usuário
  
  
  
  //setFontSource(eFontOrigin::User);
  
  delay(8000);
#endif  

#ifdef ACTIVE13

  //ra8889lite.pwm_Configuration(RA8889_PWM_TIMER1_INVERTER_ON,RA8889_PWM_TIMER1_AUTO_RELOAD,RA8889_PWM_TIMER1_START,RA8889_PWM_TIMER0_DEAD_ZONE_DISABLE 
  //                    , RA8889_PWM_TIMER0_INVERTER_ON, RA8889_PWM_TIMER0_AUTO_RELOAD,RA8889_PWM_TIMER0_START);                     

  //preescaler : if core_freq = 120MHz, pwm base clock = 120/(3+1) = 30MHz
  //clockdivider:  pwm timer clock = 30/4 = 7.5MHz
  // pwm0 = 7.5MHz/1024 = 7.3KHz
  //pwm0 set 10/1024 duty
  gfx.PWM0(true, eDividerClock::X4, 0x03,  1024, 10);

  //preescaler : if core_freq = 120MHz, pwm base clock = 120/(3+1) = 30MHz
  //clockdivider:  pwm timer clock = 30/4 = 7.5MHz
  // pwm1 = 7.5MHz/256 = 29.2KHz
  //pwm1 set 5/256 duty
  gfx.PWM1(true, eDividerClock::X4, 0x03,  256, 5);

#endif

#ifdef ACTIVE14
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
  
  gfx.Font_UseInternalCGROM();
  gfx.Font_SetHeight_24();
  gfx.Text(0,gfx.Height()-24-1,"Demo BTE Compare",clBlack, clWhite);
  gfx.Text(0,gfx.Height()--48-1,"Execute Logic 'OR' 0xf000",clBlack, clWhite); 

  gfx.DrawPicture(20, 40, 80, 80, pic_80x80);
  
  gfx.DrawPicture(20+80+20, 40, 80, 80, pic_80x80);
  
  gfx.DrawPicture(20+80+20+80+20, 40, 80, 80, pic_80x80);
    
  gfx.setWindow(0,0, gfx.Width(), gfx.Height());

  gfx.Font_UseInternalCGROM();
  gfx.Text(0,130,"NO BTE",clBlack, clWhite); 
  gfx.Text(120,130,"BTE ",clBlack, clWhite); 
  gfx.Text(120,154,"ROP ",clBlack, clWhite); 
  gfx.Text(220,130,"BTE Move",clBlack, clWhite); 
  gfx.Text(220,154,"ROP",clBlack, clWhite); 
  delay(1000);

  gfx.setWindow(20,40,80,80);

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
  
  gfx.setWindow(0,0, gfx.Width(), gfx.Height());
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
