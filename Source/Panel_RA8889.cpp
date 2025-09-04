//Notas:
// substitua "unsigned char" por "uint8_t", embora ambos funcionam do mesmo jeito

/*----------------------------------------------------------------------------/
  Lovyan GFX - Graphics library for embedded devices.

Original Source:
 https://github.com/lovyan03/LovyanGFX/

Licence:
 [FreeBSD](https://github.com/lovyan03/LovyanGFX/blob/master/license.txt)

Author:
 [lovyan03](https://twitter.com/lovyan03)

Contributors:
 [gwiethaus](https://github.com/gwiethaus)
 [ciniml](https://github.com/ciniml)
 [mongonta0716](https://github.com/mongonta0716)
 [tobozo](https://github.com/tobozo)
/----------------------------------------------------------------------------*/
#include "Panel_RA8889.hpp"
#include "../Bus.hpp"
#include "../platforms/common.hpp"
#include "../misc/pixelcopy.hpp"

#include "../platforms/device.hpp"

/*
	Notas para Fazer:
	- Determinar a autoconfiguracao das portas MISO, MOSI, CLK da micrcontroladora
	- Determinar manual das portas MISO, MOSI, CLK da micrcontroladora
	- tipo de comunciacao SPI MODO 0, 1, 2 dependedo da microcontroladora
    - Sistema da porta de itnerrupção para a tela de toque do display
	- Verificar a funcao DrawEnable_AA() deve ser do RA8876, pois no RA8889 deve ser zero
	- usar a variavel _bpp, apra definir a cores de fundo ao inves de usar ForegroundColor_24bpp, usar ForegroundColor()
	  e detnrod esta funcao seleciona entao o nivel de cores pela variavel. O sistema fica dinamico podendo fazer troca de profundicade no momento desejao, 
	  evitando que isso seja fixo em um display mna hora da compilacao. Principalmente em sistema que requerem otimizacao de cores pelo usuario.
	  
	Tarefas:
    Fazendo as funções: void GotoPixelXY(uint16_t Wx, uint16_t Hy)	
	                    void GotoLinearAddr(uint32_t addr)  
						void Goto_Text_XY(unsigned short WX, unsigned short HY)
						void DrawPixel(unsigned short x,unsigned short y,unsigned short color)
                        void Show_picture(unsigned long numbers,const unsigned char *datap)
                        void Show_String(char *str)
						void putPixel(
						

*/


//================================================================================
// Funções auxiliar
//================================================================================


// Template genérico para qualquer enum class
template <typename E>
constexpr auto toValue(E e) -> typename std::underlying_type<E>::type {
    return static_cast<typename std::underlying_type<E>::type>(e);
}


//================================================================================
// Funções Principais de Inicializacao
//================================================================================


/**
 * @brief Construtor da Classe Panel_RA8889
 *
 * @param None
 * 
 * @note None
 */
Panel_RA8889::Panel_RA8889(uint8_t cs, uint8_t rst) {
  _cs     = cs;
  _rst    = rst;
  _width  = LCD_HW;
  _height = LCD_VH;
  _colorfmt = static_cast<uint8_t>PDATAColorFmt::RGB; //iniciar com o formato de cor RGB
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
void Panel_RA8889::PanelResolution(PanelResolution resolution);
{
  if (resolution::r320x240)  {_width=320;  _height=240;}
  if (resolution::r480x272)  {_width=480;  _height=272;}
  if (resolution::r640x480)  {_width=640;  _height=480;}
  if (resolution::r800x480)  {_width=800;  _height=480;}
  if (resolution::r800x600)  {_width=800;  _height=600;}
  if (resolution::r960x540)  {_width=960;  _height=540;}
  if (resolution::r1024x600) {_width=1024; _height=600;}
  if (resolution::r1024x768) {_width=1024; _height=768;}
  if (resolution::r1280x768) {_width=1280; _height=768;}
  if (resolution::r1280x800) {_width=1280; _height=800;}
  if (resolution::r1366x768) {_width=1366; _height=768;}
}


/**
 * @brief Inicializa a configurações básicas do display RA8889.
 *
 * @param None
 *
 * @note None
 */
uint8_t Panel_RA8889::init(void) {

  SPI_Init();
  ChipHardwareReset();
  PLL_WaitReady();
  
  delay(100);
  
  // Aguarda até que a inicialização interna do RA8889 termine
  // Bit 1 do STSR (0x02) = 1 → inicialização em andamento
  // Bit 1 do STSR (0x02) = 0 → inicialização concluída
  while(StatusRead() & 0x02);
  
  //Funcao aberta de void ER_TFTBasic::initial(void)
  //colcoar aqui abaixo as partes  
  PLL_ConfigClocks();                          //Configura clock Pixel/SDRAM/Core PLL
  
  SDRAM_Init();                                //Inicializa a SDRAM
  
//Chip Configuration Register (CCR) [01h]

  TFT_16bit();
  HostDataBus_Select_16bit();                  //Host bus 16bit
      
//Memory Access Control Register (MACR) [02h]

  HostColorDepthFormat(0);                     //Host MPU/MCU I/F 8/16 bits color depth 8/16/24 mode 1
  HostReadMemoryDirection(MemoryDirection::LeftRight_TopBotom);

//Input Control Register (ICR) [03h]

  GraphicMode();
  MemorySelect_SDRAM();

  LCD_SetPanel();                              //Configuração do Panel Screen LCD, de acordo com o tipo do fabricante
  
  Select_MainWindow_16bpp();

  Memory_BlockMode();                          //Set Block mode (X-Y coordination addressing)
  Memory_16bpp_BlockMode(void);                //Set 16bpp Block mode
  
  _bpp =  ColorDepthBPP::bpp16                 //Indica que selecionou Color depth 16bpp
  
}


uint16_t Panel_RA8889::Width(void);
{
  return _width;
}


uint16_t Panel_RA8889::Height(void);
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
void Panel_RA8889::LCD_SetPanel(void);
{

#ifdef EK9713                                  //Fitipower EK9713 800x600/800x480

  //Display Configuration Register (DPCR) [12h]

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
  
#end if
}


//================================================================================
// Funções SPI
//================================================================================


//Inicializa o SPI para a comunicacao com o Display RA8889
void Panel_RA8889::SPI_Init()
{
	pinMode(_cs, OUTPUT);
	SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));
	SPI.begin();
}


//SPISetCS
//Seta o Chip Select
//Use esta função para resetar o CS (false) antes de escrever para o SPI e setar novamente (true) apos escrever para o SPI
void Panel_RA8889::SPISetCS(bool active)
{
  if(active) {
    digitalWrite(_cs, HIGH);  //SS_SET
  else
	digitalWrite(_cs, LOW);   //SS_RESET
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
  SPISetCS(false);    //SS_RESET;
  SPIRwByte(0x00);    //Indica Commando para escrever 
  SPIRwByte(cmd);     //Envia um comando byte para o SPI 
  SPISetCS(true);     //SS_SET;
}


//SPI_DataWrite
//Escreve dados para o SPI
void Panel_RA8889::SPI_DataWrite(uint8_t data)
{
  SPISetCS(false);    //SS_RESET;
  SPIRwByte(0x80);    //Indica Dados para escrever
  SPIRwByte(data);    //Envia um byte de Dado para o SPI
  SPISetCS(true);     //SS_SET;
}


//SPI_DataWritePixel
void Panel_RA8889::SPI_DataWrite_Pixel(uint16_t data)
{
  SPISetCS(false);          //SS_RESET;
  SPIRwByte(0x80);          //Indica Dados para escrever
  SPIRwByte(data);          //Escreve a parte baixa da palavra
  SPISetCS(true);           //SS_SET;
  
  SPISetCS(false);          //SS_RESET;
  SPIRwByte(0x80);          //Indica Dados para escrever
  SPIRwByte(data >> 8);     //escreve a parte alta da palavra
  SPISetCS(true);           //SS_SET;
}


//SPI_DataRead
//Ler um byte de dados no SPI
uint8_t Panel_RA8889::SPI_DataRead(void)
{
  uint8_t temp = 0;
  SPISetCS(false);          //SS_RESET;
  SPIRwByte(0xc0);
  temp = SPIRwByte(0x00);
  SPISetCS(true);           //SS_SET;
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
  SPISetCS(false);       //SS_RESET;
  SPIRwByte(0x40);   
  temp = SPIRwByte(REG_STSR);
  SPISetCS(true);        //SS_SET;
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
	uint8_t result;
	SPI_CmdWrite(Cmd);
	temp = SPI_DataRead();
	return result;
}


//Antigo HW_Reset(void)
/**
 * Executa um reset de hardware no RA8889 através do pino RESET.
 *
 * Mantém o pino de reset (configurado em _rst) em nível baixo por 500 ms para
 * garantir que o chip seja reinicializado, depois volta a nível
 * alto por mais 500 ms para concluir o processo de reset.
 *
 * Aplicação:
 * Esse procedimento força o RA8889 a retornar ao estado inicial,
 * sendo útil quando o PLL ou a inicialização interna falham.
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
 *
 * @param true:  IC em modo operação normal e pronto,
 *        false: IC ainda não concluiu a inicializacao
 *
 * @note None
 */
bool Panel_RA8889::IC_WaitReady(void)
{
  unint8_t temp;
  for(unsigned long i = 0; i < 1000000; i++) { //de acordo com o uso, altere o valor de i.
    temp = StatusRead();
    if( (temp & 0x02) == 0x00 ) {return true;}
	delayMicroseconds(1);
  }
  return false;
}


/**
 * @brief Aguarde até as tarefas do sistema estejam prontas
 *              
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
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::CoreTask_WaitReady(void)
{
  uint8_t temp = 0;
  for(unsigned long i = 0; i < 1000000; i++) {  //Ajuste valor de i de acordo com a necessidade
	temp = StatusRead();
    if((temp & 0x08) == 0x00) {break;}
    delayMicroseconds(1);
  } 
}  


/**
 * @brief Aguarde até as tarefas de desenho grafico e texto estejam prontos
 *              
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
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::Draw_WaitReady(void)  {CoreTask_WaitReady();}


/**
 * @brief Aguarde até que a memória de escrita FIFO tenha algo nela
 *              
 *        Status Register (STSR)
 *        bit [7] Host Memory Write FIFO full
 *                0b0 : Memory Write FIFO is not full.
 *                0b1 : Memory Write FIFO is full.
 * 
 *        Only when Memory Write FIFO is not full, MPU may write another one 
 *        pixel.
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
 *        Status Register (STSR)
 *        bit [6] Host Memory Write FIFO empty
 *                0b0 : Memory Write FIFO is not empty.
 *                0b1 : Memory Write FIFO is empty.
 * 
 *        When Memory Write FIFO is empty, MPU may write 8bpp data 64
 *        pixels, or 16bpp data 32 pixels, 24bpp data 16 pixels directly.
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
    if( (temp & 0x40) == 0x40 ){break;}
  }
}


/**
 * @brief Aguarde até que a memória de leitura FIFO Não esteja cheio
 *              
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
    if( (temp & 0x20) == 0x00 ){break;}
}


/**
 * @brief Aguarde até que a memória de leitura FIFO tenha algo para ler
 *              
 *        Status Register (STSR)
 *        bit [4] Host Memory Read FIFO empty
 *                0b0 : Memory Read FIFO is not empty.
 *                0b1 : Memory Read FIFO is empty.
 * 
 *        Dizer que algo não está vazio, quer dizer que tem alguma coisa 
 *        ou esta completo (totalmente cheio)
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
	if( (temp & 0x10) == 0x00 ){break;}
  }
}




//================================================================================
// Funcoes PLL
//================================================================================


/**
 * @brief Aguarda até que o RA8889 finalize sua inicialização interna e o PLL esteja pronto.
 * 
 * Fluxo:
 *  - Verifica o registrador de status (STSR) para saber se a inicialização interna terminou.
 *  - Quando terminado, acessa o Chip Configuration Register (CCR) e checa se o PLL (bit 7) está pronto.
 *  - Se o PLL não estiver pronto, reconfigura-o e tenta novamente.
 *  - Caso o sistema não responda após várias tentativas, executa um reset de hardware e repete o processo.
 * 
 * A função só retorna quando o sistema está estável e pronto para operar.
 */
void Panel_RA8889::PLL_WaitReady(void)
{
  uint8_t count_timeout = 0;
  uint8_t temp = 0;
  bool system_ok = false;
  
  do {
    temp = StatusRead();                       //Read Status Register STSR
    if((temp & 0x02) == 0x00)                  //Veja se o bit 2 esta limpo (0x00=modo de operação normal, evento de inicialização interna terminou)
    {
      
	  delay(2);                                //MCU too fast, necessary
      SPI_CmdWrite(0x01);                      //Access register Chip Configuration Register (CCR)
      delay(2);                                //MCU too fast, necessary
      temp = SPI_DataRead();                   //Leia o CCR 
      if((temp & 0x80) == 0x80)                //Check CCR register's PLL is ready or not (bit 7 = 1)
      {
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
	
    if(system_ok==false && count_timeout==5)   //Sistema ainda nao está pronto e houve timeout
    {
      ChipHardwareReset();                     //*note1, envia um reset novamente
      count_timeout = 0;                       //zera o cotnador de timeout 
    }
	
  } while(system_ok==false);                   //faz enquanto não ficar pronto o sistema
}


/**
 * @brief Configura o PLL do RA8889 para ajustar as frequências de:
 *        - Pixel Clock (SCAN_FREQ)
 *        - SDRAM Clock (DRAM_FREQ)
 *        - Core Clock (CORE_FREQ)
 * 
 *        REG[05h] SCLK PLL Control Register 1 (PPLLC1) - SCAN or PIXEL Clock PLL
 *        REG[07h] MCLK PLL Control Register 1 (MPLLC1) - MEMORY Clock PLL
 *        REG[09h] CCLK PLL Control Register 1 (SPLLC1) - CORE or SYSTEM Clock PLL
 *
 * A função calcula automaticamente o divisor e multiplicador adequado
 * com base no cristal externo (OSC_FREQ) e nas frequências alvo.
 * 
 * Fluxo seguro:
 * 1. Desliga o PLL temporariamente
 * 2. Configura os registros de divisores/multiplicadores
 * 3. Habilita o PLL com os novos valores
 */
void Panel_RA8889::PLL_ConfigClocks(void) 
{
  
  // ---------- Set Pixel/Scan Clock ----------
 
  if(SCAN_FREQ>=63)        //&&(SCAN_FREQ<=100))
  {
	SPI_CmdWrite(REG_PPLLC1);                  //0x05 
	SPI_DataWrite(0x04);                       //PLL Divided by 4
	SPI_CmdWrite(REG_PPLLC2);                  //0x06
	SPI_DataWrite((SCAN_FREQ*4/OSC_FREQ)-1);   //Deve ser de 1~63, 0 é proibido
  }                                            
  if((SCAN_FREQ>=32)&&(SCAN_FREQ<=62))         
  {                                            
	SPI_CmdWrite(REG_PPLLC1);                  //0x05    
	SPI_DataWrite(0x06);                       //PLL Divided by 8
	SPI_CmdWrite(REG_PPLLC2);                  //0x06
	SPI_DataWrite((SCAN_FREQ*8/OSC_FREQ)-1);   //Deve ser de 1~63, 0 é proibido
  }                                            
  if((SCAN_FREQ>=16)&&(SCAN_FREQ<=31))         
  {                                            
	SPI_CmdWrite(REG_PPLLC1);                  //0x05     
	SPI_DataWrite(0x16);                       //PLL Divided by 16
	SPI_CmdWrite(REG_PPLLC2);                  //0x06
	SPI_DataWrite((SCAN_FREQ*16/OSC_FREQ)-1);  //Deve ser de 1~63, 0 é proibido
  }                                            
  if((SCAN_FREQ>=8)&&(SCAN_FREQ<=15))          
  {                                            
	SPI_CmdWrite(REG_PPLLC1);                  //0x05    
	SPI_DataWrite(0x26);                       //PLL Divided by 32
	SPI_CmdWrite(REG_PPLLC2);                  //0x06
	SPI_DataWrite((SCAN_FREQ*32/OSC_FREQ)-1);  //Deve ser de 1~63, 0 é proibido
  }                                            
  if((SCAN_FREQ>0)&&(SCAN_FREQ<=7))            
  {                                            
	SPI_CmdWrite(REG_PPLLC1);                  //0x05    
	SPI_DataWrite(0x36);                       //PLL Divided by 64
	SPI_CmdWrite(REG_PPLLC2);                  //0x06
	SPI_DataWrite((SCAN_FREQ*64/OSC_FREQ)-1);  //Deve ser de 1~63, 0 é proibido
  }            
  
  // ---------- Set SDRAM clock ----------

  if(DRAM_FREQ>=125) //&&(DRAM_FREQ<=166))
  {
	SPI_CmdWrite(REG_MPLLC1);                  //0x07 
	SPI_DataWrite(0x02);                       //PLL Divided by 2
	SPI_CmdWrite(REG_MPLLC2);                  //0x08
	SPI_DataWrite((DRAM_FREQ*2/OSC_FREQ)-1);   //Deve ser de 1~63, 0 é proibido
  }
  if((DRAM_FREQ>=63)&&(DRAM_FREQ<=124))  //&&(DRAM_FREQ<=166)
  {
	SPI_CmdWrite(REG_MPLLC1);                  //0x07     
	SPI_DataWrite(0x04);                       //PLL Divided by 4
	SPI_CmdWrite(REG_MPLLC2);                  //0x08
	SPI_DataWrite((DRAM_FREQ*4/OSC_FREQ)-1);   //Deve ser de 1~63, 0 é proibido
  }
  if((DRAM_FREQ>=31)&&(DRAM_FREQ<=62))
  {           
	SPI_CmdWrite(REG_MPLLC1);                  //0x07     
	SPI_DataWrite(0x06);                       //PLL Divided by 8
	SPI_CmdWrite(REG_MPLLC2);                  //0x08
	SPI_DataWrite((DRAM_FREQ*8/OSC_FREQ)-1);   //Deve ser de 1~63, 0 é proibido
  }
  if(DRAM_FREQ<=30)
  {
	SPI_CmdWrite(REG_MPLLC1);                  //0x07   
	SPI_DataWrite(0x06);                       //PLL Divided by 8
	SPI_CmdWrite(REG_MPLLC2);                  //0x08
	SPI_DataWrite((30*8/OSC_FREQ)-1);          //Deve ser de 1~63, 0 é proibido
  }
 
  // ---------- Set Core clock ----------
  
  if(CORE_FREQ>=125)
  {
	SPI_CmdWrite(REG_SPLLC1);                  //0x09
	SPI_DataWrite(0x02);                       //PLL Divided by 2
	SPI_CmdWrite(REG_SPLLC2);                  //0x0A
	SPI_DataWrite((CORE_FREQ*2/OSC_FREQ)-1);   //Deve ser de 1~63, 0 é proibido
  }
  if((CORE_FREQ>=63)&&(CORE_FREQ<=124))     
  {
	SPI_CmdWrite(REG_SPLLC1);                  //0x09   
	SPI_DataWrite(0x04);                       //PLL Divided by 4
	SPI_CmdWrite(REG_SPLLC2);                  //0x0A
	SPI_DataWrite((CORE_FREQ*4/OSC_FREQ)-1);   //Deve ser de 1~63, 0 é proibido
  }
  if((CORE_FREQ>=31)&&(CORE_FREQ<=62))
  {           
	SPI_CmdWrite(REG_SPLLC1);                  //0x09  
	SPI_DataWrite(0x06);                       //PLL Divided by 8
	SPI_CmdWrite(REG_SPLLC2);                  //0x0A
	SPI_DataWrite((CORE_FREQ*8/OSC_FREQ)-1);   //Deve ser de 1~63, 0 é proibido
  }
  if(CORE_FREQ<=30)
  {
	SPI_CmdWrite(REG_SPLLC1);                  //0x09   
	SPI_DataWrite(0x06);                       //PLL Divided by 8
	SPI_CmdWrite(REG_SPLLC2);                  //0x0A
	SPI_DataWrite((30*8/OSC_FREQ)-1);          //Deve ser de 1~63, 0 é proibido
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
  delay(1);                                    //Aguarda para estabilizar

}


//================================================================================
// Funcoes SDRAM
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
 * - Bit 2 do STSR:
 *   0: SDRAM não está pronta para acesso.
 *   1: SDRAM pronta para acesso.
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
  uint16_t sdram_itv;
  
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
// [0x01] Chip Configuration Register (CCR)
//================================================================================


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
  SPI_CmdWrite(0x01);                          //0x01, Chip Configuration Register (CCR) 
  temp = SI_LCD_DataRead();
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
  SPI_CmdWrite(0x01);                          //0x01, Chip Configuration Register (CCR) 
  temp = SI_LCD_DataRead();
  temp |= cSetb0;                              //Set bit 0
  SPI_DataWrite(temp);
}


//================================================================================
// [0x02] Memory Access Control Register (MACR)
//================================================================================


/**
 * @brief Host Read/Write Image Data Format
 *        MPU/MCU read/write data format when access memory data port.      
 *        
 *        Data format setting: MCU interface, color depth
 *
 *        bit [7-6] Direct write
 *        0b0x: Direct write for all 8 bits MPU I/F, 16 bits MPU I/F with 
 *              16bpp, 16 bits MPU I/F with 24bpp data mode 1 and serial host 
 *              interface.
 *        0b11: Mask high byte of even data (ex. 16 bit MPU I/F with 24-bpp data mode 2)
 *
 * @param 0: Aplicado a todas as MCU e color depth
 *        1: Apenas para MCU de 16bit com Color Depth de 24bpp no Modo 2
 *
 * @note type=0 - Uso em MCU de 8/16-bit:
 *         - MCU 8-bit color depth 8-bpp
 *         - MCU 8-bit color depth 16-bpp
 *         - MCU 8-bit color depth 24-bpp
 *         - MCU 16-bit color depth 16-bpp
 *         - MCU 16-bit color depth 24-bpp Mode 1
 *      type=1 - Uso em MCU de 16-bit:
 *         - MCU 16-bit color depth 24-bpp Mode 2
 */
void Panel_RA8889::HostColorDepthFormat(uint_t type)
{
  uint8_t temp;
  SPI_CmdWrite(REG_MACR);                      //0x02, Memory Access Control Register (MACR)
  temp = SPI_DataRead();
  if (type == 0) temp &= cClrb7;               //Reset bit 7
  if (type == 1) {
	  temp |= cSetb7;                          //Set bit 7
	  temp |= cSetb6;                          //Set bit 7
  }
  SPI_DataWrite(temp);                         //Mask high byte of each data (ex. 16 bit MPU I/F with 8-bpp data mode 1)
}


/**
 * @brief Host Read/Write Memory Direction (Only for Graphic Mode)
 *        Video memory write direction setting
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
 * @note Esta função só tem efeito no modo gráfico. O modo grafico pode ser 
 *       ativado após o uso desta função.
 */
void Panel_RA8889::HostReadMemoryDirection(MemoryDirection direction)
{
  uint8_t temp;
  SPI_CmdWrite(REG_MACR);                      //0x02, Memory Access Control Register (MACR)
  temp = SPI_DataRead();
  temp &= cClrb5;                              //Reset bit 5
  temp &= cClrb4;                              //Reset bit 4
  temp |= static_cast<uint8_t>(direction);     //Converte enum para uint8_t
  SPI_DataWrite(temp);                         //Host Read Memory Direction
}


//================================================================================
// [0x03] Input Control Register (ICR)
//================================================================================


/**
 * @brief Muda o display para modo grafico
 *
 * @param None
 *
 * @note None
 *
 */
//
void Panel_RA8889::GraphicMode(void)
{
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
bool Panel_RA8889::GraphicMode(void){
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
 * Configura os bits [1:0] do registrador ICR (0x03) de acordo com
 * o destino escolhido.
 *
 * Exemplo: MemoryPort_Select(MemoryPortDest::SDRAM);
 *          Este exemplo é o memo que suar a funcao Memory_Select_SDRAM();
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
  temp |= static_cast<uint8_t>(dest);          //Define o destino
  SPI_DataWrite(temp);                         //Atualiza registrador
}


//================================================================================
// [0x10] Main/PIP Window Control Register (MPWCTR)
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
 *        bit [3-2] Main Window Image Color Depth Setting
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
  LCD_DataWrite(temp);                         //Only XDE enable, XVSYNC & XHSYNC in idle state
}


//================================================================================
// [0x12] Display Configuration Register (DPCR)
//================================================================================


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
void Panel_RA8889::PDATA_ColorFmt(PDATAColorFmt: fmt)
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


/**
 * @brief Panel Scan Clock PCLK Rising
 *
 *        PCLK Inversion:
 *        bit [7] 0b0 : 0: PDAT, DE, HSYNC etc. Panel (Drive/change) fetches PDAT at PCLK rising edge.
 *                0b1 : 1: PDAT, DE, HSYNC etc. Panel (Drive/change) fetches PDAT at PCLK falling edge.
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


//================================================================================
// [0x13] Panel scan Clock and Data Setting Register (PCSR)
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
// [0x14] Horizontal Display Width Register (HDWR)
// [0x15] Horizontal Display Width Fine Tune Register (HDWFTR)
// [0x1a] Vertical Display Height Register 0(VDHR0)
// [0x1b] Vertical Display Height Register 1 (VDHR1)
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
// [0x16] Horizontal Non-Display Period Register (HNDR)
// [0x17] Horizontal Non-Display Period Fine Tune Register (HNDFTR)
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
// [0x18] HSYNC Start Position Register (HSTR)
// [0x19] HSYNC Pulse Width Register (HPWR)
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
// [0x1c] Vertical Non-Display Period Register 0(VNDR0)
// [0x1d] Vertical Non-Display Period Register 1(VNDR1)
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
// [0x1e] VSYNC Start Position Register (VSTR)
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
// [0x1f] VSYNC Pulse Width Register (VPWR)
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
// [0x5e] Color Depth of Canvas & Active Window (AW_COLOR)
//================================================================================


/**
 * @brief Block Mode X-Y Coordinates Addressing
 *        
 *        [5Eh] Color Depth of Canvas & Active Window (AW_COLOR)
 8        bit [2] Canvas addressing mode
 *                0b0: Block mode (X-Y coordinates addressing)
 *                0b1: Linear mode
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::Memory_XY_Mode(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_AW_COLOR);                //0x5e, Color Depth of Canvas & Active Window (AW_COLOR)
  temp = SPI_DataRead();
  temp &= cClrb2;                            //Reset bit 2
  SPI_DataWrite(temp);
}


/**
 * @brief Linear Mode Addressing
 *        
 *        [5Eh] Color Depth of Canvas & Active Window (AW_COLOR)
 *        bit [2] Canvas addressing mode
 *                0b0: Block mode (X-Y coordinates addressing)
 *                0b1: Linear mode
 *
 * @param None
 *
 * @note None
 */
void Panel_RA8889::Memory_Linear_Mode(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_AW_COLOR);                //0x5e, Color Depth of Canvas & Active Window (AW_COLOR)
  temp = SPI_DataRead();
  temp |= cSetb2;                            //Set bit 2
  SPI_DataWrite(temp);
}


/**
 * @brief Profundidade de cor da imagem da tela e largura dos dados de leitura 
 *        e gravação da memória no modo de bloco 8bpp
 *        
 *        [5Eh] Color Depth of Canvas & Active Window (AW_COLOR)
 *        bit [1-0] Canvas image’s color depth & memory R/W data width
 *                  In Block Mode:
 *                  00: 8bpp
 *                  01: 16bpp
 *                  1x: 24bpp
 *                  In Linear Mode:
 *                  x0: 8-bits memory data read/write.
 *                  x1: 16-bits memory data read/write
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
 *        [5Eh] Color Depth of Canvas & Active Window (AW_COLOR)
 *        bit [1-0] Canvas image’s color depth & memory R/W data width
 *                  In Block Mode:
 *                  00: 8bpp
 *                  01: 16bpp
 *                  1x: 24bpp
 *                  In Linear Mode:
 *                  x0: 8-bits memory data read/write.
 *                  x1: 16-bits memory data read/write
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
 *        [5Eh] Color Depth of Canvas & Active Window (AW_COLOR)
 *        bit [1-0] Canvas image’s color depth & memory R/W data width
 *                  In Block Mode:
 *                  00: 8bpp
 *                  01: 16bpp
 *                  1x: 24bpp
 *                  In Linear Mode:
 *                  x0: 8-bits memory data read/write.
 *                  x1: 16-bits memory data read/write
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
// [0x20] Main Image Start Address 0 (MISA0)
// [0x21] Main Image Start Address 1 (MISA1)
// [0x22] Main Image Start Address 2 (MISA2)
// [0x23] Main Image Start Address 3 (MISA3)
//================================================================================


/**
 * @brief Page Layer Start Address
 *
 * @param layer: camada 0..8 (layer 0, layer 1, layer 2....)
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
 * @return Page layer start address
 */
unsigned long Panel_RA8889::LayerStartAddr(uint8_t layer)
{
  if (layer > MAX_LAYER) return 0;
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
void Panel_RA8889::MainImage_StartAddress(unsigned long addr)
{
  RegisterWrite(REG_MISA0, addr);              //0x20, Main Image Start Address 0 (MISA0)
  RegisterWrite(REG_MISA1, addr >> 8);         //0x21, Main Image Start Address 1 (MISA1)
  RegisterWrite(REG_MISA2, addr >> 16);        //0x22, Main Image Start Address 2 (MISA2)
  RegisterWrite(REG_MISA3, addr >> 24);	       //0x23, Main Image Start Address 3 (MISA3)
}


//================================================================================
// [0x24] Main Image Width 0 (MIW0)
// [0x25] Main Image Width 1 (MIW1)
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
void Panel_RA8889::MainImage_Width(uint16_t wx)
{
  RegisterWrite(REG_MIW0, wx);                 //0x24, Main Image Width 0 (MIW0) 
  RegisterWrite(REG_MIW1, wx >> 8);            //0x25, Main Image Width 1 (MIW1)
}


//================================================================================
// [0x26] Main Window Upper-Left corner X-coordinates 0 (MWULX0)
// [0x27] Main Window Upper-Left corner X-coordinates 1 (MWULX1)
// [0x28] Main Window Upper-Left corner Y-coordinates 0 (MWULY0)
// [0x29] Main Window Upper-Left corner Y-coordinates 1 (MWULY1)
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
// [0x50] Canvas Start address 0 (CVSSA0)
// [0x51] Canvas Start address 1 (CVSSA1)
// [0x52] Canvas Start address 2 (CVSSA2)
// [0x53] Canvas Start address 3 (CVSSA3)
//================================================================================


/**
 * @brief 
 *
 *        
 *        REG [50h] Canvas Start address 0 (CVSSA0)
 *                  Start address of Canvas [7:0]
 *        REG [51h] Canvas Start address 1 (CVSSA1)
 *                  Start address of Canvas [15:8]
 *        REG [52h] Canvas Start address 2 (CVSSA2)
 *                  Start address of Canvas [23:16]
 *        REG [53h] Canvas Start address 3 (CVSSA3)
 *                  Start address of Canvas [31:24]
 *
 * @param addr: endereço
 *
 * @note None
 */
void Panel_RA8889::CanvasImage_StartAddr(unsigned long addr)
{
  RegisterWrite(REG_CVSSA0, addr);             //0x50, Canvas Start address 0 (CVSSA0)
  RegisterWrite(REG_CVSSA1, addr >> 8);        //0x51, Canvas Start address 1 (CVSSA1)
  RegisterWrite(REG_CVSSA2, addr >> 16);       //0x52, Canvas Start address 2 (CVSSA2)
  RegisterWrite(REG_CVSSA3, addr >> 24);       //0x53, Canvas Start address 3 (CVSSA3)
}


//================================================================================
// [0x54] Canvas image width 0 (CVS_IMWTH0)
// [0x55] Canvas image width 1 (CVS_IMWTH1)
//================================================================================


/**
 * @brief 
 *        
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
 *
 * @param wx: width
 *
 * @note None
 */
void Panel_RA8889::CanvasImage_Width(uint16_t wx)
{
  RegisterWrite(REG_CVS_IMWTH0, wx);           //0x54, Canvas image width 0 (CVS_IMWTH0)
  RegisterWrite(REG_CVS_IMWTH1, wx >> 8);      //0x55, Canvas image width 1 (CVS_IMWTH1)
}


//================================================================================
// [0x56] Active Window Upper-Left corner X-coordinates 0 (AWUL_X0)
// [0x57] Active Window Upper-Left corner X-coordinates 1 (AWUL_X1)
// [0x58] Active Window Upper-Left corner Y-coordinates 0 (AWUL_Y0)
// [0x59] Active Window Upper-Left corner Y-coordinates 1 (AWUL_Y1)
//================================================================================


/**
 * @brief 
 *        
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
 *
 * @param wx: width, hy: height
 *
 * @note None
 */
void Panel_RA8889::ActiveWindow_XY(uint16_t wx, uint16_t hy)  
{
  RegisterWrite(REG_AWUL_X0, wx);              //0x56, Active Window Upper-Left corner X-coordinates 0 (AWUL_X0)
  RegisterWrite(REG_AWUL_X1, wx >> 8);         //0x57, Active Window Upper-Left corner X-coordinates 1 (AWUL_X1)
  RegisterWrite(REG_AWUL_Y0, hy);              //0x58, Active Window Upper-Left corner Y-coordinates 0 (AWUL_Y0)
  RegisterWrite(REG_AWUL_Y1, hy >> 8);         //0x59, Active Window Upper-Left corner Y-coordinates 1 (AWUL_Y1)
}


//================================================================================
// [0x5A] Active Window Width 0 (AW_WTH0)
// [0x5B] Active Window Width 1 (AW_WTH1)
// [0x5C] Active Window Height 0 (AW_HT0)
// [0x5D] Active Window Height 1 (AW_HT1)
//================================================================================


/**
 * @brief 
 *        
 *        REG [0x5A] Active Window Width 0 (AW_WTH0)
 *                   Width of Active Window [7:0]
 *        REG [0x5B] Active Window Width 1 (AW_WTH1)
 *                   Width of Active Window [12:8]
 *        REG [0x5C] Active Window Height 0 (AW_HT0)
 *                   Height of Active Window [7:0]
 *        REG [0x5D] Active Window Height 1 (AW_HT1)
 *                   Height of Active Window [12:8]
 *
 * @param wx: width, hy: height
 *
 * @note None
 */
void Panel_RA8889::ActiveWindow_WidhtHeight(uint16_t wx, uint16_t hy)  
{
  RegisterWrite(REG_AW_WTH0, wx);              //0x5a, Active Window Width 0 (AW_WTH0)
  RegisterWrite(REG_AW_WTH1, wx >> 8);         //0x5b, Active Window Width 1 (AW_WTH1)
  RegisterWrite(REG_AW_HT0, hy);               //0x5c, Active Window Height 0 (AW_HT0)
  RegisterWrite(REG_AW_HT1, hy >> 8);          //0x5d, Active Window Height 1 (AW_HT1)
}


//================================================================================
// [0x5F] Graphic Read/Write position Horizontal Position Register 0 (CURH0)
// [0x60] Graphic Read/Write position Horizontal Position Register 1 (CURH1)
// [0x61] Graphic Read/Write position Vertical Position Register 0 (CURV0)
// [0x62] Graphic Read/Write position Vertical Position Register 1 (CURV1)
//================================================================================


/**
 * @brief Set Graphic Read/Write Position
 *        
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
 *
 *
 * @param (Wx, Hy): Posicao de coordenada
 *
 *        
 *        REG[5Eh] bit 3 Não existe no RA8889/8877/8876 ?????????
 *
 * @note  REG[5Eh] bit3, Select to read back Graphic Read/Write position.
 *                 When DPRAM Linear mode:Graphic Read/Write Position [31:24][23:16][15:8][7:0]
 *                 When DPRAM Active window mode:Graphic Read/Write
 *                 Horizontal Position [12:8][7:0],
 *                 Vertical Position [12:8][7:0].
 *                 Reference Canvas image coordinate. Unit: Pixel
 *
 */
void GotoPixelXY(uint16_t Wx, uint16_t Hy)
{
    LCD_CmdWrite(0x5F);
    LCD_DataWrite(WX);
    LCD_CmdWrite(0x60);
    LCD_DataWrite(WX >> 8);
    LCD_CmdWrite(0x61);
    LCD_DataWrite(HY);
    LCD_CmdWrite(0x62);
    LCD_DataWrite(HY >> 8);
}


void GotoLinearAddr(uint32_t addr)
{
    /*
    Set Graphic Read/Write position

    REG[5Eh] bit3, Select to read back Graphic Read/Write position.
    When DPRAM Linear mode:Graphic Read/Write Position [31:24][23:16][15:8][7:0]
    When DPRAM Active window mode:Graphic Read/Write
    Horizontal Position [12:8][7:0],
    Vertical Position [12:8][7:0].
    Reference Canvas image coordinate. Unit: Pixel
    */
    LCD_CmdWrite(0x5F);
    LCD_DataWrite(Addr);
    LCD_CmdWrite(0x60);
    LCD_DataWrite(Addr >> 8);
    LCD_CmdWrite(0x61);
    LCD_DataWrite(Addr >> 16);
    LCD_CmdWrite(0x62);
    LCD_DataWrite(Addr >> 24);
}


//================================================================================
// [0x63] Text Write X-coordinates Register 0 (F_CURX0)
// [0x64] Text Write X-coordinates Register 1 (F_CURX1)
// [0x65] Text Write Y-coordinates Register 0 (F_CURY0)
// [0x66] Text Write Y-coordinates Register 1 (F_CURY1)
//================================================================================

void Goto_Text_XY(unsigned short WX, unsigned short HY)
{
    /*
    Set Text Write position
    Text Write X-coordinate [12:8][7:0]
    Text Write Y-coordinate [12:8][7:0]
    Reference Canvas image coordinate.
    Unit: Pixel
    */
    LCD_CmdWrite(0x63);
    LCD_DataWrite(WX);
    LCD_CmdWrite(0x64);
    LCD_DataWrite(WX >> 8);
    LCD_CmdWrite(0x65);
    LCD_DataWrite(HY);
    LCD_CmdWrite(0x66);
    LCD_DataWrite(HY >> 8);
}





//================================================================================
// [0xD2] Foreground Color Register - Red (FGCR)
// [0xD3] Foreground Color Register - Green (FGCG)
// [0xD4] Foreground Color Register - Blue (FGCB)
//================================================================================


/**
 * @brief Cor de frente nas componentes Vermelho, Verde e Azul (R,G,B)
 *        
 *        REG [0xd2] Foreground Color Register - Red (FGCR)
 *                   bit [7~0] Foreground Color - Red; for draw, text or color expansion
 *                             256 colors, the register only uses Bit[7:5].
 *                             65K colors, the register uses Bit[7:3].
 *                             16.7M colors, the register uses Bit[7:0].
 *        REG [0xd3] Foreground Color Register - Green (FGCG)
 *                   bit [7~0] Foreground Color - Green; for draw, text or color expansion
 *                             256 colors, the register only uses Bit[7:5].
 *                             65K colors, the register uses Bit[7:2].
 *                             16.7M colors, the register uses Bit[7:0].
 *        REG [0xd4] Foreground Color Register - Blue (FGCB)
 *                   bit [7~0] Foreground Color - Blue; for draw, text or color expansion
 *                             256 colors, the register only uses Bit[7:6].
 *                             65K colors, the register uses Bit[7:3].
 *                             16.7M colors, the register uses Bit[7:0].
 *
 * @param red:   componente cor vermelha
 *        green: componente cor verde
 *        blue:  componente cor azul
 *
 * @note use o determinado numero de bits para compor o n umero de cores 256, 
 *       65K e 16.7M cores.
 */
void Panel_RA8889::ForegroundColor_RGB(uint8_t red, uint8_t green, uint8_t blue)
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
 *        Color depht de 8bpp
 *
 *        REG [0xd2] Foreground Color Register - Red (FGCR)
 *                   bit [7~0] Foreground Color - Red; for draw, text or color expansion
 *                             256 colors, the register only uses Bit[7:5].
 *                             65K colors, the register uses Bit[7:3].
 *                             16.7M colors, the register uses Bit[7:0].
 *        REG [0xd3] Foreground Color Register - Green (FGCG)
 *                   bit [7~0] Foreground Color - Green; for draw, text or color expansion
 *                             256 colors, the register only uses Bit[7:5].
 *                             65K colors, the register uses Bit[7:2].
 *                             16.7M colors, the register uses Bit[7:0].
 *        REG [0xd4] Foreground Color Register - Blue (FGCB)
 *                   bit [7~0] Foreground Color - Blue; for draw, text or color expansion
 *                             256 colors, the register only uses Bit[7:6].
 *                             65K colors, the register uses Bit[7:3].
 *                             16.7M colors, the register uses Bit[7:0].
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
void Panel_RA8889::ForegroundColor_256(uint8_t color)
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
 *        Color depht de 16bpp
 *
 *        REG [0xd2] Foreground Color Register - Red (FGCR)
 *                   bit [7~0] Foreground Color - Red; for draw, text or color expansion
 *                             256 colors, the register only uses Bit[7:5].
 *                             65K colors, the register uses Bit[7:3].
 *                             16.7M colors, the register uses Bit[7:0].
 *        REG [0xd3] Foreground Color Register - Green (FGCG)
 *                   bit [7~0] Foreground Color - Green; for draw, text or color expansion
 *                             256 colors, the register only uses Bit[7:5].
 *                             65K colors, the register uses Bit[7:2].
 *                             16.7M colors, the register uses Bit[7:0].
 *        REG [0xd4] Foreground Color Register - Blue (FGCB)
 *                   bit [7~0] Foreground Color - Blue; for draw, text or color expansion
 *                             256 colors, the register only uses Bit[7:6].
 *                             65K colors, the register uses Bit[7:3].
 *                             16.7M colors, the register uses Bit[7:0].
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
void Panel_RA8889::ForegroundColor_65k(uint16_t color)
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
 *        Color depht de 24bpp
 *
 *        REG [0xd2] Foreground Color Register - Red (FGCR)
 *                   bit [7~0] Foreground Color - Red; for draw, text or color expansion
 *                             256 colors, the register only uses Bit[7:5].
 *                             65K colors, the register uses Bit[7:3].
 *                             16.7M colors, the register uses Bit[7:0].
 *        REG [0xd3] Foreground Color Register - Green (FGCG)
 *                   bit [7~0] Foreground Color - Green; for draw, text or color expansion
 *                             256 colors, the register only uses Bit[7:5].
 *                             65K colors, the register uses Bit[7:2].
 *                             16.7M colors, the register uses Bit[7:0].
 *        REG [0xd4] Foreground Color Register - Blue (FGCB)
 *                   bit [7~0] Foreground Color - Blue; for draw, text or color expansion
 *                             256 colors, the register only uses Bit[7:6].
 *                             65K colors, the register uses Bit[7:3].
 *                             16.7M colors, the register uses Bit[7:0].
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
void Panel_RA8889::ForegroundColor_16M(uint32_t color)
{
  SPI_CmdWrite(REG_FGCR);                      //0xd2, Foreground Color Register - Red (FGCR)
  SPI_DataWrite(color >> 16);                  //Desloca os 8 bits do vermelho, usa o bit de [7~0]
  SPI_CmdWrite(REG_FGCG);                      //0xd3, Foreground Color Register - Green (FGCG)
  SPI_DataWrite(color >> 8);                   //Deslocar os 8 bits do verde, usa os bits [7~0]
  SPI_CmdWrite(REG_FGCB);                      //0xd4, Foreground Color Register - Blue (FGCB)
  SPI_DataWrite(color);                        //Deslocar os 8 bits do azul, usa os bits [7~0]
}


//================================================================================
// [0xD5] Background Color Register - Red (BGCR)
// [0xD6] Background Color Register - Green (BGCG)
// [0xD7] Background Color Register - Blue (BGCB)
//================================================================================


/**
 * @brief Cor de frente nas componentes Vermelho, Verde e Azul (R,G,B)
 *        
 *        REG [0xd5] Background Color Register - Red (BGCR)
 *                   bit [7~0] Background Color - Red, for Text or color expansion
 *                             256 colors, the register only uses Bit[7:5].
 *                             65K colors, the register uses Bit[7:3].
 *                             16.7M colors, the register uses Bit[7:0].
 *        REG [0xd6] Background Color Register - Green (BGCG)
 *                   bit [7~0] Background Color - Green, for Text or color expansion
 *                             256 colors, the register only uses Bit[7:5].
 *                             65K colors, the register uses Bit[7:2].
 *                             16.7M colors, the register uses Bit[7:0].
 *        REG [0xd7] Background Color Register - Blue (BGCB)
 *                   bit [7~0] Background Color - Blue, for Text or color expansion
 *                             256 colors, the register only uses Bit[7:6].
 *                             65K colors, the register uses Bit[7:3].
 *                             16.7M colors, the register uses Bit[7:0].
 *
 *        ***Note: No matter background transparency is enabled or not, don’t 
 *           set same value with Foreground Color otherwise image or text will 
 *           become a square with Foreground Color even BTE function.   
 *
 *        *** Note: If user wants to change rotate attribute, character line gap, 
 *            character-to-character space, foreground color, background color 
 *            and Text/graphic mode setting, please make sure core_busy (fontwr_
 *            busy) status bit is low.
 *
 * @param red:   componente cor vermelha
 *        green: componente cor verde
 *        blue:  componente cor azul
 *
 * @note Use o determinado numero de bits para compor o numero de cores 256, 
 *       65K e 16.7M cores.
 */
void Panel_RA8889::BackgroundColor_RGB(uint8_t red, uint8_t green, uint8_t blue)
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
 *        Color depht de 8bpp
 *        
 *        REG [0xd5] Background Color Register - Red (BGCR)
 *                   bit [7~0] Background Color - Red, for Text or color expansion
 *                             256 colors, the register only uses Bit[7:5].
 *                             65K colors, the register uses Bit[7:3].
 *                             16.7M colors, the register uses Bit[7:0].
 *        REG [0xd6] Background Color Register - Green (BGCG)
 *                   bit [7~0] Background Color - Green, for Text or color expansion
 *                             256 colors, the register only uses Bit[7:5].
 *                             65K colors, the register uses Bit[7:2].
 *                             16.7M colors, the register uses Bit[7:0].
 *        REG [0xd7] Background Color Register - Blue (BGCB)
 *                   bit [7~0] Background Color - Blue, for Text or color expansion
 *                             256 colors, the register only uses Bit[7:6].
 *                             65K colors, the register uses Bit[7:3].
 *                             16.7M colors, the register uses Bit[7:0].
 *
 *        ***Note: No matter background transparency is enabled or not, don’t 
 *           set same value with Foreground Color otherwise image or text will 
 *           become a square with Foreground Color even BTE function.   
 *
 *        *** Note: If user wants to change rotate attribute, character line gap, 
 *            character-to-character space, foreground color, background color 
 *            and Text/graphic mode setting, please make sure core_busy (fontwr_
 *            busy) status bit is low.
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
void Panel_RA8889::BackgroundColor_256(uint8_t color)
{
  SPI_CmdWrite(REG_BGCR);                      //0xd5, Background Color Register - Red (BGCR)
  SPI_DataWrite(color);                        //Vermelho so usa o bit de [7~5], o resto ignorado
  SPI_CmdWrite(REG_BGCG);                      //0xd6, Background Color Register - Green (BGCG)
  SPI_DataWrite(color << 3);                   //Deslocar a posicao do verde para o bit [7~5], o resto ignorado
  SPI_CmdWrite(REG_BGCB);                      //0xd7, Background Color Register - Blue (BGCB)
  SPI_DataWrite(color << 6);                   //Deslocar a posicao do azul para o bit [7~6], o resto ignorado
}


// Input data format:R5G6B6
/**
 * @brief Cor de fundo nas componentes Vermelho, Verde e Azul (RGB5:6:5) de 65k cores
 *        
 *        Color depht de 16bpp
 *
 *        REG [0xd5] Background Color Register - Red (BGCR)
 *                   bit [7~0] Background Color - Red, for Text or color expansion
 *                             256 colors, the register only uses Bit[7:5].
 *                             65K colors, the register uses Bit[7:3].
 *                             16.7M colors, the register uses Bit[7:0].
 *        REG [0xd6] Background Color Register - Green (BGCG)
 *                   bit [7~0] Background Color - Green, for Text or color expansion
 *                             256 colors, the register only uses Bit[7:5].
 *                             65K colors, the register uses Bit[7:2].
 *                             16.7M colors, the register uses Bit[7:0].
 *        REG [0xd7] Background Color Register - Blue (BGCB)
 *                   bit [7~0] Background Color - Blue, for Text or color expansion
 *                             256 colors, the register only uses Bit[7:6].
 *                             65K colors, the register uses Bit[7:3].
 *                             16.7M colors, the register uses Bit[7:0].
 *
 *        ***Note: No matter background transparency is enabled or not, don’t 
 *           set same value with Foreground Color otherwise image or text will 
 *           become a square with Foreground Color even BTE function.   
 *
 *        *** Note: If user wants to change rotate attribute, character line gap, 
 *            character-to-character space, foreground color, background color 
 *            and Text/graphic mode setting, please make sure core_busy (fontwr_
 *            busy) status bit is low.
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
void Panel_RA8889::BackgroundColor_65k(uint16_t color)
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
 *        Color depht de 24bpp
 *
 *        REG [0xd5] Background Color Register - Red (BGCR)
 *                   bit [7~0] Background Color - Red, for Text or color expansion
 *                             256 colors, the register only uses Bit[7:5].
 *                             65K colors, the register uses Bit[7:3].
 *                             16.7M colors, the register uses Bit[7:0].
 *        REG [0xd6] Background Color Register - Green (BGCG)
 *                   bit [7~0] Background Color - Green, for Text or color expansion
 *                             256 colors, the register only uses Bit[7:5].
 *                             65K colors, the register uses Bit[7:2].
 *                             16.7M colors, the register uses Bit[7:0].
 *        REG [0xd7] Background Color Register - Blue (BGCB)
 *                   bit [7~0] Background Color - Blue, for Text or color expansion
 *                             256 colors, the register only uses Bit[7:6].
 *                             65K colors, the register uses Bit[7:3].
 *                             16.7M colors, the register uses Bit[7:0].
 *
 *        ***Note: No matter background transparency is enabled or not, don’t 
 *           set same value with Foreground Color otherwise image or text will 
 *           become a square with Foreground Color even BTE function.   
 *
 *        *** Note: If user wants to change rotate attribute, character line gap, 
 *            character-to-character space, foreground color, background color 
 *            and Text/graphic mode setting, please make sure core_busy (fontwr_
 *            busy) status bit is low.
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
void Panel_RA8889::BackgroundColor_16M(uint32_t color)
{
  SPI_CmdWrite(REG_BGCR);                      //0xd5, Background Color Register - Red (BGCR)
  SPI_DataWrite(color >> 16);                  //Desloca os 8 bits do vermelho, usa o bit de [7~0]
  SPI_CmdWrite(REG_BGCG);                      //0xd6, Background Color Register - Green (BGCG)
  SPI_DataWrite(color >> 8);                   //Deslocar os 8 bits do verde, usa os bits [7~0]
  SPI_CmdWrite(REG_BGCB);                      //0xd7, Background Color Register - Blue (BGCB)
  SPI_DataWrite(color);                        //Deslocar os 8 bits do azul, usa os bits [7~0]
}


//================================================================================
// [0x67] Draw Line / Triangle Control Register 0 (DCR0)
//================================================================================


/**
 * Talvez seja uma funcao do RA8876 (verificar no manual)
 * @brief Enable/Disable Drawing
 *        
 * @param b: true habilita, false: desabilita a linha de desenhos
 *
 * @note Não está descrito no maual. indica apenas que o bit[0] precisa 
 *       ser zero.
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
 *        Draw Line Start Signal Write Function
 *
 *        REG[67h] Draw Line / Triangle Control Register 0 (DCR0)
 *                 bit [7] Draw Line / Triangle Start Signal 
 *                         Write Function:
 *                         0b0: Stop the drawing function.
 *                         0b1: Start the drawing function.
 *                         Read Function:
 *                         0b0 : Drawing function complete.
 *                         0b1 : Drawing function is processing.
 *                 bit [1] Draw Triangle or Line Select Signal
 *                         0b0: Draw Line
 *                         0b1: Draw Triangle
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
 *        Draw Triangle Start Signal Write Function Non-Fill
 *
 *        REG[67h] Draw Line / Triangle Control Register 0 (DCR0)
 *                 bit [7] Draw Line / Triangle Start Signal 
 *                         Write Function:
 *                         0b0: Stop the drawing function.
 *                         0b1: Start the drawing function.
 *                         Read Function:
 *                         0b0 : Drawing function complete.
 *                         0b1 : Drawing function is processing.
 *                 bit [5] Fill function for Triangle Signal
 *                         0b0: Non fill.
 *                         0b1: Fill 
 *                 bit [1] Draw Triangle or Line Select Signal
 *                         0b0: Draw Line
 *                         0b1: Draw Triangle
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
// [0x76] Draw Circle/Ellipse/Ellipse Curve/Circle Square Control Register 1 (DCR1)
//================================================================================


/**
 * @brief Ativa o Modo de desenho de Circulo / Elipse
 *        
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
void Panel_RA8889::EllipseMode_Start(bool fill) { CircleMode_Start(fill);}


/**
 * @brief Ativa o Modo de desenho de curva circular / eliptica 
 *        Quadrante Esquerda e Abaixo
 *
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
  if (fill) temp |= cSetrb6 else temp &= cClrb6 //Set bit 6 = Fill, Reset bit 6 = Non-Fill
  temp |= cSetb5;                              //Set bit 5, Draw Circle Square
  temp |= cSetb4;                              //Set bit 4, Draw Circle Square
  SPI_DataWrite(temp);                         //0b1n11 xxxx   n=1/0
  CoreTask_WaitReady();                        //Espere ate ficar pronto
}


//================================================================================
// [0x77] Draw Circle/Ellipse/Circle Square Major radius Setting Register (ELL_A0)
// [0x78] Draw Circle/Ellipse/Circle Square Major radius Setting Register (ELL_A1)
// [0x79] Draw Circle/Ellipse/Circle Square Minor radius Setting Register (ELL_B0)
// [0x7a] Draw Circle/Ellipse/Circle Square Minor radius Setting Register (ELL_B1)
// [0x7b] Draw Circle/Ellipse/Circle Square Center X-coordinates Register0 (DEHR0)
// [0x7c] Draw Circle/Ellipse/Circle Square Center X-coordinates Register1 (DEHR1)
// [0x7d] Draw Circle/Ellipse/Circle Square Center Y-coordinates Register0 (DEVR0)
// [0x7e] Draw Circle/Ellipse/Circle Square Center Y-coordinates Register1 (DEVR1)
//================================================================================


/**
 * @brief Raio do círculo                           Rx = Ry
 *        Raio da elipse                            Rx > Ry or Rx < Ry
 *        Raio da curva circular no canto quadrado  Rx = Ry, Rx > Ry or Rx < Ry
 *
 *        REG[0x77] Draw Circle/Ellipse/Circle Square Major radius Setting Register (ELL_A0)
 *                  bit [7~0] Draw Circle/Ellipse/Circle Square Major radius [7:0]
 *        REG[0x78] Draw Circle/Ellipse/Circle Square Major radius Setting Register (ELL_A1)
 *                  bit [4~0] Draw Circle/Ellipse/Circle Square Major radius [12:8]
 *
 *        Unit: Pixel
 *        To draw a circle needs to set major axis equal to minor radius.
 * 
 *        REG[0x79] Draw Circle/Ellipse/Circle Square Minor radius Setting Register (ELL_B0)
 *                  bit [7~0] Draw Circle/Ellipse/Circle Square Minor radius [7:0]
 *        REG[0x7a] Draw Circle/Ellipse/Circle Square Minor radius Setting Register (ELL_B1)
 *                  bit [4~0] Draw Circle/Ellipse/Circle Square Minor radius [12:8]
 *
 *        Unit: Pixel
 *        To draw a circle needs to set major axis equal to minor radius.
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
void Panel_RA8889::CircleRadius_R(uint16_t R) {Radius_RxRy(R, R);}
void Panel_RA8889::EllipseRadius_RxRy(uint16_t Rx, uint16_t Ry) {Radius_RxRy(Rx, Ry);}
void Panel_RA8889::CircleSquareRadius_RxRy(uint16_t Rx, uint16_t Ry) {Radius_RxRy(Rx, Ry);}


/**
 * @brief Posição do Centro do Círculo/Elipse/Circulo do quadrado
 *
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
//================================================================================


/**
 * @brief Line Start Point
 *        
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
void Panel_RA8889::Line_Point1XY(uint16_t wx, uint16_t hy) {Point1_XY(wx, hy);}


/**
 * @brief Line End Point
 *        
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
void Panel_RA8889::Line_Point2XY(uint16_t wx, uint16_t hy) {Point2_XY(wx, hy);}


/**
 * @brief Triangle Point 1
 *        
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
 *
 * @param wx: ponto de coordenada x
 *        hy: ponto de coordenada y
 *
 * @note None
 */ 
void Panel_RA8889::Triangle_Point1XY(uint16_t wx, uint16_t hy) {Point1_XY(wx, hy);}

/**
 * @brief Triangle Point 2
 *        
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
 *
 * @param wx: ponto de coordenada x
 *        hy: ponto de coordenada y
 *
 * @note None
 */ 
void Panel_RA8889::Triangle_Point2XY(uint16_t wx, uint16_t hy)  {Point2_XY(wx, hy);}


/**
 * @brief Triangle Point 3
 *        
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
void Panel_RA8889::Triangle_Point3XY(uint16_t wx, uint16_t hy) {Point3_XY(wx, hy)}


/**
 * @brief Square Start Point
 *        
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
 *
 * @param wx: ponto de coordenada x
 *        hy: ponto de coordenada y
 *
 * @note None
 */ 
void Panel_RA8889::Square_Point1XY(uint16_t wx, uint16_t hy) {Point1_XY(wx, hy);}


/**
 * @brief Square End Point
 *        
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
 *
 * @param wx: ponto de coordenada x
 *        hy: ponto de coordenada y
 *
 * @note None
 */
void Panel_RA8889::Square_Point2XY(uint16_t wx, uint16_t hy) {Point2_XY(wx, hy);}







//================================================================================
// Funções de Desenho
// 
// 
//================================================================================


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
  ForegroundColor_65k(color);
  Point1_XY(x1, y1);
  Point2_XY(x2, y2);
  LineMode_Start();
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
  ForegroundColor_65k(forecolor);
  Point1_XY(x1, y1);
  Point2_XY(x2, y2);
  SquareMode_Start(bfill);
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
  ForegroundColor_65k(forecolor);
  Point1_XY(x1, y1);
  Point2_XY(x2, y2);
  Point3_XY(x3, y3);
  TriangleMode_Start(bfill);
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
  ForegroundColor_65k(forecolor);
  Center_XY(x1,y1);
  Radius_R(R);
  CircleMode_Start(bfill);
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
  ForegroundColor_65k(forecolor);
  Center_XY(x1, y1);
  Radius_RxRy(Rx, Ry);
  EllipseMode_Start(bfill);
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
  ForegroundColor_65k(forecolor);
  Center_XY(x1, y1);
  Radius_RxRy(Rx, Ry);
  CurveLeftUpMode_Start(bfill);
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
  ForegroundColor_65k(forecolor);
  Center_XY(x1, y1);
  Radius_RxRy(Rx, Ry);
  CurveRightDownMode_Start(bfill);
}


/**
 * @brief Desenha uma curva com curvatura direita/superior
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
void Panel_RA8889::DrawCurveRightUp(uint16_t x1,
                                    uint16_t y1,
                                    uint16_t Rx,
                                    uint16_t Ry,
                                    uint32_t forecolor,
                                    bool bfill = false
                                   )
{
  ForegroundColor_65k(forecolor);
  Center_XY(x1, y1);
  Radius_RxRy(Rx, Ry);
  CurveRightUpMode_Start(bfill);
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
  ForegroundColor_65k(forecolor);
  Center_XY(x1, y1);
  Radius_RxRy(Rx, Ry);
  CurveLeftDownMode_Start(bfill); 
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
  ForegroundColor_65k(forecolor);
  Point1_XY(x1, y1);
  Point2_XY(x2, y2);
  Radius_RxRy(Rx, Ry);
  CircleSquareMode_Start();
}







void Show_String(char *str)
{   
    Text_Mode();     
    ER_TFT.LCD_CmdWrite(0x04);
    while(*str != '\0')
    {
      ER_TFT.LCD_DataWrite(*str);
      Check_Mem_WR_FIFO_not_Full();
      ++str;   
    }
    Check_2D_Busy();

    Graphic_Mode(); //back to graphic mode;
}

void DrawPixel(unsigned short x,unsigned short y,unsigned short color)
{  
 //   ER_TFT.Goto_Pixel_XY(x,y);
    ER_TFT.LCD_CmdWrite(0x04); 
    ER_TFT.LCD_DataWrite(color);
    Check_Mem_WR_FIFO_not_Full();
    ER_TFT.LCD_DataWrite(color>>8);
    Check_Mem_WR_FIFO_not_Full();  
}  


void Show_picture(unsigned long numbers,const unsigned char *datap)
{   
  unsigned long i;

  ER_TFT.LCD_CmdWrite(0x04);  
  for(i=0;i<numbers*2;i+=2)
  {
    ER_TFT.LCD_DataWrite(pgm_read_byte(&datap[i+1]));
    Check_Mem_WR_FIFO_not_Full();
    ER_TFT.LCD_DataWrite(pgm_read_byte(&datap[i]));
    Check_Mem_WR_FIFO_not_Full();
  }




// Note. this API does not support the case that MCU=16bit, 24bpp and mode1
void putPixel(
    unsigned short x // x of coordinate
    ,
    unsigned short y // y of coordinate
    ,
    unsigned long color
    /*color : 8bpp:R3G3B2
    16bpp:R5G6B5
    24bpp:R8G8B8 */
)

{

    Goto_Pixel_XY(x, y);
    LCD_CmdWrite(0x04);
    Check_Mem_WR_FIFO_not_Full();

#ifdef MCU_8bit_ColorDepth_8bpp
    LCD_DataWrite(color);
#endif
#ifdef MCU_8bit_ColorDepth_16bpp
    LCD_DataWrite(color);
    Check_Mem_WR_FIFO_not_Full();
    LCD_DataWrite(color >> 8);
#endif
#ifdef MCU_8bit_ColorDepth_24bpp
    LCD_DataWrite(color);
    Check_Mem_WR_FIFO_not_Full();
    LCD_DataWrite(color >> 8);
    Check_Mem_WR_FIFO_not_Full();
    LCD_DataWrite(color >> 16);
#endif
#ifdef MCU_16bit_ColorDepth_16bpp
    LCD_DataWrite(color);
#endif
#ifdef MCU_16bit_ColorDepth_24bpp_Mode_2
    LCD_DataWrite(color);
    Check_Mem_WR_FIFO_not_Full();
    LCD_DataWrite(color >> 16);
#endif
}






















namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  const uint8_t* Panel_RA8875::getInitCommands(uint8_t listno) const
  {
    static constexpr uint8_t list0[] PROGMEM =
    {
      0x88      , 1+CMD_INIT_DELAY, 0x0B, 1, // PLL ini
      0x89      , 1+CMD_INIT_DELAY, 0x01, 1,
      0x04      , 1+CMD_INIT_DELAY, 0x82, 1,    //PCLK
    // 0x14      , 1, 0x63, //HDWR//Horizontal Display Width Setting Bit[6:0]  //Horizontal display width(pixels) = (HDWR + 1)*8       0x27
      0x15      , 1, 0x02, //HNDFCR//Horizontal Non-Display Period fine tune Bit[3:0]  //(HNDR + 1)*8 +HNDFCR
      0x16      , 1, 0x03, //HNDR//Horizontal Non-Display Period Bit[4:0] //Horizontal Non-Display Period (pixels) = (HNDR + 1)*8
      0x17      , 1, 0x01, //HSTR//HSYNC Start Position[4:0]  //HSYNC Start Position(PCLK) = (HSTR + 1)*8
      0x18      , 1, 0x03, //HPWR//HSYNC Polarity ,The period width of HSYNC.  //HSYNC Width [4:0]   HSYNC Pulse width(PCLK) = (HPWR + 1)*8

//Vertical set
    // 0x19      , 1, 0xDF, //VDHR0 //Vertical Display Height Bit [7:0] //Vertical pixels = VDHR + 1	0xef
    // 0x1a      , 1, 0x01, //VDHR1 //Vertical Display Height Bit [8]  //Vertical pixels = VDHR + 1 	0x00
      0x1b      , 1, 0x0F, //VNDR0 //Vertical Non-Display Period Bit [7:0]  //Vertical Non-Display area = (VNDR + 1)
      0x1c      , 1, 0x00, //VNDR1 //Vertical Non-Display Period Bit [8] //Vertical Non-Display area = (VNDR + 1)
      0x1d      , 1, 0x0e, //VSTR0 //VSYNC Start Position[7:0]  //VSYNC Start Position(PCLK) = (VSTR + 1)
      0x1e      , 1, 0x06, //VSTR1 //VSYNC Start Position[8]  //VSYNC Start Position(PCLK) = (VSTR + 1)
      0x1f      , 1, 0x01, //VPWR //VSYNC Polarity ,VSYNC Pulse Width[6:0]  //VSYNC Pulse Width(PCLK) = (VPWR + 1)

      0xc7      , 1, 0x01,  // set Extra General Purpose IO Register to Output (used to enable Display by Adafruit)
    
      0x8a      , 1, 0x80, //PWM setting
      0x8a      , 1, 0x81, //PWM setting //open PWM
      0x8b      , 1, 0x7F, //Backlight brightness setting //Brightness parameter 0xff-0x00

      0x01      , 1, 0x80, //display on

      0xFF,0xFF, // end
    };
    switch (listno)
    {
    case 0: return list0;
    default: return nullptr;
    }
  }

  bool Panel_RA8875::init(bool use_reset)
  {
    _flg_serialbus = _bus->busType() == bus_spi;

    if (!Panel_Device::init(use_reset))
    {
      return false;
    }

    if (_cfg.pin_busy >= 0)
    {
      pinMode(_cfg.pin_busy, pin_mode_t::input);
    }

    uint32_t freq_write = _bus->getClock();
    if (freq_write > 5000000)
    {
      /// O processo de inicialização é realizado em uma velocidade de clock menor.;
      /// O RA8875 tem um clock operacional baixo imediatamente após a inicialização e não processará se o clock de transmissão for muito rápido.;
      /// Durante a inicialização, um comando para aumentar o relógio operacional é executado,;
      /// Após a inicialização, o relógio é restaurado para a configuração original definida pelo usuário.;
      _bus->setClock(5000000);
    }

    startWrite(true);

    for (uint8_t i = 0; auto cmds = getInitCommands(i); i++)
    {
      command_list(cmds);
    }

    {
      //HDWR  // Horizontal Display Width Setting
      _write_reg( 0x14, (_cfg.offset_x + _cfg.panel_width +7) >> 3);

      uint_fast16_t height = _cfg.offset_y + _cfg.panel_height - 1;
      //VDHR0  // Vertical Display Height Setting
      _write_reg( 0x19, height);
      _write_reg( 0x1a, height >> 8);
    }

    endWrite();

    /// 初期化後にクロックをユーザー設定値に戻す;
    _bus->setClock(freq_write);

    _latestcolor = 0;

    uint16_t data16[6] = { (uint16_t)_cfg.offset_x
                         , (uint16_t)_cfg.offset_y
                         , (uint16_t)(_cfg.offset_x + _cfg.panel_width)
                         , (uint16_t)(_cfg.offset_y + _cfg.panel_height)
                         };
    auto data = (uint8_t*)data16;
    for (size_t idx = 0; idx < 8; ++idx)
    {
      _write_reg(0x30 + idx, data[idx]);
    }

    for (size_t reg = 0x51; reg <= 0x65; ++reg)
    {
      _write_reg(reg, 0);
    }

    return true;
  }

  void Panel_RA8875::beginTransaction(void)
  {
    begin_transaction();
  }
  void Panel_RA8875::begin_transaction(void)
  {
    if (_in_transaction) return;
    _in_transaction = true;
    _flg_memorywrite = false;
    _bus->beginTransaction();

    if (!_flg_serialbus) { cs_control(false); }
  }

  void Panel_RA8875::endTransaction(void)
  {
    end_transaction();
  }
  void Panel_RA8875::end_transaction(void)
  {
    if (!_in_transaction) return;
    _in_transaction = false;

    _bus->endTransaction();
    cs_control(true);
  }

  color_depth_t Panel_RA8875::setColorDepth(color_depth_t depth)
  {
    depth = ((int)depth & color_depth_t::bit_mask) >= 16 ? rgb565_2Byte : rgb332_1Byte;
    _write_depth = depth;
    _read_depth = depth;

    update_madctl();

    return depth;
  }
  void Panel_RA8875::setRotation(uint_fast8_t r)
  {
    r &= 7;
    _rotation = r;
    _internal_rotation = ((r + _cfg.offset_rotation) & 3) | ((r & 4) ^ (_cfg.offset_rotation & 4));

    _width  = _cfg.panel_width;
    _height = _cfg.panel_height;
    if (_internal_rotation & 1) { std::swap(_width, _height); }

    _colstart = _cfg.offset_x;
    _rowstart = _cfg.offset_y;

    _xs = _xe = _ys = _ye = INT16_MAX;

    update_madctl();
  }

  void Panel_RA8875::update_madctl(void)
  {
    //SYSR   bit[4:3]=00 256 color  bit[2:1]=  00 8bit MPU interface    1x 64k color  1x 16bit
    _write_reg(0x10, _write_depth == rgb565_2Byte ? 0x0C : 0x00);

    uint_fast8_t data = 0;
    switch (_internal_rotation & 3)
    {
    default:
    case 0: case 4:              break; // left to right 0x00
    case 1: case 5: data = 0x02; break; // top to bottom 0x08
    case 2: case 6: data = 0x01; break; // right to left 0x04
    case 3: case 7: data = 0x03; break; // bottom to top 0x0C
    }
    _write_reg(0x40, data << 2); // write direction
    _write_reg(0x45, data); // read direction

    _latestcolor = ~0u;

    return;
  }

  void Panel_RA8875::waitDisplay(void)
  {
    _wait_busy();
  }

  bool Panel_RA8875::displayBusy(void)
  {
    if (_bus->busy()) return true;
    if (_cfg.pin_busy >= 0 && !lgfx::gpio_in(_cfg.pin_busy)) return true;
    return false;
  }

  bool Panel_RA8875::_wait_busy(uint32_t timeout)
  {
    _bus->wait();
    cs_control(true);
    int_fast16_t pin = _cfg.pin_busy;
    if (pin >= 0 && !lgfx::gpio_in(pin))
    {
      auto time = millis();
      do
      {
        if (millis() - time > timeout)
        {
          return false;
        }
      } while (!lgfx::gpio_in(pin));
    }
    cs_control(false);
    return true;
  }

  void Panel_RA8875::_write_reg(uint_fast16_t reg, uint_fast16_t data)
  {
    _flg_memorywrite = false;
    if (_flg_serialbus)
    {
      uint32_t value = (data << 24) + (reg << 8) + 0x80;
      _wait_busy();
      _bus->writeCommand(value, 32);
    }
    else
    {
      _bus->flush();
      uint_fast8_t len = 8;
      if (_cfg.dlen_16bit)
      {
        len <<= 1;
        reg <<= 8;
        data <<= 8;
      }
      _wait_busy();
      _bus->writeCommand(reg, len);
      _bus->writeData(data, len);
    }
  }

  void Panel_RA8875::_write_reg_0x51(uint8_t reg, uint8_t data)
  {
    size_t index = reg - 0x51;
    if (index < sizeof(_reg_0x51))
    {
      if (_reg_0x51[index] == data) { return; }
      _reg_0x51[index] = data;
    }
    _write_reg(reg, data);
  }

  void Panel_RA8875::_start_memorywrite(void)
  {
    if (_flg_memorywrite) { return; }
    _flg_memorywrite = true;
    if (_flg_serialbus)
    {
      _wait_busy();
      _bus->writeCommand((0x02 << 8) + 0x80, 24);
    }
    else
    {
      _bus->flush();
      uint32_t reg = 0x02;
      uint_fast8_t len = 8;
      if (_cfg.dlen_16bit)
      {
        len <<= 1;
        reg <<= 8;
      }
      _wait_busy();
      _bus->writeCommand(reg, len);
    }
  }

  void Panel_RA8875::writeCommand(uint32_t cmd, uint_fast8_t length)
  {
    if (_flg_serialbus)
    {
      cmd = (cmd << 8) + 0x80;
      _wait_busy();
      _bus->writeCommand(cmd, 16);
    }
    else
    {
      Panel_Device::writeCommand(cmd, length);
    }
  }
  void Panel_RA8875::writeData(uint32_t data, uint_fast8_t length)
  {
    if (_flg_serialbus)
    {
      data <<= 8;
      _wait_busy();
      _bus->writeData(data, 16);
    }
    else
    {
      Panel_Device::writeData(data, length);
    }
  }

  void Panel_RA8875::setWindow(uint_fast16_t xs, uint_fast16_t ys, uint_fast16_t xe, uint_fast16_t ye)
  {
    _xs = xs;
    _xe = xe;
    _ys = ys;
    _ye = ye;
    _xpos = _ypos = INT16_MAX;
  }

  void Panel_RA8875::_set_write_pos(uint_fast16_t x, uint_fast16_t y)
  {
    auto flg_x = (_xpos != x);
    _xpos = x;
    auto flg_y = (_ypos != y);
    _ypos = y;
    if (flg_x || flg_y)
    {
      uint_fast8_t r = _internal_rotation;
      if (r)
      {
        if ((1u << r) & 0b10010110) { y = _height - (y + 1); }
        if (r & 2)                  { x = _width  - (x + 1); }
        if (r & 1) { std::swap(x, y); std::swap(flg_x, flg_y); }
      }
      if (flg_x)
      {
        x += _colstart;
        _write_reg(0x46, x);
        _write_reg(0x47, x >> 8);
      }
      if (flg_y)
      {
        y += _rowstart;
        _write_reg(0x48, y);
        _write_reg(0x49, y >> 8);
      }
    }
  }

  void Panel_RA8875::drawPixelPreclipped(uint_fast16_t x, uint_fast16_t y, uint32_t rawcolor)
  {
    bool tr = _in_transaction;
    if (!tr) begin_transaction();

    writeFillRectPreclipped(x, y, 1, 1, rawcolor);

    if (!tr) end_transaction();
  }

  void Panel_RA8875::writeFillRectPreclipped(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, uint32_t rawcolor)
  {
    _xs = x;
    _xe = x + w - 1;
    _ys = y;
    _ye = y + h - 1;
    if (h == 1 && w <= 8)
    {
      _set_write_pos(x, y);
      writeBlock(rawcolor, w);
    }
    else
    {
      if (_latestcolor != rawcolor)
      {
        _latestcolor = rawcolor;
        if (_write_depth == rgb565_2Byte)
        {
          rawcolor = getSwap16(rawcolor);
          _write_reg(0x63, rawcolor >>11);
          _write_reg(0x64, rawcolor >> 5);
          _write_reg(0x65, rawcolor     );
        }
        else
        {
          _write_reg(0x63, rawcolor >> 5);
          _write_reg(0x64, rawcolor >> 2);
          _write_reg(0x65, rawcolor     );
        }
      }

      uint_fast8_t r = _internal_rotation;
      if (r)
      {
        if ((1u << r) & 0b10010110) { y = _height - (y + h); }
        if (r & 2)                  { x = _width  - (x + w); }
        if (r & 1) { std::swap(x, y);  std::swap(w, h); }
      }

      _write_reg_0x51(0x51, 0x0C); // Solid Fill.

      x += _colstart;
      y += _rowstart;
      uint16_t data16[6] = { (uint16_t)x
                           , (uint16_t)y
                           , (uint16_t)w
                           , (uint16_t)h
                           };
      auto data = (uint8_t*)data16;
      for (size_t idx = 0; idx < 8; ++idx)
      {
        _write_reg_0x51(0x58 + idx, data[idx]);
      }
      _write_reg(0x50, 0x80);
    }
  }

  void Panel_RA8875::writeBlock(uint32_t rawcolor, uint32_t length)
  {
    uint32_t xpos = _xpos;
    uint32_t ypos = _ypos;
    do
    {
      bool flg_setpos = false;
      if (xpos > _xe) { flg_setpos = true; xpos = _xs; ++ypos; }
      if (ypos > _ye) { flg_setpos = true; ypos = _ys;         }
      if (flg_setpos)
      {
        _set_write_pos(xpos, ypos);
      }

      _start_memorywrite();

      auto w = std::min<uint32_t>(length, _xe + 1 - xpos);
      xpos += w;
      _xpos = xpos;
      _bus->writeDataRepeat(rawcolor, _write_bits, w);
      length -= w;
    } while (length);
  }

  void Panel_RA8875::writePixels(pixelcopy_t* param, uint32_t length, bool use_dma)
  {
    uint32_t xpos = _xpos;
    uint32_t ypos = _ypos;
    do
    {
      bool flg_setpos = false;
      if (xpos > _xe) { flg_setpos = true; xpos = _xs; ++ypos; }
      if (ypos > _ye) { flg_setpos = true; ypos = _ys;         }
      if (flg_setpos)
      {
        _set_write_pos(xpos, ypos);
      }

      _start_memorywrite();

      auto w = std::min<uint32_t>(length, _xe + 1 - xpos);
      xpos += w;
      _xpos = xpos;

      if (param->no_convert)
      {
        _bus->writeBytes(reinterpret_cast<const uint8_t*>(param->src_data), w * _write_bits >> 3, true, use_dma);
      }
      else
      {
        _bus->writePixels(param, w);
      }

      length -= w;
    } while (length);
  }

  void Panel_RA8875::write_bytes(const uint8_t* data, uint32_t length, bool use_dma)
  {
    uint32_t xpos = _xpos;
    uint32_t ypos = _ypos;
    do
    {
      bool flg_setpos = false;
      if (xpos > _xe) { flg_setpos = true; xpos = _xs; ++ypos; }
      if (ypos > _ye) { flg_setpos = true; ypos = _ys;         }
      if (flg_setpos)
      {
        _set_write_pos(xpos, ypos);
      }

      _start_memorywrite();

      auto w = std::min<uint32_t>(length >> (_write_bits >> 4), _xe + 1 - xpos);
      xpos += w;
      _xpos = xpos;
      w <<= (_write_bits >> 4);
      _bus->writeBytes(data, w, true, use_dma);
      data += w;
      length -= w;
    } while (length);
  }

  void Panel_RA8875::writeImage(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, pixelcopy_t* param, bool use_dma)
  {
    auto bytes = param->dst_bits >> 3;
    auto src_x = param->src_x;

    if (param->transp == pixelcopy_t::NON_TRANSP)
    {
      setWindow(x, y, x + w - 1, y + h - 1);
      if (param->no_convert)
      {
        auto wb = w * bytes;
        uint32_t i = (src_x + param->src_y * param->src_bitwidth) * bytes;
        auto src = &((const uint8_t*)param->src_data)[i];
        auto add = param->src_bitwidth * bytes;
        do
        {
          write_bytes(src, wb, use_dma);
          src += add;
        } while (--h);
      }
      else
      {
        do
        {
          writePixels(param, w, use_dma);
          param->src_x = src_x;
          param->src_y++;
        } while (--h);
      }
    }
    else
    {
      h += y;
      uint32_t wb = w * bytes;
      do
      {
        uint32_t i = 0;
        while (w != (i = param->fp_skip(i, w, param)))
        {
          auto buf = _bus->getDMABuffer(wb);
          int32_t len = param->fp_copy(buf, 0, w - i, param);
          setWindow(x + i, y, x + i + len - 1, y);
          write_bytes(buf, len * bytes, true);
          if (w == (i += len)) break;
        }
        param->src_x = src_x;
        param->src_y++;
      } while (++y != h);
    }
  }

  void Panel_RA8875::copyRect(uint_fast16_t dst_x, uint_fast16_t dst_y, uint_fast16_t w, uint_fast16_t h, uint_fast16_t src_x, uint_fast16_t src_y)
  {
    uint_fast8_t r = _internal_rotation;
    if (r)
    {
      if ((1u << r) & 0b10010110) { src_y = _height - (src_y + h); dst_y = _height - (dst_y + h); }
      if (r & 2)                  { src_x = _width  - (src_x + w); dst_x = _width  - (dst_x + w); }
      if (r & 1) { std::swap(src_x, src_y); std::swap(dst_x, dst_y); std::swap(w, h); }
    }

    bool positive = (dst_y < src_y || (dst_y == src_y && dst_x < src_x));

    _write_reg_0x51( 0x51, positive ? 0xC2 : 0xC3 );
    if (!positive)
    {
      src_x += w - 1;
      dst_x += w - 1;
      src_y += h - 1;
      dst_y += h - 1;
    }
    src_x += _colstart;
    dst_x += _colstart;
    src_y += _rowstart;
    dst_y += _rowstart;

    uint16_t data16[6] = { (uint16_t)src_x
                         , (uint16_t)src_y
                         , (uint16_t)dst_x
                         , (uint16_t)dst_y
                         , (uint16_t)w
                         , (uint16_t)h
                         };
    auto data = (uint8_t*)data16;
    for (size_t idx = 0; idx < 12; ++idx)
    {
      _write_reg_0x51(0x54 + idx, data[idx]);
    }
    _write_reg(0x50, 0x80);
  }

  void Panel_RA8875::readRect(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, void* dst, pixelcopy_t* param)
  {
    uint_fast16_t bytes = param->dst_bits >> 3;
    auto len = w * h;
    if (!_cfg.readable)
    {
      memset(dst, 0, len * bytes);
      return;
    }

    auto dst8 = (uint8_t*)dst;

    int xadd = 0;
    int yadd = 1;
    uint_fast8_t r = _internal_rotation;
    if (r)
    {
      if ((1u << r) & 0b10010110) { y = _height - (y + 1); yadd = -1; }
      if (r & 2)                  { x = _width  - (x + 1); }
      if (r & 1) { std::swap(x, y); std::swap(xadd, yadd); } // ここでは wとhは入れ替えない;
    }

    x += _colstart;
    y += _rowstart;

    startWrite();
    do
    {
      _write_reg(0x4A, x);
      _write_reg(0x4B, x >> 8);
      _write_reg(0x4C, y);
      _write_reg(0x4D, y >> 8);

      if (_flg_serialbus)
      {
        _wait_busy();
        _bus->writeCommand(0x80 + (0x02 << 8) + (0x40 << 16), 24);
      }
      else
      {
        _bus->flush();
        uint32_t reg = 0x02;
        uint_fast8_t len_ = 8;
        if (_cfg.dlen_16bit)
        {
          len_ <<= 1;
          reg <<= 8;
        }
        _wait_busy();
        _bus->writeCommand(reg, len_);
      }

      _bus->beginRead(_cfg.dummy_read_pixel);
      if (param->no_convert)
      {
        _bus->readBytes(dst8, w * bytes);
      }
      else
      {
        _bus->readPixels(dst8, param, w);
      }
      dst8 += w * bytes;
      _bus->endRead();
      x += xadd;
      y += yadd;
    } while (--h);

    endWrite();

    if (_in_transaction) { cs_control(false); }
  }

//----------------------------------------------------------------------------
 }
}
