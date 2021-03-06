#include "Message/RouteMessage.h"
#include "Util/ByteBuffer.h"

namespace nRFTP{

    RouteMessage::RouteMessage(ByteBuffer& src)
    : Message(src){
      fromAddress = src.getWord();
      targetAddress = src.getWord();
    }

    RouteMessage::RouteMessage(uint16_t _srcAddress, uint16_t _destAddress)
    : Message(_srcAddress,_destAddress,Message::TYPE_ROUTE), fromAddress(_srcAddress){
    }

    void RouteMessage::copyToByteBuffer(ByteBuffer& dest){
      Message::copyToByteBuffer(dest);
      dest.putWord(fromAddress);
      dest.putWord(targetAddress);
    }

}
