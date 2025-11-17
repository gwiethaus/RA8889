#ifndef BUSI2C_HPP
#define BUSI2C_HPP

#include <Arduino.h>
#include <Wire.h>
#include <Bus.hpp>
#include <Config.hpp>
#include <Registers.hpp>
#include <Debug.hpp>

class Bus_I2C : public IBus {
  public:
    Bus_I2C() = default;
    void Config(const IBusConfig_t* cfg) override;
  protected:
    I2CBusConfig_t _cfg;                         //Config local específica de I2C
    bool _i2c_init = false;
	
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
    uint32_t DataRead(uint8_t step) override;
    uint32_t DataRead(uint8_t address, uint8_t step) override;
    uint8_t StatusRead(void) override;
    void RegisterWrite(uint8_t reg, uint8_t data) override;
    void WriteBytes(const uint8_t* data, size_t len) override;
    uint8_t RegisterRead(uint8_t reg) override;
};

#endif    // fim de include guard BUSI2C_HPP