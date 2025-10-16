#include <Arduino.h>
#include <string.h>
#include <Debug.hpp>

#ifdef SERIAL_DEBUG
  bool serialStarted = false;


/**
 * @brief Depuracao do codigo
 *
 * @verbatim
 * None
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void SerialPrint(String msg, uint32_t value, bool b, bool newline)
{
  #ifdef SERIAL_DEBUG
  if (!serialStarted) return;  // segurança extra
  #endif
  Serial.print(msg);
  if (b) {
    newline ? Serial.println(value) : Serial.print(value);
  } else {
    if (newline) Serial.println("");
  }
}


/**
 * @brief Depuracao do codigo com valor ponto flutuante
 *
 * @verbatim
 * None
 * @endverbatim
 *
 * @param None
 *
 * @note None
 */
void SerialPrintF(String msg, double value, uint8_t decimal, bool b, bool newline)
{
  #ifdef SERIAL_DEBUG
  if (!serialStarted) return;  // segurança extra
  #endif
  Serial.print(msg);
  if (b) {
    newline ? Serial.println(value, decimal) : Serial.print(value, decimal);
  } else {
    if (newline) Serial.println("");
  }
}


void SerialPrintH(String msg, uint64_t value, bool b, bool newline)
{
  #ifdef SERIAL_DEBUG
  if (!serialStarted) return;  // segurança extra
  #endif

  Serial.print(msg);

  if (b) {
    char buffer[19]; // "0x" + 16 dígitos + '\0'
    #if defined(ARDUINO_ARCH_AVR)
      // AVR tem apenas 32 bits
      sprintf(buffer, "0x%lX", (unsigned long)value);
    #else
      // Plataformas com suporte 64 bits
      sprintf(buffer, "0x%llX", (unsigned long long)value);
    #endif

    if (newline)
      Serial.println(buffer);
    else
      Serial.print(buffer);
  }
  else {
    if (newline)
      Serial.println();
  }
}


// --- 2. Para strings (char*) ---
void SerialPrintH(String msg, const char* value, bool b, bool newline)
{
  #ifdef SERIAL_DEBUG
  if (!serialStarted) return;  // segurança extra
  #endif

  Serial.print(msg);

  if (b && value && *value) {
    uint64_t val = 0;

   // Detecta base automaticamente
    int base = (value[0] == '0' && (value[1] == 'x' || value[1] == 'X')) ? 16 : 10;

    #if defined(ARDUINO_ARCH_AVR)
      // AVR não tem strtoull(), usa 32 bits
      val = strtoul(value, nullptr, base);
    #else
      // ESP32, ARM, etc. — têm strtoull()
      val = strtoull(value, nullptr, base);
    #endif

    // Converte e imprime em formato 0xHEX
    char buffer[19];  // "0x" + 16 dígitos + '\0'
    sprintf(buffer, "0x%llX", (unsigned long long)val);

    if (newline)
      Serial.println(buffer);
    else
      Serial.print(buffer);
  }
  else {
    if (newline)
      Serial.println();
  }
}

#endif


