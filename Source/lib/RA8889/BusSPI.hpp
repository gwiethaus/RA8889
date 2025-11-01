#ifndef BUSSPI_HPP
#define BUSSPI_HPP

#include <Arduino.h>
#include <SPI.h>
#include <Bus.hpp>
#include <Config.hpp>
#include <Registers.hpp>


enum SPIHostType {
    HOST_AUTO = 0,   // Deixa a biblioteca decidir automaticamente o SPI (fallback seguro)
    HOST_SPI,        // SPI padrão (Arduino Uno, Mega, Due, STM32, etc.)
    HOST_HSPI,       // High-Speed SPI secundário (ESP32 e ESP32-S3)
    HOST_VSPI,       // SPI terciário (apenas ESP32 clássico)
    HOST_FSPI        // Flash SPI rápido (ESP32-S2 / S3 / C3)
};


//Quais SPI existem nas diversas plataformas
#if CONFIG_IDF_TARGET_ESP32
  #define HAS_HSPI 1
  #define HAS_VSPI 1
  #define HAS_FSPI 0
  #define HAS_SPI  0                           //SPI padrão não é usado nesse caso
  #define SPI_CLOCK_SPEED_MAX  20000000        //Para ESP32 Genérico
#elif CONFIG_IDF_TARGET_ESP32S3
  #define HAS_HSPI 1
  #define HAS_VSPI 0
  #define HAS_FSPI 1
  #define HAS_SPI  0
  #define SPI_CLOCK_SPEED_MAX  20000000        //Para ESP32-S2, ESP32-S3, ESP32-C3
#elif CONFIG_IDF_TARGET_ESP32S2
  #define HAS_HSPI 0
  #define HAS_VSPI 0
  #define HAS_FSPI 1
  #define HAS_SPI  0
  #define SPI_CLOCK_SPEED_MAX  20000000        //Para ESP32-S2, ESP32-S3, ESP32-C3
#elif CONFIG_IDF_TARGET_ESP32C3
  #define HAS_HSPI 0
  #define HAS_VSPI 0
  #define HAS_FSPI 1
  #define HAS_SPI  0
  #define SPI_CLOCK_SPEED_MAX  20000000        //Para ESP32-S2, ESP32-S3, ESP32-C3
#elif defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_MEGA2560)
  //Placas genéricas sem múltiplos SPI (Arduino Uno, Mega, Duo, etc.)
  #define HAS_HSPI 0
  #define HAS_VSPI 0
  #define HAS_FSPI 0
  #define HAS_SPI  1
  #define SPI_CLOCK_SPEED_MAX  8000000
#else
  //Placas genéricas não implementadas
  #warning "SPI não implementada para esta plataforma"
#endif


class Bus_SPI : public IBus {
  public:
    Bus_SPI() = default;                         //gera um construtor padrão para a classe
    void Config(const IBusConfig_t* cfg)  override;
  protected:
    //SPIClass spi;                                //Manter por motivo histórico: Será ajustado no Init (isso causa crach quando tenta definir objeto ja existente)
    SPIClass* spi = nullptr;                     //Ponteiro para spi inicializado
    SPISettings _spisetting;
    SPIBusConfig_t _cfg;                         //Config local específica de SPI
    bool _spi_init = false;                      //inicializou o SPI
    uint32_t _spi_clockmax;                      //velocidade de comunucacao de clock maximo do SPI
    uint8_t _spi_datamode;                       //SPI_MODE0[1,2,3] de comunicacao
    uint8_t _spi_dataorder;                      //ordem de dados spi MSBFIRST ou LSBFIRST
    bool _spi_transaction;                       //A trasação rpincipal do barramento spi está ativo
    bool _spi_startwrite = false;                //inicia o bloco de escrita (evita overhead de bulk de dados)

    void createSPI(SPIHostType hostType);
    void SetCS(uint8_t level_cs);
    uint8_t RwByte(uint8_t value);
    void CheckTransaction(void);

    void Init(void) override;
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
    uint8_t StartWrite(void);
    void EndWrite(void);
};

#endif    // fim de include guard BUSSPI_HPP