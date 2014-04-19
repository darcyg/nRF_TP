#ifndef nRFTP_UTIL
#define nRFTP_UTIL

#include <Arduino.h>

namespace nRFTP {
  
  class Util{
    public:
      static uint64_t TPAddress_to_nRF24L01Address(uint16_t address);
  };
  
}

#endif
