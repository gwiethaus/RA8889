#ifndef BUSPARALLEL_HPP
#define BUSPARALLEL_HPP

#include <Arduino.h>
#include <Bus.hpp>
#include <Config.hpp>
#include <Registers.hpp>

class Bus_Parallel : public IBus {
  public:
    Bus_Parallel() = default;
    void Config(const IBusConfig_t* cfg)  override;
  protected:
    ParallelBusConfig_t _cfg;                         //Config local específica de Parallel
    bool _parallel_init = false;                      //inicializou o Paralelo 8080/6800
    ParallelType _parallel_type;
    int _parallel_pindir = OUTPUT;                    //Direção INPUT/OUTPUT de configuracao dos pinos
    bool _parallel_startwrite = false;                //inicia o bloco de escrita (evita overhead de bulk de dados)
    bool _lock_bus = false;                           //permite uso por dentro do SetCS()
	uint8_t _channel_high = 0;                        //seleciona canal de bits altos [15..8] e bits baixos [7..0]
	
    inline void PulseEN();
    void SetCS(uint8_t level_cs);
    void SetRS(uint8_t level_rs);
    void SetDataPinsDirection(int direction);
	  void RwHighByte(void);

    uint32_t Init() override;
    uint8_t StartWrite(void) override;
    void EndWrite(void) override;
    void LockBus(bool force_unlock = false) override;
    void UnlockBus(void) override;	
    uint8_t RwByte(uint8_t value) override;
    uint32_t RwBytes(const uint8_t* data, uint32_t len) override;
    void CmdWrite(uint8_t cmd) override;
    void DataWrite(uint8_t data) override;
    void  DataWrite(uint32_t data, uint8_t step) override;
    void WriteBytes(const uint8_t* data, size_t len) override;
    uint8_t DataRead(void) override;
    uint32_t DataRead(uint8_t step) override;
    uint32_t DataRead(uint8_t address, uint8_t step) override;
    uint8_t StatusRead(void) override;
    void RegisterWrite(uint8_t reg, uint8_t data) override;
    uint8_t RegisterRead(uint8_t reg) override;
};

#endif //fim do include guard BUSPARALLEL_HPP