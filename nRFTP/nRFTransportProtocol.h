#ifndef nRFTP_CLASS
#define nRFTP_CLASS

#include "Message/Message.h"
#include "Routing/RoutingTable.h"

#define DEBUG_NRFTP 1

namespace nRFTP {

  class IPhysicalLayer;
  class IMessageHandler;

  class nRFTransportProtocol{
    public:
	  static const uint16_t broadcastAddress = 0xffff;

	  nRFTransportProtocol(IPhysicalLayer* _physicalLayer, uint16_t _address);
      void begin(IMessageHandler* _messageHandler);

      void requestRouteTo(uint16_t destAddress);
      void ping(uint16_t destAddress);
      void discoverNeighbourhood();
      void sendRoutingTable(uint16_t destAddress);
      void sendNeighbourhoodList(uint16_t destAddress);
      bool sendMessage(ByteBuffer& bb, uint16_t destAddress);

      bool available(void);
      bool read( ByteBuffer& buf );

      void run(void);

      static bool doPing;

#ifndef ARDUINO
      static uint64_t startTime;
#endif

    protected:
      uint16_t address;
      IPhysicalLayer* physicalLayer;
      IMessageHandler* messageHandler;

      uint16_t activity_counter;

      uint8_t readBuffer[Message::SIZE];
      uint16_t readedType;
      bool readedIsResponse;

      nRFTP::RoutingTable routing;

      unsigned long waitingForPingResponse;
      uint16_t currentlyPingingAddress;

      void checkForPingTimeOut();
      void handleMessage(nRFTP::ByteBuffer& bb, uint8_t type, bool isResponse);

  };

}
#endif
