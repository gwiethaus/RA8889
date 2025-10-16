#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <Arduino.h>

//Ativa a Depuração da porta serial
#define SERIAL_DEBUG


#ifdef SERIAL_DEBUG
  extern bool serialStarted;
  #define DEBUG_BEGIN(baud) do { Serial.begin(baud); serialStarted = true; } while(0)
  #define DEBUG_PRINT(msg, val, b, newline) SerialPrint(msg, val, b, newline)
  //Serial print para valroes hexadecimal
  #define DEBUG_PRINTH(msg, hex, b, newline) SerialPrintH(msg, hex, b, newline)
  //Serial Print com delay
  #define DEBUG_PRINTD(msg, val, b, delayms, newline) do { SerialPrint(msg, val, b, newline); if (delayms>0) delay(delayms); } while(0)
  //Serial Print com ponto flutuante
  #define DEBUG_PRINTF(msg, val, decimal, b, newline) SerialPrintF(msg, val, decimal, b, newline)
#else
   // Se não houver debug, macros não fazem nada
  #define DEBUG_BEGIN(baud)
  #define DEBUG_PRINT(msg, val, b, newline)
  #define DEBUG_PRINTH(msg, hex, b, newline)
  #define DEBUG_PRINTD(msg, val, b, delayms, newline)
  #define DEBUG_PRINTF(msg, val, decimal, b, newline)
#endif

#ifdef SERIAL_DEBUG
void SerialPrint(String msg, uint32_t value, bool b, bool newline);
void SerialPrintF(String msg, double value, uint8_t decimal, bool b, bool newline);
void SerialPrintH(String msg, const char* value, bool b, bool newline);
void SerialPrintH(String msg, uint64_t value, bool b, bool newline);
#endif

#endif