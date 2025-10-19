#include <Arduino.h>
#include <BusSPI.hpp>
#include <Registers.hpp>
#include <Debug.hpp>

#define PIN_MOSI 11
#define PIN_MISO 13
#define PIN_SCK  12
#define PIN_CS   10

#include <SPI.h>
//SPIClass spi = SPIClass(FSPI);

void Bus_SPI::Config(const IBusConfig_t* cfg) 
{
  // Cast seguro para SPIBusConfig
  const SPIBusConfig_t* scfg = static_cast<const SPIBusConfig_t*>(cfg);
  _cfg = *scfg;
}


void Bus_SPI::Init()
{
  // Inicializa SPI com pinos definidos
  pinMode(PIN_CS, OUTPUT);
  //digitalWrite(PIN_CS, HIGH);
  
  DEBUG_PRINTD("SS   ", SS, true, 0, true);         //Debug
  DEBUG_PRINTD("CS   ", PIN_CS, true, 0, true);         //Debug
  DEBUG_PRINTD("MISO ", MISO, true, 0, true);         //Debug
  DEBUG_PRINTD("MOSI ", MOSI, true, 0, true);         //Debug
  DEBUG_PRINTD("SCK ", SCK, true, 0, true);         //Debug
  
  // Configuração padrão: 8 MHz, MSB primeiro, modo 0
  SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));
  SPI.begin(PIN_SCK, PIN_MISO, PIN_MOSI, PIN_CS);             //metodo esta respondendo
  //SPI.begin();
  DEBUG_PRINTD("<<< SPI Init >>>", 0, false, 0, true);         //Debug
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
void Bus_SPI::SetCS(uint8_t level_cs)
{
  level_cs == 0 ? digitalWrite(_cfg.pin_cs, LOW) : /*SS_RESET */  digitalWrite(_cfg.pin_cs, HIGH); /*SS_SET*/
}


/**
 * @brief Escrever alguma informação para barramento SPI
 *
 * @verbatim
 * None
 * @endverbatim
 * 
 * @param value: dados para SPI
 *
 * @note Note
 */
uint8_t Bus_SPI::RwByte(uint8_t value)
{
  uint8_t result;
  result =  SPI.transfer(value);
  return result;
}


/**
 * @brief Escreve comando 1 byte para a controladora display via barramento SPI
 *
 * @verbatim
 * None
 * @endverbatim
 * 
 * @param cmd: command value for RA8889
 *
 * @note Note
 */
void Bus_SPI::CmdWrite(uint8_t cmd)
{
  SetCS(0);                                    //SS_RESET
  RwByte(SPI_CMDWRITE);                        //0x00, Avisa Display que será um comando
  RwByte(cmd);                                 //Envia um comando de 1 byte para o Display
  SetCS(1);                                    //SS_SET
}


/**
 * @brief Escreve dados 1 byte para a controladora display via barramento SPI
 *
 * @verbatim
 * None
 * @endverbatim
 * 
 * @param cmd: command value for RA8889
 *
 * @note Note
 */
void Bus_SPI::DataWrite(uint8_t data)
{
  SetCS(0);                                    //SS_RESET;
  RwByte(SPI_DATAWRITE);                       //0x80, Indica Dados para escrever
  RwByte(data);                                //Envia um byte de Dado para o SPI
  SetCS(1);                                    //SS_SET;
}
void Bus_SPI::DataWrite8(uint8_t data) {DataWrite(data);}


/**
 * @brief Escreve dados de 2 byte (16 bits) para a controladora display via barramento SPI
 *
 * @verbatim
 * None
 * @endverbatim
 * 
 * @param data: dados de 2 bytes para display
 *
 * @note Note
 */
void Bus_SPI::DataWrite16(uint16_t data)
{
  SetCS(0);                                    //SS_RESET;
  RwByte(SPI_DATAWRITE);                       //0x80, Indica Dados para escrever
  RwByte(data);                                //Envia um byte menos significativo de Dado para o SPI
  RwByte(data >> 8);                           //Envia um byte mais significativo de Dado para o SPI
  SetCS(1);                                    //SS_SET;
}


/**
 * @brief Escreve dados de 3 byte (24 bits) para a controladora display via barramento SPI
 *
 * @verbatim
 * None
 * @endverbatim
 * 
 * @param uint32_t data: dados de 3 bytes para display. A parte alta de bit 31-28 será truncado
 *
 * @note None
 */
void Bus_SPI::DataWrite24(uint32_t data)
{
  SetCS(0);                                    //SS_RESET;
  RwByte(SPI_DATAWRITE);                       //0x80, Indica Dados para escrever 
  RwByte(data);                                //Envia byte 1 de Dado para o SPI
  RwByte(data >> 8);                           //Envia byte 2 de Dado para o SPI
  RwByte(data >> 16);                          //Envia byte 3 de Dado para o SPI
  SetCS(1);                                    //SS_SET;
}


/**
 * @brief Ler dados de 1 byte da controladora display via barramento SPI
 *
 * @verbatim
 * None
 * @endverbatim
 * 
 * @param None
 *
 * @note Note
 */
uint8_t Bus_SPI::DataRead(void)
{
  uint8_t temp;
  SetCS(0);                                    //SS_RESET
  RwByte(SPI_DATAREAD);                        //0xc0, Leitura de dados
  temp = RwByte(0x00);                         //
  SetCS(1);                                    //SS_SET
  return temp;
}


/**
 * @brief Ler dados de 2 byte da controladora display via barramento SPI
 *
 * @verbatim
 * None
 * @endverbatim
 * 
 * @param None
 *
 * @note Note
 */
uint16_t Bus_SPI::DataRead16(uint8_t address)
{
  uint16_t data;
  SetCS(0);                                    //SS_RESET
  SPI.transfer(address);                      //
  data =  SPI.transfer(0x00);                  //MSB
  data <<= 8;                                  //Shift 8 bits right
  data |=  SPI.transfer(0x00);                 //LSB
  SetCS(1);                                    //SS_SET
  return data;
}


/** OK
 * @brief Ler o registardor de estado STSR da controladora display
 *
 * @verbatim
 * None
 * @endverbatim
 * 
 * @param None
 *
 * @note Note
 *
 * @return valor de estadado do STSR
 */
uint8_t Bus_SPI::StatusRead(void)
{
  uint8_t temp = 0;
  SetCS(0);                                    //SS_RESET
  RwByte(SPI_STATUSREAD);                      //0x40, Read Status SPI
  temp = RwByte(REG_STSR);                     //0x00, Read STSR Register
  SetCS(1);                                    //SS_SET
  //DEBUG_PRINT("Pos dentro do Bus_SPI::StatusRead ----> ", temp,true,true);
  return temp;
}


/** 
 * @brief Escrever em um registrador da controladora display
 *
 * @verbatim
 * None
 * @endverbatim
 * 
 * @param None
 *
 * @note reg: registrador do display, data: dados para escrever no registrador
 *
 * @return None
 */
void Bus_SPI::RegisterWrite(uint8_t reg, uint8_t data)
{
  CmdWrite(reg);
  DataWrite(data);
}


/** 
 * @brief Ler um registrador da controladora display
 *
 * @verbatim
 * None
 * @endverbatim
 * 
 * @param None
 *
 * @note reg: registrador do display
 *
 * @return dados do registrador
 */
uint8_t Bus_SPI::RegisterRead(uint8_t reg)
{
  CmdWrite(reg);
  return DataRead();
}
