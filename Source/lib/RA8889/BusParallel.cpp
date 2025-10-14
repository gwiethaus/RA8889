#include <BusParallel.hpp>


void Bus_Parallel::Config(const config_t& cfg) 
{
  _cfg = cfg;
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
