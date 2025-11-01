#ifndef BUS_HPP
#define BUS_HPP

#include <Arduino.h>

class DisplayBase; // classe base para todos os displays

class IBus {
  public:

    //Structs de configuração por barramento
	
    struct IBusConfig_t {
        uint32_t freq_write = 0;
        uint32_t freq_read  = 0;
    };

    struct SPIBusConfig_t : public IBusConfig_t {
        uint8_t spi_type   = 0;
        uint8_t spi_host   = 0;
        uint8_t pin_mosi   = 0;
        uint8_t pin_miso   = 0;
        uint8_t pin_sclk   = 0;
        uint8_t pin_cs     = 0;        // Chip Select (SPI padrão)
    };

    struct I2CBusConfig_t : public IBusConfig_t {
        uint8_t pin_sda = 0;
        uint8_t pin_scl = 0;
        uint32_t freq   = 400000;
    };

    struct ParallelBusConfig_t : public IBusConfig_t {
        uint8_t parallel_type = 0;            //type 8/16 parallel bus
		uint8_t data_pins[16]{};              //data bus 8 pin or 16 pin
        uint8_t pin_wr       = 0;             //write signal
		uint8_t pin_rd       = 0;             //read signal
        uint8_t pin_rs       = 0;             //0=command, 1=data signal
        uint8_t pin_cs       = 0;             //bus chip select
		uint8_t pin_en       = 0;             //enable (/RD or/WR)
		uint8_t pin_wait     = 0;             //sait signal
        uint8_t pin_int     = 0xff;           //se nao utilziar nao requer preenchimento
    };
    
    virtual void Config(const IBusConfig_t* cfg) = 0; // público: usuário pode configurar

    //Nota:
	//Não utilizar diretamente estes métodos externamente. Apenas nas classees 
	//Descendentes de DisplayBase. Na atual arquitetura, não é possível fazer 
	//da classe Base DisplayBase como uma classe amiga e fazer com que os 
	//metodos protegidos consigam ter visibilidade em todas as classes que 
	//herdam de DisplayBase
    virtual void Init() = 0;
    virtual void CmdWrite(uint8_t cmd) = 0;
    virtual void DataWrite(uint8_t data) = 0;
    virtual void DataWrite8(uint8_t data) = 0;
    virtual void DataWrite16(uint16_t data) = 0;
    virtual void DataWrite24(uint32_t data) = 0;
    virtual uint8_t DataRead(void) = 0;
    virtual uint16_t DataRead16(uint8_t address) = 0;
    virtual uint8_t StatusRead(void) = 0;
    virtual void RegisterWrite(uint8_t reg, uint8_t data) = 0;
    virtual uint8_t RegisterRead(uint8_t reg) = 0;  
    virtual uint8_t StartWrite(void) = 0;
    virtual void EndWrite(void) = 0;
    virtual ~IBus() {}
  protected:
    friend class DisplayBase;  //Na classe DisplayBase podem usar os metodos protegidos desta classe.
};

#endif //fim do include guard BUS_HPP