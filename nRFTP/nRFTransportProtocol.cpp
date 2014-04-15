#include <nRFTransportProtocol.h>

namespace nRFTP {

	  nRFTransportProtocol::nRFTransportProtocol(IPhysicalLayer* _physicalLayer, uint16_t _address, IMessageHandler* _messageHandler)
      : physicalLayer(_physicalLayer),
        address(_address),
        messageHandler(_messageHandler){
      }

      void nRFTransportProtocol::begin(void){
        physicalLayer->begin();
      }


      void nRFTransportProtocol::requestRouteTo(uint16_t destAddress){

      }


        void nRFTransportProtocol::ping(uint16_t destAddress){
        PingMessage pingMessage(address, destAddress);
        uint8_t sendBuffer[Message::SIZE];
        ByteBuffer bb(sendBuffer);
        pingMessage.copyToByteBuffer(bb);

        waitingForPingResponse = millis();
        currentlyPingingAddress = destAddress;

        bool res = sendMessage(bb, destAddress);
      }


      void nRFTransportProtocol::discoverNeighbourhood(){

      }


      void nRFTransportProtocol::sendRoutingTable(uint16_t destAddress){

      }


      void nRFTransportProtocol::sendNeighbourhoodList(uint16_t destAddress){

      }


      bool nRFTransportProtocol::sendMessage(ByteBuffer& bb, uint16_t destAddress){
        return physicalLayer->write((const void*)bb.data, Message::SIZE, Util::TPAddress_to_nRF24L01Address(destAddress));
      }


      bool nRFTransportProtocol::available(void){
        return physicalLayer->available();
      }

      bool nRFTransportProtocol::read( ByteBuffer& buf ){
        return physicalLayer->read((void*)buf.data, Message::SIZE);
      }

      void nRFTransportProtocol::run(void){
        if (waitingForPingResponse > 0){
            if (millis() - waitingForPingResponse < PingMessage::MAX_WAIT_TIME){
              if (available()){
                ByteBuffer bb(readBuffer);
                read(bb);
                readedType = Message::getTypeFromReadBuffer(readBuffer);

                if (readedType == Message::TYPE_PING){
#if DEBUG_NRFTP == 1
                  Header h(bb);
                  h.printHeader();
#endif // DEBUG_NRFTP
                  messageHandler->pingResponseArrived((uint16_t)(millis() - waitingForPingResponse), currentlyPingingAddress );
                  waitingForPingResponse = 0;
                  currentlyPingingAddress = 0;
                } else {
                  messageHandler->handleMessage(bb, readedType);
                }
              }
            } else {
              messageHandler->pingResponseArrived(PingMessage::TIMEOUT_VAL, currentlyPingingAddress );
              waitingForPingResponse = 0;
              currentlyPingingAddress = 0;
            }
        }

        if (available()){
          ByteBuffer bb(readBuffer);
          read(bb);
          readedType = Message::getTypeFromReadBuffer(readBuffer);

#if DEBUG_NRFTP == 1
          ByteBuffer dbgBb(readBuffer);
          Header h(dbgBb);
          h.printHeader();
#endif // DEBUG_NRFTP

          bool forwardToApp = true;

          switch (readedType){
            case Message::TYPE_PING:
              if (waitingForPingResponse == 0){
                PingMessage pingMessage(bb);
                uint16_t tmp = pingMessage.header.srcAddress;
                pingMessage.header.srcAddress = pingMessage.header.destAddress;
                pingMessage.header.destAddress = tmp;
                pingMessage.copyToByteBuffer(bb);
                delay(20);
                sendMessage(bb, pingMessage.header.destAddress);
              }
            break;

            case Message::TYPE_ROUTE:
            {
            	RouteMessage routeMessage(bb);
            	if(routeMessage.header.getFlag(Header::FLAG_IS_RESPONSE)){ //if FLAG == 1 then its a response, else its a request


            	}
            }
            break;

            case Message::TYPE_SENSORDATA:
            break;

            default:
            break;
          }

          if (forwardToApp){
            messageHandler->handleMessage(bb, readedType);
          }
        }
      }
}

