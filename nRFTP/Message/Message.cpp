#include "Message.h"

namespace nRFTP {

  Message::Message( ByteBuffer& src )
  : header(src){
  }

  Message::Message(uint16_t srcAddress, uint16_t destAddress, uint8_t _type){
    header.srcAddress = srcAddress;
    header.destAddress = destAddress;
    header.messageId = (uint16_t)(millis());
    header.setType(_type);
  }

  uint8_t Message::getTypeFromReadBuffer(uint8_t* buffer){
    Header::getTypeFromFirstByte(buffer[0]);
  }

  void  Message::copyToByteBuffer(ByteBuffer& dest){
    header.copyToByteBuffer(dest);
  }

}
