#ifndef RA8889_HPP
#define RA8889_HPP
#include <Arduino.h>
#include <SPI.h>
#include "DisplayBase.hpp"
#include "RA8889_Reg.hpp"


//--------------------------------------------------------------------------------
// Set/Reset bits mask
//--------------------------------------------------------------------------------


//uso para Set bits (or apply)
#define cSetb0    0x01
#define cSetb1    0x02
#define cSetb2    0x04
#define cSetb3    0x08
#define cSetb4    0x10
#define cSetb5    0x20
#define cSetb6    0x40
#define cSetb7    0x80


//Uso para Clear bits (and apply)
#define cClrb0    0xfe
#define cClrb1    0xfd
#define cClrb2    0xfb
#define cClrb3    0xf7
#define cClrb4    0xef
#define cClrb5    0xdf
#define cClrb6    0xbf
#define cClrb7    0x7f

//--------------------------------------------------------------------------------
//
// Manipulação de bits em variáveis
//
//--------------------------------------------------------------------------------

/* Set bit */
#define SETB(var, bit)   ((var) |=  (1U << (bit)))

/* Clear bit */
#define CLRB(var, bit)   ((var) &= ~(1U << (bit)))

/* Togle bit */
#define TGLB(var, bit)   ((var) ^=  (1U << (bit)))  

/* Check bit */
#define CHKB(var, bit)   (((var) >> (bit)) & 1U)    

/* Retorna a máscara inteira se o bit estiver setado, 0 caso contrário */
#define CHKMSKB(var, bit)  ((var) & (1U << (bit)))  


#define WRTB(var, bit, val) \
    ((val) ? ((var) |=  (1U << (bit))) : ((var) &= ~(1U << (bit))))
	
/* Mascara para limpar e mascara para setar, se nao for usar o set/clear use 0 
   Escreve múltiplos bits de uma vez: limpa bits da clear_mask, seta bits da set_mask
   ex. MSKB(temp,1U<<5|1U<<4,1U<<1);
*/
#define MSKB(var, clear_mask, set_mask) \
    ((var) = ((var) & ~(clear_mask)) | (set_mask))


//--------------------------------------------------------------------------------
//
// RA8889 Frequency Configuration PLL
//
//--------------------------------------------------------------------------------


//Notes
//  1. Crystal resonator for RA8889, suggested 10MHz
//  2. SDRAM Access Clock, suggested 50~160MHz
//  3. RA8889 System Core Clock, suggested 50~130MHz
//  4. TFT Driving Clock PCLK, refer to LCD SPEC specified PCLK frequency requirements 
//  5. DRAM_FREQ >= CORE_FREQ   
//  6. CORE_FREQ >= (2 * SCAN_FREQ)

#define OSC_FREQ     10	                       // Crystal clock, unti: MHz
#define DRAM_FREQ    133                       // SDRAM clock frequency, unti: MHz	(valor de 140, funciona bem)	  
#define CORE_FREQ    120                       // Core (system) clock frequency, unit: MHz 
#define SCAN_FREQ    34                        // PSCK Panel Scan clock frequency, unit: MHz


//--------------------------------------------------------------------------------
//
// Panel LCD
//
//--------------------------------------------------------------------------------


//==== Valores tipicos do fabricante da tela de display LCD =====
//O RA8889 usa os valores adequados apra envio ao display LCD par a exibição correta das imagam
//Cada fabricante de tela possui as especificações e caracteristicas configuradas nesta constantes abaixo
//Consulte o Datasheet do fabricante de tela para entender estes valores 
//Exemplo de alguns Fabricnates
//https://www.twscreen.com/index.php/lcdpanel/search?brand=19&size=3

//===== LCD screen model =====
//#define ILI6122                                //ILITEK ILI6122 800x480 7.0" TFT-LCD, https://www.buydisplay.com/7-tft-lcd-touch-screen-display-module-800x480-for-mp4-gps-tablet-pc
//#define ST7277                                 //Sitronix ST7277 800x480 7.0" TFT-LCD, IPS, https://www.buydisplay.com/wide-angle-7-inch-800x480-color-ips-tft-display-st7277-controller
//#define HX8282                                 //HiMAX HX8282
//#define HX8264                                 //HiMAX HX8264 800x480 7.0"
//#define HX8664                                 //HiMAX HX8664 800x480 7.0"
//#define AWY_800480T70N02                       //AWY_800480T70N02
//#define LQ035NC111                             //Innolux LQ035NC111 320x240
//#define AWT_1024600L7N03                       //AWT_1024600L7N03
#define EK9713                                 //Fitipower EK9713 800x600/800x480, https://www.buydisplay.com/7-inch-lcd-screen-tft-display-module-wvga-800x480-at070tn90-at070tn92
//#define EK7330                                 //Fitipower EK7330 800x480 7.0" TFT
//#define EK73002                                //Fitipower EK73002
//#define L80480R70		                         //L80480R70 800x480
//#define AT070TN90                              //Innolux AT070TN90
//#define AT070TN92                              //Innolux AT070TN92 800X480 7"
//#define AT070TN94                              //Innolux AT070TN94
//#define AT070TN92_V1_TP                        //Innolux AT070TN92 V.1 TP 800X480 7.0" TFT-LCD 
//#define EJ080NA_04A                            //Innolux EJ080NA-04A 1024X768 8.0" TFT-LCD
//#define EJ080NA_04B                            //Innolux EJ080NA-04B 1024X768 8.0" TFT-LCD
//#define EJ080NA_04C                            //Innolux EJ080NA-04C 1024X768 8.0" TFT-LCD
//#define EJ080NA_05B                            //Innolux EJ080NA-05B 800x600 8.0" TFT-LCD
//#define NJ070NA_23A                            //Innolux NJ070NA-23A 1024X600 7" TFT-LCD
//#define ZJ070NA_01B                            //Innolux ZJ070NA-01B 1024x600
//#define HJ070IA_02F                            //Innolux HJ070IA-02F 1280x800
//#define AT070TN90                              //CMO AT070TN90 800x480 7.0" TFT-LCD 
//#define ET101000DM6                            //EDT ET101000DM6 1024x600 
//#define B116XW03_V0                            //AUO B116XW03 V0 1366x768
//#define G190SVT01                              //AUO G190SVT01 1680x342 
//#define G070VW01_V1                            //AUO G070VW01 V1 800X480 7" TFT-LCD 
//#define G070VW01_V0                            //AUO G070VW01 V0 800X480 7" TFT-LCD 
//#define LQ150X1LGN2C                           //SHARP LQ150X1LGN2C
//#define LQ190E1LW52                            //SHARP LQ190E1LW52 1280x1024 
//#define LQ121S1LG81                            //SHARP LQ121S1LG81
//#define LQ156M1LG21                            //SHARP LQ156M1LG21 1920x1080
//#define LQ201U1LW32                            //SHARP LQ201U1LW32 1600x1200
//#define LQ150X1LGN2C_LVDS1                     //SHARP LQ150X1LGN2C_LVDS1 1024x768
//#define LQ150X1LGN2C_LVDS2                     //SHARP LQ150X1LGN2C_LVDS2 1024x768

#define LCD_IF_RGB_TTL     0                   // Padrão paralelo RGB (TTL)
#define LCD_IF_VESA        1                   // Padrão LVDS - VESA mapping
#define LCD_IF_JEIDA       2                   // Padrão LVDS - JEIDA mapping

//Valores foi fornecido pela BuyDisplay
#ifdef EK9713                                  //Fitipower EK9713 800x600/800x480
  #define LCD_FORMAT       LCD_IF_RGB_TTL      //Standard Format panel interface
  #define LCD_HW           800                 //Horizontal Width
  #define LCD_VH           480                 //Vertical Height
  #define LCD_HBPD         20                  //HS Back Porch (Blanking)
  #define LCD_HFPD         160                 //HS Front Porch
  #define LCD_HSPW         5                   //HS Pulse Width
  #define LCD_VBPD         20                  //VS Back Porch (Blanking)
  #define LCD_VFPD         12                  //VS Front Porch
  #define LCD_VSPW         3                   //VS Pulse Width
#endif

#ifdef AT070TN92                               //Innolux AT070TN92 800X480 7"  TFT-LCD
  #define LCD_FORMAT       LCD_IF_RGB_TTL      //Standard Format panel interface
  #define LCD_HW           800                 //Horizontal Width
  #define LCD_VH           480                 //Vertical Height
  #define LCD_HBPD         30                  //HS Back Porch (Blanking) - 46
  #define LCD_HFPD         210                 //HS Front Porch - 16~354
  #define LCD_HSPW         16                  //HS Pulse Width - 1~40
  #define LCD_VBPD         13                  //VS Back Porch (Blanking) - 23
  #define LCD_VFPD         22                  //VS Front Porch - 7~147
  #define LCD_VSPW         10                  //VS Pulse Width - 1~20
#endif


#ifdef AWT_1024600L7N03
  #define LCD_FORMAT       LCD_IF_RGB_TTL      //Standard Format panel interface
  #define LCD_HW           1024                //Horizontal Width
  #define LCD_VH           600                 //Vertical Height
  #define LCD_HBPD         80                  //HS Back Porch (Blanking) - 
  #define LCD_HFPD         80                  //HS Front Porch - 
  #define LCD_HSPW         160                 //HS Pulse Width - 
  #define LCD_VBPD         8                   //VS Back Porch (Blanking) - 
  #define LCD_VFPD         5                   //VS Front Porch - 
  #define LCD_VSPW         8                   //VS Pulse Width - 
#endif


#ifdef AWY_800480T70N02
  #define LCD_FORMAT       LCD_IF_RGB_TTL      //Standard Format panel interface
  #define LCD_HW           800                 //Horizontal Width
  #define LCD_VH           480                 //Vertical Height
  #define LCD_HBPD         40                  //HS Back Porch (Blanking) - 
  #define LCD_HFPD         40                  //HS Front Porch - 
  #define LCD_HSPW         48                  //HS Pulse Width - 
  #define LCD_VBPD         24                  //VS Back Porch (Blanking) - 
  #define LCD_VFPD         13                  //VS Front Porch - 
  #define LCD_VSPW         8                   //VS Pulse Width - 
#endif


#ifdef EJ080NA_05B                             //Innolux EJ080NA-05B 800x600 8.0" TFT-LCD
  #define LCD_FORMAT       LCD_IF_RGB_TTL      //Standard Format panel interface
  #define LCD_HW           800                 //Horizontal Width
  #define LCD_VH           600                 //Vertical Height
  #define LCD_HBPD         38                  //HS Back Porch (Blanking) - 46
  #define LCD_HFPD         210                 //HS Front Porch - 16~354
  #define LCD_HSPW         8                   //HS Pulse Width - 1~40C
  #define LCD_VBPD         15                  //VS Back Porch (Blanking) - 23
  #define LCD_VFPD         12                  //VS Front Porch - 1~77
  #define LCD_VSPW         8                   //VS Pulse Width - 1~20C
#endif


#ifdef ET0700B3DMA
  #define LCD_FORMAT       LCD_IF_RGB_TTL      //Standard Format panel interface
  #define LCD_HW           800                 //Horizontal Width
  #define LCD_VH           480                 //Vertical Height
  #define LCD_HBPD         46                  //HS Back Porch (Blanking) - 
  #define LCD_HFPD         210                 //HS Front Porch - 
  #define LCD_HSPW         10                  //HS Pulse Width - 
  #define LCD_VBPD         25                  //VS Back Porch (Blanking) - 
  #define LCD_VFPD         20                  //VS Front Porch - 
  #define LCD_VSPW         10                  //VS Pulse Width - 
#endif


#ifdef ET101000DM6
  #define LCD_FORMAT       LCD_IF_VESA          //Standard Format panel interface
  #define LCD_HW           1024                 //Horizontal Width
  #define LCD_VH           600                  //Vertical Height
  #define LCD_HBPD         160                  //HS Back Porch (Blanking) - 90~376@320 <-verificar
  #define LCD_HFPD         160                  //HS Front Porch - 90~376@320 <-verificar
  #define LCD_HSPW         60                   //HS Pulse Width - 90~376@320 <- verificar
  #define LCD_VBPD         23                   //VS Back Porch (Blanking) - 10~200@35 <-verificar
  #define LCD_VFPD         12                   //VS Front Porch - 10~200@35 <-verificar
  #define LCD_VSPW         5                    //VS Pulse Width - 10~200@35 <-verificar
#endif


#ifdef G190SVT01
  #define LCD_FORMAT       LCD_IF_RGB_TTL      //Standard Format panel interface
  #define LCD_HW           1680                //Horizontal Width
  #define LCD_VH           342                 //Vertical Height
  #define LCD_HBPD         30                  //HS Back Porch (Blanking) - 288
  #define LCD_HFPD         30                  //HS Front Porch - 
  #define LCD_HSPW         30                  //HS Pulse Width - 
  #define LCD_VBPD         6                   //VS Back Porch (Blanking) - 16
  #define LCD_VFPD         6                   //VS Front Porch - 
  #define LCD_VSPW         6                   //VS Pulse Width - 
#endif


#ifdef HJ070IA_02F
  #define LCD_FORMAT       LCD_IF_VESA         //Standard Format panel interface
  #define LCD_HW           1280                //Horizontal Width
  #define LCD_VH           800                 //Vertical Height
  #define LCD_HBPD         30                  //HS Back Porch (Blanking) - 160
  #define LCD_HFPD         30                  //HS Front Porch - 
  #define LCD_HSPW         100                 //HS Pulse Width - 
  #define LCD_VBPD         3                   //VS Back Porch (Blanking) - 23
  #define LCD_VFPD         3                   //VS Front Porch - 
  #define LCD_VSPW         18                  //VS Pulse Width - 
#endif


#ifdef LQ190E1LW52
  #define LCD_FORMAT       LCD_IF_RGB_TTL      //Standard Format panel interface
  #define LCD_HW           1280                //Horizontal Width
  #define LCD_VH           1024                //Vertical Height
  #define LCD_HBPD         110                 //HS Back Porch (Blanking) - 204x2
  #define LCD_HFPD         110                 //HS Front Porch - 
  #define LCD_HSPW         110                 //HS Pulse Width - 
  #define LCD_VBPD         15                  //VS Back Porch (Blanking) - 42
  #define LCD_VFPD         15                  //VS Front Porch - 
  #define LCD_VSPW         12                  //VS Pulse Width - 
#endif


#ifdef LQ035NC111
  #define LCD_FORMAT       LCD_IF_RGB_TTL      //Standard Format panel interface
  #define LCD_HW           320                 //Horizontal Width
  #define LCD_VH           240                 //Vertical Height
  #define LCD_HBPD         38                  //HS Back Porch (Blanking) - 38
  #define LCD_HFPD         20                  //HS Front Porch - 20
  #define LCD_HSPW         20                  //HS Pulse Width - 5~30
  #define LCD_VBPD         4                   //VS Back Porch (Blanking) - 4
  #define LCD_VFPD         15                  //VS Front Porch - 15
  #define LCD_VSPW         3                   //VS Pulse Width - 3
#endif


#ifdef ZJ070NA_01B
  #define LCD_FORMAT       LCD_IF_VESA         //Standard Format panel interface
  #define LCD_HW           1024                //Horizontal Width
  #define LCD_VH           600                 //Vertical Height
  #define LCD_HBPD         50                  //HS Back Porch (Blanking) - 320
  #define LCD_HFPD         50                  //HS Front Porch - 
  #define LCD_HSPW         220                 //HS Pulse Width - 
  #define LCD_VBPD         8                   //VS Back Porch (Blanking) - 35
  #define LCD_VFPD         7                   //VS Front Porch - 
  #define LCD_VSPW         20                  //VS Pulse Width - 
#endif


//--------------------------------------------------------------------------------
//
// GT Serial Character ROM Select
//
//--------------------------------------------------------------------------------

//Escolha o modelo de chip Flash ROM de Font
//Escolha o modelo da Flash ROM que vem com o fabricante do display
//#define CHIP_GT21L16T1W                      //Integrate Circuit External CGROM GT21L16T1W OK
//#define CHIP_GT30L16U2W                      //Integrate Circuit External CGROM GT30L16U2W  OK
//#define CHIP_GT30L24T3Y                      //Integrate Circuit External CGROM GT30L24T3Y  OK
//#define CHIP_GT30L24M1Z                      //Integrate Circuit External CGROM GT30L24M1Z  OK
//#define CHIP_GT30L32S4W                      //Integrate Circuit External CGROM GT30L32S4W
#define CHIP_GT20L24F6Y                        //Integrate Circuit External CGROM GT20L24F6Y  OK
//#define CHIP_GT21L24S1W                      //Integrate Circuit External CGROM GT21L24S1W  OK

#if defined(CHIP_GT21L16T1W)
  #define GTSERIAL_CGROM       BIT_CGROM_GT21L16T1W //Genitop's IC GT21L16T1W Character Flash ROM
#elif defined(CHIP_GT30L16U2W) 
  #define GTSERIAL_CGROM       BIT_CGROM_GT30L16U2W //Genitop's IC GT30L16U2W Character Flash ROM
#elif defined(CHIP_GT30L24T3Y) 
  #define GTSERIAL_CGROM       BIT_CGROM_GT30L24T3Y //Genitop's IC GT30L24T3Y Character Flash ROM
#elif defined(CHIP_GT30L24M1Z)   
  #define GTSERIAL_CGROM       BIT_CGROM_GT30L24T3Y //Genitop's IC GT30L24M1Z Character Flash ROM
#elif defined(CHIP_GT30L32S4W) 
  #define GTSERIAL_CGROM       BIT_CGROM_GT30L32S4W //Genitop's IC GT30L32S4W Character Flash ROM
#elif defined(CHIP_GT20L24F6Y) 
  /* GT20L24F6Y
     ----------
     - ISO8859 (14 sets)：5x7, 8x16
     - ASCII (11 sets)：5x7, 7x8, 6x12, 8x16, 12x12, 16x16, 24x24
     - LCM Character Set (8 sets)：5x7, 5x10
     - Unicode (173 coutries’ language, 16 dot matrix)： 5x7 ~ 24x24
       （Latin, Greek, Cyrillic, Arabic, Hebrew, Thai）
     - Data Arrangement: Vertical Byte,Horizontal String
     - Data Arrangement: Vertical Byte,Horizontal String
     - Bus Interface: SPI
  */
  #define GTSERIAL_CGROM       BIT_CGROM_GT20L24F6Y  //Genitop's IC GT20L24F6Y Character Flash ROM
#elif defined(CHIP_GT21L24S1W)
  #define GTSERIAL_CGROM       BIT_CGROM_GT21L24S1W  //Genitop's IC GT21L24S1W Character Flash ROM
#else 
  #warning "Genitop Chip não implementada para esta plataforma"
#endif


//--------------------------------------------------------------------------------
// Page(image buffer) configure
//--------------------------------------------------------------------------------


/*The maximum number of pages is based on SDRAM capacity and color depth and width and height of one page*/
/*For example we used 128Mbit SDRAM that capacity =  16Mbyte */
/*The SDRAM is divided into several image buffers and the maximum number of image buffers is limited by the 
memory size. For example : page_size = 800*600*2byte(16bpp) = 960000byte, maximum number = 16/0.96 = 16.6 */
/*vertical mulit page application*/
#define MAX_LAYER 10     //numero maximo de layer (páginas) para endereçar
#define MEMORY_SIZE 128 * 1024 * 1024  //Tamanho da memoria em bits (128Mb convertidos em bits)

//se tornar obsoleto (Não utilziar)
//Calculo é feito por LayerStartAddr()
/*
#define LAYER1_START_ADDR  800*480*2*0
#define LAYER2_START_ADDR  800*480*2*1
#define LAYER3_START_ADDR  800*480*2*2
#define LAYER4_START_ADDR  800*480*2*3
#define LAYER5_START_ADDR  800*480*2*4
#define LAYER6_START_ADDR  800*480*2*5
#define LAYER7_START_ADDR  800*480*2*6
#define LAYER8_START_ADDR  800*480*2*7
#define LAYER9_START_ADDR  800*480*2*8
#define LAYER10_START_ADDR 800*480*2*9
*/

//--------------------------------------------------------------------------------
// Select SPI / I2C / Parallel
//--------------------------------------------------------------------------------


//Padrão de comunicacao do MCU com o RA8889
//  #define COM_SPI3
    #define COM_SPI4
//  #define COM_I2C
//  #define COM_PARALLEL

#ifdef COM_SPI3
  #define COM_SPI
#endif

#ifdef COM_SPI4
  #define COM_SPI
#endif

/*====  Master I2C filter Enabled/Disable =====*/
    #define Enable_I2CM_Noise_Filter
//  #define Disable_I2CM_Noise_Filter

//Select velocidade de clock máximo da comuncaicao SPI
    #define SPI_CLOCK_SPEED_MAX  8000000       //Para Arduino Uno/Mega
//#define SPI_CLOCK_SPEED_MAX  20000000        //Para ESP32


//Selecione um para configurar o SPI Port
#define USE_SPI_PORT
//#define USE_HSPI_PORT
//#define USE_VSPI_PORT


//--------------------------------------------------------------------------------
// Configuracao do Usuario
//--------------------------------------------------------------------------------

//Ativa a Depuração da porta serial
//#define SERIAL_DEBUG

//Ativa funcao de existencia da familia Raio RA8875/RA8876/RA8877/RA8889
#define CHECK_RAIOFAMILY

//Antes de usar macro DEBUG_PRINT, use no Setup() a macro DEBUG_BEGIN para iniciar a comunicacao serial.

#ifdef SERIAL_DEBUG
  extern bool serialStarted;
  #define DEBUG_BEGIN(baud) do { Serial.begin(baud); serialStarted = true; } while(0)
  #define DEBUG_PRINT(msg, val, b, newline) SerialPrint(msg, val, b, newline)
  //Serial print para valroes hexadecimal
  #define DEBUG_PRINTH(msg, hex, b, newline) SerialPrintH(msg, hex, b, newline)
  //Serial Print com delay
  #define DEBUG_PRINTD(msg, val, b, delayms, newline) do { SerialPrint(msg, val, b, newline); if (delayms>0) delay(delayms); } while(0)
  //Serial Print com ponto flutuante
  #define DEBUG_PRINTF(msg, val, decimal, b, newline) SerialPrintF(msg, val, decimal, b, newline)
#else
   // Se não houver debug, macros não fazem nada
  #define DEBUG_BEGIN(baud)
  #define DEBUG_PRINT(msg, val, b, newline)
  #define DEBUG_PRINTH(msg, hex, b, newline)
  #define DEBUG_PRINTD(msg, val, b, delayms, newline)
  #define DEBUG_PRINTF(msg, val, decimal, b, newline)
#endif


//--------------------------------------------------------------------------------
// System
//--------------------------------------------------------------------------------

//Cosntatens para definir se as funcoes de GPIO sao de entrada ou saida
#define INPUT              0
#define IN                 INPUT
#define OUTPUT             1
#define OUT                OUTPUT

//--------------------------------------------------------------------------------
// Color depth
//--------------------------------------------------------------------------------

#define COLOR_DEPH_8BPP    8
#define COLOR_DEPH_16BPP   16
#define COLOR_DEPH_24BPP   24

//--------------------------------------------------------------------------------
// Select Wait FIFO apra evitar que ocorra transbordamento. Isso acotence para 
// MCU muito lentas. Ative esta funcao caso houver corrompimento de pixels na tela
// imagens distorcidas e pixels estranhos
//--------------------------------------------------------------------------------

//#define USE_XNWAIT

//--------------------------------------------------------------------------------
// Select MCU and Color Depth
//--------------------------------------------------------------------------------

//Descomente esta linha para selecionar escala convertida para tom de cinza 
//#define USE_GRAYSCALE

//	#define MCU_8bit_COLORDEPTH_8bpp			  
//	#define MCU_8bit_COLORDEPTH_16bpp	
//	#define MCU_8bit_COLORDEPTH_24bpp
//  #define MCU_16bit_COLORDEPTH_8bpp_Mode1	
	#define MCU_16bit_COLORDEPTH_16bpp		
//  #define MCU_16bit_COLORDEPTH_24bpp_Mode1
//	#define MCU_16bit_COLORDEPTH_24bpp_Mode2

#if defined(MCU_8bit_COLORDEPTH_8bpp)
  #define MCU8
  #define COLOR_DEPTH_8
  #define MCU              8
  #define COLOR_DEPTH      COLOR_DEPH_8BPP
#elif defined(MCU_8bit_COLORDEPTH_16bpp)
  #define MCU8
  #define COLOR_DEPTH_16
  #define MCU              8
  #define COLOR_DEPTH      COLOR_DEPH_16BPP  
#elif defined(MCU_8bit_COLORDEPTH_24bpp)
  #define MCU8
  #define COLOR_DEPTH_24
  #define MCU              8
  #define COLOR_DEPTH      COLOR_DEPH_24BPP
#elif defined(MCU_16bit_COLORDEPTH_8bpp_Mode2)
  #define MCU16
  #define COLOR_DEPTH_8
  #define MCU              16
  #define COLOR_DEPTH      COLOR_DEPH_8BPP
  #define MODE2
#elif defined(MCU_16bit_COLORDEPTH_16bpp)
  #define MCU16
  #define COLOR_DEPTH_16
  #define MCU              16
  #define COLOR_DEPTH      COLOR_DEPH_16BPP
#elif defined(MCU_16bit_COLORDEPTH_24bpp_Mode1)
  #define MCU16
  #define COLOR_DEPTH_24
  #define MCU              16
  #define COLOR_DEPTH      COLOR_DEPH_24BPP
  #define MODE1
#elif defined(MCU_16bit_COLORDEPTH_8bpp_Mode1)
  #define MCU16
  #define COLOR_DEPTH_8
  #define MCU              16
  #define COLOR_DEPTH      COLOR_DEPH_8BPP
  #define MODE1
#elif defined(MCU_16bit_COLORDEPTH_24bpp_Mode2)
  #define MCU16
  #define COLOR_DEPTH_24
  #define MCU              16
  #define COLOR_DEPTH      COLOR_DEPH_24BPP
  #define MODE2
#endif


//--------------------------------------------------------------------------------
//
// Macro RGB(r,g,b) 
// Adaptável para cores e escala de cinza de acordo com o Color Depth escolhido 
// 8, 16 ou 25bpp
//
//--------------------------------------------------------------------------------


#ifdef COLOR_DEPTH_8
  #if USE_GRAYSCALE
    #define RGB(r,g,b) ((uint8_t) \
      (((((r)*30 + (g)*59 + (b)*11)/100) & 0xE0))        | \
      (((((r)*30 + (g)*59 + (b)*11)/100) & 0xE0) >> 3)   | \
      (((((r)*30 + (g)*59 + (b)*11)/100) & 0xC0) >> 6) )
  #else
    #define RGB(r,g,b) ( ((r & 0xE0)) | ((g & 0xE0)>>3) | ((b & 0xC0)>>6) )
  #endif
#endif

#ifdef COLOR_DEPTH_16
  #if USE_GRAYSCALE
    #define RGB(r,g,b) ((uint16_t) \
      (((((r)*30 + (g)*59 + (b)*11)/100) & 0xF8) << 8) | \
      (((((r)*30 + (g)*59 + (b)*11)/100) & 0xFC) << 3) | \
      (((((r)*30 + (g)*59 + (b)*11)/100) & 0xF8) >> 3) )
  #else
    #define RGB(r,g,b) ((uint16_t) ((r & 0xF8)<<8) | ((g & 0xFC)<<3) | ((b & 0xF8)>>3) )
  #endif
#endif

#ifdef COLOR_DEPTH_24
  #if USE_GRAYSCALE
    #define RGB(r,g,b) ((uint32_t) \
      (((((r)*30 + (g)*59 + (b)*11)/100) << 16) | \
       ((((r)*30 + (g)*59 + (b)*11)/100) << 8)  | \
       (((r)*30 + (g)*59 + (b)*11)/100)) )
  #else
    #define RGB(r,g,b) ( ((r & 0xFF)<<16) | ((g & 0xFF)<<8) | (b & 0xFF) )
  #endif
#endif


//--------------------------------------------------------------------------------
//
// Color
//
//--------------------------------------------------------------------------------


#define  clBlack                   RGB(0,0,0)
#define  clBlackBlue               RGB(4,7,32)
#define  clNight                   RGB(12,9,10)
#define  clCharcoal                RGB(52,40,44)
#define  clOil                     RGB(59,49,49)
#define  clStormyGray              RGB(58,59,60)
#define  clLightBlack              RGB(69,69,69)
#define  clDarkSteampunk           RGB(77,77,79)
#define  clBlackCat                RGB(65,56,57)
#define  clIridium                 RGB(61,60,58)
#define  clBlackEel                RGB(70,62,63)
#define  clBlackCow                RGB(76,70,70)
#define  clGrayWolf                RGB(80,74,75)
#define  clVampireGray             RGB(86,80,81)
#define  clIronGray                RGB(82,89,93)
#define  clGrayDolphin             RGB(92,88,88)
#define  clCarbonGray              RGB(98,93,93)
#define  clAshGray                 RGB(102,99,98)
#define  clDimGray                 RGB(105,105,105)
#define  clNardoGray               RGB(104,106,108)
#define  clCloudyGray              RGB(109,105,104)
#define  clSmokeyGray              RGB(114,110,109)
#define  clAlienGray               RGB(115,111,110)
#define  clSonicSilver             RGB(117,117,117)
#define  clPlatinumGray            RGB(121,121,121)
#define  clGranite                 RGB(131,126,124)
#define  clGray                    RGB(128,128,128)
#define  clBattleshipGray          RGB(132,132,130)
#define  clSheetMetal              RGB(136,139,144)
#define  clDarkGainsboro           RGB(140,140,140)
#define  clGunmetalGray            RGB(141,145,141)
#define  clColdMetal               RGB(155,154,150)
#define  clStainlessSteelGray      RGB(153,163,163)
#define  clDarkGray                RGB(169,169,169)
#define  clChromeAluminum          RGB(168,169,173)
#define  clGrayCloud               RGB(182,182,180)
#define  clMetal                   RGB(182,182,182)
#define  clSilver                  RGB(192,192,192)
#define  clSteampunk               RGB(201,193,193)
#define  clPaleSilver              RGB(201,192,187)
#define  clGearSteelGray           RGB(192,198,199)
#define  clGrayGoose               RGB(209,208,206)
#define  clPlatinumSilver          RGB(206,206,206)
#define  clLightGray               RGB(211,211,211)
#define  clSilverWhite             RGB(218,219,221)
#define  clGainsboro               RGB(220,220,220)
#define  clLightSteelGray          RGB(224,229,229)
#define  clWhiteSmoke              RGB(245,245,245)
#define  clWhiteGray               RGB(238,238,238)
#define  clPlatinum                RGB(229,228,226)
#define  clMetallicSilver          RGB(188,198,204)
#define  clBlueGray                RGB(152,175,199)
#define  clRomanSilver             RGB(131,137,150)
#define  clLightSlateGray          RGB(119,136,153)
#define  clSlateGray               RGB(112,128,144)
#define  clRatGray                 RGB(109,123,141)
#define  clSlateGraniteGray        RGB(101,115,131)
#define  clJetGray                 RGB(97,109,126)
#define  clMistBlue                RGB(100,109,126)
#define  clSteelGray               RGB(113,121,126)
#define  clMarbleBlue              RGB(86,109,126)
#define  clSlateBlueGray           RGB(115,124,161)
#define  clLightPurpleBlue         RGB(114,143,206)
#define  clAzureBlue               RGB(72,99,160)
#define  clEstorilBlue             RGB(47,83,155)
#define  clBlueJay                 RGB(43,84,126)
#define  clCharcoalBlue            RGB(54,69,79)
#define  clDarkBlueGray            RGB(41,70,91)
#define  clDarkSlate               RGB(43,56,86)
#define  clDeepSeaBlue             RGB(18,52,86)
#define  clNightBlue               RGB(21,27,84)
#define  clMidnightBlue            RGB(25,25,112)
#define  clNavy                    RGB(0,0,128)
#define  clDenimDarkBlue           RGB(21,27,141)
#define  clDarkBlue                RGB(0,0,139)
#define  clLapisBlue               RGB(21,49,126)
#define  clNewMidnightBlue         RGB(0,0,160)
#define  clEarthBlue               RGB(0,0,165)
#define  clCobaltBlue              RGB(0,32,194)
#define  clMediumBlue              RGB(0,0,205)
#define  clBlueberryBlue           RGB(0,65,194)
#define  clCanaryBlue              RGB(41,22,245)
#define  clBlue                    RGB(0,0,255)
#define  clSamcoBlue               RGB(0,2,255)
#define  clBrightBlue              RGB(9,9,255)
#define  clBlueOrchid              RGB(31,69,252)
#define  clSapphireBlue            RGB(37,84,199)
#define  clBlueEyes                RGB(21,105,199)
#define  clBrightNavyBlue          RGB(25,116,210)
#define  clBalloonBlue             RGB(43,96,222)
#define  clRoyalBlue               RGB(65,105,225)
#define  clOceanBlue               RGB(43,101,236)
#define  clDarkSkyBlue             RGB(0,89,255)
#define  clBlueRibbon              RGB(48,110,255)
#define  clBlueDress               RGB(21,125,236)
#define  clNeonBlue                RGB(21,137,255)
#define  clDodgerBlue              RGB(30,144,255)
#define  clWaterBlue               RGB(14,135,204)
#define  clGlacialBlueIce          RGB(54,139,193)
#define  clSteelBlue               RGB(70,130,180)
#define  clSilkBlue                RGB(72,138,199)
#define  clWindowsBlue             RGB(53,126,199)
#define  clBlueIvy                 RGB(48,144,199)
#define  clCyanBlue                RGB(20,163,199)
#define  clBlueKoi                 RGB(101,158,199)
#define  clColumbiaBlue            RGB(135,175,199)
#define  clBabyBlue                RGB(149,185,199)
#define  clCornflowerBlue          RGB(100,149,237)
#define  clSkyBlueDress            RGB(102,152,255)
#define  clIceberg                 RGB(86,165,236)
#define  clButterflyBlue           RGB(56,172,236)
#define  clDeepSkyBlue             RGB(0,191,255)
#define  clMiddayBlue              RGB(59,185,255)
#define  clCrystalBlue             RGB(92,179,255)
#define  clDenimBlue               RGB(121,186,236)
#define  clDaySkyBlue              RGB(130,202,255)
#define  clLightSkyBlue            RGB(135,206,250)
#define  clSkyBlue                 RGB(135,206,235)
#define  clJeansBlue               RGB(160,207,236)
#define  clBlueAngel               RGB(183,206,236)
#define  clPastelBlue              RGB(180,207,236)
#define  clLightDayBlue            RGB(173,223,255)
#define  clSeaBlue                 RGB(194,223,255)
#define  clHeavenlyBlue            RGB(198,222,255)
#define  clRobinEggBlue            RGB(189,237,255)
#define  clPowderBlue              RGB(176,224,230)
#define  clCoralBlue               RGB(175,220,236)
#define  clLightBlue               RGB(173,216,230)
#define  clLightSteelBlue          RGB(176,207,222)
#define  clGulfBlue                RGB(201,223,236)
#define  clPastelLightBlue         RGB(213,214,234)
#define  clLavenderBlue            RGB(227,228,250)
#define  clWhiteBlue               RGB(219,233,250)
#define  clLavender                RGB(230,230,250)
#define  clWater                   RGB(235,244,250)
#define  clAliceBlue               RGB(240,248,255)
#define  clGhostWhite              RGB(248,248,255)
#define  clAzure                   RGB(240,255,255)
#define  clLightCyan               RGB(224,255,255)
#define  clLightSlate              RGB(204,255,255)
#define  clElectricBlue            RGB(154,254,255)
#define  clTronBlue                RGB(125,253,254)
#define  clBlueZircon              RGB(87,254,255)
#define  clCyan                    RGB(0,255,255)
#define  clAqua                    RGB(0,255,255)
#define  clBrightCyan              RGB(10,255,255)
#define  clCeleste                 RGB(80,235,236)
#define  clBlueDiamond             RGB(78,226,236)
#define  clBrightTurquoise         RGB(22,226,245)
#define  clBlueLagoon              RGB(142,235,236)
#define  clPaleTurquoise           RGB(175,238,238)
#define  clPaleBlueLily            RGB(207,236,236)
#define  clLightTeal               RGB(179,217,217)
#define  clTiffanyBlue             RGB(129,216,208)
#define  clBlueHosta               RGB(119,191,199)
#define  clCyanOpaque              RGB(146,199,199)
#define  clNorthernLightsBlue      RGB(120,199,199)
#define  clBlueGreen               RGB(123,204,181)
#define  clMediumAquaMarine        RGB(102,205,170)
#define  clAquaSeafoamGreen        RGB(147,233,190)
#define  clMagicMint               RGB(170,240,209)
#define  clLightAquamarine         RGB(147,255,232)
#define  clAquamarine              RGB(127,255,212)
#define  clBrightTeal              RGB(1,249,198)
#define  clTurquoise               RGB(64,224,208)
#define  clMediumTurquoise         RGB(72,209,204)
#define  clDeepTurquoise           RGB(72,204,205)
#define  clJellyfish               RGB(70,199,199)
#define  clBlueTurquoise           RGB(67,198,219)
#define  clDarkTurquoise           RGB(0,206,209)
#define  clMacawBlueGreen          RGB(67,191,199)
#define  clLightSeaGreen           RGB(32,178,170)
#define  clSeafoamGreen            RGB(62,169,159)
#define  clCadetBlue               RGB(95,158,160)
#define  clDeepSea                 RGB(59,156,156)
#define  clDarkCyan                RGB(0,139,139)
#define  clTealGreen               RGB(0,130,127)
#define  clTeal                    RGB(0,128,128)
#define  clTealBlue                RGB(0,124,128)
#define  clMediumTeal              RGB(4,95,95)
#define  clDarkTeal                RGB(4,93,93)
#define  clDeepTeal                RGB(3,62,62)
#define  clDarkSlateGray           RGB(37,56,60)
#define  clGunmetal                RGB(44,53,57)
#define  clBlueMossGreen           RGB(60,86,91)
#define  clBeetleGreen             RGB(76,120,126)
#define  clGrayishTurquoise        RGB(94,125,126)
#define  clGreenishBlue            RGB(48,125,126)
#define  clAquamarineStone         RGB(52,135,129)
#define  clSeaTurtleGreen          RGB(67,141,128)
#define  clDullSeaGreen            RGB(78,137,117)
#define  clDarkGreenBlue           RGB(31,99,87)
#define  clDeepSeaGreen            RGB(48,103,84)
#define  clBottleGreen             RGB(0,106,78)
#define  clSeaGreen                RGB(46,139,87)
#define  clElfGreen                RGB(27,138,107)
#define  clDarkMint                RGB(49,144,110)
#define  clJade                    RGB(0,163,108)
#define  clEarthGreen              RGB(52,165,111)
#define  clChromeGreen             RGB(26,162,96)
#define  clMint                    RGB(62,180,137)
#define  clEmerald                 RGB(80,200,120)
#define  clIsleOfManGreen          RGB(34,206,131)
#define  clMediumSeaGreen          RGB(60,179,113)
#define  clMetallicGreen           RGB(124,157,142)
#define  clCamouflageGreen         RGB(120,134,107)
#define  clSageGreen               RGB(132,139,121)
#define  clHazelGreen              RGB(97,124,88)
#define  clVenomGreen              RGB(114,140,0)
#define  clOliveDrab               RGB(107,142,35)
#define  clOlive                   RGB(128,128,0)
#define  clEbony                   RGB(85,93,80)
#define  clDarkOliveGreen          RGB(85,107,47)
#define  clMilitaryGreen           RGB(78,91,49)
#define  clGreenLeaves             RGB(58,95,11)
#define  clArmyGreen               RGB(75,83,32)
#define  clFernGreen               RGB(102,124,38)
#define  clFallForestGreen         RGB(78,146,88)
#define  clIrishGreen              RGB(8,160,75)
#define  clPineGreen               RGB(56,124,68)
#define  clMediumForestGreen       RGB(52,114,53)
#define  clRacingGreen             RGB(39,116,44)
#define  clJungleGreen             RGB(52,124,44)
#define  clCactusGreen             RGB(34,116,66)
#define  clForestGreen             RGB(34,139,34)
#define  clGreen                   RGB(0,128,0)
#define  clDarkGreen               RGB(0,100,0)
#define  clDeepGreen               RGB(5,102,8)
#define  clDeepEmeraldGreen        RGB(4,99,7)
#define  clHunterGreen             RGB(53,94,59)
#define  clDarkForestGreen         RGB(37,65,23)
#define  clLotusGreen              RGB(0,66,37)
#define  clBroccoliGreen           RGB(2,108,61)
#define  clSeaweedGreen            RGB(67,124,23)
#define  clShamrockGreen           RGB(52,124,23)
#define  clGreenOnion              RGB(106,161,33)
#define  clMossGreen               RGB(138,154,91)
#define  clGrassGreen              RGB(63,155,11)
#define  clGreenPepper             RGB(74,160,44)
#define  clDarkLimeGreen           RGB(65,163,23)
#define  clParrotGreen             RGB(18,173,43)
#define  clCloverGreen             RGB(62,160,85)
#define  clDinosaurGreen           RGB(115,161,108)
#define  clGreenSnake              RGB(108,187,60)
#define  clAlienGreen              RGB(108,196,23)
#define  clGreenApple              RGB(76,196,23)
#define  clLimeGreen               RGB(50,205,50)
#define  clPeaGreen                RGB(82,208,23)
#define  clKellyGreen              RGB(76,197,82)
#define  clZombieGreen             RGB(84,197,113)
#define  clGreenPeas               RGB(137,195,92)
#define  clDollarBillGreen         RGB(133,187,101)
#define  clFrogGreen               RGB(153,198,142)
#define  clTurquoiseGreen          RGB(160,214,180)
#define  clDarkSeaGreen            RGB(143,188,143)
#define  clBasilGreen              RGB(130,159,130)
#define  clGrayGreen               RGB(162,173,156)
#define  clLightOliveGreen         RGB(184,188,134)
#define  clIguanaGreen             RGB(156,176,113)
#define  clCitronGreen             RGB(143,179,29)
#define  clAcidGreen               RGB(176,191,26)
#define  clAvocadoGreen            RGB(178,194,72)
#define  clPistachioGreen          RGB(157,194,9)
#define  clSaladGreen              RGB(161,201,53)
#define  clYellowGreen             RGB(154,205,50)
#define  clPastelGreen             RGB(119,221,119)
#define  clHummingbirdGreen        RGB(127,232,23)
#define  clNebulaGreen             RGB(89,232,23)
#define  clStoplightGoGreen        RGB(87,233,100)
#define  clNeonGreen               RGB(22,245,41)
#define  clJadeGreen               RGB(94,251,110)
#define  clSpringGreen             RGB(0,255,127)
#define  clOceanGreen              RGB(0,255,128)
#define  clLimeMintGreen           RGB(54,245,127)
#define  clMediumSpringGreen       RGB(0,250,154)
#define  clAquaGreen               RGB(18,225,147)
#define  clEmeraldGreen            RGB(95,251,23)
#define  clLime                    RGB(0,255,0)
#define  clLawnGreen               RGB(124,252,0)
#define  clBrightGreen             RGB(102,255,0)
#define  clChartreuse              RGB(127,255,0)
#define  clYellowLawnGreen         RGB(135,247,23)
#define  clAloeVeraGreen           RGB(152,245,22)
#define  clDullGreenYellow         RGB(177,251,23)
#define  clLemonGreen              RGB(173,248,2)
#define  clGreenYellow             RGB(173,255,47)
#define  clChameleonGreen          RGB(189,245,22)
#define  clNeonYellowGreen         RGB(218,238,1)
#define  clYellowGreenGrosbeak     RGB(226,245,22)
#define  clTeaGreen                RGB(204,251,93)
#define  clSlimeGreen              RGB(188,233,84)
#define  clAlgaeGreen              RGB(100,233,134)
#define  clLightGreen              RGB(144,238,144)
#define  clDragonGreen             RGB(106,251,146)
#define  clPaleGreen               RGB(152,251,152)
#define  clMintGreen               RGB(152,255,152)
#define  clGreenThumb              RGB(181,234,170)
#define  clOrganicBrown            RGB(227,249,166)
#define  clLightJade               RGB(195,253,184)
#define  clLightMintGreen          RGB(194,229,211)
#define  clLightRoseGreen          RGB(219,249,219)
#define  clChromeWhite             RGB(232,241,212)
#define  clHoneyDew                RGB(240,255,240)
#define  clMintCream               RGB(245,255,250)
#define  clLemonChiffon            RGB(255,250,205)
#define  clParchment               RGB(255,255,194)
#define  clCream                   RGB(255,255,204)
#define  clCreamWhite              RGB(255,253,208)
#define  clLightGoldenRodYellow    RGB(250,250,210)
#define  clLightYellow             RGB(255,255,224)
#define  clBeige                   RGB(245,245,220)
#define  clWhiteYellow             RGB(242,240,223)
#define  clCornsilk                RGB(255,248,220)
#define  clBlonde                  RGB(251,246,217)
#define  clAntiqueWhite            RGB(250,235,215)
#define  clLightBeige              RGB(255,240,219)
#define  clPapayaWhip              RGB(255,239,213)
#define  clChampagne               RGB(247,231,206)
#define  clBlanchedAlmond          RGB(255,235,205)
#define  clBisque                  RGB(255,228,196)
#define  clWheat                   RGB(245,222,179)
#define  clMoccasin                RGB(255,228,181)
#define  clPeach                   RGB(255,229,180)
#define  clLightOrange             RGB(254,216,177)
#define  clPeachPuff               RGB(255,218,185)
#define  clCoralPeach              RGB(251,213,171)
#define  clNavajoWhite             RGB(255,222,173)
#define  clGoldenBlonde            RGB(251,231,161)
#define  clGoldenSilk              RGB(243,227,195)
#define  clDarkBlonde              RGB(240,226,182)
#define  clLightGold               RGB(241,229,172)
#define  clVanilla                 RGB(243,229,171)
#define  clTanBrown                RGB(236,229,182)
#define  clDirtyWhite              RGB(232,228,201)
#define  clPaleGoldenRod           RGB(238,232,170)
#define  clKhaki                   RGB(240,230,140)
#define  clCardboardBrown          RGB(237,218,116)
#define  clHarvestGold             RGB(237,226,117)
#define  clSunYellow               RGB(255,232,124)
#define  clCornYellow              RGB(255,243,128)
#define  clPastelYellow            RGB(250,248,132)
#define  clNeonYellow              RGB(255,255,51)
#define  clYellow                  RGB(255,255,0)
#define  clLemonYellow             RGB(254,242,80)
#define  clCanaryYellow            RGB(255,239,0)
#define  clBananaYellow            RGB(245,226,22)
#define  clMustardYellow           RGB(255,219,88)
#define  clGoldenYellow            RGB(255,223,0)
#define  clBoldYellow              RGB(249,219,36)
#define  clSafetyYellow            RGB(238,210,2)
#define  clRubberDuckyYellow       RGB(255,216,1)
#define  clGold                    RGB(255,215,0)
#define  clBrightGold              RGB(253,208,23)
#define  clChromeGold              RGB(255,206,68)
#define  clGoldenBrown             RGB(234,193,23)
#define  clDeepYellow              RGB(246,190,0)
#define  clMacaroniandCheese       RGB(242,187,102)
#define  clAmber                   RGB(255,191,0)
#define  clSaffron                 RGB(251,185,23)
#define  clNeonGold                RGB(253,189,1)
#define  clBeer                    RGB(251,177,23)
#define  clYellowOrange            RGB(255,174,66)
#define  clOrangeYellow            RGB(255,174,66)
#define  clCantaloupe              RGB(255,166,47)
#define  clCheeseOrange            RGB(255,166,0)
#define  clOrange                  RGB(255,165,0)
#define  clBrownSand               RGB(238,154,77)
#define  clSandyBrown              RGB(244,164,96)
#define  clBrownSugar              RGB(226,167,111)
#define  clCamelBrown              RGB(193,154,107)
#define  clDeerBrown               RGB(230,191,131)
#define  clBurlyWood               RGB(222,184,135)
#define  clTan                     RGB(210,180,140)
#define  clLightFrenchBeige        RGB(200,173,127)
#define  clSand                    RGB(194,178,128)
#define  clSoftHazel               RGB(198,186,139)
#define  clSage                    RGB(188,184,138)
#define  clFallLeafBrown           RGB(200,181,96)
#define  clGingerBrown             RGB(201,190,98)
#define  clBronzeGold              RGB(201,174,93)
#define  clDarkKhaki               RGB(189,183,107)
#define  clOliveGreen              RGB(186,184,108)
#define  clBrass                   RGB(181,166,66)
#define  clCookieBrown             RGB(199,163,23)
#define  clMetallicGold            RGB(212,175,55)
#define  clMustard                 RGB(225,173,1)
#define  clBeeYellow               RGB(233,171,23)
#define  clMarigold                RGB(235,168,50)
#define  clSchoolBusYellow         RGB(232,163,23)
#define  clGoldenRod               RGB(218,165,32)
#define  clOrangeGold              RGB(212,160,23)
#define  clChampagneGold           RGB(210,159,81)
#define  clCaramel                 RGB(198,142,23)
#define  clDarkGoldenRod           RGB(184,134,11)
#define  clCinnamon                RGB(197,137,23)
#define  clPeru                    RGB(205,133,63)
#define  clBronze                  RGB(205,127,50)
#define  clPumpkinPie              RGB(202,118,43)
#define  clTigerOrange             RGB(200,129,65)
#define  clCopper                  RGB(184,115,51)
#define  clDarkGold                RGB(170,108,57)
#define  clMetallicBronze          RGB(169,113,66)
#define  clDarkAlmond              RGB(171,120,78)
#define  clWood                    RGB(150,111,51)
#define  clKhakiBrown              RGB(144,110,62)
#define  clOakBrown                RGB(128,101,23)
#define  clAntiqueBronze           RGB(102,93,30)
#define  clHazel                   RGB(142,118,24)
#define  clDarkYellow              RGB(139,128,0)
#define  clDarkMoccasin            RGB(130,120,57)
#define  clKhakiGreen              RGB(138,134,93)
#define  clMillenniumJade          RGB(147,145,124)
#define  clDarkBeige               RGB(159,140,118)
#define  clBulletShell             RGB(175,155,96)
#define  clArmyBrown               RGB(130,123,96)
#define  clSandstone               RGB(120,109,95)
#define  clTaupe                   RGB(72,60,50)
#define  clDarkGrayishOlive        RGB(74,65,42)
#define  clDarkHazelBrown          RGB(71,56,16)
#define  clMocha                   RGB(73,61,38)
#define  clMilkChocolate           RGB(81,59,28)
#define  clGrayBrown               RGB(61,54,53)
#define  clDarkCoffee              RGB(59,47,47)
#define  clWesternCharcoal         RGB(73,65,63)
#define  clOldBurgundy             RGB(67,48,46)
#define  clRedBrown                RGB(98,47,34)
#define  clBakersBrown             RGB(92,51,23)
#define  clPullmanBrown            RGB(100,65,23)
#define  clDarkBrown               RGB(101,67,33)
#define  clSepiaBrown              RGB(112,66,20)
#define  clDarkBronze              RGB(128,74,0)
#define  clCoffee                  RGB(111,78,55)
#define  clBrownBear               RGB(131,92,59)
#define  clRedDirt                 RGB(127,82,23)
#define  clSepia                   RGB(127,70,44)
#define  clSienna                  RGB(160,82,45)
#define  clSaddleBrown             RGB(139,69,19)
#define  clDarkSienna              RGB(138,65,23)
#define  clSangria                 RGB(126,56,23)
#define  clBloodRed                RGB(126,53,23)
#define  clChestnut                RGB(149,69,53)
#define  clCoralBrown              RGB(158,70,56)
#define  clDeepAmber               RGB(160,85,68)
#define  clChestnutRed             RGB(195,74,44)
#define  clGingerRed               RGB(184,60,8)
#define  clMahogany                RGB(192,64,0)
#define  clRedGold                 RGB(235,84,6)
#define  clRedFox                  RGB(195,88,23)
#define  clGinger                  RGB(176,101,0)
#define  clDarkBisque              RGB(184,101,0)
#define  clLightBrown              RGB(181,101,29)
#define  clPetraGold               RGB(183,103,52)
#define  clBrownRust               RGB(165,93,53)
#define  clRust                    RGB(195,98,65)
#define  clCopperRed               RGB(203,109,81)
#define  clOrangeSalmon            RGB(196,116,81)
#define  clChocolate               RGB(210,105,30)
#define  clSedona                  RGB(204,102,0)
#define  clPapayaOrange            RGB(229,103,23)
#define  clHalloweenOrange         RGB(230,108,44)
#define  clNeonOrange              RGB(255,103,0)
#define  clBrightOrange            RGB(255,95,31)
#define  clFluroOrange             RGB(254,99,42)
#define  clPumpkinOrange           RGB(248,114,23)
#define  clSafetyOrange            RGB(255,121,0)
#define  clCarrotOrange            RGB(248,128,23)
#define  clDarkOrange              RGB(255,140,0)
#define  clConstructionConeOrang   RGB(248,116,49)
#define  clIndianSaffron           RGB(255,119,34)
#define  clSunriseOrange           RGB(230,116,81)
#define  clMangoOrange             RGB(255,128,64)
#define  clCoral                   RGB(255,127,80)
#define  clBasketBallOrange        RGB(248,129,88)
#define  clLightSalmonRose         RGB(249,150,107)
#define  clLightSalmon             RGB(255,160,122)
#define  clPinkOrange              RGB(248,152,128)
#define  clDarkSalmon              RGB(233,150,122)
#define  clTangerine               RGB(231,138,97)
#define  clLightCopper             RGB(218,138,103)
#define  clSalmonPink              RGB(255,134,116)
#define  clSalmon                  RGB(250,128,114)
#define  clPeachPink               RGB(249,139,136)
#define  clLightCoral              RGB(240,128,128)
#define  clPastelRed               RGB(246,114,128)
#define  clPinkCoral               RGB(231,116,113)
#define  clBeanRed                 RGB(247,93,89)
#define  clValentineRed            RGB(229,84,81)
#define  clIndianRed               RGB(205,92,92)
#define  clTomato                  RGB(255,99,71)
#define  clShockingOrange          RGB(229,91,60)
#define  clOrangeRed               RGB(255,69,0)
#define  clRed                     RGB(255,0,0)
#define  clNeonRed                 RGB(253,28,3)
#define  clScarletRed              RGB(255,36,0)
#define  clRubyRed                 RGB(246,34,23)
#define  clFerrariRed              RGB(247,13,26)
#define  clFireEngineRed           RGB(246,40,23)
#define  clLavaRed                 RGB(228,34,23)
#define  clLoveRed                 RGB(228,27,23)
#define  clGrapefruit              RGB(220,56,31)
#define  clStrawberryRed           RGB(200,63,73)
#define  clCherryRed               RGB(194,70,65)
#define  clChilliPepper            RGB(193,27,23)
#define  clFireBrick               RGB(178,34,34)
#define  clTomatoSauceRed          RGB(178,24,7)
#define  clBrown                   RGB(165,42,42)
#define  clCarbonRed               RGB(167,13,42)
#define  clCranberry               RGB(159,0,15)
#define  clSaffronRed              RGB(147,19,20)
#define  clCrimsonRed              RGB(153,0,0)
#define  clRedWine                 RGB(153,0,18)
#define  clWineRed                 RGB(153,0,18)
#define  clDarkRed                 RGB(139,0,0)
#define  clMaroonRed               RGB(143,11,11)
#define  clMaroon                  RGB(128,0,0)
#define  clBurgundy                RGB(140,0,26)
#define  clVermilion               RGB(126,25,27)
#define  clDeepRed                 RGB(128,5,23)
#define  clDarkBurgundy            RGB(128,0,32)
#define  clGarnetRed               RGB(115,54,53)
#define  clRedBlood                RGB(102,0,0)
#define  clBloodNight              RGB(85,22,6)
#define  clDarkScarlet             RGB(86,3,25)
#define  clChocolateBrown          RGB(63,0,15)
#define  clBlackBean               RGB(61,12,2)
#define  clDarkMaroon              RGB(47,9,9)
#define  clMidnight                RGB(43,27,23)
#define  clPurpleLily              RGB(85,10,53)
#define  clPurpleMaroon            RGB(129,5,65)
#define  clPlumPie                 RGB(125,5,65)
#define  clPlumVelvet              RGB(125,5,82)
#define  clDarkRaspberry           RGB(135,38,87)
#define  clVelvetMaroon            RGB(126,53,77)
#define  clRosyFinch               RGB(127,78,82)
#define  clDullPurple              RGB(127,82,93)
#define  clPuce                    RGB(127,90,88)
#define  clRoseDust                RGB(153,112,112)
#define  clPastelBrown             RGB(177,144,127)
#define  clRosyPink                RGB(179,132,129)
#define  clRosyBrown               RGB(188,143,143)
#define  clKhakiRose               RGB(197,144,142)
#define  clLipstickPink            RGB(196,135,147)
#define  clDuskyPink               RGB(204,122,139)
#define  clPinkBrown               RGB(196,129,137)
#define  clOldRose                 RGB(192,128,129)
#define  clDustyPink               RGB(213,138,148)
#define  clPinkDaisy               RGB(231,153,163)
#define  clRose                    RGB(232,173,170)
#define  clDustyRose               RGB(201,169,166)
#define  clSilverPink              RGB(196,174,173)
#define  clGoldPink                RGB(230,199,194)
#define  clRoseGold                RGB(236,197,192)
#define  clDeepPeach               RGB(255,203,164)
#define  clPastelOrange            RGB(248,184,139)
#define  clDesertSand              RGB(237,201,175)
#define  clUnbleachedSilk          RGB(255,221,202)
#define  clPigPink                 RGB(253,215,228)
#define  clPalePink                RGB(242,212,215)
#define  clBlush                   RGB(255,230,232)
#define  clMistyRose               RGB(255,228,225)
#define  clPinkBubbleGum           RGB(255,223,221)
#define  clLightRose               RGB(251,207,205)
#define  clLightRed                RGB(255,204,203)
#define  clRoseQuartz              RGB(247,202,201)
#define  clWarmPink                RGB(246,198,189)
#define  clDeepRose                RGB(251,187,185)
#define  clPink                    RGB(255,192,203)
#define  clLightPink               RGB(255,182,193)
#define  clSoftPink                RGB(255,184,191)
#define  clPowderPink              RGB(255,178,208)
#define  clDonutPink               RGB(250,175,190)
#define  clBabyPink                RGB(250,175,186)
#define  clFlamingoPink            RGB(249,167,176)
#define  clPastelPink              RGB(254,163,170)
#define  clRosePink                RGB(231,161,176)
#define  clPinkRose                RGB(231,161,176)
#define  clCadillacPink            RGB(227,138,174)
#define  clCarnationPink           RGB(247,120,161)
#define  clPastelRose              RGB(229,120,143)
#define  clBlushRed                RGB(229,110,148)
#define  clPaleVioletRed           RGB(219,112,147)
#define  clPurplePink              RGB(209,101,135)
#define  clTulipPink               RGB(194,90,124)
#define  clBashfulPink             RGB(194,82,131)
#define  clDarkPink                RGB(231,84,128)
#define  clDarkHotPink             RGB(246,96,171)
#define  clHotPink                 RGB(255,105,180)
#define  clWatermelonPink          RGB(252,108,133)
#define  clVioletRed               RGB(246,53,138)
#define  clHotDeepPink             RGB(245,40,135)
#define  clBrightPink              RGB(255,0,127)
#define  clRedMagenta              RGB(255,0,128)
#define  clDeepPink                RGB(255,20,147)
#define  clNeonPink                RGB(245,53,170)
#define  clChromePink              RGB(255,51,170)
#define  clNeonHotPink             RGB(253,52,156)
#define  clPinkCupcake             RGB(228,94,157)
#define  clRoyalPink               RGB(231,89,172)
#define  clDimorphothecaMagenta    RGB(227,49,157)
#define  clBarbiePink              RGB(218,24,132)
#define  clPinkLemonade            RGB(228,40,124)
#define  clRedPink                 RGB(250,42,85)
#define  clRaspberry               RGB(227,11,93)
#define  clCrimson                 RGB(220,20,60)
#define  clBrightMaroon            RGB(195,33,72)
#define  clRoseRed                 RGB(194,30,86)
#define  clRoguePink               RGB(193,40,105)
#define  clBurntPink               RGB(193,34,103)
#define  clPinkViolet              RGB(202,34,107)
#define  clMagentaPink             RGB(204,51,139)
#define  clMediumVioletRed         RGB(199,21,133)
#define  clDarkCarnationPink       RGB(193,34,131)
#define  clLightMaroon             RGB(162,72,87)
#define  clRaspberryPurple         RGB(179,68,108)
#define  clPinkPlum                RGB(185,59,143)
#define  clOrchid                  RGB(218,112,214)
#define  clDeepMauve               RGB(223,115,212)
#define  clViolet                  RGB(238,130,238)
#define  clFuchsiaPink             RGB(255,119,255)
#define  clBrightNeonPink          RGB(244,51,255)
#define  clMagenta                 RGB(255,0,255)
#define  clFuchsia                 RGB(255,0,255)
#define  clCrimsonPurple           RGB(226,56,236)
#define  clHeliotropePurple        RGB(212,98,255)
#define  clTyrianPurple            RGB(196,90,236)
#define  clMediumOrchid            RGB(186,85,211)
#define  clPurpleFlower            RGB(167,74,199)
#define  clOrchidPurple            RGB(176,72,181)
#define  clRichLilac               RGB(182,102,210)
#define  clPastelViolet            RGB(210,145,188)
#define  clRosy                    RGB(161,113,136)
#define  clMauveTaupe              RGB(145,95,109)
#define  clViolaPurple             RGB(126,88,126)
#define  clEggplant                RGB(97,64,81)
#define  clPlumPurple              RGB(88,55,89)
#define  clGrape                   RGB(94,90,128)
#define  clPurpleNavy              RGB(78,81,128)
#define  clSlateBlue               RGB(106,90,205)
#define  clBlueLotus               RGB(105,96,236)
#define  clBlurple                 RGB(88,101,242)
#define  clLightSlateBlue          RGB(115,106,255)
#define  clMediumSlateBlue         RGB(123,104,238)
#define  clPeriwinklePurple        RGB(117,117,207)
#define  clVeryPeri                RGB(102,103,171)
#define  clDarkLavender            RGB(115,79,150)
#define  clBrightGrape             RGB(111,45,168)
#define  clBrightPurple            RGB(106,13,173)
#define  clPurpleAmethyst          RGB(108,45,199)
#define  clBlueMagenta             RGB(130,46,255)
#define  clDarkBlurple             RGB(85,57,204)
#define  clDeepPeriwinkle          RGB(84,83,166)
#define  clDarkSlateBlue           RGB(72,61,139)
#define  clPurpleHaze              RGB(78,56,126)
#define  clPurpleIris              RGB(87,27,126)
#define  clDarkPurple              RGB(75,1,80)
#define  clDeepPurple              RGB(54,1,63)
#define  clMidnightPurple          RGB(46,26,71)
#define  clPurpleMonster           RGB(70,27,126)
#define  clIndigo                  RGB(75,0,130)
#define  clBlueWhale               RGB(52,45,126)
#define  clRebeccaPurple           RGB(102,51,153)
#define  clPurpleJam               RGB(106,40,126)
#define  clDarkMagenta             RGB(139,0,139)
#define  clPurple                  RGB(128,0,128)
#define  clFrenchLilac             RGB(134,96,142)
#define  clDarkOrchid              RGB(153,50,204)
#define  clDarkViolet              RGB(148,0,211)
#define  clPurpleViolet            RGB(141,56,201)
#define  clJasminePurple           RGB(162,59,236)
#define  clPurpleDaffodil          RGB(176,65,255)
#define  clClematisViolet          RGB(132,45,206)
#define  clBlueViolet              RGB(138,43,226)
#define  clPurpleSageBush          RGB(122,93,199)
#define  clLovelyPurple            RGB(127,56,236)
#define  clNeonPurple              RGB(157,0,255)
#define  clPurplePlum              RGB(142,53,239)
#define  clAztechPurple            RGB(137,59,255)
#define  clMediumPurple            RGB(147,112,219)
#define  clLightPurple             RGB(132,103,215)
#define  clCrocusPurple            RGB(145,114,236)
#define  clPurpleMimosa            RGB(158,123,255)
#define  clPastelIndigo            RGB(134,134,175)
#define  clLavenderPurple          RGB(150,123,182)
#define  clRosePurple              RGB(176,159,202)
#define  clViola                   RGB(200,196,223)
#define  clPeriwinkle              RGB(204,204,255)
#define  clPaleLilac               RGB(220,208,255)
#define  clLilac                   RGB(200,162,200)
#define  clMauve                   RGB(224,176,255)
#define  clBrightLilac             RGB(216,145,239)
#define  clPurpleDragon            RGB(195,142,199)
#define  clPlum                    RGB(221,160,221)
#define  clBlushPink               RGB(230,169,236)
#define  clPastelPurple            RGB(242,162,232)
#define  clBlossomPink             RGB(249,183,255)
#define  clWisteriaPurple          RGB(198,174,199)
#define  clPurpleThistle           RGB(210,185,211)
#define  clThistle                 RGB(216,191,216)
#define  clPurpleWhite             RGB(223,211,227)
#define  clPeriwinklePink          RGB(233,207,236)
#define  clCottonCandy             RGB(252,223,255)
#define  clLavenderPinocchio       RGB(235,221,226)
#define  clDarkWhite               RGB(225,217,209)
#define  clAshWhite                RGB(233,228,212)
#define  clWarmWhite               RGB(239,235,216)
#define  clWhiteChocolate          RGB(237,230,214)
#define  clCreamyWhite             RGB(240,233,214)
#define  clOffWhite                RGB(248,240,227)
#define  clSoftIvory               RGB(250,240,221)
#define  clCosmicLatte             RGB(255,248,231)
#define  clPearlWhite              RGB(248,246,240)
#define  clRedWhite                RGB(243,232,234)
#define  clLavenderBlush           RGB(255,240,245)
#define  clPearl                   RGB(253,238,244)
#define  clEggShell                RGB(255,249,227)
#define  clOldLace                 RGB(254,240,227)
#define  clWhiteIce                RGB(234,238,233)
#define  clLinen                   RGB(250,240,230)
#define  clSeaShell                RGB(255,245,238)
#define  clBoneWhite               RGB(249,246,238)
#define  clRice                    RGB(250,245,239)
#define  clFloralWhite             RGB(255,250,240)
#define  clIvory                   RGB(255,255,240)
#define  clWhiteGold               RGB(255,255,244)
#define  clLightWhite              RGB(255,255,247)
#define  clCotton                  RGB(251,251,249)
#define  clSnow                    RGB(255,250,250)
#define  clMilkWhite               RGB(254,252,255)
#define  clHalfWhite               RGB(255,254,250)
#define  clWhite                   RGB(255,255,255)

#ifdef GRAY_SCALE
  #define color65k_grayscale1      2113
  #define color65k_grayscale2      2113*2
  #define color65k_grayscale3      2113*3
  #define color65k_grayscale4      2113*4
  #define color65k_grayscale5      2113*5
  #define color65k_grayscale6      2113*6
  #define color65k_grayscale7      2113*7
  #define color65k_grayscale8      2113*8
  #define color65k_grayscale9      2113*9
  #define color65k_grayscale10     2113*10
  #define color65k_grayscale11     2113*11
  #define color65k_grayscale12     2113*12
  #define color65k_grayscale13     2113*13
  #define color65k_grayscale14     2113*14
  #define color65k_grayscale15     2113*15
  #define color65k_grayscale16     2113*16
  #define color65k_grayscale17     2113*17
  #define color65k_grayscale18     2113*18
  #define color65k_grayscale19     2113*19
  #define color65k_grayscale20     2113*20
  #define color65k_grayscale21     2113*21
  #define color65k_grayscale22     2113*22
  #define color65k_grayscale23     2113*23
  #define color65k_grayscale24     2113*24
  #define color65k_grayscale25     2113*25
  #define color65k_grayscale26     2113*26
  #define color65k_grayscale27     2113*27
  #define color65k_grayscale28     2113*28
  #define color65k_grayscale29     2113*29
  #define color65k_grayscale30     2113*30
#endif


//--------------------------------------------------------------------------------
// Class enum
//--------------------------------------------------------------------------------

//#pragma message "DEBUG: entrando em PageReg"

//Troca de pagina de registrador

enum class ePageReg : uint8_t {
  Page0 = 0x00,                                /* Page 0 Register Set */
  Page1 = cSetb0                               /* Page 1 Register Set */
};


//Tipo de seleção de destino da porta de memória do RA8889
enum class MemoryPortDest : uint8_t {
  SDRAM            = 0x0,
  GammaTable       = cSetb0,
  GraphicCursorRAM = cSetb1,
  ColorPaletteRAM  = cSetb0 | cSetb1
};


//Interface de saída do TFT com o RA8889
enum class TFTInterface : uint8_t {            
  IF_24BIT = 0x0,                              /* Saída TFT 24-bits */
  IF_18BIT = cSetb3,                           /* Saída TFT 18-bits */
  IF_16BIT = cSetb4,                           /* Saída TFT 16-bits */
  IF_NONE  = cSetb4 | cSetb3                   /* Sem Saída TFT */
};


//Host Read/Write Memory Direction
enum class MemoryDirection : uint8_t {
  LeftRight_TopBotom  = 0,
  RightLeft_TopBotom  = 1,
  TopBottom_LeftRight = 2,      
  BottomTop_LeftRight = 3
};


//Horizontal Scan Direction
enum class HSCANDir : uint8_t {                //bit 4
  LeftToRight        = 0x0,                    
  RightToLeft        = cSetb4                  
};


//Vertical Scan Direction
enum class VSCANDir : uint8_t {                //bit 3
  TopToBottom        = 0x0,                    
  BottomToTop        = cSetb3                  
};


//Parallel XDATA[23~0] Color Format
enum class ePDATAColorFmt : uint8_t {          //bit [2~0]
  RGB               = 0x0,
  RBG               = 0x1,
  GRB               = 0x2,
  GBR               = 0x3,
  BRG               = 0x4,
  BGR               = 0x5,
  Gray              = 0x6,
  BW                = 0x7
};


//Panel Scan Clock PCLK Edge Type
enum class ePCLKEdge : uint8_t {               //bit [7]
  Rising            = 0x0,                     //Borda de subida
  Falling           = cSetb7                   //Borda de descida
};


enum class eHSYNCPolarity : uint8_t {          //bit [7]
  Low              = 0x0,                      //Low active
  High             = cSetb7                    //High active
};


enum class eVSYNCPolarity : uint8_t {           //bit [6]
  Low              = 0x0,                       //Low active
  High             = cSetb6                     //High active
};


enum class eDEPolarity : uint8_t {              //bit [5]
  High             = 0x0,
  Low              = cSetb5
};


enum class ePIPSelect : uint8_t {              //bit [4]
  PIP1            = 0x00,                     //Picture-to-Picute (PIP 1) 
  PIP2            = cSetb4                    //Picture-to-Picute (PIP 2)
};


//Supported Panel Resolution (Horiz x Vert)
//Note:  The actual panel resolution depends on the pixel clock and color depth.
//When RA8889 TFT Output supports 24bpp 
//CCLK Max. = 120MHz 
//SCLK Max. = 60MHz 
//Required LCD clock ≒ LCD vertical Pixel * LCD horizontal Pixel * 60(Hz) * 1.1 
//If Required LCD Clock > SCLK, the LCD refresh rate (Refresh Rate or VSYNC rate) will be lower than 
//60Hz under this application condition.
enum class ePanelResolution : uint8_t {
  r320x240,                       //QVGA:  320 x 240 x 16/18/24-bit LCD panel 
  r480x272,                       //WQVGA: 480 x 272 x 16/18/24-bit LCD panel
  r640x480,                       //VGA:   640 x 480 x 16/18/24-bit LCD panel 
  r800x480,                       //WVGA:  800 x 480 x 16/18/24-bit LCD panel   
  r800x600,                       //SVGA:  800 x 600 x 16/18/24-bit LCD panel
  r960x540,                       //QHD:   960 x 540 x 16/18/24-bit LCD panel
  r1024x600,                      //WSVGA: 1024 x 600 x 16/18/24-bit LCD panel
  r1024x768,                      //XGA:   1024 x 768 x 16/18/24-bit LCD panel 
  r1280x768,                      //WXGA:  1280 x 768 x 16/18/24-bit LCD panel
  r1280x800,                      //WXGA:  1280 x 800 x 16/18/24-bit LCD panel 
  r1366x768                       //WXGA:  1366 x 768 x 16/18/24-bit LCD panel
};


//minha nova impelemtnacao, colocar isso de forma ajustada no sistema apra selecao do COLOR DEPTH 8/16/24 bpp
enum class eColorDepthBPP : uint8_t {
  bpp8  = COLOR_DEPH_8BPP,                     //Color Depth 8bpp
  bpp16 = COLOR_DEPH_16BPP,                    //Color Depth 16bpp
  bpp24 = COLOR_DEPH_24BPP                     //Color Depth 24bpp
};


enum class eFontSource : uint8_t {
  InternalCGROM =  0x00,                        //0b00 Select internal CGROM Character.
  ExternalCGROM =  cSetb6,                      //0b01 Select external CGROM Character. (Genitop serial flash) 
  UserDefined   =  cSetb7                       //0b10 Select user-defined Character.
};


// Enum para os padrões ISO/IEC 8859 suportados pela CGROM interna
enum class eInternalCharSet : uint8_t {
 ISO8859_1 = 0x0,                              // 0b00: Character ISO/IEC 8859-1
 ISO8859_2 = cSetb0,                           // 0b01: Character ISO/IEC 8859-2.
 ISO8859_4 = cSetb1,                           // 0b10: Character ISO/IEC 8859-4.
 ISO8859_5 = cSetb1 | cSetb0                   // 0b11: Character ISO/IEC 8859-5.
};


// Enum para os padrões ISO/IEC 8859 suportados pela CGROM externa (Genitop)
enum class eExternalCharSet : uint8_t {
  ASCII       = 0x20,                          //ASCII only (00h-1Fh, 80-FFh will send “blank space”)
  ISO8859_1   = 0x88,                          //ISO-8859-1 and ASCII code
  ISO8859_2   = 0x90,                          //ISO-8859-2 and ASCII code
  ISO8859_3   = 0x98,                          //ISO-8859-3 and ASCII code
  ISO8859_4   = 0xA0,                          //ISO-8859-4 and ASCII code
  ISO8859_5   = 0xA8,                          //ISO-8859-5 and ASCII code
  ISO8859_7   = 0xB0,                          //ISO-8859-7 and ASCII code
  ISO8859_8   = 0xB8,                          //ISO-8859-8 and ASCII code
  ISO8859_9   = 0xC0,                          //ISO-8859-9 and ASCII code
  ISO8859_10  = 0xC8,                          //ISO-8859-10 and ASCII code
  ISO8859_11  = 0xD0,                          //ISO-8859-11 and ASCII code
  ISO8859_13  = 0xD8,                          //ISO-8859-13 and ASCII code
  ISO8859_14  = 0xE0,                          //ISO-8859-14 and ASCII code
  ISO8859_15  = 0xE8,                          //ISO-8859-15 and ASCII code
  ISO8859_16  = 0xF0,                          //ISO-8859-16 and ASCII code
  GB2312      = 0x00,                          //GB2312
  GB12345     = 0x08,                          //GB12345/GB18030
  GB18030     = 0x08,                          //GB12345/GB18030
  BIG5        = 0x10,                          //BIG5
  UNICODE     = 0x18,                          //Unicode
  UNIJapanese = 0x28,                          //UNI-Japanese
  JIS0208     = 0x30,                          //JIS0208
  LGCATH      = 0x38                           //Latin / Greek / Cyrillic / Arabic / Thai / Hebrew
};


// Enum para os padrões ISO/IEC 8859 Width Setting suportados pela CGROM externa (Genitop)
enum class eExternalCharWidthSet : uint8_t {
 Fixed              = 0x00,                    //bit 1-0, Fixed width
 VariableArial      = 0x01,                    //bit 1-0, Variable width for Arial
 VariableFixedRoman = 0x02,                    //bit 1-0, Variable and fixed width for Roman
 Bold               = 0x03                     //bit 1-0, Fonte Bold
};


// Enum para alturas de fonte suportadas
enum class eFontHeight : uint8_t {
  H16 = 16,                                    // 8x16 / 16x16
  H24 = 24,                                    // 12x24 / 24x24
  H32 = 32                                     // 16x32 / 32x32
};


// Enum Font Horizontal/Vertical Enlagement Factors
enum class eFontEnlargFactor : uint8_t {
  X1 = 0,                                      //factor 1x
  X2 = 1,                                      //factor 2x
  X3 = 2,                                      //factor 3x
  X4 = 3                                       //factor 4x
};


enum class eInterruptLevel : uint8_t {
  Low  = 0x0,                                  //Reset bit 7
  High = cSetb7                                //Set bit 7
};


enum class eInterrupLevelTrigger : uint8_t {
  Low      = 0b00,                             //low level trigger    
  Falling  = 0b01,                             //falling edge trigger
  High     = 0b10,                             //high level trigger
  rising   = 0b11                              //rising edge trigger
};


enum class eNSS_Channel : uint8_t {
  XNSFCS0 = 0,                   
  XNSFCS1 = 1,                 
  XNSFCS2 = 2,               
  XNSFCS3 = 3                
};


enum class eDividerClock : uint8_t {
  X1 = 0b00,                                   //divided by 1, 1/1 
  X2 = 0b01,                                   //divided by 2, 1/2  
  X4 = 0b10,                                   //divided by 4, 1/4
  X8 = 0b11                                    //divided by 8, 1/8
};


//Uso com BTE Operation Code
enum class eBTEOpCode : uint8_t {
  MPU_WROP            = 0b0000,                //MPU Write BTE with ROP.
  MPU_RNOP            = 0b0001,                //MPU Read BTE without ROP.
  MEM_COPY_POS_ROP    = 0b0010,                //Memory copy (move) BTE in positive direction with ROP.
  MEM_COPY_NEG_ROP    = 0b0011,                //Memory copy (move) BTE in negative direction with ROP.
  MPU_TWRITE          = 0b0100,                //MPU Transparent Write BTE. (w/o ROP.)
  MPU_T_COPY_POS      = 0b0101,                //Transparent Memory copy (move) BTE in positive direction (w/o ROP.)
  PAT_ROP             = 0b0110,                //Pattern Fill with ROP.
  PAT_CHROMA          = 0b0111,                //Pattern Fill with key-chroma
  COLOR_EXP           = 0b1000,                //Color Expansion
  COLOR_EXP_T         = 0b1001,                //Color Expansion with transparency
  MOVE_POS_ALPHA      = 0b1010,                //Move BTE in positive direction with Alpha blending
  MPU_WALPHA          = 0b1011,                //MPU Write BTE with Alpha blending
  SOLID_FILL          = 0b1100                 //Solid Fill
};


/**
 * @brief BTE ROP (Raster Operation) Codes for RA8889
 *
 * These codes define how the source(s) (S0, S1) are combined to produce
 * the destination (D). They are used with BTE operations that support ROP.
 */
enum class eROPCode : uint8_t {
  Blackness       = 0b0000,                    // 0
  NotS0_And_NotS1 = 0b0001,                    // ~S0・~S1  or  ~(S0 + S1)
  NotS0_And_S1    = 0b0010,                    // ~S0・S1
  NotS0           = 0b0011,                    // ~S0
  S0_And_NotS1    = 0b0100,                    // S0・~S1
  NotS1           = 0b0101,                    // ~S1
  S0_Xor_S1       = 0b0110,                    // S0 ^ S1
  NotS0_Or_NotS1  = 0b0111,                    // ~S0 + ~S1  or  ~(S0・S1)
  S0_And_S1       = 0b1000,                    // S0・S1
  Not_S0_Xor_S1   = 0b1001,                    // ~(S0 ^ S1)
  S1              = 0b1010,                    // S1
  NotS0_Or_S1     = 0b1011,                    // ~S0 + S1
  S0              = 0b1100,                    // S0
  S0_Or_NotS1     = 0b1101,                    // S0 + ~S1
  S0_Or_S1        = 0b1110,                    // S0 + S1
  Whiteness       = 0b1111                     // 1
};


//IDEC Serial Flash/ROM I/F # Select
enum class eSFIROM : uint8_t { 
  ROM0 = 0b00,                 //Serial Flash/ROM 0 I/F is selected. 
  ROM1 = 0b01,                 //Serial Flash/ROM 1 I/F is selected. 
  ROM2 = 0b10,                 //Serial Flash/ROM 2 I/F is selected. 
  ROM3 = 0b11                  //Serial Flash/ROM 3 I/F is selected. 
};


//IDEC AVI Set Mode 
enum class eAVIMode: uint8_t {
  AVI_PIP1_Shadow = 0,
  AVI_PIP2_Shadow = 1,
  AVI_PIP1_NoShadow = 2
};


//posicao do cursos na tela
struct pospixel_t {
  uint16_t x;
  uint16_t y;
};


//API de estrutura para configuração de fonte Interna
struct FontInternalParam {
  eInternalCharSet charset_select;             //iso_select = 0 : iso8859-1, iso_select = 1 : iso8859-2, iso_select = 2 : iso8859-4, iso_select = 3 : iso8859-5
  //eFontHeight size_select;                   //size_select = 0 : 8*16/16*16, size_select = 1 : 12*24/24*24, size_select = 2 : 16*32/32*32
  bool full_align;                             //align = 0 : full alignment disable, align = 1 : full alignment enable 
  bool chroma_key;                             //chroma_key = 0 : text with chroma key disable, chroma_key = 1 : text with chroma key enable
  eFontEnlargFactor width_enlarge;             //width_enlarge can be set 0~3, (00b: X1) (01b : X2)  (10b : X3)  (11b : X4)
  eFontEnlargFactor height_enlarge;            //height_enlarge can be set 0~3, (00b: X1) (01b : X2)  (10b : X3)  (11b : X4)
};


//API de estrutura para configuração de fonte Externa
struct FontExternalParam {
  uint8_t scs_select;                          //Select chip ROM source ROM #0/ ROM #1
  eExternalCharSet charset_select;             //ISO8859 + ASCII Code
  eExternalCharWidthSet gt_width;              //GT Width Setting Char
};


//API de estrutura para configuração de fonte Usuário
struct FontUserParam {
  char *font_table;	
};

#ifdef SERIAL_DEBUG
void SerialPrint(String msg, uint32_t value, bool b, bool newline);
void SerialPrintF(String msg, double value, uint8_t decimal, bool b, bool newline);
void SerialPrintH(String msg, const char* value, bool b, bool newline);
void SerialPrintH(String msg, uint64_t value, bool b, bool newline);
#endif

class RA8889 : public DisplayBase {   //herdado de DisplayBase
  private:
    IBus* _bus = nullptr;
  public:
    RA8889(uint8_t cs, uint8_t rst);
    void setBus(IBus& bus);

    void HardwareReset(void);
    uint8_t SoftwareReset(void);
    void RA8876_brightness(uint16_t val);

    bool Begin(void);
    void DisplayOn(bool on);
    void setBacklight(uint8_t pin);
    void Backlight(bool on);
    void DisplayTestBar(bool b);
    bool GraphicMode(void);
    bool IsGraphicMode(void);
    bool TextMode(void);
    uint16_t Width(void);
    uint16_t Height(void);
    uint8_t getColorDepth(void);
    uint32_t LayerStartAddr(uint8_t layer);
    void MainImage_StartAddress(uint32_t addr);
    void MainImage_Width(uint16_t Wx);
    void MainWindow_StartXY(uint16_t wx, uint16_t hy);
    void CanvasImage_StartAddr(uint32_t addr);
    void CanvasImage_Width(uint16_t Wx);
    void setWindow(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
    void ForegroundColorRGB(uint8_t red, uint8_t green, uint8_t blue);
    void ForegroundColor(uint32_t color);
    void BackgroundColorRGB(uint8_t red, uint8_t green, uint8_t blue);
    void BackgroundColor(uint32_t color);
    void FillScreen(uint32_t color);
    void ClearScreen();
    uint8_t Color16To8bpp(uint16_t color);
    uint16_t Color8To16bpp(uint8_t color8);
    uint16_t Color24To16bpp(uint32_t color24);
    uint32_t Color16To24bpp(uint16_t color16);
    void SetPixelPosXY(uint16_t x, uint16_t y);
    void SetPixelPos(pospixel_t pos);
    uint16_t GetPixelPosX();
    uint16_t GetPixelPosY();
    pospixel_t GetPixelPosXY();
    void ClearCurrentPage(uint32_t color = 0x00000000);
    uint8_t ReadIDCode(void);
    void SetPage(uint8_t page);
    void ShowPage(uint8_t page);
    void ShowPicturePgm(uint32_t size, const uint8_t *datap);
    void ShowPicture(eColorDepthBPP pictureBpp, uint32_t numpixels, const uint8_t *datap);

    void PutPixel(uint16_t x, uint16_t y, uint32_t color);
    void PushBlock(uint16_t x, uint16_t y, uint16_t num_pixels, const void* color_buffer);
    void WritePixels(const void* color_buffer, uint32_t num_pixels, bool auto_increment = true);
    uint32_t getPixel(uint16_t x, uint16_t y);
    void DrawPixel(uint16_t x, uint16_t y, uint32_t color);
    void DrawPixels(uint16_t x, uint16_t y, uint32_t num_pixels, uint16_t *data);
    void DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color);
    void DrawSquare (uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t forecolor, bool bfill = false);
    void DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint32_t forecolor, bool bfill = false);
    void DrawCircle (uint16_t x1, uint16_t y1, uint16_t R, uint32_t forecolor, bool bfill = false);
    void DrawEllipse (uint16_t x1, uint16_t y1, uint16_t Rx, uint16_t Ry, uint32_t forecolor, bool bfill = false);
    void DrawCurveLeftUp(uint16_t x1, uint16_t y1, uint16_t Rx, uint16_t Ry, uint32_t forecolor, bool bfill = false);
    void DrawCurveRightDown(uint16_t x1, uint16_t y1, uint16_t Rx, uint16_t Ry, uint32_t forecolor, bool bfill = false);
    void DrawCurveRightUp(uint16_t x1, uint16_t y1, uint16_t Rx, uint16_t Ry, uint32_t forecolor, bool bfill = false);
    void DrawCurveLeftDown(uint16_t x1, uint16_t y1, uint16_t Rx, uint16_t Ry, uint32_t forecolor, bool bfill = false);
    void DrawCircleSquare(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t Rx, uint16_t Ry, uint32_t forecolor, bool bfill = false);
    void DrawPicturePgm(uint16_t Wx, uint16_t Hy, uint16_t width, uint16_t height, const uint8_t *datap);
    void DrawBitmap(uint8_t *pixels, eColorDepthBPP pictureBpp, uint16_t x, uint16_t y, uint16_t w, uint16_t h);

    bool CheckFontExternal(uint8_t font_model);
    void setFontSource(eFontSource source);
    void setFontUser(FontUserParam param, bool enable = false);
    void setFontInternal(FontInternalParam param, bool enable = false);
    void setFontExternal(FontExternalParam param, bool enable = false);
    void ShowText(char *str);
    void Text(uint16_t x, uint16_t y, char *str, uint32_t foregcolor, uint32_t backgcolor);
    bool PutUnicodeString(uint16_t x, uint16_t y, uint32_t fgcolor, uint32_t bgcolor, char *str);
    void PutString(uint16_t x, uint16_t y, char *str);
    void setPosCursor(uint16_t x, uint16_t y);
    void TextColor(uint32_t fgcolor, uint32_t bgcolor);
    void PutChar8x12(uint16_t x, uint16_t y, uint32_t fgcolor, uint32_t bgcolor, bool bgtransparent, uint8_t code);
    void PutString8x12(uint16_t x, uint16_t y, uint32_t fgcolor, uint32_t bgcolor, bool bgtransparent, char *ptr);
    void PutChar16x24(uint16_t x, uint16_t y, uint32_t fgcolor, uint32_t bgcolor, bool bgtransparent, uint8_t code);
    void PutString16x24(uint16_t x, uint16_t y, uint32_t fgcolor, uint32_t bgcolor, bool bgtransparent, char *ptr);
    void PutChar32x48(uint16_t x, uint16_t y, uint32_t fgcolor, uint32_t bgcolor, bool bgtransparent, uint8_t code);
    void PutString32x48(uint16_t x, uint16_t y, uint32_t fgcolor, uint32_t bgcolor, bool bgtransparent, char *ptr);
    void PutFloat(uint16_t x, uint16_t y, double value, const char *fmt);
    void PutHexa(uint16_t x, uint16_t y, uint32_t value, const char *fmt);
    void PutDecimal(uint16_t x, uint16_t y, uint32_t value, const char *fmt);
    
    void useDMA(bool b = true);	
    void DMA_24bit_Block (uint8_t SCS, uint8_t Clk, uint16_t X1, uint16_t Y1, uint16_t X_W, uint16_t Y_H, uint16_t P_W, uint32_t Addr);
    void DMA_24bit(uint8_t clk, uint16_t x1, uint16_t y1, uint16_t Wx, uint16_t Hy, uint16_t picwidth, uint32_t addr);
    void DMA_32bit(uint8_t clk, uint16_t x1, uint16_t y1, uint16_t Wx, uint16_t Hy, uint16_t picwidth, uint32_t addr);
    void Switch_24bitsTo32bits(uint8_t bus, uint8_t scs);

    void DMA_24bitAddressBlockMode(uint8_t bus_select,
                                   uint8_t scs_select,
                                   uint8_t clk_div,
                                   uint16_t x0,
                                   uint16_t y0,
                                   uint16_t width,
                                   uint16_t height,
                                   uint16_t picture_width,
                                   uint32_t addr);
    void DMA_32bitAddressBlockMode(uint8_t bus_select,
                                   uint8_t scs_select,
                                   uint8_t clk_div,
                                   uint16_t x0,
                                   uint16_t y0,
                                   uint16_t width,
                                   uint16_t height,
                                   uint16_t picture_width,
                                   uint32_t addr);
    void DMA_24bitAddressLinearMode(uint8_t bus_select,
                                    uint8_t scs_select,
                                    uint8_t clk_div,
                                    uint32_t source_addr,
                                    uint32_t des_address,
                                    uint32_t number);
    void DMA_32bitAddressLinearMode(uint8_t bus_select,
                                    uint8_t scs_select,
                                    uint8_t clk_div,
                                    uint32_t source_addr,
                                    uint32_t des_address,
                                    uint32_t number);

    void PWM0(bool on_off, eDividerClock clock_divided, uint8_t prescalar, uint16_t clock_per_period, uint16_t duty);
    void PWM1(bool on_off, eDividerClock clock_divided, uint8_t prescalar, uint16_t clock_per_period, uint16_t duty);
    void AVI_Window(bool on_off);
    void PIP(bool On_Off, uint8_t PSelect, uint32_t PAddr, uint16_t Px, uint16_t Py, uint32_t ImageWidth, uint16_t Dx, uint16_t Dy, uint16_t Dwidth, uint16_t DHeight);

    void BTE_MemoryCopy(uint32_t s0_addr,
                        uint16_t s0_image_width,
                        uint16_t s0_x,
                        uint16_t s0_y,
                        uint32_t des_addr,
                        uint16_t des_image_width, 
                        uint16_t des_x,
                        uint16_t des_y,
                        uint16_t copy_width,
                        uint16_t copy_height);
    void BTE_MemoryCopyWithROP(uint32_t s0_addr,
                               uint16_t s0_image_width,
                               uint16_t s0_x,
                               uint16_t s0_y,
                               uint32_t s1_addr,
                               uint16_t s1_image_width,
                               uint16_t s1_x,
                               uint16_t s1_y,
                               uint32_t des_addr,
                               uint16_t des_image_width,
                               uint16_t des_x,
                               uint16_t des_y,
                               uint16_t copy_width,
                               uint16_t copy_height,
                               uint8_t rop_code);
    void BTE_MemoryCopyWithChromaKey(uint32_t s0_addr,
                                     uint16_t s0_image_width,
                                     uint16_t s0_x,
                                     uint16_t s0_y,
                                     uint32_t des_addr,
                                     uint16_t des_image_width, 
                                     uint16_t des_x,
                                     uint16_t des_y,
                                     uint16_t copy_width,
                                     uint16_t copy_height, 
                                     uint32_t chromakey_color);
    void BTE_MPUWriteWithROP(uint32_t s1_addr,
                             uint16_t s1_image_width,
                             uint16_t s1_x,
                             uint16_t s1_y,
                             uint32_t des_addr,
                             uint16_t des_image_width,
                             uint16_t des_x,
                             uint16_t des_y,
                             uint16_t width,
                             uint16_t height,
                             uint8_t rop_code,
                             const uint8_t *data);
    void BTE_MPUWriteWithROP(uint32_t s1_addr,
                             uint16_t s1_image_width,
                             uint16_t s1_x,
                             uint16_t s1_y,
                             uint32_t des_addr,
                             uint16_t des_image_width,
                             uint16_t des_x,
                             uint16_t des_y,
                             uint16_t width,
                             uint16_t height,
                             uint8_t rop_code,
                             const uint16_t *data);
    void BTE_MPUWriteWithROP(uint32_t s1_addr,
                             uint16_t s1_image_width,
                             uint16_t s1_x,
                             uint16_t s1_y,
                             uint32_t des_addr,
                             uint16_t des_image_width,
                             uint16_t des_x,
                             uint16_t des_y,
                             uint16_t width,
                             uint16_t height,
                             uint8_t rop_code);
    void BTE_MPUWriteWithChromaKey(uint32_t des_addr,
                                   uint16_t des_image_width, 
                                   uint16_t des_x,
                                   uint16_t des_y,
                                   uint16_t width,
                                   uint16_t height,
                                   uint32_t chromakey_color,
                                   const uint8_t *data);
    void BTE_MPUWriteWithChromaKey(uint32_t des_addr,
                                  uint16_t des_image_width, 
                                  uint16_t des_x,
                                  uint16_t des_y,
                                  uint16_t width,
                                  uint16_t height,
                                  uint32_t chromakey_color,
                                  const uint16_t *data);
    void BTE_MPUWriteWithChromaKey(uint32_t des_addr,
                                  uint16_t des_image_width, 
                                  uint16_t des_x,
                                  uint16_t des_y,
                                  uint16_t width,
                                  uint16_t height,
                                  uint32_t chromakey_color);
    void BTE_MPUWriteColorExpansion(uint32_t des_addr,
                                   uint16_t des_image_width, 
                                   uint16_t des_x,
                                   uint16_t des_y,
                                   uint16_t width,
                                   uint16_t height,
                                   uint32_t foreground_color,
                                   uint32_t background_color,
                                   const uint8_t *data);
    void BTE_MPUWriteColorExpansion(uint32_t des_addr,
                                   uint16_t des_image_width,
                                   uint16_t des_x,
                                   uint16_t des_y,
                                   uint16_t width,
                                   uint16_t height,
                                   uint32_t foreground_color,
                                   uint32_t background_color);
    void BTE_MPUWriteColorExpansionWithChromaKey(uint32_t des_addr,
                                                uint16_t des_image_width, 
                                                uint16_t des_x,
                                                uint16_t des_y,
                                                uint16_t width,
                                                uint16_t height,
                                                uint32_t foreground_color,
                                                uint32_t background_color,
                                                const uint8_t *data);
    void BTE_MPUWriteColorExpansionWithChromaKey(uint32_t des_addr,
                                                uint16_t des_image_width,
                                                uint16_t des_x,
                                                uint16_t des_y,
                                                uint16_t width,
                                                uint16_t height,
                                                uint32_t foreground_color,
                                                uint32_t background_color);
    void BTE_MemoryCopyWith_ARGB8888(uint32_t s1_addr,
                                    uint16_t s1_image_width,
                                    uint32_t des_addr,
                                    uint16_t des_image_width,
                                    uint16_t des_x,
                                    uint16_t des_y,
                                    uint16_t copy_width,
                                    uint16_t copy_height);

    void MPU8_8bpp_MemoryWrite(uint16_t x,uint16_t y, uint16_t w , uint16_t h , const uint8_t *data);
    void MPU8_16bpp_MemoryWrite(uint16_t x,uint16_t y, uint16_t w , uint16_t h , const uint8_t *data);
    void MPU8_24bpp_MemoryWrite(uint16_t x,uint16_t y, uint16_t w , uint16_t h , const uint8_t *data);
    void MPU16_16bpp_MemoryWrite(uint16_t x,uint16_t y, uint16_t w , uint16_t h , const uint16_t *data);
    void MPU16_24bpp_Mode1_MemoryWrite(uint16_t x,uint16_t y, uint16_t w , uint16_t h , const uint16_t *data);
    void MPU16_24bpp_Mode2_MemoryWrite(uint16_t x,uint16_t y, uint16_t w , uint16_t h , const uint16_t *data);
    void MemoryWrite(uint16_t x, uint16_t y, uint16_t w , uint16_t h , const uint8_t *data);

  protected:
    uint8_t _xnreset;	                         //Chip reset pin
    uint16_t _displaywidth;                      //lardura do display
    uint16_t _displayheight;                     //altura do display
    uint8_t _bpp;                                //color depht 8/16/24 bit per pixel (bpp)
    uint8_t _mcu;                                //tipo MCU/MPU 8 ou 16 bits 
    uint8_t _colorfmt;                           //formato da cor RGB, RBG, GRB, GBR, ....
    bool _usedma;                                //Uso de DMA para as funções que podem se utilziar deste recurso
    uint8_t _display_spi_clk_divider;            //spi master clock divisor for setup SPI Master Clock period, Fsck = Fcore / ((divisor + 1)* 2)
    uint8_t _dispplay_sfi_clk_divider;           //serial flash i/f clock divisor for setup Clock period, Fsck = Fcore / (divisor* 2)
    uint8_t _pin_backlight = 0;                  //Pino para controle de luz de fundo do display 
    uint32_t _bgcolor;                           //cor de fundo 
    uint32_t _fgcolor;                           //cor de frente
    uint32_t _text_bgcolor;                      //cor do texto de fundo 
    uint32_t _text_fgcolor;                      //cor do texto de frente
    uint16_t _cursor_x = 0;                      //Global cursor position y variables
    uint16_t _cursor_y = 0;                      //Global cursor position y variables

    //Parametros da fonte
    uint8_t _fnt_rom_scs = 0;                    //SCS
    uint8_t _fnt_dma_bus = 0;                    //Font DMA Bus
    uint32_t _charsetresourceMap = 0;            //mapa de recurso de 32 bits do char set Chips Font Flash ROM
    eFontSource _fntparam_source_select;
    eFontHeight _fntparam_size_select;
    uint8_t _fntparam_height;
    eExternalCharSet _fntparam_extern_charset_select;
    eInternalCharSet _fntparam_intern_charset_select;
    eExternalCharWidthSet _fntparam_gt_width;
    bool _fntparam_full_align;
    bool _fntparam_chroma_key;
    eFontEnlargFactor _fntparam_width_enlarge;
    eFontEnlargFactor _fntparam_height_enlarge;

//   _charsetresourceMap cada posicao de bits é recursos presente do CI
//   bit    Conjunto de Fontes  Valor Hex.   Valor Dec.
//   0      GB2312              0x00         0
//   1      GB12345/GB18030     0x08         8
//   2      BIG5                0x10         16
//   3      ASCII               0x18         24
//   4      UNICODE             0x20         32
//   5      UNI Japanese        0x28         40
//   6      JIS0208             0x30         48
//   7      LGCATH              0x38         56
//   8      reservado           0x40         64
//   9      reservado           0x48         72
//   10     ISO8859_1_ASCII     0x88         136
//   11     ISO8859_2_ASCII     0x90         144
//   12     ISO8859_3_ASCII     0x98         152
//   13     ISO8859_4_ASCII     0xA0         160
//   14     ISO8859_5_ASCII     0xA8         168
//   15     ISO8859_7_ASCII     0xB0         176
//   16     ISO8859_8_ASCII     0xB8         184
//   17     ISO8859_9_ASCII     0xC0         192
//   18     ISO8859_10_ASCII    0xC8         200
//   19     ISO8859_11_ASCII    0xD0         208
//   20     ISO8859_13_ASCII    0xD8         216
//   21     ISO8859_14_ASCII    0xE0         224
//   22     ISO8859_15_ASCII    0xE8         232
//   23     ISO8859_16_ASCII    0xF0         240

    bool CoreTask_WaitReady(void);
    bool Draw_WaitReady(void);
    bool IC_WaitReady(void);
    bool Initialize(void);
    bool Wait_WriteFIFO_NotFull(void);
    bool Wait_WriteFIFO_Empty(void);
    bool Wait_ReadFIFO_NotFull(void);
    bool Wait_ReadFIFO_NotEmpty(void);
    void GotoLinearAddr(uint32_t addr);
    void GotoPixel_Linear(uint32_t addr);
    void GotoPixel_XY(uint16_t Wx, uint16_t Hy);
    void GotoText_XY(uint16_t Wx, uint16_t Hy);

    void PanelResolution(ePanelResolution resolution);
    void PageSwitch(ePageReg pr);
    void SPIM_SelectableBusMode(void);
    void SPIM_FixedBusMode(void);
    void SPIM_ClockDivided_2(void);
    void SPIM_ClockDivided_1(void);

    void SPI_Clock_Period(uint8_t divisor);

    void PLL_InitilizeWaitReady(void);
    void PLL_Disable(void);
    void PLL_Enable(void);
    void PLL_ConfigClocks(uint8_t scanclk, uint8_t dramclk, uint8_t coreclk, uint8_t xtalclk);
    void PLL_Init(void);
    bool SDRAM_WaitReady(void);
    void SDRAM_Init(void);

    void MemorySelect_SDRAM(void);
    void MemorySelect_GammaTable(void);
    void MemorySelect_GraphicCursorRAM(void);
    void MemorySelect_ColorPaletteRAM(void);
    void MemoryPort_Select(MemoryPortDest dest);

    void Interrupt_Resume_Enable(bool b);
    void ExtInterrupt_Input_Enable(bool b);
    void Interrupt_I2CM_Enable(bool b);
    void Interrupt_VSync_Enable(bool b);
    void Interrupt_KeyScan_Enable(bool b);
    void Interrupt_ClearMultiEventTask_Enable(bool b);
    void Interrupt_PWM1_Enable(bool b);
    void Interrupt_PWM0_Enable(bool b);
    uint8_t Interrupt_Status(void);
    void VSYNC_WaitReady(void);
    void Interrupt_ClearResume_Flag(void);
    void ExtInterrupt_ClearInput_Flag(void);
    void Interrupt_ClearI2CM_Flag(void);
    void Interrupt_ClearVSync_Flag(void);
    void Interrupt_ClearKeyScan_Flag(void);
    bool Interrupt_IsKeyPressed(void);
    void Interrupt_ClearMultiEventTask_Flag(void);
    void Interrupt_ClearPWM0_Flag(void);
    void Interrupt_ClearPWM1_Flag(void);

    void PIP1_Enable(bool b);
    void PIP2_Enable(bool b);
    void PIP_Select_Parameter(ePIPSelect pip);
    void Select_MainWindow_8bpp(void);
    void Select_MainWindow_16bpp(void);
    void Select_MainWindow_24bpp(void);
    void Select_LCD_SyncMode(void);
    void Select_LCD_DEMode(void);

    void XnWAIT_Mask(bool mask);
    void TFT_24bit(void);
    void TFT_18bit(void);
    void TFT_16bit(void);
    void TFT_Without(void);
    void TFT_Interface(TFTInterface mode);
    void KeyScan_Enable(bool b);

    void SFlashSPI_Enable(bool b = true);
    void HostDataBus_Select_8bit(void);
    void HostDataBus_Select_16bit(void);
    void HostColorDepthFormat(uint8_t type);
    void Select_MCU_ColorDepth(void);
    void HostReadMemoryDirection(MemoryDirection direction);
    void HostWriteMemoryDirection(MemoryDirection direction);
		
    void HScanDirection_LeftToRight(void);
    void HScanDirection_RightToLeft(void);
    void HorizontalScanDirection(HSCANDir direction);
    void VScanDirection_TopToBottom(void);
    void VScanDirection_BottomToTop(void);
    void VerticalScanDirection(VSCANDir direction);
    void PDATA_ColorRGB(void);
    void PDATA_ColorFmt(ePDATAColorFmt fmt);
    void PCLK_Rising(void);
    void PCLK_Falling(void);
    void PCLK_EdgeType(ePCLKEdge edge);

    void HSYNC_PolarityLow(void);
    void HSYNC_PolarityHigh(void);
    void HSYNC_Polarity(eHSYNCPolarity val);
    void VSYNC_PolarityLow(void);
    void VSYNC_PolarityHigh(void);
    void VSYNC_Polarity(eVSYNCPolarity val);
    void DE_PolarityLow(void);
    void DE_PolarityHigh(void);
    void DE_Polarity(eDEPolarity val);
    void DE_IdleStateLow(void);
    void DE_IdleStateHigh(void);
    void PCLK_IdleStateLow(void);
    void PCLK_IdleStateHigh(void);
    void PDAT_IdleStateLow(void);
    void PDAT_IdleStateHigh(void);
    void HSYNC_IdleStateLow(void);
    void HSYNC_IdleStateHigh(void);
    void VSYNC_IdleStateLow(void);
    void VSYNC_IdleStateHigh(void);

    void HorizontalWidth_VerticalHeight(uint16_t WX, uint16_t HY);
    void Horizontal_NonDisplay(uint16_t hbpd);
    void HSYNC_StartPosition(uint16_t hfpd);
    void HSYNC_PulseWidth(uint16_t hspw);
    void Vertical_NonDisplay(uint16_t vbpd);
    void VSYNC_StartPosition(uint16_t vfpd);
    void VSYNC_PulseWidth(uint8_t vspw);
    void LCD_SetPanel(void);

    void Memory_BlockMode(void);
    bool Memory_IsBlockMode(void);
    void Memory_XYMode(void);
    bool Memory_IsXYMode(void);
    void Memory_LinearMode(void);
    bool Memory_IsLinearMode(void);
    void Memory_8bpp_BlockMode(void);
    void Memory_16bpp_BlockMode(void);
    void Memory_24bpp_BlockMode(void);
    void Memory_ColorDepth_BlockMode(eColorDepthBPP colordepth);

    uint8_t SPIM_TxRxFIFOData_Get(void);
    uint8_t SPIM_TxRxFIFOData_Put(uint8_t data);
    bool SPIM_TxFIFO_Empty(void);
    bool SPIM_TxFIFO_Full(void);
    bool SPIM_RxFIFO_Empty(void);
    bool SPIM_RxFIFO_Full(void);
    void SPIM_Select_Bus0(void);
    void SPIM_Select_Bus1(void);
    void SPIM_RxLatchEdge_Rising(void);
    void SPIM_RxLatchEdge_Falling(void);

    bool Interrupt_Overflow_Flag(void);
    void Interrupt_ClearOverflow_Flag(void);
    bool EMTI_Flag(void);
    void EMTI_Clear_Flag(void);

    void Interrupt_ActiveLevel(eInterruptLevel level);
    void ExtInterrupt_Debounce(void);
    void ExtInterrupt_NoDebounce(void);
    void ExtInterrupt_InputLevelTrigger(eInterrupLevelTrigger leveltrg);
    void LVDS_DataFormat_VESA(void);
    void LVDS_DataFormat_JEIDA(void);
		
    void nSS_Select_Channel(eNSS_Channel channel);
    void Interrupt_SPIM_Enable(bool b);
    void nSS_Inactive(void);
    void nSS_Active(void);
    void Interrupt_FIFOOverflow_Enable(bool b);
    void Interrupt_EMTIRQEN_Enable(bool b);
    void Reset_CPOL(void);
    void Set_CPOL(void);
    void Reset_CPHA(void);
    void Set_CPHA(void);

    void ActiveWindow_XY(uint16_t Wx, uint16_t Hy);
    void ActiveWindow_WidhtHeight(uint16_t Wx, uint16_t Hy); 
    void DrawEnable_AA(bool b);                              //Verificar se funcao do RA8876/RA8877
    void LineMode_Start(void);
    void TriangleMode_Start(bool fill);
    void CircleMode_Start(bool fill);
    void EllipseMode_Start(bool fill);
    void CurveLeftDownMode_Start(bool fill);
    void CurveLeftUpMode_Start(bool fill);
    void CurveRightUpMode_Start(bool fill);
    void CurveRightDownMode_Start(bool fill);
    void SquareMode_Start(bool fill);
    void CircleSquareMode_Start(bool fill);
		
    void Point1_XY(uint16_t wx, uint16_t hy);
    void Point2_XY(uint16_t wx, uint16_t hy);
    void Point3_XY(uint16_t wx, uint16_t hy);
    void Line_Point1XY(uint16_t wx, uint16_t hy);            //Mesmo que Point1_XY()
    void Line_Point2XY(uint16_t wx, uint16_t hy);            //Mesmo que Point2_XY()
    void Triangle_Point1XY(uint16_t wx, uint16_t hy);        //Mesmo que Point1_XY()
    void Triangle_Point2XY(uint16_t wx, uint16_t hy);        //Mesmo que Point2_XY()
    void Triangle_Point3XY(uint16_t wx, uint16_t hy);        //Mesmo que Point3_XY()
    void Square_Point1XY(uint16_t wx, uint16_t hy);          //Mesmo que Point1_XY()
    void Square_Point2XY(uint16_t wx, uint16_t hy);          //Mesmo que Point2_XY()
    void Radius_RxRy(uint16_t Rx, uint16_t Ry);
    void CircleRadius_R(uint16_t R);                         //Adaptado, mesmo que Radius_RxRy()
    void EllipseRadius_RxRy(uint16_t Rx, uint16_t Ry);       //Mesmo que Radius_RxRy()
    void CircleSquareRadius_RxRy(uint16_t Rx, uint16_t Ry);  //Mesmo que Radius_RxRy()
    void Center_XY(uint16_t Wx, uint16_t Hy);
    void CircleCenter_XY(uint16_t Wx, uint16_t Hy);          //Mesmo que Center_XY()
    void EllipseCenter_XY(uint16_t Wx, uint16_t Hy);         //Mesmo que Center_XY()
    void ForegroundColor8bpp(uint8_t color);
    void ForegroundColor16bpp(uint16_t color);
    void ForegroundColor24bpp(uint32_t color);
    void BackgroundColor8bpp(uint8_t color);
    void BackgroundColor16bpp(uint16_t color);
    void BackgroundColor24bpp(uint32_t color);

    void GammaCorrection_Enable(bool b);
    void GammaTableforBlue(void);
    void GammaTableforGreen(void);
    void GammaTableforRed(void);

    void CursorGraphic_Enable(bool b);
    void CursorGraphic_Set1(void);
    void CursorGraphic_Set2(void);
    void CursorGraphic_Set3(void);
    void CursorGraphic_Set4(void);
    void CursorText_Enable(bool b);
    void CursorText_Blinking_Enable(bool b);
    void CursorText_BlinkingTimeFrames(uint8_t frames);
    void CursorText_Dimensions(uint8_t Wx, uint8_t Hy);
    void CursorGraphic_Position(uint16_t Wx, uint16_t Hy);
    void CursorGraphic_Color0(uint8_t color);
    void CursorGraphic_Color1(uint8_t color);

    void Font_Init(void);
    void External_CGROM_CharSetResourceMap(void);                       //Cria mapa de recursos de fontes Externa do CGROM
    void Font_UseUserDefined(void);
    void Font_UseInternalCGROM(void);
    void Font_UseExternalCGROM(void);
    void Font_SetSource(eFontSource source);
    void Font_SetHeight_16(void);
    void Font_SetHeight_24(void);
    void Font_SetHeight_32(void);
    void Font_SetHeight(eFontHeight height);
    void Font_LineDistance(uint8_t gap);
    void Font_toFontWidthSetting(uint8_t pixels);
    void Font_FullAlignmentEnable(void);
    void Font_FullAlignmentDisable(void);
    void Font_UseBackgroundTransparency(void);
    void Font_UseBackgroundColor(void);
    void Font_0degree(void);
    void Font_90degree(void);
    void Font_WidthEnlargFactor(eFontEnlargFactor factor);
    void Font_HeightEnlargFactor(eFontEnlargFactor factor);
    void CGRAM_StartAddress(uint32_t addr);
    void SetTextParameter0(uint8_t sourceselect, uint8_t sizeselect, uint8_t isoselect);
    void SetTextParameter1(uint8_t align, uint8_t chromakey, uint8_t widthenlarge, uint8_t heightenlarge);

    void Select_Internal_CGROM_ISOIEC8859_1(void);
    void Select_Internal_CGROM_ISOIEC8859_2(void);
    void Select_Internal_CGROM_ISOIEC8859_4(void);
    void Select_Internal_CGROM_ISOIEC8859_5(void);
    void Select_Internal_CGROM_ISO8859(eInternalCharSet iso);
    void GTFont_Select_GT21L16T1W(void);
    void GTFont_Select_GT30L16U2W(void);
    void GTFont_Select_GT30L24T3Y(void);
    void GTFont_Select_GT30L24M1Z(void);
    void GTFont_Select_GT30L32S4W(void);
    void GTFont_Select_GT20L24F6Y(void);
    void GTFont_Select_GT21L24S1W(void);
    void GTFont_CharacterParameter(uint8_t scs_select, uint8_t clk_div, uint8_t rom_select, uint8_t character_select, uint8_t gt_width);
    void GTFont_SetDecoder(uint8_t temp);
    void GTFont_CharacterROMParameter(uint8_t scsselect, 
                                      uint8_t clkdiv, 
                                      uint8_t romselect, 
                                      uint8_t characterselect,
                                      uint8_t gtwidth);

    void Select_SFI_SingleData_03h(void);
    void Select_SFI_SingleData_0Bh(void);
    void Select_SFI_SingleData_1Bh(void);
    void Select_SFI_DualData_3Bh(void);
    void Select_SFI_DualData_BBh(void);                      //Only RA8876/RA8877
    void Select_SFI_QuadData_6Bh(void);
    void Select_SFI_QuadData_EBh(void);

    void IDEC_SPI_Select_StandardMode0orMode3(void);         //Only RA8876/RA8887
    void IDEC_RA8875_SPI_Select_Mode0andMode3(void);         //Only RA8876/RA8887
    void IDEC_Unmask_Interrupt(void);
    void IDEC_Mask_Interrupt(void);
    void IDEC_Interrupt_Mask(bool b);
    void IDEC_InterruptEnable(bool b);
    void IDEC_ClearInterrupt_Flag(void);
    void IDEC_Reset(void);
    void IDEC_SFI_Select_ROM(eSFIROM sfirom);
    void Font_DMA_Select_Bus0(void);
    void Font_DMA_Select_Bus1(void);
    void IDEC_Select_Bus0(void);
    void IDEC_Select_Bus1(void);
    void IDEC_Destination_ColorDepth_8bpp(void);
    void IDEC_Destination_ColorDepth_16bpp(void);
    void IDEC_Destination_ColorDepth_24bpp(void);
    void IDEC_Destination_ColorDepth(eColorDepthBPP bpp);
    void IDEC_Starts_Decoding(void);
    void IDEC_WaitReady(void);
    uint8_t IDEC_Busy(void);
    void IDEC_SFI_Select_24bitAddress(void);
    void IDEC_SFI_Select_32bitAddress(void);
    void IDEC_SPI_ClockDivide(uint8_t spiclockdivide);
    void IDEC_Source_StartAddress(uint32_t addr);
    void IDEC_Destination_UpperLeftCorner(uint16_t Wx, uint16_t Hy);
    void IDEC_AVI_Decoding_PIP1_Shadow(void);
    void IDEC_AVI_Decoding_PIP2_Shadow(void);
    void IDEC_AVI_Decoding_PIP1_NoShadow(void);
    void IDEC_AVI_SetMode(eAVIMode mode);
    void IDEC_SetImageDMANumber(uint32_t num);
    void IDEC_Destination_StartAddress(uint32_t addr);
    void IDEC_Destination_ImageWidth(uint16_t Wx);

    void PWM_Prescaler(uint8_t prescaler);
    void PWM0_ClockDividedBy(eDividerClock divider);
    void PWM1_ClockDividedBy(eDividerClock divider);
    void PWM1_Select_ErrorFlag(void);
    void PWM1_Select(void);
    void PWM1_Select_OscillatorClock(void);
    void PWM0_Select_GPIOC7(void);
    void PWM0_Select(void);
    void PWM0_Select_CoreClock(void);
    void PWM1_InverterOn(boolean on);
    void PWM1_Select_AutoReload(void);
    void PWM1_Select_OneShot(void);
    void PWM1_StartTimer(void);
    void PWM1_StopTimer(void);
    void PWM0_DeadZoneEnable(bool b);
    void PWM0_InverterOn(bool on);
    void PWM0_Select_AutoReload(void);
    void PWM0_Select_OneShot(void);
    void PWM0_StartTimer(void);
    void PWM0_StopTimer(void);
    void PWM0_DeadZoneLength(uint8_t len);
    void PWM0_SetCompareBuffer(uint16_t duty);
    void PWM0_SetCountBuffer(uint16_t clock_per_period);
    void PWM1_SetCompareBuffer(uint16_t duty);
    void PWM1_SetCountBuffer(uint16_t clock_per_period);
		
    void SFI_Select_ROM0(void);
    void SFI_Select_ROM1(void);
    void SFI_DMA_WaitReady(void);
    void SFI_DMA_Start(void);
    void SFI_Select_FontMode(void);
    void SFI_Select_DMAMode(void);
    void SFI_Select_24bitAddress(void);
    void SFI_Select_32bitAddress(void);
    void SFI_Select_WaveformMode0(void);              //Only RA8876/RA8887
    void SFI_Select_WaveformMode3(void);              //Only RA8876/RA8887
    void SFI_SelectROM_CS0 (void);
    void SFI_SelectROM_CS1 (void);
    void SFI_SelectROM_CS2(void);
    void SFI_SelectROM_CS3(void);
    inline void SFI_SelectROM_CS_Fast(uint8_t cs_num);
    void SFI_DMA_SourceAddress(uint32_t addr);
    void SFI_DMA_DestinationAddress(uint32_t addr);
    void SFI_DMA_DestinationUpperLeftCorner(uint16_t Wx, uint16_t Hy);
    void SFI_DMA_TransferNumber(uint32_t addr);
    void SFI_DMA_TransferWidthHeight(uint16_t Wx, uint16_t Hy);
    void SFI_DMA_SourceWidth(uint16_t Wx);

    void Power_NormalMode(void);
    void Power_SavingStandbyMode(void);
    void Power_SavingSuspendMode(void);
    void Power_SavingSleepMode(void);

    void BTE_WaitReady(void);
    void BTE_DualWaitReady(void);
    void BTE_PatternFormat8X8(void);
    void BTE_PatternFormat16X16(void);
    void BTE_Enable(bool b);
    void BTE_OperationCode(eBTEOpCode opcode);
    void BTE_S0_ColorDeph(eColorDepthBPP bpp);
    void BTE_S0_ColorDeph_8bpp(void);
    void BTE_S0_ColorDeph_16bpp(void);
    void BTE_S0_ColorDeph_24bpp(void);
    void BTE_S1_ColorDeph_8bpp(void);
    void BTE_S1_ColorDeph_16bpp(void);
    void BTE_S1_ColorDeph_24bpp(void);
    void BTE_S1_ColorDeph_Constant(void);
    void BTE_S1_ColorDeph_8bitAlpha(void);
    void BTE_S1_ColorDeph_16bitAlpha(void);
    void BTE_Destination_ColorDeph(eColorDepthBPP bpp);
    void BTE_Destination_ColorDeph_16bpp(void);
    void BTE_Destination_ColorDeph_24bpp(void);
    void BTE_S0_MemoryStartAddress(uint32_t addr);
    void BTE_S0_ImageWidth(uint16_t Wx);
    void BTE_S0_WindowStart_XY(uint16_t Wx, uint16_t Hy);
    void BTE_S1_MemoryStartAddress(uint32_t addr);
    void S1_ConstantColor_256(uint8_t color);
    void S1_ConstantColor_65k(uint16_t color);
    void S1_ConstantColor_16M(uint32_t color);
    void BTE_S1_ImageWidth(uint16_t Wx);
    void BTE_S1_WindowStart_XY(uint16_t Wx, uint16_t Hy);
    void BTE_Destination_MemoryStartAddress(uint32_t addr);
    void BTE_Destination_ImageWidth(uint16_t Wx);
    void BTE_Destination_WindowStart_XY(uint16_t Wx, uint16_t Hy);
    void BTE_WindowSize(uint16_t Wx, uint16_t Hy);
    void BTE_AlphaBlendingEffect(uint8_t value);
    void BTE_ROPCode(eROPCode code);

    void PIP_Display_StartXY(uint16_t Wx, uint16_t Hy);
    void PIP_Image_StartAddress(uint32_t addr);
    void PIP_Image_Width(uint16_t Wx);
    void PIP_WindowImage_StartXY(uint16_t Wx, uint16_t Hy);
    void PIP_Window_WidthHeight(uint16_t Wx, uint16_t Hy);
    void PIP1_Window_ColorDepth(eColorDepthBPP bpp);
    void PIP1_Window_ColorDepth_8bpp(void);
    void PIP1_Window_ColorDepth_16bpp(void);
    void PIP1_Window_ColorDepth_24bpp(void);
    void PIP2_Window_ColorDepth(eColorDepthBPP bpp);
    void PIP2_Window_ColorDepth_8bpp(void);
    void PIP2_Window_ColorDepth_16bpp(void);
    void PIP2_Window_ColorDepth_24bpp(void);

    void I2CM_Enable(bool b);
    void I2CM_WriteWithStart(void);
    void I2CM_Stop(void);
    void I2CM_ReadWithAck(void);
    void I2CM_ReadWithNack(void);
    void I2CM_Write(void);
    bool I2CM_CheckSlaveACK(void);
    bool I2CM_BusBusy(void);
    uint8_t I2CM_TransmitProgress(void);
    uint8_t I2CM_Arbitration(void);
    void I2CM_ClockPrescale(uint16_t prescale);
    void I2CM_TransmitData(uint8_t data);
    uint8_t I2CM_Receiver_Data(void);
    void I2CM_SetFrequency(uint32_t xscl_hz, uint16_t coreclk_mhz);

    void GPIOA_InOut(uint8_t dir);
    void GPIOA_Write(uint8_t value);
    uint8_t GPIOA_Read(void);
    void GPIOB_Write(uint8_t value);
    uint8_t GPIOB_Read(void);
    void GPIOC_InOut(uint8_t dir);
    void GPIOC_Write(uint8_t value);
    uint8_t GPIOC_Read(void);
    void GPIOD_InOut(uint8_t dir);
    void GPIOD_Write(uint8_t value);
    uint8_t GPIOD_Read(void);
    void GPIOE_InOut(uint8_t dir);
    void GPIOE_Write(uint8_t value);
    uint8_t GPIOE_Read(void);
    void GPIOF_InOut(uint8_t dir);
    void GPIOF_Write(uint8_t value);
    uint8_t GPIOF_Read(void);

    void GPIOF_PullUp_Enable(void);
    void GPIOF_PullUp_Disable(void);
    void GPIOE_PullUp_Enable(void);
    void GPIOE_PullUp_Disable(void);
    void GPIOD_PullUp_Enable(void);
    void GPIOD_PullUp_Disable(void);
    void GPIOC_PullUp_Enable(void);
    void GPIOC_PullUp_Disable(void);
    void XDB15_8_PullUp_Enable(void);
    void XDB15_8_PullUp_Disable(void);
    void XDB7_0_PullUp_Enable(void);
    void XDB7_0_PullUp_Disable(void);

    void XPDAT18_GPIO_D7_Mode(void);
    void XPDAT18_KOUT4_Mode(void);
    void XPDAT17_GPIO_D5_Mode(void);
    void XPDAT17_KOUT2_Mode(void);
    void XPDAT16_GPIO_D4_Mode(void);
    void XPDAT16_KOUT1_Mode(void);
    void XPDAT9_GPIO_D3_Mode(void);
    void XPDAT9_KOUT3_Mode(void);
    void XPDAT8_GPIO_D2_Mode(void);
    void XPDAT8_KIN3_Mode(void);
    void XPDAT2_GPIO_D6_Mode(void);
    void XPDAT2_KIN4_Mode(void);
    void XPDAT1_GPIO_D1_Mode(void);
    void XPDAT1_KIN2_Mode(void);
    void XPDAT0_GPIO_D0_Mode(void);
    void XPDAT0_KIN1_Mode(void);
		
    void KeyScan_LongKeyEnable(bool b = true);
    void KeyScan_Freguency(uint8_t setx);
    void KeyScan_WakeupFunctionEnable(bool b = true);
    void KeyScan_LongKeyTimingAdjust(uint8_t setx);
    uint8_t KeyScan_KeyHits(void);
    uint8_t KeyScan_ReadKeyStrobeData0(void);
    uint8_t KeyScan_ReadKeyStrobeData1(void);
    uint8_t KeyScan_ReadKeyStrobeData2(void);
    uint8_t KeyScan_ReadKeyStrobeData(uint8_t index);

    void XnINTR_ResumeInterrupt_Mask(bool b);
    void XnINTR_ExtInterruptInput_Mask(bool b);
    void XnINTR_I2CMInterrupt_Mask(bool b);
    void XnINTR_VsyncInterrupt_Mask(bool b);
    void XnINTR_KeyScanInterrupt_Mask(bool b);
    void XnINTR_GenericInterrupt_Mask(bool b);
    void XnINTR_PWM1Interrupt_Mask(bool b);
    void XnINTR_PWM0Interrupt_Mask(bool b);

    void AVI_ShadowPIP_StartAddress(uint32_t addr);
    uint8_t Media_Error_Flag(void);
    void Media_DecodeBusy(void);
    void Media_DecodeWaitReady(void);
    uint8_t MediaDecodeBusy(void);
    uint8_t Media_Fifo_Empty(void);
    uint16_t Media_HeaderImageHeight(void);
    uint16_t Media_HeaderImageWidth(void);
    uint32_t AVI_HeaderFramePeriod(void);
    void AVI_Pause(void);
    void AVI_Stop(void);
};

#endif    // fim de include guard RA8889_HPP
