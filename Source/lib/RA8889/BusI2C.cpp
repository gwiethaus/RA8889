#include <BusI2C.hpp>


/**
 * @brief Configura o proctocolo de comunicação Paralela 8/16 bits para o display
 *
 * @verbatim
 * Usa structs internas do IBus (proposta “genérica”)
 * A área de código mostra exemplo de como usar o Config()
 * @endverbatim
 * 
 * @code
 * Bus_I2C i2c;
 * RA8889 gfx(PIN_CS, PIN_RESET);
 * 
 * void setup() {
 *      
 *   IBus::I2CBusConfig cfg;   // Struct específica para I²C
 *   cfg.pin_sda = 21;
 *   cfg.pin_scl = 22;
 *   cfg.freq = 400000;        // 400 kHz padrão, se quiser
 *  
 *   i2c.Config(&cfg);                         // Grava a configuração
 *   gfx.setBus(i2c);                          // Seta o Bus I2C
 *   gfx.Begin();                              // inicializa o display 
 * @endcode
 *
 * @param IBusConfig_t* cfg
 *        
 * @note None
 */
void Bus_I2C::Config(const IBusConfig_t* cfg) 
{
  const I2CBusConfig_t* icfg = static_cast<const I2CBusConfig_t*>(cfg);
  _cfg = *icfg;
  DEBUG_PRINT("Config()----------------------------------------", 0,false,true);
  DEBUG_PRINT("_cfg.freq       ", _cfg.freq,true,true);  
  DEBUG_PRINT("_cfg.pin_sda    ", _cfg.pin_sda,true,true);
  DEBUG_PRINT("_cfg.pin_scl    ", _cfg.pin_scl,true,true);
}



/**
 * @brief Inicializa o I2C para a comunicacao com o Display
 *
 * @verbatim
 * None
 * @endverbatim
 * 
 */
uint32_t Bus_I2C::Init(void) 
{
  if (_i2c_init) return -1;
  DEBUG_PRINT("Entrou em Bus_I2C::Init", 0,false,true);

  Wire.begin();                                // Inicializa I2C padrão
  Wire.setClock(_cfg.freq);                    // I2C Fast Mode 400kHz (se seu display suportar)
  
  _i2c_init = true;
  return 0;
}


uint8_t Bus_I2C::StartWrite(void)
{
  return 0;
}


void Bus_I2C::EndWrite(void)
{
  return;
}


//esta funcao nao existe para I2C
void Bus_I2C::LockBus(bool force_unlock)
{
  return;
}


//esta funcao nao existe para I2C
void  Bus_I2C::UnlockBus(void)
{
  return;
}


uint8_t Bus_I2C::RwByte(uint8_t value)
{
  uint8_t result;
  result = Wire.write(value);
  return result;
}


/**
 * @brief Envia um bloco de bytes para a controladora RA887x via I²C.
 *
 * Esta função é usada para transmitir sequências de dados (por exemplo,
 * transferência contínua de pixels) para o display através da interface I²C.
 * O comportamento interno depende da plataforma, pois o tamanho máximo de buffer
 * suportado pelo driver I²C pode variar.
 *
 * @param data Ponteiro para o buffer contendo os bytes a serem enviados.
 * @param len  Número de bytes a transmitir.
 *
 * @note Considerações importantes sobre desempenho:
 * - O padrão I²C **não possui chip select (CS)**, logo **cada chamada desta função
 *   inicia e encerra explicitamente uma transmissão** (`beginTransmission` / `endTransmission`).
 *   Isso introduz overhead significativo em transmissões sucessivas.
 *
 * - Em plataformas **AVR (Arduino Uno, Mega, Nano, etc.)**, o buffer interno da
 *   biblioteca `Wire` é normalmente limitado a **32 bytes**. Portanto, transmissões
 *   maiores que este valor são automaticamente enviadas em blocos menores para evitar overflow.
 *
 * - Em plataformas **ESP32 / ARM / RP2040**, o buffer de I²C costuma ser maior e
 *   otimizado, mas **o barramento I²C continua sendo lento** comparado ao SPI ou
 *   barramentos paralelos. Portanto, ainda assim não é indicado para atualizações
 *   frequentes de framebuffer (risco de efeito "cortina").
 *
 * @warning Esta função deve ser usada apenas para envio de registradores ou pequenas
 * quantidades de dados. Para telas grandes (ex.: 480x272, 800x480), recomenda-se:
 *   - **SPI em alta velocidade (40–80 MHz)**, ou
 *   - **Paralelo FSMC / I80 (8 ou 16 bits)**.
 *
 * @see Bus_SPI::RwBytes() para transferência rápida via SPI.
 */
uint32_t Bus_I2C::RwBytes(const uint8_t* data, uint32_t len)
{
  // Inicia transmissão para o endereço do RA887x
  //Wire.beginTransmission(_cfg.address);

#if defined(ESP32) || defined(ESP8266)
  // --- ESP32 / ESP8266 ---
  // Wire.write suporta buffer direto -> mais rápido.
  Wire.write(data, len);

#elif defined(ARDUINO_ARCH_STM32)
  // --- STM32 (Arduino_Core_STM32) ---
  // Também suporta buffer direto.
  Wire.write(data, len);

#elif defined(ARDUINO_ARCH_RP2040)
  // --- Raspberry Pi Pico (RP2040) ---
  // Implementação do core suporta write com buffer.
  Wire.write(data, len);

#elif defined(ARDUINO_ARCH_SAM)
  // --- Arduino Due ---
  Wire.write(data, len);

#elif defined(ARDUINO_ARCH_AVR)
  // --- AVR (UNO, Mega, Nano etc.) ---
  // Wire.write(data, len) existe, MAS o buffer interno é pequeno (32 bytes).
  // Então enviamos byte a byte para evitar overflow.
  for(uint32_t i = 0; i < len; i++) {
    Wire.write(data[i]);
  }

#else
  // --- Fallback genérico ---
  for(uint32_t i = 0; i < len; i++) {
    Wire.write(data[i]);
  }

#endif

  // Finaliza transmissão
  //Wire.endTransmission();

  return 0;
}


/**
 * @brief Escreve comando 1 byte para a controladora display via barramento I2C
 *
 * @verbatim
 * None
 * @endverbatim
 * 
 * @param cmd: command value for RA8889
 *
 * @note none
 */
void Bus_I2C::CmdWrite(uint8_t cmd)
{
  Wire.beginTransmission(_cfg.address);
  RwByte(I2C_CMDWRITE);
  RwByte(cmd);
  Wire.endTransmission();
}


void Bus_I2C::DataWrite(uint8_t data)
{
  Wire.beginTransmission(_cfg.address);
  RwByte(I2C_DATAWRITE);
  RwByte(data);
  Wire.endTransmission();
}


void Bus_I2C::DataWrite(uint32_t data, uint8_t step)
{
  Wire.beginTransmission(_cfg.address);
  step = (step < 1) ? 1 : (step > 4 ? 4 : step);
  RwByte(SPI_DATAWRITE);                       //0x80, Indica Dados para escrever 
  for (uint8_t i = 0; i < step; i++)
    RwByte(data >> (i * 8));                   //Envia de byte 1 até 4 de dados para o SPI
  Wire.endTransmission();
}


uint8_t Bus_I2C::DataRead(void)
{
  Wire.beginTransmission(_cfg.address);
  RwByte(SPI_DATAREAD);                        //0xc0, Leitura de dados
  Wire.endTransmission(false);                 //Repeated Start
  Wire.requestFrom(_cfg.address, 1);
  return Wire.available() ? Wire.read() : 0x00;
}


/**
 * @brief Lê 1 a 4 bytes consecutivos da controladora RA887x via I²C.
 *
 * Esta função envia o comando de leitura do registrador de dados da RA887x
 * e em seguida lê a quantidade de bytes solicitada (1 a 4), sempre no formato
 * **MSB primeiro** (big-endian). O valor retornado é composto automaticamente.
 *
 * @param step  Quantidade de bytes a serem lidos (1 a 4). 
 *              Valores fora do intervalo são automaticamente ajustados.
 *
 * @return uint32_t Valor resultante contendo os bytes lidos, alinhados à direita.
 *
 * @note Sobre desempenho e protocolo:
 * - Diferente do SPI, o I²C **não possui pino CS**. Portanto, cada operação de
 *   leitura precisa iniciar com `Wire.beginTransmission()` e terminar com
 *   `Wire.endTransmission()`. Isso introduz overhead a cada acesso.
 * - I²C é adequado para **escrita/leituras ocasionais de registradores**,
 *   mas **não é recomendado** para transferência intensiva de framebuffer
 *   (como telas de 480x272 ou 800x480), sob risco de efeito "cortina" (renderização lenta).
 * - O desempenho máximo dependerá da plataforma:
 *      * AVR (UNO/Mega): buffer I²C interno ~32 bytes → limite de transferência em blocos curtos.
 *      * ESP32 / ARM: melhorias com drivers otimizados e buffers maiores, mas ainda limitado
 *        pelo protocolo serial I²C.
 *
 * @warning Se estiver utilizando LVGL ou qualquer rotina de desenho contínuo,
 *          prefira **SPI em alta velocidade (40~80 MHz)** ou **paralelo FSMC / I80**
 *          para evitar queda drástica de FPS.
 */
uint32_t Bus_I2C::DataRead(uint8_t step)
{
  uint32_t data;
  Wire.beginTransmission(_cfg.address);
  step = (step < 1) ? 1 : (step > 4 ? 4 : step);  
  RwByte(SPI_DATAREAD);                        //0xc0, Leitura de dados
  Wire.endTransmission(false);                 //Repeated Start

  Wire.requestFrom(_cfg.address, step);     //Solicita 'step' bytes do chip
  
  for(uint8_t i = 0; i < step; i++) {
    if (Wire.available()) {
      uint8_t byte_in = Wire.read();
      data = (data << 8) | byte_in; // MSB primeiro
    } else {
      // Falha na leitura → Preenche com 0xFF (comportamento típico)
      data = 0x00;
    }
  }

  return data;
}


uint32_t Bus_I2C::DataRead(uint8_t address, uint8_t step)
{
  uint32_t data;
  Wire.beginTransmission(_cfg.address);
  step = (step < 1) ? 1 : (step > 4 ? 4 : step);  
  RwByte(address);                             //
  Wire.endTransmission(false);                 //Repeated Start

  Wire.requestFrom(_cfg.address, step);        //Solicita 'step' bytes do chip
  
  for(uint8_t i = 0; i < step; i++) {
    if (Wire.available()) {
      uint8_t byte_in = Wire.read();
      data = (data << 8) | byte_in; // MSB primeiro
    } else {
      // Falha na leitura → Preenche com 0xFF (comportamento típico)
      data = 0x00;
    }
  }

  return data;

}


uint8_t Bus_I2C::StatusRead(void)
{
  Wire.beginTransmission(_cfg.address);
  RwByte(SPI_STATUSREAD);                      //0x40, Read Status SPI
  Wire.endTransmission(false);                 //Repeated Start
  Wire.requestFrom(_cfg.address, 1);
  return Wire.available() ? Wire.read() : 0x00;
}


void Bus_I2C::RegisterWrite(uint8_t reg, uint8_t data)
{
  CmdWrite(reg);
  DataWrite(data);
}


uint8_t Bus_I2C::RegisterRead(uint8_t reg)
{
  CmdWrite(reg);
  return DataRead();
}


void Bus_I2C::WriteBytes(const uint8_t* data, size_t len)
{
  if (!_i2c_init || data == nullptr || len == 0) return;
  return;
}
