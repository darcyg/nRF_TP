#ifndef nRFTP_UTIL
#define nRFTP_UTIL

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <stdint.h>
#include <boost/thread/thread.hpp>
#include <sys/time.h>
#include <nRFTransportProtocol.h>
#include <iostream>
#endif

namespace nRFTP {
  
  class Util{
    public:
      static uint64_t TPAddress_to_nRF24L01Address(uint16_t address);
#ifndef ARDUINO
      static uint64_t millisSinceEpoch();
      static uint64_t millisSinceStart();
#endif

  };
  
}

#ifdef ARDUINO
#define RFLOGLN(x)  Serial.println(x)
#define RFLOG(x)  Serial.print(x)
#define RFDELAY(x) delay(x)
#define RFMILLIS millis
#else
#define RFLOGLN(x) std::cout << (x) << std::endl
#define RFLOG(x) std::cout << (x)
#define RFDELAY(x) boost::this_thread::sleep( boost::posix_time::milliseconds( x ));
#define RFMILLIS nRFTP::Util::millisSinceStart
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#endif

#endif
