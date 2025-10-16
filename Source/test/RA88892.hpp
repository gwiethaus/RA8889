#ifndef RA8889_HPP
#define RA8889_HPP

#include <stdint.h>  // ou #include <Arduino.h>
#include <SPI.h>
#include "DisplayBase.hpp"
#include "RA8889_Reg.hpp"

class RA8889 : public DisplayBase {
public:
    RA8889(uint8_t pin_cs, uint8_t pin_rst);
};

#endif