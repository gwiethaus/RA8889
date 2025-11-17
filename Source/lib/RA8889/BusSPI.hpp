#ifndef BUSSPI_HPP
#define BUSSPI_HPP

#include <Arduino.h>
#include <Bus.hpp>
#include <Config.hpp>
#include <Registers.hpp>
#include <Debug.hpp>

#if defined(SPI_ARDUINO_CORE)
  #include <SPI.h>
#elif defined(SPI_ESP32_NATIVE)
  #include "esp_system.h"
  #include "driver/spi_master.h"
  #include "esp_err.h"
  #include "driver/spi_common.h"
#endif

enum SPIHost {
    HOST_SPI  = 0,   // SPI padrão (Arduino Uno, Mega, Due, STM32, etc.)
    HOST_FSPI = 0,   // SPI1, Flash SPI rápido (ESP32-S2 / S3 / C3)
    HOST_HSPI = 1,   // SPI2, High-Speed SPI secundário (ESP32 e ESP32-S3)
    HOST_VSPI = 2,   // SPI3, SPI terciário (apenas ESP32 clássico)
    HOST_AUTO = 15   // Deixa a biblioteca decidir automaticamente o SPI (fallback seguro)
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
  #if defined(SPI_ARDUINO_CORE)
    #define SPI_CLOCK_SPEED_MAX  80000000      //Para ESP32-S2, ESP32-S3, ESP32-C3
  #elif defined(SPI_ESP32_NATIVE)
    #define SPI_CLOCK_SPEED_MAX  APB_CLK_FREQ  //Para ESP32-S2, ESP32-S3, ESP32-C3
  #endif
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
  private:
    #if defined(SPI_ARDUINO_CORE)   
    SPIClass* spi = nullptr;                     //Ponteiro para spi inicializado
    SPISettings _spisetting;
    #endif
    uint8_t *dma_buffer = nullptr;               //Memoria DMA alocada na area de 384 Kb Inerno para dados
    bool use_dma = false;                        //Não usa buffer de DMA 
    #if defined(SPI_ESP32_NATIVE)   
    static spi_device_handle_t spi_handle;
    #endif
  public:
    Bus_SPI() = default;                         //gera um construtor padrão para a classe
    void Config(const IBusConfig_t* cfg)  override;
  protected:
    SPIBusConfig_t _cfg;                         //Config local específica de SPI
    bool _spi_init = false;                      //Inicializou o SPI
    uint32_t _spi_clockmax;                      //Velocidade de comunucacao de clock maximo do SPI
    uint8_t _spi_divider_clock = 0;              //divisor do clock spi
    uint8_t _spi_datamode;                       //SPI_MODE0[1,2,3] de comunicacao
    uint8_t _spi_dataorder;                      //ordem de dados spi MSBFIRST ou LSBFIRST
    bool _spi_transaction;                       //A trasação rpincipal do barramento spi está ativo
    bool _spi_startwrite = false;                //Inicia o bloco de escrita (evita overhead de bulk de dados)
    bool _lock_bus = false;                      //Permite uso por dentro do SetCS()

    uint32_t Find_SPI_Frequence(uint32_t requested_hz, uint8_t* div_out);
    void createSPI(SPIHostType hostType);
    void SetCS(uint8_t level_cs);
    void CheckTransaction(void);
    void WaitTransfersDone(uint32_t expected_chunks);

    uint32_t Init(void) override;
    uint8_t StartWrite(void) override;
    void EndWrite(void) override;
    void LockBus(bool force_unlock = false) override;
    void UnlockBus(void) override;
    uint8_t RwByte(uint8_t value) override;
    void RwByteN(uint32_t data, uint8_t len);
    uint32_t RwBytes(const uint8_t* data, uint32_t len) override;
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

#endif    // fim de include guard BUSSPI_HPP