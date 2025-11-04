#ifndef BUSI2C_HPP
#define BUSI2C_HPP

#include <Arduino.h>
#include <Bus.hpp>
#include <Config.hpp>

#if defined(RA8875)
  #include <RA8875_Reg.hpp>
#elif defined(RA8876)
  #include <RA8876_Reg.hpp>
#elif defined(RA8877)
  #include <RA8877_Reg.hpp>
#elif defined(RA8889)
  #include <RA8889_Reg.hpp>
#endif

class Bus_I2C : public IBus {
  public:
    Bus_I2C() = default;
    void Config(const IBusConfig_t* cfg) override;
  protected:
    I2CBusConfig_t _cfg;                         //Config local específica de I2C

    void Init() override;
    uint8_t StartWrite(void) override;
    void EndWrite(void) override;
    void LockBus(bool force_unlock = false) override;
    void UnlockBus(void) override;    
    uint8_t RwByte(uint8_t value) override;
	  void RwBytes(const uint8_t* data, uint32_t len) override;
    void CmdWrite(uint8_t cmd) override;
    void DataWrite(uint8_t data) override;
    void DataWrite(uint32_t data, uint8_t step) override;
    uint8_t DataRead(void) override;
    uint16_t DataRead16(uint8_t address) override;
    uint8_t StatusRead(void) override;
    void RegisterWrite(uint8_t reg, uint8_t data) override;
    uint8_t RegisterRead(uint8_t reg) override;
};

#endif    // fim de include guard BUSI2C_HPP