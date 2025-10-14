#include <stdint.h>

class DisplayBase; // classe base para todos os displays

class IBus {
  public:
    struct config_t {
        uint8_t spi_type;
        uint8_t spi_host;
        uint8_t pin_mosi;
        uint8_t pin_miso;
        uint8_t pin_sclk;
        uint8_t pin_cs;          // Chip Select (SPI padrão)
        uint32_t freq_write;
        uint32_t freq_read;
    };

    virtual void Config(const config_t& cfg) = 0; // público: usuário pode configurar
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
