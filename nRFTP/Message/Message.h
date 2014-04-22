#ifndef nRFTP_MESSAGE
#define nRFTP_MESSAGE

#ifdef ARDUINO
	#include <Arduino.h>
#else
#include <stdint.h>
#endif

#include "Header.h"

namespace nRFTP {

  class Message{
    public:
      static const uint8_t TYPE_ROUTE = 0;
      static const uint8_t TYPE_PING = 1;
      static const uint8_t TYPE_SENSORDATA = 2;
      static const uint8_t TYPE_ROUTING_TABLE = 3;
      static const uint8_t TYPE_NEIGHBORHOOD_TABLE = 4;
      static const uint8_t TYPE_DISCOVERY = 5;
      static const uint8_t TYPE_COMMAND = 6;
      static const uint8_t TYPE_ERROR = 7;

      static const uint8_t SIZE = 24;

      Message(ByteBuffer& src);
      Message(uint16_t _srcAddress, uint16_t _destAddress, uint8_t _type);
      virtual ~Message(){}

      static uint8_t getTypeFromReadBuffer(uint8_t* buffer);
      static bool isResponseFromReadBuffer( uint8_t* buffer);

      virtual void copyToByteBuffer(ByteBuffer& dest);

      Header header;
  };

}

#endif
