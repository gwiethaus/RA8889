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
  #define HAS_SPI  0       // SPI padrão não é usado nesse caso
#elif CONFIG_IDF_TARGET_ESP32S3
  #define HAS_HSPI 1
  #define HAS_VSPI 0
  #define HAS_FSPI 1
  #define HAS_SPI  0
#elif CONFIG_IDF_TARGET_ESP32S2
  #define HAS_HSPI 0
  #define HAS_VSPI 0
  #define HAS_FSPI 1
  #define HAS_SPI  0  
#elif CONFIG_IDF_TARGET_ESP32C3
  #define HAS_HSPI 0
  #define HAS_VSPI 0
  #define HAS_FSPI 1
  #define HAS_SPI  0
#else
 // Placas genéricas sem múltiplos SPI (Arduino, STM32, etc.)
  #define HAS_HSPI 0
  #define HAS_VSPI 0
  #define HAS_FSPI 0
  #define HAS_SPI  1
#endif

//issso deve sair, pois ja tem a possibildiade do usuario configurar a velocidade de escrita do esp32 ou qualeru outra MCU
#if defined(CONFIG_IDF_TARGET_ESP32) 
  #define SPI_CLOCK_SPEED_MAX  20000000        //Para ESP32 Genérico
  #define HAS_SPI_TRANSACTION
#elif defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3) || defined(CONFIG_IDF_TARGET_ESP32C3)
  #define SPI_CLOCK_SPEED_MAX  20000000        //Para ESP32-S2, ESP32-S3, ESP32-C3
  #define HAS_SPI_TRANSACTION
#elif defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_MEGA2560)
  #define SPI_CLOCK_SPEED_MAX  8000000       //Para Arduino Uno/Mega
#else
   #warning "SPI não implementada para esta plataforma"
#endif


class Bus_SPI : public IBus {
  public:
    Bus_SPI() = default;                         //gera um construtor padrão para a classe
    void Config(const IBusConfig_t* cfg)  override;
  protected:
    //SPIClass spi;                                //Será ajustado no Init (isso causa crach pois estou tntado efinir um objeto ja defindio)
    SPIClass* spi = nullptr;                     // ponteiro
    SPISettings _spisetting;
    SPIBusConfig_t _cfg;                         //Config local específica de SPI
    bool _spi_init = false;                      //inicializou o SPI
    uint32_t _spi_clockmax;                      //velocidade de comunucacao de clock maximo do SPI
    uint8_t _spi_datamode;                       //SPI_MODE0[1,2,3] de comunicacao
    uint8_t _spi_dataorder;                      //ordem de dados spi MSBFIRST ou LSBFIRST
    bool _spi_transaction;                       //A trasação rpincipal do barramento spi está ativo

    void createSPI(SPIHostType hostType);
    void SetCS(uint8_t level_cs);
    uint8_t RwByte(uint8_t value);
	
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
};

#endif    // fim de include guard BUSSPI_HPP