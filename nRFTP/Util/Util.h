#ifndef nRFTP_UTIL
#define nRFTP_UTIL

#ifdef ARDUINO
	#include <Arduino.h>
#else
#include <stdint.h>
#endif

namespace nRFTP {
  
  class Util{
    public:
      static uint64_t TPAddress_to_nRF24L01Address(uint16_t address);
  };
  
}

#endif
