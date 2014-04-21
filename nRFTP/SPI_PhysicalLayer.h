#ifndef nRFTP_SPI_PHYSICALLAYER
#define nRFTP_SPI_PHYSICALLAYER

#ifdef ARDUINO
	#include <Arduino.h>
#else
#include <stdint.h>
#endif
#include "IPhysicalLayer.h"

namespace nRFTP{

  class SPI_PhysicalLayer : public IPhysicalLayer {
    public:
	  SPI_PhysicalLayer(uint8_t _cepin, uint8_t _cspin);

      bool write( const void* buf, uint8_t len, uint64_t destAddress);
      bool available(void);
      bool read( void* buf, uint8_t len );
      void begin(void);

    protected:

  };

}

#endif /* nRFTP_SPI_PHYSICALLAYER */
