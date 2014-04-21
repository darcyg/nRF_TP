#include "Message/PingMessage.h"

namespace nRFTP{

    PingMessage::PingMessage(ByteBuffer& src)
    : Message(src){
      sendTime = src.getUint32_t();
    }

    PingMessage::PingMessage(uint16_t _srcAddress, uint16_t _destAddress)
    : Message(_srcAddress,_destAddress,Message::TYPE_PING){
#ifdef ARDUINO
    	sendTime = (uint16_t)millis();
#else
    	sendTime = 0;
#endif
    }

    void PingMessage::copyToByteBuffer(ByteBuffer& dest){
      Message::copyToByteBuffer(dest);
      dest.putUint32_t(sendTime);
    }

}
