#ifndef nRFTP_BYTEBUFFER
#define nRFTP_BYTEBUFFER

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <stdint.h>
#endif

namespace nRFTP {

  class ByteBuffer{
    public:
      ByteBuffer(uint8_t* _data);
      void putByte(const uint8_t& val);
      void putWord(const uint16_t& val);
      void putUint32_t(const uint32_t& val);
      void putFloat(const float& val);

      void reset();

      uint8_t getByte(void);
      uint16_t getWord(void);
      uint32_t getUint32_t(void);
      float getFloat(void);

      uint8_t* data;

    private:
      uint16_t pos;
  };

}

#endif
