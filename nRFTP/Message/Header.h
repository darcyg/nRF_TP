#ifndef nRFTP_HEADER
#define nRFTP_HEADER

#ifdef ARDUINO
	#include <Arduino.h>
	#include <HardwareSerial.h>
#else
#include <stdint.h>
#endif

#include "Util/ByteBuffer.h"


#define TYPE_BITS 4

#define DEBUG_HEADER 1

namespace nRFTP {

  class Header{
    public:
      static const uint8_t FLAG_FRAGMENT = 1;
      static const uint8_t FLAG_IS_RESPONSE = 2;

      uint8_t flagsAndType;
      uint16_t srcAddress;
      uint16_t destAddress;
      uint16_t messageId;
      uint8_t fragmentOffset;
      uint32_t crc;

      Header();
      Header(ByteBuffer& src);
      void copyToByteBuffer(ByteBuffer& dest);

      void setType(uint8_t type);
      void setFlag(uint8_t flagNum, bool value);
      bool getFlag(uint8_t flagNum);
      uint8_t getType();

      static uint8_t getTypeFromFirstByte( uint8_t firstByteOfReadBuffer);
      static bool isResponseFromFirstByte( uint8_t firstByteOfReadBuffer);

#if DEBUG_HEADER == 1
     void printHeader();
#endif

    private:

      static const uint8_t typeMask = 0xf;
      static const uint8_t flagsMask = 0xf0;
  };
}

#endif
