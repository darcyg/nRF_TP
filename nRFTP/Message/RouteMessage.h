#ifndef nRFTP_ROUTEREQUESTMESSAGE
#define nRFTP_ROUTEREQUESTMESSAGE

#include "Message/Message.h"

namespace nRFTP{

	class ByteBuffer;

    class RouteMessage : public Message
    {
        public:
            RouteMessage(ByteBuffer& src);
            RouteMessage(uint16_t _srcAddress, uint16_t _destAddress);
            void copyToByteBuffer(ByteBuffer& dest);
            uint16_t fromAddress;

        protected:


    };

}

#endif // nRFTP_ROUTEMESSAGE
