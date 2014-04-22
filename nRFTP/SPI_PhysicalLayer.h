#ifndef nRFTP_SPI_PHYSICALLAYER
#define nRFTP_SPI_PHYSICALLAYER

#ifdef ARDUINO
	#include <Arduino.h>
#else
#include <stdint.h>
#endif
#include "IPhysicalLayer.h"
#include "Message/SensorData.h"

namespace nRFTP{

  class SPI_PhysicalLayer : public IPhysicalLayer {
    public:
	  SPI_PhysicalLayer(uint8_t _cepin, uint8_t _cspin);

      bool write( const void* buf, uint8_t len, uint64_t destAddress);
      bool available(void);
      bool read( void* buf, uint8_t len );
      void begin(void);

      int test_readed_messages;
      int test_available_counter;
      SensorData test_data[15];
      uint8_t test_buffer[Message::SIZE];

    protected:

  };

}

#endif /* nRFTP_SPI_PHYSICALLAYER */
