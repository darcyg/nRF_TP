#ifndef nRFTP_NRF24L01_PHYSICALLAYER
#define nRFTP_NRF24L01_PHYSICALLAYER

#ifdef ARDUINO

#include <Arduino.h>
#include "IPhysicalLayer.h"
#include <RF24.h>

namespace nRFTP{

  class nRF24L01_PhysicalLayer : public IPhysicalLayer {
    public:
      nRF24L01_PhysicalLayer(uint64_t _selfAddress, uint8_t _cepin, uint8_t _cspin);

      bool write( const void* buf, uint8_t len, uint16_t destAddress);
      bool available(void);
      bool read( void* buf, uint8_t len );
      void begin(void);

    protected:
      RF24 radio;
      uint64_t selfAddress;
  };

}

#endif
#endif
