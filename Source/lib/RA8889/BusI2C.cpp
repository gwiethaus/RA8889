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
 *   cfg.freq_write = 400000;  // opcional, se você quiser controlar
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
}


void Bus_I2C::Init(void) 
{
  return;
}


void Bus_I2C::CmdWrite(uint8_t cmd)
{
  return;
}


void Bus_I2C::DataWrite(uint8_t data)
{
  return;  
}
void Bus_I2C::DataWrite8(uint8_t data) {DataWrite(data);}



void Bus_I2C::DataWrite16(uint16_t data)
{
  return;
}


void Bus_I2C::DataWrite24(uint32_t data)
{
  return;
}


uint8_t Bus_I2C::DataRead(void)
{
  return 0;
}


uint16_t Bus_I2C::DataRead16(uint8_t address)
{
  return 0;
}


uint8_t Bus_I2C::StatusRead(void)
{
  return 0;
}


void Bus_I2C::RegisterWrite(uint8_t reg, uint8_t data)
{
  return;
}


uint8_t Bus_I2C::RegisterRead(uint8_t reg)
{
  return 0;
}
