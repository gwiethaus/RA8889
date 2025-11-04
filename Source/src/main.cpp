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
//https://www.youtube.com/watch?v=jAQiMWmSlIo 
//Basicamente é um outro tipo de memória flash.

//ESP32
//#define  PIN_RESET  16
//#define  PIN_CS     5

//ESP_32
#define  PIN_RESET      9  //Reset do RA8889
#define  PIN_RESET_CP   0  //Reset da Tela de toque não é conectado
#define  PIN_CS         10  
#define  PIN_INT        16 //interrupção da tela de toque 
#define  PIN_SDA        18 //I2C
#define  PIN_SCL        17 //I2C
#define  PIN_BLCONTROL  15 //External backlight control connected to this Arduino pin (quando nao estiver usando modulo display shield)

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


Bus_SPI busspi;
RA8889 gfx(PIN_CS, PIN_RESET);
FT touch(PIN_SDA, PIN_SCL, PIN_INT, PIN_RESET_CP);
TouchEventInfo* events;

void setup() {

  DEBUG_BEGIN(115200);

  //Para Arduino, placa de desenvolvimento shield, ER5517
  //Somente arduino
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);                       //Disable  SD 
  pinMode(2, OUTPUT);                        
  digitalWrite(2, HIGH);                       //Disable  RTP
  DEBUG_PRINT("Disable SD and RTP pin", 0,false,true);  

  IBus::SPIBusConfig_t cfg;
  cfg.spi_type = HOST_FSPI;
  cfg.pin_mosi = 11;
  cfg.pin_miso = 13;
  cfg.pin_sclk = 12;
  cfg.pin_cs   = 10;
  cfg.freq_write = 20000000;
  busspi.Config(&cfg);                         // Grava a configuração
  gfx.setBus(busspi);                          // Seta o Bus SPI
  DEBUG_PRINT("Bus SPI configurado", 0,false,true);

  //So irá funcionar se o pino do MCU tiver potencia suficiente apra manter o sinal a 3,3V
  //Se nao funcionar ligue diretamente aos 3,3V de alimentacao o pino 14 do BL_CONTROLE
  gfx.setBacklight(PIN_BLCONTROL);             //Controle de luz de fundo
  gfx.BacklightOn(true);                       //Liga luz de fundo
  DEBUG_PRINT("Backlight ON", 0, false, true);         //Debug

  bool b = gfx.Begin();                        //Inicializa o display
  DEBUG_PRINTD("Begin Sucessfull: ", b, true, 0, true);         //Debug
  gfx.FillScreen(clWhite);                     //Limpa a tela da ultima exibição apos power off
  gfx.DisplayOn(true);                         //esta funcao nao seria necessaria, pois init() já inicializa o display no modo grafico
  DEBUG_PRINTD("Start Display ON", 0, false, 1000, true);         //Debug
  
  touch.setDebounceTouch(false);
  touch.setTouchArea(gfx.Width(), gfx.Height(), false);
  touch.setDisplayArea(gfx.Width(), gfx.Height());
  touch.setTransitionTime(20);
  b = touch.Begin(FT_I2C_ADDRESS);
  if (!b) {
    DEBUG_PRINTD("Erro ao inicializar o touch! ", b, true, 0, true);
  }
  touch.EnableInterrupt(true);
  DEBUG_PRINTD("Habilitou interrupções de hardware", 0, false, 0, true);         //Debug

  touch.AllowMultitouch(true);
  touch.OnCallback(myInterrupt_cb);
  touch.CallbackEnable(true);
  DEBUG_PRINTD("Start Touch Display", 0, false, 0, true);         //Debug

  DEBUG_PRINT("",0,false,true);
  DEBUG_PRINT("----------------------------------------------------",0,false,true);
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
  DEBUG_PRINTD("----------------------------------------------------",0,false,1000,true);

  delay(2000);

  gfx.GraphicMode();
  gfx.setWindow(0,0, gfx.Width(), gfx.Height());  
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
