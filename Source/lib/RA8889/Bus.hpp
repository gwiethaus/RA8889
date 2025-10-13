#include <stdint.h>

class DisplayBase; // classe base para todos os displays

class IBus {
  public:
    struct Config {
        uint8_t spi_host;
        uint8_t pin_mosi;
        uint8_t pin_miso;
        uint8_t pin_sclk;
        uint8_t pin_dc;
        uint32_t freq_write;
        uint32_t freq_read;
    };

    virtual void config(const Config& cfg) = 0; // público: usuário pode configurar
    virtual ~IBus() {}

  protected:
    virtual void init();
    virtual void writeCommand(uint8_t cmd) = 0;
    virtual void writeData(uint8_t data) = 0;
    virtual void readData(void) = 0;
 
    friend class DisplayBase;  // agora todos os displays derivados de DisplayBase podem usar os metodos protegidos
};
