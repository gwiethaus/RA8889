#include <Arduino.h>
#include <BusSPI.hpp>
#include <Registers.hpp>
#include <Debug.hpp>


//SPIClass spi(FSPI);

/**
 * @brief Configura o proctocolo de comunicação SPI para o display
 *
 * @verbatim
 * Usa structs internas do IBus (proposta “genérica”)
 * A área de código mostra exemplo de como usar o Config()
 * @endverbatim
 * 
 * @code
 * Bus_SPI spi;
 * RA8889 gfx(PIN_CS, PIN_RESET);
 * 
 * void setup() {
 *      
 *   IBus::SPIBusConfig_t cfg;
 *   cfg.spi_type = FSPI_HOST;                 //Usando barramento FSPI
 *   cfg.pin_mosi = 23;
 *   cfg.pin_miso = 19;
 *   cfg.pin_sclk = 18;
 *   cfg.pin_cs   = 5;
 *   cfg.freq_write = 40000000;                //Frequencia de escrita de 40MHz
 *   
 *   spi.Config(&cfg);                         //Grava a configuração
 *   gfx.setBus(spi);                          //Seta o Bus SPI
 *   gfx.Begin();                              //inicializa o display 
 * @endcode
 *
 * @param IBusConfig_t* cfg
 *        
 * @note None
 */
void Bus_SPI::Config(const IBusConfig_t* cfg) 
{
  // Cast seguro para SPIBusConfig
  const SPIBusConfig_t* scfg = static_cast<const SPIBusConfig_t*>(cfg);
  _cfg = *scfg;
  DEBUG_PRINT("Config()----------------------------------------", 0,false,true);
  DEBUG_PRINT("_cfg.freq_write ", _cfg.freq_write,true,true);  
  DEBUG_PRINT("_cfg.pin_cs     ", _cfg.pin_cs,true,true);
  DEBUG_PRINT("_cfg.pin_miso   ", _cfg.pin_miso,true,true);
  DEBUG_PRINT("_cfg.pin_mosi   ", _cfg.pin_mosi,true,true);
  DEBUG_PRINT("_cfg.pin_sclk   ", _cfg.pin_sclk,true,true);
  DEBUG_PRINT("_cfg.spi_host   ", _cfg.spi_host,true,true);
  DEBUG_PRINT("_cfg.spi_type   ", _cfg.spi_type,true,true);
}


/**
 * @brief Escolha do barramento de comunicação SPI para o display
 *
 * @verbatim
 * None
 * @endverbatim
 *
 * @param SPIHostType hostType
 *        
 * @note None
 */
void Bus_SPI::createSPI(SPIHostType hostType)
{

  //Se já existia um ponteiro de uma instância do objeto no heap, destrói
  if(spi) {
    delete spi;
    spi = nullptr;
	_spi_init = false;
  }

#if defined(ESP32)  //familia do ESP32

  // Família ESP32
  switch (hostType) {
    
  #if HAS_HSPI
    case HOST_HSPI:
         spi =  new SPIClass(HSPI);
         DEBUG_PRINT("Selecionou Objeto HSPI Padrao criado (valor): ", HSPI,true,true);
         break;
  #endif
  
  #if HAS_VSPI
    case HOST_VSPI:
         spi = new SPIClass(VSPI);
		 DEBUG_PRINT("Selecionou Objeto VSPI criado (valor): ", VSPI,true,true);
         break;
  #endif
  
  #if HAS_FSPI
     case HOST_FSPI:
          spi = new SPIClass(FSPI);
          //spi = new SPIClass(3);  //testando estes valores 3 e 2
          DEBUG_PRINT("Selecionou Objeto FSPI criado (valor): ", FSPI,true,true);
          break;
  #endif
  
     case HOST_SPI:
     default:
          spi = new SPIClass(); // SPI padrão
          DEBUG_PRINT("Selecionou Objeto SPI Padrao criado (valor): ", 0,false,true);
          break;
  }
#else
  // Outros MCUs (Arduino, STM32, etc.)
  spi = new SPIClass();
#endif

}


/**
 * @brief Inicializa o SPI para a comunicacao com o Display
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
void Bus_SPI::Init(void) 
{
  if (_spi_init) return;
  DEBUG_PRINT("Entrou em Bus_SPI::Init", 0,false,true);

  pinMode(_cfg.pin_cs, OUTPUT);
  digitalWrite(_cfg.pin_cs, HIGH);
  _lock_bus = false;

  createSPI(static_cast<SPIHostType>(_cfg.spi_type));

  _spi_clockmax = (_cfg.freq_write > 0) ? _cfg.freq_write : SPI_CLOCK_SPEED_MAX;
  _spi_datamode  = SPI_MODE0;
  _spi_dataorder = MSBFIRST;

  DEBUG_PRINT("Entrou em HAS_SPI_TRANSCATION", 0,false,true);
  spi->beginTransaction(SPISettings(_spi_clockmax, _spi_dataorder, _spi_datamode));
  _spi_transaction = true;
  DEBUG_PRINT("Valor do spi clock max: ", _spi_clockmax,true,true);
  DEBUG_PRINT("Valor do spi data order: ", _spi_dataorder,true,true);
  DEBUG_PRINT("Valor do spi data mode: ", _spi_datamode,true,true);

// Determina qual SPI usar
#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3) || defined(CONFIG_IDF_TARGET_ESP32C3)
 
  spi->begin(_cfg.pin_sclk, _cfg.pin_miso, _cfg.pin_mosi, _cfg.pin_cs);

  DEBUG_PRINT("-----------------------------", 0,false,true);
  DEBUG_PRINT("Valor do Freq. write: ", _spi_clockmax,true,true);
  DEBUG_PRINT("Valor do SCLK:        ", _cfg.pin_sclk,true,true);
  DEBUG_PRINT("Valor do MISO:        ", _cfg.pin_miso,true,true);
  DEBUG_PRINT("Valor do MOSI:        ", _cfg.pin_mosi,true,true);
  DEBUG_PRINT("Valor do CS:          ", _cfg.pin_cs,true,true);
#elif defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_MEGA2560)

  spi->begin();

#else

  #warning "SPI não implementada para esta plataforma"
  return;
  
#endif

  _spi_startwrite = false;
  _spi_init = true;
}


/**
 * @brief Verifica e garante que exista uma transação SPI ativa.
 *
 * Esta função é utilizada em chamadas que não fazem parte de um bloco de escrita
 * volumosa (StartWrite/EndWrite), assegurando que ao menos uma transação esteja ativa.
 *
 * Caso nenhuma transação esteja em andamento, inicia uma nova automaticamente.
 *
 * É especialmente útil em comandos isolados, como leituras de status ou registros
 * de controle, prevenindo falhas por ausência de beginTransaction().
 *
 * @return void
 *
 * @note
 * - Se `_spi_startwrite == false` e `_spi_transaction == false`, inicia uma nova transação SPI.
 * - Não altera `_spi_startwrite`, pois seu objetivo é apenas manter o barramento ativo.
 */
void Bus_SPI::CheckTransaction()
{
  if (!_spi_startwrite && !_spi_transaction) {
    spi->beginTransaction(SPISettings(_spi_clockmax, _spi_dataorder, _spi_datamode));
  	_spi_transaction = true;
  }
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
  if (_lock_bus) return;
  level_cs == 0 ? digitalWrite(_cfg.pin_cs, LOW) : /*SS_RESET */  digitalWrite(_cfg.pin_cs, HIGH); /*SS_SET*/
}


/**
 * @brief Realiza bloqueio do barramento pelo pino Chip Select (CS)
 *
 * Não serão executados bloqueio/desbloqueio de barramento pelo pino CS dentro 
 * de metodos/funções. Desta forma ocorrendo apenas uma única chamada de 
 * bloqueio  por esta função LockBus()
 * 
 * @verbatim
 * None
 * @endverbatim
 * 
 * @param force_unlock Força o desbloquio do barramento caso por algum motivo estava bloqueado os o esquecimento do uso da função UnlockBus();
 *
 */
void Bus_SPI::LockBus(bool force_unlock)
{
  if (force_unlock) {
    if (digitalRead(_cfg.pin_cs) == LOW) {
	  SetCS(1);  //para evitar alguma coisa mal resolvida
	  _lock_bus = false;
	}
  }
  if (_lock_bus) return;
  SetCS(0);
  _lock_bus = true;
}


/**
 * @brief Realiza desbloqueio do barramento pelo pino Chip Select (CS)
 *
 * Serão novamente executados bloqueio/desbloqueios de barramento pelo pino CS 
 * dentro de metodos/funções. Desta forma, é liberado o barramento por essa 
 * função LockBus().
 * 
 * @verbatim
 * None
 * @endverbatim
 * 
 * @param None
 *
 */
void Bus_SPI::UnlockBus(void)
{
  if (!_lock_bus) return;
  _lock_bus = false;
  SetCS(1);
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
  result = spi->transfer(value);
  return result;
}


/**
 * @brief Escrever um buffer de dados para barramento SPI
 *
 * @verbatim
 * None
 * @endverbatim
 * 
 * @param value: ponteiro de buffer de dados para o SPI
 * @param len:   tamanho de dados para enviar para o SPI
 *
 * @note Note
 */
void Bus_SPI::RwBytes(const uint8_t* data, uint32_t len)
{

  #if defined(ESP32) || defined(ESP8266)
    // --- ESP32 / ESP8266 ---
    // Usa DMA interno e é o método mais rápido disponível.
    spi->writeBytes(data, len);
    //spi->transferBytes(data, nullptr, len);
  #elif defined(ARDUINO_ARCH_SAM)
    // --- Arduino Due (ARM Cortex-M3) ---
    // Usa o método nativo de envio em bloco.
    for (size_t i = 0; i < len; i++) {
        spi->transfer(data[i]);
    }
  #elif defined(ARDUINO_ARCH_AVR)
    // --- AVR (UNO, Mega, Nano, etc.) ---
    // transfer(buf, len) nem sempre existe no AVR SPI padrão.
    // Então envia byte a byte.
    for (size_t i = 0; i < len; i++) {
        spi->transfer(data[i]);
    }
  #elif defined(ARDUINO_ARCH_RP2040)
    // --- Raspberry Pi Pico ---
    spi->transfer((void*)data, len);
  #else
    // --- Fallback genérico ---
    spi->transfer((void*)data, len);
  #endif
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
  CheckTransaction();
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
  CheckTransaction();
  SetCS(0);                                    //SS_RESET;
  RwByte(SPI_DATAWRITE);                       //0x80, Indica Dados para escrever
  RwByte(data);                                //Envia um byte de Dado para o SPI
  SetCS(1);                                    //SS_SET;
}


/**
 * @brief Escreve dados de 1 a 4 bytes (até 32 bits) para a controladora de display via barramento SPI.
 *
 * Esta função envia de 1 até 4 bytes consecutivos (definidos por @p step) ao barramento SPI, 
 * precedidos pelo prefixo de controle SPI_DATAWRITE (0x80), que indica à controladora 
 * que os bytes subsequentes são dados de escrita.
 *
 * O valor de @p data é deslocado em blocos de 8 bits e enviado no formato LSB → MSB, 
 * conforme o padrão de transmissão SPI da maioria dos controladores gráficos (ex.: RA8889).
 * 
 * O número de bytes a enviar é automaticamente limitado entre 1 e 4, mesmo que o valor de @p step 
 * informado esteja fora desse intervalo.
 * 
 * @param data Valor de até 32 bits contendo os dados a serem transmitidos.
 *              Apenas os bytes menos significativos são utilizados, conforme @p step.
 * @param step Quantidade de bytes a transmitir (1 a 4). 
 *              Valores fora desse intervalo serão automaticamente ajustados.
 *
 * @note 
 * - A sequência de transmissão é:
 *   1. Verificação de transação ativa via CheckTransaction().
 *   2. Ativação do sinal CS (SetCS(0)).
 *   3. Envio do byte de controle SPI_DATAWRITE (0x80).
 *   4. Transmissão de @p step bytes de dados em ordem LSB → MSB.
 *   5. Desativação do sinal CS (SetCS(1)).
 * - Caso a controladora utilize convenção MSB → LSB, a ordem de envio deverá ser ajustada no loop.
 * 
 * @see Bus_SPI::CmdWrite()
 */
void Bus_SPI::DataWrite(uint32_t data, uint8_t step)
{
  CheckTransaction();
  SetCS(0);                                    //SS_RESET;
  step = (step < 1) ? 1 : (step > 4 ? 4 : step);
  RwByte(SPI_DATAWRITE);                       //0x80, Indica Dados para escrever 
  for (uint8_t i = 0; i < step; i++)
    RwByte(data >> (i * 8));                   //Envia de byte 1 até 4 de dados para o SPI
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
  CheckTransaction();
  SetCS(0);                                    //SS_RESET
  RwByte(SPI_DATAREAD);                        //0xc0, Leitura de dados
  temp = RwByte(0x00);                         //
  SetCS(1);                                    //SS_SET
  return temp;
}



/**
 * @brief Lê dados da controladora RA8889 (ou similar) via barramento SPI.
 *
 * Esta função realiza a leitura de 1 a 4 bytes consecutivos a partir do registrador
 * de dados da controladora gráfica. O primeiro byte recebido corresponde à parte mais
 * alta (MSB) e o último byte à parte mais baixa (LSB). O valor final é composto em uma
 * variável de 32 bits no formato MSB→LSB (ex: 0xAABBCCDD).
 *
 * @verbatim
 * Exemplo de sequência:
 *  Bytes recebidos: 0xAA, 0xBB, 0xCC, 0xDD
 *  Valor final:     0xAABBCCDD
 * @endverbatim
 *
 * @param step Quantidade de bytes a ler (1 a 4). Valores fora do intervalo são
 *             automaticamente limitados.
 *
 * @return uint32_t Valor lido (até 32 bits), composto na ordem MSB→LSB.
 *
 * @note A comunicação SPI deve estar previamente configurada. A função controla
 *       automaticamente o sinal de chip select (CS) e envia o comando SPI_DATAREAD
 *       antes de iniciar a leitura dos bytes de dados.
 *
 * @see RwByte()
 * @see CheckTransaction()
 * @see SetCS()
 */
uint32_t Bus_SPI::DataRead(uint8_t step)
{
  uint32_t data;
  CheckTransaction();
  SetCS(0);                                    //SS_RESET
  step = (step < 1) ? 1 : (step > 4 ? 4 : step);  
  RwByte(SPI_DATAREAD);                        //0xc0, Leitura de dados
  for (uint8_t i = 0; i < step; i++) {
    uint8_t byte_in = RwByte(0x00);
    data = (data << 8) | byte_in;              // MSB primeiro → desloca antes de somar
  }
  SetCS(1);                                    //SS_SET
  return data;
}


/**
 * @brief Lê dados da controladora RA8889 (ou similar) via barramento SPI.
 *
 * Esta função realiza a leitura de 1 a 4 bytes consecutivos a partir de um endereço
 * da controladora gráfica. O primeiro byte recebido corresponde à parte mais alta
 * (MSB) e o último byte à parte mais baixa (LSB). O valor final é composto em uma
 * variável de 32 bits no formato MSB→LSB (ex: 0xAABBCCDD).
 *
 * @verbatim
 * Exemplo de sequência:
 *  Bytes recebidos: 0xAA, 0xBB, 0xCC, 0xDD
 *  Valor final:     0xAABBCCDD
 * @endverbatim
 *
 * @param address Endereço ou comando a ser enviado antes da leitura dos dados.
 * @param step Quantidade de bytes a ler (1 a 4). Valores fora do intervalo são
 *             automaticamente limitados.
 *
 * @return uint32_t Valor lido (até 32 bits), alinhado conforme a ordem MSB→LSB.
 *
 * @note A comunicação SPI deve estar previamente configurada. A função controla
 *       automaticamente o sinal de chip select (CS).
 *
 * @see RwByte()
 * @see CheckTransaction()
 * @see SetCS()
 */
uint32_t Bus_SPI::DataRead(uint8_t address, uint8_t step)
{
  uint32_t data;
  CheckTransaction();
  SetCS(0);                                    //SS_RESET
  step = (step < 1) ? 1 : (step > 4 ? 4 : step);  
  RwByte(address);                             //
  for (uint8_t i = 0; i < step; i++) {
    uint8_t byte_in = RwByte(0x00);
    data = (data << 8) | byte_in;              // MSB primeiro → desloca antes de somar
  }
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
  CheckTransaction();
  SetCS(0);                                    //SS_RESET
  RwByte(SPI_STATUSREAD);                      //0x40, Read Status SPI
  temp = RwByte(REG_STSR);                     //0x00, Read STSR Register
  SetCS(1);                                    //SS_SET
  return temp;
}


/** 
 * @brief Escrever em um registrador da controladora display
 *
 * @warning Não é permitido uso de LockBus()/UnlockBus()
 * 
 * @verbatim
 * None
 * @endverbatim
 * 
 * @param None
 *
 * @note reg: registrador do display, data: dados para escrever no registrador
 *       Não é necessário o uso interno da função CheckTransaction(), pois  CmdWrite() e DataRead() já possuem o  CheckTransaction()
 * 
 * @return None
 *
 * @see LockBus()
 * @see UnlockBus()
 * @see CmdWrite();
 * @see DataWrite(data);
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
 *       Não é necessário o uso interno da função CheckTransaction(), pois  CmdWrite() e DataRead() já possuem o  CheckTransaction()
 *
 * @return dados do registrador
 */
uint8_t Bus_SPI::RegisterRead(uint8_t reg)
{
  CmdWrite(reg);
  return DataRead();
}


/** 
 * @brief Escreve um buffer de dados para SPI
 *
 * @verbatim
 * None
 * @endverbatim
 * 
 * @param None
 *
 * @return None
 *
 * @see CheckTransaction()
 * @see SetCS()
 * @see RwBytes()
 */
void Bus_SPI::WriteBytes(const uint8_t* data, size_t len)
{
  if (!_spi_init || data == nullptr || len == 0) return;
  CheckTransaction();
  SetCS(0);                                    //SS_RESET
  RwBytes(data, len);
  SetCS(1);                                    //SS_SET
}


/**
 * @brief Inicia um bloco de escrita SPI e garante uma transação ativa.
 *
 * Esta função inicia uma transação SPI se ainda não houver uma ativa,
 * configurando os parâmetros do barramento (clock, ordem de bits e modo SPI)
 * e marcando o início de um bloco de escrita de dados volumosos.
 *
 * É ideal para ser utilizada em operações de alto tráfego, como transferências
 * gráficas no LVGL, evitando o overhead de múltiplos beginTransaction().
 *
 * @return 0 SPI nao foi iniciado
 *         1 Já existe uma transação ativa
 *         2 Transação inciciada
 * @note
 * - Define os flags internos `_spi_transaction = true` e `_spi_startwrite = true`.
 * - Caso o SPI ainda não tenha sido inicializado (`_spi_init == false`), a função não executa.
 *
 * @code
 * bus->StartWrite();
 * bus->DrawPixels(...);
 * bus->EndWrite();
 * @endcode
 */
uint8_t Bus_SPI::StartWrite(void)
{
  uint8_t status = 0;
  
  if (!_spi_init) return status;

  if (!_spi_transaction) {
    spi->beginTransaction(SPISettings(_spi_clockmax, _spi_dataorder, _spi_datamode));
    _spi_transaction = true;
    status = 2;
    Serial.println("ja tinha umsa transacao... nao podia entrar aqui!!!!!!!!");
  } else status = 3;

  if (!_spi_startwrite) {
    _spi_startwrite = true;
  }

  return status;
}


/**
 * @brief Finaliza um bloco de escrita SPI e encerra a transação ativa.
 *
 * Esta função encerra uma transação SPI iniciada por StartWrite(),
 * liberando o barramento e retornando o SPI ao estado ocioso.
 *
 * Deve ser utilizada após o término de transferências intensas,
 * como o envio de buffers de imagem para o display.
 *
 * @return void
 *
 * @note
 * - Finaliza a transação somente se `_spi_startwrite == true` e `_spi_transaction == true`.
 * - Define `_spi_transaction = false` e `_spi_startwrite = false` ao final.
 */
void Bus_SPI::EndWrite(void)
{
  if (!_spi_init) return;
  if (!_spi_startwrite) return;
  
  if (_spi_startwrite) {
    _spi_startwrite = false;
  }
  
  if (_spi_transaction) {
    spi->endTransaction();
   _spi_transaction = false;
  }
}
