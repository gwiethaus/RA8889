#ifndef CONFIG_HPP
#define CONFIG_HPP

//-----------------------------------------------------------------------------
//
// Driver RA88xx
//
//-----------------------------------------------------------------------------

//#define USE_RA8875
//#define USE_RA8876
//#define USE_RA8877
#define USE_RA8889


//-----------------------------------------------------------------------------
//
// Core de Comunicação SPI 
//
//-----------------------------------------------------------------------------

//#define SPI_ARDUINO_CORE          //SPI padrão do Arduino Core
#define SPI_ESP32_NATIVE            //SPI do ESP32 NAtivo


#endif //fim do include guard CONFIG_HPP
