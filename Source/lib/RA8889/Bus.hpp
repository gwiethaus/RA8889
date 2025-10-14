#include <stdint.h>

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
        uint8_t data_pins[16]{};
        uint8_t pin_wr = 0;
        uint8_t pin_rd = 0;
        uint8_t pin_cs = 0;
    };
    virtual void Config(const IBusConfig_t* cfg) = 0; // público: usuário pode configurar

    //Nota:
	//Não utilizar diretamente estes métodos externamente. Apenas nas classees Descendentes de DisplayBase.
	//Na atual arquitetura, não é possível fazer da classe Base DisplayBase 
	//como uma classe amiga e fazer com que os metodos protegidos consgam ter visibilidade
	//de todas as classes que herdam de DisplayBase
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
    virtual ~IBus() {}
  protected:
    friend class DisplayBase;  // agora todos os displays derivados de DisplayBase podem usar os metodos protegidos
};
