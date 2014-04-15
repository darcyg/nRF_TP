#ifndef nRFTP_PINGMESSAGE
#define nRFTP_PINGMESSAGE

#include "Message/Message.h"

namespace nRFTP{

    class PingMessage : public Message
    {
        public:
            static const uint16_t TIMEOUT_VAL = 9999;
            static const uint16_t MAX_WAIT_TIME = 2000;

            PingMessage(ByteBuffer& src);
            PingMessage(uint16_t _srcAddress, uint16_t _destAddress);
            void copyToByteBuffer(ByteBuffer& dest);

        protected:
            uint32_t sendTime;
    };

}

#endif // nRFTP_PINGMESSAGE
