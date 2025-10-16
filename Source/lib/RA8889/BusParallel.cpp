#include <BusParallel.hpp>


/**
 * @brief Configura o proctocolo de comunicação Paralela 8/16 bits para o display
 *
 * @verbatim
 * Usa structs internas do IBus (proposta “genérica”)
 * A área de código mostra exemplo de como usar o Config()
 * @endverbatim
 * 
 * @code
 * Bus_Parallel par;
 * RA8889 gfx(PIN_CS, PIN_RESET);
 * 
 * void setup() {
 *      
 *   IBus::ParallelBusConfig_t cfg;            //typedef structure
 *   cfg.data_pins[0] = 29;
 *   cfg.data_pins[1] = 23;
 *   cfg.data_pins[2] = 19;
 *   cfg.data_pins[3] = 18;
 *   cfg.data_pins[4] = 8;
 *   cfg.data_pins[5] = 10;
 *   cfg.data_pins[6] = 14;
 *   cfg.data_pins[7] = 5;
 *   cfg.freq_write = 40000000;
 *   
 *   par.Config(&cfg);                         // Grava a configuração
 *   gfx.setBus(par);                          // Seta o Bus Paralela
 *   gfx.Begin();                              // inicializa o display 
 * @endcode
 *
 * @param IBusConfig_t* cfg
 *        
 * @note None
 */
void Bus_Parallel::Config(const IBusConfig_t* cfg)
{
  const ParallelBusConfig_t* pcfg = static_cast<const ParallelBusConfig_t*>(cfg);
  _cfg = *pcfg;
}


void Bus_Parallel::Init(void) 
{
  return;
}


void Bus_Parallel::CmdWrite(uint8_t cmd)
{
  return;
}


void Bus_Parallel::DataWrite(uint8_t data)
{
  return;  
}
void Bus_Parallel::DataWrite8(uint8_t data) {DataWrite(data);}


void Bus_Parallel::DataWrite16(uint16_t data)
{
  return;
}


void Bus_Parallel::DataWrite24(uint32_t data)
{
  return;
}


uint8_t Bus_Parallel::DataRead(void)
{
  return 0;
}


uint16_t Bus_Parallel::DataRead16(uint8_t address)
{
  return 0;
}


uint8_t Bus_Parallel::StatusRead(void)
{
  return 0;
}


void Bus_Parallel::RegisterWrite(uint8_t reg, uint8_t data)
{
  return;
}


uint8_t Bus_Parallel::RegisterRead(uint8_t reg)
{
  return 0;
}
