#ifndef nRFTP_IPHYSICALLAYER
#define nRFTP_IPHYSICALLAYER

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <stdint.h>
#endif

namespace nRFTP{

  class IPhysicalLayer{
    public:
      virtual bool write( const void* buf, uint8_t len, uint64_t destAddress) = 0;
      virtual bool available(void) = 0;
      virtual bool read( void* buf, uint8_t len ) = 0;
      virtual void begin(void) = 0;
  };

}
#endif
