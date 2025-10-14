#include <Bus.hpp>
#include <Config.hpp>

#if defined(RA8875)
  //#include <RA8875_Reg.hpp>
#elif defined(RA8876)
  #include <RA8876_Reg.hpp>
#elif defined(RA8877)
  #include <RA8877_Reg.hpp>
#elif defined(RA8889)
  #include <RA8889_Reg.hpp>
#endif

class Bus_Parallel : public IBus {
  public:
    Bus_Parallel() = default;
    void Config(const config_t& cfg) override;
  protected:
    config_t _cfg;

    void Init() override;
    void CmdWrite(uint8_t cmd) override;
    void DataWrite(uint8_t data) override;
    void DataWrite8(uint8_t data) override;
    void DataWrite16(uint16_t data) override;
    void DataWrite24(uint32_t data) override;
    uint8_t DataRead(void) override;
    uint16_t DataRead16(uint8_t address) override;
    uint8_t StatusRead(void) override;
    void RegisterWrite(uint8_t reg, uint8_t data) override;
    uint8_t RegisterRead(uint8_t reg) override;
};