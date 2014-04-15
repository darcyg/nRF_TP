#ifndef nRFTP_BYTEBUFFER
#define nRFTP_BYTEBUFFER

#include <arduino.h>

namespace nRFTP {

  class ByteBuffer{
    public:
      ByteBuffer(uint8_t* _data);
      void putByte(const uint8_t& val);
      void putWord(const uint16_t& val);
      void putUint32_t(const uint32_t& val);

      uint8_t getByte(void);
      uint16_t getWord(void);
      uint32_t getUint32_t(void);

      uint8_t* data;

    private:
      uint16_t pos;
  };

}

#endif
