#ifndef nRFTP_ROUTEREQUESTMESSAGE
#define nRFTP_ROUTEREQUESTMESSAGE

#include "Message/Message.h"

namespace nRFTP{

    class RouteMessage : public Message
    {
        public:
            RouteMessage(ByteBuffer& src);
            RouteMessage(uint16_t _srcAddress, uint16_t _destAddress);
            void copyToByteBuffer(ByteBuffer& dest);

        protected:
            uint16_t fromAddress;

    };

}

#endif // nRFTP_ROUTEMESSAGE
