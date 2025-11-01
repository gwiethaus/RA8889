#ifndef BUSPARALLEL_HPP
#define BUSPARALLEL_HPP

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

enum ParallelType {
    PARALLEL8  = 8,  //Porta paralela de 8 bits
    PARALLEL16 = 16  //Porta paralela de 16 bits
};

class Bus_Parallel : public IBus {
  public:
    Bus_Parallel() = default;
    void Config(const IBusConfig_t* cfg)  override;
  protected:
    ParallelBusConfig_t _cfg;                         //Config local específica de Parallel
    bool _parallel_init = false;                      //inicializou o Paralelo 8080/6800
    ParallelType _parallel_type;
	int _parallel_pindir = OUTPUT;                    //Direção INPUT/OUTPUT de configuracao dos pinos
	
	inline void PulseEN();
	void SetCS(uint8_t level_cs);
    void SetRS(uint8_t level_rs);
	void SetDataPinsDirection(int direction);
	
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

#endif //fim do include guard BUSPARALLEL_HPP