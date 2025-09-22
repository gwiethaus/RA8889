#include "Arduino.h"
#include "SPI.h"
#include "Panel_RA8889.h"


//#include "Panel_RA8889.hpp"


/*
	Notas para Fazer:
	- Determinar a autoconfiguracao das portas MISO, MOSI, CLK da micrcontroladora
	- Determinar manual das portas MISO, MOSI, CLK da micrcontroladora
	- tipo de comunciacao SPI MODO 0, 1, 2 dependedo da microcontroladora
    - Sistema da porta de itnerrupção para a tela de toque do display
	- Verificar a funcao DrawEnable_AA() deve ser do RA8876, pois no RA8889 deve ser zero
	- usar a variavel _bpp, para definir a cores de fundo ao inves de usar ForegroundColor_24bpp, usar ForegroundColor()
	  e detnrod esta funcao seleciona entao o nivel de cores pela variavel. O sistema fica dinamico podendo fazer troca de profundicade no momento desejao, 
	  evitando que isso seja fixo em um display mna hora da compilacao. Principalmente em sistema que requerem otimizacao de cores pelo usuario.
	  
	  marcar na RA8889_51.c a funcao Check_IC_ready equivale a IC_WaitReady
	  
	Tarefas:
    Fazendo as funções: void Show_picture(unsigned long numbers,const unsigned char *datap)
                        void Show_String(char *str)
						


*/


//================================================================================
// Funções auxiliar
//================================================================================




//================================================================================
//
// Funções Principais de Inicializacao
//
//================================================================================


/**
 * @brief Construtor da Classe Panel_RA8889
 *
 * @param None
 * 
 * @note None
 */
Panel_RA8889::Panel_RA8889(uint8_t cs, uint8_t rst)
{
  _cs            = cs;
  _rst           = rst;
  _width         = LCD_HW;
  _height        = LCD_VH;
  _spi_clockmax  = SPI_CLOCK_SPEED_MAX;
  _spi_datamode  = SPI_MODE0;
  _spi_dataorder = MSBFIRST;
  _colorfmt      = static_cast<uint8_t>PDATAColorFmt::RGB; //iniciar com o formato de cor RGB
}


/**
 * @brief Define a resolução do display width x height
 *
 * @param None
 * 
 * @note Mesmo que o RA8889 consiga gerenciar todas estas resoluções
 *       fica limitado pela caracteristica de tela (screen) produzidos
 *       por um fabricante. Para o caso do modelo de display da 
 *       BuyDisplay ER-TFT070-2-6105, a controladora da tela (screen) é
*        o EK9713 800x480/800x600 (1200x960 pixel panel).
 */
void Panel_RA8889::PanelResolution(ePanelResolution resolution)
{
  if (resolution == ePanelResolution::r320x240)  {_width=320;  _height=240;}
  if (resolution == ePanelResolution::r480x272)  {_width=480;  _height=272;}
  if (resolution == ePanelResolution::r640x480)  {_width=640;  _height=480;}
  if (resolution == ePanelResolution::r800x480)  {_width=800;  _height=480;}
  if (resolution == ePanelResolution::r800x600)  {_width=800;  _height=600;}
  if (resolution == ePanelResolution::r960x540)  {_width=960;  _height=540;}
  if (resolution == ePanelResolution::r1024x600) {_width=1024; _height=600;}
  if (resolution == ePanelResolution::r1024x768) {_width=1024; _height=768;}
  if (resolution == ePanelResolution::r1280x768) {_width=1280; _height=768;}
  if (resolution == ePanelResolution::r1280x800) {_width=1280; _height=800;}
  if (resolution == ePanelResolution::r1366x768) {_width=1366; _height=768;}
}


/**
 * @brief Inicializa configurações basicas do display RA8889
 *
 * @verbatim
 * None
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
bool Panel_RA8889::Begin(void)
{

  SPI_Init();                           //inicializa comuncicao SPI
  HardwareReset();                      //Inicia o reset de hardware

  #ifdef CHECK_RA8889
  //Verifica se é um RA8889
  if (readIDCode() != 0x89) {
    Serial.println("RA8889 not found!");
    return false;
  } else { Serial.println("RA8889 connect pass!"); }
  #endif
  
  //Inicializa as configurações basicas do display RA8889
  if(!Initialize()) {
    Serial.println("RA8889 initial fail!");
    return false;
  } else {
    Serial.println("RA8889 initial sucess!");
  }
  
  return true;
}


/**
 * @brief Inicializa a configurações básicas do display RA8889.
 *
 * @verbatim
 * None
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */bool Panel_RA8889::Initialize(void)
{
  uint8_t temp;
  
  PLL_WaitReady();
  
  delay(100);
  
  // Aguarda até que a inicialização interna do RA8889 termine
  // Bit 1 do STSR (0x02) = 1 → inicialização em andamento
  // Bit 1 do STSR (0x02) = 0 → inicialização concluída
  while(StatusRead() & 0x02);
  
  //Configura clock Pixel/SDRAM/Core PLL
  PLL_ConfigClocks(SCAN_FREQ, DRAM_FREQ, CORE_FREQ, OSC_FREQ);
  
  SDRAM_Init();                                //Inicializa a SDRAM
  
//Chip Configuration Register (CCR) [01h]

  //TFT_16bit();
  //HostDataBus_Select_16bit();                  //Host bus 16bit
  //Mask_XnWAIT(false);                          //Wait no mask
  
  SPI_CmdWrite(REG_CCR);                       //0x01, Chip Configuration Register (CCR)
  temp = SPI_DataRead();
  temp |= cSetb7;                              //PLL Enable/reconfigura frequency
  temp &= cClrb6;                              //Mask XnWAIT
  temp &= cClrb5;                              //Key Scan Disable
  temp |= cSetb4;                              //TFT 16-bits Panel I/F Output
  temp &= cClrb3;                              //
  temp &= cClrb2;                              //I2C Master disable
  temp |= cSetb1;                              //Serial Flash or SPI Interface Enable
  temp |= cSetb0;                              //16-bit Host Data Bus Width Selection
  SPI_DataWrite(temp);

//Memory Access Control Register (MACR) [02h]

  //HostColorDepthFormat(0);                     //Host MPU/MCU I/F 8/16 bits color depth 8/16/24 mode 1
  //HostReadMemoryDirection(MemoryDirection::LeftRight_TopBotom);

  SPI_CmdWrite(REG_MACR);                      //0x02, Memory Access Control Register (MACR)
  temp = SPI_DataRead();
  temp &= cClrb7;                              //Diret Write (for all 8 bits MPU I/F, 16 bits MPU I/F with 8bpp data mode 1 & 2, 16 bits MPU I/F with 16/24-bpp data mode 1 & serial host interface)
  temp &= cClrb6;                              //
  temp &= cClrb5;                              //Host Read Memory Direction (Only for Graphic Mode) Left → Right then Top → Bottom.
  temp &= cClrb4;
  temp &= cClrb2;                              //Host Write Memory Direction (Only for Graphic Mode) Left → Right then Top →Bottom. (Original)
  temp &= cClrb1;
  SPI_DataWrite(temp);

//Input Control Register (ICR) [03h]

  //GraphicMode();
  //MemorySelect_SDRAM();

  SPI_CmdWrite(REG_ICR);                       //0x03, Input Control Register (ICR)
  temp = SPI_DataRead();
  temp &= cClrb2;                              //Graphic mode
  temp &= cClrb1;                              //Memory port Read/Write Destination Selection, Image buffer (SDRAM) for image data, pattern, user-characters. Support Read-modify-Write.
  temp &= cClrb0;                              //
  SPI_DataWrite(temp);

#ifdef COLOR_DEPTH_16
  //Select_MainWindow_16bpp();
  //PIP1_Window_ColorDepth(eColorDepthBPP::bpp16);
  //PIP2_Window_ColorDepth(eColorDepthBPP::bpp16);
  //Memory_16bpp_BlockMode();                    //Set 16bpp Block mode
  //Memory_BlockMode();                          //Set Block mode (X-Y coordination addressing)
  _bpp =  eColorDepthBPP::bpp16;               //Indica que selecionou Color depth 16bpp

//Main/PIP Window Control Register (MPWCTR) [10h]
  SPI_CmdWrite(REG_MACR);                      //0x10, Main/PIP Window Control Register (MPWCTR)
  temp = SPI_DataRead();
  temp &= cClrb7;                              //Desabilita o Picture-in-Picure (PIP 1) do display
  temp &= cClrb6;                              //Desabilita o Picture-in-Picure (PIP 2) do display
  temp &= cClrb4;                              //Select Configure PIP 1 Window’s parameters
  temp &= cClrb3;                              //Main image color depth 16 bpp
  temp |= cSetb2;                              //
  temp &= cClrb0                               //Sync Mode: Enable XVSYNC, XHSYNC, XDE
  SPI_DataWrite(temp);
//PIP Window Color Depth Setting (PIPCDEP) [11h]
  SPI_CmdWrite(REG_PIPCDEP);                   //0x11, PIP Window Color Depth Setting (PIPCDEP)
  temp = SPI_DataRead();
  temp &= cClrb3;                              //PIP 1 Window Color Depth Setting, 16-bpp generic TFT, i.e. 65K colors.
  temp |= cSetb2;                              //
  temp &= cClrb1;                              //PIP 2 Window Color Depth Setting, 16-bpp generic TFT, i.e. 65K colors.
  temp |= cSetb0                               //
  SPI_DataWrite(temp);
//Color Depth of Canvas & Active Window (AW_COLOR) [5Eh]
  SPI_CmdWrite(REG_AW_COLOR);                   //0x5e, Color Depth of Canvas & Active Window (AW_COLOR)
  temp = SPI_DataRead();
  temp &= cClrb2;                              //Canvas addressing block x-y mode
  temp &= cClrb1;                              //Canvas image’s color depth & memory R/W data width in block mode x-y, 16bpp
  temp |= cSetb0                               //
  SPI_DataWrite(temp);
//Source 0/1 & Destination Color Depth (BTE_COLR) [92h]
  SPI_CmdWrite(REG_BTE_COLR);                   //0x92, Source 0/1 & Destination Color Depth (BTE_COLR)
  temp &= cClrb6;                               //S0 Color Depth 16bpp
  temp |= cSetb5;                               //
  temp &= cClrb4;                               //S1 Color Depth 16bpp
  temp &= cClrb3;                               //
  temp |= cSetb2;                               //
  temp &= cClrb1;                               //Destination Color Depth 16bpp
  temp |= cSetb0;                               //
  temp = SPI_DataRead();
#endif

#ifdef COLOR_DEPTH_24
  //Select_MainWindow_24bpp();
  //PIP1_Window_ColorDepth(eColorDepthBPP::bpp24);
  //PIP2_Window_ColorDepth(eColorDepthBPP::bpp24);
  //Memory_24bpp_BlockMode();                    //Set 16bpp Block mode
  //Memory_BlockMode();                          //Set Block mode (X-Y coordination addressing)
  _bpp =  eColorDepthBPP::bpp24;               //Indica que selecionou Color depth 24bpp

//Main/PIP Window Control Register (MPWCTR) [10h]
  SPI_CmdWrite(REG_MACR);                      //0x10, Main/PIP Window Control Register (MPWCTR)
  temp = SPI_DataRead();
  temp &= cClrb7;                              //Desabilita o Picture-in-Picure (PIP 1) do display
  temp &= cClrb6;                              //Desabilita o Picture-in-Picure (PIP 2) do display
  temp &= cClrb4;                              //Select Configure PIP 1 Window’s parameters
  temp |= cSetb3;                              //Main image color depth 24 bpp
  temp &= cClrb2;                              //
  temp &= cClrb0                               //Sync Mode: Enable XVSYNC, XHSYNC, XDE
  SPI_DataWrite(temp);
//PIP Window Color Depth Setting (PIPCDEP) [11h]
  SPI_CmdWrite(REG_PIPCDEP);                   //0x11, PIP Window Color Depth Setting (PIPCDEP)
  temp = SPI_DataRead();
  temp |= cSetb3;                              //PIP 1 Window Color Depth Setting, 24-bpp generic TFT, i.e. 1.67M colors.
  temp &= cClrb2;                              //
  temp |= cSetb1;                              //PIP 2 Window Color Depth Setting, 24-bpp generic TFT, i.e. 1.67M colors.
  temp &= cClrb0                               //
  SPI_DataWrite(temp);
//Color Depth of Canvas & Active Window (AW_COLOR) [5Eh]
  SPI_CmdWrite(REG_AW_COLOR);                   //0x5e, Color Depth of Canvas & Active Window (AW_COLOR)
  temp = SPI_DataRead();
  temp &= cClrb2;                              //Canvas addressing block x-y mode
  temp |= cSetb1;                              //Canvas image’s color depth & memory R/W data width in block mode x-y, 24bpp
  temp &= cClrb0                               //
  SPI_DataWrite(temp);
//Source 0/1 & Destination Color Depth (BTE_COLR) [92h]
  SPI_CmdWrite(REG_BTE_COLR);                   //0x92, Source 0/1 & Destination Color Depth (BTE_COLR)
  temp |= cSetb6;                               //S0 Color Depth 24bpp
  temp &= cClrb5;                               //
  temp &= cClrb4;                               //S1 Color Depth 24bpp
  temp |= cSetb3;                               //
  temp &= cClrb2;                               //
  temp |= cSetb1;                               //Destination Color Depth 24bpp
  temp &= cClrb0;                               //
  temp = SPI_DataRead();
#endif

//Display Configuration Register (DPCR) [12h]
//Panel scan Clock and Data Setting Register (PCSR) [13h]
//Horizontal Display Width Register (HDWR) [14h]
//Horizontal Display Width Fine Tune Register (HDWFTR) [15h]
//Vertical Display Height Register 0(VDHR0) [1Ah]
//Vertical Display Height Register 1 (VDHR1) [1Bh]
//Horizontal Non-Display Period(HNDR) [16h]
//Horizontal Non-Display Period Fine Tuning(HNDFT) [17h]
//HSYNC Start Position Register (HSTR) [18h]
//HSYNC Pulse Width Register (HPWR) [19h]
//Vertical Non-Display Period Register 0(VNDR0) [0x1c]
//Vertical Non-Display Period Register 1(VNDR1) [0x1d]
//VSYNC Pulse Width Register (VPWR) [0x1f]

  LCD_SetPanel();                              //Configuração do Panel Screen LCD, de acordo com o tipo do fabricante
  
//Image buffer configure

  MainImage_StartAddress( LayerStartAddr(0) );
  MainImage_Width(_width); 
  MainWindow_StartXY(0,0);
  CanvasImage_StartAddr( LayerStartAddr(0) );
  CanvasImage_Width(_width);
  ActiveWindow_XY(0,0);
  ActiveWindow_WidhtHeight(_width, _height);  
  
}


/**
 * @brief Retorna a largura da tela
 *
 * @param None
 *
 * @note None
 */
uint16_t Panel_RA8889::Width(void)
{
  return _width;
}


/**
 * @brief Retorna o comprimento da tela
 *
 * @param None
 *
 * @note None
 */
uint16_t Panel_RA8889::Height(void)
{
  return _height;
}


/**
 * @brief Inicializa a configuração da Tela LCD (Screen) de acordo com a 
 *        montagem feita pelo fornecedor do display.
 *        Informações baseada no IC do drive da tela utilziada no display.
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::LCD_SetPanel(void)
{

#ifdef EK9713                                  //Fitipower EK9713 800x600/800x480
   
  //Display Configuration Register (DPCR) [12h]
  
  DisplayOn(false);                            //Display off
  HScanDirection_LeftToRight();                //HSCAN Left to Right
  VScanDirection_TopToBottom();                //VSCAN Top to Bottom
  PDATA_ColorFmt(PDATAColorFmt::RGB);          //Select RGB output
  PCLK_EdgeType(PCLKEdge::Falling);            //LCD PCLK Falling

  //Panel scan Clock and Data Setting Register (PCSR) [13h]

  HSYNC_Polarity(HSYNCPolarity::Low);
  VSYNC_Polarity(VSYNCPolarity::Low);
  DE_Polarity(DEPolarity::High);

  //Horizontal Display Width Register (HDWR) [14h]
  //Horizontal Display Width Fine Tune Register (HDWFTR) [15h]
  //Vertical Display Height Register 0(VDHR0) [1Ah]
  //Vertical Display Height Register 1 (VDHR1) [1Bh]
  
  HorizontalWidth_VerticalHeight(LCD_HW, LCD_VH);
  
  //Seta a resolução do display baseado no painel
  
  _width  = LCD_HW;
  _height = LCD_VH;
	
  //Minha notas:
  //Estas funcoes são de caracteristica da tela LCD (Screen) gerenciado pelo
  //RA8889, no entanto o RA8889 controla o tipo de tela. Cada fabricante de 
  //tela possui seus determinados valores tipicos de Blanking horiz/Vert,  
  //HSYNC/VSYNC largura de pulso e front porch que precisam ser grenciados 
  //pelo RA8889.

  //Horizontal Non-Display Period(HNDR) [16h]
  //Horizontal Non-Display Period Fine Tuning(HNDFT) [17h]
  //HSYNC Start Position Register (HSTR) [18h]
  //HSYNC Pulse Width Register (HPWR) [19h]

  Horizontal_NonDisplay(LCD_HBPD);             //(HS Blanking)
  HSYNC_StartPosition(LCD_HFPD);               //(HS Front Porch)                  
  HSYNC_PulseWidth(LCD_HSPW);                  //(HS Pulse Width)
                            
  //Vertical Non-Display Period Register 0(VNDR0) [0x1c]
  //Vertical Non-Display Period Register 1(VNDR1) [0x1d]
  //VSYNC Pulse Width Register (VPWR) [0x1f]
 
  Vertical_NonDisplay(LCD_VBPD);               //(VS Blanking)
  VSYNC_StartPosition(LCD_VFPD);               //(VS Front Porch)    
  VSYNC_PulseWidth(LCD_VSPW);                  //(VS pulse width)
  
#endif

}


//================================================================================
// Funções SPI
//================================================================================


//Inicializa o SPI para a comunicacao com o Display RA8889
void Panel_RA8889::SPI_Init(void)
{
  pinMode(_cs, OUTPUT);
  SPI.beginTransaction(SPISettings(_spi_clockmax, MSBFIRST, SPI_MODE0));
  SPI.begin();
}


/**
 * @brief Controla o pino Chip Select (CS) do barramento SPI.
 *
 * Esta função ativa ou desativa o dispositivo SPI conectado ao pino CS.
 * É utilizada para garantir que apenas um dispositivo SPI esteja ativo
 * no barramento por vez, evitando conflitos de comunicação.
 *
 * @verbatim
 * A lógica de controle do CS é:
 * - Nível LOW (0): Ativa o dispositivo (CS ativo).
 * - Nível HIGH (1): Desativa o dispositivo (CS inativo/liberado).
 * @endverbatim
 * 
 * @param level_cs O nível a ser definido para o pino CS:
 *        - 0: Ativa o dispositivo (CS em LOW).
 *        - 1: Desativa o dispositivo (CS em HIGH).
 *
 * @note O pino CS é tratado como "ativo baixo". Portanto, um nível LOW
 * (representado por 0) "bloqueia" o barramento para este dispositivo
 * específico, permitindo operações de escrita/leitura. Um nível HIGH
 * (representado por 1) "libera" o barramento para que outros dispositivos
 * possam utilizá-lo.
 */
void Panel_RA8889::SPISetCS(uint8_t level_cs)
{
  level_cs == 0 ? digitalWrite(_cs, LOW) : /*SS_RESET */  digitalWrite(_cs, HIGH); /*SS_SET*/
}


uint8_t Panel_RA8889::SPIRwByte(uint8_t value)
{
  uint8_t result;
  result = SPI.transfer(value);
  return result;
}


//SPI_CommandWrite
//Escreve um comando para o SPI do Display
void Panel_RA8889::SPI_CmdWrite(uint8_t cmd)
{
  SPISetCS(0);                                 //SS_RESET;
  SPIRwByte(RA8889_SPI_CMDWRITE);              //0x00, Indica Commando para escrever 
  SPIRwByte(cmd);                              //Envia um comando byte para o SPI 
  SPISetCS(1);                                 //SS_SET;
}


//SPI_DataWrite
//Escreve dados para o SPI
void Panel_RA8889::SPI_DataWrite(uint8_t data)
{
  SPISetCS(0);                                 //SS_RESET;
  SPIRwByte(RA8889_SPI_DATAWRITE);             //0x80, Indica Dados para escrever
  SPIRwByte(data);                             //Envia um byte de Dado para o SPI
  SPISetCS(1);                                 //SS_SET;
}


void Panel_RA8889::SPI_DataWrite16bpp(uint16_t data)
{
  SPISetCS(0);                                 //SS_RESET;
  SPIRwByte(RA8889_SPI_DATAWRITE);             //0x80, Indica Dados para escrever
  SPIRwByte(data);                             //Envia um byte menos significativo de Dado para o SPI
  SPIRwByte(data >> 8);                        //Envia um byte mais significativo de Dado para o SPI
  SPISetCS(1);                                 //SS_SET;
}


void Panel_RA8889::SPI_DataWrite24bpp(uint32_t data)
{
  SPISetCS(0);                                 //SS_RESET;
  SPIRwByte(RA8889_SPI_DATAWRITE);             //0x80, Indica Dados para escrever
  SPIRwByte(data);                             //Envia byte 1 de Dado para o SPI
  SPIRwByte(data >> 8);                        //Envia byte 2 de Dado para o SPI
  SPIRwByte(data >> 16);                       //Envia byte 3 de Dado para o SPI
  SPISetCS(1);                                 //SS_SET;
}


//SPI_DataWritePixel
void Panel_RA8889::SPI_DataWrite_Pixel(uint16_t data)
{
  SPISetCS(0);                                 //SS_RESET;
  SPIRwByte(RA8889_SPI_DATAWRITE);             //0x80, Indica Dados para escrever
  SPIRwByte(data);                             //Escreve a parte baixa da palavra
  SPISetCS(1);                                 //SS_SET;
											   
  SPISetCS(0);                                 //SS_RESET;
  SPIRwByte(RA8889_SPI_DATAWRITE);             //0x80, Indica Dados para escrever
  SPIRwByte(data >> 8);                        //Escreve a parte alta da palavra
  SPISetCS(1);                                 //SS_SET;
}


//SPI_DataRead
//Ler um byte de dados no SPI
uint8_t Panel_RA8889::SPI_DataRead(void)
{
  uint8_t temp = 0;
  SPISetCS(0);                             //SS_RESET;
  SPIRwByte(RA8889_SPI_DATAREAD);          //0xc0, Leitura de dados
  temp = SPIRwByte(0x00);
  SPISetCS(1);                             //SS_SET;
  return temp;
}


//================================================================================
// Comandos para o Display
//================================================================================


//StatusRead
//Leia o estado do registrado STSR
uint8_t Panel_RA8889::StatusRead(void)
{
  int temp = 0;
  SPISetCS(0);                            //SS_RESET;
  SPIRwByte(RA8889_SPI_STATUSREAD);       //0x40, Read Status SPI
  temp = SPIRwByte(REG_STSR);             //Read STSR Register
  SPISetCS(1);                            //SS_SET;
  return temp;
}


//escreve valor no registrador do display
void Panel_RA8889::RegisterWrite(uint8_t Cmd, uint8_t Data)
{
  SPI_CmdWrite(Cmd);
  SPI_DataWrite(Data);
}


//Ler valor do registrador do display
uint8_t Panel_RA8889::RegisterRead(uint8_t Cmd)
{
  uint8_t result = 0;
  SPI_CmdWrite(Cmd);
  temp = SPI_DataRead();
  return result;
}


/**
 * @brief Executa um reset de hardware no RA8889 através do pino RESET.
 *
 * @verbatim
 * Mantém o pino de reset (configurado em _rst) em nível baixo por 500 ms para
 * garantir que o chip seja reinicializado, depois volta a nível
 * alto por mais 500 ms para concluir o processo de reset.
 *
 * Aplicação:
 * Esse procedimento força o RA8889 a retornar ao estado inicial,
 * sendo útil quando o PLL ou a inicialização interna falham.
 * @endverbatim
 * 
 * @param None
 *
 * @return None 
 */
void Panel_RA8889::HardwareReset(void)
{
  pinMode(_rst, OUTPUT);
  digitalWrite(_rst, LOW);
  delay(500);
  digitalWrite(_rst, HIGH);
  delay(500);
}


/**
 * @brief Aguarde o sistema estaeja em modo de operação normal
 *        apos uma inicilização, ou termine de sair do estado de 
 *        economia de energia.
 *
 * @verbatim
 *        Status Register (STSR)
 *        bit [1] Operation mode status
 *                0b0 : Normal operation state  → inicialização concluída
 *                0b1 : Inhibit operation state → inicialização em andamento
 *                      Inhibit operation state means internal reset event 
 *                      keep running or initial display still running or chip 
 *                      enter power saving state.
 *
 *        Aplicação: Até que a inicialização do IC (Core) tenha terminado uma 
 *                   operação de inicialização (rest) ou um retorno de uma 
 *                   economia de energia.
 * @endverbatim
 *
 * @param true:  IC em modo operação normal e pronto,
 *        false: IC ainda não concluiu a inicializacao
 *
 * @note None
 */
bool Panel_RA8889::IC_WaitReady(void)
{
  uint8_t temp;
  for(uint32_t i = 0; i < 1000000; i++) { //de acordo com o uso, altere o valor de i.
    temp = StatusRead();
    if( (temp & 0x02) == 0x00 ) {return true;}
	delayMicroseconds(1);
  }
  return false;
}


/**
 * @brief Aguarde até as tarefas do sistema estejam prontas
 *              
 * @verbatim
 *        Status Register (STSR)
 *        bit [3] Core task is busy
 *                0b0 : task is done or idle.
 *                0b1 : task is busy
 *                      Following task is running:
 *                      BTE, Geometry engine, Serial flash DMA, Text write 
 *                      or Graphic write.
 *
 *        Aplicação: Quando o envio de certos comandos o controlador fica 
 *                   ocupado até o momento de terminar a tarefa. Envie este 
 *                   comando para aguardar até que a tarefa saia no modo 
 *                   ocupado (busy) para o modo ocioso (idle) ou feito (done)
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::CoreTask_WaitReady(void)
{
  for(uint32_t i = 0; i < 1000000; i++) {  //Ajuste valor de i de acordo com a necessidade
    if((StatusRead() & 0x08) == 0x00) {break;}
    delayMicroseconds(1);
  } 
}


/**
 * @brief Aguarde até as tarefas de desenho grafico e texto estejam prontos
 *              
 * @verbatim
 * Status Register (STSR)
 * bit [3] Core task is busy
 *         0b0 : task is done or idle.
 *         0b1 : task is busy
 *               Following task is running:
 *               BTE, Geometry engine, Serial flash DMA, Text write 
 *               or Graphic write.
 *
 * Aplicação: Quando o envio de certos comandos o controlador fica 
 *            ocupado até o momento de terminar a tarefa. Envie este 
 *            comando para aguardar até que a tarefa saia no modo 
 *            ocupado (busy) para o modo ocioso (idle) ou feito (done)
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::Draw_WaitReady(void)  { CoreTask_WaitReady(); }


/**
 * @brief Aguarde até que a memória de escrita FIFO tenha algo nela
 *              
 * @verbatim
 * Status Register (STSR)
 * bit [7] Host Memory Write FIFO full
 *         0b0 : Memory Write FIFO is not full.
 *         0b1 : Memory Write FIFO is full.
 * 
 * Only when Memory Write FIFO is not full, MPU may write another one 
 * pixel.
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::Wait_WriteFIFO_NotFull(void)
{  
  uint8_t temp = 0;
  for(uint16_t i = 0; i < 10000; i++) {        //Ajuste valor de i de acordo com a necessidade
    temp = StatusRead();
    if( (temp & 0x80) == 0x00 ){break;}
  }
}


/**
 * @brief Aguarde até que a memória de escrita FIFO esteja livre para escrever
 *              
 * @verbatim
 *        Status Register (STSR)
 *        bit [6] Host Memory Write FIFO empty
 *                0b0 : Memory Write FIFO is not empty.
 *                0b1 : Memory Write FIFO is empty.
 * 
 *        When Memory Write FIFO is empty, MPU may write 8bpp data 64
 *        pixels, or 16bpp data 32 pixels, 24bpp data 16 pixels directly.
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::Wait_WriteFIFO_Empty(void)
{
  uint8_t temp = 0;
  for(uint16_t i = 0; i < 10000; i++) {        //Ajuste valor de i de acordo com a necessidade
    temp = StatusRead();    
    if( (temp & 0x40) == 0x40 ) { break; }
  }
}


/**
 * @brief Aguarde até que a memória de leitura FIFO Não esteja cheio
 *              
 * @verbatim
 *        Status Register (STSR)
 *        bit [5] Host Memory Read FIFO full
 *                0b0 : Memory Read FIFO is not full.
 *                0b1 : Memory Read FIFO is full.
 *        
 *        When Memory Read FIFO is full, MPU may read 8bpp data 32
 *        pixels, or 16bpp data 16 pixels, 24bpp data 8 pixels directly.
 *
 *        Dizer que algo nao está cheio, quer dizer que está pela metade,
 *        nem vazio, nem cheio.
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::Wait_ReadFIFO_NotFull(void)
{
  uint8_t temp = 0;
  for(uint16_t i = 0; i < 10000; i++) {        //Ajuste valor de i de acordo com a necessidade
    temp = StatusRead();
    if( (temp & 0x20) == 0x00 ){ break; }
  }
}


/**
 * @brief Aguarde até que a memória de leitura FIFO tenha algo para ler
 *              
 * @verbatim
 *        Status Register (STSR)
 *        bit [4] Host Memory Read FIFO empty
 *                0b0 : Memory Read FIFO is not empty.
 *                0b1 : Memory Read FIFO is empty.
 * 
 *        Dizer que algo não está vazio, quer dizer que tem alguma coisa 
 *        ou esta completo (totalmente cheio)
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::Wait_ReadFIFO_NotEmpty(void)
{ 
  uint8_t temp = 0;
  for(uint16_t i = 0; i < 10000; i++) {        //Ajuste valor de i de acordo com a necessidade
    temp = StatusRead();    
    if( (temp & 0x10) == 0x00 ) { break; }
  }
}


/**
 * @brief Verifica se é um RA8889
 *        ATENÇÂO: Ainda não foi testado este codigo
 *
 * @verbatim
 * Para ler o ID code precisa desabilitar o PLL, 0x01 bit 7 setando para 0.
 * 
 * A RAIO em alguns de seus chipos como o RA8876/RA8877 e RA8889 possuem 
 * páginas de registradores 0 e 1. Para poder ler o Id nestes controladores, 
 * precisa acessar a página de registradores 1.
 *
 * O RA8875 apresenta todos na mesma pagina de registradrores.
 *
 * Chip       ID esperado (REG 0xFF)
 * ---------------------------------
 * RA8875     0x75
 * RA8877     0x76
 * RA8876     0x76
 * RA8889     0x89
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @return true indica que reconheceu o RA8889
 */
uint8_t Panel_RA8889::RA8889_ReadIDCode(void)
 { 
  uint8_t datareg = 0;
  uint8_t temp;
  datareg = RegisterRead(REG_CCR);               //Ler o registrador 0x01
  temp = datareg;  
  temp &= cClrb7;                                //Desligar o PLL momentaneamente
  RegisterWrite(REG_CCR, temp);                  //0x01, Chip Configuration Register (CCR)
  delay(1);
  PageSwitch(ePageReg::Page1);                   //Muda para pagina de registradores 1
  temp = RegisterRead(0xff);                     //Registrador não documentado, para ID do Chip
  PageSwitch(ePageReg::Page0);                   //Retorna para  pagina de registradores 1
  delay(1);
  RegisterWrite(REG_CCR, datareg);               //Devolve o status original do 0x01, Chip Configuration Register (CCR)
  return (temp&0x89);                            //O ID do RA8889, tem que retornar 0x89
 }








//================================================================================
// Funcoes PLL
//================================================================================


/**
 * @brief Aguarda até que o RA8889 finalize sua inicialização interna e o PLL esteja pronto.
 * 
 * @verbatim
 * Fluxo:
 *  - Verifica o registrador de status (STSR) para saber se a inicialização interna terminou.
 *  - Quando terminado, acessa o Chip Configuration Register (CCR) e checa se o PLL (bit 7) está pronto.
 *  - Se o PLL não estiver pronto, reconfigura-o e tenta novamente.
 *  - Caso o sistema não responda após várias tentativas, executa um reset de hardware e repete o processo.
 * 
 * A função só retorna quando o sistema está estável e pronto para operar.
 * @endverbatim
 */
void Panel_RA8889::PLL_WaitReady(void)
{
  uint8_t count_timeout = 0;
  uint8_t temp = 0;
  bool system_ok = false;
  
  do {
    temp = StatusRead();                       //Read Status Register STSR
    if((temp & 0x02) == 0x00) {                //Veja se o bit 2 esta limpo (0x00=modo de operação normal, evento de inicialização interna terminou)
	    delay(2);                                //MCU too fast, necessary
      SPI_CmdWrite(0x01);                      //Access register Chip Configuration Register (CCR)
      delay(2);                                //MCU too fast, necessary
      temp = SPI_DataRead();                   //Leia o CCR 
      if((temp & 0x80) == 0x80) {               //Check CCR register's PLL is ready or not (bit 7 = 1)
        system_ok = true;                      //PLL pronto
        count_timeout = 0;
      } else {
        delay(2);                              //MCU too fast, necessary
        SPI_CmdWrite(0x01);                    //Access register Chip Configuration Register (CCR)
        delay(2);                              //MCU too fast, necessary
        SPI_DataWrite(0x80);                   //Reconfigura a frequencia do PLL
      }
	  
    } else {                          
      system_ok = false;                       //A inicialização interna ainda está sendo feita
      count_timeout++;                         //fazer outra tentativa
    }
	
    if(system_ok==false && count_timeout==5) { //Sistema ainda nao está pronto e houve timeout
      HardwareReset();                         //*note1, envia um reset novamente
      count_timeout = 0;                       //zera o cotnador de timeout 
    }
	
  } while(system_ok==false);                   //faz enquanto não ficar pronto o sistema
}


/**
 * @brief Configura o PLL do RA8889 para ajustar frequências de Pixel Clock, 
 *        SDRAM Clock e Core Clock
 * 
 * @verbatim
 * Configura o PLL do RA8889 para ajustar as frequências de:
 * - Pixel Clock (SCAN_FREQ)
 * - SDRAM Clock (DRAM_FREQ)
 * - Core Clock (CORE_FREQ)
 *
 * REG[05h] SCLK PLL Control Register 1 (PPLLC1) - SCAN or PIXEL Clock PLL
 * REG[07h] MCLK PLL Control Register 1 (MPLLC1) - MEMORY Clock PLL
 * REG[09h] CCLK PLL Control Register 1 (SPLLC1) - CORE or SYSTEM Clock PLL
 *
 * A função calcula automaticamente o divisor e multiplicador adequado
 * com base no cristal externo (OSC_FREQ) e nas frequências alvo.
 * 
 * Fluxo seguro:
 * 1. Desliga o PLL temporariamente
 * 2. Configura os registros de divisores/multiplicadores
 * 3. Habilita o PLL com os novos valores
 *
 *
 * (1) OSC_FREQ = 10MHz 
 * (2) 10MHz <= (OSC_FREQ/PLLDIVM) <= 40MHz
 * (3) 250MHz <= [OSC_FREQ/(PLLDIVM+1)]x(PLLDIVN+1) <= 500MHz
 *     PLLDIVM:0
 *     PLLDIVN:1~63
 *     PLLDIVK:CPLL & MPLL = 1/2/4/8.SPLL = 1/2/4/8/16/32/64/128.
 *     example:
 *       OSC_FREQ = 10MHz
 *       Set X_DIVK=2
 *       Set X_DIVM=0
 *       => (X_DIVN+1)=(XPLLx4)/10
 * @endverbatim
 *
 * @param scanclk : PSCK Panel Scan clock frequency, unit: MHz
 * @param sdramclk: SDRAM clock frequency, unti: MHz	
 * @param coreclk : Core (system) clock frequency, unit: MHz 
 * @param xtalclk : Crystal clock, unit: MHz 
 */
bool Panel_RA8889::PLL_ConfigClocks(int8_t scanclk, int8_t dramclk, int8_t coreclk, int8_t xtalclk)
{
  
  // ---------- Set Pixel/Scan Clock ----------
 
  if(scanclk>=63)
  {
	SPI_CmdWrite(REG_PPLLC1);                  //0x05 
	SPI_DataWrite(0x04);                       //PLL Divided by 4
	SPI_CmdWrite(REG_PPLLC2);                  //0x06
	SPI_DataWrite((scanclk*4/xtalclk)-1);      //Deve ser de 1~63, 0 é proibido
  }                                            
  if((scanclk>=32)&&(scanclk<=62))         
  {                                            
	SPI_CmdWrite(REG_PPLLC1);                  //0x05    
	SPI_DataWrite(0x06);                       //PLL Divided by 8
	SPI_CmdWrite(REG_PPLLC2);                  //0x06
	SPI_DataWrite((scanclk*8/xtalclk)-1);      //Deve ser de 1~63, 0 é proibido
  }                                            
  if((scanclk>=16)&&(scanclk<=31))         
  {                                            
	SPI_CmdWrite(REG_PPLLC1);                  //0x05     
	SPI_DataWrite(0x16);                       //PLL Divided by 16
	SPI_CmdWrite(REG_PPLLC2);                  //0x06
	SPI_DataWrite((scanclk*16/xtalclk)-1);     //Deve ser de 1~63, 0 é proibido
  }                                            
  if((scanclk>=8)&&(scanclk<=15))          
  {                                            
	SPI_CmdWrite(REG_PPLLC1);                  //0x05    
	SPI_DataWrite(0x26);                       //PLL Divided by 32
	SPI_CmdWrite(REG_PPLLC2);                  //0x06
	SPI_DataWrite((scanclk*32/xtalclk)-1);     //Deve ser de 1~63, 0 é proibido
  }                                            
  if((scanclk>0)&&(scanclk<=7))            
  {                                            
	SPI_CmdWrite(REG_PPLLC1);                  //0x05    
	SPI_DataWrite(0x36);                       //PLL Divided by 64
	SPI_CmdWrite(REG_PPLLC2);                  //0x06
	SPI_DataWrite((scanclk*64/xtalclk)-1);     //Deve ser de 1~63, 0 é proibido
  }            
  
  // ---------- Set SDRAM clock ----------

  if(dramclk>=125)
  {
	SPI_CmdWrite(REG_MPLLC1);                  //0x07 
	SPI_DataWrite(0x02);                       //PLL Divided by 2
	SPI_CmdWrite(REG_MPLLC2);                  //0x08
	SPI_DataWrite((dramclk*2/xtalclk)-1);      //Deve ser de 1~63, 0 é proibido
  }
  if((dramclk>=63)&&(dramclk<=124))
  {
	SPI_CmdWrite(REG_MPLLC1);                  //0x07     
	SPI_DataWrite(0x04);                       //PLL Divided by 4
	SPI_CmdWrite(REG_MPLLC2);                  //0x08
	SPI_DataWrite((dramclk*4/xtalclk)-1);      //Deve ser de 1~63, 0 é proibido
  }
  if((dramclk>=31)&&(dramclk<=62))
  {           
	SPI_CmdWrite(REG_MPLLC1);                  //0x07     
	SPI_DataWrite(0x06);                       //PLL Divided by 8
	SPI_CmdWrite(REG_MPLLC2);                  //0x08
	SPI_DataWrite((dramclk*8/xtalclk)-1);      //Deve ser de 1~63, 0 é proibido
  }
  if(dramclk<=30)
  {
	SPI_CmdWrite(REG_MPLLC1);                  //0x07   
	SPI_DataWrite(0x06);                       //PLL Divided by 8
	SPI_CmdWrite(REG_MPLLC2);                  //0x08
	SPI_DataWrite((30*8/xtalclk)-1);           //Deve ser de 1~63, 0 é proibido
  }
 
  // ---------- Set Core clock ----------
  
  if(coreclk>=125)
  {
	SPI_CmdWrite(REG_SPLLC1);                  //0x09
	SPI_DataWrite(0x02);                       //PLL Divided by 2
	SPI_CmdWrite(REG_SPLLC2);                  //0x0A
	SPI_DataWrite((coreclk*2/xtalclk)-1);      //Deve ser de 1~63, 0 é proibido
  }
  if((coreclk>=63)&&(coreclk<=124))     
  {
	SPI_CmdWrite(REG_SPLLC1);                  //0x09   
	SPI_DataWrite(0x04);                       //PLL Divided by 4
	SPI_CmdWrite(REG_SPLLC2);                  //0x0A
	SPI_DataWrite((coreclk*4/xtalclk)-1);      //Deve ser de 1~63, 0 é proibido
  }
  if((coreclk>=31)&&(coreclk<=62))
  {           
	SPI_CmdWrite(REG_SPLLC1);                  //0x09  
	SPI_DataWrite(0x06);                       //PLL Divided by 8
	SPI_CmdWrite(REG_SPLLC2);                  //0x0A
	SPI_DataWrite((coreclk*8/xtalclk)-1);      //Deve ser de 1~63, 0 é proibido
  }
  if(coreclk<=30)
  {
	SPI_CmdWrite(REG_SPLLC1);                  //0x09   
	SPI_DataWrite(0x06);                       //PLL Divided by 8
	SPI_CmdWrite(REG_SPLLC2);                  //0x0A
	SPI_DataWrite((30*8/xtalclk)-1);           //Deve ser de 1~63, 0 é proibido
  }

  //PLL_Enable();
  //O fabricante permite que o dispositivo, possa fazer as configurações do PLL sem estar desligado ou desativado o PLL
  //basta delsigar momentaneamente e religar ele para entrar em vigo as configurações de frequencia.
  //Isso é feito apra simplificar e facilitar a progarmação e arquitetura sem gerar instabilidade no disposiutivo.
  
  // ---------- Desliga temporariamente o PLL ----------
  SPI_CmdWrite(REG_CCR);                       //0x01, Envia comando Chip Configuration Register (CCR) 
  SPI_CmdWrite(0x00);                          //Como o CCR possui tudo zerado por default ainda na inicilizacao e configuração do dispositivo, o bit 7 será zerado (inicia com 1 como default)
  delay(1);                                    //Aguarda para estabilizar
  
  // ---------- Habilita PLL com novos valores ----------
  SPI_CmdWrite(0x80);                          //Comando para ligar PLL
  delay(2);                                    //Aguarda para estabilizar

  (SPI_DataRead() & 0x80) == 0x80 ? return true : return false;
}


//================================================================================
//
//Funcoes de DMA
//
//================================================================================



void Panel_RA8889::DMA_24bit_Block (
                                    uint8_t SCS,   // SCS: Select SPI : SCS��0       SCS��1
                                    uint8_t Clk,   // Clk: SPI Clock = System Clock /{(Clk+1)*2}
                                    uint16_t X1,   // X1: Transfer to SDRAM address:X1
                                    uint16_t Y1,   // Y1: Transfer to SDRAM address:Y1
                                    uint16_t X_W,  // X_W: DMA data width
                                    uint16_t Y_H,  // Y_H: DMA data height
                                    uint16_t P_W,  // P_W: Picture's width
                                    uint32_t Addr  // Addr: Flash address
                                   )
{
  SFlashSPI_Enable(true);

  if(SCS == 0)  SFI_Select_ROM0();                       // Select SPI0
  if(SCS == 1)  SFI_Select_ROM1();                       // Select SPI1

  Select_SFI_DualData_3Bh();                             //Dual mode 0
  Memory_XYMode();                     
  Select_SFI_DMAMode();                                  // Select SPI DMA mode
  SPI_Clock_Period(Clk);                                 // Select SPI clock

  GotoPixel_XY(X1, Y1);                                  // Setting the location of memory in the graphic mode
  SFI_DMA_DestinationUpperLeftCorner(X1, Y1);            // DMA destination(SDRAM address)
  SFI_DMA_TransferWidthHeight(X_W, Y_H);                 // Setting Block data: width&height
  SFI_DMA_SourceWidth(P_W);                              // Setting the width of the source data
  SFI_DMA_SourceAddress(Addr);                           // Setting the FLASH address of the source data

  SFI_DMA_Start();                                       //
  SFI_DMA_WaitReady();                                   //
}



//================================================================================
//
// Funcoes SDRAM
//
//================================================================================


/**
 * @brief Aguarda até que a SDRAM do RA8889 esteja pronta para acesso.
 *
 * Esta função realiza leituras repetidas do registrador de status (STSR) 
 * do RA8889 até que o bit 2 esteja definido como '1', indicando que a 
 * memória SDRAM já está inicializada e pronta para operações de leitura 
 * e escrita. 
 *
 * Uso típico: deve ser chamada após a inicialização da SDRAM ou 
 * antes de qualquer operação que dependa do acesso estável à memória.
 * 
 * Status Register (STSR) 
 * Bit [2] SDRAM ready for access
 *         0: SDRAM não está pronta para acesso.
 *         1: SDRAM pronta para acesso.
 * Before user check this bit staus , user must be set ”sdr_initdone” bit as 1
 *
 */
bool Panel_RA8889::SDRAM_WaitReady(void)
{
  uint8_t temp = 0;
  for (unsigned long i = 0; i < 1000000; i++) {
	  temp = StatusRead();
	  if ((temp & 0x04) == 0x04) return true;
    delayMicroseconds(1);
  }
  return false;
}


/**
 * @brief Inicializa a memória SDRAM externa usada pelo RA8889.
 *
 * Esta função configura os parâmetros necessários para o controlador SDRAM do RA8889,
 * incluindo a latência CAS e o intervalo de refresh, de acordo com a frequência de operação
 * (64 MHz) e os requisitos típicos da SDRAM (8192 ciclos de refresh a cada 64 ms).
 *
 * Passos executados:
 *  - Define o modo de operação da SDRAM.
 *  - Configura a latência CAS (3 ciclos).
 *  - Calcula e ajusta o intervalo de refresh com base no clock do sistema.
 *  - Grava os valores de refresh nos registradores correspondentes.
 *  - Ativa a SDRAM e aguarda até que esteja pronta para uso.
 *
 * @note Deve ser chamada apenas uma vez na inicialização do sistema, 
 *       antes de qualquer acesso à memória SDRAM (framebuffer).
 */
void Panel_RA8889::SDRAM_Init(void)
{
  uint16_t sdram_itv;  //autorefresh
  
  //0xe0, SDRAM attribute register (SDRAR)
  //Configura o modo da SDRAM
  //  0x29 = indica parâmetros como largura do barramento e o tipo de refresh.
  //  SDRAM Bank number (sdr_bank)      bit 5=1b     -> uso 4 bancos
  //  SDRAM Row addressing (sdr_row)    bit 4-3=01b  -> 4096 (A0-A11) 
  //  SDRAM Column addressing (sdr_col) bit 2-0=001b -> 512 (A0-A8)
  RegisterWrite(REG_SDRAR, 0x29);            
    
  //0xe1, SDRAM mode register & extended mode register (SDRMD)
  //Define a latência CAS (Column Address Strobe latency).
  //  0x03 = Define a latência CAS (Column Address Strobe latency)
  //  SDRAM CAS latency (sdr-caslat)    bit 2-0 CAS:2 010b=0x02 -> 2 ciclos CAS:3 011b=0x03 -> 3 ciclos  
  RegisterWrite(REG_SDRMD, 0x03);
  
  //Set SDRAM refresh interval via SDRAM auto refresh interval registers
  
  //Calcula o intervalo de refresh da SDRAM.
  // - A maioria das SDRAM precisa de 8192 ciclos de refresh em 64 ms.
  // - Esse cálculo pega o clock de 64 MHz, divide pelo número de linhas (8192), e ajusta para a taxa de atualização (60 Hz).
  // - O -2 é um ajuste de margem para não ficar no limite.
  sdram_itv = (64000000 / 8192) / (1000/60);
  sdram_itv-=2;
  
  //este faz sentido com a literatura de outros que usaram
  //o sdram_itv acima, foi testado no Arduinio e funcionou
  //sdram_itv = (64 * DRAM_FREQ * 1000) / (4096);
  //sdram_itv-=2;
  
  //0xe2, SDRAM auto refresh interval (SDR_REF_ITVL0) - Byte low
  RegisterWrite(REG_SDR_REF_ITVL0, sdram_itv); //envia byte menos significativo da palavra
  
  //0xe3, SDRAM auto refresh interval (SDR_REF_ITVL1) - Byte high
  RegisterWrite(REG_SDR_REF_ITVL1, sdram_itv >> 8); //envia byte mais significativo da palavra
  
  //0xe4, SDRAM Control register (SDRCR)
  //  0x01 = Iniciar procedimento de inicialização da SDRAM (sdr_initdone)
  RegisterWrite(REG_SDRCR, 0x01);
  
  //Espera até que a SDRAM esteja pronta
  SDRAM_WaitReady();
  delay(1);
}


//================================================================================
//
// [0x01] Chip Configuration Register (CCR)
//
//================================================================================



/**
 * @brief Mask XnWAIT on XnCS deassert
 *
 *
 * REG [0x01] Chip Configuration Register (CCR)
 *            bit [6] Mask XnWAIT on XnCS deassert
 *            0b0: No mask
 *            XnWAIT keep assert if internal state keep busy and cannot accept 
 *            next R/W cycle, no matter XnCS assert/deassert. If MPU cycle 
 *            cannot be extended while XnWAIT keep low, user should poll 
 *            XnWAIT and wait it goes high then start next access.
 *            0b1: Mask
 *            XnWAIT deassert when XnCS deassert. Use in MPU cycle can be 
 *            extended by XnWAIT automatically.
 *            
 * 
 * Em oturas palacras, quando bit=6 estiver reetado, independente da MCU soltar o 
 * xnCS, antes de começar a próxima escrita/leitura você precisa verificar se o 
 * XnWAIT já voltou para alto (liberado).
 *
 * @param wait: true wait, false no wait
 *
 */
void PanelRA8889::Mask_XnWAIT(bool mask)
{
  uint8_t temp;
  SPI_CmdWrite(REG_CCR);                       //0x01, Chip Configuration Register (CCR)
  temp = SPI_DataRead();
  temp &= cClrb6;                              //Reset bit 6 Mask XnWAIT on XnCS deassert
  mask ? temp |= cSetb6 : temp &= cClrb6;     
  SPI_DataWrite(temp);	
}


/**
 * @brief Configuração do pino de saída 24-bits da interface (I/F) do painel TFT
 *
 * Escreve no registrador 0x01 Chip Configuration Register (CCR)
 *   bit [4-3]
 *   00b: Saída TFT 24-bits
 *   01b: Saída TFT 18-bits
 *   10b: Saída TFT 16-bits
 *   11b: Sem Nenhuma Saída TFT
 * 
 * Outros pinos de saída TFT não utilizados são definidos como GPIO ou função de tecla.
 *
 * @param None
 *
 */
void Panel_RA8889::TFT_24bit(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_CCR);                       //0x01, Chip Configuration Register (CCR)
  temp = SPI_DataRead();
  temp &= cClrb4;                              //Reset bit 4
  temp &= cClrb3;                              //Reset bit 3
  SPI_DataWrite(temp);
}



/**
 * @brief Configuração do pino de saída 24-bits da interface (I/F) do painel TFT
 *
 * Escreve no registrador 0x01 Chip Configuration Register (CCR)
 *   bit [4-3]
 *   00b: Saída TFT 24-bits
 *   01b: Saída TFT 18-bits
 *   10b: Saída TFT 16-bits
 *   11b: Sem Nenhuma Saída TFT
 * 
 * Outros pinos de saída TFT não utilizados são definidos como GPIO ou função de tecla.
 *
 * @param None
 *
 */
void Panel_RA8889::TFT_24bit(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_CCR);                       //0x01, Chip Configuration Register (CCR)
  temp = SPI_DataRead();
  temp &= cClrb4;                              //Reset bit 4
  temp &= cClrb3;                              //Reset bit 3
  SPI_DataWrite(temp);
}


/**
 * @brief Configuração do pino de saída 18-bits da interface (I/F) do painel TFT
 *
 * Escreve no registrador 0x01 Chip Configuration Register (CCR)
 *   bit [4-3]
 *   00b: Saída TFT 24-bits
 *   01b: Saída TFT 18-bits
 *   10b: Saída TFT 16-bits
 *   11b: Sem Nenhuma Saída TFT
 * 
 * Outros pinos de saída TFT não utilizados são definidos como GPIO ou função de tecla.
 *
 * @param None
 *
 */
void Panel_RA8889::TFT_18bit(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_CCR);                       //0x01, Chip Configuration Register (CCR)
  temp = SPI_DataRead();
  temp &= cClrb4;                              //Reset bit 4
  temp |= cSetb3;                              //Set bit 3
  SPI_DataWrite(temp);
}


/**
 * @brief Configuração do pino de saída 18-bits da interface (I/F) do painel TFT
 *
 * Escreve no registrador 0x01 Chip Configuration Register (CCR)
 *   bit [4-3]
 *   00b: Saída TFT 24-bits
 *   01b: Saída TFT 18-bits
 *   10b: Saída TFT 16-bits
 *   11b: Sem Nenhuma Saída TFT
 * 
 * Outros pinos de saída TFT não utilizados são definidos como GPIO ou função de tecla.
 *
 * @param None
 *
 */
void Panel_RA8889::TFT_16bit(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_CCR);                       //0x01, Chip Configuration Register (CCR)
  temp = SPI_DataRead();
  temp |= cSetb4;                              //Set bit 4
  temp &= cClrb3;                              //Reset bit 3 
  SPI_DataWrite(temp);
}


/**
 * @brief Configuração do pino sem nenhuma saída interface (I/F) do painel TFT
 *
 * Desativa o uso de interface do painel de TFT
 *
 * Escreve no registrador 0x01 Chip Configuration Register (CCR)
 *   bit [4-3]
 *   00b: Saída TFT 24-bits
 *   01b: Saída TFT 18-bits
 *   10b: Saída TFT 16-bits
 *   11b: Sem Nenhuma Saída TFT
 * 
 * Outros pinos de saída TFT não utilizados são definidos como GPIO ou função de tecla.
 *
 * @param None
 *
 */
void Panel_RA8889::TFT_Without(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_CCR);                       //0x01, Chip Configuration Register (CCR)
  temp = SPI_DataRead();
  temp |= cSetb4;                              //Set bit 4
  temp |= cSetb3;                              //Set bit 3
  SPI_DataWrite(temp);
}


/**
 * @brief Configuração do pino de saída de interface (I/F) do painel TFT
 *
 *        Escreve no registrador 0x01 Chip Configuration Register (CCR)
 *        bit [4-3] 0b00: Saída TFT 24-bits
 *                  0b01: Saída TFT 18-bits
 *                  0b10: Saída TFT 16-bits
 *                  0b11: Sem Nenhuma Saída TFT
 * 
 *        Outros pinos de saída TFT não utilizados são definidos como GPIO ou 
 *        função de tecla.
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::TFT_Interface(TFTInterface mode)
{
  uint8_t temp;
  SPI_CmdWrite(REG_CCR);                       //0x01, Chip Configuration Register (CCR) 
  temp = SPI_DataRead();
  temp &= cClrb4;                              //Reset bits 4
  temp &= cClrb3;                              //Reset bits 3
  temp |= static_cast<uint8_t>(mode);          //Converte enum para uint8_t
  SPI_DataWrite(temp);
}


/**
 * @brief Parallel Host Data Bus 8-bit Width Selection
 *
 * Escreve no registrador 0x01 Chip Configuration Register (CCR)
 *   bit [0]
 *   0b: 8-bit Parallel Host Data Bus
 *   1b: 16-bit Parallel Host Data Bus
 *
 * @param None
 *
 * @note Uso em MCU de 16-bit com: 
 *         - color depth 8-bpp
 *         - color depth 16-bpp
 *         - color depth 24-bpp
 */
void Panel_RA8889::HostDataBus_Select_8bit(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_CCR);                       //0x01, Chip Configuration Register (CCR) 
  temp = SI_DataRead();
  temp &= cClrb0;                              //Reset bit 0
  SPI_DataWrite(temp);
}


/**
 * @brief Parallel Host Data Bus 16-bit Width Selection
 *
 * Escreve no registrador 0x01 Chip Configuration Register (CCR)
 *   bit [0]
 *   0b: 8-bit Parallel Host Data Bus
 *   1b: 16-bit Parallel Host Data Bus
 *
 * @param None
 *
 * @note Uso em MCU de 16-bit com: 
 *         - color depth 16-bpp
 *         - color depth 24-bpp Mode 1
 *         - color depth 24-bpp Mode 2
 */
void Panel_RA8889::HostDataBus_Select_16bit(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_CCR);                       //0x01, Chip Configuration Register (CCR) 
  temp = SI_DataRead();
  temp |= cSetb0;                              //Set bit 0
  SPI_DataWrite(temp);
}


/**
 * @brief Ativa ou desativa a interface SPI da Serial Flash
 *
 * @verbatim
 *	      REG [01h] Chip Configuration Register (CCR)  
 *                  bit [1] Serial Flash or SPI Interface Enable/Disable
 *                          0b0: Disable (GPIO function)
 *                          0b1: Enable (SPI master function)
 *           
 *                  When SDR SDRAM 32bits bus function enable, this bit is ignored
 *                  & Serial flash pins become SDR SDRAM bus function.
 * @endverbatim
 * 
 * @code
 * SFlashSPI_Enable(true);
 * @endcode
 *
 * @param b: true/false
 * 
 * @note None
 */
void Panel_RA8889::SFlashSPI_Enable(bool b)
{
  unsigned char temp;
  SPI_CmdWrite(REG_CCR);                       //0x01, Chip Configuration Register (CCR) 
  temp = SPI_DataRead();
  if (b) temp |= cSetb1 else temp &= cClrb1;
  SPI_DataWrite(temp);
}


//================================================================================
//
// [0x02] Memory Access Control Register (MACR)
//
//================================================================================


/**
 * @brief Host Read/Write Image Data Format
 *
 * REG [0x02] Memory Access Control Register (MACR)
 *        MPU/MCU read/write data format when access memory data port.      
 *        bit [7-6] Data format setting: MCU interface, color depth
 *        0b0x: Direct write for all 8 bits MPU I/F, 16 bits MPU I/F with 
 *              16bpp, 16 bits MPU I/F with 24bpp data mode 1 and serial host 
 *              interface.
 *        0b10: Mask high byte of each data (ex. 16 bit MPU I/F with 8-bpp data mode 1)
 *        0b11: Mask high byte of even data (ex. 16 bit MPU I/F with 24-bpp data mode 2)
 *
 * @param 0: Aplicado a todas as MCU e color depth
 *        1: Apenas para MCU de 16bit com Color Depth de 24bpp no Modo 2
 *
 * @note type=0 - Uso em MCU de 8/16-bit:
 *              - MCU 8-bit color depth 8-bpp
 *              - MCU 8-bit color depth 16-bpp
 *              - MCU 8-bit color depth 24-bpp
 *              - MCU 16-bit color depth 16-bpp
 *              - MCU 16-bit color depth 24-bpp Mode 1
 *
 *       type=1 - Uso em MCU de 16-bit:
 *              - MCU 16-bit color depth 24-bpp Mode 2
 */
void Panel_RA8889::HostColorDepthFormat(uint8_t type)
{
  uint8_t temp;
  SPI_CmdWrite(REG_MACR);                      //0x02, Memory Access Control Register (MACR)
  temp = SPI_DataRead();
  if (type == 0) temp &= cClrb7;               //Reset bit 7
  if (type == 1) {
	  temp |= cSetb7;                           //Set bit 7
	  temp |= cSetb6;                           //Set bit 7
  }
  SPI_DataWrite(temp);                         //Mask high byte of each data (ex. 16 bit MPU I/F with 8-bpp data mode 1)
}


/**
 * @brief Host Read Memory Direction (Only for Graphic Mode)
 *        Video memory read direction setting
 *        
 *        Efeito somente no modo gráfico
 *
 *        bit [5-4] 0b00: Left to Right then Top to Bottom
 *                  0b01: Right to Left then Top to Bottom
 *                  0b10: Top to Bottom then Left to Right
 *                  0b11: Bottom to Top then Left to Right
 *
 * @param MemoryDirection::LeftRight_TopBotom 
 *        MemoryDirection::RightLeft_TopBotom
 *        MemoryDirection::TopBotom_LeftRight
 *        MemoryDirection::BotomTop_LeftRight 
 *
 *
 * @note Only for Graphic Mode. Graphics mode can be activated after using 
 *       this function.
 */
void Panel_RA8889::HostReadMemoryDirection(MemoryDirection direction)
{
  uint8_t temp;
  SPI_CmdWrite(REG_MACR);                      //0x02, Memory Access Control Register (MACR)
  temp = SPI_DataRead();
  temp &= ~(cSetb5 | cSetb4);                  //Reset bit 5 e 4
  temp |= (static_cast<uint8_t>(direction) << 4);  //posiciona o valor para o bit 5 e 4
  SPI_DataWrite(temp);                         //Host Read Memory Direction
}


/**
 * @brief Host Write Memory Direction (Only for Graphic Mode)
 *        Video memory write direction setting
 *        
 *        Efeito somente no modo gráfico
 *
 *        REG [02h] Memory Access Control Register (MACR)
 *                  bit [2-1] 0b00: Left to Right then Top to Bottom (Original)
 *                            0b01: Right to Left then Top to Bottom (Horizontal flip)
 *                            0b10: Top to Bottom then Left to Right (Rotate right 90˚& Horizontal flip)
 *                            0b11: Bottom to Top then Left to Right (Rotate left 90˚)
 *
 * @param MemoryDirection::LeftRight_TopBotom 
 *        MemoryDirection::RightLeft_TopBotom
 *        MemoryDirection::TopBotom_LeftRight
 *        MemoryDirection::BotomTop_LeftRight 
 *
 *
 * @note Only for Graphic Mode. Ignored if canvas in linear addressing mode.
 */
void Panel_RA8889::HostWriteMemoryDirection(MemoryDirection direction)
{
  uint8_t temp;
  SPI_CmdWrite(0x02);
  temp = SPI_DataRead();
  temp &= ~(cSetb2 | cSetb1);                         //Reset bit 2 e 1
  temp |= (static_cast<uint8_t>(direction) << 1);     //posiciona o valor para o bit 2 e 1
  SPI_DataWrite(temp);
}


//================================================================================
//
// [0x03] Input Control Register (ICR)
//
//================================================================================


/**
 * @brief Seleciona o nível lógico que a interrupção fica ativa
 *
 *        REG[0x03] Input Control Register (ICR)
 *                  bit [7] Output to MPU Interrupt pin’s active level
 *                         0b0 : active low.
 *                         0b1 : active high.
 *
 * @param eInterruptLevel::Low
 *        eInterruptLevel::High
 *
 */
void Panel_RA8889::Interrupt_ActiveLevel(eInterruptLevel level)
{
  uint8_t temp;
  SPI_CmdWrite(REG_ICR);                       //0x03, Input Control Register (ICR)
  temp = SPI_DataRead();                       
  temp &= cClrb7;
  temp |= static_cast<uint8_t>(level);         //Define o nível
  SPI_DataWrite(temp);
}


/**
 * @brief External interrupt input (XPS[0] pin) with de-bounce
 *
 *        REG[0x03] Input Control Register (ICR)
 *                  bit [6] External interrupt input (XPS[0] pin) de-bounce
 *                         0b0 : without de-bounce
 *                         0b1 : enable de-bounce (1024 OSC clock)	
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::ExtInterrupt_Debounce(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_ICR);                       //0x03, Input Control Register (ICR)
  temp = SPI_DataRead();                       
  temp |= cSetb6;
  SPI_DataWrite(temp);
}


/**
 * @brief External interrupt input (XPS[0] pin) without de-bounce
 *
 *        REG[0x03] Input Control Register (ICR)
 *                  bit [6] External interrupt input (XPS[0] pin) de-bounce
 *                          0b0 : without de-bounce
 *                          0b1 : enable de-bounce (1024 OSC clock)	
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::ExtInterrupt_NoDebounce(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_ICR);                       //0x03, Input Control Register (ICR)
  temp = SPI_DataRead();                       
  temp &= cSetb6;
  SPI_DataWrite(temp);
}


/**
 * @brief External interrupt input (XPS[0] pin) trigger type
 *
 *        REG[0x03] Input Control Register (ICR)
 *                  bit [5-4] External interrupt input (XPS[0] pin) trigger type
 *                            0b00 : low level trigger
 *                            0b01 : falling edge trigger
 *                            0b10 : high level trigger
 *                            0b11 : rising edge trigger
 *                            
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::ExtInterrupt_InputLevelTrigger(eInterrupLevelTrigger leveltrg)
{
  uint8_t temp;
  SPI_CmdWrite(REG_ICR);                       //0x03, Input Control Register (ICR)
  temp = SPI_DataRead();                       
  temp &= ~(cSetb5 | cSetb4);
  temp |= (static_cast<uint8_t>(leveltrg) << 3); 
  SPI_DataWrite(temp);
}


/**
 * @brief Select LVDS Data Format VESA Standard
 *
 *        REG[0x03] Input Control Register (ICR)
 *                  bit [3] FPD-Link Data Format / LVDS Data Format
 *                          0b0 : Format 1 (VESA format) --- use with displays expecting the 2 MSB to be transmitted over the 4th data channel Y3.
 *                          0b1 : Format 2 (JEIDA format) --- use with displays expecting the 2 LSB to be transmitted over the 4th data channel Y3.
 *                            
 *
 * @param None
 *
 * @note RA8877 only
 */
void Panel_RA8889::LVDS_DataFormat_VESA(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_ICR);                       //0x03, Input Control Register (ICR)
  temp = SPI_DataRead();                       
  temp &= cClrb3;
  SPI_DataWrite(temp);
}


/**
 * @brief Select LVDS Data Format JEIDA Standard
 *
 *        REG[0x03] Input Control Register (ICR)
 *                  bit [3] FPD-Link Data Format / LVDS Data Format
 *                          0b0 : Format 1 (VESA format) --- use with displays expecting the 2 MSB to be transmitted over the 4th data channel Y3.
 *                          0b1 : Format 2 (JEIDA format) --- use with displays expecting the 2 LSB to be transmitted over the 4th data channel Y3.
 *                            
 *
 * @param None
 *
 * @note RA8877 only
 */
void Panel_RA8889::LVDS_DataFormat_JEIDA(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_ICR);                       //0x03, Input Control Register (ICR)
  temp = SPI_DataRead();                       
  temp |= cSetb3;
  SPI_DataWrite(temp);
}


/**
 * @brief Muda o display para modo grafico
 *
 * @param None
 *
 * @note None
 *
 */
//
void Panel_RA8889::GraphicMode(void){
  uint8_t temp;
  SPI_CmdWrite(REG_ICR);                       //0x03, Input Control Register (ICR)
  temp = SPI_DataRead();                       //Lê valor atual do registrador
  temp &= cClrb2;                              //Reset bit 2
  SPI_DataWrite(temp);                         //Ativa modo grafico
}


/**
 * @brief Verifica se o modo grafico está ativo
 *
 * @param None
 *
 * @note None
 *
 * @return true: Modo Grafico, false: Modo texto
 */
bool Panel_RA8889::IsGraphicMode(void){
  uint8_t temp;
  SPI_CmdWrite(REG_ICR);                       //0x03, Input Control Register (ICR)
  temp = SPI_DataRead();                       //Lê valor atual do registrador
  return ((temp == cSetb2) == 0x00);           //Verificar bit 2 está desligado
}


/**
 * @brief Muda o display para modo texto
 *
 * @param None
 *
 * @note None
 *        
 */
void Panel_RA8889::TextMode(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_ICR);                       //0x03, Input Control Register (ICR)
  temp = SPI_DataRead();                       //Lê valor atual do registrador
  temp |= cSetb2;                              //Set bit 2
  SPI_DataWrite(temp);                         //Ativa o modo texto
}


/**
 * @brief Seleciona o destino da porta de memória do RA8889 para a SDRAM.
 *
 * Configura os bits [1:0]=00b do registrador ICR (0x03)
 *
 * @param Nenhum
 *
 * @note Image buffer (SDRAM) for image data, pattern (palette), user-characters. 
 *        
 */
void Panel_RA8889::MemorySelect_SDRAM(void)
{
  uint8_t temp = 0;
  SPI_CmdWrite(REG_ICR);               //0x03, Input Control Register (ICR)
  temp = SPI_DataRead();               //Lê valor atual do registrador
  temp &= cClrb0;                      //Clear bit 0
  temp &= cClrb1;                      //Clear bit 1
  SPI_DataWrite(temp);                 //Atualiza registrador
}


/**
 * @brief Seleciona o destino da porta de memória do RA8889 para Tabela Gama.
 *
 * Configura os bits [1:0]=01b do registrador ICR (0x03)
  *
 * @param Nenhum
 *
 * @note Tabela Gama para cores Vermelho/Verde/Azul.
 *        
 */
void Panel_RA8889::MemorySelect_GammaTable(void)
{
  uint8_t temp = 0;
  SPI_CmdWrite(REG_ICR);               //0x03, Input Control Register (ICR)
  temp = SPI_DataRead();               //Lê valor atual do registrador
  temp &= cClrb1;                      //Clear bit 1 
  temp |= cSetb0;                      //Set bit 0
  SPI_DataWrite(temp);                 //Atualiza registrador
}


/**
 * @brief Seleciona o destino da porta de memória do RA8889 para Cursor Gráfico.
 *
 * Configura os bits [1:0]=10b do registrador ICR (0x03)
 *
 * @param Nenhum
 *
 * @note RAM do Cursor Gráfico (aceita apenas dados MPU de 8 bits, leitura e 
 *       gravação de dados de registradores normais semelhantes), não suporta 
 *       leitura de RAM do Cursor Gráfico. Contém 4 conjuntos de cursores 
 *       gráficos. Cada conjunto tem 128x16 bits. O usuário precisa especificar 
 *       o conjunto de cursores gráficos de destino e continuar a gravação de 
 *       256 bytes..
 */
void Panel_RA8889::MemorySelect_GraphicCursorRAM(void)
{
  uint8_t temp = 0;
  SPI_CmdWrite(REG_ICR);               //0x03, Input Control Register (ICR)
  temp = SPI_DataRead();               //Lê valor atual do registrador
  temp |= cSetb1;                      //Set bit 1  
  temp &= cClrb0;                      //Clear bit 0
  SPI_DataWrite(temp);                 //Atualiza registrador
}


/**
 * @brief Seleciona o destino da porta de memória do RA8889 para RAM de Palette.
 *
 * Configura os bits [1:0]=11b do registrador ICR (0x03)
  *
 * @param Nenhum
 *
 * @note RAM do Cursor Gráfico (aceita apenas dados MPU de 8 bits, leitura e 
 *       gravação de dados de registradores normais semelhantes), não suporta 
 *       leitura de RAM do Cursor Gráfico. Contém 4 conjuntos de cursores 
 *       gráficos. Cada conjunto tem 128x16 bits. O usuário precisa especificar 
 *       o conjunto de cursores gráficos de destino e continuar a gravação de 
 *       256 bytes..
 */
void Panel_RA8889::MemorySelect_ColorPaletteRAM(void)
{
  uint8_t temp = 0;
  SPI_CmdWrite(REG_ICR);               //0x03, Input Control Register (ICR)
  temp = SPI_DataRead();               //Lê valor atual do registrador
  temp |= cSetb1;                      //Set bit 1
  temp |= cSetb0;                      //Set bit 0
  SPI_DataWrite(temp);                 //Atualiza registrador
}


/**
 * @brief Seleciona o destino da porta de memória do RA8889.
 *
 * @verbatim
 * Configura os bits [1:0] do registrador ICR (0x03) de acordo com
 * o destino escolhido.
 *
 * Exemplo: MemoryPort_Select(MemoryPortDest::SDRAM);
 *          Este exemplo é o memo que suar a funcao Memory_Select_SDRAM();
 * @endverbatim
 *
 * @param dest Destino da porta de memória (SDRAM, Tabela Gama, Cursor Grafico e Palete)
 *
 */
void Panel_RA8889::MemoryPort_Select(MemoryPortDest dest)
{
  uint8_t temp;
  SPI_CmdWrite(REG_ICR);                       //0x03, Input Control Register (ICR)
  temp = SPI_DataRead();                       //Lê valor atual do registrador
  temp &= 0xfc;                                //Reset bit 1 e 0
  temp |= static_cast<uint8_t>(dest);          //
  SPI_DataWrite(temp);                         //Atualiza registrador
}


//================================================================================
//
// [0x0B] Interrupt Enable Register (INTEN)
//
//================================================================================


/**
 * @brief Interrupt Resume Enable/Disable
 *
 *        REG [0x0B] Interrupt Enable Register (INTEN)
 *                   bit [7] Wakeup/resume Interrupt Enable
 *                           0: Disable.
 *                           1: Enable.
 *
 * @param b: true Active Interrupt, False Deactive Interrupt
 *
 */
void Panel_RA8889::Interrupt_Resume_Enable(bool b)
{
  uint8_t temp;
  SPI_CmdWrite(REG_INTEN);                     //0x0b, Interrupt Enable Register (INTEN)
  temp = SPI_DataRead();
  b ? temp |= cSetb7 : temp &= cClrb7;
  SPI_DataWrite(temp);
}


/**
 * @brief External Interrupt Input Enable/Disable
 *
 *        REG [0x0B] Interrupt Enable Register (INTEN)
 *                   bit [6] External Interrupt (PS[0] pin) Enable
 *                           0: Disable.
 *                           1: Enable.
 *
 * @param b: true Active Interrupt, False Deactive Interrupt
 *
 */
void Panel_RA8889::ExtInterrupt_Input_Enable(bool b)
{
  uint8_t temp;
  SPI_CmdWrite(REG_INTEN);     //0x0b, Interrupt Enable Register (INTEN)
  temp = SPI_DataRead();
  b ? temp |= cSetb6 : temp &= cClrb6;
  SPI_DataWrite(temp);
}


/**
 * @brief Interrupt I2C Master Enable/Disable
 *
 *        REG [0x0B] Interrupt Enable Register (INTEN)
 *                   bit [5] I2C Master Interrupt Enable
 *                           0: Disable.
 *                           1: Enable.
 *
 * @param b: true Active Interrupt, False Deactive Interrupt
 *
 */
void Panel_RA8889::Interrupt_I2CM_Enable(bool b)
{
  uint8_t temp;
  SPI_CmdWrite(REG_INTEN);     //0x0b, Interrupt Enable Register (INTEN)
  temp = SPI_DataRead();
  b ? temp |= cSetb5 : temp &= cClrb5;
  SPI_DataWrite(temp);
}


/**
 * @brief Interrupt Vertical Synchronization time base Enable/Disable
 *
 *        REG [0x0B] Interrupt Enable Register (INTEN)
 *                   bit [4] Vsync time base interrupt Enable Bit
 *                           0: Disable Interrupt.
 *                           1: Enable Interrupt.
 *
 * @param b: true Active Interrupt, False Deactive Interrupt
 *
 */
void Panel_RA8889::Interrupt_VSync_Enable(bool b)
{
  uint8_t temp;
  SPI_CmdWrite(REG_INTEN);     //0x0b, Interrupt Enable Register (INTEN)
  temp = SPI_DataRead();
  b ? temp |= cSetb4 : temp &= cClrb4;
  SPI_DataWrite(temp);
}


/**
 * @brief Interrupt Key Scan Enable/Disable
 *
 * @verbatim
 * REG [0x0B] Interrupt Enable Register (INTEN)
 *            bit [3] Key Scan Interrupt Enable Bit
 *                    0: Disable Key scan interrupt.
 *                    1: Enable Key scan interrupt.
 * @endverbatim
 *
 * @param b: true Active Interrupt, False Deactive Interrupt
 *
 */
void Panel_RA8889::Interrupt_KeyScan_Enable(bool b)
{
  uint8_t temp;
  SPI_CmdWrite(REG_INTEN);     //0x0b, Interrupt Enable Register (INTEN)
  temp = SPI_DataRead();
  b ? temp |= cSetb3 : temp &= cClrb3;
  SPI_DataWrite(temp);
}


/**
 * @brief Interrupt Serial Flash DMA, Draw Task, BTE Process Complete, etc. Enable/Disable
 *
 * @verbatim
 * REG [0x0B] Interrupt Enable Register (INTEN)
 *            bit [2] Serial flash DMA Complete | Draw task finished | 
 *                    BTE Process Complete etc. Interrupt Enable
 *                    0: Disable interrupt.
 *                    1: Enable interrupt.
 * @endverbatim
 *
 * @param b: true Active Interrupt, False Deactive Interrupt
 *
 */
void Panel_RA8889::Interrupt_ClearMultiEventTask_Enable(bool b)
{
  uint8_t temp;
  SPI_CmdWrite(REG_INTEN);                     //0x0b, Interrupt Enable Register (INTEN)
  temp = SPI_DataRead();
  b ? temp |= cSetb2 : temp &= cClrb2;
  SPI_DataWrite(temp);
}


/**
 * @brief Interrupt PWM Timer 1 Enable/Disable
 *
 * @verbatim
 * REG [0x0B] Interrupt Enable Register (INTEN)
 *            bit [1] PWM timer 1 Interrupt Enable Bit
 *                    0: Disable interrupt.
 *                    1: Enable interrupt.
 * @endverbatim
 *
 * @param b: true Active Interrupt, False Deactive Interrupt
 *
 */
void Panel_RA8889::Interrupt_PWM1_Enable(bool b)
{
  uint8_t temp;
  SPI_CmdWrite(REG_INTEN);                     //0x0b, Interrupt Enable Register (INTEN)
  temp = SPI_DataRead();
  b ? temp |= cSetb1 : temp &= cClrb1;
  SPI_DataWrite(temp);
}


/**
 * @brief Interrupt PWM Timer 0 Enable/Disable
 *
 *        REG [0x0B] Interrupt Enable Register (INTEN)
 *                   bit [0] PWM timer 0 Interrupt Enable Bit
 *                           0: Disable interrupt.
 *                           1: Enable interrupt.
 *
 * @param b: true Active Interrupt, False Deactive Interrupt
 *
 */
void Panel_RA8889::Interrupt_PWM0_Enable(bool b)
{
  uint8_t temp;
  SPI_CmdWrite(REG_INTEN);                     //0x0b, Interrupt Enable Register (INTEN)
  temp = SPI_DataRead();
  b ? temp |= cSetb0 : temp &= cClrb0;
  SPI_DataWrite(temp);
}


//================================================================================
//
// [0x0C] Interrupt Event Flag Register (INTF)
//  *If you received an interrupt but cannot identify it on Interrupt Event Flag 
//  Register, please check SPI master status register’s interrupt flag bits 
//  REG[BAh].
//
//================================================================================


/**
 * @brief Read Interrupt Status
 *        
 * @verbatim
 * REG [0x0c] Interrupt Event Flag Register (INTF)
 *            Bit [7] Read Function ..Resume Interrupt Status
 *                    0: No Resume interrupt happens.
 *                    1: Resume interrupt happens.
 *            Bit [6] Read Function .. PS[0] pin Interrupt Status
 *                    0: No PS[0] pin interrupt happens.
 *                    1: PS[0] pin interrupt happens.
 *            Bit [5] Read Function .. I2C master Interrupt Status
 *                    0: No I2C master interrupt happens.
 *                    1: I2C master interrupt happens.
 *            Bit [4] Read Function .. Vsync Interrupt Status
 *                    0: No interrupt happens.
 *                    1: interrupt happens.
 *            Bit [3] Read Function ..Key Scan Interrupt Status
 *                    0: No Key Scan interrupt happens.
 *                    1: Key Scan interrupt happens.
 *            Bit [2] Read Function..Interrupt Status
 *                    0: No interrupt happens.
 *                    1: interrupt happens.
 *            Bit [1] Read Function..Interrupt Status
 *                    0: No interrupt happens.
 *                    1: interrupt happens.
 *            Bit [0] Read Function..Interrupt Status
 *                    0: No interrupt happens.
 *                    1: interrupt happens.
 * @endverbatim
 *                         
 * @param b: true, PIP enable, false, PIP disable
 *
 * @note PIP 1 window always on top of PIP 2 window
 */
uint8_t Panel_RA8889::Interrupt_Status(void)
{
  SPI_CmdWrite(REG_INTF);                      //0x0c, Interrupt Event Flag Register (INTF)
  return LCD_DataRead();
}


/**
 * @brief Aguarde até o VSync termine de fazer o sincronismo vertical
 *        
 * @verbatim
 * REG [0x0c] Interrupt Event Flag Register (INTF)
 *            Bit [4] Read Function .. Vsync Interrupt Status
 *                    0: No interrupt happens.
 *                    1: interrupt happens.
 * @endverbatim
 *                         
 * @param None
 *
 * @note É enviado um comando de verificaçãod a interrupção, pois pode estar 
 *       no meio do caminho ainda fazendo o VSync, antes de atualizar a tela ou
 *       escrever novos dados. 
 */
void Panel_RA8889::VSYNC_WaitReady(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_INTF);                      //0x0c, Interrupt Event Flag Register (INTF)
  SPI_DataWrite(cSetb4);                       //Set bit 4, solicita que verifique se o VSync está pronto
  do {
      temp = SPI_DataRead();                   //Leia o status
  } while ( (temp & 0x10) == 0x00);            //Aguarde ate que seja resetado o bit, terminou o retraço
}


/**
 * @brief Clear Resume Interrupt Flag
 *        
 *        REG [0x0c] Interrupt Event Flag Register (INTF)
 *                   Bit [7] Wakeup/resume Interrupt flag
 *                           Write Function ➔ Wakeup/resume Interrupt Clear Bit
 *                           0: No operation.
 *                           1: Clear Wakeup/resume interrupt.
 *                           
 *                           Read Function ➔ Wakeup/resume Interrupt Status
 *                           0: No Wakeup/resume interrupt happens.
 *                           1: Wakeup/resume interrupt happens.
 *                         
 * @param None
 *
 * @note None
 * 
 */
void Panel_RA8889::Interrupt_ClearResume_Flag(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_INTF);                      //0x0c, Interrupt Event Flag Register (INTF)
  temp = SPI_DataRead();
  temp |= cSetb7;
  SPI_DataWrite(temp);
}


/**
 * @brief Clear External Interrupt Input (PS[0] pin) Flag
 *        
 *        REG [0x0c] Interrupt Event Flag Register (INTF)
 *                   Bit [6] External Interrupt (PS[0] pin) flag
 *                           Write Function ➔ XPS[0] pin edge Interrupt Clear Bit
 *                           0: No operation.
 *                           1: Clear the XPS[0] pin edge interrupt.
 *                           
 *                           Read Function ➔ XPS[0] pin Interrupt Status
 *                           0: No XPS[0] pin interrupt happens.
 *                           1: XPS[0] pin interrupt happens.
 *                           
 * @param None
 *
 * @note None
 * 
 */
void Panel_RA8889::ExtInterrupt_ClearInput_Flag(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_INTF);                      //0x0c, Interrupt Event Flag Register (INTF)
  temp = SPI_DataRead();
  temp |= cSetb6;                              //Resetar interrupção
  SPI_DataWrite(temp);
}


/**
 * @brief Clear I2C Master Interrupt Flag
 *        
 *        REG [0x0c] Interrupt Event Flag Register (INTF)
 *                   Bit [5] IIC master Interrupt flag
 *                           Write Function➔ IIC master Interrupt Clear Bit
 *                           0: No operation.
 *                           1: Clear the IIC master interrupt.
 *
 *                           Read Function ➔ IIC master Interrupt Status
 *                           0: No IIC master interrupt happens.
 *                           1: IIC master interrupt happens.
 *                           
 * @param None
 *
 * @note None
 * 
 */
void Panel_RA8889::Interrupt_ClearI2CM_Flag(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_INTF);                      //0x0c, Interrupt Event Flag Register (INTF)
  temp = SPI_DataRead();
  temp |= cSetb5;                              //Resetar interrupção
  SPI_DataWrite(temp);                         
}


/**
 * @brief Clear Interrupt Vertical Synchronization Flag
 *        
 *        REG [0x0c] Interrupt Event Flag Register (INTF)
 *                   Bit [4] Vsync Time base interrupt flag
 *                           Write Function ➔Vsync Interrupt Clear Bit
 *                           0: No operation.
 *                           1: Clear the Vsync interrupt.
 *                           Read Function ➔ Vsync Interrupt Status
 *                           0: No Vsync interrupt happens.
 *                           1: Vsync interrupt happens.
 *                         
 * @param None
 *
 * @note None
 * 
 */
void Panel_RA8889::Interrupt_ClearVSync_Flag(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_INTF);                      //0x0c, Interrupt Event Flag Register (INTF)
  temp = SPI_DataRead();
  temp |= cSetb4;                              //Resetar interrupção
  SPI_DataWrite(temp);                         //Limpa status de interrupção VSync
}


/**
 * @brief Clear Interrupt Key Scan Flag
 *        
 *        REG [0x0c] Interrupt Event Flag Register (INTF)
 *                   Bit [3] Key Scan Interrupt flag
 *                           Write Function ➔ Key Scan Interrupt Clear Bit
 *                           0: No operation.
 *                           1: Clear the Key Scan interrupt.
 *
 *                           Read Function ➔ Key Scan Interrupt Status
 *                           0: No Key Scan interrupt happens.
 *                           1: Key Scan interrupt happens.
 *                         
 * @param None
 *
 * @note None
 * 
 */
void Panel_RA8889::Interrupt_ClearKeyScan_Flag(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_INTF);                      //0x0c, Interrupt Event Flag Register (INTF)
  temp = SPI_DataRead();
  temp |= cSetb3;                              //Resetar interrupção
  SPI_DataWrite(temp);
}


/**
 * @brief Check for Interrupt Key Scan Occurred
 *        
 *        REG [0x0c] Interrupt Event Flag Register (INTF)
 *                   Bit [3] Key Scan Interrupt flag
 *                           Write Function ➔ Key Scan Interrupt Clear Bit
 *                           0: No operation.
 *                           1: Clear the Key Scan interrupt.
 *
 *                           Read Function ➔ Key Scan Interrupt Status
 *                           0: No Key Scan interrupt happens.
 *                           1: Key Scan interrupt happens.
 *                         
 * @param None
 *
 * @note Para limpar a interrupção de teclas (Key Scan) após tratamento, use a funcao
 *       Interrupt_ClearKeyScan_Flag()
 */
bool Panel_RA8889::Interrupt_IsKeyPressed(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_INTF);                      //0x0c, Interrupt Event Flag Register (INTF)
  temp = SPI_DataRead();
  return (temp & 0x08);                        //Recebeu a interrupção
}


/**
 * @brief  Clear Interrupt Serial Flash DMA, Draw Task, BTE Process Complete, etc. Flag
 *        
 *        REG [0x0c] Interrupt Event Flag Register (INTF)
 *                   Bit [2] Serial flash DMA Complete | Draw task finished | 
 *                           BTE Process Complete | etc. Interrupt flag
 *                           Write Function➔ Interrupt Clear Bit
 *                           0: No operation.
 *                           1: Clear interrupt.
 *                           Read Function➔Interrupt Status
 *                           0: No interrupt happens.
 *                           1: interrupt happens.
 *                         
 * @param None
 *
 * @note De acordo com a requisição de evento de Serial Flash DMA, Draw Task 
 *       ou processos completos de BTE ou outros este flag é setado ou pode 
 *       ser limpo para qualquer um destes casos.
 */
void Panel_RA8889::Interrupt_ClearMultiEventTask_Flag(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_INTF);                      //0x0c, Interrupt Event Flag Register (INTF)
  temp = SPI_DataRead();
  temp |= cSetb2;
  SPI_DataWrite(temp);
}


/**
 * @brief Clear Interrupt PWM 0 Timer Flag
 *        
 *        REG [0x0c] Interrupt Event Flag Register (INTF)
 *                   Bit [0] PWM 0 timer Interrupt flag
 *                           Write Function ➔ Interrupt Clear Bit
 *                           0: No operation.
 *                           1: Clear PWM0 interrupt.
 *
 *                           Read Function ➔ Interrupt Status
 *                           0: No PWM0 interrupt happens.
 *                           1: PWM0 interrupt happens.
 *                         
 * @param None
 *
 * @note None
 */
void Panel_RA8889::Interrupt_ClearPWM0_Flag(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_INTF);                      //0x0c, Interrupt Event Flag Register (INTF)
  temp = SPI_DataRead();
  temp |= cSetb0;
  SPI_DataWrite(temp);
}


/**
 * @brief Clear Interrupt PWM 1 Timer Flag
 *        
 *        REG [0x0c] Interrupt Event Flag Register (INTF)
 *                   Bit [1] PWM 1 timer Interrupt flag
 *                           Write Function ➔ Interrupt Clear Bit
 *                           0: No operation.
 *                           1: Clear PWM1 interrupt.
 *
 *                           Read Function ➔ Interrupt Status
 *                           0: No PWM1 interrupt happens.
 *                           1: PWM1 interrupt happens.
 *                         
 * @param None
 *
 * @note None
 */
void Panel_RA8889::Interrupt_ClearPWM1_Flag(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_INTF);                      //0x0c, Interrupt Event Flag Register (INTF)
  temp = SPI_DataRead();
  temp |= cSetb1;
  SPI_DataWrite(temp);
}


//================================================================================
//
// [0x10] Main/PIP Window Control Register (MPWCTR)
//
//================================================================================


/**
 * @brief Habilita o Picture-in-Picure (PIP 1) do display
 *        PIP: Para entender, é uma pequena área de iamgem (janelinha) sobre 
 *        outra área de imagem semelhante a uma janelinha da TV no canto 
 *        esquerdo, enquanto vai zapeando os canais nas janelas principal.
 *
 *        REG[10h] Main/PIP Window Control Register (MPWCTR)
 *        bit [7] PIP 1 window Enable/Disable
 *                0b0: PIP 1 window disable.
 *                0b1: PIP 1 window enable
 *
 * @param b: true, PIP enable, false, PIP disable
 *
 * @note PIP 1 window always on top of PIP 2 window
 */
void Panel_RA8889::Enable_PIP1(bool b)
{
  uint8_t temp;
  SPI_CmdWrite(REG_MPWCTR);                    //0x10, Main/PIP Window Control Register
  temp = SPI_DataRead();
  if (b) temp |= cSetb7 else temp &= cClrb7;
  SPI_DataWrite(temp);
}


/**
 * @brief Habilita o Picture-in-Picure (PIP 2) do display
 *        PIP: Para entender, é uma pequena área de iamgem (janelinha) sobre 
 *        outra área de imagem semelhante a uma janelinha da TV no canto 
 *        esquerdo, enquanto vai zapeando os canais nas janelas principal.
 *
 *        REG[10h] Main/PIP Window Control Register (MPWCTR)
 *        bit [6] PIP 2 window Enable/Disable
 *                0b0: PIP 2 window disable.
 *                0b1: PIP 2 window enable
 *
 * @param b: true, PIP enable, false, PIP disable
 *
 * @note PIP 1 window always on top of PIP 2 window
 */
void Panel_RA8889::Enable_PIP2(bool b)
{
  uint8_t temp;
  SPI_CmdWrite(REG_MPWCTR);                    //0x10, Main/PIP Window Control Register
  temp = SPI_DataRead();
  if (b) emp |= cSetb6 else temp &= cClrb6;
  LCD_DataWrite(temp);
}


/**
 * @brief Select Configure PIP 1 or 2 Window’s parameters
 *
 *        REG[10h] Main/PIP Window Control Register (MPWCTR)
 *        bit [4] Select Configure PIP 1 or 2 Window’s parameters
 *                PIP window’s parameter including Color Depth, starting address,
 *                image width, display coordinates, window coordinates, window
 *                width, and window height.
 *                0b0: To configure PIP 1’s parameters.
 *                0b1: To configure PIP 2’s parameters.
 *
 * @param PIPSelect::PIP1 : To configure PIP 1’s parameters
 *        PIPSelect::PIP2 : To configure PIP 2’s parameters.
 *
 * @note None
 */
void Panel_RA8889::Select_PIP_Parameter(PIPSelect pip)
{
  uint8_t temp;
  SPI_CmdWrite(REG_MPWCTR);                    //0x10, Main/PIP Window Control Register
  temp = SPI_DataRead();
  temp &= cClrb4;                              //Reset bit 4
  temp |= static_cast<uint8_t>(pip);           //Converte enum para uint8_t
  SPI_DataWrite(temp);
}


/**
 * @brief Seleciona Janela de imagem principal para 8bpp
 *
 *        REG[10h] Main/PIP Window Control Register (MPWCTR)
 *        bit [3-2] Main Window Image Color Depth Setting
 *                  0b00: 8-bpp generic TFT, i.e. 256 colors.
 *                  0b01: 16-bpp generic TFT, i.e. 65K colors.
 *                  0b1x: 24-bpp generic TFT, i.e. 1.67M colors.
 *
 * @param None
 *
 */
void Panel_RA8889::Select_MainWindow_8bpp(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_MPWCTR);                    //0x10, Main/PIP Window Control Register
  temp = SPI_DataRead();
  temp &= cClrb3;                              //Reset bit 3
  temp &= cClrb2;                              //Reset bit 2
  SPI_DataWrite(temp);                         //Set main windows image to 8bpp
}


/**
 * @brief Seleciona Janela de imagem principal para 16bpp
 *
 *        REG[10h] Main/PIP Window Control Register (MPWCTR)
 *        bit [3-2] Main Image Color Depth Setting
 *                  0b00: 8-bpp generic TFT, i.e. 256 colors.
 *                  0b01: 16-bpp generic TFT, i.e. 65K colors.
 *                  0b1x: 24-bpp generic TFT, i.e. 1.67M colors.
 *
 * @param None
 *
 */
void Panel_RA8889::Select_MainWindow_16bpp(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_MPWCTR);                    //0x10, Main/PIP Window Control Register
  temp = SPI_DataRead();
  temp &= cClrb3;                              //Reset bit 3
  temp |= cSetb2;                              //Set bit 2
  SPI_DataWrite(temp);                         //Set main windows image to 16bpp
}


/**
 * @brief Seleciona Janela de imagem principal para 24bpp
 *
 *        REG[10h] Main/PIP Window Control Register (MPWCTR)
 *        bit [3-2] Main Window Image Color Depth Setting
 *                  0b00: 8-bpp generic TFT, i.e. 256 colors.
 *                  0b01: 16-bpp generic TFT, i.e. 65K colors.
 *                  0b1x: 24-bpp generic TFT, i.e. 1.67M colors.
 *
 * @param None
 *
 */
void Panel_RA8889::Select_MainWindow_24bpp(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_MPWCTR);                    //0x10, Main/PIP Window Control Register
  temp = SPI_DataRead();
  temp |= cSetb3;                              //Set bit 3
  SPI_DataWrite(temp);                         //Set main windows image to 16bpp
}


/**
 * @brief Controlar o sinal de modo de sincronização do painel LCD habilitando
 *        o modo de sincronização XVSYNC, XHSYNC, XDE
 *
 *        REG[10h] Main/PIP Window Control Register (MPWCTR)
 *        bit [0] To Control panel’s synchronous signals
 *                0b0: Sync Mode: Enable XVSYNC, XHSYNC, XDE
 *                0b1: DE Mode: Only XDE enable, XVSYNC & XHSYNC in idle state
 *
 * @param None
 *
 */
void Panel_RA8889::Select_LCD_SyncMode(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_MPWCTR);                    //0x10, Main/PIP Window Control Register
  temp = SPI_DataRead();
  temp &= cClrb0;                              //Reset bit 0
  SPI_DataWrite(temp);                         //Enable XVSYNC, XHSYNC, XDE
}


/**
 * @brief Controlar o sinal de modo de sincronização do painel LCD habilitando
 *        apenas o modo de sincronização XDE
 *
 *        REG[10h] Main/PIP Window Control Register (MPWCTR)
 *        bit [0] To Control panel’s synchronous signals
 *                0b0: Sync Mode: Enable XVSYNC, XHSYNC, XDE
 *                0b1: DE Mode: Only XDE enable, XVSYNC & XHSYNC in idle state
 *
 * @param None
 *
 */
void Panel_RA8889::Select_LCD_DEMode(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_MPWCTR);                    //0x10, Main/PIP Window Control Register
  temp = SPI_DataRead();
  temp |= cSetb0;                              //Set bit 0
  SPI_DataWrite(temp);                         //Only XDE enable, XVSYNC & XHSYNC in idle state
}


//================================================================================
//
// [0x11] PIP Window Color Depth Setting (PIPCDEP)
//
//================================================================================


/**
 * @brief PIP 1 Window Color Depth Setting
 *
 * @verbatim
 * REG[0x11] PIP Window Color Depth Setting (PIPCDEP)
 * bit [3-2] PIP 1 Window Color Depth Setting
 *           0b00: 8-bpp generic TFT, i.e. 256 colors
 *           0b01: 16-bpp generic TFT, i.e. 65K colors (Default)
 *           0b1x: 24-bpp generic TFT, i.e. 1.67M colors
 * @endverbatim
 *
 * @param None
 *
 */
void PIP1_Window_ColorDepth(eColorDepthBPP bpp)
{
  uint8_t temp;
  uint8_t bit;
  SPI_CmdWrite(REG_PIPCDEP);                   //0x11, PIP Window Color Depth Setting (PIPCDEP)
  temp = SPI_DataRead();
  temp &= ~(cSetb3 | cSetb2);                  //Reset bit 3 e 2
  bit = static_cast<uint8_t>(bpp);             //
  bit = (bit >> 3)-1;                          //transforma 8,16,24 em 0, 1, 2
  bit = bit << 2;                              //posiciona no bit 3 e 2
  temp |= bit;                                 //cobina os bits
  SPI_DataWrite(temp);                         
}



/**
 * @brief PIP 2 Window Color Depth Setting
 *
 * @verbatim
 * REG[0x11] PIP Window Color Depth Setting (PIPCDEP)
 * bit [1-0] PIP 1 Window Color Depth Setting
 *           0b00: 8-bpp generic TFT, i.e. 256 colors
 *           0b01: 16-bpp generic TFT, i.e. 65K colors (Default)
 *           0b1x: 24-bpp generic TFT, i.e. 1.67M colors
 * @endverbatim
 *
 * @param None
 *
 */
void PIP2_Window_ColorDepth(eColorDepthBPP bpp)
{
  uint8_t temp;
  uint8_t bit;
  SPI_CmdWrite(REG_PIPCDEP);                   //0x11, PIP Window Color Depth Setting (PIPCDEP)
  temp = SPI_DataRead();
  temp &= ~(cSetb3 | cSetb2);                  //Reset bit 3 e 2
  bit = static_cast<uint8_t>(bpp);             //
  bit = (bit >> 3)-1;                          //transforma 8,16,24 em 0, 1, 2
  temp |= bit;                                 //cobina os bits
  SPI_DataWrite(temp);                         
}


//================================================================================
//
// [0x12] Display Configuration Register (DPCR)
//
//================================================================================


/**
 * @brief Panel Scan Clock PCLK Rising
 *
 * @verbatim
 * PCLK Inversion:
 * bit [7] 0b0 : 0: PDAT, DE, HSYNC etc. Panel (Drive/change) fetches PDAT at PCLK rising edge.
 *         0b1 : 1: PDAT, DE, HSYNC etc. Panel (Drive/change) fetches PDAT at PCLK falling edge.
 * @endverbatim
 *
 * @param None
 * 
 * @note None
 */
void Panel_RA8889::PCLK_Rising(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_DPCR);                      //0x12, Display Configuration Register (DPCR)
  temp = SPI_DataRead();                       //Lê valor atual do registrador
  temp &= cClrb7;                              //Reset bit 7 
  SPI_DataWrite(temp);
}


/**
 * @brief Panel Scan Clock PCLK Failing
 *
 *        PCLK Inversion:
 *        bit [7] 0b0 : 0: PDAT, DE, HSYNC etc. Panel (Drive/change) fetches PDAT at PCLK rising edge.
 *                0b1 : 1: PDAT, DE, HSYNC etc. Panel (Drive/change) fetches PDAT at PCLK falling edge.
 *
 * @param None
 * 
 * @note None
 */
void Panel_RA8889::PCLK_Falling(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_DPCR);                      //0x12, Display Configuration Register (DPCR)
  temp = SPI_DataRead();                       //Lê valor atual do registrador
  temp |= cSetb7;                              //Set bit 7
  SPI_DataWrite(temp);
}


/**
 * @brief Panel Scan Clock PCLK Edge Type
 *
 *        PCLK Inversion:
 *        bit [7] 0b0 : 0: PDAT, DE, HSYNC etc. Panel (Drive/change) fetches PDAT at PCLK rising edge.
 *                0b1 : 1: PDAT, DE, HSYNC etc. Panel (Drive/change) fetches PDAT at PCLK falling edge.
 *
 * @param None
 * 
 * @note None
 */
void Panel_RA8889::PCLK_EdgeType(PCLKEdge edge)
{
  uint8_t temp;
  SPI_CmdWrite(REG_DPCR);                      //0x12, Display Configuration Register (DPCR)
  temp = SPI_DataRead();                       //Lê valor atual do registrador
  temp &= cClrb7;                              //Reset bit 7
  temp |= static_cast<uint8_t>(edge);          
  SPI_DataWrite(temp);
}


/**
 * @brief Liga o display
 *
 *        REG[12h] Display Configuration Register (DPCR)
 *        bit [6] Display ON/OFF
 *                0b0: Display Off.
 *                0b1: isplay On.
 *
 * @param on: true, liga display, false: desliga display 
 *
 * @note None
 *       
 */
void Panel_RA8889::DisplayOn(bool on)
{
  uint8_t temp;
  SPI_CmdWrite(REG_DPCR);                      //0x12, Display Configuration Register (DPCR)
  temp = SPI_DataRead();                       //Lê valor atual do registrador
  temp &= cClrb6;                              //Rest bit 
  if (on) temp |= cSetb6;
  SPI_DataWrite(temp);
}


/**
 * @brief Horizontal Scan Direction Left to Right
 *
 *        bit [4] 0b0: Horizontal Scan Left to Right
 *                0b1: Horizontal Scan Right to Left
 *
 * @param Nenhum
 *
 * @note O HSCAN deve acompanhar a configuração do registrador [0x02] bit [2-1]
 *       PIP window will be disabled when HDIR set as 1.
 *       
 */
void Panel_RA8889::HScanDirection_LeftToRight (void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_DPCR);                      //0x12, Display Configuration Register (DPCR)
  temp = SPI_DataRead();                       //Lê valor atual do registrador
  temp &= cClrb4;                              //reset bit 4
  SPI_DataWrite(temp);                         //Write HDIR
}


/**
 * @brief Horizontal Scan Direction Left to Right
 *
 *        bit [4] 0b0: Horizontal Scan Left to Right
 *                0b1: Horizontal Scan Right to Left
 *
 * @param Nenhum
 *
 * @note O HSCAN deve acompanhar a configuração do registrador [0x02] bit [2-1]
 *       PIP window will be disabled when HDIR set as 1.
 *       
 */
void Panel_RA8889::HScanDirection_RightToLeft (void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_DPCR);                      //0x12, Display Configuration Register (DPCR)
  temp = SPI_DataRead();                       //Lê valor atual do registrador
  temp |= cSetb4;                              //Set bit 4
  SPI_DataWrite(temp);                         //Write HDIR
}


/**
 * @brief Horizontal Scan Direction Left to Right / Right to Left
 *
 *        bit [4] 0b0: Horizontal Scan Left to Right
 *                0b1: Horizontal Scan Right to Left
 *
 * @param Nenhum
 *
 * @note O HSCAN deve acompanhar a configuração do registrador [0x02] bit [2-1]
 *       PIP window will be disabled when HDIR set as 1.
 *       
 */
void Panel_RA8889::HorizontalScanDirection (HSCANDir direction)
{
  uint8_t temp;
  SPI_CmdWrite(REG_DPCR);                      //0x12, Display Configuration Register (DPCR)
  temp = SPI_DataRead();                       //Lê valor atual do registrador
  temp &= cClrb4;                              //reset bit 4
  temp |= static_cast<uint8_t>(direction);     //Define o destino
  SPI_DataWrite(temp);                         //Write HDIR
}


/**
 * @brief Vertical Scan Direction Top to Bottom
 *
 *        bit [3] 0b0: Vertical Scan from top to bottom
 *                0b1: Vertical Scan from bottom to top
 * 
 * @note O VSCAN deve acompanhar a configuração do registrador [0x02] bit [2-1]
 *       PIP window will be disabled when VDIR set as 1.
 *       
 */
void Panel_RA8889::VScanDirection_TopToBottom(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_DPCR);                      //0x12, Display Configuration Register (DPCR)
  temp = SPI_DataRead();                       //Lê valor atual do registrador
  temp &= cClrb3;                              //Reset bit 3
  SPI_DataWrite(temp);                         //Write VDIR
}


/**
 * @brief Vertical Scan Direction Bottom to Top
 *
 *        bit [3] 0b0: Vertical Scan from top to bottom
 *                0b1: Vertical Scan from bottom to top
 * 
 * @note O VSCAN deve acompanhar a configuração do registrador [0x02] bit [2-1]
 *       PIP window will be disabled when VDIR set as 1.
 *       
 */
void Panel_RA8889::VScanDirection_BottomToTop(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_DPCR);                      //0x12, Display Configuration Register (DPCR)
  temp = SPI_DataRead();                       //Lê valor atual do registrador
  temp |= cSetb3;                              //Set bit 3
  SPI_DataWrite(temp);                         //Write VDIR
}


/**
 * @brief Vertical Scan Direction Top to Bottom / Bottom to Top
 *
 *        bit [3] 0b0: Vertical Scan from top to bottom
 *                0b1: Vertical Scan from bottom to top
 *
 * @param dir: VSCANDir::TopToBottom
 *             VSCANDir::BottomToTop
 * 
 * @note O VSCAN deve acompanhar a configuração do registrador [0x02] bit [2-1]
 *       PIP window will be disabled when VDIR set as 1.
 */
void Panel_RA8889::VerticalScanDirection(VSCANDir direction)
{
  uint8_t temp;
  SPI_CmdWrite(REG_DPCR);                      //0x12, Display Configuration Register (DPCR)
  temp = SPI_DataRead();                       //Lê valor atual do registrador
  temp &= cClrb3;                              //Reset bit 3
  temp |= static_cast<uint8_t>(direction);     //Define o destino
  SPI_DataWrite(temp);                         //Write VDIR
}


/**
 * @brief Set the type of parallel data output sequence in RGB format
 *
 *        Parallel XPDAT[23:0] Output Sequence:
 *        bit [2-0] 0b000 : RGB
 *                  0b001 : RBG
 *                  0b010 : GRB
 *                  0b011 : GBR
 *                  0b100 : BRG
 *                  0b101 : BGR
 *                  0b110 : Gray
 *                  0b111 : Send out idle state (all 0 or 1, black or white color).

 * @param fmt: Formato de cor (PDATAColorFmt)
 * 
 * @note None
 */
void Panel_RA8889::PDATA_ColorFmt(PDATAColorFmt fmt)
{
  uint8_t temp;
  SPI_CmdWrite(REG_DPCR);                      //0x12, Display Configuration Register (DPCR)
  temp = SPI_DataRead();                       //Lê valor atual do registrador
  temp &= cClrb0                               //Reset bit 0 
  temp &= cClrb1                               //Reset bit 1
  temp &= cClrb2                               //Reset bit 2
  _colorfmt = static_cast<uint8_t>(fmt);       //Guarda o formato da cor
  temp |= _colorfmt;                           //Define o destino  
  ER_TFT.LCD_DataWrite(temp);
}


//================================================================================
//
// [0x13] Panel scan Clock and Data Setting Register (PCSR)
//
//================================================================================


/**
 * @brief HSYNC Polarity Active Low
 *
 *        Polarity:
 *        bit [7] 0b0 : Low active
 *                0b1 : High active
 *
 * @param None
 * 
 * @note None
 */
void Panel_RA8889::HSYNC_PolarityLow(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_PCSR);                      //0x13, Panel scan Clock and Data Setting Register (PCSR)
  temp = SPI_DataRead();                       //Lê valor atual do registrador
  temp &= cClrb7;                              //Reset bit 7
  SPI_DataWrite(temp);
}


/**
 * @brief HSYNC Polarity Active High
 *
 *        Polarity:
 *        bit [7] 0b0 : Low active
 *                0b1 : High active
 *
 * @param None
 * 
 * @note None
 */
void Panel_RA8889::HSYNC_PolarityHigh(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_PCSR);                      //0x13, Panel scan Clock and Data Setting Register (PCSR)
  temp = SPI_DataRead();                       //Lê valor atual do registrador
  temp |= cSetb7;                              //Set bit 7
  SPI_DataWrite(temp);
}


/**
 * @brief HSYNC Polarity
 *
 *        Polarity:
 *        bit [7] 0b0 : Low active
 *                0b1 : High active
 *
 * @param HSYNCPolarity::Low
 *        HSYNCPolarity::High
 *
 * @note None
 */
void Panel_RA8889::HSYNC_Polarity(HSYNCPolarity val)
{
  uint8_t temp;
  SPI_CmdWrite(REG_PCSR);                      //0x13, Panel scan Clock and Data Setting Register (PCSR)
  temp = SPI_DataRead();                       //Lê valor atual do registrador
  temp &= cClrb7;                              //Reset bit 7
  temp |= static_cast<uint8_t>(val);
  SPI_DataWrite(temp);
}


/**
 * @brief VSYNC Polarity Active Low
 *
 *        Polarity:
 *        bit [6] 0b0 : Low active
 *                0b1 : High active
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::VSYNC_PolarityLow(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_PCSR);                      //0x13, Panel scan Clock and Data Setting Register (PCSR)
  temp = SPI_DataRead();                       //Lê valor atual do registrador
  temp &= cClrb6;                              //Reset bit 6
  SPI_DataWrite(temp);
}


/**
 * @brief VSYNC Polarity Active High
 *
 *        Polarity:
 *        bit [6] 0b0 : Low active
 *                0b1 : High active
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::VSYNC_PolarityHigh(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_PCSR);                      //0x13, Panel scan Clock and Data Setting Register (PCSR)
  temp = SPI_DataRead();                       //Lê valor atual do registrador
  temp = ER_TFT.LCD_DataRead();
  temp |= cSetb6;                              //Set bit 6
  SPI_DataWrite(temp);
}


/**
 * @brief VSYNC Polarity
 *
 *        Polarity:
 *        bit [6] 0b0 : Low active
 *                0b1 : High active
 *
 * @param VSYNCPolarity::Low
 *        VSYNCPolarity::High
 *
 * @note None
 */
void Panel_RA8889::VSYNC_Polarity(VSYNCPolarity val)
{
  uint8_t temp;
  SPI_CmdWrite(REG_PCSR);                      //0x13, Panel scan Clock and Data Setting Register (PCSR)
  temp = SPI_DataRead();                       //Lê valor atual do registrador
  temp &= cClrb6;                              //Reset bit 6
  temp |= static_cast<uint8_t>(val);
  SPI_DataWrite(temp);
}


/**
 * @brief Data Enable Polarity Low Active
 *        Generic TFT interface signal for data valid or data enable.
 *
 *        bit [5] 0b0 : High active
 *                0b1 : Low active
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::DE_PolarityLow(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_PCSR);                      //0x13, Panel scan Clock and Data Setting Register (PCSR)
  temp = SPI_DataRead();                       //Lê valor atual do registrador
  temp |= cSetb5;                              //Set bit 5
  SPI_DataWrite(temp);
}


/**
 * @brief Data Enable Polarity High
 *        Generic TFT interface signal for data valid or data enable.
 *
 *        bit [5] 0b0 : High active
 *                0b1 : Low active
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::DE_PolarityHigh(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_PCSR);                      //0x13, Panel scan Clock and Data Setting Register (PCSR)
  temp = SPI_DataRead();                       //Lê valor atual do registrador
  temp &= cClrb5;                              //Reset bit 5
  SPI_DataWrite(temp);
}


/**
 * @brief Data Enable Polarity
 *        Generic TFT interface signal for data valid or data enable.
 *
 *        bit [5] 0b0 : High active
 *                0b1 : Low active
 *
 * @param DataEnable::High
 *        DataEnable::Low
 *
 * @note None
 */
void Panel_RA8889::DE_Polarity(DEPolarity val)
{
  uint8_t temp;
  SPI_CmdWrite(REG_PCSR);                      //0x13, Panel scan Clock and Data Setting Register (PCSR)
  temp = SPI_DataRead();                       //Lê valor atual do registrador
  temp &= cClrb5;                              //Reset bit 5
  temp |= static_cast<uint8_t>(val);
  SPI_DataWrite(temp);
}


//================================================================================
//
// [0x14] Horizontal Display Width Register (HDWR)
// [0x15] Horizontal Display Width Fine Tune Register (HDWFTR)
// [0x1a] Vertical Display Height Register 0(VDHR0)
// [0x1b] Vertical Display Height Register 1 (VDHR1)
//
//================================================================================


/**
 * @brief Define a resolução vertical e horizontal do display
 *        
 *        [14h] Horizontal Display Width Setting (HDWR) Bit[7:0]
 *        [15h] Horizontal Display Width Fine Tuning (HDWFTR) [3:0]
 *        The register specifies the LCD panel horizontal display width in
 *        the unit of 8 pixels resolution.
 *        Horizontal display width(pixels) = (HDWR + 1) * 8 + HDWFTR
 *
 *	      [1Ah] Vertical Display Height Bit[7:0] (low register) 
 *        Vertical Display Height(Line) = VDHR + 1
 *   
 *        [1Bh] Vertical Display Height Bit[10:8] (high register)
 *        Vertical Display Height(Line) = VDHR + 1
 *
 * @param wx Width horizontal
 *        hy Height vertical
 *        
 *
 * @note None
 */
void Panel_RA8889::HorizontalWidth_VerticalHeight(uint16_t wx, uint16_t hy)
{
  uint8_t temp;
  
  if (wx < 8) {
    SPI_CmdWrite(REG_HDWR);                    //0x14, Horizontal Display Width Register (HDWR)
    SPI_DataWrite(0x00);                       //
    
    SPI_CmdWrite(REG_HDWFTR);                  //0x15, Horizontal Display Width Fine Tune Register (HDWFTR)
    SPI_DataWrite(wx);                         //
    
    temp = hy - 1;
    SPI_CmdWrite(REG_VDHR0);                   //0x1a, Vertical Display Height Register 0(VDHR0)
    SPI_DataWrite(temp);                       //Store bit [7-0]
    
    temp = (hy - 1) >> 8;                      //
    SPI_CmdWrite(REG_VDHR1);                   //0x1b, Vertical Display Height Register 1 (VDHR1)
    SPI_DataWrite(temp);                       //Store bit [2-0]
  } else {
    temp = (wx / 8) - 1;                       //exemplo: temp = (800/8)-1 = 99
    SPI_CmdWrite(REG_HDWR);                    //0x14, Horizontal Display Width Register (HDWR)
    SPI_DataWrite(temp);                       //
    
    temp = wx % 8;                             //temp = 800 % 8 = 0
    SPI_CmdWrite(REG_HDWFTR);                  //0x15, Horizontal Display Width Fine Tune Register (HDWFTR)
    SPI_DataWrite(temp);                       //
    
    temp = hy - 1;                             //temp = 480 - 1 = 479 = 0x01df -> low(0xdf) 
    SPI_CmdWrite(REG_VDHR0);                   //0x1a, Vertical Display Height Register 0(VDHR0)
    SPI_DataWrite(temp);                       //Store bit [7-0]
    
    temp = (hy - 1) >> 8;                      //temp = 480 - 1 = 479 = 0x01df -> high(0x01)
    SPI_CmdWrite(REG_VDHR1);                   //0x1b, Vertical Display Height Register 1 (VDHR1)
    SPI_DataWrite(temp);                       //Store bit [2-0]
  }
}


//================================================================================
//
// [0x16] Horizontal Non-Display Period Register (HNDR)
// [0x17] Horizontal Non-Display Period Fine Tune Register (HNDFTR)
//
//================================================================================


/**
 * @brief Define o Período Horizontal de Não Exibição do LCD
 *        
 *        [16h] Horizontal Non-Display Period(HNDR) Bit[4:0]
 *        This register specifies the horizontal non-display period. Also called "back porch".
 *        Horizontal non-display period(pixels) = (HNDR + 1) * 8 + HNDFTR
 *
 *        [17h] Horizontal Non-Display Period Fine Tuning(HNDFT) [3:0]
 *        This register specifies the fine tuning for horizontal non-display
 *        period; it is used to support the SYNC mode panel. Each level of
 *        this modulation is 1-pixel.
 *        Horizontal non-display period(pixels) = (HNDR + 1) * 8 + HNDFTR
 *
 * @param hbpd HS Back Porch (Blanking)
 *
 * @note None
 */
void Panel_RA8889::Horizontal_NonDisplay(uint16_t hbpd)
{
  uitn8_t temp;
  if (hbpd < 8) {
    SPI_CmdWrite(REG_HNDR);                    //0x16, Horizontal Non-Display Period Register (HNDR)
    SPI_DataWrite(0x00);                       //
    
    SPI_CmdWrite(REG_HNDFTR);                  //0x17, Horizontal Non-Display Period Fine Tune Register (HNDFTR)
    SPI_DataWrite(hbpd);
  } else {
    temp = (hbpd / 8) - 1;                     // 
    SPI_CmdWrite(REG_HNDR);                    //0x16, Horizontal Non-Display Period Register (HNDR)
    SPI_DataWrite(temp);
    
    temp = hbpd % 8;                           //
    SPI_CmdWrite(REG_HNDFTR);                  //0x17, Horizontal Non-Display Period Fine Tune Register (HNDFTR)
    SPI_DataWrite(temp);
  }
}


//================================================================================
//
// [0x18] HSYNC Start Position Register (HSTR)
// [0x19] HSYNC Pulse Width Register (HPWR)
//
//================================================================================


/**
 * @brief 
 *        
 *        [18h] HSYNC Start Position Register (HSTR)
 *        bit [4:0] HSYNC Start Position
 *                  The starting position from the end of display area to the
 *                  beginning of HSYNC. Each level of this modulation is 8-pixel.
 *                  Also called front porch.
 *                  HSYNC Start Position(pixels) = (HSTR + 1)x8
 *
 * @param hfpd: HS Front Porch
 *
 * @note None
 */
void Panel_RA8889::HSYNC_StartPosition(uint16_t hfpd)
{
  uint8_t temp;
  if(hfpd<8) {
    SPI_CmdWrite(REG_HSTR);                    //0x18, HSYNC Start Position Register (HSTR)
    SPI_DataWrite(0x00);                       //
  } else {
    temp = (hfpd / 8) - 1;                     //
    SPI_CmdWrite(REG_HSTR);                    //0x18, HSYNC Start Position Register (HSTR)
    SPI_DataWrite(temp);                       //
  }
}
	

/**
 * @brief 
 *        
 *        [19h] HSYNC Pulse Width Register (HPWR) 
 *        bit [4:0] HSYNC Pulse Width(HPW)
 *                  The period width of HSYNC.
 *                  HSYNC Pulse Width(pixels) = (HPW + 1) x 8
 *
 * @param hspw: HS Pulse Width
 *
 * @note None
 */
void Panel_RA8889::HSYNC_PulseWidth(uint16_t hspw)
{
  uint16_t temp;
  if(hspw<8) {
    SPI_CmdWrite(REG_HPWR);                    //0x19, HSYNC Pulse Width Register (HPWR)
    SPI.LCD_DataWrite(0x00);                   //
  } else {
    temp = (hspw / 8) - 1;
    SPI_CmdWrite(REG_HPWR);                    //0x19, HSYNC Pulse Width Register (HPWR)
    SPI_DataWrite(temp);                       //
  }
}


//================================================================================
//
// [0x1c] Vertical Non-Display Period Register 0(VNDR0)
// [0x1d] Vertical Non-Display Period Register 1(VNDR1)
//
//================================================================================

/**
 * @brief Define o Período Vertical de Não Exibição do LCD
 *        
 *        [1Ch] Vertical Non-Display Period Bit[7:0]
 *        Vertical Non-Display Period(Line) = (VNDR + 1)
 *
 *        [1Dh] Vertical Non-Display Period Bit[9:8]
 *        Vertical Non-Display Period(Line) = (VNDR + 1)
 *
 * @param vbpd VS Back Porch (Blanking)
 *
 * @note None
 */
void Panel_RA8889::Vertical_NonDisplay(uint16_t vbpd)
{
  uint16_t temp;
  temp = vbpd - 1;
  SPI_CmdWrite(REG_VNDR0);                     //0x1c, Vertical Non-Display Period Register 0(VNDR0)       
  SPI_DataWrite(temp);                         //
  SPI_CmdWrite(REG_VNDR1);                     //0x1d, Vertical Non-Display Period Register 1(VNDR1)
  SPI_DataWrite(temp >> 8);	                   //
}


//================================================================================
//
// [0x1e] VSYNC Start Position Register (VSTR)
//
//================================================================================


/**
 * @brief 
 *        
 *        [18h] HSYNC Start Position[4:0]
 *        The starting position from the end of display area to the
 *        beginning of HSYNC. Each level of this modulation is 8-pixel.
 *        Also called front porch.
 *        HSYNC Start Position(pixels) = (HSTR + 1)x8
 *
 * @param vfpd: VS Front Porch
 *
 * @note None
 */
void Panel_RA8889::VSYNC_StartPosition(uint16_t vfpd)
{
    uint8_t temp;
    temp = vfpd - 1;
    SPI_CmdWrite(REG_VSTR);                    //0x1e, VSYNC Start Position Register (VSTR)
    SPI_DataWrite(temp);
}


//================================================================================
//
// [0x1f] VSYNC Pulse Width Register (VPWR)
//
//================================================================================


/**
 * @brief 
 *        
 *         [1Fh] VSYNC Pulse Width[5:0]
 *         The pulse width of VSYNC in lines.
 *         VSYNC Pulse Width(Line) = (VPWR + 1)
 *
 * @param vspw: VS Pulse Width
 *
 * @note None
 */
void Panel_RA8889::VSYNC_PulseWidth(uint8_t vspw)
{
  uint8_t temp;
  temp = vspw - 1;
  SPI_CmdWrite(REG_VPWR);                //0x1f, VSYNC Pulse Width Register (VPWR)
  SPI_DataWrite(temp);
}


//================================================================================
//
// [0x20] Main Image Start Address 0 (MISA0)
// [0x21] Main Image Start Address 1 (MISA1)
// [0x22] Main Image Start Address 2 (MISA2)
// [0x23] Main Image Start Address 3 (MISA3)
//
//================================================================================


/**
 * @brief Page Layer Start Address
 *
 * @param layer: camada 0..MAX_LAYER-1 (layer 0, layer 1, layer 2....)
 *
 * @note  Page (image buffer) Configure: 
 *        The maximum number of pages is based on SDRAM capacity and color 
 *        depth and width and height of one page.
 *        For example we used 128Mbit SDRAM that capacity =  16Mbyte
 *        The SDRAM is divided into several image buffers and the maximum 
 *        number of image buffers is limited by the memory size. For example : 
 *        page_size = 800*600*2byte(16bpp) = 960000byte, 
 *        maximum number = 16/0.96 = 16.6
 *        
 *        Vertical mulit page application
 *        
 *        Se o valor for alem do numero de paginas permitida pelo display, ira 
 *        retornar o endreço da primeira pagina 0, ou seja endreço 0
 *
 * @return Page layer start address
 */
uint32_t Panel_RA8889::LayerStartAddr(uint8_t layer)
{
  if (layer > MAX_LAYER-1) return 0;
  return _width * _height * (_bpp/8) * layer;   //ex. 800x480 * (16 (16bpp)/8) * 1 = 768000 = 0xbb800
}


/**
 * @brief Main Image Start Address
 *        
 *        REG [20h] Main Image Start Address[7:2]   : Deve ser divisível por 4. O bit [1:0] está vinculado a “0” internamente.
 *        REG [21h] Main Image Start Address[15:8]
 *        REG [22h] Main Image Start Address [23:16]
 *        REG [23h] Main Image Start Address [31:24]
 *
 * @param addr
 *
 * @note None
 */
void Panel_RA8889::MainImage_StartAddress(uint32_t addr)
{
  RegisterWrite(REG_MISA0, addr);              //0x20, Main Image Start Address 0 (MISA0)
  RegisterWrite(REG_MISA1, addr >> 8);         //0x21, Main Image Start Address 1 (MISA1)
  RegisterWrite(REG_MISA2, addr >> 16);        //0x22, Main Image Start Address 2 (MISA2)
  RegisterWrite(REG_MISA3, addr >> 24);	       //0x23, Main Image Start Address 3 (MISA3)
}


//================================================================================
//
// [0x24] Main Image Width 0 (MIW0)
// [0x25] Main Image Width 1 (MIW1)
//
//================================================================================


/**
 * @brief Main Image Width
 *        
 *        REG [24h] Main Image Width [7:0]
 *        REG [25h] Main Image Width [12:8]
 *
 *        Unit: Pixel.
 *        It must be divisible by 4. MIW Bit [1:0] tie to “0” internally.
 *        The value is physical pixel number.
 *        Maximum value is 8188 pixels
 *
 * @param wx
 *
 * @note None
 */
void Panel_RA8889::MainImage_Width(uint16_t Wx)
{
  RegisterWrite(REG_MIW0, Wx);                 //0x24, Main Image Width 0 (MIW0) 
  RegisterWrite(REG_MIW1, Wx >> 8);            //0x25, Main Image Width 1 (MIW1)
}


//================================================================================
//
// [0x26] Main Window Upper-Left corner X-coordinates 0 (MWULX0)
// [0x27] Main Window Upper-Left corner X-coordinates 1 (MWULX1)
// [0x28] Main Window Upper-Left corner Y-coordinates 0 (MWULY0)
// [0x29] Main Window Upper-Left corner Y-coordinates 1 (MWULY1)
//
//================================================================================


/**
 * @brief Main Windows Start Position
 *
 *        
 *        REG [26h] Main Window Upper-Left corner X-coordinates 0 (MWULX0)
 *                  Main Window Upper-Left corner X-coordination [7:0] 
 *        REG [27h] Main Window Upper-Left corner X-coordinates 1 (MWULX1)
 *                  Main Window Upper-Left corner X-coordination [12:8]  
 *        
 *        Reference Main Image coordinates.
 *        It must be divisible by 4. MWULX Bit [1:0] tie to "0" internally.
 *        X-axis coordination plus Horizontal display width cannot large 
 *        than 8188.
 *
 *        [28h] Main Window Upper-Left corner Y-coordinates 0 (MWULY0)
 *              Main Window Upper-Left corner Y-coordination [7:0]
 *        [29h] Main Window Upper-Left corner Y-coordinates 1 (MWULY1)
 *              Main Window Upper-Left corner Y-coordination [12:8]
 * 
 *        Reference Main Image coordinates.
 *        Range is between 0 and 8191.
 *
 * @param wx, hy : coordinate (x, y) in pixel
 *
 * @note None
 */
void Panel_RA8889::MainWindow_StartXY(uint16_t wx, uint16_t hy)  
{
  RegisterWrite(REG_MWULX0, wx);               //0x026, Main Window Upper-Left corner X-coordinates 0 (MWULX0)
  RegisterWrite(REG_MWULX1, wx >> 8);          //0x027, Main Window Upper-Left corner X-coordinates 1 (MWULX1)
  RegisterWrite(REG_MWULY0, hy);               //0x028, Main Window Upper-Left corner Y-coordinates 0 (MWULY0)
  RegisterWrite(REG_MWULY1, hy >> 8);          //0x029, Main Window Upper-Left corner Y-coordinates 1 (MWULY1)
}


//================================================================================
//
// [0x46] PAGE Switch
//
//================================================================================


/**
 * @brief PAGE Switch
 *
 *        REG [46h] PAGE Switch
 *                  bit [0] Page switch, and SPIM bus switch
 *                          0: page 0, SPIM bus 0, for lower 256 register setting (Default)
 *                          1: page 1, SPIM bus 1, for the register settings of media decoder
 *
 * @param ePageReg::Page0, ePageReg::Page1
 *
 * @note RA8889 has two page-page0 / page1 registers. Users can switch 
 *       page1 / page0 at REG [46h] bit0 of page0 / page1. Default is Page 0
 */
void Panel_RA8889::PageSwitch(ePageReg pr)
{
  uint8_t temp;
  SPI_CmdWrite(REG_PAGE_SWITCH);              //0x46, PAGE Switch
  temp = SPI_DataRead();
  temp &= cClrb0;                             //Rest bit 0
  temp |= static_cast<uint8_t>(pr);           //Converte enum para uint8_t  
  SPI_DataWrite(temp);
  delay(1);
}


//================================================================================
//
// [0x50] Canvas Start address 0 (CVSSA0)
// [0x51] Canvas Start address 1 (CVSSA1)
// [0x52] Canvas Start address 2 (CVSSA2)
// [0x53] Canvas Start address 3 (CVSSA3)
//
//================================================================================


/**
 * @brief 
 *
 * @verbatim
 *        REG [50h] Canvas Start address 0 (CVSSA0)
 *                  Start address of Canvas [7:0]
 *        REG [51h] Canvas Start address 1 (CVSSA1)
 *                  Start address of Canvas [15:8]
 *        REG [52h] Canvas Start address 2 (CVSSA2)
 *                  Start address of Canvas [23:16]
 *        REG [53h] Canvas Start address 3 (CVSSA3)
 *                  Start address of Canvas [31:24]
 * @endverbatim
 *
 * @param addr: endereço
 *
 * @note None
 */
void Panel_RA8889::CanvasImage_StartAddr(uint32_t addr)
{
  RegisterWrite(REG_CVSSA0, addr);             //0x50, Canvas Start address 0 (CVSSA0)
  RegisterWrite(REG_CVSSA1, addr >> 8);        //0x51, Canvas Start address 1 (CVSSA1)
  RegisterWrite(REG_CVSSA2, addr >> 16);       //0x52, Canvas Start address 2 (CVSSA2)
  RegisterWrite(REG_CVSSA3, addr >> 24);       //0x53, Canvas Start address 3 (CVSSA3)
}


//================================================================================
//
// [0x54] Canvas image width 0 (CVS_IMWTH0)
// [0x55] Canvas image width 1 (CVS_IMWTH1)
//
//================================================================================


/**
 * @brief 
 *
 * @verbatim
 *        REG [54h] Canvas image width 0 (CVS_IMWTH0)
 *                  Canvas image width [7:2]
 *                  The bits are Canvas image width.
 *                  Unit: Pixel, it is 4 pixel resolutions.
 *                  Width = Set Value
 *                  Ignored if canvas is in linear addressing mode.
 *        REG [55h] Canvas image width 1 (CVS_IMWTH1)
 *                  Canvas image width [12:8]
 *                  The bits are Canvas image width
 *                  Ignored if canvas is in linear addressing mode.
 * @endverbatim
 *
 * @param Wx: width
 *
 * @note None
 */
void Panel_RA8889::CanvasImage_Width(uint16_t Wx)
{
  RegisterWrite(REG_CVS_IMWTH0, Wx);           //0x54, Canvas image width 0 (CVS_IMWTH0)
  RegisterWrite(REG_CVS_IMWTH1, Wx >> 8);      //0x55, Canvas image width 1 (CVS_IMWTH1)
}


//================================================================================
//
// [0x56] Active Window Upper-Left corner X-coordinates 0 (AWUL_X0)
// [0x57] Active Window Upper-Left corner X-coordinates 1 (AWUL_X1)
// [0x58] Active Window Upper-Left corner Y-coordinates 0 (AWUL_Y0)
// [0x59] Active Window Upper-Left corner Y-coordinates 1 (AWUL_Y1)
//
//================================================================================


/**
 * @brief 
 *        
 * @verbatim
 *        REG [56h] Active Window Upper-Left corner X-coordinates 0 (AWUL_X0)
 *                  Active Window Upper-Left corner X-coordination [7:0]
 *                  Please refer to the Canvas image coordinates.
 *                  Unit: Pixel
 *                  X-axis coordinates plus Active Window width cannot be larger than 8188.
 *                  Ignored if canvas is in linear addressing mode.
 *        REG [57h] Active Window Upper-Left corner X-coordinates 1 (AWUL_X1)
 *                  Active Window Upper-Left corner X-coordination [12:8]
 *                  Please refer to the Canvas image coordinates.
 *                  Unit: Pixel
 *                  X-axis coordinates plus Active Window width cannot be larger than 8188.
 *                  Ignored if canvas is in linear addressing mode.
 *        REG [58h] Active Window Upper-Left corner Y-coordinates 0 (AWUL_Y0)
 *                  Active Window Upper-Left corner Y-coordination [7:0]
 *                  Please refer to the Canvas image coordinates.
 *                  Unit: Pixel
 *                  Y-axis coordinates plus Active Window height cannot be larger than 8191.
 *                  Ignored if canvas is in linear addressing mode.
 *        REG [59h] Active Window Upper-Left corner Y-coordinates 1 (AWUL_Y1)
 *                  Active Window Upper-Left corner Y-coordination [12:8] 
 *                  Please refer to the Canvas image coordinates.
 *                  Unit: Pixel
 *                  Y-axis coordinates plus Active Window height cannot large than 8191.
 *                  Ignored if canvas is in linear addressing mode.
 * @endverbatim
 *
 * @param wx: width, hy: height
 *
 * @note None
 */
void Panel_RA8889::ActiveWindow_XY(uint16_t Wx, uint16_t Hy)
{
  RegisterWrite(REG_AWUL_X0, Wx);              //0x56, Active Window Upper-Left corner X-coordinates 0 (AWUL_X0)
  RegisterWrite(REG_AWUL_X1, Wx >> 8);         //0x57, Active Window Upper-Left corner X-coordinates 1 (AWUL_X1)
  RegisterWrite(REG_AWUL_Y0, Hy);              //0x58, Active Window Upper-Left corner Y-coordinates 0 (AWUL_Y0)
  RegisterWrite(REG_AWUL_Y1, Hy >> 8);         //0x59, Active Window Upper-Left corner Y-coordinates 1 (AWUL_Y1)
}


//================================================================================
//
// [0x5A] Active Window Width 0 (AW_WTH0)
// [0x5B] Active Window Width 1 (AW_WTH1)
// [0x5C] Active Window Height 0 (AW_HT0)
// [0x5D] Active Window Height 1 (AW_HT1)
//
//================================================================================


/**
 * @brief 
 *
 * @endverbatim
 *        REG [0x5a] Active Window Width 0 (AW_WTH0)
 *                   Width of Active Window [7:0]
 *        REG [0x5b] Active Window Width 1 (AW_WTH1)
 *                   Width of Active Window [12:8]
 *        REG [0x5c] Active Window Height 0 (AW_HT0)
 *                   Height of Active Window [7:0]
 *        REG [0x5d] Active Window Height 1 (AW_HT1)
 *                   Height of Active Window [12:8]
 * @endverbatim
 *
 * @param wx: width, hy: height
 *
 * @note None
 */
void Panel_RA8889::ActiveWindow_WidhtHeight(uint16_t Wx, uint16_t Hy)  
{
  RegisterWrite(REG_AW_WTH0, Wx);              //0x5a, Active Window Width 0 (AW_WTH0)
  RegisterWrite(REG_AW_WTH1, Wx >> 8);         //0x5b, Active Window Width 1 (AW_WTH1)
  RegisterWrite(REG_AW_HT0, Hy);               //0x5c, Active Window Height 0 (AW_HT0)
  RegisterWrite(REG_AW_HT1, Hy >> 8);          //0x5d, Active Window Height 1 (AW_HT1)
}


//================================================================================
//
// [0x5E] Color Depth of Canvas & Active Window (AW_COLOR)
//
//================================================================================


/**
 * @brief Block Mode X-Y Coordinates Addressing
 *        
 * @verbatim
 * REG [0x5e] Color Depth of Canvas & Active Window (AW_COLOR)
 *            bit [2] Canvas addressing mode
 *                    0b0: Block mode (X-Y coordinates addressing)
 *                    0b1: Linear mode
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::Memory_BlockMode(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_AW_COLOR);                //0x5e, Color Depth of Canvas & Active Window (AW_COLOR)
  temp = SPI_DataRead();
  temp &= cClrb2;                            //Reset bit 2
  SPI_DataWrite(temp);
}
void Panel_RA8889::Memory_XYMode(void) { Memory_BlockMode(); }


/**
 * @brief Check for Block Mode X-Y Coordinates Addressing
 *
 * @verbatim
 *        REG [0x5e] Color Depth of Canvas & Active Window (AW_COLOR)
 *                   bit [2] Canvas addressing mode
 *                           0b0: Block mode (X-Y coordinates addressing)
 *                           0b1: Linear mode
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
bool Panel_RA8889::Memory_IsBlockMode(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_AW_COLOR);                //0x5e, Color Depth of Canvas & Active Window (AW_COLOR)
  temp = SPI_DataRead();
  return (temp &= cSetb2) == 0x00;
}
bool Panel_RA8889::Memory_IsXYMode(void) { return Memory_BlockMode(); }


/**
 * @brief Linear Mode Addressing
 *
 * @verbatim
 *        REG [0x5e] Color Depth of Canvas & Active Window (AW_COLOR)
 *                   bit [2] Canvas addressing mode
 *                           0b0: Block mode (X-Y coordinates addressing)
 *                           0b1: Linear mode
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::Memory_LinearMode(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_AW_COLOR);                //0x5e, Color Depth of Canvas & Active Window (AW_COLOR)
  temp = SPI_DataRead();
  temp |= cSetb2;                            //Set bit 2
  SPI_DataWrite(temp);
}


/**
 * @brief Check for Linear Mode Addressing
 *
 * @verbatim
 * REG [0x5e] Color Depth of Canvas & Active Window (AW_COLOR)
 *            bit [2] Canvas addressing mode
 *                    0b0: Block mode (X-Y coordinates addressing)
 *                    0b1: Linear mode
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
bool Panel_RA8889::Memory_IsLinearMode(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_AW_COLOR);                //0x5e, Color Depth of Canvas & Active Window (AW_COLOR)
  temp = SPI_DataRead();
  return (temp &= cSetb2) == cSetb2;
}


/**
 * @brief Profundidade de cor da imagem da tela e largura dos dados de leitura 
 *        e gravação da memória no modo de bloco 8bpp
 *
 * @verbatim
 * REG [0x5e] Color Depth of Canvas & Active Window (AW_COLOR)
 *            bit [1-0] Canvas image’s color depth & memory R/W data width
 *                      In Block Mode:
 *                      00: 8bpp
 *                      01: 16bpp
 *                      1x: 24bpp
 *                      In Linear Mode:
 *                      x0: 8-bits memory data read/write.
 *                      x1: 16-bits memory data read/write
 * @endverbatim
 *
 * @param None
 *
 * @note Use esta funcao somente se ativou o modo de Bloco de endereçamento X-Y bit[2]=0b0
 */
void Panel_RA8889::Memory_8bpp_BlockMode(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_AW_COLOR);                //0x5e, Color Depth of Canvas & Active Window (AW_COLOR)
  temp = SPI_DataRead();
  temp &= cClrb1;                            //Reset bit 1
  temp &= cClrb0;                            //Reset bit 2
  SPI_DataWrite(temp);                       //Set block mode x-y 8bpp
}


/**
 * @brief Profundidade de cor da imagem da tela e largura dos dados de leitura 
 *        e gravação da memória no modo de bloco 16bpp
 *
 * @verbatim
 * REG [0x5e] Color Depth of Canvas & Active Window (AW_COLOR)
 *            bit [1-0] Canvas image’s color depth & memory R/W data width
 *                      In Block Mode:
 *                      00: 8bpp
 *                      01: 16bpp
 *                      1x: 24bpp
 *                      In Linear Mode:
 *                      x0: 8-bits memory data read/write.
 *                      x1: 16-bits memory data read/write
 * @endverbatim
 *
 * @param None
 *
 * @note Use esta funcao somente se ativou o modo de Bloco de endereçamento X-Y bit[2]=0b0
 */
void Panel_RA8889::Memory_16bpp_BlockMode(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_AW_COLOR);                //0x5e, Color Depth of Canvas & Active Window (AW_COLOR)
  temp = SPI_DataRead();
  temp &= cClrb1;                            //Reset bit 1
  temp |= cSetb0;                            //Set bit 0
  SPI_DataWrite(temp);                       //Set block mode x-y 16bpp
}


/**
 * @brief Profundidade de cor da imagem da tela e largura dos dados de leitura 
 *        e gravação da memória no modo de bloco 24bpp
 *
 * @verbatim
 * REG [0x5e] Color Depth of Canvas & Active Window (AW_COLOR)
 *            bit [1-0] Canvas image’s color depth & memory R/W data width
 *                      In Block Mode:
 *                      00: 8bpp
 *                      01: 16bpp
 *                      1x: 24bpp
 *                      In Linear Mode:
 *                      x0: 8-bits memory data read/write.
 *                      x1: 16-bits memory data read/write
 * @endverbatim
 *
 * @param None
 *
 * @note Use esta funcao somente se ativou o modo de Bloco de endereçamento X-Y bit[2]=0b0
 */
void Panel_RA8889::Memory_24bpp_BlockMode(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_AW_COLOR);                //0x5e, Color Depth of Canvas & Active Window (AW_COLOR)
  temp = SPI_DataRead();
  temp |= cSetb1;                            //Set bit 1
  SPI_DataWrite(temp);                       //Set block mode x-y 24bpp
}


//================================================================================
//
// [0x5F] Graphic Read/Write position Horizontal Position Register 0 (CURH0)
// [0x60] Graphic Read/Write position Horizontal Position Register 1 (CURH1)
// [0x61] Graphic Read/Write position Vertical Position Register 0 (CURV0)
// [0x62] Graphic Read/Write position Vertical Position Register 1 (CURV1)
//
//================================================================================


/**
 * @brief Set Graphic Read/Write Position X,Y
 *
 * @verbatim
 *        User should program proper active window related parameters before configure this register.
 *                   
 *        REG [0x5f] Graphic Read/Write position Horizontal Position Register 0 (CURH0)
 *                   bit [7~0] Write: Set Graphic Read/Write position
 *                             When Canvas In Linear mode:
 *                             Memory Read/Write address [7:0]
 *                             Unit: Byte
 *                             When Canvas In Block mode:
 *                             Graphic Read/Write Horizontal Position 0 [7:0]
 *                             Please refer to the Canvas image coordinates.
 *                             Unit: Pixel                   
 *        REG [0x60] Graphic Read/Write position Horizontal Position Register 1 (CURH1)
 *                   bit [7~5] Write: Set Graphic Read/Write position
 *                             When Canvas In Linear mode:
 *                             Memory Read/Write address [15:13]
 *                             Unit: Byte
 *                             When Canvas In Block mode: NA
 *                             Please refer to the Canvas image coordinates.
 *                             Unit: Pixel
 *                   bit [4~0] Write: Set Graphic Read/Write position
 *                             When Canvas In Linear mode:
 *                             Memory Read/Write address [12:8]
 *                             Unit: Byte
 *                             When Canvas In Block mode:
 *                             Graphic Read/Write Horizontal Position 1 [12:8]
 *                             Please refer to the Canvas image coordinates.
 *                             Unit: Pixel
 *        REG [0x61] Graphic Read/Write position Vertical Position Register 0 (CURV0)
 *                   bit [7~0] Write: Set Graphic Read/Write position
 *                             When Canvas In Linear mode:
 *                             Memory Read/Write address [23:16]
 *                             Unit: Byte
 *                             When Canvas In Block mode:
 *                             Graphic Read/Write Vertical Position 0 [7:0]
 *                             Please refer to the Canvas image coordinates.
 *                             Unit: Pixel
 *        REG [0x62] Graphic Read/Write position Vertical Position Register 1 (CURV1)
 *                   bit [7~5] Write: Set Graphic Read/Write position
 *                             When Canvas In Linear mode:
 *                             Memory Read/Write address [31:29]
 *                             Unit: Byte
 *                             When Canvas In Block mode:NA
 *                             Please refer to the Canvas image coordinates.
 *                             Unit: Pixel
 *                   bit [4~0] Write: Set Graphic Read/Write position
 *                             When Canvas In Linear mode:
 *                             Memory Read/Write address [28:24]
 *                             Unit: Byte
 *                             When Canvas In Block mode:
 *                             Graphic Read/Write Vertical Position 1 [12:8]
 *                             Please refer to the Canvas image coordinates.
 *                             Unit: Pixel
 * @endverbatim
 *
 * @param (Wx, Hy): Posicao de coordenada
 *
 * @note        
 *        REG[5Eh].bit[3] Não existe a funcionalidade descrita por alguns autores 
 *        no RA8889/8877/8876/8875/8870
 *
 *        Nota: REG[0x5E].bit2 = AW_COLOR Linear/Block mode (em RA8889 family).
 *        Quando Linear Mode = 1, registradores DMA (C0..C3) são interpretados como
 *        Destination Address [7:0]..[31:2] na SDRAM (32-bit). 
 *        Quando em Block Mode = 0, registrador de DMR (C0..C3) são interpretados como
 *        coordenadas X e Y a partir de 0,0. Não existe REG[0x5E].bit3 com essa função.
 *
 *        Precisa estar no modo SDRAM de Bloco, ou seja de coordenadas x,y. Para 
 *        verificar o modo atual, leia o registrador [5Eh].bit[2]
 *
 *        Original da RAIO:
 *  
 *        REG[5Eh] bit3, Select to read back Graphic Read/Write position.
 *                 When DPRAM Linear mode:        Graphic Read/Write Position [31:24][23:16][15:8][7:0]
 *                 When DPRAM Active window mode: Graphic Read/Write
 *                 Horizontal Position [12:8][7:0],
 *                 Vertical Position [12:8][7:0].
 *                 Reference Canvas image coordinate. Unit: Pixel
 *
 * Use este para posicionar o local de um pixel na tela
 */
void Panel_RA8889::GotoPixel_XY(uint16_t Wx, uint16_t Hy)
{
  SPI_CmdWrite(REG_CURH0);       //0x5f, Graphic Read/Write position Horizontal Position Register 0 (CURH0)
  SPI_DataWrite(Wx);             //byte baixo de x
  SPI_CmdWrite(REG_CURH1);       //0x60, Graphic Read/Write position Horizontal Position Register 1 (CURH1)
  SPI_DataWrite(Wx >> 8);        //byte alto de x
							     
  SPI_CmdWrite(REG_CURV0);       //0x61, Graphic Read/Write position Vertical Position Register 0 (CURV0)
  SPI_DataWrite(Hy);             //byte baixo de y
  SPI_CmdWrite(REG_CURV1);       //0x62, raphic Read/Write position Vertical Position Register 1 (CURV1)
  SPI_DataWrite(Hy >> 8);        //byte alto de y
}


/**
 * @brief Set Graphic Read/Write Position Linear
 *
 * @verbatim
 *        User should program proper active window related parameters before configure this register.
 *                   
 *        REG [0x5F] Graphic Read/Write position Horizontal Position Register 0 (CURH0)
 *                   bit [7~0] Write: Set Graphic Read/Write position
 *                             When Canvas In Linear mode:
 *                             Memory Read/Write address [7:0]
 *                             Unit: Byte
 *                             When Canvas In Block mode:
 *                             Graphic Read/Write Horizontal Position 0 [7:0]
 *                             Please refer to the Canvas image coordinates.
 *                             Unit: Pixel                   
 *        REG [0x60] Graphic Read/Write position Horizontal Position Register 1 (CURH1)
 *                   bit [7~5] Write: Set Graphic Read/Write position
 *                             When Canvas In Linear mode:
 *                             Memory Read/Write address [15:13]
 *                             Unit: Byte
 *                             When Canvas In Block mode: NA
 *                             Please refer to the Canvas image coordinates.
 *                             Unit: Pixel
 *                   bit [4~0] Write: Set Graphic Read/Write position
 *                             When Canvas In Linear mode:
 *                             Memory Read/Write address [12:8]
 *                             Unit: Byte
 *                             When Canvas In Block mode:
 *                             Graphic Read/Write Horizontal Position 1 [12:8]
 *                             Please refer to the Canvas image coordinates.
 *                             Unit: Pixel
 *        REG [0x61] Graphic Read/Write position Vertical Position Register 0 (CURV0)
 *                   bit [7~0] Write: Set Graphic Read/Write position
 *                             When Canvas In Linear mode:
 *                             Memory Read/Write address [23:16]
 *                             Unit: Byte
 *                             When Canvas In Block mode:
 *                             Graphic Read/Write Vertical Position 0 [7:0]
 *                             Please refer to the Canvas image coordinates.
 *                             Unit: Pixel
 *        REG [0x62] Graphic Read/Write position Vertical Position Register 1 (CURV1)
 *                   bit [7~5] Write: Set Graphic Read/Write position
 *                             When Canvas In Linear mode:
 *                             Memory Read/Write address [31:29]
 *                             Unit: Byte
 *                             When Canvas In Block mode:NA
 *                             Please refer to the Canvas image coordinates.
 *                             Unit: Pixel
 *                   bit [4~0] Write: Set Graphic Read/Write position
 *                             When Canvas In Linear mode:
 *                             Memory Read/Write address [28:24]
 *                             Unit: Byte
 *                             When Canvas In Block mode:
 *                             Graphic Read/Write Vertical Position 1 [12:8]
 *                             Please refer to the Canvas image coordinates.
 *                             Unit: Pixel
 * @endverbatim
 *
 * @param (Wx, Hy): Posicao de coordenada
 *
 * @note        
 *        REG[5Eh].bit[3] Não exsite a funcionalidade descrita por alguns autores 
 *        no RA8889/8877/8876/8875/8870
 *
 *        Nota: REG[0x5E].bit2 = AW_COLOR Linear/Block mode (em RA8889 family).
 *        Quando Linear Mode = 1, registradores DMA (C0..C3) são interpretados como
 *        Destination Address [7:0]..[31:2] na SDRAM (32-bit). 
 *        Quando em Block Mode = 0, registrador de DMR (C0..C3) são interpretados como
 *        coordenadas X e Y a partir de 0,0. Não existe REG[0x5E].bit3 com essa função.
 *
 *        Precisa estar no modo SDRAM de Bloco, ou seja de coordenadas x,y. Para 
 *        verificar o modo atual, leia o registrador [5Eh].bit[2]
 *
 *        Original da RAIO:
 *  
 *        REG[5Eh] bit3, Select to read back Graphic Read/Write position.
 *                 When DPRAM Linear mode:        Graphic Read/Write Position [31:24][23:16][15:8][7:0]
 *                 When DPRAM Active window mode: Graphic Read/Write
 *                 Horizontal Position [12:8][7:0],
 *                 Vertical Position [12:8][7:0].
 *                 Reference Canvas image coordinate. Unit: Pixel
 */
void Panel_RA8889::GotoPixel_Linear(uint32_t addr)
{
  SPI_CmdWrite(REG_CURH0);       //0x5f, Graphic Read/Write position Horizontal Position Register 0 (CURH0)
  SPI_DataWrite(addr);           //bit [7..0] do endreço
  SPI_CmdWrite(REG_CURH1);       //0x60, Graphic Read/Write position Horizontal Position Register 1 (CURH1)
  SPI_DataWrite(addr >> 8);      //bit [15..8] do endereço
  SPI_CmdWrite(REG_CURV0);       //0x61, Graphic Read/Write position Vertical Position Register 0 (CURV0)
  SPI_DataWrite(Addr >> 16);     //bit [23..16] do endereço
  SPI_CmdWrite(REG_CURV1);       //0x62, raphic Read/Write position Vertical Position Register 1 (CURV1)
  SPI_DataWrite(Addr >> 24);     //bit [31..24] do endereço
}
void Panel_RA8889::GotoLinearAddr(uint32_t addr) { GotoPixel_Linear(addr); }


//================================================================================
//
// [0x63] Text Write X-coordinates Register 0 (F_CURX0)
// [0x64] Text Write X-coordinates Register 1 (F_CURX1)
// [0x65] Text Write Y-coordinates Register 0 (F_CURY0)
// [0x66] Text Write Y-coordinates Register 1 (F_CURY1)
//
//================================================================================


/**
 * @brief Set Text Write Position X-Y
 *
 * @verbatim
 * User should program proper active window related parameters before configure this register.
 *            
 * REG [0x63] Text Write X-coordinates Register 0 (F_CURX0)
 *            bit [7~0] Write: Set Text Write position
 *                      Read: Current Text Write position
 *                      Text Write X-coordinates [7:0]
 *                      The setting of the horizontal cursor position for text writing.
 *                      Please refer to the Canvas image coordinates.
 *                      Unit: Pixel
 *   
 * REG [0x64] Text Write X-coordinates Register 1 (F_CURX1)
 *            bit [4~0] Write: Set Text Write position
 *                      Read: Current Text Write position
 *                      Text Write X-coordinates [12:8]
 *                      The setting of the horizontal cursor position for text writing.
 *                      Please refer to the Canvas image coordinates.
 *                      Unit: Pixel
 * REG [0x65] Text Write Y-coordinates Register 0 (F_CURY0)
 *            bit [7~0] Write: Set Text Write position
 *                      Read: Current Text Write position
 *                      Text Write Y-coordinates [7:0]
 *                      The setting of the vertical cursor position for text writing.
 *                      Please refer to the Canvas image coordinates.
 *                      Unit: Pixel
 * REG [0x66] Text Write Y-coordinates Register 1 (F_CURY1)
 *            bit [4~0] Write: Set Text Write position
 *                      Read: Current Text Write position
 *                      Text Write Y-coordinates [12:8]
 *                      The setting of the vertical cursor position for text writing.
 *                      Please refer to the Canvas image coordinates.
 *                      Unit: Pixel
 * @endverbatim
 *
 * @param (Wx, Hy): Posicao de coordenada
 *
 * @note  Precisa estar no modo SDRAM de Bloco, ou seja de coordenadas x,y. Para 
 *        verificar o modo atual, leia o registrador [5Eh].bit[2]
 *
 */
void Panel_RA8889::GotoText_XY(uint16_t Wx, uint16_t Hy)
{
  SPI_CmdWrite(REG_F_CURX0);                   //0x63, Text Write X-coordinates Register 0 (F_CURX0)
  SPI_DataWrite(Wx);                           //Text Write X-coordinate [7:0]
  SPI_CmdWrite(REG_F_CURX1);                   //0x64, Text Write X-coordinates Register 1 (F_CURX1)
  SPI_DataWrite(Wx >> 8);                      //Text Write X-coordinate [12:8]
  SPI_CmdWrite(REG_F_CURY0);                   //0x65, Text Write Y-coordinates Register 0 (F_CURY0)
  SPI_DataWrite(Hy);                           //Text Write Y-coordinate [7:0]
  SPI_CmdWrite(REG_F_CURY1);                   //0x66, Text Write Y-coordinates Register 1 (F_CURY1)
  SPI_DataWrite(Hy >> 8);                      //Text Write Y-coordinate [12:8]
}


//================================================================================
//
// [0x67] Draw Line / Triangle Control Register 0 (DCR0)
//
//================================================================================


/**
 * @brief Enable/Disable Drawing
 *        
 * @param b: true habilita, false: desabilita a linha de desenhos
 *
 * @note Não está descrito no manual. indica apenas que o bit[0] precisa 
 *       ser zero. Não existe descrição apra o RA8875/RA8876/RA8877/RA8889
 */ 
void Panel_RA8889::DrawEnable_AA(bool b)
{
  uint8_t temp;
  SPI_CmdWrite(REG_DCR0);           //0x67, Draw Line / Triangle Control Register 0 (DCR0)
  temp = SPI_DataRead();
  temp &= cClrb0;                   //Reset bit 0
  if (b) temp |= cSetb0 else temp &= cClrb0;
  SPI_DataWrite(temp);
}


/**
 * @brief Ativa o Modo de desenho de Linha
 *
 * @verbatim
 * Draw Line Start Signal Write Function
 *
 * REG[67h] Draw Line / Triangle Control Register 0 (DCR0)
 *          bit [7] Draw Line / Triangle Start Signal 
 *                  Write Function:
 *                  0b0: Stop the drawing function.
 *                  0b1: Start the drawing function.
 *                  Read Function:
 *                  0b0 : Drawing function complete.
 *                  0b1 : Drawing function is processing.
 *          bit [1] Draw Triangle or Line Select Signal
 *                  0b0: Draw Line
 *                  0b1: Draw Triangle
 * @endverbatim
 *
 * @param None
 *
 * @note Antes de executar esta função precisa entrar com os valores em registradores de coordenadas
 *       através das funções Point1_XY(), Point2_XY().
 */ 
void Panel_RA8889::LineMode_Start(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_DCR0);                      //0x67, Draw Line / Triangle Control Register 0 (DCR0)
  temp = SPI_DataRead();                       
  temp &= cClrb1;                              //Reset bit 1, Select Draw Line
  temp |= cSetb7;                              //Set bit 7, Start draw function
  SPI_DataWrite(temp);                         
  CoreTask_WaitReady();                        //Espere ate ficar pronto
}


/**
 * @brief Ativa o Modo de desenho de Triangulo
 *        
 * @verbatim
 * Draw Triangle Start Signal Write Function Non-Fill
 *
 * REG[67h] Draw Line / Triangle Control Register 0 (DCR0)
 *          bit [7] Draw Line / Triangle Start Signal 
 *                  Write Function:
 *                  0b0: Stop the drawing function.
 *                  0b1: Start the drawing function.
 *                  Read Function:
 *                  0b0 : Drawing function complete.
 *                  0b1 : Drawing function is processing.
 *          bit [5] Fill function for Triangle Signal
 *                  0b0: Non fill.
 *                  0b1: Fill 
 *          bit [1] Draw Triangle or Line Select Signal
 *                  0b0: Draw Line
 *                  0b1: Draw Triangle
 * @endverbatim
 *
 * @param None
 *
 * @note Antes de executar esta função precisa entrar com os valores em registradores de coordenadas
 *       através das funções Point1_XY(), Point2_XY() e Point3_XY().
*/ 
void Panel_RA8889::TriangleMode_Start(bool fill)
{
  uint8_t temp;
  SPI_CmdWrite(REG_DCR0);                      //0x67, Draw Line / Triangle Control Register 0 (DCR0)
  temp = SPI_DataRead();                       
  temp |= cSetb1;                              //Set bit 1, Select Draw Triangle
  if (fill) temp |= cClrb5 else temp &= cClrb5; //Set bit 5, Com preenchimento do triangulo
  temp |= cSetb7;                              //Set bit 7, Draw Triangle
  SPI_DataWrite(temp);
  CoreTask_WaitReady();                        //Espere ate ficar pronto
}


//================================================================================
//
// [0x68] Draw Line/Square/Triangle Point 1 X-coordinates Register0 (DLHSR0)
// [0x69] Draw Line/Square/Triangle Point 1 X-coordinates Register1 (DLHSR1)
// [0x6a] Draw Line/Square/Triangle Point 1 Y-coordinates Register0 (DLVSR0)
// [0x6b] Draw Line/Square/Triangle Point 1 Y-coordinates Register1 (DLVSR1)
// [0x6c] Draw Line/Square/Triangle Point 2 X-coordinates Register0 (DLHER0)
// [0x6d] Draw Line/Square/Triangle Point 2 X-coordinates Register1 (DLHER1)
// [0x6e] Draw Line/Square/Triangle Point 2 Y-coordinates Register0 (DLVER0)
// [0x6f] Draw Line/Square/Triangle Point 2 Y-coordinates Register1 (DLVER1)
// [0x70] Draw Triangle Point 3 X-coordinates Register 0 (DTPH0)
// [0x71] Draw Triangle Point 3 X-coordinates Register 1 (DTPH1)
// [0x72] Draw Triangle Point 3 Y-coordinates Register 0 (DTPV0)
// [0x73] Draw Triangle Point 3 Y-coordinates Register 1 (DTPV1)
//
//================================================================================


/**
 * @brief Line Start Point
 *
 * @verbatim
 *        REG [68h] Draw Line/Square/Triangle Point 1 X-coordinates Register0 (DLHSR0)
 *                  bit [7~0] Draw Line/Square/Triangle Start X-coordinates [7:0]
 *        REG [69h] Draw Line/Square/Triangle Point 1 X-coordinates Register1 (DLHSR1)
 *                  bit [7~0] Draw Line/Square/Triangle Start X-coordinate [12:8]
 *        REG [6Ah] Draw Line/Square/Triangle Point 1 Y-coordinates Register0 (DLVSR0)
 *                  bit [7~0] Draw Line/Square/Triangle Start Y-coordinate [7:0]
 *        REG [6Bh] Draw Line/Square/Triangle Point 1 Y-coordinates Register1 (DLVSR1)
 *                  bit [7~0] Draw Line/Square/Triangle Start Y-coordinate [12:8]
 *
 *        Please refer to the Canvas image coordinates.
 *        Unit: Pixel 
 *        ***Note: When draw a square, start point & end point cannot be 
 *           located at the same point or at the same X-axis or Y-axis.
 * @endverbatim
 *
 * @param wx: ponto de coordenada x
 *        hy: ponto de coordenada y
 *
 * @note None
 */
void Panel_RA8889::Point1_XY(uint16_t wx, uint16_t hy)
{
  SPI_CmdWrite(REG_DLHSR0);                    //0x68, Draw Line/Square/Triangle Point 1 X-coordinates Register0 (DLHSR0)
  SPI_DataWrite(wx);                           
  SPI_CmdWrite(REG_DLHSR1);                    //0x69, Draw Line/Square/Triangle Point 1 X-coordinates Register1 (DLHSR1)
  SPI_DataWrite(wx >> 8);                      
  SPI_CmdWrite(REG_DLVSR0);                    //0x6a, Draw Line/Square/Triangle Point 1 Y-coordinates Register0 (DLVSR0)
  SPI_DataWrite(hy);                           
  SPI_CmdWrite(REG_DLVSR1);                    //0x6b, Draw Line/Square/Triangle Point 1 Y-coordinates Register1 (DLVSR1)
  SPI_DataWrite(hy >> 8);                      
}
void Panel_RA8889::Line_Point1XY(uint16_t wx, uint16_t hy) { Point1_XY(wx, hy); }


/**
 * @brief Line End Point
 *
 * @verbatim
 *        REG [6Ch] Draw Line/Square/Triangle Point 2 X-coordinates Register0 (DLHER0)
 *                  bit [7~0] Draw Line/Square/Triangle End X-coordinate [7:0]
 *        REG [6Dh] Draw Line/Square/Triangle Point 2 X-coordinates Register1 (DLHER1)
 *                  bit [7~0] Draw Line/Square/Triangle End X-coordinate [12:8]
 *        REG [6Eh] Draw Line/Square/Triangle Point 2 Y-coordinates Register0 (DLVER0)
 *                  bit [7~0] Draw Line/Square/Triangle End Y-coordinate [7:0]
 *        REG [6Fh] Draw Line/Square/Triangle Point 2 Y-coordinates Register1 (DLVER1)
 *                  bit [7~0] Draw Line/Square/Triangle End Y-coordinate [12:8]
 *
 *        Please refer to the Canvas image coordinates.
 *        Unit: Pixel 
 *        ***Note: When draw a square, start point & end point cannot be 
 *           located at the same point or at the same X-axis or Y-axis.
 * @endverbatim
 *
 * @param wx: ponto de coordenada x
 *        hy: ponto de coordenada y
 *
 * @note None
 */
void Panel_RA8889::Point2_XY(uint16_t wx, uint16_t hy)
{
  SPI_CmdWrite(REG_DLHER0);                    //0x6c, Draw Line/Square/Triangle Point 2 X-coordinates Register0 (DLHER0)
  SPI_DataWrite(wx);                           //
  SPI_CmdWrite(REG_DLHER1);                    //0x6d, Draw Line/Square/Triangle Point 2 X-coordinates Register1 (DLHER1)
  SPI_DataWrite(wx >> 8);                      //
  SPI_CmdWrite(REG_DLVER0);                    //0x6e, Draw Line/Square/Triangle Point 2 Y-coordinates Register0 (DLVER0)
  SPI_DataWrite(hy);                           //
  SPI_CmdWrite(REG_DLVER1);                    //0x6f, Draw Line/Square/Triangle Point 2 Y-coordinates Register1 (DLVER1)
  SPI_DataWrite(hy >> 8);                      //
}
void Panel_RA8889::Line_Point2XY(uint16_t wx, uint16_t hy) { Point2_XY(wx, hy); }


/**
 * @brief Triangle Point 1
 *
 * @verbatim
 *        REG [68h] Draw Line/Square/Triangle Point 1 X-coordinates Register0 (DLHSR0)
 *                  bit [7~0] Draw Line/Square/Triangle Start X-coordinates [7:0]
 *        REG [69h] Draw Line/Square/Triangle Point 1 X-coordinates Register1 (DLHSR1)
 *                  bit [7~0] Draw Line/Square/Triangle Start X-coordinate [12:8]
 *        REG [6Ah] Draw Line/Square/Triangle Point 1 Y-coordinates Register0 (DLVSR0)
 *                  bit [7~0] Draw Line/Square/Triangle Start Y-coordinate [7:0]
 *        REG [6Bh] Draw Line/Square/Triangle Point 1 Y-coordinates Register1 (DLVSR1)
 *                  bit [7~0] Draw Line/Square/Triangle Start Y-coordinate [12:8]
 *
 *        Please refer to the Canvas image coordinates.
 *        Unit: Pixel 
 *        ***Note: When draw a square, start point & end point cannot be 
 *           located at the same point or at the same X-axis or Y-axis.
 * @endverbatim
 *
 * @param wx: ponto de coordenada x
 *        hy: ponto de coordenada y
 *
 * @note None
 */ 
void Panel_RA8889::Triangle_Point1XY(uint16_t wx, uint16_t hy) { Point1_XY(wx, hy); }

/**
 * @brief Triangle Point 2
 *
 * @verbatim
 *        REG [6Ch] Draw Line/Square/Triangle Point 2 X-coordinates Register0 (DLHER0)
 *                  bit [7~0] Draw Line/Square/Triangle End X-coordinate [7:0]
 *        REG [6Dh] Draw Line/Square/Triangle Point 2 X-coordinates Register1 (DLHER1)
 *                  bit [7~0] Draw Line/Square/Triangle End X-coordinate [12:8]
 *        REG [6Eh] Draw Line/Square/Triangle Point 2 Y-coordinates Register0 (DLVER0)
 *                  bit [7~0] Draw Line/Square/Triangle End Y-coordinate [7:0]
 *        REG [6Fh] Draw Line/Square/Triangle Point 2 Y-coordinates Register1 (DLVER1)
 *                  bit [7~0] Draw Line/Square/Triangle End Y-coordinate [12:8]
 *
 *        Please refer to the Canvas image coordinates.
 *        Unit: Pixel 
 *        ***Note: When draw a square, start point & end point cannot be 
 *           located at the same point or at the same X-axis or Y-axis.
 * @endverbatim
 *
 * @param wx: ponto de coordenada x
 *        hy: ponto de coordenada y
 *
 * @note None
 */ 
void Panel_RA8889::Triangle_Point2XY(uint16_t wx, uint16_t hy)  { Point2_XY(wx, hy); }


/**
 * @brief Triangle Point 3
 *
 * @verbatim
 *        REG [70h] Draw Triangle Point 3 X-coordinates Register 0 (DTPH0)
 *                  bit [7~0] Draw Triangle Point 3 X-coordination [7:0]
 *        REG [71h] Draw Triangle Point 3 X-coordinates Register 1 (DTPH1)
 *                  bit [7~0] Draw Triangle Point 3 X-coordination [12:8]
 *        REG [72h] Draw Triangle Point 3 Y-coordinates Register 0 (DTPV0)
 *                  bit [7~0] Draw Triangle Point 3 Y-coordination [7:0]
 *        REG [73h] Draw Triangle Point 3 Y-coordinates Register 1 (DTPV1)
 *                  bit [7~0] Draw Triangle Point 3 Y-coordination [12:8]
 *
 *        Please refer to the Canvas image coordinates.
 *        Unit: Pixel
 * @endverbatim
 *
 * @param wx: ponto de coordenada x
 *        hy: ponto de coordenada y
 *
 * @note None
 */ 
void Panel_RA8889::Point3_XY(uint16_t wx, uint16_t hy)
{
  SPI_CmdWrite(REG_DTPH0);                     //0x70, Draw Triangle Point 3 X-coordinates Register 0 (DTPH0)
  SPI_DataWrite(wx);                           
  SPI_CmdWrite(REG_DTPH1);                     //0x71, Draw Triangle Point 3 X-coordinates Register 1 (DTPH1)
  SPI_DataWrite(wx >> 8);                      
  SPI_CmdWrite(REG_DTPV0);                     //0x72, Draw Triangle Point 3 Y-coordinates Register 0 (DTPV0)
  SPI_DataWrite(hy);                           
  SPI_CmdWrite(REG_DTPV1);                     //0x73, Draw Triangle Point 3 Y-coordinates Register 1 (DTPV1)
  SPI_DataWrite(hy >> 8);                      
}
void Panel_RA8889::Triangle_Point3XY(uint16_t wx, uint16_t hy) { Point3_XY(wx, hy); }


/**
 * @brief Square Start Point
 *
 * @verbatim
 *        REG [68h] Draw Line/Square/Triangle Point 1 X-coordinates Register0 (DLHSR0)
 *                  bit [7~0] Draw Line/Square/Triangle Start X-coordinates [7:0]
 *        REG [69h] Draw Line/Square/Triangle Point 1 X-coordinates Register1 (DLHSR1)
 *                  bit [7~0] Draw Line/Square/Triangle Start X-coordinate [12:8]
 *        REG [6Ah] Draw Line/Square/Triangle Point 1 Y-coordinates Register0 (DLVSR0)
 *                  bit [7~0] Draw Line/Square/Triangle Start Y-coordinate [7:0]
 *        REG [6Bh] Draw Line/Square/Triangle Point 1 Y-coordinates Register1 (DLVSR1)
 *                  bit [7~0] Draw Line/Square/Triangle Start Y-coordinate [12:8]
 *
 *        Please refer to the Canvas image coordinates.
 *        Unit: Pixel 
 *        ***Note: When draw a square, start point & end point cannot be 
 *           located at the same point or at the same X-axis or Y-axis.
 * @endverbatim
 *
 * @param wx: ponto de coordenada x
 *        hy: ponto de coordenada y
 *
 * @note None
 */ 
void Panel_RA8889::Square_Point1XY(uint16_t wx, uint16_t hy) { Point1_XY(wx, hy); }


/**
 * @brief Square End Point
 *
 * @verbatim
 *        REG [6Ch] Draw Line/Square/Triangle Point 2 X-coordinates Register0 (DLHER0)
 *                  bit [7~0] Draw Line/Square/Triangle End X-coordinate [7:0]
 *        REG [6Dh] Draw Line/Square/Triangle Point 2 X-coordinates Register1 (DLHER1)
 *                  bit [7~0] Draw Line/Square/Triangle End X-coordinate [12:8]
 *        REG [6Eh] Draw Line/Square/Triangle Point 2 Y-coordinates Register0 (DLVER0)
 *                  bit [7~0] Draw Line/Square/Triangle End Y-coordinate [7:0]
 *        REG [6Fh] Draw Line/Square/Triangle Point 2 Y-coordinates Register1 (DLVER1)
 *                  bit [7~0] Draw Line/Square/Triangle End Y-coordinate [12:8]
 *
 *        Please refer to the Canvas image coordinates.
 *        Unit: Pixel 
 *        ***Note: When draw a square, start point & end point cannot be 
 *           located at the same point or at the same X-axis or Y-axis.
 * @endverbatim
 *
 * @param wx: ponto de coordenada x
 *        hy: ponto de coordenada y
 *
 * @note None
 */
void Panel_RA8889::Square_Point2XY(uint16_t wx, uint16_t hy) { Point2_XY(wx, hy); }


//================================================================================
//
// [0x76] Draw Circle/Ellipse/Ellipse Curve/Circle Square Control Register 1 (DCR1)
//
//================================================================================


/**
 * @brief Ativa o Modo de desenho de Circulo / Elipse
 *        
 * @verbatim
 * REG[76h] Draw Circle/Ellipse/Ellipse Curve/Circle Square Control Register 1 (DCR1)
 *          bit [7] Draw Circle / Ellipse / Square /Circle Square Start Signal
 *                  Write Function:
 *                  0b0: Stop the drawing function.
 *                  0b1: Start the drawing function.
 *                  Read Function:
 *                  0b0: Drawing function complete.
 *                  0b1: Drawing function is processing.
 *          bit [6] Fill the Circle / Ellipse / Square / Circle Square Signal
 *                  0b0: Non fill.
 *                  0b1: fill.
 *          bit [5-4] Draw Circle / Ellipse / Square / Ellipse Curve / Circle Square Select
 *                  0b00: Draw Circle / Ellipse
 *                  0b01: Draw Circle / Ellipse Curve
 *                  0b10: Draw Square.
 *                  0b11: Draw Circle Square.
 *          bit [1-0] Draw Circle / Ellipse Curve Part Select(DECP)
 *                  0b00: bottom-left Ellipse Curve
 *                  0b01: upper-left Ellipse Curve
 *                  0b10: upper-right Ellipse Curve
 *                  0b11: bottom-right Ellipse Curve 
 * @endverbatim
 *
 * @param fill: true preenche figura, false não preenche figura
 *
 * @note None
 */
void Panel_RA8889::CircleMode_Start(bool fill)
{
  uint8_t temp;
  SPI_CmdWrite(REG_DCR1);                      //0x76, Draw Circle/Ellipse/Ellipse Curve/Circle Square Control Register 1 (DCR1)
  temp = SPI_DataRead();                       
  temp |= cSetb7;                              //Set bit 7, Start the drawing function
  temp &= cClrb5 & cClrb4;                     //Reset bit 5-4, Draw Circle / Ellipse
  if (fill) temp |= cSetrb6 else temp &= cClrb6 //Set bit 6 = Fill, Reset bit 6 = Non-Fill
  temp &= cClrb1 & cClrb0;                     //Reset bit 1-0, bottom-left Ellipse Curve
  SPI_DataWrite(temp);                         //0b1n00 xx00, n=0/1
  CoreTask_WaitReady();                        //Espere ate ficar pronto
}
void Panel_RA8889::EllipseMode_Start(bool fill) { CircleMode_Start(fill); }


/**
 * @brief Ativa o Modo de desenho de curva circular / eliptica 
 *        Quadrante Esquerda e Abaixo
 *
 * @verbatim
 * REG[76h] Draw Circle/Ellipse/Ellipse Curve/Circle Square Control Register 1 (DCR1)
 *          bit [7] Draw Circle / Ellipse / Square /Circle Square Start Signal
 *                  Write Function:
 *                  0b0: Stop the drawing function.
 *                  0b1: Start the drawing function.
 *                  Read Function:
 *                  0b0: Drawing function complete.
 *                  0b1: Drawing function is processing.
 *          bit [6] Fill the Circle / Ellipse / Square / Circle Square Signal
 *                  0b0: Non fill.
 *                  0b1: fill.
 *          bit [5-4] Draw Circle / Ellipse / Square / Ellipse Curve / Circle Square Select
 *                  0b00: Draw Circle / Ellipse
 *                  0b01: Draw Circle / Ellipse Curve
 *                  0b10: Draw Square.
 *                  0b11: Draw Circle Square.
 *          bit [1-0] Draw Circle / Ellipse Curve Part Select(DECP)
 *                  0b00: bottom-left Ellipse Curve
 *                  0b01: upper-left Ellipse Curve
 *                  0b10: upper-right Ellipse Curve
 *                  0b11: bottom-right Ellipse Curve 
 * @endverbatim
 *
 * @param fill: true preenche região da curva figura, false não preenche a região da curva
 *
 * @note None
 */
void Panel_RA8889::CurveLeftDownMode_Start(bool fill)
{
  uint8_t temp;
  SPI_CmdWrite(REG_DCR1);                      //0x76, Draw Circle/Ellipse/Ellipse Curve/Circle Square Control Register 1 (DCR1)
  temp = SPI_DataRead();                       
  temp |= cSetb7;                              //Set bit 7, Start the drawing function
  temp &= cClrb5;                              //Reset bit 5, Draw Circle / Ellipse Curve   
  if (fill) temp |= cSetrb6 else temp &= cClrb6 //Set bit 6 = Fill, Reset bit 6 = Non-Fill
  temp |= cSetb4;                              //Set bit 4, Draw Circle / Ellipse Curve
  temp &= cClrb1 & cClrb0;                     //Reset bit 1-0, bottom-left Ellipse Curve  
  SPI_DataWrite(temp);                         //0b1n01 xx00   n=1/0 
  CoreTask_WaitReady();                        //Espere ate ficar pronto
}


/**
 * @brief Ativa o Modo de desenho de curva circular / eliptica 
 *        Quadrante Esquerda e Acima
 *
 * @verbatim
 * REG[76h] Draw Circle/Ellipse/Ellipse Curve/Circle Square Control Register 1 (DCR1)
 *          bit [7] Draw Circle / Ellipse / Square /Circle Square Start Signal
 *                  Write Function:
 *                  0b0: Stop the drawing function.
 *                  0b1: Start the drawing function.
 *                  Read Function:
 *                  0b0: Drawing function complete.
 *                  0b1: Drawing function is processing.
 *          bit [6] Fill the Circle / Ellipse / Square / Circle Square Signal
 *                  0b0: Non fill.
 *                  0b1: fill.
 *          bit [5-4] Draw Circle / Ellipse / Square / Ellipse Curve / Circle Square Select
 *                  0b00: Draw Circle / Ellipse
 *                  0b01: Draw Circle / Ellipse Curve
 *                  0b10: Draw Square.
 *                  0b11: Draw Circle Square.
 *          bit [1-0] Draw Circle / Ellipse Curve Part Select(DECP)
 *                  0b00: bottom-left Ellipse Curve
 *                  0b01: upper-left Ellipse Curve
 *                  0b10: upper-right Ellipse Curve
 *                  0b11: bottom-right Ellipse Curve 
 * @endverbatim
 *
 * @param fill: true preenche região da curva figura, false não preenche a região da curva
 *
 * @note None
 */
void Panel_RA8889::CurveLeftUpMode_Start(bool fill)
{
  uint8_t temp;
  SPI_CmdWrite(REG_DCR1);                      //0x76, Draw Circle/Ellipse/Ellipse Curve/Circle Square Control Register 1 (DCR1)
  temp = SPI_DataRead();                       
  temp |= cSetb7;                              //Set bit 7, Start the drawing function
  if (fill) temp |= cSetrb6 else temp &= cClrb6 //Set bit 6 = Fill, Reset bit 6 = Non-Fill
  temp &= cClrb5;                              //Reset bit 5, Draw Circle / Ellipse Curve   
  temp |= cSetb4;                              //Set bit 4, Draw Circle / Ellipse Curve
  temp &= cClrb1                               //Reset bit 1, upper-left Ellipse Curve  
  temp |= cSetb0;                              //Set bit 0, upper-left Ellipse Curve 
  SPI_DataWrite(temp);                         //0b1n01 xx01   n=1/0
  CoreTask_WaitReady();                        //Espere ate ficar pronto
}


/**
 * @brief Ativa o Modo de desenho de curva circular / eliptica 
 *        Quadrante Direita e Acima
 *
 * @verbatim
 *        REG[76h] Draw Circle/Ellipse/Ellipse Curve/Circle Square Control Register 1 (DCR1)
 *                 bit [7] Draw Circle / Ellipse / Square /Circle Square Start Signal
 *                         Write Function:
 *                         0b0: Stop the drawing function.
 *                         0b1: Start the drawing function.
 *                         Read Function:
 *                         0b0: Drawing function complete.
 *                         0b1: Drawing function is processing.
 *                 bit [6] Fill the Circle / Ellipse / Square / Circle Square Signal
 *                         0b0: Non fill.
 *                         0b1: fill.
 *                 bit [5-4] Draw Circle / Ellipse / Square / Ellipse Curve / Circle Square Select
 *                         0b00: Draw Circle / Ellipse
 *                         0b01: Draw Circle / Ellipse Curve
 *                         0b10: Draw Square.
 *                         0b11: Draw Circle Square.
 *                 bit [1-0] Draw Circle / Ellipse Curve Part Select(DECP)
 *                         0b00: bottom-left Ellipse Curve
 *                         0b01: upper-left Ellipse Curve
 *                         0b10: upper-right Ellipse Curve
 *                         0b11: bottom-right Ellipse Curve 
 * @endverbatim
 *
 * @param fill: true preenche região da curva figura, false não preenche a região da curva
 *
 * @note None
 */
void Panel_RA8889::CurveRightUpMode_Start(bool fill)
{
  uint8_t temp;
  SPI_CmdWrite(REG_DCR1);                      //0x76, Draw Circle/Ellipse/Ellipse Curve/Circle Square Control Register 1 (DCR1)
  temp = SPI_DataRead(); 
  temp |= cSetb7;                              //Set bit 7, Start the drawing function
  if (fill) temp |= cSetrb6 else temp &= cClrb6 //Set bit 6 = Fill, Reset bit 6 = Non-Fill
  temp &= cClrb5;                              //Reset bit 5, Draw Circle / Ellipse Curve   
  temp |= cSetb4;                              //Set bit 4, Draw Circle / Ellipse Curve
  temp &= cSetb1                               //Set bit 1, upper-right Ellipse Curve
  temp |= cClrb0;                              //Reset bit 0, upper-right Ellipse Curve
  SPI_DataWrite(temp);                         //0b1n01 xx10   n=1/0
  CoreTask_WaitReady();                        //Espere ate ficar pronto
}


/**
 * @brief Ativa o Modo de desenho de curva circular / eliptica 
 *        Quadrante Esquerda e Abaixo
 *
 * @verbatim
 *        REG[76h] Draw Circle/Ellipse/Ellipse Curve/Circle Square Control Register 1 (DCR1)
 *                 bit [7] Draw Circle / Ellipse / Square /Circle Square Start Signal
 *                         Write Function:
 *                         0b0: Stop the drawing function.
 *                         0b1: Start the drawing function.
 *                         Read Function:
 *                         0b0: Drawing function complete.
 *                         0b1: Drawing function is processing.
 *                 bit [6] Fill the Circle / Ellipse / Square / Circle Square Signal
 *                         0b0: Non fill.
 *                         0b1: fill.
 *                 bit [5-4] Draw Circle / Ellipse / Square / Ellipse Curve / Circle Square Select
 *                         0b00: Draw Circle / Ellipse
 *                         0b01: Draw Circle / Ellipse Curve
 *                         0b10: Draw Square.
 *                         0b11: Draw Circle Square.
 *                 bit [1-0] Draw Circle / Ellipse Curve Part Select(DECP)
 *                         0b00: bottom-left Ellipse Curve
 *                         0b01: upper-left Ellipse Curve
 *                         0b10: upper-right Ellipse Curve
 *                         0b11: bottom-right Ellipse Curve 
 * @endverbatim
 *
 * @param fill: true preenche região da curva figura, false não preenche a região da curva
 *
 * @note None
 */
void Panel_RA8889::CurveRightDownMode_Start(bool fill)
{
  uint8_t temp;
  SPI_CmdWrite(REG_DCR1);                      //0x76, Draw Circle/Ellipse/Ellipse Curve/Circle Square Control Register 1 (DCR1)
  temp = SPI_DataRead();  
  temp |= cSetb7;                              //Set bit 7, Start the drawing function
  if (fill) temp |= cSetrb6 else temp &= cClrb6 //Set bit 6 = Fill, Reset bit 6 = Non-Fill
  temp &= cClrb5;                              //Reset bit 5, Draw Circle / Ellipse Curve   
  temp |= cSetb4;                              //Set bit 4, Draw Circle / Ellipse Curve
  temp &= cSetb1                               //Set bit 1, bottom-right Ellipse Curve
  temp |= cSetb0;                              //Set bit 0, bottom-right Ellipse Curve
  SPI_DataWrite(temp);                         //0b1n01 xx11   n=1/0
  CoreTask_WaitReady();                        //Espere ate ficar pronto
}


/**
 * @brief Ativa o Modo de desenho de quadrado
 *
 * @verbatim
 *        REG[76h] Draw Circle/Ellipse/Ellipse Curve/Circle Square Control Register 1 (DCR1)
 *                 bit [7] Draw Circle / Ellipse / Square /Circle Square Start Signal
 *                         Write Function:
 *                         0b0: Stop the drawing function.
 *                         0b1: Start the drawing function.
 *                         Read Function:
 *                         0b0: Drawing function complete.
 *                         0b1: Drawing function is processing.
 *                 bit [6] Fill the Circle / Ellipse / Square / Circle Square Signal
 *                         0b0: Non fill.
 *                         0b1: fill.
 *                 bit [5-4] Draw Circle / Ellipse / Square / Ellipse Curve / Circle Square Select
 *                         0b00: Draw Circle / Ellipse
 *                         0b01: Draw Circle / Ellipse Curve
 *                         0b10: Draw Square.
 *                         0b11: Draw Circle Square.
 *                 bit [1-0] Draw Circle / Ellipse Curve Part Select(DECP)
 *                         0b00: bottom-left Ellipse Curve
 *                         0b01: upper-left Ellipse Curve
 *                         0b10: upper-right Ellipse Curve
 *                         0b11: bottom-right Ellipse Curve 
 * @endverbatim
 *
 * @param fill: true preenche região da curva figura, false não preenche a região da curva
 *
 * @note None
 */
void Panel_RA8889::SquareMode_Start(bool fill)
{
  uint8_t temp;
  SPI_CmdWrite(REG_DCR1);                      //0x76, Draw Circle/Ellipse/Ellipse Curve/Circle Square Control Register 1 (DCR1)
  temp = SPI_DataRead();  
  temp |= cSetb7;                              //Set bit 7, Start the drawing function
  if (fill) temp |= cSetrb6 else temp &= cClrb6 //Set bit 6 = Fill, Reset bit 6 = Non-Fill
  temp |= cSetb5;                              //Set bit 5, Draw Square.
  temp &= cClrb4;                              //Reset bit 4, Draw Square.
  SPI_DataWrite(temp);                         //0b1n10 xxxx   n=1/0
  CoreTask_WaitReady();                        //Espere ate ficar pronto
}


/**
 * @brief Ativa o Modo de desenho de curva circular no canto quadrado
 *        
 * @verbatim  
 * REG[76h] Draw Circle/Ellipse/Ellipse Curve/Circle Square Control Register 1 (DCR1)
 *          bit [7] Draw Circle / Ellipse / Square /Circle Square Start Signal
 *                  Write Function:
 *                  0b0: Stop the drawing function.
 *                  0b1: Start the drawing function.
 *                  Read Function:
 *                  0b0: Drawing function complete.
 *                  0b1: Drawing function is processing.
 *          bit [6] Fill the Circle / Ellipse / Square / Circle Square Signal
 *                  0b0: Non fill.
 *                  0b1: fill.
 *          bit [5-4] Draw Circle / Ellipse / Square / Ellipse Curve / Circle Square Select
 *                  0b00: Draw Circle / Ellipse
 *                  0b01: Draw Circle / Ellipse Curve
 *                  0b10: Draw Square.
 *                  0b11: Draw Circle Square.
 *          bit [1-0] Draw Circle / Ellipse Curve Part Select(DECP)
 *                  0b00: bottom-left Ellipse Curve
 *                  0b01: upper-left Ellipse Curve
 *                  0b10: upper-right Ellipse Curve
 *                  0b11: bottom-right Ellipse Curve 
 * @endverbatim  
 *
 * @param fill: true preenche região da curva figura, false não preenche a região da curva
 *
 * @note None
 */
void Panel_RA8889::CircleSquareMode_Start(bool fill)
{
  uint8_t temp;
  SPI_CmdWrite(REG_DCR1);                      //0x76, Draw Circle/Ellipse/Ellipse Curve/Circle Square Control Register 1 (DCR1)
  temp = SPI_DataRead();  
  temp |= cSetb7;                              //Set bit 7, Start the drawing function
  fill ? temp |= cSetrb6 : temp &= cClrb6;     //Set bit 6 = Fill, Reset bit 6 = Non-Fill
  temp |= cSetb5;                              //Set bit 5, Draw Circle Square
  temp |= cSetb4;                              //Set bit 4, Draw Circle Square
  SPI_DataWrite(temp);                         //0b1n11 xxxx   n=1/0
  CoreTask_WaitReady();                        //Espere ate ficar pronto
}


//================================================================================
//
// [0x77] Draw Circle/Ellipse/Circle Square Major radius Setting Register (ELL_A0)
// [0x78] Draw Circle/Ellipse/Circle Square Major radius Setting Register (ELL_A1)
// [0x79] Draw Circle/Ellipse/Circle Square Minor radius Setting Register (ELL_B0)
// [0x7a] Draw Circle/Ellipse/Circle Square Minor radius Setting Register (ELL_B1)
// [0x7b] Draw Circle/Ellipse/Circle Square Center X-coordinates Register0 (DEHR0)
// [0x7c] Draw Circle/Ellipse/Circle Square Center X-coordinates Register1 (DEHR1)
// [0x7d] Draw Circle/Ellipse/Circle Square Center Y-coordinates Register0 (DEVR0)
// [0x7e] Draw Circle/Ellipse/Circle Square Center Y-coordinates Register1 (DEVR1)
//
//================================================================================


/**
 * @brief Raio do círculo                           Rx = Ry
 *        Raio da elipse                            Rx > Ry or Rx < Ry
 *        Raio da curva circular no canto quadrado  Rx = Ry, Rx > Ry or Rx < Ry
 *
 * @verbatim
 * REG[0x77] Draw Circle/Ellipse/Circle Square Major radius Setting Register (ELL_A0)
 *           bit [7~0] Draw Circle/Ellipse/Circle Square Major radius [7:0]
 * REG[0x78] Draw Circle/Ellipse/Circle Square Major radius Setting Register (ELL_A1)
 *           bit [4~0] Draw Circle/Ellipse/Circle Square Major radius [12:8]
 *
 * Unit: Pixel
 * To draw a circle needs to set major axis equal to minor radius.
 * 
 * REG[0x79] Draw Circle/Ellipse/Circle Square Minor radius Setting Register (ELL_B0)
 *           bit [7~0] Draw Circle/Ellipse/Circle Square Minor radius [7:0]
 * REG[0x7a] Draw Circle/Ellipse/Circle Square Minor radius Setting Register (ELL_B1)
 *           bit [4~0] Draw Circle/Ellipse/Circle Square Minor radius [12:8]
 *
 * Unit: Pixel
 * To draw a circle needs to set major axis equal to minor radius.
 * @endverbatim
 *
 * @param radius: raio do circulo
 *
 * @note No criculo o R = Rx = Ry e na elipse Rx > Ry ou Rx < Ry
 */
void Panel_RA8889::Radius_RxRy(uint16_t Rx, uint16_t Ry)
{
  SPI_CmdWrite(REG_ELL_A0);                    //0x77, Draw Circle/Ellipse/Circle Square Major radius Setting Register (ELL_A0)
  SPI_DataWrite(Rx);                           //
  SPI_CmdWrite(REG_ELL_A1);                    //0x78, Draw Circle/Ellipse/Circle Square Major radius Setting Register (ELL_A1)
  SPI_DataWrite(Rx >> 8);                      //
  
  SPI_CmdWrite(REG_ELL_B0);                    //0x79, Draw Circle/Ellipse/Circle Square Minor radius Setting Register (ELL_B0)
  SPI_DataWrite(Ry);                           //
  SPI_CmdWrite(REG_ELL_B1);                    //0x7a, Draw Circle/Ellipse/Circle Square Minor radius Setting Register (ELL_B1)
  SPI_DataWrite(Ry >> 8);                      //
}
void Panel_RA8889::CircleRadius_R(uint16_t R) { Radius_RxRy(R, R); }
void Panel_RA8889::EllipseRadius_RxRy(uint16_t Rx, uint16_t Ry) { Radius_RxRy(Rx, Ry); }
void Panel_RA8889::CircleSquareRadius_RxRy(uint16_t Rx, uint16_t Ry) { Radius_RxRy(Rx, Ry); }


/**
 * @brief Posição do Centro do Círculo/Elipse/Circulo do quadrado
 *
 * @verbatim
 *        REG[0x7b] Draw Circle/Ellipse/Circle Square Center X-coordinates Register0 (DEHR0)
 *                  bit [7~0] Draw Circle/Ellipse/Circle Square Center X-coordinates [7:0]
 *        REG[0x7c] Draw Circle/Ellipse/Circle Square Center X-coordinates Register1 (DEHR1)
 *                  bit [4~0] Draw Circle/Ellipse/Circle Square Center X-coordinates [12:8]
 *        REG[0x7d] Draw Circle/Ellipse/Circle Square Center Y-coordinates Register0 (DEVR0)
 *                  bit [7~0] Draw Circle/Ellipse/Circle Square Center Y-coordinates [7:0]
 *        REG[0x7e] Draw Circle/Ellipse/Circle Square Center Y-coordinates Register1 (DEVR1)
 *                  bit [4~0] Draw Circle/Ellipse/Circle Square Center Y-coordinates [12:8]
 *
 *        Please refer to the Canvas image coordinates.
 *        Unit: Pixel
 * @endverbatim
 *
 * @param Wx, Hy: coordenada central (x,y)
 *
 * @note 
 */
 void Panel_RA8889::Center_XY(uint16_t Wx, uint16_t Hy)
 {
  SPI_CmdWrite(REG_DEHR0);      //0x7b, Draw Circle/Ellipse/Circle Square Center X-coordinates Register0 (DEHR0)
  SPI_DataWrite(Wx);            //
  SPI_CmdWrite(REG_DEHR1);      //0x7c, Draw Circle/Ellipse/Circle Square Center X-coordinates Register0 (DEHR1)
  SPI_DataWrite(Wx >> 8);       //
						   
  SPI_CmdWrite(REG_DEVR0);      //0x7d, Draw Circle/Ellipse/Circle Square Center Y-coordinates Register0 (DEVR0)
  SPI_DataWrite(Hy);            //
  SPI_CmdWrite(REG_DEVR1);      //0x7e, Draw Circle/Ellipse/Circle Square Center Y-coordinates Register0 (DEVR1)
  SPI_DataWrite(Hy >> 8);       //
 }
void Panel_RA8889::CircleCenter_XY(uint16_t Wx, uint16_t Hy) {Center_XY(Wx, Hy);}
void Panel_RA8889::EllipseCenter_XY(uint16_t Wx, uint16_t Hy) {Center_XY(Wx, Hy);}


//================================================================================
//
// [0x84] PWM Prescaler Register (PSCLR)
//
//================================================================================


/**
 * @brief PWM Prescaler 1 to 256
 *
 * @verbatim
 * REG[0x84] PWM Prescaler Register (PSCLR)
 *           bit [7~0] PWM Prescaler Register
 *           These 8 bits determine prescaler value for Timer 0 and 1.
 *           Time base is “Core_Freq / (Prescaler + 1)”
 *           Fcore: Core Frequency
 * @endverbatim
 *
 * @param Wx, Hy: coordenada central (x,y)
 *
 * @note 
 */
 void Panel_RA8889::PWM_Prescaler(uint8_t prescaler)
{
  prescaler = prescaler - 1;
  SPI_CmdWrite(REG_PSCLR);   //0x84, PWM Prescaler Register (PSCLR)
  SPI_DataWrite(prescaler);
}


//================================================================================
//
// [0x85] PWM clock Mux Register (PMUXR)
//
//================================================================================


/**
 * @brief Select MUX input for PWM Timer 1.
 *
 * @verbatim
 * REG [0x85] PWM clock Mux Register (PMUXR)
 *           bit [7-6] Select 2nd clock divider’s MUX input for PWM Timer 1
 *                     0b00 = 1
 *                     0b01 = 1/2
 *                     0b10 = 1/4
 *                     0b11 = 1/8
 * @endverbatim
 *
 * @param eDividerClock::X1
 *        eDividerClock::X2
 *        eDividerClock::X4
 *        eDividerClock::X8
 *
 * @note Selecione a entrada MUX do 2º divisor de clock para o PWM Timer 1.
 */
void Panel_RA8889::PWM1_ClockDividedBy(eDividerClock divider)
{
  uint8_t temp;
  SPI_CmdWrite(REG_PMUXR);                     //0x85, PWM clock Mux Register (PMUXR)
  temp = SPI_DataRead();                       
  temp &= ~(cSetb7 | cSetb6);                  //Rest bit 7 and 6
  temp |=  static_cast<uint8_t>(divider);
  SPI_DataWrite(temp);
}


/**
 * @brief Select MUX input for PWM Timer 0.
 *
 * @verbatim
 * REG [0x85] PWM clock Mux Register (PMUXR)
 *           bit [5-4] Select 2nd clock divider’s MUX input for PWM Timer 0
 *                     0b00 = 1/1
 *                     0b01 = 1/2
 *                     0b10 = 1/4
 *                     0b11 = 1/8
 * @endverbatim
 *
 * @param eDividerClock::X1
 *        eDividerClock::X2
 *        eDividerClock::X4
 *        eDividerClock::X8
 *
 * @note Selecione a entrada MUX do 2º divisor de clock para o PWM Timer 0.
 */
void Panel_RA8889::PWM0_ClockDividedBy(eDividerClock divider);
{
  uint8_t temp;
  SPI_CmdWrite(REG_PMUXR);                     //0x85, PWM clock Mux Register (PMUXR)
  temp = SPI_DataRead();                       
  temp &= ~(cSetb5 | cSetb4);                  //Reset bit 5 and 4
  temp |=  static_cast<uint8_t>(divider);
  SPI_DataWrite(temp);
}


/**
 * @brief Seleciona o pino XPWM[1] para atuar como saída do sinal de indicação de erro.
 *
 *        Esta função ajusta o registrador PMUXR (0x85), bits [3-2], de modo que o pino XPWM[1]
 *        seja direcionado para indicar condições de erro detectadas pelo RA8889,
 *        como insuficiência de dados no FIFO de largura de banda de varredura
 *        ou acesso de memória fora do intervalo permitido (REG[00h] bit[1:0]).
 *        
 *        Ao selecionar esta opção, o XPWM[1] deixa de funcionar como saída de PWM/clock
 *        e passa a refletir diretamente o estado do "System Error Flag".
 *
 * @verbatim
 * REG [0x85] PWM clock Mux Register (PMUXR)
 *            bit [3-2] XPWM[1] pin function control
 *                      0b0x: XPWM[1] output system error flag (REG[00h] bit[1:0], Scan bandwidth FIFO insufficient pop error or Memory access out of range)
 *                      0b10: XPWM[1] output PWM timer 1 event or invert of PWM timer 0
 *                      0b11: XPWM[1] output oscillator clock
 *            If XTEST[0] set high, then XPWM[1] will become panel scan clock input.
 * @endverbatim
 *
 * @param None
 *
 * @note Se o bit XTEST[0] estiver configurado como alto, o pino XPWM[1] é forçado
 *       a atuar como entrada de clock de varredura do painel, independentemente desta configuração.
 */
void Panel_RA8889::PWM1_Select_ErrorFlag(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_PMUXR);                     //0x85, PWM clock Mux Register (PMUXR)
  temp = SPI_DataRead();                       
  temp &= cClrb3;                              //Reset bit 3
  SPI_DataWrite(temp);
}


/**
 * @brief Select Output Oscillator PWM Timer 1
 *
 * @verbatim
 * REG [0x85] PWM clock Mux Register (PMUXR)
 *            bit [3-2] XPWM[1] pin function control
 *                      0b0x: XPWM[1] output system error flag (REG[00h] bit[1:0], Scan bandwidth FIFO insufficient pop error or Memory access out of range)
 *                      0b10: XPWM[1] output PWM timer 1 event or invert of PWM timer 0
 *                      0b11: XPWM[1] output oscillator clock
 *            If XTEST[0] set high, then XPWM[1] will become panel scan clock input.
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::PWM1_Select(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_PMUXR);                     //0x85, PWM clock Mux Register (PMUXR)
  temp = SPI_DataRead();                       
  temp |= cSetb3;
  temp &= cClrb2;
  SPI_DataWrite(temp);
}


/**
 * @brief Seleciona o pino XPWM[1] para atuar como uma saída do clock do oscilador interno.
 *
 * @verbatim
 * REG [0x85] PWM clock Mux Register (PMUXR)
 *            bit [3-2] XPWM[1] pin function control
 *                      0b0x: XPWM[1] output system error flag (REG[00h] bit[1:0], Scan bandwidth FIFO insufficient pop error or Memory access out of range)
 *                      0b10: XPWM[1] output PWM timer 1 event or invert of PWM timer 0
 *                      0b11: XPWM[1] output oscillator clock
 *            If XTEST[0] set high, then XPWM[1] will become panel scan clock input.
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::PWM1_Select_OscillatorClock(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_PMUXR);                     //0x85, PWM clock Mux Register (PMUXR)
  temp = SPI_DataRead();                       
  temp |= cSetb3;
  temp |= cSetb2;
  SPI_DataWrite(temp);
}


/**
 * @brief Seleciona o pino XPWM[0] para atuar como um GPIO-C[7].
 *        
 *        Ajusta o registrador PMUXR (0x85), bits [1-0], para multiplexar o pino XPWM[0]
 *        como um pino de propósito geral (GPIO) ao invés de saída PWM ou clock.
 *
 *        Esse modo é utilizado quando você precisa controlar XPWM[0] manualmente
 *        como pino digital, por exemplo para sinais de controle ou I/O externo.
 *
 * @verbatim
 * REG [0x85] PWM clock Mux Register (PMUXR)
 *            bit [1-0] XPWM[0] pin function control
 *                      0b0x: XPWM[0] becomes GPIO-C[7]
 *                      0b10: XPWM[0] output PWM timer 0, enabled and controlled by PWM timer 0
 *                      0b11: XPWM[0] output core clock
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::PWM0_Select_GPIOC7(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_PMUXR);                     //0x85, PWM clock Mux Register (PMUXR)
  temp = SPI_DataRead();                       
  temp &= cClrb1;
  SPI_DataWrite(temp);
}


/**
 * @brief Select Output Oscillator PWM Timer 0
 *
 * @verbatim
 * REG [0x85] PWM clock Mux Register (PMUXR)
 *            bit [1-0] XPWM[0] pin function control
 *                      0b0x: XPWM[0] becomes GPIO-C[7]
 *                      0b10: XPWM[0] output PWM timer 0, enabled and controlled by PWM timer 0
 *                      0b11: XPWM[0] output core clock
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::PWM0_Select(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_PMUXR);                     //0x85, PWM clock Mux Register (PMUXR)
  temp = SPI_DataRead();                       
  temp |= cSetb1;
  temp &= cClrb0;
  SPI_DataWrite(temp);
}


/**
 * @brief Seleciona o pino XPWM[0] para atuar como uma saída do clock interno do núcleo (core clock).
 *
 *        Ajusta o registrador PMUXR (0x85), bits [1-0], para que o pino XPWM[0] seja
 *        multiplexado MUX como saída do sinal de clock principal do RA8889, em vez de
 *        funcionar como GPIO-C[7] ou como saída do PWM Timer 0.
 *
 *        Esse modo é normalmente utilizado para depuração ou como referência de clock
 *        para dispositivos externos. 
 *
 * @verbatim
 * REG [0x85] PWM clock Mux Register (PMUXR)
 *            bit [1-0] XPWM[0] pin function control
 *                      0b0x: XPWM[0] becomes GPIO-C[7]
 *                      0b10: XPWM[0] output PWM timer 0, enabled and controlled by PWM timer 0
 *                      0b11: XPWM[0] output core clock
  * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::PWM0_Select_CoreClock(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_PMUXR);                     //0x85, PWM clock Mux Register (PMUXR)
  temp = SPI_DataRead();                       
  temp |= cSetb1;
  temp |= cSetb0;
  SPI_DataWrite(temp);
}


//================================================================================
//
// [0x86] PWM Configuration Register (PCFGR)
//
//================================================================================


/**
 * @brief Ativa ou desativa o inversor de saída do PWM Timer 1 (XPWM[1]).
 *
 * @verbatim
 * Ajusta o registrador PCFGR (0x86), bit 6, para definir se a saída do PWM1
 * será invertida ou não.  
 *
 *
 * REG [0x86] PWM Configuration Register (PCFGR)
 *            bit [6] PWM Timer 1 output inverter on/off
 *            Determine the output inverter on/off for Timer 1.        
 *                    0b0: Inverter off
 *                    0b1: Inverter on for PWM1
 * @endverbatim
 *
 * @param on
 *        true  - Ativa o inversor (PWM1 invertido)
 *        false - Desativa o inversor (PWM1 normal)
 * @note O inversor afeta apenas a forma de onda do PWM Timer 1 na saída XPWM[1].
 */
void Panel_RA8889::PWM1_InverterOn(boolean on)
{
  uint8_t temp;
  SPI_CmdWrite(REG_PCFGR);                     //0x86, PWM Configuration Register (PCFGR)
  temp = SPI_DataRead();                       
  on ? temp |= cSetb6 : temp &= cClrb6;
  SPI_DataWrite(temp);
}


/**
 * @brief Seleciona o modo Auto Reload PWM Timer 1
 *
 * @verbatim
 * REG [0x86] PWM Configuration Register (PCFGR)
 *            bit [5] PWM Timer 1 auto reload on/off
 *            Determine auto reload on/off for Timer 1.        
 *                    0b0: One-shot
 *                    0b1: Interval mode(auto reload) (default)
 * @endverbatim
 *
 * @param None
 * @note None
 */
void Panel_RA8889::PWM1_Select_AutoReload(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_PCFGR);                     //0x86, PWM Configuration Register (PCFGR)
  temp = SPI_DataRead();                       
  temp |= cSetb5;
  SPI_DataWrite(temp);
}


/**
 * @brief Seleciona o modo One Shot PWM Timer 1
 *
 * @verbatim
 * REG [0x86] PWM Configuration Register (PCFGR)
 *            bit [5] PWM Timer 1 auto reload on/off
 *            Determine auto reload on/off for Timer 1.
 *                    0b0: One-shot
 *                    0b1: Interval mode(auto reload) (default)
 * @endverbatim
 *
 * @param None
 * @note None
 */
void Panel_RA8889::PWM1_Select_OneShot(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_PCFGR);                     //0x86, PWM Configuration Register (PCFGR)
  temp = SPI_DataRead();                       
  temp &= cClrb5;
  SPI_DataWrite(temp);
}


/**
 * @brief Inicia o PWM Timer 1 (XPWM[1]).
 *
 *        Ajusta o registrador PCFGR (0x86), bit 4, para iniciar o PWM Timer 1.
 *        Em modo Interval (auto reload), o timer continuará rodando até que o bit seja zerado.
 *        Em modo One-shot, o bit é limpo automaticamente ao final do ciclo.
 *
 *@verbatim
 * REG [0x86] PWM Configuration Register (PCFGR)
 *            bit [4] PWM Timer 1 start/stop
 *            Determine start/stop for Timer 1.        
 *                    0b0: Stop
 *                    0b1: Start for timer 1
 *            In Interval mode (auto reload), user needs to set this bit to 0 to stop PWM timer.
 *            In One-shot, this bit will auto clear.
 *            User may read this bit to know the current PWMx is running or stopped.
 * @endverbatim
 *
 * @param None
 * @note É possível ler o mesmo bit para verificar se o Timer 1 está ativo ou parado.
 */
void Panel_RA8889::PWM1_StartTimer(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_PCFGR);                     //0x86, PWM Configuration Register (PCFGR)
  temp = SPI_DataRead();                       
  temp |= cSetb4;
  SPI_DataWrite(temp);
}


/**
 * @brief Para o PWM Timer 1 (XPWM[1]).
 *
 *        Ajusta o registrador PCFGR (0x86), bit 4, para parar o PWM Timer 1.
 *        Em modo Interval (auto reload), o timer será interrompido até que o bit seja novamente setado.  
 *        Em modo One-shot, o bit se limpa automaticamente ao final do ciclo.  
 *
 *@verbatim
 * REG [0x86] PWM Configuration Register (PCFGR)
 *            bit [4] PWM Timer 1 start/stop
 *            Determine start/stop for Timer 1.        
 *                    0b0: Stop
 *                    0b1: Start for timer 1
 *            In Interval mode (auto reload), user needs to set this bit to 0 to stop PWM timer.
 *            In One-shot, this bit will auto clear.
 *            User may read this bit to know the current PWMx is running or stopped.
 * @endverbatim
 *
 * @param None
 * @note É possível ler o mesmo bit para verificar se o Timer 1 está ativo ou parado.
 */
void Panel_RA8889::PWM1_StopTimer(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_PCFGR);                     //0x86, PWM Configuration Register (PCFGR)
  temp = SPI_DataRead();                       
  temp &= cClrb4;
  SPI_DataWrite(temp);
}


/**
 * @brief Habilita ou desabilita a Dead Zone do PWM Timer 0.
 *
 *        Ajusta o registrador PCFGR (0x86), bit 3, para ativar ou desativar
 *        o intervalo de Dead Zone no PWM Timer 0.
 *
 * @verbatim
 * REG [0x86] PWM Configuration Register (PCFGR)
 *            bit [3] PWM Timer 0 Dead zone enable
 *            Determine the dead zone operation.       
 *                    0b0: Disable
 *                    0b1: Enable
 * @endverbatim
 *
 * @param None
 * @note Quando habilitado, há um curto período em que ambos os sinais complementares
 *       do PWM ficam desligados, prevenindo condução simultânea em H-bridges ou MOSFETs.
 */
void Panel_RA8889::PWM0_DeadZoneEnable(bool b)
{
  uint8_t temp;
  SPI_CmdWrite(REG_PCFGR);                     //0x86, PWM Configuration Register (PCFGR)
  temp = SPI_DataRead();                       
  b ? temp |= cSetb3 : temp &= cClrb3;
  SPI_DataWrite(temp);
}


/**
 * @brief Ativa ou desativa o inversor de saída do PWM Timer 0 (XPWM[0]).
 *
 *        Ajusta o registrador PCFGR (0x86), bit 2, para definir se a saída do PWM0
 *        será invertida ou não.
 *
 * @verbatim
 * REG [0x86] PWM Configuration Register (PCFGR)
 *            bit [2] PWM Timer 0 output inverter on/off
 *            Determine the output inverter on/off for Timer 0.        
 *                    0b0: Inverter off
 *                    0b1: Inverter on for PWM0
 * @endverbatim
 *
 * @param on
 *        true  - Ativa o inversor (PWM1 invertido)
 *        false - Desativa o inversor (PWM1 normal)
 * @note O inversor afeta apenas a forma de onda do PWM Timer 0 na saída XPWM[0].
 */
void Panel_RA8889::PWM0_InverterOn(bool on)
{
  uint8_t temp;
  SPI_CmdWrite(REG_PCFGR);                     //0x86, PWM Configuration Register (PCFGR)
  temp = SPI_DataRead();                       
  b ? temp |= cSetb2 : temp &= cClrb2;
  SPI_DataWrite(temp);
}


/**
 * @brief Seleciona o modo Auto Reload PWM Timer 0
 *
 * @verbatim
 * REG [0x86] PWM Configuration Register (PCFGR)
 *            bit [1] PWM Timer 0 auto reload on/off
 *            Determine auto reload on/off for Timer 0.        
 *                    0b0: One-shot
 *                    0b1: Interval mode(auto reload) (default)
 * @endverbatim
 *
 * @param None
 * @note None
 */
void Panel_RA8889::PWM0_Select_AutoReload(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_PCFGR);                     //0x86, PWM Configuration Register (PCFGR)
  temp = SPI_DataRead();                       
  temp |= cSetb1;
  SPI_DataWrite(temp);
}


/**
 * @brief Seleciona o modo One Shot PWM Timer 0
 *
 * @verbatim
 * REG [0x86] PWM Configuration Register (PCFGR)
 *            bit [1] PWM Timer 0 auto reload on/off
 *            Determine auto reload on/off for Timer 0.
 *                    0b0: One-shot
 *                    0b1: Interval mode(auto reload) (default)
 * @endverbatim
 *
 * @param None
 * @note None
 */
void Panel_RA8889::PWM0_Select_OneShot(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_PCFGR);                     //0x86, PWM Configuration Register (PCFGR)
  temp = SPI_DataRead();                       
  temp &= cClrb1;
  SPI_DataWrite(temp);
}


/**
 * @brief Inicia o PWM Timer 0 (XPWM[0]).
 *
 *        Configura o bit [0] do registrador PCFGR (0x86) para "1",
 *        habilitando o funcionamento do PWM Timer 0.  
 *        O Timer 0 pode ser roteado para o pino XPWM[0] (dependendo da configuração
 *        do registrador PMUXR) e passa a gerar o sinal PWM de acordo com os
 *        parâmetros previamente configurados (período, duty cycle, etc.).
 *
 *@verbatim
 * REG [0x86] PWM Configuration Register (PCFGR)
 *            bit [0] PWM Timer 0 start/stop
 *            Determine start/stop for Timer 0.
 *                    0b0: Stop
 *                    0b1: Start for timer 1
 *            In Interval mode (auto reload), user needs to set this bit to 0 to stop PWM timer.
 *            In One-shot, this bit will auto clear.
 *            User may read this bit to know the current PWMx is running or stopped.
 * @endverbatim
 *
 * @param None
 * @note Esta função apenas inicia o Timer; para realmente gerar sinal PWM,
 *       é necessário que os registradores de configuração de frequência,
 *       duty cycle e roteamento da saída já estejam ajustados.
 */
void Panel_RA8889::PWM0_StartTimer(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_PCFGR);                     //0x86, PWM Configuration Register (PCFGR)
  temp = SPI_DataRead();                       
  temp |= cSetb0;
  SPI_DataWrite(temp);
}


/**
 * @brief Para o PWM Timer 0 (XPWM[0]).
 *
 *        Configura o bit [0] do registrador PCFGR (0x86) para "0",
 *        habilitando o funcionamento do PWM Timer 0.  
 *        O Timer 0 pode ser roteado para o pino XPWM[0] (dependendo da configuração
 *        do registrador PMUXR) e passa a gerar o sinal PWM de acordo com os
 *        parâmetros previamente configurados (período, duty cycle, etc.).
 *
 *@verbatim
 * REG [0x86] PWM Configuration Register (PCFGR)
 *            bit [0] PWM Timer 0 start/stop
 *            Determine start/stop for Timer 0.
 *                    0b0: Stop
 *                    0b1: Start for timer 1
 *            In Interval mode (auto reload), user needs to set this bit to 0 to stop PWM timer.
 *            In One-shot, this bit will auto clear.
 *            User may read this bit to know the current PWMx is running or stopped.
 * @endverbatim
 *
 * @param None
 * @note Esta função apenas inicia o Timer; para realmente gerar sinal PWM,
 *       é necessário que os registradores de configuração de frequência,
 *       duty cycle e roteamento da saída já estejam ajustados.
 */
void Panel_RA8889::PWM0_StopTimer(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_PCFGR);                     //0x86, PWM Configuration Register (PCFGR)
  temp = SPI_DataRead();                       
  temp &= cClrb0;
  SPI_DataWrite(temp);
}


//================================================================================
//
// [0x87] Timer 0 Dead zone length register [DZ_LENGTH]
//
//================================================================================


/**
 * @brief Configura o tempo morto (Dead Zone) do PWM Timer 0.
 *
 *@verbatim
 * REG [0x87] Timer 0 Dead zone length register [DZ_LENGTH]
 *            bit [7-0] Timer 0 Dead zone length register
 *            This byte is used to determine the dead zone length. The 1 unit 
 *            time of the dead zone length is equal to the whole cycle of 
 *            timer 0.
 * @endverbatim
 *
 * @param len: Número de ciclos de período do Timer 0 que definem a duração do 
 *        tempo morto. Faixa: 0–255.
 *
 * @note Length expressa que se trata da “duração” do tempo morto, em ciclos. 
 *       "Timer 0 Dead zone length" → isso é só um parâmetro adicional do PWM 
 *       Timer 0, que define o tempo do “gap” morto na comutação.
 */
void Panel_RA8889::PWM0_DeadZoneLength(uint8_t len)
{
  SPI_CmdWrite(REG_DZ_LENGTH);                 //0x87, Timer 0 Dead zone length register [DZ_LENGTH]
  SPI_DataWrite(len);
}


//================================================================================
//
// [0x88] Timer 0 compare buffer register [TCMPB0L]
// [0x89] Timer 0 compare buffer register [TCMPB0H]
//
//================================================================================


/**
 * @brief Configura o valor do Compare Buffer do PWM Timer 0.
 *
 *@verbatim
 * REG [0x88] Timer 0 compare buffer register [TCMPB0L]
 *            bit [7-0] Timer 0 compare buffer register --- Low Byte
 *            Compare buffer register is 16 bits in total. When timer counter 
 *            is eqaual to or less than compare buffer register, PWM 0 will 
 *            output high level if PWM Timer 0 output inverter on/off bit is 
 *            off.
 *
 * REG [0x89] Timer 0 compare buffer register [TCMPB0H]
 *            bit [7-0] Timer 0 compare buffer register --- High Byte
 *            Compare buffer register is 16 bits in total. When timer counter 
 *            is eqaual to or less than compare buffer register, PWM 0 will 
 *            output high level if PWM Timer 0 output inverter on/off bit is 
 *            off.
 *
 * Como o Timer funciona com ele:
 *
 *   O PWM Timer 0 conta de 0 até o valor máximo do período configurado. A 
 *   cada ciclo:
 *
 *   - Se o contador ≤ valor do Compare Buffer, o pino XPWM[0] é colocado em 
 *     nível alto (ou invertido se o bit de inversão estiver ativo).
 *
 *   - Quando o contador ultrapassa o valor do Compare Buffer, o pino vai para 
 *     nível baixo.
 *
 *   Recebe Wx (0 a 65535) → quanto maior o valor, maior o tempo em que o PWM 
 *   fica em nível alto durante o ciclo. 
 *
 *   Em outras palavras: o Compare Buffer define o duty cycle do PWM.
 * @endverbatim
 *
 * @param Wx Valor de 16 bits para o compare buffer do Timer 0.
 *
 * @note When timer counter equal or less than compare buffer register will 
 *       cause PWM out high level if inv_on bit is off. 
 *       Este buffer determina o ponto em que o PWM muda de nível durante o ciclo.
 *       É útil para ajustar o duty cycle do PWM com precisão.
 */
void Panel_RA8889::PWM0_SetCompareBuffer(uint16_t Wx)   
{   
  SPI_CmdWrite(REG_TCMPB0L);                   //0x88, Timer 0 compare buffer register [TCMPB0L]
  SPI_DataWrite(Wx);                           
  SPI_CmdWrite(REG_TCMPB0H);                   //0x89, Timer 0 compare buffer register [TCMPB0H]
  SPI_DataWrite(Wx >> 8);                      
}


//================================================================================
//
// [0x8A] Timer 0 count buffer register [TCNTB0L]
// [0x8B] Timer 0 count buffer register [TCNTB0H]
//
//================================================================================


/**
 * @brief Count Buffer PWM Timer 0
 *
 *@verbatim
 * REG [0x8a] Timer 0 count buffer register [TCNTB0L]
 *            bit [7-0] Timer 0 count buffer register --- Low Byte
 *            Count buffer register is 16 bits in total. When timer counter is equal to 0 and reload_en bit is enabled, the PWM timer will reload the value of Count buffer register to the counter.
 *            The current value of the timer counter (TCNT0) can be read back when the PWM timer starts.
 *
 * REG [0x8b] Timer 0 count buffer register [TCNTB0H]
 *            bit [7-0] Timer 0 count buffer register --- High Byte
 *            Count buffer register is 16 bits in total. When timer counter is equal to 0 and reload_en bit is enabled, the PWM timer will reload the value of Count buffer register to the counter.
 *            The current value of the timer counter (TCNT0) can be read back when the PWM timer starts.
 * @endverbatim
 *
 * @param Wx
 *
 * @note Count buffer register total has 16 bits.
 *       When timer counter equal to 0 will cause PWM timer reload Count buffer register if reload_en bit set as enable.
 *       It may read back timer counter��s real time value when PWM timer start.
 */
void Panel_RA8889::PWM0_SetCountBuffer(uint16_t Wx)
{
  SPI_CmdWrite(REG_TCNTB0L);                   //0x8a, Timer 0 count buffer register [TCNTB0L]
  SPI_DataWrite(Wx);                           
  SPI_CmdWrite(REG_TCNTB0H);                   //0x8b, Timer 0 count buffer register [TCNTB0H]
  SPI_DataWrite(Wx >> 8);                      
}


//================================================================================
//
// [0x8C] Timer 1 compare buffer register [TCMPB1L]
// [0x8D] Timer 1 compare buffer register [TCMPB1H]
//
//================================================================================


/**
 * @brief Configura o valor do Compare Buffer do PWM Timer 1.
 *
 *@verbatim
 * REG [0x8c] Timer 1 compare buffer register [TCMPB1L]
 *            bit [7-0] Compare buffer register is 16 bits in total. When timer counter is equal to or less than the value of compare buffer register and inv_on bit is off, PWM will output high level.
 *
 * REG [0x8d] Timer 1 compare buffer register [TCMPB1H]
 *            bit [7-0] Timer 0 compare buffer register --- High Byte
 *            Compare buffer register is 16 bits in total. When timer counter is equal to or less than the value of compare buffer register and inv_on bit is off, PWM will output high level.
 *
 * Como o Timer funciona com ele:
 *
 *   O PWM Timer 1 conta de 0 até o valor máximo do período configurado. A 
 *   cada ciclo:
 *
 *   - Se o contador ≤ valor do Compare Buffer, o pino XPWM[1] é colocado em 
 *     nível alto (ou invertido se o bit de inversão estiver ativo).
 *
 *   - Quando o contador ultrapassa o valor do Compare Buffer, o pino vai para 
 *     nível baixo.
 *
 *   Recebe Wx (0 a 65535) → quanto maior o valor, maior o tempo em que o PWM 
 *   fica em nível alto durante o ciclo. 
 *
 *   Em outras palavras: o Compare Buffer define o duty cycle do PWM.
 * @endverbatim
 *
 * @param Wx Valor de 16 bits para o compare buffer do Timer 1.
 *
 * @note When timer counter equal or less than compare buffer register will 
 *       cause PWM out high level if inv_on bit is off. 
 *       Este buffer determina o ponto em que o PWM muda de nível durante o ciclo.
 *       É útil para ajustar o duty cycle do PWM com precisão.
 */
void Panel_RA8889::PWM1_SetCompareBuffer(uint16_t Wx)
{
  SPI_CmdWrite(REG_TCMPB1L);                   //0x8c, Timer 1 compare buffer register [TCMPB1L]
  SPI_DataWrite(Wx);                           
  SPI_CmdWrite(REG_ TCMPB1H);                  //0x8d, Timer 1 compare buffer register [TCMPB1H]
  SPI_DataWrite(Wx >> 8);                      
}


//================================================================================
//
// [0x8E] Timer 1 count buffer register [TCNTB1L]
// [0x8F] Timer 1 count buffer register [TCNTB1H]
//
//================================================================================


/**
 * @brief Count Buffer PWM Timer 1
 *
 *@verbatim
 * REG [0x8e] Timer 1 count buffer register [TCNTB1L]
 *            bit [7-0] Timer 1 count buffer register --- Low Byte
 *            Count buffer register is 16 bits in total. When timer counter is equal to 0 and reload_en bit is enabled, PWM timer will reload the value of Count buffer register.
 *            The current value of the timer counter (TCNT1) can be read back when the PWM timer starts.
 *
 * REG [0x8f] Timer 1 count buffer register [TCNTB1H]
 *            bit [7-0] Timer 1 count buffer register --- High Byte
 *            Count buffer register is 16 bits in total. When timer counter is equal to 0 and reload_en bit is enabled, PWM timer will reload the value of Count buffer register.
 *            The current value of the timer counter (TCNT1) can be read back when the PWM timer starts.
 * @endverbatim
 *
 * @param Wx
 *
 * @note Count buffer register total has 16 bits.
 *       When timer counter equal to 0 will cause PWM timer reload Count buffer register if reload_en bit set as enable.
 *       It may read back timer counter��s real time value when PWM timer start.
 */
void Panel_RA8889::PWM1_SetCountBuffer(uint16_t Wx)
{
  SPI_CmdWrite(REG_TCNTB1L);                   //0x8e, Timer 1 count buffer register [TCNTB1L]
  SPI_DataWrite(Wx);                           
  SPI_CmdWrite(REG_TCNTB1H);                   //0x8f, Timer 1 count buffer register [TCNTB1H]
  SPI_DataWrite(Wx >> 8);                      
}


//================================================================================
//
// [0x90] BTE Function Control Register 0 (BTE_CTRL0)
//
//================================================================================


/**
 * @brief BTE function Enable/Disable
 *
 * @verbatim
 * REG [0x90] BTE Function Control Register 0 (BTE_CTRL0)
 *            bit [4] BTE Function Enable / Status
 *            Write:
 *            0: BTE function is disabled.
 *            1: BTE function is enabled.
 *            Read:
 *            0: BTE function is idle.
 *            1: BTE function is busy.
 *
 *            Note: When BTE function is enabled, it’s not allowed to access 
 *            the memory space of the active window in canvas.
 *
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::BTE_Enable(bool b)
{ 
  uint8_t temp;
  SPI_CmdWrite(REG_BTE_CTRL0);                 //0x90, BTE Function Control Register 0 (BTE_CTRL0)
  temp = SPI_DataRead();
  b ? temp |= cSetb4 : temp &= cClrb4;
  SPI_DataWrite(temp);
}


/**
 * @brief Wait until Core Task (BTE) is Ready
 *
 *@verbatim
 * Status Register (STSR)
 * 
 * bit [3] Core task is busy (fontwr_busy)
 *        Following task is running:
 *        BTE, Geometry engine, Serial flash DMA, Text write or Graphic write
 *        0: task is done or idle.
 *        1: task is busy.
 *        While User change canvas relative setting & switch text 
 *        mode or graphic mode must make sure core task is done. 
 *        Note: BTE, Geometry drawing & Serial flash DMA also may 
 *        check each start bit. Under text mode, if user wants to 
 *        change rotate attribute, character line gap, 
 *        character-to-character space, foreground color, 
 *        background color and Text/graphic mode setting, he must 
 *        make sure core_busy (fontwr_busy) status bit is low.
 * @endverbatim
 *
 * @param None
 *
 * @note Only RA8876/RA8877/RA8889
 */
void Panel_RA8889::BTE_WaitReady(void) 
{
    while (SPI_StatusRead() & 0x08) delayMicroseconds(1);
}


/**
 * @brief Wait until Core Task (BTE) is Ready
 *
 * @verbatim
 * Status Register (STSR)
 * 
 * bit [3] Core task is busy (fontwr_busy)
 *        Following task is running:
 *        BTE, Geometry engine, Serial flash DMA, Text write or Graphic write
 *        0: task is done or idle.
 *        1: task is busy.
 *        While User change canvas relative setting & switch text 
 *        mode or graphic mode must make sure core task is done. 
 *        Note: BTE, Geometry drawing & Serial flash DMA also may 
 *        check each start bit. Under text mode, if user wants to 
 *        change rotate attribute, character line gap, 
 *        character-to-character space, foreground color, 
 *        background color and Text/graphic mode setting, he must 
 *        make sure core_busy (fontwr_busy) status bit is low.
 *
 * REG [0x90] BTE Function Control Register 0 (BTE_CTRL0)
 *            bit [4] BTE Function Enable / Status
 *            Write:
 *            0: BTE function is disabled.
 *            1: BTE function is enabled.
 *            Read:
 *            0: BTE function is idle.
 *            1: BTE function is busy.
 *
 *            Note: When BTE function is enabled, it’s not allowed to access 
 *            the memory space of the active window in canvas.
 *
 * @endverbatim
 *
 * @param None
 *
 * @note Only RA8876/RA8877/RA8889. Block Transfer Engine (BTE)
 */
void Panel_RA8889::BTE_DualWaitReady(void) 
{
  uint8_t temp;
  
  //Case1: using BTE Function Control Register
  
  do {
    SPI_CmdWrite(REG_BTE_CTRL0);               //0x090, Seleciona o registro BTE Function Control
    temp = SPI_DataRead();                     //
  	delayMicroseconds(1);                      //Libera CPU parcialmente
  } while (temp & 0x10);                       //Continua enquanto BTE estiver ocupado
  
  //Case2: using STSR Status Register
  
  do {
    temp = SPI_StatusRead();                   //Ler STSR (status geral do core) 
  	delayMicroseconds(1);                      //Libera CPU parcialmente
  } while (temp & 0x08);                       //Continua enquanto o core estiver ocupado
}


/**
 * @brief Pattern Format 8x8
 *
 * @verbatim
 * REG [0x90] BTE Function Control Register 0 (BTE_CTRL0)
 *            bit [0] PATTERN Format
 *            0b0: 8x8
 *            0b1: 16x16
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::BTE_PatternFormat8X8(void)
{ 
  uint8_t temp;
  SPI_CmdWrite(REG_BTE_CTRL0);          //0x90, BTE Function Control Register 0 (BTE_CTRL0)
  temp = SPI_DataRead();
  temp &= cClrb0;
  SPI_DataWrite(temp);
} 


/**
 * @brief Pattern Format 16x16
 *
 * @verbatim
 * REG [0x90] BTE Function Control Register 0 (BTE_CTRL0)
 *            bit [0] PATTERN Format
 *            0b0: 8x8
 *            0b1: 16x16
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::BTE_PatternFormat16X16(void)
{ 
  uint8_t temp;
  SPI_CmdWrite(REG_BTE_CTRL0);          //0x90, BTE Function Control Register 0 (BTE_CTRL0)
  temp = SPI_DataRead();
  temp |= cSetb0 ;
  SPI_DataWrite(temp);
} 


//================================================================================
//
// [0x91] BTE Function Control Register1 (BTE_CTRL1)
//
//================================================================================


/**
 * @brief BTE ROP Code
 *
 * @verbatim
 * REG [0x91] BTE Function Control Register1 (BTE_CTRL1)
 *            bit [7-4] BTE ROP Code Bit[3:0] or color expansion starting bit
 *            
 *            a. ROP is the acronym for Raster Operation. Some BTE operations 
 *               can be combined with ROP operations. (Please refer to Section 
 *               2.7)
 *
 *            Code        Description              enum eROPCode
 *            ----------------------------------------------     
 *            0b0000 0x0  0 ( Blackness )          Blackness
 *            0b0001 0x1  ~S0・~S1 or ~ ( S0+S1 )   NotS0_And_NotS1
 *            0b0010 0x2  ~S0・S1                   NotS0_And_S1
 *            0b0011 0x3  ~S0                      NotS0
 *            0b0100 0x4  S0・~S1                   S0_And_NotS1
 *            0b0101 0x5  ~S1                      NotS1
 *            0b0110 0x6  S0^S1                    S0_Xor_S1
 *            0b0111 0x7  ~S0+~S1 or ~ ( S0・S1 )   NotS0_Or_NotS1
 *            0b1000 0x8  S0・S1                    S0_And_S1
 *            0b1001 0x9  ~ ( S0^S1 )              Not_S0_Xor_S1
 *            0b1010 0xa  S1                       S1
 *            0b1011 0xb  ~S0+S1                   NotS0_Or_S1
 *            0b1100 0xc  S0                       S0
 *            0b1101 0xd  S0+~S1                   S0_Or_NotS1
 *            0b1110 0xe  S0+S1                    S0_Or_S1
 *            0b1111 0xf  1 ( Whiteness )          Whiteness
 *
 *            b. If BTE operation code function are color expansion with or 
 *               without chroma key (08h / 09h / Eh / Fh), then these bits 
 *               stand for starting bit on BTE window left boundary. MSB 
 *               stands for left most pixel. For 8-bits MPU, value should 
 *               within 0 to 7. For 16-bits MPU, value should within 0 to 15.
 * @endverbatim
 *
 * @param code
 *
 * @note ROP is the acronym for Raster Operation.
 */
void Panel_RA8889::BTE_ROPCode(eROPCode code)
{ 
  uint8_t temp;
  SPI_CmdWrite(REG_BTE_CTRL1);                 //BTE Function Control Register1 (BTE_CTRL1)
  temp = SPI_DataRead();                       
  temp &= 0x0f;                                //Limpar os bits [7-4]
  temp |= (static_cast<uint8_t>(code) << 4);   
  SPI_DataWrite(temp);
}


/**
 * @brief BTE Operation Code
 *
 * @verbatim
 * REG [0x91] BTE Function Control Register1 (BTE_CTRL1)
 *            bit [3-0] BTE Operation Code Bit[3:0]
 *            RA8889 builds in 2D BTE Engine which provides 13 BTE functions. Some of BTE Operations can be combined with the
 *            0b0000 MPU Write with ROP.
 *              S0: comes from MPU data
 *              S1: comes from memory
 *              D:  According to ROP write to memory
 *            0b0001 Reserved
 *            0b0010 Memory Copy with ROP
 *              S0: comes from memory.
 *              S1: comes from memory
 *              D:  According to ROP Write to memory
 *            0b0011 Reserved
 *            0b0100 MPU Write w/ chroma keying (w/o ROP)
 *              S0: comes from MPU data
 *              If MPU data doesn’t match with chroma key color (specified by background color) then writes to destination.
 *            0b0101 Memory Copy (move) w/ chroma keying (w/o ROP)
 *              S0 comes from memory, and S1 is useless.
 *              If S0 data doesn’t match with chroma key color (specified by background color) then S0 data will write to destination.
 *            0b0110 Pattern Fill with ROP
 *              Pattern is specified by S0.
 *            0b0111 Pattern Fill with chroma keying
 *              Pattern is specified by S0.
 *              If S0 data doesn’t match with chroma key color (specified by background color) then writes to destination.
 *            0b1000 MPU Write w/ Color Expansion
 *              S0 comes from MPU data and convert to specified color & color depth then write to destination.
 *            0b1001 MPU Write w/ Color Expansion and chroma keying
 *              S0 comes from MPU data and If the data bit is “1” then convert to specified foreground color & color depth then write to destination.
 *            0b1010b Memory Copy with opacity
 *              S0, S1 & D: locate in memory
 *            0b1011 MPU Write with opacity
 *              S0: comes from MPU data
 *              S1: comes from memory
 *              D: According to Alpha blending operation write to memory
 *            0b1100 Solid Fill
 *              Destination data comes from register.
 *            0b1101 Reserved
 *            0b1110 Memory Copy w/ Color Expansion
 *              S0 & D locate in memory and S1 is useless
 *              S0 must be pre-loaded into memory with 8bpp or 16bpp color depth via MPU write or DMA, thus S0 color depth should follow that color depth.
 *            0b1111 Memory Copy w/ Color Expansion and chroma keying
 *              S0 & D locate in memory and S1 is useless.
 *              S0 must be pre-loaded into memory with 8bpp or 16bpp color depth via MPU write or DMA, thus S0 color depth should follow that color depth.
 *              If S0 data bit=0 then no data will be written into D. If S0 data bit=1 then foreground color data will be written to D.
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::BTE_OperationCode(eBTEOpCode opcode)
{ 
  uint8_t temp;
  SPI_CmdWrite(REG_BTE_CTRL1);                 //BTE Function Control Register1 (BTE_CTRL1)
  temp = SPI_DataRead();                       
  temp &= 0xf0;                                //Limpa os bits de 3-0
  temp |= static_cast<uint8_t>(opcode);        //Coloca opcode
  SPI_DataWrite(temp);                         
}


//================================================================================
//
// [0x92] Source 0/1 & Destination Color Depth (BTE_COLR)
//
//================================================================================


/**
 * @brief BTE Source 0 (S0) Color Depth
 *
 * @verbatim
 * REG [0x92] Source 0/1 & Destination Color Depth (BTE_COLR)
 *            bit [6-5] S0 Color Depth
 *            00 : 256 Color
 *            01 : 64k Color
 *            1x : 16M Color
 * @endverbatim
 *
 * @param bpp:
 *        eColorDepthBPP::bpp8
 *        eColorDepthBPP::bpp16
 *        eColorDepthBPP::bpp24
 *
 * @note None
 */
void Panel_RA8889::BTE_S0_ColorDeph(eColorDepthBPP bpp)
{ 
  uint8_t temp;
  uint8_t bit;
  SPI_CmdWrite(REG_BTE_COLR);                     //0x92, Source 0/1 & Destination Color Depth (BTE_COLR)
  temp = SPI_DataRead();
  temp &= ~(cSetb6 | cSetb5);                     //Reseta bits 6 e 5
  bit = static_cast<uint8_t>(bpp);
  bit = (bit >> 3)-1;                             //transforma 8,16,24 em 0, 1, 2
  bit = bit << 5;                                 //posiciona no bit 6 e 5
  temp |= bit;
  SPI_DataWrite(temp);
} 


/**
 * @brief BTE Source 1 (S1) Color Depth 256 Color 
 *
 * @verbatim
 * REG [0x92] Source 0/1 & Destination Color Depth (BTE_COLR)
 *            bit [4-2] S1 Color Depth
 *            0b000 : 256 Color
 *            0b001 : 64k Color
 *            0b010 : 16M Color
 *            0b011 : Constant Color
 *            0b100 : 8 bit pixel alpha blending
 *            0b101 : 16 bit pixel alpha blending
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::BTE_S1_ColorDeph_8bpp(void)
{ 
  uint8_t temp;
  SPI_CmdWrite(REG_BTE_COLR);                     //0x92, Source 0/1 & Destination Color Depth (BTE_COLR)
  temp = SPI_DataRead();
  temp &= cClrb4 ;
  temp &= cClrb3 ;
  temp &= cClrb2 ;
  SPI_DataWrite(temp);
} 


/**
 * @brief BTE Source 1 (S1) Color Depth 64k Color 
 *
 * @verbatim
 * REG [0x92] Source 0/1 & Destination Color Depth (BTE_COLR)
 *            bit [4-2] S1 Color Depth
 *            0b000 : 256 Color
 *            0b001 : 64k Color
 *            0b010 : 16M Color
 *            0b011 : Constant Color
 *            0b100 : 8 bit pixel alpha blending
 *            0b101 : 16 bit pixel alpha blending
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::BTE_S1_ColorDeph_16bpp(void)
{ 
  uint8_t temp;
  SPI_CmdWrite(REG_BTE_COLR);                     //0x92, Source 0/1 & Destination Color Depth (BTE_COLR)
  temp = SPI_DataRead();
  temp &= cClrb4 ;
  temp &= cClrb3 ;
  temp |= cSetb2 ;
  SPI_DataWrite(temp);
}


/**
 * @brief BTE Source 1 (S1) Color Depth 16M Color 
 *
 * @verbatim
 * REG [0x92] Source 0/1 & Destination Color Depth (BTE_COLR)
 *            bit [4-2] S1 Color Depth
 *            0b000 : 256 Color
 *            0b001 : 64k Color
 *            0b010 : 16M Color
 *            0b011 : Constant Color
 *            0b100 : 8 bit pixel alpha blending
 *            0b101 : 16 bit pixel alpha blending
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::BTE_S1_ColorDeph_24bpp(void)
{ 
  uint8_t temp;
  SPI_CmdWrite(REG_BTE_COLR);                     //0x92, Source 0/1 & Destination Color Depth (BTE_COLR)
  temp = SPI_DataRead();
  temp &= cClrb4 ;
  temp |= cSetb3 ;
  temp &= cClrb2 ;
  SPI_DataWrite(temp);
}


/**
 * @brief BTE Source 1 (S1) Color Depth Constant Color 
 *
 * @verbatim
 * REG [0x92] Source 0/1 & Destination Color Depth (BTE_COLR)
 *            bit [4-2] S1 Color Depth
 *            0b000 : 256 Color
 *            0b001 : 64k Color
 *            0b010 : 16M Color
 *            0b011 : Constant Color
 *            0b100 : 8 bit pixel alpha blending
 *            0b101 : 16 bit pixel alpha blending
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::BTE_S1_ColorDeph_Constant(void)
{ 
  uint8_t temp;
  SPI_CmdWrite(REG_BTE_COLR);                     //0x92, Source 0/1 & Destination Color Depth (BTE_COLR)
  temp = SPI_DataRead();
  temp &= cClrb4;
  temp |= cSetb3;
  temp |= cSetb2;
  SPI_DataWrite(temp);
}


/**
 * @brief BTE Source 1 (S1) Color Depth 8-bit Alpha Blend
 *
 * @verbatim
 * REG [0x92] Source 0/1 & Destination Color Depth (BTE_COLR)
 *            bit [4-2] S1 Color Depth
 *            0b000 : 256 Color
 *            0b001 : 64k Color
 *            0b010 : 16M Color
 *            0b011 : Constant Color
 *            0b100 : 8 bit pixel alpha blending
 *            0b101 : 16 bit pixel alpha blending
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::BTE_S1_ColorDeph_8bitAlpha(void)
{ 
  uint8_t temp;
  SPI_CmdWrite(REG_BTE_COLR);                     //0x92, Source 0/1 & Destination Color Depth (BTE_COLR)
  temp = SPI_DataRead();
  temp |= cSetb4;
  temp &= cClrb3;
  temp &= cClrb2;
  SPI_DataWrite(temp);
}


/**
 * @brief BTE Source 1 (S1) Color Depth 16-bit Alpha Blend
 *
 * @verbatim
 * REG [0x92] Source 0/1 & Destination Color Depth (BTE_COLR)
 *            bit [4-2] S1 Color Depth
 *            0b000 : 256 Color
 *            0b001 : 64k Color
 *            0b010 : 16M Color
 *            0b011 : Constant Color
 *            0b100 : 8 bit pixel alpha blending
 *            0b101 : 16 bit pixel alpha blending
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::BTE_S1_ColorDeph_16bitAlpha(void)
{ 
  uint8_t temp;
  SPI_CmdWrite(REG_BTE_COLR);                     //0x92, Source 0/1 & Destination Color Depth (BTE_COLR)
  temp = SPI_DataRead();
  temp |= cSetb4;
  temp &= cClrb3;
  temp |= cSetb2;
  SPI_DataWrite(temp);
}


/**
 * @brief BTE Destination (D) Color Depth
 *
 * @verbatim
 * REG [0x92] Source 0/1 & Destination Color Depth (BTE_COLR)
 *            bit [1-0] Destination Color Depth
 *            00 : 256 Color
 *            01 : 64k Color
 *            1x : 16M Color
 * @endverbatim
 *
 * @param bpp:
 *        eColorDepthBPP::bpp8  = 256 Color
 *        eColorDepthBPP::bpp16 = 64k Color  
 *        eColorDepthBPP::bpp24 = 16M Color
 *
 * @note None
 */
void Panel_RA8889::BTE_Destination_ColorDeph(eColorDepthBPP bpp)
{ 
  uint8_t temp;
  uint8_t bit;
  SPI_CmdWrite(REG_BTE_COLR);                     //0x92, Source 0/1 & Destination Color Depth (BTE_COLR)
  temp = SPI_DataRead();
  temp &= ~(cSetb1 | cSetb0);                     //Reseta bits 6 e 5
  bit = static_cast<uint8_t>(bpp);
  bit = (bit >> 3)-1;                             //transforma 8,16,24 em 0, 1, 2
  temp |= bit;
  SPI_DataWrite(temp);
} 


//================================================================================
//
// [0x93] Source 0 memory start address 0 (S0_STR0)
// [0x94] Source 0 memory start address 1 (S0_STR1)
// [0x95] Source 0 memory start address 2 (S0_STR2)
// [0x96] Source 0 memory start address 3 (S0_STR3)
//
//================================================================================



/**
 * @brief BTE Source 0 (S0) Memory Start Address
 *
 * @verbatim
 * REG [0x93] Source 0 memory start address 0 (S0_STR0)
 *            bit [7-2] Source 0 memory start address [7:2]
 *            bit [1-0] Fix at 0 [1:0]
 *
 * REG [0x94] Source 0 memory start address 1 (S0_STR1)
 *            bit [7-0] Source 0 memory start address [15:8]
 *
 * REG [0x95] Source 0 memory start address 2 (S0_STR2)
 *            bit [7-0] Source 0 memory start address [23:16]

 * REG [0x96] Source 0 memory start address 3 (S0_STR3)
 *            bit [7-0] Source 0 memory start address [31:24]
 * @endverbatim
 *
 * @param None
 *
 * @note Bit [1:0] tie to "00" internally.
 */
void Panel_RA8889::BTE_S0_MemoryStartAddress(uint32_t addr)
{
  RegisterWrite(REG_S0_STR0, addr);            //0x93, Source 0 memory start address 0 (S0_STR0)
  RegisterWrite(REG_S0_STR1, addr >> 8);       //0x94, Source 0 memory start address 1 (S0_STR1)
  RegisterWrite(REG_S0_STR2, addr >> 16);      //0x95, Source 0 memory start address 2 (S0_STR2)
  RegisterWrite(REG_S0_STR3, addr >> 24);      //0x96, Source 0 memory start address 3 (S0_STR3)
}


//================================================================================
//
// [0x97] Source 0 image width 0 (S0_WTH0)
// [0x98] Source 0 image width 1 (S0_WTH1)
//
//================================================================================


/**
 * @brief BTE Source 0 (S0) Image Width
 *
 * @verbatim
 * REG [0x97] Source 0 image width 0 (S0_WTH0)
 *            bit [7-2] Source 0 image width [7:2]
 *            bit [1-0] Fix to 00 [1:0]
 *
 *            Unit: Pixel.
 *            It must be divisible by 4. S0_WTH Bit [1:0] is internally fixed to 0.
 *            This value is the physical pixel number.
 *
 * REG [0x98] Source 0 image width 1 (S0_WTH1)
 *            bit [7-5] NA
 *            bit [4-0] Source 0 image width [12:8]
 *            Unit: Pixel.
 *            It must be divisible by 4. S0_WTH Bit [1:0] is internally fixed to 0.
 *            This value is the physical pixel number.
 * @endverbatim
 *
 * @param None
 *
 * @note Bit [1:0] tie to "00" internally.
 */
void Panel_RA8889::BTE_S0_ImageWidth(uint16_t Wx)
{
  RegisterWrite(REG_S0_WTH0, Wx);              //0x97, Source 0 image width 0 (S0_WTH0)
  RegisterWrite(REG_S0_WTH1, Wx >> 8);         //0x98, Source 0 image width 1 (S0_WTH1)
}


//================================================================================
//
// [0x99] Source 0 Window Upper-Left corner X-coordinates 0 (S0_X0)
// [0x9A] Source 0 Window Upper-Left corner X-coordinates 1 (S0_X1)
// [0x9B] Source 0 Window Upper-Left corner Y-coordinates 0 (S0_Y0)
// [0x9C] Source 0 Window Upper-Left corner Y-coordinates 1 (S0_Y1)
//
//================================================================================



/**
 * @brief BTE Source 0 (S0) Window Upper-Left Corner X,Y Cooordinates
 *
 * @verbatim
 * REG [0x99] Source 0 Window Upper-Left corner X-coordinates 0 (S0_X0)
 *            bit [7-0] Source 0 Window Upper-Left corner X-coordinates [7:0]
 *            The register is source 0 window upper-left corner x-coordinates
 *
 * REG [0x9a] Source 0 Window Upper-Left corner X-coordinates 1 (S0_X1)
 *            bit [7-5] NA
 *            bit [4-0] Source 0 Window Upper-Left corner X-coordinates [12:8]
 *            The register is source 0 window upper-left corner x-coordinates
 *
 * REG [0x9b] Source 0 Window Upper-Left corner Y-coordinates 0 (S0_Y0)
 *            bit [7-0] Source 0 Window Upper-Left corner Y-coordinates [7:0]
 *            The register is source 0 window upper-left corner y-coordinates
 *
 * REG [0x9c] Source 0 Window Upper-Left corner Y-coordinates 1 (S0_Y1)
 *            bit [7-5] NA
 *            bit [4-0] Source 0 Window Upper-Left corner Y-coordinates [12:8]
 *            The register is source 0 window upper-left corner y-coordinates
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::BTE_S0_WindowStart_XY(uint16_t Wx, uint16_t Hy)
{
  RegisterWrite(REG_S0_X0, Wx);                //0x99, Source 0 Window Upper-Left corner X-coordinates 0 (S0_X0)
  RegisterWrite(REG_S0_X1, Wx >> 8);           //0x9a, Source 0 Window Upper-Left corner X-coordinates 1 (S0_X1)
  RegisterWrite(REG_S0_Y0, Hy);                //0x9b, Source 0 Window Upper-Left corner Y-coordinates 0 (S0_Y0)
  RegisterWrite(REG_S0_Y1, Hy >> 8);           //0x9c, Source 0 Window Upper-Left corner Y-coordinates 1 (S0_Y1)
}


//================================================================================
//
// [0x9D] Source 1 memory start address 0 (S1_STR0) / S1 constant color – Red element (S1_RED)
// [0x9E] Source 1 memory start address 1 (S1_STR1) / S1 constant color – Green element (S1_GREEN)
// [0x9F] Source 1 memory start address 2 (S1_STR2) / S1 constant color – Blue element (S1_BLUE)
// [0xA0] Source 1 memory start address 3 (S1_STR3)
//
//================================================================================


/**
 * @brief BTE Source 1 (S1) Memory Start Address
 *
 * @verbatim
 * REG [0x9d] Source 1 memory start address 0 (S1_STR0) / S1 constant color – Red element (S1_RED)
 *            bit [7-0] Source 1 memory start address [7:2]
 *            If source 1 is set as constant color, then the register is defined as red element of this constant color. Otherwise, the register is defined as the start address (bit[7:2]) of source 1 and bit[1:0] must be set to 0.
 *
 * REG [0x9e] Source 1 memory start address 1 (S1_STR1) / S1 constant color – Green element (S1_GREEN)
 *            bit [7-0] Source 1 memory start address [15:8]
 *            If source 1 is set as constant color, then the register is defined as green element of this constant color.
 * 
 * REG [0x9f] Source 1 memory start address 2 (S1_STR2) / S1 constant color – Blue element (S1_BLUE)
 *            bit [7-0] Source 1 memory start address [23:16]
 *            If source 1 is set as constant color, then the register is defined as blue element of this constant color.
 *
 * REG [0xa0] Source 1 memory start address 3 (S1_STR3)
 *            bit [7-0] Source 1 memory start address [31:24]
 *            If source 1 is set as constant color, then the register is not used.
 * @endverbatim
 *
 * @param None
 *
 * @note Bit [1:0] tie to "00" internally.
 */
void Panel_RA8889::BTE_S1_MemoryStartAddress(uint32_t addr)
{
  RegisterWrite(REG_S1_STR0_RED, addr);        //0x9d, Source 1 memory start address 0 (S1_STR0) / S1 constant color – Red element (S1_RED)
  RegisterWrite(REG_S1_STR1_GREE, addr >> 8);  //0x9e, Source 1 memory start address 1 (S1_STR1) / S1 constant color – Green element (S1_GREEN)
  RegisterWrite(REG_S1_STR2_BLUE, addr >> 16); //0x9f, Source 1 memory start address 2 (S1_STR2) / S1 constant color – Blue element (S1_BLUE)
  RegisterWrite(REG_S1_STR3, addr >> 24);      //0xa0, Source 1 memory start address 3 (S1_STR3)
}


/**
 * @brief BTE Source 1 (S1) Constant Color R3G3B2 (256 color)
 *
 * @verbatim
 * If source 1 is set as constant color, then the register is defined as red element of this constant color.
 * Para setar o BTE S1 como constant color use a funcao BTE_S1_ColorDeph_Constant()
 *
 * REG [0x9d] Source 1 (S1) constant color – Red element (S1_RED)
 *            bit [7-5] Red element para 256 cores (8bpp)
 *
 * REG [0x9e] Source 1 (S1) constant color – Green element (S1_GREEN)
 *            bit [7-5] Green element para 256 cores (8bpp)
 * 
 * REG [0x9f] Source 1 (S1) constant color – Blue element (S1_BLUE)
 *            bit [7-5] Blue element para 256 cores (8bpp)
 *
 * @endverbatim
 *
 * @param color: Input Data Format:R3G3B2
 *
 * @note None
 */
void Panel_RA8889::S1_ConstantColor_256(uint8_t color)
{
  SPI_CmdWrite(REG_S1_STR0_RED);            //0x9d, Source 1 (S1) constant color – Red element (S1_RED)
  SPI_DataWrite(color);                     //RRRGGGBB >> 0 = RRRgggbbb para colocar na posicao [7:5]

  SPI_CmdWrite(REG_S1_STR1_GREE);           //0x9e, Source 1 (S1) constant color – Green element (S1_GREEN)
  SPI_DataWrite(color << 3);                //RRRGGGBB << 3 GGGbb000 para colocar na posicao [7:5]

  SPI_CmdWrite(REG_S1_STR2_BLUE);           //0x9f, Source 1 (S1) constant color – Blue element (S1_BLUE)
  SPI_DataWrite(color << 6);                //RRRGGGBB << 6 gg000000 para colocar na posicao [7:5]
}


/**
 * @brief BTE Source 1 (S1) Constant Color R5G6B5 (65k color)
 *
 * @verbatim
 * If source 1 is set as constant color, then the register is defined as red element of this constant color.
 * Para setar o BTE S1 como constant color use a funcao BTE_S1_ColorDeph_Constant()
 *
 * REG [0x9d] Source 1 (S1) constant color – Red element (S1_RED)
 *            bit [7-3] Red element para 65k cores (16bpp)
 *
 * REG [0x9e] Source 1 (S1) constant color – Green element (S1_GREEN)
 *            bit [7-2] Green element para 65k cores (16bpp)
 * 
 * REG [0x9f] Source 1 (S1) constant color – Blue element (S1_BLUE)
 *            bit [7-3] Blue element para 65k cores (16bpp)
 *
 * @endverbatim
 *
 * @param color: Input Data Format:R5G6B5
 *
 * @note None
 */
void Panel_RA8889::S1_ConstantColor_65k(uint16_t color)
{
  SPI_CmdWrite(REG_S1_STR0_RED);            //0x9d, Source 1 (S1) constant color – Red element (S1_RED)
  SPI_DataWrite(color >> 8);                //RRRRRGGGGGGBBBBB >> 8 = 00000000RRRRRggg para colocar na posicao [7:3]
  SPI_CmdWrite(REG_S1_STR1_GREE);           //0x9e, Source 1 (S1) constant color – Green element (S1_GREEN)
  SPI_DataWrite(color >> 3);                //RRRRRGGGGGGBBBBB >> 3 = 000rrrrrGGGGGGbb para colocar na posicao [7:2]
  SPI_CmdWrite(REG_S1_STR2_BLUE);           //0x9f, Source 1 (S1) constant color – Blue element (S1_BLUE)
  SPI_DataWrite(color << 3);                //RRRRRGGGGGGBBBBB << 3 = rrggggggBBBBB000 para colocar na posicao [7:3]
}


/**
 * @brief BTE Source 1 (S1) Constant Color R8G8B8 (16.7M color)
 *
 * @verbatim
 * If source 1 is set as constant color, then the register is defined as red element of this constant color.
 * Para setar o BTE S1 como constant color use a funcao BTE_S1_ColorDeph_Constant()
 *
 * REG [0x9d] Source 1 (S1) constant color – Red element (S1_RED)
 *            bit [7-0] Red element para 16M cores (24bpp)
 *
 * REG [0x9e] Source 1 (S1) constant color – Green element (S1_GREEN)
 *            bit [7-0] Green element para 16M cores (24bpp)
 * 
 * REG [0x9f] Source 1 (S1) constant color – Blue element (S1_BLUE)
 *            bit [7-0] Blue element para 16M cores (24bpp)
 *
 * @endverbatim
 *
 * @param color: Input Data Format:R8G8B8
 *
 * @note None
 */
void Panel_RA8889::S1_ConstantColor_16M(uint32_t color) 
{
  SPI_CmdWrite(REG_S1_STR0_RED);            //0x9d, Source 1 (S1) constant color – Red element (S1_RED)
  SPI_DataWrite(color >> 16);               //RRRRRRRRGGGGGGGGBBBBBBBB >> 16 = 0000000000000000RRRRRRRR para colocar na posicao [7:0]
  SPI_CmdWrite(REG_S1_STR1_GREE);           //0x9e, Source 1 (S1) constant color – Green element (S1_GREEN)
  SPI_DataWrite(color >> 8);                //00000000rrrrrrrrGGGGGGGG >> 8 = 00000000rrrrrrrrGGGGGGGG para colocar na posicao [7:0]
  SPI_CmdWrite(REG_S1_STR2_BLUE);           //0x9f, Source 1 (S1) constant color – Blue element (S1_BLUE)
  SPI_DataWrite(color);                     //RRRRRRRRGGGGGGGGBBBBBBBB >> 0 = rrrrrrrrggggggggBBBBBBBB para colocar na posicao [7:0]
}


//================================================================================
//
// [0xA1] Source 1 image width 0 (S1_WTH0)
// [0xA2] Source 1 image width 1 (S1_WTH1)
//
//================================================================================


/**
 * @brief Source 1 (S1) Image Width
 *
 * @verbatim
 * REG [0xa1] Source 1 image width 0 (S1_WTH0)
 *            bit [7-2] Source 1 image width [7:2]
 *                      Unit: Pixel.
 *                      It must be divisible by 4. S1_WTH Bit [1:0] is internally fixed to 0.
 *                      The value is the physical pixel number.
 *            bit [1-0] Fix to 00
 *
 * REG [0xa2] Source 1 image width 1 (S1_WTH1)
 *            bit [7-5] N/A
 *            bit [4-0] Source 1 image width [12:8]
 *                      Unit: Pixel.
 *                      It must be divisible by 4. S1_WTH Bit [1:0] is internally fixed to 0.
 *                      The value is the physical pixel number.
 *
 * @endverbatim
 *
 * @param Wx: width
 *
 * @note Bit [1:0] tie to "0" internally.
 */
void Panel_RA8889::BTE_S1_ImageWidth(uint16_t Wx)
{
  RegisterWrite(REG_S1_WTH0, Wx);              //0xa1, Source 1 image width 0 (S1_WTH0)
  RegisterWrite(REG_S1_WTH1, Wx >> 8);         //0xa2, Source 1 image width 1 (S1_WTH1)
}


//================================================================================
//
// [0xA3] Source 1 Window Upper-Left corner X-coordinates 0 (S1_X0)
// [0xA4] Source 1 Window Upper-Left corner X-coordinates 1 (S1_X1)
// [0xA5] Source 1 Window Upper-Left corner Y-coordinates 0 (S1_Y0)
// [0xA6] Source 1 Window Upper-Left corner Y-coordinates 1 (S1_Y1)
//
//================================================================================


/**
 * @brief BTE Source 1 (S1) Window Upper-Left Corner X,Y Cooordinates
 *
 * @verbatim
 * REG [0xa3] Source 1 Window Upper-Left corner X-coordinates 0 (S1_X0)
 *            bit [7-0] Source 1 Window Upper-Left corner X-coordinates [7:0]
 *            The register is source 1 window upper-left corner x-coordinates
 *
 * REG [0xa4] Source 1 Window Upper-Left corner X-coordinates 1 (S1_X1)
 *            bit [7-5] N/A
 *            bit [4-0] Source 1 Window Upper-Left corner X-coordinates [12:8]
 *            The register is source 1 window upper-left corner x-coordinates
 *
 * REG [0xa5] Source 0 Window Upper-Left corner Y-coordinates 0 (S1_Y0)
 *            bit [7-0] Source 1 Window Upper-Left corner Y-coordinates [7:0]
 *            The register is source 1 window upper-left corner y-coordinates
 *
 * REG [0xa6] Source 1 Window Upper-Left corner Y-coordinates 1 (S1_Y1)
 *            bit [7-5] N/A
 *            bit [4-0] Source 1 Window Upper-Left corner Y-coordinates [12:8]
 *            The register is source 1 window upper-left corner y-coordinates
 * @endverbatim
 *
 * @param Wx, Hy, coordinate x,y
 *
 * @note None
 */
void Panel_RA8889::BTE_S1_WindowStart_XY(uint16_t Wx, uint16_t Hy)
{
  RegisterWrite(REG_S1_X0, Wx);                //0xa3, Source 1 Window Upper-Left corner X-coordinates 0 (S1_X0)
  RegisterWrite(REG_S1_X1, Wx >> 8);           //0xa4, Source 1 Window Upper-Left corner X-coordinates 1 (S1_X1)
  RegisterWrite(REG_S1_Y0, Hy);                //0xa5, Source 1 Window Upper-Left corner Y-coordinates 0 (S1_Y0)
  RegisterWrite(REG_S1_Y1, Hy >> 8);           //0xa6, Source 1 Window Upper-Left corner Y-coordinates 1 (S1_Y1)
}


//================================================================================
//
// [0xA7] Destination memory start address 0 (DT_STR0)
// [0xA8] Destination memory start address 1 (DT_STR1)
// [0xA9] Destination memory start address 2 (DT_STR2)
// [0xAA] Destination memory start address 3 (DT_STR3)
//
//================================================================================


/**
 * @brief BTE Destination (D) Memory Start Address
 *
 * @verbatim
 * REG [0xa7] Destination memory start address 0 (DT_STR0)
 *            bit [7-2] Destination memory start address [7:2]
 *            bit [1-0] Fix at 0
 *
 * REG [0xa8] Destination memory start address 1 (DT_STR1)
 *            bit [7-0] Destination memory start address [15:8]
 *
 * REG [0xa9] Destination memory start address 2 (DT_STR2)
 *            bit [7-0] Destination memory start address [23:16]
 *
 * REG [0xaa] Destination memory start address 3 (DT_STR3)
 *            bit [7-0] Destination memory start address [31:24]
 * @endverbatim
 *
 * @param addr
 *
 * @note Bit [1:0] tie to "0" internally.
 *       Destination memory start address cannot set within from source 0|1 
 *       start address to source 0|1’s 
 *       (image width) * (image height) * (color depth[1|2|3])
 */
void Panel_RA8889::BTE_Destination_MemoryStartAddress(uint32_t addr) 
{
  RegisterWrite(REG_DT_STR0, addr);       //0xa7, Destination memory start address 0 (DT_STR0)
  RegisterWrite(REG_DT_STR1, addr >> 8);  //0xa8, Destination memory start address 0 (DT_STR1)
  RegisterWrite(REG_DT_STR2, addr >> 16); //0xa9, Destination memory start address 0 (DT_STR2)
  RegisterWrite(REG_DT_STR3, addr >> 24); //0xaa, Destination memory start address 0 (DT_STR3)
}


//================================================================================
//
// [0xAB] Destination image width 0 (DT_WTH0)
// [0xAC] Destination image width 1 (DT_WTH1)
//
//================================================================================


/**
 * @brief Destination (D) Image Width
 *
 * @verbatim
 * REG [0xab] Destination image width 0 (DT_WTH0)
 *            bit [7-2] Destination image width [7:2]
 *                      Unit: Pixel.
 *                      It must be divisible by 4. DT_WTH Bit [1:0] is internally fixed to 0.
 *                      The value is the physical pixel number.
 *            bit [1-0] Fix at 00
 *
 * REG [0xac] Destination image width 1 (DT_WTH1)
 *            bit [7-5] N/A
 *            bit [4-0] Destination image width [12:8]
 *                      Unit: Pixel.
 *                      It must be divisible by 4. DT_WTH Bit [1:0] is internally fixed to 0.
 *                      The value is the physical pixel number.
 *
 * @endverbatim
 *
 * @param Wx: width
 *
 * @note Bit [1:0] tie to "00" internally.
 */
void Panel_RA8889::BTE_Destination_ImageWidth(uint16_t Wx)
{
  RegisterWrite(REGDT_WTH0, Wx);               //0xab, Destination image width 0 (DT_WTH0)
  RegisterWrite(REGDT_WTH1, Wx >> 8);          //0xac, Destination image width 1 (DT_WTH1)
}


//================================================================================
//
// [0xAD] Destination Window Upper-Left corner X-coordinates 0 (DT_X0)
// [0xAE] Destination Window Upper-Left corner X-coordinates 1 (DT_X1)
// [0xAF] Destination Window Upper-Left corner Y-coordinates 0 (DT_Y0)
// [0xB0] Destination Window Upper-Left corner Y-coordinates 1 (DT_Y1)
//
//================================================================================


/**
 * @brief Destination (D) Windows Upper-Left Corner X,Y Coordinates
 *
 * @verbatim
 * REG [0xad] Destination Window Upper-Left corner X-coordinates 0 (DT_X0)
 *            bit [7-0] Destination Window Upper-Left corner X-coordinates [7:0]
 *
 * REG [0xae] Destination Window Upper-Left corner X-coordinates 1 (DT_X1)
 *            bit [7-5] N/A
 *            bit [4-0] Destination Window Upper-Left corner X-coordinates [12:8]
 *
 * REG [0xaf] Destination Window Upper-Left corner Y-coordinates 0 (DT_Y0)
 *            bit [7-0] Destination Window Upper-Left corner Y-coordinates [7:0]
 *
 * REG [0xb0] Destination Window Upper-Left corner Y-coordinates 1 (DT_Y1)
 *            bit [7-5] N/A
 *            bit [4-0] Destination Window Upper-Left corner Y-coordinates [12:8]
 * @endverbatim
 *
 * @param Wx, Hy
 *
 * @note None
 */
void Panel_RA8889::BTE_Destination_WindowStart_XY(uint16_t Wx, uint16_t Hy)
{
  RegisterWrite(REG_DT_X0, Wx);                //0xad, Destination Window Upper-Left corner X-coordinates 0 [7:0] (DT_X0)
  RegisterWrite(REG_DT_X1, Wx >> 8);           //0xae, Destination Window Upper-Left corner X-coordinates 1 [12:8] (DT_X1)
  RegisterWrite(REG_DT_Y0, Hy);                //0xaf, Destination Window Upper-Left corner Y-coordinates 0 [7:0] (DT_Y0)
  RegisterWrite(REG_DT_Y1, HY >> 8);           //0xb0, Destination Window Upper-Left corner Y-coordinates 1 12:8] (DT_Y1)
}


//================================================================================
//
// [0xB1] BTE Window Width 0 (BTE_WTH0)
// [0xB2] BTE Window Width 1 (BTE_WTH1)
// [0xB3] BTE Window Height 0 (BTE_HIG0)
// [0xB4] BTE Window Height 1 (BTE_HIG1)
//
//================================================================================


/**
 * @brief BTE Window Size (width, height)
 *
 * @verbatim
 * REG [0xb1] BTE Window Width 0 (BTE_WTH0)
 *            bit [7-0] BTE Window Width Setting[7:0]
 *            Unit: Pixel.
 *            The value is physical pixel number.
 *
 * REG [0xb2] BTE Window Width 1 (BTE_WTH1)
 *            bit [7-5] N/A
 *            bit [4-0] BTE Window Width Setting [12:8]
 *            Unit: Pixel.
 *            The value is physical pixel number.
 *
 * REG [0xb3] BTE Window Height 0 (BTE_HIG0)
 *            bit [7-0] BTE Window Height Setting[7:0]
 *            Unit: Pixel.
 *            The value is physical pixel number.
 *
 * REG [0xb4] BTE Window Height 1 (BTE_HIG1)
 *            bit [7-5] N/A
 *            bit [4-0] BTE Window Height Setting [12:8]
 *            Unit: Pixel.
 *            The value is physical pixel number.
 * @endverbatim
 *
 * @param Wx, Hy: Width and Height size
 *
 * @note None
 */
void Panel_RA8889::BTE_WindowSize(uint16_t Wx, uint16_t Hy)
{
  RegisterWrite(REG_BTE_WTH0, Wx);       //0xb1, BTE Window Width 0 [7:0] (BTE_WTH0)
  RegisterWrite(REG_BTE_WTH1, Wx >> 8);  //0xb2, BTE Window Width 1 [12:8] (BTE_WTH1)
  RegisterWrite(REG_BTE_HIG0, Hy);       //0xb3, BTE Window Height 0 [7:0] (BTE_HIG0)
  RegisterWrite(REG_BTE_HIG1, Hy >> 8);  //0xb4, BTE Window Height 1 [12:8] (BTE_HIG1)
}


//================================================================================
//
// [0xB5] Alpha Blending (APB_CTRL)
//
//================================================================================


/**
 * @brief Window Alpha Blending Effect for S0 & S1
 *
 * @verbatim
 * REG [0xab] Alpha Blending (APB_CTRL)
 *            bit [7-4] N/A
 *            bit [5-0] Window Alpha Blending effect for S0 & S1
 *            The value of alpha in the color code ranges from 0.0 to 1.0,
 *            where 0.0 represents a fully transparent color, and 1.0
 *            represents a fully opaque color.
 *            00h: 0
 *            01h: 1/32
 *            02h: 2/32
 *            :
 *            1Eh: 30/32
 *            1Fh: 31/32
 *            2Xh: 1
 *            
 *            Output Effect = (S0 image x (1 - alpha setting value))
 *                          + (S1 image x alpha setting value)
 * @endverbatim
 *
 * @param value
 *
 * @note Bit [5:4] from Window Alpha Blending effect bit [5-0] overlap in [7-4] N/A
 */
void Panel_RA8889::BTE_AlphaBlendingEffect(uint8_t value)
{ 
  SPI_CmdWrite(REG_APB_CTRL);                  //0xb5, Alpha Blending (APB_CTRL)
  SPI_DataWrite(temp);  
}


//================================================================================
//
// [0xB6] Serial flash DMA Controller REG (DMA_CTRL)
// [0xB7] Serial Flash/ROM Controller Register (SFL_CTRL)
//
//================================================================================


/**
 * @brief Serial Flash DMA Start
 *        
 * @verbatim                  
 * REG [0xb6] Serial flash DMA Controller REG (DMA_CTRL)
 *            bit [0] Write: DMA Start Bit
 *                    Set to 1 by MPU and reset to 0 automatically
 *                    The bit cannot start when fontwr_busy is 1. On 
 *                    the contrary, if DMA is enabled, the text mode & 
 *                    sending character code are disabled.
 * @endverbatim
 *
 * @param None
 *
 * @note  None
 *
 * @result None
 */
void Panel_RA8889::SFI_DMA_Start(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_DMA_CTRL);                  //0xb6, Serial flash DMA Controller REG (DMA_CTRL)
  temp = SPI_DataRead();                       
  temp |= cSetb0;
  SPI_DataWrite(temp);
}


/**
 * @brief Aguarde até que o Serial flash DMA Controller esteja pronto/ocioso
 *        
 * @verbatim                  
 * REG [0xb6] Serial flash DMA Controller REG (DMA_CTRL)
 *            bit [0] Read: DMA Busy Check Bit
 *                    0: Idle
 *                    1: Bsy
 *                    *** about DMA transfer of serial flash, its 
 *                    destination starting address, destination image 
 *                    width, color depth & address mode in SDRAM are 
 *                    followed by Canvas’ setting and only operate in 
 *                    graphic mode.
 * @endverbatim
 *
 * @param None
 *
 * @note  None
 *
 * @result None
 */
void Panel_RA8889::SFI_DMA_WaitReady(void)
{
  uint8_t temp;
  //Case 1: Using DMA Function Control Register
  SPI_CmdWrite(REG_DMA_CTRL);                  //0xb6, Serial flash DMA Controller REG (DMA_CTRL)
  do {
    temp = LCD_DataRead();
  } while (temp & 0x01);                       //Enquanto estiver em Busy 
}


/**
 * @brief Select Serial Flash/ROM Access Font Mode
 *        
 * @verbatim                  
 * REG [0xb7] Serial Flash/ROM Controller Register (SFL_CTRL)
 *            bit [6] Serial Flash / ROM Access Mode
 *                    0b0: Font mode – for external CGROM
 *                    0b1: DMA mode – for CGRAM , pattern , boot start image or OSD
 * @endverbatim
 *
 * @param None
 *
 * @note  None
 *
 * @result None
 */
void Panel_RA8889::Select_SFI_FontMode(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_SFL_CTRL);                  //0xb7, Serial Flash/ROM Controller Register (SFL_CTRL)
  temp = SPI_DataRead();
  temp &= cClrb6;                              //Reset bit 6
  SPI_DataWrite(temp);
}


/**
 * @brief Select Serial Flash/ROM Access DMA Mode
 *        
 * @verbatim                  
 * REG [0xb7] Serial Flash/ROM Controller Register (SFL_CTRL)
 *            bit [6] Serial Flash / ROM Access Mode
 *                    0b0: Font mode – for external CGROM
 *                    0b1: DMA mode – for CGRAM, pattern, boot start image or OSD
 * @endverbatim
 *
 * @param None
 *
 * @note  None
 *
 * @result None
 */
void Panel_RA8889::Select_SFI_DMAMode(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_SFL_CTRL);                  //0xb7, Serial Flash/ROM Controller Register (SFL_CTRL)
  temp = SPI_DataRead();
  temp |= cSetb6;                              //Set bit 6
  SPI_DataWrite(temp);
}


/**
 * @brief Select Serial Flash/ROM Address 24-bit Mode
 *        
 * @verbatim                  
 * REG [0xb7] Serial Flash/ROM Controller Register (SFL_CTRL)
 *            bit [5] Serial Flash / ROM Access Mode
 *                    0b0: 24 bits address mode
 *                    0b1: 32 bits address mode
 *                    
 *                    If user wants to use 32 bits address mode, user 
 *                    must manual send EX4B command (B7h) to serial 
 *                    flash then set this bit to 1.
 * @endverbatim
 *
 * @param None
 *
 * @note  None
 *
 * @result None
 */
void Panel_RA8889::Select_SFI_24bitAddress(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_SFL_CTRL);                  //0xb7, Serial Flash/ROM Controller Register (SFL_CTRL)
  temp = SPI_DataRead();
  temp &= cClrb5;                              //Reset bit 5
  SPI_DataWrite(temp);
}


/**
 * @brief Select Serial Flash/ROM Address 32-bit Mode
 *        
 * @verbatim                  
 * REG [0xb7] Serial Flash/ROM Controller Register (SFL_CTRL)
 *            bit [5] Serial Flash / ROM Access Mode
 *                    0b0: 24 bits address mode
 *                    0b1: 32 bits address mode
 *                    
 *                    If user wants to use 32 bits address mode, user 
 *                    must manual send EX4B command (B7h) to serial 
 *                    flash then set this bit to 1.
 * @endverbatim
 *
 * @param None
 *
 * @note  None
 *
 * @result None
 */
void Panel_RA8889::Select_SFI_32bitAddress(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_SFL_CTRL);                  //0xb7, Serial Flash/ROM Controller Register (SFL_CTRL)
  temp = SPI_DataRead();
  temp |= cSetb5;                              //Set bit 5
  SPI_DataWrite(temp);
}


/**
 * @brief Select Serial Flash/ROM for Single Data Without Dummy Cycles Normal Read Mode
 *        
 * @verbatim                  
 * REG [0xb6] Serial flash DMA Controller REG (DMA_CTRL)
 *            bit [7-6] 0b00: use [B7h] B3-0
 *                      0b01: 4x read command code – 6Bh.
 *                            Address output & data input interleaved 
 *                            on xmiso & xmosi & xsio2 & xsio3. 
 *                      0b10: 4x read command code – EBh.
 *                            Address output & data input interleaved 
 *                            on xmiso & xmosi & xsio2 & xsio3   
 * REG [0xb7] Serial Flash/ROM Controller Register (SFL_CTRL)
 *            bit [3-0] Read Command code & behavior selection
 *                      0b000x: 1x read command code – 03h. Normal read 
 *                      speed. Single data input on xmiso. Without 
 *                      dummy cycle between address and data.
 *                      
 *                      0b010x: 1x read command code – 0Bh. To some 
 *                      serial flash provide faster read speed. Single 
 *                      data input on xmiso. 8 dummy cycles inserted 
 *                      between address and data.
 *                      
 *                      0b1x0x: 1x read command code – 1Bh. To some 
 *                      serial flash provide fastest read speed. Single 
 *                      data input on xmiso. 16 dummy cycles inserted 
 *                      between address and data.
 *
 *                      0bxx10: 2x read command code – 3Bh. Interleaved 
 *                      data input on xmiso & xmosi. 8 dummy cycles 
 *                      inserted between address and data phase. (dual 
 *                      mode 0, reference Figure 16-7).
 *
 * Summary:
 *   000xb: 1x read command code = 03h. Without (0T) dummy cycle between address and data.
 *   010xb: 1x read command code = 0Bh. 8 dummy (8T) cycles inserted between address and data.
 *   1x0xb: 1x read command code = 1Bh. 16 dummy (16T) cycles inserted between address and data.
 *   xx10b: 2x read command code = 3Bh. 8 dummy cycles inserted between address and data phase. (mode 0)
 *   xx11b: 2x read command code = BBh. 4 dummy cycles inserted between address and data phase. (mode 1)
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
void Panel_RA8889::Select_SFI_SingleData_03h(void)
{
  uint8_t temp;

  SPI_CmdWrite(REG_DMA_CTRL);                  //0xb6, Serial flash DMA Controller REG (DMA_CTRL)
  temp = SPI_DataRead();                       
  temp &= (cClrb7 | cClrb6);                   //Use [0xb7] bit [3-0]
  SPI_DataWrite(temp);
  
  SPI_CmdWrite(REG_SFL_CTRL);                  //0xb7, Serial Flash/ROM Controller Register (SFL_CTRL)
  temp = SPI_DataRead();
  temp &= 0xF0;                                //Reset bit [3~0]
  SPI_DataWrite(temp);                         
}


/**
 * @brief Select Serial Flash/ROM for Single Data 8 Dummy Cycles Mode
 *        
 * @verbatim                  
 * REG [0xb6] Serial flash DMA Controller REG (DMA_CTRL)
 *            bit [7-6] 0b00: use [B7h] B3-0
 *                      0b01: 4x read command code – 6Bh.
 *                            Address output & data input interleaved 
 *                            on xmiso & xmosi & xsio2 & xsio3. 
 *                      0b10: 4x read command code – EBh.
 *                            Address output & data input interleaved 
 *                            on xmiso & xmosi & xsio2 & xsio3   
 * REG [0xb7] Serial Flash/ROM Controller Register (SFL_CTRL)
 *            bit [3-0] Read Command code & behavior selection
 *                      0b000x: 1x read command code – 03h. Normal read 
 *                      speed. Single data input on xmiso. Without 
 *                      dummy cycle between address and data.
 *                      
 *                      0b010x: 1x read command code – 0Bh. To some 
 *                      serial flash provide faster read speed. Single 
 *                      data input on xmiso. 8 dummy cycles inserted 
 *                      between address and data.
 *                      
 *                      0b1x0x: 1x read command code – 1Bh. To some 
 *                      serial flash provide fastest read speed. Single 
 *                      data input on xmiso. 16 dummy cycles inserted 
 *                      between address and data.
 *
 *                      0bxx10: 2x read command code – 3Bh. Interleaved 
 *                      data input on xmiso & xmosi. 8 dummy cycles 
 *                      inserted between address and data phase. (dual 
 *                      mode 0, reference Figure 16-7).
 *
 * Summary: 
 * 000xb: 1x read command code = 03h. Without (0T) dummy cycle between address and data.
 * 010xb: 1x read command code = 0Bh. 8 dummy (8T) cycles inserted between address and data.
 * 1x0xb: 1x read command code = 1Bh. 16 dummy (16T) cycles inserted between address and data.
 * xx10b: 2x read command code = 3Bh. 8 dummy cycles inserted between address and data phase. (mode 0)
 * xx11b: 2x read command code = BBh. 4 dummy cycles inserted between address and data phase. (mode 1)
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
 void Panel_RA8889::Select_SFI_SingleData_0Bh(void)
{
  uint8_t temp;

  SPI_CmdWrite(REG_DMA_CTRL);                  //0xb6, Serial flash DMA Controller REG (DMA_CTRL)
  temp = SPI_DataRead();                       
  temp &= (cClrb7 | cClrb6);                   //Use [0xb7] bit [3-0]
  SPI_DataWrite(temp);
  
  SPI_CmdWrite(REG_SFL_CTRL);                  //0xb7, Serial Flash/ROM Controller Register (SFL_CTRL)
  temp = SPI_DataRead();
  temp &= 0xF0;                                //Reset bit [3~0]
  temp |= cSetb2;                              //Set x read command code = 0Bh. 8 dummy cycles inserted between address and data.
  SPI_DataWrite(temp);                         
}


/**
 * @brief Select Serial Flash/ROM Single Data 16 Dummy Cycles Fast Read Mode
 *        
 * @verbatim                  
 * REG [0xb6] Serial flash DMA Controller REG (DMA_CTRL)
 *            bit [7-6] 0b00: use [B7h] B3-0
 *                      0b01: 4x read command code – 6Bh.
 *                            Address output & data input interleaved 
 *                            on xmiso & xmosi & xsio2 & xsio3. 
 *                      0b10: 4x read command code – EBh.
 *                            Address output & data input interleaved 
 *                            on xmiso & xmosi & xsio2 & xsio3   
 * REG [0xb7] Serial Flash/ROM Controller Register (SFL_CTRL)
 *            bit [3-0] Read Command code & behavior selection
 *                      0b000x: 1x read command code – 03h. Normal read 
 *                      speed. Single data input on xmiso. Without 
 *                      dummy cycle between address and data.
 *                      
 *                      0b010x: 1x read command code – 0Bh. To some 
 *                      serial flash provide faster read speed. Single 
 *                      data input on xmiso. 8 dummy cycles inserted 
 *                      between address and data.
 *                      
 *                      0b1x0x: 1x read command code – 1Bh. To some 
 *                      serial flash provide fastest read speed. Single 
 *                      data input on xmiso. 16 dummy cycles inserted 
 *                      between address and data.
 *
 *                      0bxx10: 2x read command code – 3Bh. Interleaved 
 *                      data input on xmiso & xmosi. 8 dummy cycles 
 *                      inserted between address and data phase. (dual 
 *                      mode 0, reference Figure 16-7).
 *
 *            Note: Not serial flash support above read command, please 
 *            according to serial flash’s datasheet to select proper read 
 *            command.
 *
 * Summary:
 * 000xb: 1x read command code = 03h. Without (0T) dummy cycle between address and data.
 * 010xb: 1x read command code = 0Bh. 8 dummy (8T) cycles inserted between address and data.
 * 1x0xb: 1x read command code = 1Bh. 16 dummy (16T) cycles inserted between address and data.
 * xx10b: 2x read command code = 3Bh. 8 dummy cycles inserted between address and data phase. (mode 0)
 * xx11b: 2x read command code = BBh. 4 dummy cycles inserted between address and data phase. (mode 1)
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
 void Panel_RA8889::Select_SFI_SingleData_1Bh(void)
{
  uint8_t temp;

  SPI_CmdWrite(REG_DMA_CTRL);                  //0xb6, Serial flash DMA Controller REG (DMA_CTRL)
  temp = SPI_DataRead();                       
  temp &= (cClrb7 | cClrb6);                   //Use [0xb7] bit [3-0]
  SPI_DataWrite(temp);

  SPI_CmdWrite(REG_SFL_CTRL);                  //0xb7, Serial Flash/ROM Controller Register (SFL_CTRL)
  temp = SPI_DataRead();
  temp &= 0xF0;                                //Reset bit [3~0]
  temp |= cSetb3;
  SPI_DataWrite(temp);
}


/**
 * @brief Select Serial Flash/ROM for Dual Data 8 Dummy Cycles Mode
 *        
 * @verbatim                  
 * REG [0xb6] Serial flash DMA Controller REG (DMA_CTRL)
 *            bit [7-6] 0b00: use [B7h] B3-0
 *                      0b01: 4x read command code – 6Bh.
 *                            Address output & data input interleaved 
 *                            on xmiso & xmosi & xsio2 & xsio3. 
 *                      0b10: 4x read command code – EBh.
 *                            Address output & data input interleaved 
 *                            on xmiso & xmosi & xsio2 & xsio3   
 * REG [0xb7] Serial Flash/ROM Controller Register (SFL_CTRL)
 *            bit [3-0] Read Command code & behavior selection
 *                      0b000x: 1x read command code – 03h. Normal read 
 *                      speed. Single data input on xmiso. Without 
 *                      dummy cycle between address and data.
 *                      
 *                      0b010x: 1x read command code – 0Bh. To some 
 *                      serial flash provide faster read speed. Single 
 *                      data input on xmiso. 8 dummy cycles inserted 
 *                      between address and data.
 *                      
 *                      0b1x0x: 1x read command code – 1Bh. To some 
 *                      serial flash provide fastest read speed. Single 
 *                      data input on xmiso. 16 dummy cycles inserted 
 *                      between address and data.
 *
 *                      0bxx10: 2x read command code – 3Bh. Interleaved 
 *                      data input on xmiso & xmosi. 8 dummy cycles 
 *                      inserted between address and data phase. (dual 
 *                      mode 0, reference Figure 16-7).
 *
 * Summary:
 * 000xb: 1x read command code = 03h. Without (0T) dummy cycle between address and data.
 * 010xb: 1x read command code = 0Bh. 8 dummy (8T) cycles inserted between address and data.
 * 1x0xb: 1x read command code = 1Bh. 16 dummy (16T) cycles inserted between address and data.
 * xx10b: 2x read command code = 3Bh. 8 dummy cycles inserted between address and data phase. (mode 0)
 * xx11b: 2x read command code = BBh. 4 dummy cycles inserted between address and data phase. (mode 1)
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
void Panel_RA8889::Select_SFI_DualData_3Bh(void)
{
  uint8_t temp;

  SPI_CmdWrite(REG_DMA_CTRL);                  //0xb6, Serial flash DMA Controller REG (DMA_CTRL)
  temp = SPI_DataRead();                       
  temp &= (cClrb7 | cClrb6);                   //Use [0xb7] bit [3-0]
  SPI_DataWrite(temp);

  SPI_CmdWrite(REG_SFL_CTRL);                  //0xb7, Serial Flash/ROM Controller Register (SFL_CTRL)
  temp = SPI_DataRead();
  temp &= 0xF0;                                //Reset bit [3~0]
  temp |= cSetb1;                              //
  SPI_DataWrite(temp);
}


/**
 * @brief Select Serial Flash/ROM for Dual Data 4 Dummy Cycles Mode
 *        
 * @verbatim                  
 * REG [0xb6] Serial flash DMA Controller REG (DMA_CTRL)
 *            bit [7-6] 0b00: use [B7h] B3-0
 *                      0b01: 4x read command code – 6Bh.
 *                            Address output & data input interleaved 
 *                            on xmiso & xmosi & xsio2 & xsio3. 
 *                      0b10: 4x read command code – EBh.
 *                            Address output & data input interleaved 
 *                            on xmiso & xmosi & xsio2 & xsio3   
 * REG [0xb7] Serial Flash/ROM Controller Register (SFL_CTRL)
 *            bit [3-0] Read Command code & behavior selection
 *                      0b000x: 1x read command code – 03h. Normal read 
 *                      speed. Single data input on xmiso. Without 
 *                      dummy cycle between address and data.
 *                      
 *                      0b010x: 1x read command code – 0Bh. To some 
 *                      serial flash provide faster read speed. Single 
 *                      data input on xmiso. 8 dummy cycles inserted 
 *                      between address and data.
 *                      
 *                      0b1x0x: 1x read command code – 1Bh. To some 
 *                      serial flash provide fastest read speed. Single 
 *                      data input on xmiso. 16 dummy cycles inserted 
 *                      between address and data.
 *
 *                      0bxx10: 2x read command code – 3Bh. Interleaved 
 *                      data input on xmiso & xmosi. 8 dummy cycles 
 *                      inserted between address and data phase. (dual 
 *                      mode 0, reference Figure 16-7).
 *
 * Summary:
 * 000xb: 1x read command code = 03h. Without (0T) dummy cycle between address and data.
 * 010xb: 1x read command code = 0Bh. 8 dummy (8T) cycles inserted between address and data.
 * 1x0xb: 1x read command code = 1Bh. 16 dummy (16T) cycles inserted between address and data.
 * xx10b: 2x read command code = 3Bh. 8 dummy cycles inserted between address and data phase. (mode 0)
 * xx11b: 2x read command code = BBh. 4 dummy cycles inserted between address and data phase. (mode 1)
 * @endverbatim
 *
 * @param None
 *
 * @note Apenas aplicado ao RA8876 e RA8877, esse modo 1 BBh não existe no RA8889
 *
 * @result None
 */
void Panel_RA8889::Select_SFI_DualData_BBh(void)
{
  uint8_t temp;

  SPI_CmdWrite(REG_DMA_CTRL);                  //0xb6, Serial flash DMA Controller REG (DMA_CTRL)
  temp = SPI_DataRead();                       
  temp &= (cClrb7 | cClrb6);                   //Use [0xb7] bit [3-0]
  SPI_DataWrite(temp);

  SPI_CmdWrite(REG_SFL_CTRL);                  //0xb7, Serial Flash/ROM Controller Register (SFL_CTRL)
  temp = SPI_DataRead();
  temp &= 0xF0;                                //Reset bit [3~0]
  temp |= (cSetb1 | cSetb0);
  SPI_DataWrite(temp);
}


/**
 * @brief Select Serial Flash/ROM for Quad Data 8 Dummy Cycles Mode
 *        
 * @verbatim                  
 * REG [0xb6] Serial flash DMA Controller REG (DMA_CTRL)
 *            bit [7-6] 0b00: use [B7h] B3-0
 *                      0b01: 4x read command code – 6Bh.
 *                            Address output & data input interleaved 
 *                            on xmiso & xmosi & xsio2 & xsio3. 
 *                      0b10: 4x read command code – EBh.
 *                            Address output & data input interleaved 
 *                            on xmiso & xmosi & xsio2 & xsio3   
 *
 * Summary:
 *   01b: 4x read command code – 6Bh.
 *   10b: 4x read command code – EBh.
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
void Panel_RA8889::Select_SFI_QuadData_6Bh(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_DMA_CTRL);                  //0xb6, Serial flash DMA Controller REG (DMA_CTRL)
  temp = SPI_DataRead();                       
  temp &= 0x3F;
  temp |= cSetb6;
  SPI_DataWrite(temp);
}


/**
 * @brief Select Serial Flash/ROM for Quad Data 4 Dummy Cycles Mode
 *        
 * @verbatim                  
 * REG [0xb6] Serial flash DMA Controller REG (DMA_CTRL)
 *            bit [7-6] 0b00: use [B7h] B3-0
 *                      0b01: 4x read command code – 6Bh.
 *                            Address output & data input interleaved 
 *                            on xmiso & xmosi & xsio2 & xsio3. 
 *                      0b10: 4x read command code – EBh.
 *                            Address output & data input interleaved 
 *                            on xmiso & xmosi & xsio2 & xsio3   
 * 
 * Summary:
 *   01b: 4x read command code – 6Bh.
 *   10b: 4x read command code – EBh.
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
void Panel_RA8889::Select_SFI_QuadData_EBh(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_DMA_CTRL);                  //0xb6, Serial flash DMA Controller REG (DMA_CTRL)
  temp = SPI_DataRead();                       
  temp &= 0x3F;
  temp |= cSetb7;
  SPI_DataWrite(temp);
}


/**
 * @brief Serial Flash/ROM 0 I/F is Selected
 *        
 * @verbatim                  
 * REG [0xb7] Serial flash DMA Controller REG (DMA_CTRL)
 *            bit [7] Page 0 FONT/DMA Serial Flash/ROM I/F # Select
 *                    0b0: Serial Flash/ROM 0 I/F is selected.
 *                    0b1: Serial Flash/ROM 1 I/F is selected.
 *                    Note: when page1 B7h bit 7 = 1, then serial flash 
 *                    chip select 2,3
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
void Panel_RA8889::SFI_Select_ROM0(void)
{
  SPI_CmdWrite(REG_SFL_CTRL);                  //0xb7, Serial Flash/ROM Controller Register (SFL_CTRL)
  temp = SPI_DataRead();
  temp &= cClrb7;
  SPI_DataWrite(temp);
}


/**
 * @brief Serial Flash/ROM 1 I/F is Selected
 *        
 * @verbatim                  
 * REG [0xb7] Serial flash DMA Controller REG (DMA_CTRL)
 *            bit [7] Page 0 FONT/DMA Serial Flash/ROM I/F # Select
 *                    0b0: Serial Flash/ROM 0 I/F is selected.
 *                    0b1: Serial Flash/ROM 1 I/F is selected.
 *                    Note: when page1 B7h bit 7 = 1, then serial flash 
 *                    chip select 2,3
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
void Panel_RA8889::SFI_Select_ROM1(void)
{
  SPI_CmdWrite(REG_SFL_CTRL);                  //0xb7, Serial Flash/ROM Controller Register (SFL_CTRL)
  temp = SPI_DataRead();
  temp |= cSetb7;
  SPI_DataWrite(temp);
}


/**
 * @brief IDEC Serial Interface Standard Mode 0 or 3
 *        
 * @verbatim                  
 * [0xb7] Serial flash DMA Controller REG (DMA_CTRL)
 *            bit [7] IDEC Serial Interface
 *                    0: standard SPI mode 0 or mode 3 timing
 *                    1: Follow RA8875 mode 0 & mode 3 timing
 * @endverbatim
 *
 * @param None
 *
 * @note Only for RA8876/RA8887. No RA8889, este bit [5-1] é não atribuido (NA).
 *       Mantem o modo intercompartibildaide sem gerar excecao.
 *       Se em RA8887/RA8876 a traca de pagina não existe pelo REG[46h], logo não fará efeito, passando para a execução
 *       dos registradores seguitnes [B7h] bit 4.
 *       Se em RA8889 [REG PAGE 1],[B7h] [bit4] não tem atribuição este bit é sempre 1.
 *       Se em RA8876/RA8877 Não existe troca de páginas 0 e 1 de registradores, acessados pelos registradores [46-4E].
 *
 * @result None
 */
void Panel_RA8889::IDEC_SPI_Select_StandardMode0orMode3(void)
{
  uint8_t temp;

  //Apenas para compatibilidade. Este bit nao existe no RA8889, 
  //será ignorado. No RA8876/RA8877 esta função de troca de pagina 
  //REG [46h-4eh] é reservado (sem funcao), mas o codigo abaixo para a 
  //escolha do bit 4 sim
  PageSwitch(ePageReg::Page1);                  //Troca para a Pagina 1 de registradores do RA8889, mas não existe no RA8876/RA8877
                                       
  //Acessar o registrador SFL_CTRL do RA8876/RA8877
  LCD_CmdWrite(REG_SFL_CTRL);                  //0xb7, Serial Flash/ROM Controller Register (SFL_CTRL) do RA8876/RA8877
  temp = SPI_DataRead();                       
  temp &= cClrb4;                              
  SPI_DataWrite(temp);                         
									           
  PageSwitch(ePageReg::Page0);                  //Retorna para a Pagina 0 de registradores
}


/**
 * @brief IDEC Serial Interface Mode 0 and 3
 *        
 * @verbatim                  
 * [0xb7] Serial flash DMA Controller REG (DMA_CTRL)
 *            bit [7] IDEC Serial Interface
 *                    0: standard SPI mode 0 or mode 3 timing
 *                    1: Follow RA8875 mode 0 & mode 3 timing
 * @endverbatim
 *
 * @param None
 *
 * @note Only for RA8876/RA8887. No RA8889, PAGE 1, REG 0xB7 bit [5-1] é não atribuido (NA).
 *       Mantem o modo intercompartibildaide sem gerar excecao.
 *       Se em RA8887/RA8876 a troca de pagina não existe pelo REG[46h], logo não fará efeito, passando para a execução
 *       dos registradores seguitnes [B7h] bit 4.
 *       Se em RA8889 [REG PAGE 1],[B7h] [bit4] não tem atribuição este bit é sempre 1.
 *       Se em RA8876/RA8877 Não existe troca de páginas 0 e 1 de registradores, acessados pelos registradores [46-4E].
 *
 * @result None
 */
void Panel_RA8889::IDEC_RA8875_SPI_Select_Mode0andMode3(void)
{
  uint8_t temp;

  //Apenas para compatibilidade. Este bit nao existe no RA8889, 
  //será ignorado. No RA8876/RA8877 esta função de troca de pagina 
  //REG [46h-4eh] é reservado (sem funcao), mas o codigo abaixo para a 
  //escolha do bit 4 sim
  PageSwitch(ePageReg::Page1);                  //Troca para a Pagina 1 de registradores do RA8889, mas não existe no RA8876/RA8877
                                       
  //Acessar o registrador SFL_CTRL do RA8876/RA8877
  LCD_CmdWrite(REG_SFL_CTRL);                  //0xb7, Serial Flash/ROM Controller Register (SFL_CTRL) do RA8876/RA8877
  temp = SPI_DataRead();                       
  temp |= cClrb4;                              
  SPI_DataWrite(temp);                         
									           
  PageSwitch(ePageReg::Page0);                  //Retorna para a Pagina 0 de registradores
}


/**
 * @brief Serial Flash/ROM Waveform Mode 0
 *        
 * @verbatim                  
 * [0xb7] Serial Flash/ROM Controller Register (SFL_CTRL)
 *            bit [4] RA8875 compatible mode
 *                    0: standard SPI mode 0 or mode 3 timing
 *                    1: Follow RA8875 mode 0 & mode 3 timing
 *                    
 *                    Serial Flash/ROM Waveform Mode
 *                    In the RA8875 compatible mode,
 *                    
 *                    Data are read on the clock's falling edge (high->low transition) and data are changed on a falling edge (high->low transition).
 *                    0: For Mode 0, SPI clock park on low when idle.
 *                    1: For Mode 3, SPI clock park on high when idle.
 * @endverbatim
 *
 * @param None
 *
 * @note Only for RA8876/RA8887. No RA8889, PAGE 0, REG 0xB7 bit [4] é sempre 1.
 *       Apartir do RA8886/RA8887 modo compatibildiade com o RA8875.
 *
 * @result None
 */
void Panel_RA8889::SFI_Select_WaveformMode0(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_SFL_CTRL);                          //0xb7, RA8876/RA8877, Serial Flash/ROM Controller Register (SFL_CTRL)
  temp = SPI_DataRead();
  temp &= cClrb4;
  SPI_DataWrite(temp);
}


/**
 * @brief Serial Flash/ROM Waveform Mode 3
 *        
 * @verbatim                  
 * [0xb7] Serial Flash/ROM Controller Register (SFL_CTRL)
 *            bit [4] RA8875 compatible mode
 *                    0: standard SPI mode 0 or mode 3 timing
 *                    1: Follow RA8875 mode 0 & mode 3 timing
 *                    
 *                    Serial Flash/ROM Waveform Mode
 *                    In the RA8875 compatible mode,
 *                    
 *                    Data are read on the clock's falling edge (high->low transition) and data are changed on a falling edge (high->low transition).
 *                    0: For Mode 0, SPI clock park on low when idle.
 *                    1: For Mode 3, SPI clock park on high when idle.
 * @endverbatim
 *
 * @param None
 *
 * @note Only for RA8876/RA8887. No RA8889, PAGE 0, REG 0xB7 bit [4] é sempre 1.
 *       Apartir do RA8886/RA8887 modo compatibildiade com o RA8875.
 *
 * @result None
 */
void Panel_RA8889::SFI_Select_WaveformMode3(void)
{
  unsigned char temp;
  SPI_CmdWrite(REG_SFL_CTRL);                          //0xb7, RA8876/RA8877, Serial Flash/ROM Controller Register (SFL_CTRL)
  temp = ER_TFT.LCD_DataRead();
  temp |= cSetb4;
  SPI_DataWrite(temp);
}


//================================================================================
//
// [0xB8] SPI master Tx /Rx FIFO Data Register (SPIDR)
//
//================================================================================


/**
 * @brief SPI master Tx /Rx FIFO Get Data
 *        
 * @verbatim                  
 * REG [0xb8] SPI master Tx /Rx FIFO Data Register (SPIDR)
 *            bit [7-0] SPI master Tx /Rx FIFO Data Register
 *            
 *            After programming the core’s control register SPI 
 *            transfers can be initiated. A transfer is initiated by 
 *            writing to the Serial Peripheral Data Register [SPIDR]. 
 *            Writing to the Serial Peripheral Data Register is 
 *            actually writing to a 16 entries deep FIFO called the 
 *            Write FIFO. Each write access adds a data byte to the 
 *            Write FIFO. When the core is enabled – SS_ACTIVE is set 
 *            to 1 and the Write FIFO is not full, the core 
 *            automatically transfers the oldest data byte.
 *            
 *            Receiving data is done simultaneously with transmitting 
 *            data; whenever a data byte is transmitted a data byte is 
 *            received. For each byte that needs to be read from a 
 *            device, a dummy byte needs to be written to the Write 
 *            FIFO. This instructs the core to initiate an SPI 
 *            transfer, simultaneously transmitting the dummy byte and 
 *            receiving the desired data. Whenever a transfer is 
 *            finished, the received data byte is added to the Read 
 *            FIFO. The Read FIFO is the counterpart of the Write FIFO. 
 *            It is an independent 16 entries deep FIFO. The FIFO 
 *            contents can be read by reading from the Serial 
 *            Peripheral Data Register [SPIDR].                          
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
uint8_t Panel_RA8889::SPIM_TxRxFIFOData_Get(void)
{
  uint8_t temp;
  while (Rx_FIFO_Empty_Flag() == 1);          //If it is not empty, execute it.
  SPI_CmdWrite(REG_SPIDR);                   //0xb8, SPI master Tx /Rx FIFO Data Register (SPIDR)
  temp = SPI_DataRead();
  // while(Rx_FIFO_full_flag());             //Required only when writing 16 records continuously
  return temp;
}


/**
 * @brief SPI master Tx /Rx FIFO Put Data
 *        
 * @verbatim                  
 * REG [0xb8] SPI master Tx /Rx FIFO Data Register (SPIDR)
 *            bit [7-0] SPI master Tx /Rx FIFO Data Register
 *            
 *            After programming the core’s control register SPI 
 *            transfers can be initiated. A transfer is initiated by 
 *            writing to the Serial Peripheral Data Register [SPIDR]. 
 *            Writing to the Serial Peripheral Data Register is 
 *            actually writing to a 16 entries deep FIFO called the 
 *            Write FIFO. Each write access adds a data byte to the 
 *            Write FIFO. When the core is enabled – SS_ACTIVE is set 
 *            to 1 and the Write FIFO is not full, the core 
 *            automatically transfers the oldest data byte.
 *            
 *            Receiving data is done simultaneously with transmitting 
 *            data; whenever a data byte is transmitted a data byte is 
 *            received. For each byte that needs to be read from a 
 *            device, a dummy byte needs to be written to the Write 
 *            FIFO. This instructs the core to initiate an SPI 
 *            transfer, simultaneously transmitting the dummy byte and 
 *            receiving the desired data. Whenever a transfer is 
 *            finished, the received data byte is added to the Read 
 *            FIFO. The Read FIFO is the counterpart of the Write FIFO. 
 *            It is an independent 16 entries deep FIFO. The FIFO 
 *            contents can be read by reading from the Serial 
 *            Peripheral Data Register [SPIDR].                          
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
uint8_t Panel_RA8889::SPIM_TxRxFIFOData_Put(uint8_t data)
{
  uint8_t temp;
  SPI_CmdWrite(REG_SPIDR);                   //0xb8, SPI master Tx /Rx FIFO Data Register (SPIDR)
  SPI_DataWrite(data);
  while (Tx_FIFO_Empty_Flag() == 0);
  temp = SPIM_TxRxFIFOData_Get();
  return temp;
}


//================================================================================
//
// [0xB9] SPI master Control Register (SPIMCR2)
//
//================================================================================


/**
 * @brief Control Slave Select drive on which xnsfcs0..xnsfcs3 Channel
 *        
 * @verbatim                  
 * Controle de acionamento do sinal de seleção do escravo (xnsfcs0..xnsfcs3)
 *
 * REG [0xb9] SPI master Control Register (SPIMCR2)
 *            bit [5, 7] Control Slave Select drive on which xnsfcs
 *                       B7 and B5 = 00b: nSS drive on xnsfcs[0]
 *                       B7 and B5 = 01b: nSS drive on xnsfcs[1]
 *                       B7 and B5 = 10b: nSS drive on xnsfcs[2]
 *                       B7 and B5 = 11b: nSS drive on xnsfcs[3]
 * @endverbatim
 *
 * @param eNSS_Channel::XNSFCS0
 *        eNSS_Channel::XNSFCS1
 *        eNSS_Channel::XNSFCS2
 *        eNSS_Channel::XNSFCS3
 *
 * @note None
 *
 * @result None
 */
void Panel_RA8889::nSS_Select_Channel(eNSS_Channel channel)
{
  uint8_t temp;
  SPI_CmdWrite(REG_SPIMCR2);                   //0xb9, SPI master Control Register (SPIMCR2)
  temp = SPI_DataRead();
  temp &= ~( cSetb7 | cSetb5 );            //Reset bits 7 and 5
  // aplica valor do canal (2 bits: b7:b5)
  if (static_cast<uint8_t>(channel) & 0x02) temp |= (cSetb7);  // bit1 → b7
  if (static_cast<uint8_t>(channel) & 0x01) temp |= (cSetb5);  // bit0 → b5
  SPI_DataWrite(temp);
}


/**
 * @brief Interrupt SPI Master Enable/Disable
 *        
 * @verbatim                  
 * REG [0xb9] SPI master Control Register (SPIMCR2)
 *            bit [6] SPI Master Interrupt enable
 *                    0b0: Disable interrupt.
 *                    0b1: Enable interrupt.
 *            *** If you disable SPIM interrupt flag, then RA8889 won’t 
 *            assert interrupt event to inform MPU but you still may 
 *            check interrupt event on SPIMSR.
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
void Panel_RA8889::Interrupt_SPIM_Enable(bool b)
{
  uint8_t temp;
  SPI_CmdWrite(REG_SPIMCR2);                   //0xb9, SPI master Control Register (SPIMCR2)
  temp = SPI_DataRead();
  b ? temp |= cSetb6 : temp &= cClrb6;
  SPI_DataWrite(temp);
}


/**
 * @brief Slave Select Singal Inactive (nSS Port)
 *        
 * @verbatim                  
 * REG [0xb9] SPI master Control Register (SPIMCR2)
 *            bit [4] Slave Select signal active [SS_ACTIVE]
 *                    0b0: inactive (nSS port will goes high)
 *                    0b1: active (nSS port will goes low)
 *            While Slave Select signal be in inactive, FIFO will be 
 *            cleared and this function will stay in Idle state. Note: 
 *            Do not change CPOL/CPHA when Slave Select signal is 
 *            active.
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
void Panel_RA8889::nSS_Inactive(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_SPIMCR2);                   //0xb9, SPI master Control Register (SPIMCR2)
  temp = SPI_DataRead();
  temp &= cClrb4;
  SPI_DataWrite(temp);
}


/**
 * @brief Slave Select Singal Inactive (nSS Port)
 *        
 * @verbatim                  
 * REG [0xb9] SPI master Control Register (SPIMCR2)
 *            bit [4] Slave Select signal active [SS_ACTIVE]
 *                    0b0: inactive (nSS port will goes high)
 *                    0b1: active (nSS port will goes low)
 *            While Slave Select signal be in inactive, FIFO will be 
 *            cleared and this function will stay in Idle state. Note: 
 *            Do not change CPOL/CPHA when Slave Select signal is 
 *            active.
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
void Panel_RA8889::nSS_Active(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_SPIMCR2);                   //0xb9, SPI master Control Register (SPIMCR2)
  temp = SPI_DataRead();
  temp |= cSetb4;
  SPI_DataWrite(temp);
}


/**
 * @brief interrupt Enable/Disable for FIFO overflow error
 *        
 * @verbatim                  
 * REG [0xb9] SPI master Control Register (SPIMCR2)
 *            bit [3] Mask interrupt for FIFO overflow error [OVFIRQEN]
 *                    0b0: unmask
 *                    0b1: mask
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
void Panel_RA8889::Interrupt_FIFOOverflow_Enable(bool b)
{
  uint8_t temp;
  SPI_CmdWrite(REG_SPIMCR2);                   //0xb9, SPI master Control Register (SPIMCR2)
  temp = SPI_DataRead();
  b ? temp |= cSetb3 : temp &= cClrb3;
  SPI_DataWrite(temp);
}


/**
 * @brief Mask interrupt for while Tx FIFO empty & SPI engine/FSM idle
 *        
 * @verbatim                  
 * REG [0xb9] SPI master Control Register (SPIMCR2)
 *            bit [2] Mask interrupt for while Tx FIFO empty & SPI engine/FSM idle [EMTIRQEN]
 *                    0b0: unmask
 *                    0b1: mask
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
void Panel_RA8889::Interrupt_EMTIRQEN_Enable(bool b)
{
  uint8_t temp;
  SPI_CmdWrite(REG_SPIMCR2);                   //0xb9, SPI master Control Register (SPIMCR2)
  temp = SPI_DataRead();
  b ? temp |= cSetb2 : temp &= cClrb2;
  SPI_DataWrite(temp);
}


/**
 * @brief Reset Clock Polarity (CPOL)
 *        
 * @verbatim                  
 * REG [0xb9] SPI master Control Register (SPIMCR2)
 *            bit [1-0] SPI operation mode
 *            Only support mode 0 & mode 3, when enable DMA function or 
 *            access Getop’s CI character serial ROM device.
 *            
 *            mode   CPOL:Clock Polarity bit    CPHA:Clock Phase bit
 *            ------------------------------------------------------
 *            0               0                          0
 *            1               0                          1
 *            2               1                          0
 *            3               1                          1
 *
 * At CPOL=0 the base value of the clock is zero   
 *   o  For CPHA=0, data are read on the clock's rising edge (low->high 
 *      transition) and data are changed on a falling edge (high->low 
 *      clock transition). 
 *   o  For CPHA=1, data are read on the clock's falling edge and data 
 *      are changed on a rising edge. 
 * 
 * At CPOL=1 the base value of the clock is one (inversion of CPOL=0)   
 *   o  For CPHA=0, data are read on clock's falling edge and data are 
 *      changed on a rising edge. 
 *   o  For CPHA=1, data are read on clock's rising edge and data are 
 *      changed on a falling edge.
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
void Panel_RA8889::Reset_CPOL(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_SPIMCR2);                   //0xb9, SPI master Control Register (SPIMCR2)
  temp = SPI_DataRead();
  temp &= cClrb1;
  SPI_DataWrite(temp);
}


/**
 * @brief Set Clock Polarity (CPOL)
 *        
 * @verbatim                  
 * REG [0xb9] SPI master Control Register (SPIMCR2)
 *            bit [1-0] SPI operation mode
 *            Only support mode 0 & mode 3, when enable DMA function or 
 *            access Getop’s CI character serial ROM device.
 *            
 *            mode   CPOL:Clock Polarity bit    CPHA:Clock Phase bit
 *            ------------------------------------------------------
 *            0               0                          0
 *            1               0                          1
 *            2               1                          0
 *            3               1                          1
 *
 * At CPOL=0 the base value of the clock is zero   
 *   o  For CPHA=0, data are read on the clock's rising edge (low->high 
 *      transition) and data are changed on a falling edge (high->low 
 *      clock transition). 
 *   o  For CPHA=1, data are read on the clock's falling edge and data 
 *      are changed on a rising edge. 
 * 
 * At CPOL=1 the base value of the clock is one (inversion of CPOL=0)   
 *   o  For CPHA=0, data are read on clock's falling edge and data are 
 *      changed on a rising edge. 
 *   o  For CPHA=1, data are read on clock's rising edge and data are 
 *      changed on a falling edge.
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
void Panel_RA8889::Set_CPOL(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_SPIMCR2);                   //0xb9, SPI master Control Register (SPIMCR2)
  temp = SPI_DataRead();
  temp |= cSetb1;
  SPI_DataWrite(temp);
}


/**
 * @brief Reset Clock Phase (CPHA)
 *        
 * @verbatim                  
 * REG [0xb9] SPI master Control Register (SPIMCR2)
 *            bit [1-0] SPI operation mode
 *            Only support mode 0 & mode 3, when enable DMA function or 
 *            access Getop’s CI character serial ROM device.
 *            
 *            mode   CPOL:Clock Polarity bit    CPHA:Clock Phase bit
 *            ------------------------------------------------------
 *            0               0                          0
 *            1               0                          1
 *            2               1                          0
 *            3               1                          1
 *
 * At CPOL=0 the base value of the clock is zero   
 *   o  For CPHA=0, data are read on the clock's rising edge (low->high 
 *      transition) and data are changed on a falling edge (high->low 
 *      clock transition). 
 *   o  For CPHA=1, data are read on the clock's falling edge and data 
 *      are changed on a rising edge. 
 * 
 * At CPOL=1 the base value of the clock is one (inversion of CPOL=0)   
 *   o  For CPHA=0, data are read on clock's falling edge and data are 
 *      changed on a rising edge. 
 *   o  For CPHA=1, data are read on clock's rising edge and data are 
 *      changed on a falling edge.
 *
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
void Panel_RA8889::Reset_CPHA(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_SPIMCR2);                   //0xb9, SPI master Control Register (SPIMCR2)
  temp = SPI_DataRead();
  temp &= cClrb0;
  SPI_DataWrite(temp);
}


/**
 * @brief Set Clock Phase (CPHA)
 *        
 * @verbatim                  
 * REG [0xb9] SPI master Control Register (SPIMCR2)
 *            bit [1-0] SPI operation mode
 *            Only support mode 0 & mode 3, when enable DMA function or 
 *            access Getop’s CI character serial ROM device.
 *            
 *            mode   CPOL:Clock Polarity bit    CPHA:Clock Phase bit
 *            ------------------------------------------------------
 *            0               0                          0
 *            1               0                          1
 *            2               1                          0
 *            3               1                          1
 *
 * At CPOL=0 the base value of the clock is zero   
 *   o  For CPHA=0, data are read on the clock's rising edge (low->high 
 *      transition) and data are changed on a falling edge (high->low 
 *      clock transition). 
 *   o  For CPHA=1, data are read on the clock's falling edge and data 
 *      are changed on a rising edge. 
 * 
 * At CPOL=1 the base value of the clock is one (inversion of CPOL=0)   
 *   o  For CPHA=0, data are read on clock's falling edge and data are 
 *      changed on a rising edge. 
 *   o  For CPHA=1, data are read on clock's rising edge and data are 
 *      changed on a falling edge.
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
void Panel_RA8889::Set_CPHA(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_SPIMCR2);                   //0xb9, SPI master Control Register (SPIMCR2)
  temp = SPI_DataRead();
  temp |= cSetb0;
  SPI_DataWrite(temp);
}


//================================================================================
//
// [0xBA] SPI master Status Register (SPIMSR)
//
//================================================================================


/**
 * @brief Tx FIFO empty flag from SPI master
 *        
 * @verbatim                  
 * REG [0xba] SPI master Status Register (SPIMSR)
 *            bit [7] Tx FIFO empty flag
 *                    0: not empty
 *                    1: empty
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result true: is empty 
 */
bool Panel_RA8889::SPIM_TxFIFO_Empty(void)
{
  SPI_CmdWrite(REG_SPIMSR);                    //0xba, SPI master Status Register (SPIMSR)
  return (SPI_DataRead() & 0x80);              //Check bit 7
}


/**
 * @brief Tx FIFO full flag from SPI master
 *        
 * @verbatim                  
 * REG [0xba] SPI master Status Register (SPIMSR)
 *            bit [6] Tx FIFO full flag
 *                    0: not full
 *                    1: full
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result true: is full 
 */
bool Panel_RA8889::SPIM_TxFIFO_Full(void)
{
  SPI_CmdWrite(REG_SPIMSR);                    //0xba, SPI master Status Register (SPIMSR)
  return (SPI_DataRead() & 0x40);              //Check bit 6
} 


/**
 * @brief Rx FIFO empty flag from SPI master
 *        
 * @verbatim                  
 * REG [0xba] SPI master Status Register (SPIMSR)
 *            bit [5] Rx FIFO empty flag
 *                    0: not empty
 *                    1: empty
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result true: is empty 
 */
bool Panel_RA8889::SPIM_RxFIFO_Empty(void)
{
  SPI_CmdWrite(REG_SPIMSR);                    //0xba, SPI master Status Register (SPIMSR)
  return (SPI_DataRead() & 0x20);              //Check bit 5
} 


/**
 * @brief Rx FIFO full flag from SPI master
 *        
 * @verbatim                  
 * REG [0xba] SPI master Status Register (SPIMSR)
 *            bit [4] Rx FIFO full flag
 *                    0: not full
 *                    1: full
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result true: is full 
 */
bool Panel_RA8889::SPIM_RxFIFO_Full(void)
{
  SPI_CmdWrite(REG_SPIMSR);                    //0xba, SPI master Status Register (SPIMSR)
  return (SPI_DataRead() & 0x10);              //Check bit 4
} 


/**
 * @brief Occur Overflow interrupt flag
 *        
 * @verbatim                  
 * REG [0xba] SPI master Status Register (SPIMSR)
 *            bit [3] Overflow interrupt flag
 *            Read: 
 *            0: No Overflow
 *            1: Overflow interrupt flag
 *            Write
 *            1: will clear this flag
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result true: occur overflow
 */
bool Panel_RA8889::Interrupt_Overflow_Flag(void)
{
  SPI_CmdWrite(REG_SPIMSR);                    //0xba, SPI master Status Register (SPIMSR)
  return (SPI_DataRead() & 0x08);              //Occur Overflow Interrupt
}


/**
 * @brief Clear Overflow interrupt flag
 *        
 * @verbatim                  
 * REG [0xba] SPI master Status Register (SPIMSR)
 *            bit [3] Overflow interrupt flag
 *            Read: 
 *            0: No Overflow
 *            1: Overflow interrupt flag
 *            Write
 *            1: will clear this flag
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
void Panel_RA8889::Interrupt_ClearOverflow_Flag(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_SPIMSR);                    //0xba, SPI master Status Register (SPIMSR)
  temp = SPI_DataRead();
  temp |= cSetb3;
  SPI_DataWrite(temp);
}


/**
 * @brief Occur Tx FIFO empty /FSM idle interrupt flag
 *        
 * @verbatim                  
 * REG [0xba] SPI master Status Register (SPIMSR)
 *            bit [2] Tx FIFO empty /FSM idle interrupt flag
 *            Read: 
 *            0: No interrupt flag
 *            1: interrupt flag
 *            Write
 *            1: will clear this flag
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result true: occur Tx FIFO empty /FSM idle
 */
bool Panel_RA8889::EMTI_Flag(void)
{
  SPI_CmdWrite(REG_SPIMSR);                    //0xba, SPI master Status Register (SPIMSR)
  return (SPI_DataRead() & 0x04);              //occur Tx FIFO empty /FSM idle interrupt flag
}


/**
 * @brief Clear Tx FIFO empty /FSM idle interrupt flag
 *        
 * @verbatim                  
 * REG [0xba] SPI master Status Register (SPIMSR)
 *            bit [2] Tx FIFO empty /FSM idle interrupt flag
 *            Read: 
 *            0: No interrupt flag
 *            1: interrupt flag
 *            Write
 *            1: will clear this flag
 * @endverbatim
 *
 * @param None
 *
 * @note None
 *
 * @result None
 */
void Panel_RA8889::EMTI_Clear_Flag(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_SPIMSR);                    //0xba, SPI master Status Register (SPIMSR)
  temp = SPI_DataRead();
  temp |= cSetb2;
  SPI_DataWrite(temp);
}



//================================================================================
//
// [0xBB] SPI Clock period (SPI_DIVSOR)
//
//================================================================================


/**
 * @brief SPI Clock period
 *        
 * @verbatim                  
 * REG [0xbb] SPI Clock period (SPI_DIVSOR)
 *            bit [7-0] SPI Clock period (default Fsck=3)
 *            According to system clock to set low & high period for SPI clock. 
 *             
 *            SPI Master:
 *              Fsck = Fcore / (divisor * 2)
 *            Serial Flash:
 *              Fsck = Fcore / (divisor * 2)
 *            When SPI_DIVSOR = 0,
 *              Fsck = Fcore
 * @endverbatim
 *
 * @param divisor: valor do divisor para o ajuste de frequencia do periodo de clock do SPI
 *
 * @note None
 *
 * @result None
 */
void Panel_RA8889::SPI_Clock_Period(uint8_t divisor)
{
  SPI_CmdWrite(REG_SPI_DIVSOR);                //0xbb, SPI Clock period (SPI_DIVSOR)
  SPI_DataWrite(divisor);
} 


//================================================================================
//
// [0xBC] Serial flash DMA Source Starting Address 0 (DMA_SSTR0)
// [0xBD] Serial flash DMA Source Starting Address 1 (DMA_SSTR1)
// [0xBE] Serial flash DMA Source Starting Address 2 (DMA_SSTR2)
// [0xBF] Serial flash DMA Source Starting Address 3 (DMA_SSTR3)
//
//================================================================================


/**
 * @brief     Configura o endereço inicial de origem de leitura da Flash Serial DMA
 *            Serial Flash I/F DMA Source Starting Address
 *        
 * @verbatim
 *
 * Esta função define o endereço inicial de onde o RA8889 irá ler os dados
 * da memória serial flash quando um DMA for iniciado. O endereço é de 32 bits,
 * dividido em quatro registradores consecutivos (DMA_SSTR0 ~ DMA_SSTR3).
 *
 * REG [0xbc] Serial flash DMA Source Starting Address 0 (DMA_SSTR0)
 *            bit [7-0] Serial flash DMA Source START ADDRESS [7:0]
 *            This register sets the start address [7:0] of serial flash memory.
 *            Directly specify the start address of source image.
 *             
 * REG [0xbd] Serial flash DMA Source Starting Address 1 (DMA_SSTR1)
 *            bit [7-0] Serial flash DMA Source START ADDRESS [15:8]
 *            This register sets the start address [15:8] of serial flash memory.
 *            Directly specify the start address of source image.
 *            
 * REG [0xbe] Serial flash DMA Source Starting Address 2 (DMA_SSTR2)
 *            bit [7-0] Serial flash DMA Source START ADDRESS [23:16]
 *            This register sets the start address [23:16] of serial flash memory.
 *            Directly specify the start address of source image.
 * 
 * REG [0xbf] Serial flash DMA Source Starting Address 3 (DMA_SSTR3)
 *            bit [7-0] Serial flash DMA Source START ADDRESS [31:24]
 *            This register sets the start address [31:24] of serial flash memory.
 *            Directly specify the start address of source image.
 * @endverbatim
 *
 * @param addr: 
 *
 * @note This bits index serial flash address [7:0][15:8][23:16][31:24]
 *
 * @result None
 */
void Panel_RA8889::SFI_DMA_SourceAddress(uint32_t addr)
{
//  SPI_CmdWrite(REG_DMA_SSTR0);      //0xbc, Serial flash DMA Source Starting Address 0 (DMA_SSTR0)
//  SPI_DataWrite(addr);              //address [7:0]
//  SPI_CmdWrite(REG_DMA_SSTR1);      //0xbd, Serial flash DMA Source Starting Address 1 (DMA_SSTR1)
//  SPI_DataWrite(addr >> 8);         //address [15:8]
//  SPI_CmdWrite(REG_DMA_SSTR2);      //0xbe, Serial flash DMA Source Starting Address 2 (DMA_SSTR2)
//  SPI_DataWrite(addr >> 16);        //address [23:16]
//  SPI_CmdWrite(REG_DMA_SSTR3);      //0xbf, Serial flash DMA Source Starting Address 3 (DMA_SSTR3)
//  SPI_DataWrite(addr >> 24);        //address [31:24]
  
    // Array com os registradores consecutivos
    const uint8_t regs[4] = {REG_DMA_SSTR0, REG_DMA_SSTR1, REG_DMA_SSTR2, REG_DMA_SSTR3};
    for (int i = 0; i < 4; i++) {
        SPI_CmdWrite(regs[i]);                   //envia cada 8 bits para os registradores de endereço
        SPI_DataWrite((addr >> (8 * i)) & 0xFF); // envia cada byte do endereço
    }
}


//================================================================================
//
// [0xC0] DMA Destination Window Upper-Left corner X-coordinates 0 (DMA_DX0)
// [0xC1] DMA Destination Window Upper-Left corner X-coordinates 1 (DMA_DX1)
// [0xC2] DMA Destination Window Upper-Left corner Y-coordinates 0 (DMA_DY0)
// [0xC3] DMA Destination Window Upper-Left corner Y-coordinates 1 (DMA_DY1)
//
//================================================================================


/**
 * @brief     Configura o endereço inicial de destino da Flash Serial DMA
 *            Serial Flash I/F DMA Destination Starting Address
 *        
 * @verbatim
 * REG [0xc0] DMA Destination Window Upper-Left corner X-coordinates 0 (DMA_DX0)
 *            bit [7-0] 
 *            When REG 5Eh (AW_COLOR) bit 2 = 0 (Block Mode)
 *            This register defines Upper-Left corner X-coordinates [7:0] of DMA Destination Window on Canvas area.
 *            When REG 5Eh (AW_COLOR) bit 2 = 1 (Linear Mode)
 *            This register defines Destination address [7:2] in SDRAM.
 *
 * REG [0xc1] DMA Destination Window Upper-Left corner X-coordinates 1 (DMA_DX1)
 *            bit [7-0] 
 *            When REG 5Eh (AW_COLOR) bit 2 = 0 (Block Mode)
 *            This register defines Upper-Left corner X-coordinates [12:8] of DMA Destination Window on Canvas area.
 *            When REG 5Eh (AW_COLOR) bit 2 = 1 (Linear Mode)
 *            This register defines Destination address [15:8] in SDRAM.
 *
 * REG [0xc2] DMA Destination Window Upper-Left corner Y-coordinates 0 (DMA_DY0)
 *            bit [7-0] 
 *            When REG 5Eh (AW_COLOR) bit 2 = 0 (Block Mode)
 *            This register defines Upper-Left corner Y-coordinates [7:0] of DMA Destination Window on Canvas area.
 *            When REG 5Eh (AW_COLOR) bit 2 = 1 (Linear Mode)
 *            This register defines Destination address [23:16] in SDRAM.
 *
 * REG [0xc3] DMA Destination Window Upper-Left corner Y-coordinates 1 (DMA_DY1)
 *            bit [7-0] 
 *            When REG 5Eh (AW_COLOR) bit 2 = 0 (Block Mode)
 *            This register defines Upper-Left corner Y-coordinates [12:8] of DMA Destination Window on Canvas area.
 *            When REG 5Eh (AW_COLOR) bit 2 = 1 (Linear Mode)
 *            This register defines Destination address [31:24] in SDRAM.
 * @endverbatim
 *
 * @param addr: 
 *
 * @note This bits index SDRAM address [7:0][15:8][23:16][31:24], bits [1:0] fix at 0.
 *
 * @result None
 */
void Panel_RA8889::SFI_DMA_DestinationAddress(uint32_t addr)
{
    //Array com os registradores consecutivos
    const uint8_t regs[4] = {REG_DMA_DX0, REG_DMA_DX1, REG_DMA_DY0, REG_DMA_DY1};
    for (int i = 0; i < 4; i++) {
        SPI_CmdWrite(regs[i]);                   //envia cada 8 bits para os registradores de endereço
        SPI_DataWrite((addr >> (8 * i)) & 0xFF); //envia cada byte do endereço
    }
}


/**
 * @brief  Configura o ponto superior esquerdo da janela de destino do DMA
 *
 * @verbatim
 * Define as coordenadas (X, Y) do canto superior esquerdo da área de destino
 * para onde os dados transferidos por DMA serão gravados no canvas do RA8889.
 * 
 * Em modo Block (REG 5Eh bit2 = 0), os registradores definem diretamente
 * as coordenadas X e Y do ponto inicial no canvas.
 * 
 * Em modo Linear (REG 5Eh bit2 = 1), os mesmos registradores são usados
 * como endereço base de destino na SDRAM.
 *
 * REG [0xc0] DMA Destination Window Upper-Left corner X-coordinates 0 (DMA_DX0)
 *            bit [7-0] 
 *            When REG 5Eh (AW_COLOR) bit 2 = 0 (Block Mode)
 *            This register defines Upper-Left corner X-coordinates [7:0] of DMA Destination Window on Canvas area.
 *            When REG 5Eh (AW_COLOR) bit 2 = 1 (Linear Mode)
 *            This register defines Destination address [7:2] in SDRAM.
 *
 * REG [0xc1] DMA Destination Window Upper-Left corner X-coordinates 1 (DMA_DX1)
 *            bit [7-0] 
 *            When REG 5Eh (AW_COLOR) bit 2 = 0 (Block Mode)
 *            This register defines Upper-Left corner X-coordinates [12:8] of DMA Destination Window on Canvas area.
 *            When REG 5Eh (AW_COLOR) bit 2 = 1 (Linear Mode)
 *            This register defines Destination address [15:8] in SDRAM.
 *
 * REG [0xc2] DMA Destination Window Upper-Left corner Y-coordinates 0 (DMA_DY0)
 *            bit [7-0] 
 *            When REG 5Eh (AW_COLOR) bit 2 = 0 (Block Mode)
 *            This register defines Upper-Left corner Y-coordinates [7:0] of DMA Destination Window on Canvas area.
 *            When REG 5Eh (AW_COLOR) bit 2 = 1 (Linear Mode)
 *            This register defines Destination address [23:16] in SDRAM.
 *
 * REG [0xc3] DMA Destination Window Upper-Left corner Y-coordinates 1 (DMA_DY1)
 *            bit [7-0] 
 *            When REG 5Eh (AW_COLOR) bit 2 = 0 (Block Mode)
 *            This register defines Upper-Left corner Y-coordinates [12:8] of DMA Destination Window on Canvas area.
 *            When REG 5Eh (AW_COLOR) bit 2 = 1 (Linear Mode)
 *            This register defines Destination address [31:24] in SDRAM.
 * @endverbatim
 *
 * @param Wx Coordenada X inicial (0 ~ 4095) da janela de destino
 * @param Hy Coordenada Y inicial (0 ~ 4095) da janela de destino
 *
 * @note Para uso no modo de Canvas (Block Mode), REG 5Eh (AW_COLOR) bit 1 = 0, Block Mode
 *
 * @result None
 */
void Panel_RA8889::SFI_DMA_DestinationUpperLeftCorner(uint16_t Wx, uint16_t Hy)
{
  SPI_CmdWrite(REG_DMA_DX0);                   //0xc0, DMA Destination Window Upper-Left corner X-coordinates 0 (DMA_DX0)
  SPI_DataWrite(Wx);                           //byte baixo de Wx [7:0]  
  SPI_CmdWrite(REG_DMA_DX1);                   //0xc1, DMA Destination Window Upper-Left corner X-coordinates 1 (DMA_DX1)
  SPI_DataWrite(Wx >> 8);                      //byte alto de Wx [12:8] 
									           
  SPI_CmdWrite(REG_DMA_DY0);                   //0xc2, DMA Destination Window Upper-Left corner Y-coordinates 0 (DMA_DY0)
  SPI_DataWrite(Hy);                           //byte baixo de Hy [7:0]
  SPI_CmdWrite(REG_DMA_DY1);                   //0xc3, DMA Destination Window Upper-Left corner Y-coordinates 1 (DMA_DY1)
  SPI_DataWrite(Hy >> 8);                      //byte alto de Hy [12:8]
}


//================================================================================
//
// [0xC6] DMA Block Width 0 (DMAW_WTH0)
// [0xC7] DMA Block Width 1 (DMAW_WTH1)
// [0xC8] DMA Block Height 0 (DMAW_HIGH0)
// [0xC9] DMA Block Height 1 (DMAW_HIGH1)
//
//================================================================================


/**
 * @brief  
 *
 * @verbatim
 *
 * REG [0xc6] DMA Block Width 0 (DMAW_WTH0)
 *            bit [7-0] 
 *            When REG 5Eh (AW_COLOR) bit 2 = 0 (Block Mode)
 *            DMA Block Width [7:0]
 *            When REG 5Eh (AW_COLOR) bit 2 = 1 (Linear Mode)
 *            DMA Transfer Number [7:0]
 *
 * REG [0xc7] DMA Block Width 1 (DMAW_WTH1)
 *            bit [7-0] 
 *            When REG 5Eh (AW_COLOR) bit 2 = 0 (Block Mode)
 *            DMA Block Width [15:8]
 *            When REG 5Eh (AW_COLOR) bit 2 = 1 (Linear Mode)
 *            DMA Transfer Number [15:8]
 *
 * REG [0xc8] DMA Block Height 0 (DMAW_HIGH0)
 *            bit [7-0] 
 *            When REG 5Eh (AW_COLOR) bit 2 = 0 (Block Mode)
 *            DMA Block Height [7:0]
 *            When REG 5Eh (AW_COLOR) bit 2 = 1 (Linear Mode)
 *            DMA Transfer Number [23:16]
 *
 * REG [0xc9] DMA Block Height 1 (DMAW_HIGH1)
 *            bit [7-0] 
 *            When REG 5Eh (AW_COLOR) bit 2 = 0 (Block Mode)
 *            DMA Block Height [15:8]
 *            When REG 5Eh (AW_COLOR) bit 2 = 1 (Linear Mode)
 *            DMA Transfer Number [31:24]
 * @endverbatim
 *
 * @param addr
 *
 * @note 
 *
 * @result Unit : Pixel
 *         When REG DMACR bit 1 = 0 (Linear Mode)
 *         DMA Transfer Number [7:0][15:8][23:16][31:24]
 *         
 *         When REG DMACR bit 1 = 1 (Block Mode)
 *         DMA Block Width [7:0][15:8]
 *         DMA Block HIGH[7:0][15:8]
 */
void Panel_RA8889::SFI_DMA_TransferNumber(uint32_t addr)
{
  SPI_CmdWrite(REG_DMAW_WTH0);                   //0xc6, DMA Block Width 0 (DMAW_WTH0)
  SPI_DataWrite(addr);                           
  SPI_CmdWrite(REG_DMAW_WTH1);                   //0xc7, DMA Block Width 1 (DMAW_WTH1)
  SPI_DataWrite(addr >> 8);                      
  SPI_CmdWrite(REG_DMAW_HIGH0);                  //0xc8, DMA Block Height 0 (DMAW_HIGH0)
  SPI_DataWrite(addr >> 16);                     
  SPI_CmdWrite(REG_DMAW_HIGH1);                  //0xc9, DMA Block Height 1 (DMAW_HIGH1)
  SPI_DataWrite(addr >> 24);                     
}


/**
 * @brief  
 *
 * @verbatim
 *
 * REG [0xc6] DMA Block Width 0 (DMAW_WTH0)
 *            bit [7-0] 
 *            When REG 5Eh (AW_COLOR) bit 2 = 0 (Block Mode)
 *            DMA Block Width [7:0]
 *            When REG 5Eh (AW_COLOR) bit 2 = 1 (Linear Mode)
 *            DMA Transfer Number [7:0]
 *
 * REG [0xc7] DMA Block Width 1 (DMAW_WTH1)
 *            bit [7-0] 
 *            When REG 5Eh (AW_COLOR) bit 2 = 0 (Block Mode)
 *            DMA Block Width [15:8]
 *            When REG 5Eh (AW_COLOR) bit 2 = 1 (Linear Mode)
 *            DMA Transfer Number [15:8]
 *
 * REG [0xc8] DMA Block Height 0 (DMAW_HIGH0)
 *            bit [7-0] 
 *            When REG 5Eh (AW_COLOR) bit 2 = 0 (Block Mode)
 *            DMA Block Height [7:0]
 *            When REG 5Eh (AW_COLOR) bit 2 = 1 (Linear Mode)
 *            DMA Transfer Number [23:16]
 *
 * REG [0xc9] DMA Block Height 1 (DMAW_HIGH1)
 *            bit [7-0] 
 *            When REG 5Eh (AW_COLOR) bit 2 = 0 (Block Mode)
 *            DMA Block Height [15:8]
 *            When REG 5Eh (AW_COLOR) bit 2 = 1 (Linear Mode)
 *            DMA Transfer Number [31:24]
 * @endverbatim
 *
 * @param addr
 *
 * @note 
 *
 * @result Unit : Pixel
 *         When REG DMACR bit 1 = 0 (Linear Mode)
 *         DMA Transfer Number [7:0][15:8][23:16][31:24]
 *         
 *         When REG DMACR bit 1 = 1 (Block Mode)
 *         DMA Block Width [7:0][15:8]
 *         DMA Block HIGH[7:0][15:8]
 */
void Panel_RA8889::SFI_DMA_TransferWidthHeight(uint16_t Wx, uint16_t Hy)
{
  SPI_CmdWrite(REG_DMAW_WTH0);                 //0xc6, DMA Block Width 0 (DMAW_WTH0)
  SPI_DataWrite(Wx);                           //byte baixo de Wx [7:0]
  SPI_CmdWrite(REG_DMAW_WTH1);                 //0xc7, DMA Block Width 1 (DMAW_WTH1)
  SPI_DataWrite(Wx >> 8);                      //byte alto de Wx [12:8]
  
  SPI_CmdWrite(REG_DMAW_HIGH0);                //0xc8, DMA Block Height 0 (DMAW_HIGH0)
  SPI_DataWrite(Hy);                           //byte baixo de Hy [7:0]
  SPI_CmdWrite(REG_DMAW_HIGH1);                //0xc9, DMA Block Height 1 (DMAW_HIGH1)
  SPI_DataWrite(Hy >> 8);                      //byte alto de Hy [12:8]
}


//================================================================================
//
// [0xCA] DMA Source Picture Width 0 (DMA_SWTH0)
// [0xCB] DMA Source Picture Width 0 (DMA_SWTH1)
//
//================================================================================


/**
 * @brief  DMA Source Picture Width
 *
 * @verbatim
 * REG [0xca] DMA Source Picture Width 0 (DMA_SWTH0)
 *            bit [7-0] DMA Source Picture Width [7:0]
 *            Unit: pixel
 *
 * REG [0xcb] DMA Source Picture Width 0 (DMA_SWTH1)
 *            bit [4-0] DMA Source Picture Width [12:8]
 * @endverbatim
 *
 * @param Wx
 *
 * @note DMA Source Picture Width [7:0][12:8]
 *
 * @result None
 */
void Panel_RA8889::SFI_DMA_SourceWidth(uint16_t Wx)
{
  SPI_CmdWrite(REG_DMA_SWTH0);       //0xca, DMA Source Picture Width 0 (DMA_SWTH0)
  SPI_DataWrite(Wx);
  SPI_CmdWrite(REG_DMA_SWTH1);       //0xcb, DMA Source Picture Width 0 (DMA_SWTH1)
  SPI_DataWrite(Wx >> 8);
}


//================================================================================
//
// [0xCC] Character Control Register 0 (CCR0)
//
//================================================================================


/**
 * @brief Select User-defined Font in Text Mode
 *
 * @verbatim
 *        REG [0xcc] Character Control Register 0 (CCR0)
 *                   bit [7-6] Character source selection
 *                             User-defined Font /CGROM Font Selection Bit in Text Mode
 *                             0b00: Select internal CGROM Character.
 *                             0b01: Select external CGROM Character. (Genitop serial flash)
 *                             0b10: Select user-defined Character.
 *                             0b11: NA
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::Font_UseUserDefined(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_CCR0);                      //0xcc, Character Control Register 0 (CCR0)
  temp = SPI_DataRead();                       
  temp |= cSetb7;                              //Set bit 7, Select user-defined Character
  temp &= cClrb6;                              //Reset bit 6, Select user-defined Character
  SPI_DataWrite(temp);                         //Set selection user-defined Character
}


/**
 * @brief Select Internal CGROM Font in Text Mode
 *        
 * @verbatim
 *        REG [0xcc] Character Control Register 0 (CCR0)
 *                   bit [7-6] Character source selection
 *                             User-defined Font /CGROM Font Selection Bit in Text Mode
 *                             0b00: Select internal CGROM Character.
 *                             0b01: Select external CGROM Character. (Genitop serial flash)
 *                             0b10: Select user-defined Character.
 *                             0b11: NA
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::Font_UseInternalCGROM(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_CCR0);                      //0xcc, Character Control Register 0 (CCR0)
  temp = SPI_DataRead();                       
  temp &= cClrb7;                              //Reset bit 7, Select internal CGROM Character
  temp &= cClrb6;                              //Reset bit 6, Select internal CGROM Character
  SPI_DataWrite(temp);
}


/**
 * @brief Select External CGROM Font (enitop serial flash) in Text Mode
 *
 * @verbatim
 *        REG [0xcc] Character Control Register 0 (CCR0)
 *                   bit [7-6] Character source selection
 *                             User-defined Font /CGROM Font Selection Bit in Text Mode
 *                             0b00: Select internal CGROM Character.
 *                             0b01: Select external CGROM Character. (Genitop serial flash)
 *                             0b10: Select user-defined Character.
 *                             0b11: NA
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::Font_UseExternalCGROM(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_CCR0);                      //0xcc, Character Control Register 0 (CCR0)
  temp = SPI_DataRead();                       
  temp &= cClrb7;                              //Reset bit 7, Select external CGROM Character. (Genitop serial flash)
  temp |= cSetb6;                              //Set bit 6, Select external CGROM Character. (Genitop serial flash)
  SPI_DataWrite(temp);
}


/**
 * @brief Character source selection
 *
 * @verbatim
 *        REG [CCh] Character Control Register 0 (CCR0)
 *                  bit [7-6] Character source selection
 *                            User-defined Font /CGROM Font Selection Bit in Text Mode
 *                            0b00: Select internal CGROM Character.
 *                            0b01: Select external CGROM Character. (Genitop serial flash)
 *                            0b10: Select user-defined Character.
 *                            0b11: NA
 * @endverbatim
 *
 * @param FontSource::Internal : Select internal CGROM Character.
 *        FontSource::External : Select external CGROM Character. (Genitop serial flash)
 *        FontSource::User     : Select user-defined Character.
 *
 * @note Exemplo: Font_SetSource(FontSource::ExternalCGROM); 
 */
void Panel_RA8889::Font_SetSource(FontSource source)
{
  uint8_t temp;
  SPI_CmdWrite(REG_CCR0);                      //0xcc, Character Control Register 0 (CCR0)
  temp = SPI_DataRead();                       
  temp &= ~(cSetb6 | cSetb7);                  //Reset bits 6 e 7 de uma vez
  temp |= static_cast<uint8_t>(source);        //Converte enum para uint8_t
  SPI_DataWrite(temp);                         //Set Character source selection
}


/**
 * @brief Select Fonte Height 8x16 / 16x16
 *
 * @verbatim
 *        REG [CCh] Character Control Register 0 (CCR0)
 *                  bit [5-4] Character source selection
 *                            0b00 : 16; ex.  8x16 / 16x16 / variable character width x 16
 *                            0b01 : 24; ex. 12x24 / 24x24 / variable character width x 24
 *                            0b10 : 32; ex. 16x32 / 32x32 / variable character width x 32
 *
 *                  - for internal CGROM (12x24)
 *                  - for external CGROM (16x16, 24x24, 32x32, other)
 *                    Character ROM of Genitop Inc., CI GT21L16T1W, GT30L16U2W, GT30L24T3Y, 
 *                    GT30L24M1Z, GT30L32S4W, GT20L24F6Y, GT21L24S1W, there are different 
 *                    resolutions for character.
 *					- for user-defined Character (8x16,12x24,16x32)
 *                  
 *                  Note:
 *                  1. User-defined character width is decided by character 
 *                     code; width for code < 8000h is 8/12/16 and width for 
 *                     code >=8000h is 16/24/32.
 *                  2. The character width of Genitop’s serial flash is 
 *                     decided by chosen character sets and need to configure 
 *                     GT Font ROM registers (CEh, CFh).
 *                  3. Internal CGROM supports size 12x24.
 * @endverbatim
 *
 * @param None
 *
 * @note User-defined Font width is decided by font code. Genitop
 *       serial flash's font width is decided by font code or GT Font ROM
 *       control register.
 */
void Panel_RA8889::Font_SetHeight_16(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_CCR0);                      //0xcc, Character Control Register 0 (CCR0)
  temp = SPI_DataRead();                       
  temp &= cClrb5;
  temp &= cClrb4;
  SPI_DataWrite(temp);
}


/**
 * @brief Select Fonte Height 12x24 / 24x24
 *
 * @verbatim
 *        REG [CCh] Character Control Register 0 (CCR0)
 *                  bit [5-4] Character source selection
 *                            0b00 : 16; ex.  8x16 / 16x16 / variable character width x 16
 *                            0b01 : 24; ex. 12x24 / 24x24 / variable character width x 24
 *                            0b10 : 32; ex. 16x32 / 32x32 / variable character width x 32
 *
 *                  - for internal CGROM (12x24)
 *                  - for external CGROM (16x16, 24x24, 32x32, other)
 *                    Character ROM of Genitop Inc., CI GT21L16T1W, GT30L16U2W, GT30L24T3Y, 
 *                    GT30L24M1Z, GT30L32S4W, GT20L24F6Y, GT21L24S1W, there are different 
 *                    resolutions for character.
 *					- for user-defined Character (8x16,12x24,16x32)
 *                  
 *                  Note:
 *                  1. User-defined character width is decided by character 
 *                     code; width for code < 8000h is 8/12/16 and width for 
 *                     code >=8000h is 16/24/32.
 *                  2. The character width of Genitop’s serial flash (Intrnal 
 *                     CGROM) is decided by chosen character sets and need to 
 *                     configure GT Font ROM registers (CEh, CFh).
 *                  3. Internal CGROM supports size 12x24.
 * @endverbatim
 *
 * @param None
 *
 * @note User-defined Font width is decided by font code. Genitop
 *       serial flash's font width is decided by font code or GT Font ROM
 *       control register.
 */
void Panel_RA8889::Font_SetHeight_24(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_CCR0);                      //0xcc, Character Control Register 0 (CCR0)
  temp = SPI_DataRead();                       
  temp &= cClrb5;
  temp |= cSetb4;
  SPI_DataWrite(temp);
}


/**
 * @brief Select Fonte Height 16x32 / 32x32
 *
 * @verbatim
 *        REG [CCh] Character Control Register 0 (CCR0)
 *                  bit [5-4] Character source selection
 *                            0b00 : 16; ex.  8x16 / 16x16 / variable character width x 16
 *                            0b01 : 24; ex. 12x24 / 24x24 / variable character width x 24
 *                            0b10 : 32; ex. 16x32 / 32x32 / variable character width x 32
 *
 *                  - for internal CGROM (12x24)
 *                  - for external CGROM (16x16, 24x24, 32x32, other)
 *                    Character ROM of Genitop Inc., CI GT21L16T1W, GT30L16U2W, GT30L24T3Y, 
 *                    GT30L24M1Z, GT30L32S4W, GT20L24F6Y, GT21L24S1W, there are different 
 *                    resolutions for character.
 *					- for user-defined Character (8x16,12x24,16x32)
 *                  
 *                  Note:
 *                  1. User-defined character width is decided by character 
 *                     code; width for code < 8000h is 8/12/16 and width for 
 *                     code >=8000h is 16/24/32.
 *                  2. The character width of Genitop’s serial flash is 
 *                     decided by chosen character sets and need to configure 
 *                     GT Font ROM registers (CEh, CFh).
 *                  3. Internal CGROM supports size 12x24.
 * @endverbatim
 *
 * @param None
 *
 * @note User-defined Font width is decided by font code. Genitop
 *       serial flash's font width is decided by font code or GT Font ROM
 *       control register.
 */
void Panel_RA8889::Font_SetHeight_32(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_CCR0);                      //0xcc, Character Control Register 0 (CCR0)
  temp = SPI_DataRead();                       
  temp |= cSetb5;
  temp &= cClrb4;
  SPI_DataWrite(temp);
}


/**
 * @brief Select Fonte Height (8x16 / 16x16 / 12x24 / 24x24 / 16x32 / 32x32)
 *
 * @verbatim
 *        REG [CCh] Character Control Register 0 (CCR0)
 *                  bit [5-4] Character source selection
 *                            0b00 : 16; ex.  8x16 / 16x16 / variable character width x 16
 *                            0b01 : 24; ex. 12x24 / 24x24 / variable character width x 24
 *                            0b10 : 32; ex. 16x32 / 32x32 / variable character width x 32
 *
 *                  - for internal CGROM (12x24)
 *                  - for external CGROM (16x16, 24x24, 32x32, other)
 *                    Character ROM of Genitop Inc., CI GT21L16T1W, GT30L16U2W, GT30L24T3Y, 
 *                    GT30L24M1Z, GT30L32S4W, GT20L24F6Y, GT21L24S1W, there are different 
 *                    resolutions for character.
 *					- for user-defined Character (8x16,12x24,16x32)
 *                  
 *                  Note:
 *                  1. User-defined character width is decided by character 
 *                     code; width for code < 8000h is 8/12/16 and width for 
 *                     code >=8000h is 16/24/32.
 *                  2. The character width of Genitop’s serial flash is 
 *                     decided by chosen character sets and need to configure 
 *                     GT Font ROM registers (CEh, CFh).
 *                  3. Internal CGROM supports size 12x24.
 * @endverbatim
 *
 * @param enum FontHeight height: Altura desejada da fonte
 *             FontHeight::H16  :  8x16 / 16x16
 *             FontHeight::H24  : 12x24 / 24x24
 *             FontHeight::H32  : 16x32 / 32x32
 *
 * @note User-defined Font width is decided by font code. Genitop
 *       serial flash's font width is decided by font code or GT Font ROM
 *       control register.
 */ 
void Panel_RA8889::Font_SetHeight(FontHeight height)
{
  uint8_t temp;
  uint8_t bits = 0;
  
  // Mapear enum para bits [5:4] do CCR0
  switch(height)
  {
      case FontHeight::H16: bits = 0x00;   break;
      case FontHeight::H24: bits = cSetb0; break;
      case FontHeight::H32: bits = cSetb1; break;
      default: 
          return; // nunca deve acontecer
  }
  
  SPI_CmdWrite(REG_CCR0);       //0xcc, Character Control Register 0 (CCR0)
  temp = SPI_DataRead();        //Lê valor atual
  temp &= ~(cSetb5 | cSetb4);   //Limpa bits 5:4
  temp |= bits;                 //Seta bits corretos
  SPI_DataWrite(temp);          //Escreve de volta
}


/**
 * @brief Font Selection for internal CGROM ISO/IEC 8859-1
 *
 * @verbatim
 *        REG [CCh] Character Control Register 0 (CCR0)
 *                  bit [1-0] Character Selection for internal CGROM
 *                            When FNCR0 [7-6] 0b00, Internal CGROM 
 *                            supports character sets with the standard coding 
 *                            of ISO/IEC 8859-1,2,4,5, which supports English 
 *                            and most of European country languages
 *                            0b00 : ISO/IEC 8859-1 - Latin-1 (Ocidental/Europeu Ocidental)
 *                            0b01 : ISO/IEC 8859-2 - Latin-2 (Europeu Central)
 *                            0b10 : ISO/IEC 8859-4 - Latin-4 (Europeu do Norte)
 *                            0b11 : ISO/IEC 8859-5 - Latin/Cirílico
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::Select_Internal_CGROM_ISOIEC8859_1(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_CCR0);                      //0xcc, Character Control Register 0 (CCR0)
  temp = SPI_DataRead();                       
  temp &= cClrb1;
  temp &= cClrb0;
  SPI_DataWrite(temp);
}


/**
 * @brief Font Selection for internal CGROM ISO/IEC 8859-2
 *
 * @verbatim
 * REG [0xcc] Character Control Register 0 (CCR0)
 *            bit [1-0] Character Selection for internal CGROM
 *                      When FNCR0 [7-6] 0b00, Internal CGROM 
 *                      supports character sets with the standard coding 
 *                      of ISO/IEC 8859-1,2,4,5, which supports English 
 *                      and most of European country languages
 *                      0b00 : ISO/IEC 8859-1 - Latin-1 (Ocidental/Europeu Ocidental)
 *                      0b01 : ISO/IEC 8859-2 - Latin-2 (Europeu Central)
 *                      0b10 : ISO/IEC 8859-4 - Latin-4 (Europeu do Norte)
 *                      0b11 : ISO/IEC 8859-5 - Latin/Cirílico
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::Select_Internal_CGROM_ISOIEC8859_2(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_CCR0);                      //0xcc, Character Control Register 0 (CCR0)
  temp = SPI_DataRead();                       
  temp &= cClrb1;
  temp |= cSetb0;
  SPI_DataWrite(temp);
}


/**
 * @brief Font Selection for internal CGROM ISO/IEC 8859-4
 *
 * @verbatim
 * REG [0xcc] Character Control Register 0 (CCR0)
 *            bit [1-0] Character Selection for internal CGROM
 *                      When FNCR0 [7-6] 0b00, Internal CGROM 
 *                      supports character sets with the standard coding 
 *                      of ISO/IEC 8859-1,2,4,5, which supports English 
 *                      and most of European country languages
 *                      0b00 : ISO/IEC 8859-1 - Latin-1 (Ocidental/Europeu Ocidental)
 *                      0b01 : ISO/IEC 8859-2 - Latin-2 (Europeu Central)
 *                      0b10 : ISO/IEC 8859-4 - Latin-4 (Europeu do Norte)
 *                      0b11 : ISO/IEC 8859-5 - Latin/Cirílico
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::Select_Internal_CGROM_ISOIEC8859_4(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_CCR0);                      //0xcc, Character Control Register 0 (CCR0)
  temp = SPI_DataRead();                       
  temp |= cSetb1;
  temp &= cClrb0;
  SPI_DataWrite(temp);
}


/**
 * @brief Font Selection for internal CGROM ISO/IEC 8859-5
 *
 * @verbatim
 * REG [0xcc] Character Control Register 0 (CCR0)
 *            bit [1-0] Character Selection for internal CGROM
 *                      When FNCR0 [7-6] 0b00, Internal CGROM 
 *                      supports character sets with the standard coding 
 *                      of ISO/IEC 8859-1,2,4,5, which supports English 
 *                      and most of European country languages
 *                      0b00 : ISO/IEC 8859-1 - Latin-1 (Ocidental/Europeu Ocidental)
 *                      0b01 : ISO/IEC 8859-2 - Latin-2 (Europeu Central)
 *                      0b10 : ISO/IEC 8859-4 - Latin-4 (Europeu do Norte)
 *                      0b11 : ISO/IEC 8859-5 - Latin/Cirílico
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::Select_Internal_CGROM_ISOIEC8859_5(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_CCR0);                      //0xcc, Character Control Register 0 (CCR0)
  temp = SPI_DataRead();                       
  temp |= cSetb1;
  temp |= cSetb0;
  SPI_DataWrite(temp);
}


/**
 * @brief Font Selection for internal CGROM ISO/IEC 8859
 *
 * @verbatim
 * REG [0xcc] Character Control Register 0 (CCR0)
 *            bit [1-0] Character Selection for internal CGROM
 *                      When FNCR0 [7-6] 0b00, Internal CGROM 
 *                      supports character sets with the standard coding 
 *                      of ISO/IEC 8859-1,2,4,5, which supports English 
 *                      and most of European country languages
 *                      0b00 : ISO/IEC 8859-1 - Latin-1 (Ocidental/Europeu Ocidental)
 *                      0b01 : ISO/IEC 8859-2 - Latin-2 (Europeu Central)
 *                      0b10 : ISO/IEC 8859-4 - Latin-4 (Europeu do Norte)
 *                      0b11 : ISO/IEC 8859-5 - Latin/Cirílico
 * @endverbatim
 *
 * @param iso ISO/IEC 8859 code to select (InternalCGROM_ISO8859)
 *
 * @note None
 */
void Panel_RA8889::Select_Internal_CGROM_ISO8859(InternalCGROM_ISO8859 iso)
{
  uint8_t temp;
  SPI_CmdWrite(REG_CCR0);                      // Seleciona o registrador CCR0
  temp = SPI_DataRead();                       // Lê valor atual
  temp &= ~(cSetb1 | cSetb0);                  // Limpa os bits B1:B0
  temp |= static_cast<uint8_t>(iso);           // Seta bits conforme enum
  SPI_DataWrite(temp);                         // Escreve no registrador
}


//================================================================================
//
// [0xCD] Character Control Register 1 (CCR1)
//
//================================================================================


/**
 * @brief Characer Full Alignment Set
 *
 * @verbatim
 * REG [0xcd] Character Control Register 1 (CCR1)
 *            bit [7] Full Alignment Selection Bit
 *                    0b0 : Full alignment disable.
 *                    0b1 : Full alignment enable.
 *                    
 *                    When Full alignment is enabled, the character 
 *                    width is equal to half of the character height, 
 *                    width = height / 2, (the condition is character 
 *                    width is equal to or small than half of the 
 *                    character height), otherwise the character width 
 *                    is equal to character height.
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::Font_FullAlignmentEnable(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_CCR1);                      //0xcd, Character Control Register 1 (CCR1)
  temp = SPI_DataRead();                       
  temp |= cSetb7;                              //Set full alignment
  SPI_DataWrite(temp);                         
}


/**
 * @brief Characer Full Alignment Set
 *        
 * @verbatim  
 * REG [0xcd] Character Control Register 1 (CCR1)
 *            bit [7] Full Alignment Selection Bit
 *                    0b0 : Full alignment disable.
 *                    0b1 : Full alignment enable.
 *                   
 *                    When Full alignment is enabled, the character 
 *                    width is equal to half of the character height, 
 *                    width = height / 2, (the condition is character 
 *                    width is equal to or small than half of the 
 *                    character height), otherwise the character width 
 *                    is equal to character height.
 * @endverbatim  
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::Font_FullAlignmentDisable(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_CCR1);                      //0xcd, Character Control Register 1 (CCR1)
  temp = SPI_DataRead();                       
  temp &= cClrb7;                               //Disable full alignment
  SPI_DataWrite(temp);
}


/**
 * @brief Font Background Transparency
 *
 * @verbatim
 * REG [0xcd] Character Control Register 1 (CCR1)
 *            bit [6] Chroma keying enable on Text input
 *                    0b0 : Character’s background displayed with specified color.
 *                    0b1 : Character’s background displayed with original canvas' background (transparency).
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::Font_UseBackgroundTransparency(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_CCR1);                      //0xcd, Character Control Register 1 (CCR1)
  temp = SPI_DataRead();                       
  temp |= cSetb6;
  SPI_DataWrite(temp);
}


/**
 * @brief Font Background Color
 *
 * @verbatim
 * REG [0xcd] Character Control Register 1 (CCR1)
 *            bit [6] Chroma keying enable on Text input
 *                    0b0 : Character’s background displayed with specified color.
 *                    0b1 : Character’s background displayed with original canvas’ background (transparency).
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::Font_UseBackgroundColor(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_CCR1);                      //0xcd, Character Control Register 1 (CCR1)
  temp = SPI_DataRead();                       
  temp &= cClrb6;                              //Set original canva's color 
  SPI_DataWrite(temp);
}


/**
 * @brief Font Rotete 0 degree
 *
 * @verbatim
 * REG [0xcd] Character Control Register 1 (CCR1)
 *            bit [4] Character Rotation
 *                    0b0 : Normal
 *                          Text direction from left to right then from top to bottom
 *                    0b1 : Counterclockwise 90 degree & horizontal flip
 *                          Text direction from top to bottom then from left to right
 *                          (it should accommodate with set VDIR as 1)
 *                    This attribute can be changed only when previous font write
 *                    finished (core_busy = 0)
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::Font_0degree(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_CCR1);                      //0xcd, Character Control Register 1 (CCR1)
  temp = SPI_DataRead();                       
  temp &= cClrb4;
  SPI_DataWrite(temp);
}


/**
 * @brief Font Rotete 90 degree
 *
 * @verbatim 
 * REG [0xcd] Character Control Register 1 (CCR1)
 *            bit [4] Character Rotation
 *                    0b0 : Normal
 *                          Text direction from left to right then from top to bottom
 *                    0b1 : Counterclockwise 90 degree & horizontal flip
 *                          Text direction from top to bottom then from left to right
 *                          (it should accommodate with set VDIR as 1)
 *                    This attribute can be changed only when previous font write
 *                    finished (core_busy = 0)
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::Font_90degree(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_CCR1);                      //0xcd, Character Control Register 1 (CCR1)
  temp = SPI_DataRead();                       
  temp |= cSetb4;
  SPI_DataWrite(temp);
}


/**
 * @brief Character width enlargement factor
 *        
 * @verbatim  
 *        REG [CDh] Character Control Register 1 (CCR1)
 *                  bit [3-2] Character width enlargement factor
 *                          0b00 : factor x1
 *                          0b01 : factor x2
 *                          0b10 : factor x3
 *                          0b11 : factor x4
 * @endverbatim  
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::Font_WidthEnlargFactor(FontEnlargFactor factor)
{
  uint8_t temp;
  SPI_CmdWrite(REG_CCR1);                      //0xcd, Character Control Register 1 (CCR1)
  temp = SPI_DataRead();
  temp &= ~(cSetb3 | cSetb2);                  //Reset bits 3 e 2 de uma vez
  uint8_t f static_cast<uint8_t>factor;        //Converte para byte
  temp |= (f << 2);                            //Posiciona para o bit 3-2
  SPI_DataWrite(temp);
}


/**
 * @brief Character height enlargement factor
 *        
 * @verbatim  
 *        REG [CDh] Character Control Register 1 (CCR1)
 *                  bit [1-0] Character height enlargement factor
 *                          0b00 : factor x1
 *                          0b01 : factor x2
 *                          0b10 : factor x3
 *                          0b11 : factor x4
 * @endverbatim  
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::Font_HeightEnlargFactor(FontEnlargFactor factor)
{
  uint8_t temp;
  SPI_CmdWrite(REG_CCR1);                      //0xcd, Character Control Register 1 (CCR1)
  temp = SPI_DataRead();      
  temp &= ~(cSetb1 | cSetb0);                  //Reset bits 1 e 0 de uma vez
  temp |= (static_cast<uint8_t>factor);
  ER_TFT.LCD_DataWrite(temp);
}


//================================================================================
//
// [0xCE] GT Character ROM Select (GTFNT_SEL)
//
//================================================================================


/**
 * @brief Font Selection for External CGROM Genitop's GT21L16T1W IC
 *        
 * @verbatim  
 *        REG [CEh] GT Character ROM Select (GTFNT_SEL)
 *                  bit [7-5] GT Serial Character ROM Select (Genitop's Inc.)
 *                            0b000 : Circuito Integrado External CGROM GT21L16T1W
 *                            0b001 : Circuito Integrado External CGROM GT30L16U2W
 *                            0b010 : Circuito Integrado External CGROM GT30L24T3Y
 *                            0b011 : Circuito Integrado External CGROM GT30L24M1Z
 *                            0b100 : Circuito Integrado External CGROM GT30L32S4W
 *                            0b101 : Circuito Integrado External CGROM GT20L24F6Y
 *                            0b110 : Circuito Integrado External CGROM GT21L24S1W
 * @endverbatim  
 *                            
 * @param None
 *
 * @note None
 */
void Panel_RA8889::GTFont_Select_GT21L16T1W(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_GTFNT_SEL);                      //0xce, Character Control Register 0 (GTFNT_SEL)
  temp = SPI_DataRead();                       
  temp &= cClrb7;
  temp &= cClrb6;
  temp &= cClrb5;
  SPI_DataWrite(temp);
}


/**
 * @brief Font Selection for External CGROM Genitop's GT30L16U2W IC
 *
 * @verbatim       
 *        REG [CEh] GT Character ROM Select (GTFNT_SEL)
 *                  bit [7-5] GT Serial Character ROM Select (Genitop's Inc.)
 *                            0b000 : Circuito Integrado External CGROM GT21L16T1W
 *                            0b001 : Circuito Integrado External CGROM GT30L16U2W
 *                            0b010 : Circuito Integrado External CGROM GT30L24T3Y
 *                            0b011 : Circuito Integrado External CGROM GT30L24M1Z
 *                            0b100 : Circuito Integrado External CGROM GT30L32S4W
 *                            0b101 : Circuito Integrado External CGROM GT20L24F6Y
 *                            0b110 : Circuito Integrado External CGROM GT21L24S1W
 * @endverbatim
 * 
 * @param None
 *
 * @note None
 */
void Panel_RA8889::GTFont_Select_GT30L16U2W(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_GTFNT_SEL);                      //0xce, Character Control Register 0 (GTFNT_SEL)
  temp = SPI_DataRead();                       
  temp &= cClrb7;
  temp &= cClrb6;
  temp |= cSetb5;
  SPI_DataWrite(temp);
}


/**
 * @brief Font Selection for External CGROM Genitop's GT30L24T3Y IC
 *        
 * @verbatim  
 *        REG [CEh] GT Character ROM Select (GTFNT_SEL)
 *                  bit [7-5] GT Serial Character ROM Select (Genitop's Inc.)
 *                            0b000 : Circuito Integrado External CGROM GT21L16T1W
 *                            0b001 : Circuito Integrado External CGROM GT30L16U2W
 *                            0b010 : Circuito Integrado External CGROM GT30L24T3Y
 *                            0b011 : Circuito Integrado External CGROM GT30L24M1Z
 *                            0b100 : Circuito Integrado External CGROM GT30L32S4W
 *                            0b101 : Circuito Integrado External CGROM GT20L24F6Y
 *                            0b110 : Circuito Integrado External CGROM GT21L24S1W
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::GTFont_Select_GT30L24T3Y(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_GTFNT_SEL);                      //0xce, Character Control Register 0 (GTFNT_SEL)
  temp = SPI_DataRead();                       
  temp &= cClrb7;
  temp |= cSetb6;
  temp &= cClrb5;
  SPI_DataWrite(temp);
}


/**
 * @brief Font Selection for External CGROM Genitop's GT30L24M1Z IC
 *        
 *        REG [CEh] GT Character ROM Select (GTFNT_SEL)
 *                  bit [7-5] GT Serial Character ROM Select (Genitop's Inc.)
 *                            0b000 : Circuito Integrado External CGROM GT21L16T1W
 *                            0b001 : Circuito Integrado External CGROM GT30L16U2W
 *                            0b010 : Circuito Integrado External CGROM GT30L24T3Y
 *                            0b011 : Circuito Integrado External CGROM GT30L24M1Z
 *                            0b100 : Circuito Integrado External CGROM GT30L32S4W
 *                            0b101 : Circuito Integrado External CGROM GT20L24F6Y
 *                            0b110 : Circuito Integrado External CGROM GT21L24S1W
 *                            
 * @param None
 *
 * @note None
 */
void Panel_RA8889::GTFont_Select_GT30L24M1Z(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_GTFNT_SEL);                      //0xce, Character Control Register 0 (GTFNT_SEL)
  temp = SPI_DataRead();                       
  temp &= cClrb7;
  temp |= cSetb6;
  temp |= cSetb5;
  SPI_DataWrite(temp);
}


/**
 * @brief Font Selection for External CGROM Genitop's GT30L32S4W IC
 *        
 *        REG [CEh] GT Character ROM Select (GTFNT_SEL)
 *                  bit [7-5] GT Serial Character ROM Select (Genitop's Inc.)
 *                            0b000 : Circuito Integrado External CGROM GT21L16T1W
 *                            0b001 : Circuito Integrado External CGROM GT30L16U2W
 *                            0b010 : Circuito Integrado External CGROM GT30L24T3Y
 *                            0b011 : Circuito Integrado External CGROM GT30L24M1Z
 *                            0b100 : Circuito Integrado External CGROM GT30L32S4W
 *                            0b101 : Circuito Integrado External CGROM GT20L24F6Y
 *                            0b110 : Circuito Integrado External CGROM GT21L24S1W
 *                            
 * @param None
 *
 * @note None
 */
void Panel_RA8889::GTFont_Select_GT30L32S4W(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_GTFNT_SEL);                      //0xce, Character Control Register 0 (GTFNT_SEL)
  temp = SPI_DataRead();                       
  temp |= cSetb7;
  temp &= cClrb6;
  temp &= cClrb5;
  SPI_DataWrite(temp);
}


/**
 * @brief Font Selection for External CGROM Genitop's GT20L24F6Y IC
 *        
 *        REG [CEh] GT Character ROM Select (GTFNT_SEL)
 *                  bit [7-5] GT Serial Character ROM Select (Genitop's Inc.)
 *                            0b000 : Circuito Integrado External CGROM GT21L16T1W
 *                            0b001 : Circuito Integrado External CGROM GT30L16U2W
 *                            0b010 : Circuito Integrado External CGROM GT30L24T3Y
 *                            0b011 : Circuito Integrado External CGROM GT30L24M1Z
 *                            0b100 : Circuito Integrado External CGROM GT30L32S4W
 *                            0b101 : Circuito Integrado External CGROM GT20L24F6Y
 *                            0b110 : Circuito Integrado External CGROM GT21L24S1W
 *                            
 * @param None
 *
 * @note None
 */
void Panel_RA8889::GTFont_Select_GT20L24F6Y(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_GTFNT_SEL);                      //0xce, Character Control Register 0 (GTFNT_SEL)
  temp = SPI_DataRead();                       
  temp |= cSetb7;
  temp &= cClrb6;
  temp |= cSetb5;
  SPI_DataWrite(temp);
}


/**
 * @brief Font Selection for External CGROM Genitop's GT21L24S1W IC
 *        
 *        REG [CEh] GT Character ROM Select (GTFNT_SEL)
 *                  bit [7-5] GT Serial Character ROM Select (Genitop's Inc.)
 *                            0b000 : Circuito Integrado External CGROM GT21L16T1W
 *                            0b001 : Circuito Integrado External CGROM GT30L16U2W
 *                            0b010 : Circuito Integrado External CGROM GT30L24T3Y
 *                            0b011 : Circuito Integrado External CGROM GT30L24M1Z
 *                            0b100 : Circuito Integrado External CGROM GT30L32S4W
 *                            0b101 : Circuito Integrado External CGROM GT20L24F6Y
 *                            0b110 : Circuito Integrado External CGROM GT21L24S1W
 *                            
 * @param None
 *
 * @note None
 */
void Panel_RA8889::GTFont_Select_GT21L24S1W(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_GTFNT_SEL);                      //0xce, Character Control Register 0 (GTFNT_SEL)
  temp = SPI_DataRead();                       
  temp |= cSetb7;
  temp |= cSetb6;
  temp &= cClrb5;
  SPI_DataWrite(temp);
}


//================================================================================
//
// [0xCF] GT Character ROM Control register (GTFNT_CR)
//
//================================================================================


/**
 * @brief Genitop's Character Set and Decoder
 *
 * @verbatim  
 *        REG [CFh] GT Character ROM Control register (GTFNT_CR)
 *                  bit [7-3] Character sets
 *                            FONT ROM Coding Setting
 *                            For specific GT serial Font ROM, the coding method must be set for decoding.
 *                            
 *                            b. Two byte character code for following character sets:
 *                               0b00000: GB2312
 *                               0b00001: GB12345/GB18030
 *                               0b00010: BIG5
 *                               0b00011: UNICODE
 *                               0b00100: ASCII
 *                               0b00101: UNI-Japanese
 *                               0b00110: JIS0208
 *                               0b00111: Latin/Greek/ Cyrillic / Arabic/Thai/Hebrew
 *
 *                            a. Single byte character code for following character sets:
 *                               0b01000: Korea
 *                               0b10001: ISO-8859-1
 *                               0b10010: ISO-8859-2
 *                               0b10011: ISO-8859-3
 *                               0b10100: ISO-8859-4
 *                               0b10101: ISO-8859-5
 *                               0b10110: ISO-8859-6
 *                               0b10111: ISO-8859-7
 *                               0b11000: ISO-8859-8
 *                               0b11001: ISO-8859-9
 *                               0b11010: ISO-8859-10
 *                               0b11011: ISO-8859-11
 *                               0b11100: ISO-8859-12
 *                               0b11101: ISO-8859-13
 *                               0b11110: ISO-8859-14
 *                               0b11111: ISO-8859-15
 *
 *                  [bit 1-0] GT Character width setting
 *                            00b: for fix width’s font sets. Its width is half of character height.
 *                             Ex. ISO-8859, GB2312, GB12345/GB18030, BIG5,
 *                             UNI-Japanese, JIS0208, Thai.
 *                             Others: variable width for following character sets: ASCII, Latin,
 *                             Greek, Cyrillic & Arabic.
 *                             
 *                             ASCII / Latin/Greek/ Cyrillic / Arabic
 *                                
 *                                      (ASCII)   (Latin/Greek/Cyrillic)      (Arabic)
 *                             00b       Normal            Normal                NA
 *                             01b       Arial         Variable Width     Presentation Forms-A
 *                             10b       Roman               NA           Presentation Forms-B
 *                             11b       Bold                NA                  NA
 * @endverbatim
 * @param temp
 *
 * @note None
 */
void Panel_RA8889::GTFont_SetDecoder(uint8_t temp)
{
  SPI_CmdWrite(REG_GTFNT_CR);                  //0xcf, GT Character ROM Control register (GTFNT_CR)
  SPI_DataWrite(temp);
}


//================================================================================
//
// [0xD0] Character Line gap Setting Register (FLDR)
//
//================================================================================


/**
 * @brief Character Line gap Setting
 *        
 *        REG [D0h] Character Line gap Setting Register (FLDR)
 *                  bit [4-0] Character Line gap Setting
 *                            Setting the character line distance when setting 
 *                            memory font write cursor auto move in active 
 *                            window. (Unit: pixel)
 *                            Color of gap will fill-in background color.
 *                            *** It won’t be enlarged by character enlargement function.
 *                            
 * @param temp
 *
 * @note None
 */
void Panel_RA8889::Font_LineDistance(uint8_t gap)
{
  SPI_CmdWrite(REG_FLDR);                      //0xd0, Character Line gap Setting Register (FLDR)
  SPI_DataWrite(gap);
}


//================================================================================
//
// [0xD1] Character to Character Space Setting Register (F2FSSR)
//
//================================================================================


/**
 * @brief Character Line gap Setting
 *        
 *        REG [D1h] Character to Character Space Setting Register (F2FSSR)
 *                  bit [5-0] Character to Character Space Setting
 *                            00h : 0 pixel
 *                            01h : 1 pixel
 *                            02h : 2 pixels
 *                            ...
 *                            3Fh : 63 pixels
 *                  Color of space will fill-in background color.
 *                  *** It won’t be enlarged by character enlargement function.
 *                            
 * @param pixels: pixels spaces bethween character
 *
 * @note None
 */
void Panel_RA8889::Font_toFontWidthSetting(uint8_t pixels)
{
  SPI_CmdWrite(REG_F2FSSR);          //0xd1,  Character to Character Space Setting Register (F2FSSR)
  SPI_DataWrite(temp);
}


//================================================================================
//
// [0xD2] Foreground Color Register - Red (FGCR)
// [0xD3] Foreground Color Register - Green (FGCG)
// [0xD4] Foreground Color Register - Blue (FGCB)
//
//================================================================================


/**
 * @brief Cor de frente nas componentes Vermelho, Verde e Azul (R,G,B)
 *        
 * @verbatim
 * REG [0xd2] Foreground Color Register - Red (FGCR)
 *            bit [7~0] Foreground Color - Red; for draw, text or color expansion
 *                      256 colors, the register only uses Bit[7:5].
 *                      65K colors, the register uses Bit[7:3].
 *                      16.7M colors, the register uses Bit[7:0].
 * REG [0xd3] Foreground Color Register - Green (FGCG)
 *            bit [7~0] Foreground Color - Green; for draw, text or color expansion
 *                      256 colors, the register only uses Bit[7:5].
 *                      65K colors, the register uses Bit[7:2].
 *                      16.7M colors, the register uses Bit[7:0].
 * REG [0xd4] Foreground Color Register - Blue (FGCB)
 *            bit [7~0] Foreground Color - Blue; for draw, text or color expansion
 *                      256 colors, the register only uses Bit[7:6].
 *                      65K colors, the register uses Bit[7:3].
 *                      16.7M colors, the register uses Bit[7:0].
 * @endverbatim
 *
 * @param red:   componente cor vermelha
 *        green: componente cor verde
 *        blue:  componente cor azul
 *
 * @note use o determinado numero de bits para compor o n umero de cores 256, 
 *       65K e 16.7M cores.
 */
void Panel_RA8889::ForegroundColorRGB(uint8_t red, uint8_t green, uint8_t blue)
{
  SPI_CmdWrite(REG_FGCR);                      //0xd2, Foreground Color Register - Red (FGCR)
  SPI_DataWrite(red);                          //Escreve o formato da cor vermelha 
  SPI_CmdWrite(REG_FGCG);                      //0xd3, Foreground Color Register - Green (FGCG)
  SPI_DataWrite(green);                        //Escreve o formato da cor verde
  SPI_CmdWrite(REG_FGCB);                      //0xd4, Foreground Color Register - Blue (FGCB)
  SPI_DataWrite(blue);                         //Escreve o formato da cor azul
}


/**
 * @brief Cor de frente nas componentes Vermelho, Verde e Azul (RGB3:3:2) de 256 cores
 *        
 * @verbatim
 * Color depht 8bpp
 *
 * REG [0xd2] Foreground Color Register - Red (FGCR)
 *            bit [7~0] Foreground Color - Red; for draw, text or color expansion
 *                      256 colors, the register only uses Bit[7:5].
 *                      65K colors, the register uses Bit[7:3].
 *                      16.7M colors, the register uses Bit[7:0].
 * REG [0xd3] Foreground Color Register - Green (FGCG)
 *            bit [7~0] Foreground Color - Green; for draw, text or color expansion
 *                      256 colors, the register only uses Bit[7:5].
 *                      65K colors, the register uses Bit[7:2].
 *                      16.7M colors, the register uses Bit[7:0].
 * REG [0xd4] Foreground Color Register - Blue (FGCB)
 *            bit [7~0] Foreground Color - Blue; for draw, text or color expansion
 *                      256 colors, the register only uses Bit[7:6].
 *                      65K colors, the register uses Bit[7:3].
 *                      16.7M colors, the register uses Bit[7:0].
 * @endverbatim
 *
 * @param color: entrada de dados no formato R3G3B2 (3 bits para o vermelho, 
 *        3 bits para o verde e 2 bits para o azul.
 *
 *        Dúvida????
 *        A interpretação do formato de cor utilizado na variavel deve acompanhar o formato 
 *        con figurado no registraod REG [0x12]. Use a função "PDATA_ColorFmt(PDATAColorFmt: fmt)"
 *        para definir o modelo de formato do pixel de cores.
 *        ...ou o formato tera que ser sempre RGB?  
 *
 * @note Use o determinado numero de bits para compor o numero de cores 256, 
 *       65K e 16.7M cores.
 *       
 */
void Panel_RA8889::ForegroundColor8bpp(uint8_t color)
{
  SPI_CmdWrite(REG_FGCR);                      //0xd2, Foreground Color Register - Red (FGCR)
  SPI_DataWrite(color);                        //Vermelho so usa o bit de [7~5], o resto ignorado
  SPI_CmdWrite(REG_FGCG);                      //0xd3, Foreground Color Register - Green (FGCG)
  SPI_DataWrite(color << 3);                   //Deslocar a posicao do verde para o bit [7~5], o resto ignorado
  SPI_CmdWrite(REG_FGCB);                      //0xd4, Foreground Color Register - Blue (FGCB)
  SPI_DataWrite(color << 6);                   //Deslocar a posicao do azul para o bit [7~6], o resto ignorado
}


/**
 * @brief Cor de frente nas componentes Vermelho, Verde e Azul (RGB5:6:5) de 65k cores
 *
 * @verbatim        
 * Color depht 16bpp
 *
 * REG [0xd2] Foreground Color Register - Red (FGCR)
 *            bit [7~0] Foreground Color - Red; for draw, text or color expansion
 *                      256 colors, the register only uses Bit[7:5].
 *                      65K colors, the register uses Bit[7:3].
 *                      16.7M colors, the register uses Bit[7:0].
 * REG [0xd3] Foreground Color Register - Green (FGCG)
 *            bit [7~0] Foreground Color - Green; for draw, text or color expansion
 *                      256 colors, the register only uses Bit[7:5].
 *                      65K colors, the register uses Bit[7:2].
 *                      16.7M colors, the register uses Bit[7:0].
 * REG [0xd4] Foreground Color Register - Blue (FGCB)
 *            bit [7~0] Foreground Color - Blue; for draw, text or color expansion
 *                      256 colors, the register only uses Bit[7:6].
 *                      65K colors, the register uses Bit[7:3].
 *                      16.7M colors, the register uses Bit[7:0].
 * @endverbatim
 *
 * @param color: entrada de dados no formato R5G6B5 (5 bits para o vermelho, 
 *        6 bits para o verde e 5 bits para o azul.
 *
 *        Dúvida????
 *        A interpretação do formato de cor utilizado na variavel deve acompanhar o formato 
 *        con figurado no registraod REG [0x12]. Use a função "PDATA_ColorFmt(PDATAColorFmt: fmt)"
 *        para definir o modelo de formato do pixel de cores.
 *        ...ou o formato tera que ser sempre RGB?  
 *
 * @note Use o determinado numero de bits para compor o numero de cores 256, 
 *       65K e 16.7M cores.
 */
void Panel_RA8889::ForegroundColor16bpp(uint16_t color)
{
  SPI_CmdWrite(REG_FGCR);                      //0xd2, Foreground Color Register - Red (FGCR)
  SPI_DataWrite(color >> 8);                   //Desloca os 5 bits do vermelho so usa o bit de [7~3], a sujeira ignorado
  SPI_CmdWrite(REG_FGCG);                      //0xd3, Foreground Color Register - Green (FGCG)
  SPI_DataWrite(color >> 3);                   //Deslocar os 6 bits do verde para o bit [7~2], a sujeira ignorado
  SPI_CmdWrite(REG_FGCB);                      //0xd4, Foreground Color Register - Blue (FGCB)
  SPI_DataWrite(color << 3);                   //Deslocar os 5 bits do azul para o bit [7~3], a sujeira ignorado
}


/**
 * @brief Cor de frente nas componentes Vermelho, Verde e Azul (RGB8:8:8) de 16.7M cores
 *        
 * @verbatim
 * Color depht de 24bpp
 *
 * REG [0xd2] Foreground Color Register - Red (FGCR)
 *            bit [7~0] Foreground Color - Red; for draw, text or color expansion
 *                      256 colors, the register only uses Bit[7:5].
 *                      65K colors, the register uses Bit[7:3].
 *                      16.7M colors, the register uses Bit[7:0].
 * REG [0xd3] Foreground Color Register - Green (FGCG)
 *            bit [7~0] Foreground Color - Green; for draw, text or color expansion
 *                      256 colors, the register only uses Bit[7:5].
 *                      65K colors, the register uses Bit[7:2].
 *                      16.7M colors, the register uses Bit[7:0].
 * REG [0xd4] Foreground Color Register - Blue (FGCB)
 *            bit [7~0] Foreground Color - Blue; for draw, text or color expansion
 *                      256 colors, the register only uses Bit[7:6].
 *                      65K colors, the register uses Bit[7:3].
 *                      16.7M colors, the register uses Bit[7:0].
 * @endverbatim
 *
 * @param color: entrada de dados no formato R8G8B8 (8 bits para o vermelho, 
 *        8 bits para o verde e 8 bits para o azul.
 *
 *        Dúvida????
 *        A interpretação do formato de cor utilizado na variavel deve acompanhar o formato 
 *        con figurado no registraod REG [0x12]. Use a função "PDATA_ColorFmt(PDATAColorFmt: fmt)"
 *        para definir o modelo de formato do pixel de cores.
 *        ...ou o formato tera que ser sempre RGB?  
 *
 * @note Use o determinado numero de bits para compor o numero de cores 256, 
 *       65K e 16.7M cores.
 *       
 */ 
void Panel_RA8889::ForegroundColor24bpp(uint32_t color)
{
  SPI_CmdWrite(REG_FGCR);                      //0xd2, Foreground Color Register - Red (FGCR)
  SPI_DataWrite(color >> 16);                  //Desloca os 8 bits do vermelho, usa o bit de [7~0]
  SPI_CmdWrite(REG_FGCG);                      //0xd3, Foreground Color Register - Green (FGCG)
  SPI_DataWrite(color >> 8);                   //Deslocar os 8 bits do verde, usa os bits [7~0]
  SPI_CmdWrite(REG_FGCB);                      //0xd4, Foreground Color Register - Blue (FGCB)
  SPI_DataWrite(color);                        //Deslocar os 8 bits do azul, usa os bits [7~0]
}


//================================================================================
//
// [0xD5] Background Color Register - Red (BGCR)
// [0xD6] Background Color Register - Green (BGCG)
// [0xD7] Background Color Register - Blue (BGCB)
//
//================================================================================


/**
 * @brief Cor de frente nas componentes Vermelho, Verde e Azul (R,G,B)
 *        
 * @verbatim
 * REG [0xd5] Background Color Register - Red (BGCR)
 *            bit [7~0] Background Color - Red, for Text or color expansion
 *                      256 colors, the register only uses Bit[7:5].
 *                      65K colors, the register uses Bit[7:3].
 *                      16.7M colors, the register uses Bit[7:0].
 * REG [0xd6] Background Color Register - Green (BGCG)
 *            bit [7~0] Background Color - Green, for Text or color expansion
 *                      256 colors, the register only uses Bit[7:5].
 *                      65K colors, the register uses Bit[7:2].
 *                      16.7M colors, the register uses Bit[7:0].
 * REG [0xd7] Background Color Register - Blue (BGCB)
 *            bit [7~0] Background Color - Blue, for Text or color expansion
 *                      256 colors, the register only uses Bit[7:6].
 *                      65K colors, the register uses Bit[7:3].
 *                      16.7M colors, the register uses Bit[7:0].
 *
 * ***Note: No matter background transparency is enabled or not, don’t 
 *    set same value with Foreground Color otherwise image or text will 
 *    become a square with Foreground Color even BTE function.   
 *
 * *** Note: If user wants to change rotate attribute, character line gap, 
 *     character-to-character space, foreground color, background color 
 *     and Text/graphic mode setting, please make sure core_busy (fontwr_
 *     busy) status bit is low.
 * @endverbatim
 *
 * @param red:   componente cor vermelha
 *        green: componente cor verde
 *        blue:  componente cor azul
 *
 * @note Use o determinado numero de bits para compor o numero de cores 256, 
 *       65K e 16.7M cores.
 */
void Panel_RA8889::BackgroundColorRGB(uint8_t red, uint8_t green, uint8_t blue)
{
  SPI_CmdWrite(REG_BGCR);                      //0xd5, Background Color Register - Red (BGCR)
  SPI_DataWrite(red);                          //Escreve o formato da cor vermelha 
  SPI_CmdWrite(REG_BGCG);                      //0xd6, Background Color Register - Green (BGCG)
  SPI_DataWrite(green);                        //Escreve o formato da cor verde
  SPI_CmdWrite(REG_BGCB);                      //0xd7, Background Color Register - Blue (BGCB)
  SPI_DataWrite(blue);                         //Escreve o formato da cor azul
}


/**
 * @brief Cor de fundo nas componentes Vermelho, Verde e Azul (RGB3:3:2) de 256 cores
 *        
 * @verbatim
 * Color depht 8bpp
 * 
 * REG [0xd5] Background Color Register - Red (BGCR)
 *            bit [7~0] Background Color - Red, for Text or color expansion
 *                      256 colors, the register only uses Bit[7:5].
 *                      65K colors, the register uses Bit[7:3].
 *                      16.7M colors, the register uses Bit[7:0].
 * REG [0xd6] Background Color Register - Green (BGCG)
 *            bit [7~0] Background Color - Green, for Text or color expansion
 *                      256 colors, the register only uses Bit[7:5].
 *                      65K colors, the register uses Bit[7:2].
 *                      16.7M colors, the register uses Bit[7:0].
 * REG [0xd7] Background Color Register - Blue (BGCB)
 *            bit [7~0] Background Color - Blue, for Text or color expansion
 *                      256 colors, the register only uses Bit[7:6].
 *                      65K colors, the register uses Bit[7:3].
 *                      16.7M colors, the register uses Bit[7:0].
 *
 * ***Note: No matter background transparency is enabled or not, don’t 
 *    set same value with Foreground Color otherwise image or text will 
 *    become a square with Foreground Color even BTE function.   
 *
 * *** Note: If user wants to change rotate attribute, character line gap, 
 *     character-to-character space, foreground color, background color 
 *     and Text/graphic mode setting, please make sure core_busy (fontwr_
 *     busy) status bit is low.
 * @endverbatim
 *
 * @param color: entrada de dados no formato R3G3B2 (3 bits para o vermelho, 
 *        3 bits para o verde e 2 bits para o azul.
 *
 *        Dúvida????
 *        A interpretação do formato de cor utilizado na variavel deve acompanhar o formato 
 *        con figurado no registraod REG [0x12]. Use a função "PDATA_ColorFmt(PDATAColorFmt: fmt)"
 *        para definir o modelo de formato do pixel de cores.
 *        ...ou o formato tera que ser sempre RGB?  
 *
 * @note Use o determinado numero de bits para compor o numero de cores 256, 
 *       65K e 16.7M cores.
 */
void Panel_RA8889::BackgroundColor8bpp(uint8_t color)
{
  SPI_CmdWrite(REG_BGCR);                      //0xd5, Background Color Register - Red (BGCR)
  SPI_DataWrite(color);                        //Vermelho so usa o bit de [7~5], o resto ignorado
  SPI_CmdWrite(REG_BGCG);                      //0xd6, Background Color Register - Green (BGCG)
  SPI_DataWrite(color << 3);                   //Deslocar a posicao do verde para o bit [7~5], o resto ignorado
  SPI_CmdWrite(REG_BGCB);                      //0xd7, Background Color Register - Blue (BGCB)
  SPI_DataWrite(color << 6);                   //Deslocar a posicao do azul para o bit [7~6], o resto ignorado
}


/**
 * @brief Cor de fundo nas componentes Vermelho, Verde e Azul (RGB5:6:5) de 65k cores
 *        
 * @verbatim 
 * Color depht de 16bpp
 *
 * REG [0xd5] Background Color Register - Red (BGCR)
 *            bit [7~0] Background Color - Red, for Text or color expansion
 *                      256 colors, the register only uses Bit[7:5].
 *                      65K colors, the register uses Bit[7:3].
 *                      16.7M colors, the register uses Bit[7:0].
 * REG [0xd6] Background Color Register - Green (BGCG)
 *            bit [7~0] Background Color - Green, for Text or color expansion
 *                      256 colors, the register only uses Bit[7:5].
 *                      65K colors, the register uses Bit[7:2].
 *                      16.7M colors, the register uses Bit[7:0].
 * REG [0xd7] Background Color Register - Blue (BGCB)
 *            bit [7~0] Background Color - Blue, for Text or color expansion
 *                      256 colors, the register only uses Bit[7:6].
 *                      65K colors, the register uses Bit[7:3].
 *                      16.7M colors, the register uses Bit[7:0].
 *
 * ***Note: No matter background transparency is enabled or not, don’t 
 *    set same value with Foreground Color otherwise image or text will 
 *    become a square with Foreground Color even BTE function.   
 *
 * *** Note: If user wants to change rotate attribute, character line gap, 
 *     character-to-character space, foreground color, background color 
 *     and Text/graphic mode setting, please make sure core_busy (fontwr_
 *     busy) status bit is low.
 * @endverbatim
 *
 * @param color: entrada de dados no formato R5G6B5 (5 bits para o vermelho, 
 *        6 bits para o verde e 5 bits para o azul.
 *
 *        Dúvida????
 *        A interpretação do formato de cor utilizado na variavel deve acompanhar o formato 
 *        con figurado no registraod REG [0x12]. Use a função "PDATA_ColorFmt(PDATAColorFmt: fmt)"
 *        para definir o modelo de formato do pixel de cores.
 *        ...ou o formato tera que ser sempre RGB?  
 *
 * @note Use o determinado numero de bits para compor o numero de cores 256, 
 *       65K e 16.7M cores.
 */
void Panel_RA8889::BackgroundColor16bpp(uint16_t color)
{
  SPI_CmdWrite(REG_BGCR);                      //0xd5, Background Color Register - Red (BGCR)
  SPI_DataWrite(color >> 8);                   //Desloca os 5 bits do vermelho so usa o bit de [7~3], a sujeira ignorado
  SPI_CmdWrite(REG_BGCG);                      //0xd6, Background Color Register - Green (BGCG)
  SPI_DataWrite(color >> 3);                   //Deslocar os 6 bits do verde para o bit [7~2], a sujeira ignorado 
  SPI_CmdWrite(REG_BGCB);                      //0xd7, Background Color Register - Blue (BGCB)
  SPI_DataWrite(color << 3);                   //Deslocar os 5 bits do azul para o bit [7~3], a sujeira ignorado
}


/**
 * @brief Cor de fundo nas componentes Vermelho, Verde e Azul (RGB8:8:8) de 16.7M cores
 *        
 * @verbatim
 * Color depht 24bpp
 *
 * REG [0xd5] Background Color Register - Red (BGCR)
 *            bit [7~0] Background Color - Red, for Text or color expansion
 *                      256 colors, the register only uses Bit[7:5].
 *                      65K colors, the register uses Bit[7:3].
 *                      16.7M colors, the register uses Bit[7:0].
 * REG [0xd6] Background Color Register - Green (BGCG)
 *            bit [7~0] Background Color - Green, for Text or color expansion
 *                      256 colors, the register only uses Bit[7:5].
 *                      65K colors, the register uses Bit[7:2].
 *                      16.7M colors, the register uses Bit[7:0].
 * REG [0xd7] Background Color Register - Blue (BGCB)
 *            bit [7~0] Background Color - Blue, for Text or color expansion
 *                      256 colors, the register only uses Bit[7:6].
 *                      65K colors, the register uses Bit[7:3].
 *                      16.7M colors, the register uses Bit[7:0].
 *
 * ***Note: No matter background transparency is enabled or not, don’t 
 *    set same value with Foreground Color otherwise image or text will 
 *    become a square with Foreground Color even BTE function.   
 *
 * *** Note: If user wants to change rotate attribute, character line gap, 
 *     character-to-character space, foreground color, background color 
 *     and Text/graphic mode setting, please make sure core_busy (fontwr_
 *     busy) status bit is low.
 * @endverbatim
 *
 * @param color: entrada de dados no formato R8G8B8 (8 bits para o vermelho, 
 *        8 bits para o verde e 8 bits para o azul.
 *
 *        Dúvida????
 *        A interpretação do formato de cor utilizado na variavel deve acompanhar o formato 
 *        con figurado no registraod REG [0x12]. Use a função "PDATA_ColorFmt(PDATAColorFmt: fmt)"
 *        para definir o modelo de formato do pixel de cores.
 *        ...ou o formato tera que ser sempre RGB?  
 *
 * @note Use o determinado numero de bits para compor o numero de cores 256, 
 *       65K e 16.7M cores.
 */ 
void Panel_RA8889::BackgroundColor24bpp(uint32_t color)
{
  SPI_CmdWrite(REG_BGCR);                      //0xd5, Background Color Register - Red (BGCR)
  SPI_DataWrite(color >> 16);                  //Desloca os 8 bits do vermelho, usa o bit de [7~0]
  SPI_CmdWrite(REG_BGCG);                      //0xd6, Background Color Register - Green (BGCG)
  SPI_DataWrite(color >> 8);                   //Deslocar os 8 bits do verde, usa os bits [7~0]
  SPI_CmdWrite(REG_BGCB);                      //0xd7, Background Color Register - Blue (BGCB)
  SPI_DataWrite(color);                        //Deslocar os 8 bits do azul, usa os bits [7~0]
}


//================================================================================
//
// [0xDB] CGRAM Start Address 0 (CGRAM_STR0)
// [0xDC] CGRAM Start Address 1 (CGRAM_STR1)
// [0xDD] CGRAM Start Address 2 (CGRAM_STR2)
// [0xDE] CGRAM Start Address 3 (CGRAM_STR3)
//
//================================================================================


/**
 * @brief CGRAM Start Address [31:0]
 *        
 * @verbatim
 * REG [0xdb] CGRAM Start Address 0 (CGRAM_STR0)
 *            bit [7~0] CGRAM START ADDRESS [7:0]
 *            User-defined Characters space
 *            User must use canvas image setting to organize CGRAM data and 
 *            set CGRAM address to tell engine where to fetch CGRAM data.
 *
 * REG [0xdc] CGRAM Start Address 1 (CGRAM_STR1)
 *            bit [7~0] CGRAM START ADDRESS [15:8]
 *            User-defined Characters space
 *            User must use canvas image setting to organize CGRAM data and 
 *            set CGRAM address to tell engine where to fetch CGRAM data.
 *
 * REG [0xdd] CGRAM Start Address 2 (CGRAM_STR2)
 *            bit [7~0] CGRAM START ADDRESS [23:16]
 *            User-defined Characters space
 *            User must use canvas image setting to organize CGRAM data and 
 *            set CGRAM address to tell engine where to fetch CGRAM data.
 *
 * REG [0xde] CGRAM Start Address 3 (CGRAM_STR3)
 *            bit [7-0] CGRAM START ADDRESS [31:24]
 *            User-defined Characters space
 *            User must use canvas image setting to organize CGRAM data and 
 *            set CGRAM address to tell engine where to fetch CGRAM data.
 *
 * *** Note: If user wants to change rotate attribute, character line gap, 
 *     character-to-character space, foreground color, background color and 
 *     Text/graphic mode setting, please make sure core_busy (fontwr_busy) 
 *     status bit is low.
 * @endverbatim
 *
 * @param addr
 *
 * @note None
 */ 
void Panel_RA8889::CGRAM_StartAddress(uint32_t addr)
{
  SPI_CmdWrite(REG_CGRAM_STR0);                //0xdb, CGRAM Start Address 0 (CGRAM_STR0)
  SPI_DataWrite(addr);                         
  SPI_CmdWrite(REG_CGRAM_STR1);                //0xdc, CGRAM Start Address 1 (CGRAM_STR1)
  SPI_DataWrite(addr >> 8);                    
  SPI_CmdWrite(REG_CGRAM_STR2);                //0xdd,  CGRAM Start Address 2 (CGRAM_STR2)
  SPI_DataWrite(addr >> 16);                   
  SPI_CmdWrite(REG_CGRAM_STR3);                //0xde, CGRAM Start Address 3 (CGRAM_STR3)
  SPI_DataWrite(addr >> 24);                   
}


//================================================================================
//
// [0xDF] Power Management register (PMU)
//
//================================================================================


/**
 * @brief Entrar em modo normal de operação de energia
 *        
 * @verbatim
 * REG [0xdf] Power Management register (PMU)
 *            bit [7] Enter Power saving state
 *                    0: Normal state or wakeup from power saving state
 *                    1: Enter power saving state. 
 *                    Note:
 *                    There are 3 ways to wakeup from power saving state:
 *                    External interrupt event, Key Scan wakeup, Software wakeup.
 *                    Writing 0 to this bit will cause a software wakeup. It will be cleared until chip resume. MPU must wait until system quit from power saving state to allow writing other registers. User may check this bit or check status bit [1] (power saving status bit) to check whether chip back to normal operation.
 *            bit [6-2] NA
 *            bit [1-0] Power saving Mode definition
 *                      0b00: NA
 *                      0b01: Standby Mode
 *                            CCLK & PCLK will stop, MCLK keep MPLL clock
 *                      0b10: Suspend Mode
 *                            CCLK & PCLK will stop, MCLK switch to OSC clock
 *                      0b11: Sleep Mode
 *                            All clock & PLL will stop
 * @endverbatim
 *
 * @param addr
 *
 * @note None
 */
void Panel_RA8889::Power_NormalMode(void)
{
  SPI_CmdWrite(REG_PMU);                       //0xdf, Power Management register (PMU)
  SPI_DataWrite(0x00);                         //Limpa bits [7,1,0]
  IC_WaitReady();                              //Aguarda até está pronto
}


/**
 * @brief Entrar em modo standby de operação de economia de energia
 *        
 * @verbatim
* REG [0xdf] Power Management register (PMU)
 *            bit [7] Enter Power saving state
 *                    0: Normal state or wakeup from power saving state
 *                    1: Enter power saving state. 
 *                    Note:
 *                    There are 3 ways to wakeup from power saving state:
 *                    External interrupt event, Key Scan wakeup, Software wakeup.
 *                    Writing 0 to this bit will cause a software wakeup. It will be cleared until chip resume. MPU must wait until system quit from power saving state to allow writing other registers. User may check this bit or check status bit [1] (power saving status bit) to check whether chip back to normal operation.
 *            bit [6-2] NA
 *            bit [1-0] Power saving Mode definition
 *                      0b00: NA
 *                      0b01: Standby Mode
 *                            CCLK & PCLK will stop, MCLK keep MPLL clock
 *                      0b10: Suspend Mode
 *                            CCLK & PCLK will stop, MCLK switch to OSC clock
 *                      0b11: Sleep Mode
 *                            All clock & PLL will stop
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::Power_SavingStandbyMode(void)
{
  SPI_CmdWrite(REG_PMU);                       //0xdf, Power Management register (PMU)
  LCD_DataWrite(cSetb7 | cSetb0 0x81);         //bit 7 = 1 (power saving), bit [1-0] = 01 (standby)
}


/**
 * @brief Entrar em modo suspensão de operação de economia de energia
 *        
 * @verbatim
* REG [0xdf] Power Management register (PMU)
 *            bit [7] Enter Power saving state
 *                    0: Normal state or wakeup from power saving state
 *                    1: Enter power saving state. 
 *                    Note:
 *                    There are 3 ways to wakeup from power saving state:
 *                    External interrupt event, Key Scan wakeup, Software wakeup.
 *                    Writing 0 to this bit will cause a software wakeup. It will be cleared until chip resume. MPU must wait until system quit from power saving state to allow writing other registers. User may check this bit or check status bit [1] (power saving status bit) to check whether chip back to normal operation.
 *            bit [6-2] NA
 *            bit [1-0] Power saving Mode definition
 *                      0b00: NA
 *                      0b01: Standby Mode
 *                            CCLK & PCLK will stop, MCLK keep MPLL clock
 *                      0b10: Suspend Mode
 *                            CCLK & PCLK will stop, MCLK switch to OSC clock
 *                      0b11: Sleep Mode
 *                            All clock & PLL will stop
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::Power_SavingSuspendMode(void)
{
  SPI_CmdWrite(REG_PMU);                       //0xdf, Power Management register (PMU)
  LCD_DataWrite(0x82);                         //bit 7 = 1 (power saving), bit [1-0] = 10 (suspend)
}


/**
 * @brief Entrar em modo de dormência de operação economia de economia de energia
 *        
 * @verbatim
* REG [0xdf] Power Management register (PMU)
 *            bit [7] Enter Power saving state
 *                    0: Normal state or wakeup from power saving state
 *                    1: Enter power saving state. 
 *                    Note:
 *                    There are 3 ways to wakeup from power saving state:
 *                    External interrupt event, Key Scan wakeup, Software wakeup.
 *                    Writing 0 to this bit will cause a software wakeup. It will be cleared until chip resume. MPU must wait until system quit from power saving state to allow writing other registers. User may check this bit or check status bit [1] (power saving status bit) to check whether chip back to normal operation.
 *            bit [6-2] NA
 *            bit [1-0] Power saving Mode definition
 *                      0b00: NA
 *                      0b01: Standby Mode
 *                            CCLK & PCLK will stop, MCLK keep MPLL clock
 *                      0b10: Suspend Mode
 *                            CCLK & PCLK will stop, MCLK switch to OSC clock
 *                      0b11: Sleep Mode
 *                            All clock & PLL will stop
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::Power_SavingSleepMode(void)
{
  SPI_CmdWrite(REG_PMU);                       //0xdf, Power Management register (PMU)
  LCD_DataWrite(0x03);                         //bit [1-0] = 11 (sleep)
  SPI_CmdWrite(REG_PMU);                       //0xdf, Power Management register (PMU)
  LCD_DataWrite(0x83);                         //bit 7 = 1 (power saving), bit [1-0] = 11 (sleep)
}


//================================================================================
//
// [0xE5] IIC Master Clock Pre-scale Register 0 (IICMCPR0)
// [0xE6] IIC Master Clock Pre-scale Register 1 (IICMCPR1)
// [0xE7] IIC Master Transmit Register (IICMTXR)
// [0xE8] IIC Master Receiver Register (IICMRXR)
//
//================================================================================


/**
 * @brief I2C Master Clock Pre-Scale
 *        
 * @verbatim
 * REG [0xe5] IIC Master Clock Pre-scale Register 0 (IICMCPR0)
 *            bit [7-0] IIC Master Clock Pre-scale [7:0]
 *                      XSCL = CCLK / (5*(Pre-scale + 2))
 * 
 * REG [0xe6] IIC Master Clock Pre-scale Register 1 (IICMCPR1)
 *            bit [7-0] IIC Master Clock Pre-scale [15:8]
 *                      XSCL = CCLK / (5*(Pre-scale + 2))
 * @endverbatim
 *
 * @param prescale: valor do pre-scale
 *
 * @note Only RA8889
 *       bits [15:8][7:0] XSCL = CCLK / (5*(Pre-scale + 2))
 */
void Panel_RA8889::I2CM_ClockPrescale(uint16_t prescale)
{
  SPI_CmdWrite(REG_IICMCPR0);                  //0xe5, IIC Master Clock Pre-scale Register 0 (IICMCPR0)
  SPI_DataWrite(prescale);                     //byte baixo de pre-scale
  SPI_CmdWrite(REG_IICMCPR1);                  //0xe6, IIC Master Clock Pre-scale Register 1 (IICMCPR1)
  SPI_DataWrite(prescale >> 8);                //byte alto de pre-scale
}


/**
 * @brief I2C Master Transmit Data
 *        
 * @verbatim
 * REG [0xe7] IIC Master Transmit Register (IICMTXR)
 *            bit [7-0] IIC Master Transmit [7:0]
 * @endverbatim
 *
 * @param None
 *
 * @note Only RA8889
 *       
 */
void Panel_RA8889::I2CM_TransmitData(uint8_t data)
{
  SPI_CmdWrite(REG_IICMTXR);       //0xe7, IIC Master Transmit Register (IICMTXR)
  SPI_DataWrite(temp);
}


/**
 * @brief I2C Master Receive Data
 *        
 * @verbatim
 * REG [0xe8] IIC Master Receiver Register (IICMRXR)
 *            bit [7-0] IIC Master Receiver [7:0]
 * @endverbatim
 *
 * @param None
 *
 * @note Only RA8889
 *       
 */
uint8_t Panel_RA8889::I2CM_Receiver_Data(void)
{
  SPI_CmdWrite(REG_IICMRXR);                   //0xe8, IIC Master Receiver Register (IICMRXR)
  return SPI_DataRead();
}


/**
 * @brief Configura a frequência do clock I²C Mestre do RA8889.
 *
 * @param xscl_hz Frequência desejada do I²C (em Hz), ex: 100000 (100 kHz), 400000 (400 kHz).
 *
 * @note A fórmula usada é:
 *       PreScale = (CCLK / (5 * XSCL)) - 2
 *       XSCL = CCLK / (5 * (PreScale + 2))
 *
 *       Onde CCLK é o clock principal do RA8889.
 */
void Panel_RA8889::I2CM_SetFrequency(uint32_t xscl_hz)
{
  // Defina aqui o clock principal do RA8889 (em Hz).
  // Verifique no seu hardware/datasheet qual é o valor exato.
  const uint32_t CCLK = coreclk * 1000000UL;  //Converte MHz para Hz
  
  if (xscl_hz == 0) return; // Evita divisão por zero
  
  // Calcula PreScale conforme a fórmula
  uint32_t prescale = (CCLK / (5 * xscl_hz)) - 2;
  
  // Garante que cabe em 16 bits
  if (prescale > 0xFFFF) prescale = 0xFFFF;
  
  // Chama a função que grava nos registradores do RA8889
  I2CM_ClockPrescale((uint16_t)prescale);
}











//================================================================================
// Funções de Desenho
// 
// 
//================================================================================


/**
 * @brief Desenha um Pixel
 *        
 * @verbatim
 * Envia os bytes de cor de acordo com a configuracao anterior dos registador 
 * de formato de cor. Se for configurado registrador é 8bpp, é enviado 1 byte, 
 * 16bpp enviado 2 bytes... pelo registardor MRWDP que é um portão de entrada 
 * de dados arpa amemoria, que so passa 1 byte por vez.
 * O mecanismo itenrno do RA8889 sabe que precisa receber 1,2 ou 3 bytes para 
 * compeltar a ouperacao. Precisa setar o tipo de color depth antes. 
 * Geralemtne isso é definido na inicializacao do sistema o registrador é 
 * REG[10h] Main/PIP Window Control Register (MPWCTR) bit 3-2.
 * @endverbatim
 *
 * @param (x,y):   Posicao coordenada na tela
 *        color:   cor do pixel
 *
 * @note 
 * O autor escreveu:
 *   this API does not support the case that MCU=16bit, 24bpp and mode 1
 *       
 * Eu escrevi:
 *   Talvez agora tenha suporte para "MCU=16bit, 24bpp and mode 1" pois o 
 *   envio é realziado em 3 bytes para o display colocando o CS em LOW 
 *   (bloqueia barramento SPI) e quando terminar o envio de 3 bytes coloca o 
 *   CS em HIGH (liberando o barramento SPI). Desta forma se cotnrola os 
 *   dados que envia pela SPI. Isso é realizado usando a funcao 
 *   SPI_DataWrite24bpp()
 *
 *
 */
void Panel_RA8889::putPixel(
                            uint16_t x,      // x of coordinate
                            uint16_t y,      // y of coordinate
                            uint32_t color   //formato 8bpp:R3G3B2, 16bpp:R5G6B5 ou 24bpp:R8G8B8
                           )
{
  Goto_Pixel_XY(x, y);                         //Posiciona o pixel na tela
  SPI_CmdWrite(REG_MRWDP);                     //0x04, Memory Data Read/Write Port (MRWDP)
  Wait_WriteFIFO_NotFull();                    //espera por algum outros proce3ssamento anterior na FIFO
  #ifdef COLOR_DEPTH_16
  SPI_DataWrite16bpp(color);
  #endif
  #ifdef COLOR_DEPTH_24
  SPI_DataWrite24bpp(color);
  #endif
  #if USE_XNWAIT
  Wait_WriteFIFO_NotFull();                    // espera no final do pixel ou bloco
  #endif
}


/**
 * @brief Desenha um Pixel
 *        
 *
 *
 * @param (x,y):   Posicao coordenada na tela
 *        color:   cor do pixel
 *
 * @note A cor do pixel vai depender do Color Depth escolhido
 *       8bpp:  R3G3B2
 *       16bpp: R5G6B5
 *       24bpp: R8G8B8
 *
 */
void Panel_RA8889::DrawPixel(uint16_t x, uint16_t y, uint32_t color)
{  
  putPixel(x, y, color);
}


/**
 * @brief Desenha uma linha
 *        
 *        Color depht de 16bpp
 *
 *
 * @param (x1,y1):   primeiro ponto de coordenada na tela
 *        (x2,y2):   segundo ponto de coordenada na tela
 *        color: cor da linha
 *
 * @note A cor da linha vai depender do Color Depth escolhido
 *       8bpp:  R3G3B2
 *       16bpp: R5G6B5
 *       24bpp: R8G8B8
 *
 */ 
void Panel_RA8889::DrawLine(uint16_t x1,
                            uint16_t y1,
                            uint16_t x2,
                            uint16_t y2,
                            uint32_t color
                           )
{
  #ifdef COLOR_DEPTH_16
  ForegroundColor16bpp(forecolor);
  #endif
  #ifdef COLOR_DEPTH_24
  ForegroundColor24bpp(forecolor);
  #endif
  Point1_XY(x1, y1);
  Point2_XY(x2, y2);
  LineMode_Start();
  CoreTask_WaitReady();
}


/**
 * @brief Desenha um retangulo/quadrado
 *        
 *        Color depht de 16bpp
 *
 *
 * @param (x1,y1):   primeiro ponto de coordenada na tela
 *        (x2,y2):   segundo ponto de coordenada na tela
 *        forecolor: cor de frente de preenchimento
 *        bfill:     fazer preenchimento, default é false (sem preenchimento)
 *
 * @note A cor de preenchimeno/linha vai depender do Color Depth escolhido
 *       8bpp:  R3G3B2
 *       16bpp: R5G6B5
 *       24bpp: R8G8B8
 *
 */ 
void Panel_RA8889::DrawSquare ( uint16_t x1,
                                uint16_t y1,
                                uint16_t x2,
                                uint16_t y2,
                                uint32_t forecolor,
                                bool bfill = false
                              )
{
  #ifdef COLOR_DEPTH_16
  ForegroundColor16bpp(forecolor);
  #endif
  #ifdef COLOR_DEPTH_24
  ForegroundColor24bpp(forecolor);
  #endif
  Point1_XY(x1, y1);
  Point2_XY(x2, y2);
  SquareMode_Start(bfill);
  CoreTask_WaitReady();  
}


/**
 * @brief Desenha um triangulo
 *        
 *        Color depht de 16bpp
 *
 *
 * @param (x1,y1):   primeiro ponto de coordenada na tela
 *        (x2,y2):   segundo ponto de coordenada na tela
 *        (x3,y3):   terceiro ponto de coordenada na tela
 *        forecolor: cor da linha/preenchimento
 *        bfill:     fazer preenchimento, default é false (sem preenchimento)
 *
 * @note A cor da linha vai depender do Color Depth escolhido
 *       8bpp:  R3G3B2
 *       16bpp: R5G6B5
 *       24bpp: R8G8B8
 *
 */
void Panel_RA8889::DrawTriangle(uint16_t x1,
                                uint16_t y1,
                                uint16_t x2,
                                uint16_t y2,
                                uint16_t x3,
                                uint16_t y3,
                                uint32_t forecolor,
                                bool bfill = false
                               )
{
  #ifdef COLOR_DEPTH_16
  ForegroundColor16bpp(forecolor);
  #endif
  #ifdef COLOR_DEPTH_24
  ForegroundColor24bpp(forecolor);
  #endif
  Point1_XY(x1, y1);
  Point2_XY(x2, y2);
  Point3_XY(x3, y3);
  TriangleMode_Start(bfill);
  CoreTask_WaitReady();  
}


/**
 * @brief Desenha um circulo
 *        
 *        Color depht de 16bpp
 *
 * @param (x1,y1):   coordenada de posicionamento central do circulo
 *        R:         raio do circulo 
 *        forecolor: cor de frente de preenchimento
 *        bfill:     fazer preenchimento, default é false (sem preenchimento)
 *
 * @note A cor de preenchimeno/linha vai depender do Color Depth escolhido
 *       8bpp:  R3G3B2
 *       16bpp: R5G6B5
 *       24bpp: R8G8B8
 *
 */ 
void Panel_RA8889::DrawCircle (uint16_t x1,
                               uint16_t y1,
                               uint16_t R,
                               uint32_t forecolor,
                               bool bfill = false
                              )
{
  #ifdef COLOR_DEPTH_16
  ForegroundColor16bpp(forecolor);
  #endif
  #ifdef COLOR_DEPTH_24
  ForegroundColor24bpp(forecolor);
  #endif
  Center_XY(x1,y1);
  Radius_R(R);
  CircleMode_Start(bfill);
  CoreTask_WaitReady();  
}


/**
 * @brief Desenha uma elipse
 *        
 *        Color depht de 16bpp
 *
 * @param (x1,y1):   coordenada de posicionamento central da elispse
 *        Rx:        Raio de largura do eixo x
 *        Ry:        Raio de comprimento do eixo y
 *        forecolor: cor da linha
 *        bfill:     fazer preenchimento, default é false (sem preenchimento)
 *
 * @note A cor de preenchimeno/linha vai depender do Color Depth escolhido
 *       8bpp:  R3G3B2
 *       16bpp: R5G6B5
 *       24bpp: R8G8B8
 *
 */
void Panel_RA8889::DrawEllipse (uint16_t x1,
                                uint16_t y1,
                                uint16_t Rx,
                                uint16_t Ry,
                                uint32_t forecolor,
                                bool bfill = false
                               )
{
  #ifdef COLOR_DEPTH_16
  ForegroundColor16bpp(forecolor);
  #endif
  #ifdef COLOR_DEPTH_24
  ForegroundColor24bpp(forecolor);
  #endif
  Center_XY(x1, y1);
  Radius_RxRy(Rx, Ry);
  EllipseMode_Start(bfill);
  CoreTask_WaitReady();
}


/**
 * @brief Desenha uma curva com curvatura esquerda/superior
 *        
 *        Color depht de 16bpp
 *
 * @param (x1,y1):   coordenada de posicionamento central da curvatura
 *        Rx:        Raio de largura do eixo x
 *        Ry:        Raio de comprimento do eixo y
 *        forecolor: cor da linha
 *        bfill:     fazer preenchimento, default é false (sem preenchimento)
 *
 * @note A cor de preenchimeno/linha vai depender do Color Depth escolhido
 *       8bpp:  R3G3B2
 *       16bpp: R5G6B5
 *       24bpp: R8G8B8
 *
 */ 
void Panel_RA8889::DrawCurveLeftUp(uint16_t x1,
                                   uint16_t y1,
                                   uint16_t Rx,
                                   uint16_t Ry,
                                   uint32_t forecolor,
                                   bool bfill = false
                                  )
{
  #ifdef COLOR_DEPTH_16
  ForegroundColor16bpp(forecolor);
  #endif
  #ifdef COLOR_DEPTH_24
  ForegroundColor24bpp(forecolor);
  #endif
  Center_XY(x1, y1);
  Radius_RxRy(Rx, Ry);
  CurveLeftUpMode_Start(bfill);
  CoreTask_WaitReady();  
}


/**
 * @brief Desenha uma curva com curvatura direita/inferior
 *        
 *        Color depht de 16bpp
 *
 * @param (x1,y1):   coordenada de posicionamento central da curvatura
 *        Rx:        Raio de largura do eixo x
 *        Ry:        Raio de comprimento do eixo y
 *        forecolor: cor da linha
 *        bfill:     fazer preenchimento, default é false (sem preenchimento)
 *
 * @note A cor de preenchimeno/linha vai depender do Color Depth escolhido
 *       8bpp:  R3G3B2
 *       16bpp: R5G6B5
 *       24bpp: R8G8B8
 *
 */ 
void Panel_RA8889::DrawCurveRightDown(uint16_t x1,
                                      uint16_t y1,
                                      uint16_t Rx,
                                      uint16_t Ry,
                                      uint32_t forecolor,
                                      bool bfill = false
                                     )
{
  #ifdef COLOR_DEPTH_16
  ForegroundColor16bpp(forecolor);
  #endif
  #ifdef COLOR_DEPTH_24
  ForegroundColor24bpp(forecolor);
  #endif
  Center_XY(x1, y1);
  Radius_RxRy(Rx, Ry);
  CurveRightDownMode_Start(bfill);
  CoreTask_WaitReady();
}


/**
 * @brief Desenha uma curva com curvatura direita/superior
 * @verbatim
 *        Color depht de 16bpp
 * @endverbatim
 * @param (x1,y1):   coordenada de posicionamento central da curvatura
 *        Rx:        Raio de largura do eixo x
 *        Ry:        Raio de comprimento do eixo y
 *        forecolor: cor da linha
 *        bfill:     fazer preenchimento, default é false (sem preenchimento)
 *
 * @note A cor de preenchimeno/linha vai depender do Color Depth escolhido
 *       8bpp:  R3G3B2
 *       16bpp: R5G6B5
 *       24bpp: R8G8B8
 *
 */
void Panel_RA8889::DrawCurveRightUp(uint16_t x1,
                                    uint16_t y1,
                                    uint16_t Rx,
                                    uint16_t Ry,
                                    uint32_t forecolor,
                                    bool bfill = false
                                   )
{
  #ifdef COLOR_DEPTH_16
  ForegroundColor16bpp(forecolor);
  #endif
  #ifdef COLOR_DEPTH_24
  ForegroundColor24bpp(forecolor);
  #endif
  Center_XY(x1, y1);
  Radius_RxRy(Rx, Ry);
  CurveRightUpMode_Start(bfill);
  CoreTask_WaitReady();
}


/**
 * @brief Desenha uma curva com curvatura esquerda/inferior
 *        
 *        Color depht de 16bpp
 *
 * @param (x1,y1):   coordenada de posicionamento central da curvatura
 *        Rx:        Raio de largura do eixo x
 *        Ry:        Raio de comprimento do eixo y
 *        forecolor: cor da linha
 *        bfill:     fazer preenchimento, default é false (sem preenchimento)
 *
 * @note A cor de preenchimeno/linha vai depender do Color Depth escolhido
 *       8bpp:  R3G3B2
 *       16bpp: R5G6B5
 *       24bpp: R8G8B8
 *
 */
void Panel_RA8889::DrawCurveLeftDown(uint16_t x1,
                                     uint16_t y1,
                                     uint16_t Rx,
                                     uint16_t Ry,
                                     uint32_t forecolor,
                                     bool bfill = false
                                    )
{
  #ifdef COLOR_DEPTH_16
  ForegroundColor16bpp(forecolor);
  #endif
  #ifdef COLOR_DEPTH_24
  ForegroundColor24bpp(forecolor);
  #endif
  Center_XY(x1, y1);
  Radius_RxRy(Rx, Ry);
  CurveLeftDownMode_Start(bfill); 
  CoreTask_WaitReady();
}


/**
 * @brief Desenha um triangulo
 *        
 *        Color depht de 16bpp
 *
 *
 * @param (x1,y1):   primeiro ponto de coordenada na tela
 *        (x2,y2):   segundo ponto de coordenada na tela
 *        Rx:        Raio de largura do eixo x
 *        Ry:        Raio de comprimento do eixo y
 *        forecolor: cor da linha/preenchimento
 *        bfill:     fazer preenchimento, default é false (sem preenchimento)
 *
 * @note A cor da linha vai depender do Color Depth escolhido
 *       8bpp:  R3G3B2
 *       16bpp: R5G6B5
 *       24bpp: R8G8B8
 *
 */
void Panel_RA8889::DrawCircleSquare(uint16_t x1,
                                    uint16_t y1,
                                    uint16_t x2,
                                    uint16_t y2,
                                    uint16_t Rx,
                                    uint16_t Ry,
                                    uint32_t forecolor,
                                    bool bfill = false
                                   )
{
  #ifdef COLOR_DEPTH_16
  ForegroundColor16bpp(forecolor);
  #endif
  #ifdef COLOR_DEPTH_24
  ForegroundColor24bpp(forecolor);
  #endif
  Point1_XY(x1, y1);
  Point2_XY(x2, y2);
  Radius_RxRy(Rx, Ry);
  CircleSquareMode_Start();
  CoreTask_WaitReady();
}












/**
 * @brief Mostra uma figura
 *        
 *        Color depht de 16bpp (2 bytes por pixel)
 *
 *        Deve se lenvar em consideração a profundicade de cores 8/16/24bpp na matriz da figura
 *        O tamanho da matriz com uma figura de 80 x 80 profundidade de cores de 16bpp será:
 *        Size: Color Depth 16: 80 * 80 * (16 / 8) = 12.800 bytes
 *              Color Depth 24: 80 * 80 * (24 / 8) = 19.200 bytes
 *
 * @param size:   tamanho de bytes da figura da matriz com color depth
 *        *datap  ponteiro para dados no PROGMEM
 *       
 * @note A cor do pixel vai depender do Color Depth definido
 *       8bpp:  R3G3B2
 *       16bpp: R5G6B5
 *       24bpp: R8G8B8
 *
 * Exemplo: const uint8_t pic_80x80[] PROGMEM ={0X19,0X88,0X09,0X48,0X11,0XD2,0X01,...}
 *          ShowPicture(80*80*(16/2), pic_80x80);
 *
 */
void Panel_RA8889::ShowPicture(uint32_t size, const uint8_t *datap)
{   
  uint32_t i;
  SPI_CmdWrite(REG_MRWDP);                            //0x04, Memory Data Read/Write Port (MRWDP)
  for(i=0; i < size; i+=2) {                          //total_bytes = tamanho da image * (byte_per_pixel/8)
	//declare Arduino.h, para usar a funcao pgm_read_byte()
    SPI_DataWrite( pgm_read_byte(&datap[i+1]) );      //Envia cada byte declarados em PROGMEM byte posterior
    Wait_WriteFIFO_NotFull();
    SPI_DataWrite(pgm_read_byte(&datap[i]));          //Envia cada byte declarados em PROGMEM byte anterior
    Wait_WriteFIFO_NotFull();
  }
}


/**
 * @brief Draw Picture
 *        
 *        Color depht de 16bpp (2 bytes por pixel)
 *
 * @param (Wx,Hy):         coordenada na tela
 *        (width, height): dimensão da figura
 *        *datap           Ponteiro para dados no PROGMEM
 *
 * @note Exemplo: const uint8_t pic_80x80[] PROGMEM ={0X19,0X88,0X09,0X48,0X11,0XD2,0X01,...}
 *                DrawPicture(0, 0, 80, 80, pic_80x80);
 *
 */
void Panel_RA8889::DrawPicture(uint16_t Wx, uint16_t Hy, uint16_t width, uint16_t height, const uint8_t *datap)
{
  ActiveWindow_XY(Wx, Hy);
  ActiveWindow_WidhtHeight(width, height);
  GotoPixel_XY(Wx, Hy);
  ShowPicture(width * height * (_bpp / 8), *datap);
}




void Show_String(char *str)
{   
    Text_Mode();     
    SPI_CmdWrite(0x04);
    while(*str != '\0')
    {
      SPI_DataWrite(*str);
      Check_Mem_WR_FIFO_not_Full();
      ++str;   
    }
    Check_2D_Busy();

    Graphic_Mode(); //back to graphic mode;
}


//metodo protegido
void ShowText(char *str)
{   
  Text_Mode();  //<-- remover, avisar que o usuario controla isso 
    
  SPI_CmdWrite(REG_MRWDP);                     //0x04, Memory Data Read/Write Port (MRWDP)
  while(*str != '\0') {                        //Até final de string
    SPI_DataWrite(*str);
    Wait_WriteFIFO_NotFull();
    ++str;
  }
  CoreTask_WaitReady();
  
  Graphic_Mode(); //back to graphic mode;   //<-- remover, avisar que o usuario controla isso 
}


//metodo publico
void Text(char *str)
{
  void ShowText(char *str);
}
