#ifndef REGISTERS_HPP
#define REGISTERS_HPP

#include <Config.hpp>

#if defined(USE_RA8875)
  #include <RA8875_Reg.hpp>
#elif defined(USE_RA8876)
  #include <RA8876_Reg.hpp>
#elif defined(USE_RA8877)
  #include <RA8877_Reg.hpp>
#elif defined(USE_RA8889)
  #include <RA8889_Reg.hpp>
#endif

#endif