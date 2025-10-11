#include <Arduino.h>
#include <SPI.h>
#include "Panel_RA8889.hpp"
#include "FT5316.hpp"
#include "userconf.hpp"

#include <ascii_table_8x12.h>
#include <ascii_table_16x24.h>
#include <ascii_table_32x48.h>
#include <font8x16.h>
//https://www.youtube.com/watch?v=jAQiMWmSlIo 
//Basicamente é um outro tipo de memória flash.

//ESP32
//#define  PIN_RESET  16
//#define  PIN_CS     5

//ARDUINO
#define  PIN_RESET      9
#define  PIN_CS         10
#define  PIN_INT        00
#define  PIN_BLCONTROL  14 //External backlight control connected to this Arduino pin (quando nao estiver usando modulo display shield)

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


// --- conversão HSV → RGB565 ---
uint16_t hsvToRgb565(float h, float s, float v) {
  float r, g, b;
  int i = int(h * 6);
  float f = h * 6 - i;
  float p = v * (1 - s);
  float q = v * (1 - f * s);
  float t = v * (1 - (1 - f) * s);

  switch (i % 6) {
    case 0: r = v; g = t; b = p; break;
    case 1: r = q; g = v; b = p; break;
    case 2: r = p; g = v; b = t; break;
    case 3: r = p; g = q; b = v; break;
    case 4: r = t; g = p; b = v; break;
    case 5: r = v; g = p; b = q; break;
  }

  uint16_t R = (uint16_t)(r * 31);
  uint16_t G = (uint16_t)(g * 63);
  uint16_t B = (uint16_t)(b * 31);
  return (R << 11) | (G << 5) | B;
}

// --- função para aplicar fade em um pixel RGB565 ---
uint16_t fadePixel(uint16_t color, float factor) {
  uint16_t r = (color >> 11) & 0x1F;
  uint16_t g = (color >> 5) & 0x3F;
  uint16_t b = color & 0x1F;

  r = max(0, (int)(r * factor));
  g = max(0, (int)(g * factor));
  b = max(0, (int)(b * factor));

  return (r << 11) | (g << 5) | b;
}


Panel_RA8889 gfx(PIN_CS, PIN_RESET);

void setup() {

  DEBUG_BEGIN(115200);

  //Para Arduino, placa de desenvolvimento shield, ER5517
  //Somente arduino
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);                       //Disable  SD 
  pinMode(2, OUTPUT);                        
  digitalWrite(2, HIGH);                       //Disable  RTP
  DEBUG_PRINT("Disable SD and RTP pin", 0,false,true);  
  bool b = gfx.Begin();
  DEBUG_PRINTD("Begin Sucessfull", 0, false, 4000, true);         //Debug
  DEBUG_PRINTD("",0,false,0,true);  

  gfx.FillScreen(clWhite);                     //Limpa a tela da ultima exibição apos power off

  gfx.DisplayOn(true);                         //esta funcao nao seria necessaria, pois init() já inicializa o display no modo grafico
  DEBUG_PRINTD("Start Display ON", 0, false, 2000, true);         //Debug
  DEBUG_PRINTD("",0,false,0,true);  

  DEBUG_PRINT("",0,false,true);
  DEBUG_PRINT("----------------------------------------------------",0,false,true);
  DEBUG_PRINT("Parameter",0,false,true);
  DEBUG_PRINT("Started Display       ", 0,false,true);
  DEBUG_PRINT("Display Width:        ", gfx.Width(),true,true);
  DEBUG_PRINT("Display Height:       ", gfx.Height(),true,true);
  DEBUG_PRINT("Color Deth (bpp):     ", gfx.getColorDepth(),true,true);
  DEBUG_PRINT("Layer Start Address : ", gfx.LayerStartAddr(0),true,true);
  DEBUG_PRINTD("----------------------------------------------------",0,false,3000,true);


  randomSeed(analogRead(0)); //para animacao dos pixel - wilson explicou

  delay(3000);
}


void loop() {
#ifdef ACTIVE0
  gfx.GraphicMode();
  gfx.setWindow(0,0, gfx.Width(), gfx.Height());
#endif

//posicao do texto e espacos  
  uint8_t index_line = 0;
  uint8_t space = 4;  

#ifdef ACTIVE1
//---------------------------------------------------------------------------
//Exemplo de Desenhos com pixel
//---------------------------------------------------------------------------
  gfx.DrawSquare(0,0,gfx.Width()-1,gfx.Height()-1, clBlack, true);

  uint16_t w =  100 ;
  uint16_t h =  10 ; 
  for (uint16_t i = 0; i < 10; i++)
      gfx.DrawPixel(i, i+1,clRed);
  gfx.DrawPixel(0, 10,clRedFox);
  gfx.DrawPixel(20, 45,clBakersBrown);
  gfx.DrawPixel(200, 300,clYellowOrange);
  gfx.DrawPixel(100, 400,clRoseDust);

  gfx.DrawPixels(200,100,30,myColors);
  gfx.DrawPixels(200,200,30,myColors);
  gfx.DrawPixels(200,300,30,myColors);

  delay (2000);
#endif

#ifdef ACTIVE2
//---------------------------------------------------------------------------
//Exemplo de animação Pixel Aleatorios
//---------------------------------------------------------------------------

  gfx.FillScreen(clBlack);

  for (int i = 0; i < 10000; i++) {
    int x = random(0, gfx.Width());
    int y = random(0, gfx.Height());
    uint16_t color = ((random(0, 32) << 11) | (random(0, 64) << 5) | random(0, 32));
    gfx.DrawPixel(x, y, color);
  }

  delay (2000);

#endif

#ifdef ACTIVE3
//---------------------------------------------------------------------------
//Exemplo de animação Pixel
//---------------------------------------------------------------------------
  gfx.FillScreen(clBlack);

  static float hueShift = 0.0;
  uint32_t k = 0;

  while (k < 10){
    for (int i = 0; i < 500; i++) {
      int x = random(0, gfx.Width());
      int y = random(0, gfx.Height());
  
      // gera cor com matiz variando no tempo
      float hue = fmod((float)x / gfx.Width() + hueShift, 1.0);
      uint16_t color = hsvToRgb565(hue, 1.0, 1.0);
  
      gfx.DrawPixel(x, y, color);
    }
  
    hueShift += 0.01;        // velocidade da transição de cor
    if (hueShift > 1.0) hueShift -= 1.0;
  
    // opcional: pequeno delay pra desacelerar o efeito
    delay(20);
    k++;
  } 

  delay(2000);
#endif

#ifdef ACTIVE4
//---------------------------------------------------------------------------
//Exemplo de animação Pixel
//---------------------------------------------------------------------------

  gfx.FillScreen(clBlack);

  static float hueShift2 = 0.0;
  const float fadeFactor = 0.9; // 90% do brilho a cada quadro
  uint8_t k2 = 0;

  while (k2 < 5){
  // --- fade gradual ---  
  for (int x = 0; x < gfx.Width(); x += 5) {     // pula alguns pixels pra acelerar
    for (int y = 0; y < gfx.Height(); y += 5) {
      uint16_t oldColor = gfx.getPixel(x, y);       // se não tiver, pode usar buffer próprio
      uint16_t newColor = fadePixel(oldColor, fadeFactor);
      gfx.DrawPixel(x, y, newColor);
    }
  }

  // --- desenha pixels arco-íris aleatórios ---
  for (int i = 0; i < 500; i++) {
    int x = random(0, gfx.Width());
    int y = random(0, gfx.Height());

    float hue = fmod((float)x / gfx.Width() + hueShift2, 1.0);
    uint16_t color = hsvToRgb565(hue, 1.0, 1.0);
    gfx.DrawPixel(x, y, color);
  }

  hueShift2 += 0.01;
  if (hueShift2 > 1.0) hueShift2 -= 1.0;
    k2++;
  } 

#endif

#ifdef ACTIVE5
//---------------------------------------------------------------------------
//Exemplo de Teste de getPixel
//---------------------------------------------------------------------------
  gfx.FillScreen(clBlack);
  uint16_t x = 100;
  uint16_t y = 100;

  gfx.DrawPixel(x, y, clYellow);
  DEBUG_PRINTH("Cor enviada: ", clYellow, true,true);
  DEBUG_PRINTD("------------------------------------------", 0, false, 1000, true);
  uint16_t oldColor = gfx.getPixel(x, y);       // se não tiver, pode usar buffer próprio
  DEBUG_PRINTH("Cor Recebida: ", oldColor, true,true);
  DEBUG_PRINTD("------------------------------------------", 0, false, 2000, true);
  uint16_t oldColor2 = gfx.getPixel(x+1, y+1);       // se não tiver, pode usar buffer próprio
  DEBUG_PRINTH("Cor Recebida posicao (+1,+1): ", oldColor2, true,true);
  DEBUG_PRINTD("------------------------------------------", 0, false, 2000, true);

#endif


#ifdef ACTIVE10
//---------------------------------------------------------------------------
//Exemplo de Preenchimento de Tela
//---------------------------------------------------------------------------
  gfx.DisplayTestBar(true);
  delay(2000);
  gfx.DisplayTestBar(false);

  gfx.DrawSquare(0,0,gfx.Width()-1,gfx.Height()-1, clRed, true);
  delay(500);
  gfx.DrawSquare(0,0,gfx.Width()-1,gfx.Height()-1, clGreen, true);
  delay(500);
  gfx.DrawSquare(0,0,gfx.Width()-1,gfx.Height()-1, clBlue, true);
  delay(500);
#endif

#ifdef ACTIVE11
//---------------------------------------------------------------------------
//Exemplo de Preenchimento de Tela
//---------------------------------------------------------------------------
  gfx.DrawSquare(0,0,gfx.Width()-1,gfx.Height()-1, clYellow, true);
  delay(500); 
  gfx.DrawSquare(0,0,gfx.Width()-1,gfx.Height()-1, clPurple, true);
  delay(500);
  gfx.DrawSquare(0, 0, gfx.Width()-1, gfx.Height()-1, clPink, true);
  delay(500);
  gfx.DrawSquare(0, 0, gfx.Width()-1, gfx.Height()-1, clAmber, true);
  delay(500);
#endif

#ifdef ACTIVE12
//---------------------------------------------------------------------------
//Exemplo de Preenchimento de Tela
//---------------------------------------------------------------------------
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

#ifdef ACTIVE13
//---------------------------------------------------------------------------
//Exemplo de Quadrados
//---------------------------------------------------------------------------
  
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

//---------------------------------------------------------------------------
//Exemplo de Quadrados
//---------------------------------------------------------------------------

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

#ifdef ACTIVE14
//---------------------------------------------------------------------------
//Exemplo de Animação de Circulos
//---------------------------------------------------------------------------
  
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

//---------------------------------------------------------------------------
//Exemplo de Animação de Elipse
//---------------------------------------------------------------------------

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
  
#ifdef ACTIVE15
//---------------------------------------------------------------------------
//Exemplo de Animação de Traingulos
//---------------------------------------------------------------------------

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

#ifdef ACTIVE16
//---------------------------------------------------------------------------
//Exemplo de Linha
//---------------------------------------------------------------------------

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

#ifdef ACTIVE17
//---------------------------------------------------------------------------
//Exemplo de Teexto
//---------------------------------------------------------------------------

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

#ifdef ACTIVE18
//---------------------------------------------------------------------------
//Exemplo de Fonte de caraceres interno do RA8889
//---------------------------------------------------------------------------

  gfx.FillScreen(clBlack);
  gfx.TextMode();
  
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

#ifdef ACTIVE19
//---------------------------------------------------------------------------
//Exemplo de Fonte de caraceres externo do Getitop's CGROM
//---------------------------------------------------------------------------

  //Teste de caracteres externo
  
  FontExternalParam fntexter;
  
  fntexter.charset_select = eExternalCharSet::ISO8859_1;
  fntexter.gt_width = eExternalCharWidthSet::VariableArial;
  fntexter.scs_select = 0;
  gfx.setFontExternal(fntexter, true);
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
  delay(4000);

#endif

#ifdef ACTIVE20
//---------------------------------------------------------------------------
//Exemplo de Unicode String
//---------------------------------------------------------------------------

  FontExternalParam fntexter2;
  
  fntexter2.charset_select = eExternalCharSet::UNICODE;
  fntexter2.gt_width = eExternalCharWidthSet::Bold;
  fntexter2.scs_select = 0;
  gfx.setFontExternal(fntexter2, false);
  gfx.setFontSource(eFontSource::ExternalCGROM);

  index_line++;
  //Nao esta funcionando
  gfx.PutUnicodeString(0,(index_line*24)+space,clYellowOrange, clBlue, (wchar_t)L"1234567890");

  delay(4000);
#endif  

#ifdef ACTIVE21
//---------------------------------------------------------------------------
//Exemplo de Fonte de usuario - Não está pronto
//---------------------------------------------------------------------------
  FontUserParam fntuser;
  
  //teste de caracteres de usuário
  
  fntuser.font_table = (char *)font8x16;
  gfx.setFontUser(fntuser, false);
  gfx.setFontSource(eFontSource::UserDefined);
  
  
  delay(8000);
#endif  


#ifdef ACTIVE22
  index_line = 0;

  gfx.FillScreen(clBlue);
  gfx.TextMode();
  gfx.Text(0, (index_line*24)+space,"Em 5s, Tela desliga",clYellow,clBlue);  
  delay(2000);

  gfx.Backlight(false);

  delay (2000);

  gfx.Backlight(true);
  gfx.FillScreen(clYellow);
  gfx.Text(0, (index_line*24)+space,"Tela ligou",clBlack,clYellow); 
  delay (5000);
#endif

#ifdef ACTIVE23


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

#ifdef ACTIVE24
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
