#ifndef nRFTP_IMESSAGEHANDLER
#define nRFTP_IMESSAGEHANDLER

namespace nRFTP {

	class ByteBuffer;

    class IMessageHandler
    {
        public:
            virtual void handleMessage(nRFTP::ByteBuffer& bb, uint8_t type, bool isResponse) = 0;
            virtual void pingResponseArrived(uint16_t milis, uint16_t destAddress) = 0;
            virtual ~IMessageHandler(){}
    };

}
#endif // nRFTP_IMESSAGEHANDLER
