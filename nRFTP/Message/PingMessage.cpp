#include "Message/PingMessage.h"
#include "Util/Util.h"
#include "Util/ByteBuffer.h"

namespace nRFTP{

    PingMessage::PingMessage(ByteBuffer& src)
    : Message(src){
      sendTime = src.getUint32_t();
    }

    PingMessage::PingMessage(uint16_t _srcAddress, uint16_t _destAddress)
    : Message(_srcAddress,_destAddress,Message::TYPE_PING){
    	sendTime = (uint16_t)RFMILLIS();
    }

    void PingMessage::copyToByteBuffer(ByteBuffer& dest){
      Message::copyToByteBuffer(dest);
      dest.putUint32_t(sendTime);
    }

}
