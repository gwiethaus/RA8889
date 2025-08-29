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
    - Sistema da porta de itnerrupção para a tla de toque do display
	
*/

//================================================================================
// Funções Principais de Inicializacao
//================================================================================

//Construtor da classe
Panel_RA8889::Panel_RA8889(uint8_t cs, uint8_t rst) {
  _cs = cs;
  _rst = rst;
  _width = 800;
  _height = 480;
  
}


//tabalhan do nesta funcao....
//inicializa o display
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
  
//Descomentar a medida que as funcoes vao ficando pronta  

//Chip Configuration Register (CCR) [01h]

  TFT_16bit();
  HostDataBus_Select_16bit();                  //Host bus 16bit
      
//Memory Access Control Register (MACR) [02h]

  HostColorDepthFormat(0);                     //Host MPU/MCU I/F 8/16 bits color depth 8/16/24 mode 1
  HostReadMemoryDirection(MemoryDirection::LeftRight_TopBotom);

//Input Control Register (ICR) [03h]

  GraphicMode();
  Memory_Select_SDRAM();

//Display Configuration Register (DPCR) [12h]

//  ER_TFT.HSCAN_L_to_R();     //REG[12h]:from left to right
//  ER_TFT.VSCAN_T_to_B();       //REG[12h]:from top to bottom
//  ER_TFT.PDATA_Set_RGB();        //REG[12h]:Select RGB output




//  ER_TFT.Set_PCLK(LCD_PCLK_Falling_Rising);   //LCD_PCLK_Falling_Rising
//  ER_TFT.Set_HSYNC_Active(LCD_HSYNC_Active_Polarity);
//  ER_TFT.Set_VSYNC_Active(LCD_VSYNC_Active_Polarity);
//  ER_TFT.Set_DE_Active(LCD_DE_Active_Polarity);
 
//  ER_TFT.LCD_HorizontalWidth_VerticalHeight(LCD_XSIZE_TFT ,LCD_YSIZE_TFT);
//  ER_TFT.LCD_Horizontal_Non_Display(LCD_HBPD);                          
//  ER_TFT.LCD_HSYNC_Start_Position(LCD_HFPD);                              
//  ER_TFT.LCD_HSYNC_Pulse_Width(LCD_HSPW);                              
//  ER_TFT.LCD_Vertical_Non_Display(LCD_VBPD);                               
//  ER_TFT.LCD_VSYNC_Start_Position(LCD_VFPD);                               
//  ER_TFT.LCD_VSYNC_Pulse_Width(LCD_VSPW);                              
      
//  ER_TFT.Select_Main_Window_16bpp();

//  ER_TFT.Memory_XY_Mode(); //Block mode (X-Y coordination addressing)
//  ER_TFT.Memory_16bpp_Mode();
//  ER_TFT.Select_Main_Window_16bpp();  
    
}



//Liga o display
//on: true, liga display, false: desliga display 
void Panel_RA8889::DisplayOn(bool on)
{
  //Display ON/OFF, bit 5 do registardor DPCR
  // 0: Display Off.
  // 1: Display On.
  uint8_t temp;
  SPI_CmdWrite(REG_DPCR);  //0x12
  temp = SPI_DataRead();
  if (on) temp |= cSetb6 else temp &= (~cSetb6);
  SPI_DataWrite(temp);
}

//Passa o display para modo grafico
void Panel_RA8889::GraphicMode(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_ICR);  //0x03
  temp = SPI_DataRead();
  temp &= cClrb2;         //desliga o bit 2, ativando modo grafico
  SPI_DataWrite(temp);
}

//Verifica se o modo grafico está ativo
//false: modo texto
//true: modo grafico 
bool Panel_RA8889::GraphicMode(void){
  uint8_t temp;
  SPI_CmdWrite(REG_ICR);  //0x03
  temp = SPI_DataRead();
  return ((temp == cSetb2) == 0x00);      //Veja se o bit 2 está desligado
}

//Passa o display para modo texto
void Panel_RA8889::TextMode(void)
{
  uint8_t temp;
  SPI_CmdWrite(REG_ICR);  //0x03
  temp = SPI_DataRead();
  temp |= cSetb2;         //liga o bit 2, ativando o modo texto
  SPI_DataWrite(temp);
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
void Panel_RA8889::SPI_CmdWrite(int cmd)
{
  SPISetCS(false);    //SS_RESET;
  SPIRwByte(0x00);    //Indica Commando para escrever 
  SPIRwByte(cmd);     //Envia um comando byte para o SPI 
  SPISetCS(true);     //SS_SET;
}

//SPI_DataWrite
//Escreve dados para o SPI
void Panel_RA8889::SPI_DataWrite(int data)
{
  SPISetCS(false);    //SS_RESET;
  SPIRwByte(0x80);    //Indica Dados para escrever
  SPIRwByte(data);    //Envia um byte de Dado para o SPI
  SPISetCS(true);     //SS_SET;
}

//SPI_DataWritePixel
void Panel_RA8889::SPI_DataWrite_Pixel(int data)
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
int Panel_RA8889::SPI_DataRead(void)
{
  int temp = 0;
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
int Panel_RA8889::StatusRead(void)
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


//================================================================================
// Funcoes PLL
//================================================================================


/**
 * Aguarda até que o RA8889 finalize sua inicialização interna e o PLL esteja pronto.
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
    temp = StatusRead();              //Read Status Register STSR
    if((temp & 0x02) == 0x00)         //Veja se o bit 2 esta limpo (0x00=modo de operação normal, evento de inicialização interna terminou)
    {
      
	  delay(2);                       //MCU too fast, necessary
      SPI_CmdWrite(0x01);             //Access register Chip Configuration Register (CCR)
      delay(2);                       //MCU too fast, necessary
      temp = SPI_DataRead();          //Leia o CCR 
      if((temp & 0x80) == 0x80)       //Check CCR register's PLL is ready or not (bit 7 = 1)
      {
        system_ok = true;             //PLL pronto
        count_timeout = 0;
      } else {
        delay(2);                     //MCU too fast, necessary
        SPI_CmdWrite(0x01);           //Access register Chip Configuration Register (CCR)
        delay(2);                     //MCU too fast, necessary
        SPI_DataWrite(0x80);          //Reconfigura a frequencia do PLL
      }
	  
    } else {                          
      system_ok = false;              //A inicialização interna ainda está sendo feita
      count_timeout++;                //fazer outra tentativa
    }
	
    if(system_ok==false && count_timeout==5)      //Sistema ainda nao está pronto e houve timeout
    {
      ChipHardwareReset();            //*note1, envia um reset novamente
      count_timeout = 0;              //zera o cotnador de timeout 
    }
	
  } while(system_ok==false);          //faz enquanto não ficar pronto o sistema
}


/**
 * @brief Configura o PLL do RA8889 para ajustar as frequências de:
 *        - Pixel Clock (SCAN_FREQ)
 *        - SDRAM Clock (DRAM_FREQ)
 *        - Core Clock (CORE_FREQ)
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
  //REG[05h] SCLK PLL Control Register 1 (PPLLC1) - SCAN or PIXEL Clock PLL
  //REG[07h] MCLK PLL Control Register 1 (MPLLC1) - MEMORY Clock PLL
  //REG[09h] CCLK PLL Control Register 1 (SPLLC1) - CORE or SYSTEM Clock PLL

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
  SPI_CmdWrite(REG_CCR);         //0x01, Envia comando Chip Configuration Register (CCR) 
  SPI_CmdWrite(0x00);            //Como o CCR possui tudo zerado por default ainda na inicilizacao e configuração do dispositivo, o bit 7 será zerado (inicia com 1 como default)
  delay(1);                      //Aguarda para estabilizar
  
  // ---------- Habilita PLL com novos valores ----------
  SPI_CmdWrite(0x80);            //Comando para ligar PLL
  delay(1);                      //Aguarda para estabilizar

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
void Panel_RA8889::SDRAM_WaitReady(void)
{
  //Bit 2 do registrador STSR
  //0: SDRAM is not ready for access
  //1: SDRAM is ready for access
  uint8_t temp;
  do
  {
    temp = StatusRead();
  }
  while( (temp & 0x04) == 0x00 );
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
void Panel_RA8889::TFT_SetInterface(TFTInterface mode)
{
  uint8_t temp;
  SPI_CmdWrite(REG_CCR);                 //0x01, Chip Configuration Register (CCR) 
  temp = SPI_DataRead();
  temp &= ( cClrb4 & cClrb3);            //Limpa bits 4 e 3
  temp |= static_cast<uint8_t>(mode);    //Converte enum para uint8_t
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
 * @brief Seleciona o destino da porta de memória do RA8889 para a SDRAM.
 *
 * Configura os bits [1:0]=00b do registrador ICR (0x03)
 *
 * @param Nenhum
 *
 * @note Image buffer (SDRAM) for image data, pattern (palette), user-characters. 
 *        
 */
void Panel_RA8889::Memory_Select_SDRAM(void)
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
void Panel_RA8889::Memory_Select_Gamma_Table(void)
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
void Panel_RA8889::Memory_Select_Graphic_Cursor_RAM(void)
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
void Panel_RA8889::Memory_Select_Color_Palette_RAM(void)
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
void Panel_RA8889::MemoryPort_Select(MemoryPortDest dest);
{
  uint8_t temp = 0;
  SPI_CmdWrite(REG_ICR);               //0x03, Input Control Register (ICR)
  temp = SPI_DataRead();               //Lê valor atual do registrador
  temp &= 0xfc;                        //Limpa bit 1 e 0
  temp |= static_cast<uint8_t>(dest);  //Define o destino
  SPI_DataWrite(temp);                 //Atualiza registrador
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
