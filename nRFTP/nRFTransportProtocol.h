#ifndef nRFTP_CLASS
#define nRFTP_CLASS

#include "IPhysicalLayer.h"
#include "Message/Message.h"
#include "Util/Util.h"
#include "Message/PingMessage.h"
#include <IMessageHandler.h>
#include "Message/RouteMessage.h"

#define DEBUG_NRFTP 0

namespace nRFTP {

  class nRFTransportProtocol{
    public:
	  nRFTransportProtocol(IPhysicalLayer* _physicalLayer, uint16_t _address, IMessageHandler* _messageHandler);
      void begin(void);

      void requestRouteTo(uint16_t destAddress);
      void ping(uint16_t destAddress);
      void discoverNeighbourhood();
      void sendRoutingTable(uint16_t destAddress);
      void sendNeighbourhoodList(uint16_t destAddress);
      bool sendMessage(ByteBuffer& bb, uint16_t destAddress);

      bool available(void);
      bool read( ByteBuffer& buf );

      void run(void);

    protected:
      word address;
      IPhysicalLayer* physicalLayer;
      IMessageHandler* messageHandler;

      uint8_t readBuffer[Message::SIZE];
      uint16_t readedType;

      unsigned long waitingForPingResponse;
      uint16_t currentlyPingingAddress;

  };

}

#endif
