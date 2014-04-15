#include "Message/RouteRequestMessage.h"

namespace nRFTP{

    RouteRequestMessage::RouteRequestMessage(ByteBuffer& src)
    : Message(src){
      fromAddress = src.getWord();
    }

    RouteRequestMessage::RouteRequestMessage(uint16_t _srcAddress, uint16_t _destAddress)
    : Message(_srcAddress,_destAddress,Message::TYPE_ROUTE), fromAddress(_srcAddress){
    }

    void RouteRequestMessage::copyToByteBuffer(ByteBuffer& dest){
      Message::copyToByteBuffer(dest);
      dest.putWord(fromAddress);
    }

}
