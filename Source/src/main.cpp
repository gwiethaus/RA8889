#include <Arduino.h>
#include <SPI.h>
#include <BusSPI.hpp>
#include "FT5316.hpp"
#include "userconf.hpp"
#include "RA8889.hpp"
#include "Debug.hpp"

#include <ascii_table_8x12.h>
#include <ascii_table_16x24.h>
#include <ascii_table_32x48.h>
#include <font8x16.h>

#include <esp32-hal-psram.h>
#include "esp_heap_caps.h"
#include "esp_system.h" //This inclusion configures the peripherals in the ESP system.

//https://www.youtube.com/watch?v=jAQiMWmSlIo 
//Basicamente é um outro tipo de memória flash.

//-----------------------------------------------------------------------------
//
// Pinos ESP32-S3
//
//-----------------------------------------------------------------------------

#define  PIN_RESET      9  //Reset do RA8889
#define  PIN_RESET_CP   0  //Reset da Tela de toque não é conectado
#define  PIN_CS         10  
#define  PIN_INT        16 //interrupção da tela de toque 
#define  PIN_SDA        18 //I2C
#define  PIN_SCL        17 //I2C
#define  PIN_BLCONTROL  15 //External backlight control connected to this Arduino pin (quando nao estiver usando modulo display shield)
#define  PIN_MOSI       11 
#define  PIN_MISO       13
#define  PIN_SCLK       12

/*
  ==TFT Hardware SPI to ESP32  WROOM 32, 38 pin ==
    TFT           =>    ESP32
  1,2. GND        ->    GND
  3,4. VCC        ->    5V      3.3V OR 5V is optional, depending on the voltage of the module purchased 
  5. CS           ->    GPIO05   vspi_cs
  6. MISO         ->    GPIO1919 vspi_miso
  7. MOSI         ->    GPIO23   vspi_mosi
  8. SCK          ->    GPIO18   vspi_clk
  11. RES         ->    GPIO16 

  ==TFT Hardware SPI to ESP32-S3 ==
    TFT           =>    ESP32-S3
  1,2. GND        ->    GND
  3,4. VCC        ->    5V      3.3V OR 5V is optional, depending on the voltage of the module purchased 
  5. CS           ->    GPIO10   vspi_cs
  6. MISO         ->    GPIO19   vspi_miso
  7. MOSI         ->    GPIO23   vspi_mosi
  8. SCK          ->    GPIO18   vspi_clk
  11. RES         ->    GPIO9    reset
  14. BL CONTROL  ->    GPIO46   
*/

//-----------------------------------------------------------------------------
//
//Configuração do RA8889 / FT5x16
//
//-----------------------------------------------------------------------------

Bus_SPI bus_spi;
RA8889 gfx(PIN_CS, PIN_RESET);
FT touch(PIN_SDA, PIN_SCL, PIN_INT, PIN_RESET_CP);
TouchEventInfo* events;


//-----------------------------------------------------------------------------
//
//Recursos do ESp32-S3 
//
//-----------------------------------------------------------------------------

uint8_t* psdRamBuffer = nullptr;
uint8_t *draw_buffer = nullptr;    //Memoria de alocacao na PSRAM Externa
//uint8_t *dma_buffer = nullptr;     //Memoria DMa alocada na area de 384 Kb Inerno para dados


//-----------------------------------------------------------------------------
//
//
//
//-----------------------------------------------------------------------------

#define TFT_WIDTH            800                                     //largura do display
#define TFT_HEIGHT           480                                     //comprimento da tela
#define BUFFER_PROPORTIONAL  5
#define BYTE_PER_PIXEL       2
#define DRAW_BUFFER          (TFT_WIDTH * TFT_HEIGHT * BYTE_PER_PIXEL)/ BUFFER_PROPORTIONAL

uint8_t draw_buffer2 [DRAW_BUFFER];

//-----------------------------------------------------------------------------
//
//
//
//-----------------------------------------------------------------------------

void myInterrupt_cb(TouchPoint tpoint, uint8_t idtouch, uint8_t ntouch) 
{
  DEBUG_PRINTD("<<<<<<< Touch ID", 0, false, 0, false);         //Debug
  DEBUG_PRINTD(" ", idtouch, true, 0, false);         //Debug
  DEBUG_PRINTD(" >>>>>>>", 0, false, 0, true);         //Debug
  
  if (tpoint.event == TouchEvent::Press)   Serial.println("Pressionou");
  if (tpoint.event == TouchEvent::Release) Serial.println("Soltou");
  if (tpoint.event == TouchEvent::Change)  Serial.println("Change/Move");
  if (tpoint.event == TouchEvent::Unknown) Serial.println("None");

  DEBUG_PRINTD(">>> Pos X ", tpoint.x, true, 0, true);         //Debug
  DEBUG_PRINTD(">>> Pos y ", tpoint.y, true, 0, true);         //Debug

  DEBUG_PRINTD("-------------------------------", 0, false, 0, true);         //Debug

}


void teste_PintarTela()
{
    // Conversão correta do buffer para 16 bits (cada pixel = 2 bytes)
    //uint16_t* buf16 = (uint16_t*) draw_buffer;
    uint8_t* buf8 = (uint8_t*) draw_buffer2;
	
    // Quantidade correta de pixels no buffer
    const uint32_t draw_pixels = DRAW_BUFFER / sizeof(uint16_t);

    // Preenche corretamente
    //for(uint32_t i = 0; i < draw_pixels; i++)
    //    buf16[i] = clPink;

    //DRAW_BUFFER tem 1/5 da tela
    uint16_t color = clYellow; 
    for (uint32_t i = 0; i < DRAW_BUFFER/2; i += 2) {
        buf8[i]     = (uint8_t)(color & 0xFF);
        buf8[i + 1] = (uint8_t)(color >> 8);
    }
    color = clGreen;
    for (uint32_t i = DRAW_BUFFER/2; i < DRAW_BUFFER; i += 2) {
        buf8[i]     = (uint8_t)(color & 0xFF);
        buf8[i + 1] = (uint8_t)(color >> 8);
    }

    gfx.setPixelPos(0,0);  

    uint32_t t0 = millis();
    uint8_t i = 0;
    while (i < 5) {
	  // Aqui WritePixels RECEBE ponteiro + quantidade em BYTES
      //gfx.WritePixels((void*)buf16, DRAW_BUFFER/2, true);
	    gfx.WritePixels(buf8, DRAW_BUFFER/2, true);
	  i++;
    }
	
    uint32_t t1 = millis();

    Serial.print("Tempo de processamento = ");
    Serial.print(t1 - t0);
    Serial.println(" ms");
}



void setup() {

 DEBUG_BEGIN(115200);

  DEBUG_PRINT("Support ESP32-S3 Information Start ......................................", 0,false,true);
  esp_chip_info_t info;
  esp_chip_info(&info);
  Serial.printf("Chip cores: %d, Model: %d, Features: %d\n", info.cores, info.model, info.features);
  Serial.printf("Heap Total:  %d\n", ESP.getHeapSize());
  Serial.printf("Heap Used:   %d\n", ESP.getHeapSize() - ESP.getFreeHeap());
  Serial.printf("Heap Free:   %d\n", ESP.getFreeHeap());
  Serial.printf("PSRAM Total: %d\n", ESP.getPsramSize());
  Serial.printf("PSRAM Init:  %s\n", psramInit() ? "true" : "false");

  IBus::SPIBusConfig_t cfg;
  cfg.spi_host = SPI2_HOST;
  cfg.pin_mosi = PIN_MOSI;
  cfg.pin_miso = PIN_MISO;
  cfg.pin_sclk = PIN_SCLK;
  cfg.pin_cs   = PIN_CS;
  cfg.freq_write = 40000000;                   //suporta 40MHz, mas original era 20MHz 
  bus_spi.Config(&cfg);                        // Grava a configuração
  gfx.setBus(bus_spi);                         // Seta o Bus SPI

  bool b = gfx.Begin();                        //Inicializa o display
  if (!b) return;
  
  gfx.FillScreen(clBlack);                     //Limpa a tela da ultima exibição apos power off
  gfx.DisplayOn(true);                         //esta funcao nao seria necessaria, pois init() já inicializa o display no modo grafico
  gfx.GraphicMode();
  gfx.setWindow(0, 0, gfx.Width(), gfx.Height());
  DEBUG_PRINT("Display RA8889 Initialized ........................................... OK", 0,false,true);

  gfx.useDMA(false);
  
  uint32_t t0 = millis();
  gfx.VSYNC_WaitReady();
  uint32_t t1 = millis();
  Serial.print("Tempo de espera VSYNC = ");
  Serial.print(t1 - t0);
  Serial.println(" ms");

  touch.setDebounceTouch(false);
  touch.setTouchArea(gfx.Width(), gfx.Height(), false);
  touch.setDisplayArea(gfx.Width(), gfx.Height());
  touch.setTransitionTime(20);
  touch.AllowMultitouch(true);
  //touch.OnCallback(myInterrupt_cb);
  touch.EnableInterrupt(true);
  touch.CallbackEnable(true);
  b = touch.Begin(FT_I2C_ADDRESS);
  if (b) {
    DEBUG_PRINT("Touch Driver FT5x16 Initialized ...................................... OK", 0, false, true);
  } else {
    DEBUG_PRINT("Touch Driver FT5x16 Initialized .................................... Fail", 0, false, true);
  }

  DEBUG_PRINT("",0,false,true);
  DEBUG_PRINT("Parameter Configuration Display .........................................",0,false,true);
  DEBUG_PRINT("Parameter",0,false,true);
  DEBUG_PRINT("Started Display       ", 0,false,true);
  DEBUG_PRINT("Display Width:        ", gfx.Width(),true,true);
  DEBUG_PRINT("Display Height:       ", gfx.Height(),true,true);
  DEBUG_PRINT("Color Deth (bpp):     ", gfx.getColorDepth(),true,true);
  DEBUG_PRINT("Layer Start Address:  ", gfx.LayerStartAddr(0),true,true);
  DEBUG_PRINT("Modo Grafico:         ", gfx.IsGraphicMode(),true,true);
  DEBUG_PRINT("MISO :                ", MISO,true,true);
  DEBUG_PRINT("MOSI :                ", MOSI,true,true);
  DEBUG_PRINT("SCK :                 ", SCK,true,true);
  DEBUG_PRINT("SS :                  ", SS,true,true);
  DEBUG_PRINT("HSPI :                ", HSPI,true,true);
  DEBUG_PRINT("FSPI :                ", FSPI,true,true);
  DEBUG_PRINTD(F("-------------------------------------------------------------------------"),0,false,1000,true);

  //dma_buffer  = (uint8_t*)heap_caps_malloc(DMA_BUFFER, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
  draw_buffer = (uint8_t*)heap_caps_malloc(DRAW_BUFFER, MALLOC_CAP_SPIRAM);
  Serial.printf("PSRAM Used:  %d\n", ESP.getPsramSize() - ESP.getFreePsram());
  Serial.printf("PSRAM Free:  %d\n", ESP.getFreePsram());

  if (draw_buffer == nullptr) {
    DEBUG_PRINT("Support PSRAM Buffer................................................ Fail", 0,false,true);
    return;
  } else {
    DEBUG_PRINT("Support PSRAM Buffer ................................................. OK", 0,false,true);
    memset(draw_buffer, 0x00, DRAW_BUFFER);                                  //Preenche a memroia com 0x00
  }

  Serial.println("Um teste antes enviando buffer de tela cheia de pixel Usando DMA ...");

  gfx.setWindow(0, 0, gfx.Width(), gfx.Height());
  gfx.setPixelPos(0,0);  
  


uint8_t* buf8 = (uint8_t*) draw_buffer;
uint32_t pixels = DRAW_BUFFER / 2;
uint16_t color = clRed;
for (uint32_t i = 0; i < DRAW_BUFFER/2; i += 2) {
    buf8[i]     = (uint8_t)(color & 0xFF);
    buf8[i + 1] = (uint8_t)(color >> 8);
}
color = clBlue;
for (uint32_t i = DRAW_BUFFER/2; i < DRAW_BUFFER; i += 2) {
    buf8[i]     = (uint8_t)(color & 0xFF);
    buf8[i + 1] = (uint8_t)(color >> 8);
}

uint32_t t00 = millis();
gfx.WritePixels(buf8, DRAW_BUFFER/2, false);
uint32_t t17 = millis();
Serial.print("Tempo de processamento = ");
Serial.print(t17 - t00);
Serial.println(" ms");
delay(4000);
gfx.FillScreen(clAshGray);
delay(2000);

  teste_PintarTela();
  
  delay(3000);

  gfx.GraphicMode();
  gfx.setWindow(0,0, gfx.Width(), gfx.Height());  
  gfx.FillScreen(clAshGray);
}


void loop() {

  //1️⃣ Atualiza todos os toques e processa eventos
  touch.Poll();

  //2️⃣ Percorre os eventos processados e trata manualmente se quiser
  uint8_t count = touch.getTouchCount();
  for (uint8_t i = 0; i < count; i++) {
    const TouchEventInfo &evt = touch.getTouch(i);

    Serial.print("Loop -> ID: "); Serial.print(evt.id);
    Serial.print(" X: "); Serial.print(evt.x);
    Serial.print(" Y: "); Serial.print(evt.y);
    Serial.print(" Tipo: ");

    switch (evt.transition) {
        case TOUCH_DOWN: Serial.println("DOWN"); break;
        case TOUCH_MOVE: Serial.println("MOVE"); break;
        case TOUCH_UP:   Serial.println("UP");   break;
        default:         Serial.println("NONE"); break;
    }
  }

  delay(50); // Pequeno delay para não inundar o Serial  
  
}
