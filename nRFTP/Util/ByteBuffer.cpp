#include "ByteBuffer.h"

namespace nRFTP {

  ByteBuffer::ByteBuffer(uint8_t* _data) : data(_data), pos(0){
  }

  void ByteBuffer::reset(void){
	  pos=0;
  }

  void ByteBuffer::putByte(const uint8_t& val){
    data[pos++]=val;
  }

  void ByteBuffer::putWord(const uint16_t& val){
    data[pos++] = (val >> 8) ;
    data[pos++] = val & 0xff;
  }

  void ByteBuffer::putUint32_t(const uint32_t& val){
    data[pos++] = (val >> 24) ;
    data[pos++] = (val >> 16) ;
    data[pos++] = (val >> 8) ;
    data[pos++] = val & 0xff;
  }

  uint8_t ByteBuffer::getByte(void) {
    return data[pos++];
  }

  uint16_t ByteBuffer::getWord(void) {
    uint16_t tmp = (uint16_t)data[pos++] << 8;
    return tmp | (uint16_t)data[pos++];
  }

  uint32_t ByteBuffer::getUint32_t(void) {
    uint32_t tmp = (uint32_t)data[pos++] << 24;
    tmp = tmp | (uint32_t)data[pos++] << 16;
    tmp = tmp | (uint32_t)data[pos++] << 8;
    return tmp | (uint32_t)data[pos++];
  }

}
