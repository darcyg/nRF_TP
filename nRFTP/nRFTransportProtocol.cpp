#include <nRFTransportProtocol.h>

#define DEBUG_TL 1

namespace nRFTP {

      bool nRFTransportProtocol::doPing;

	  nRFTransportProtocol::nRFTransportProtocol(IPhysicalLayer* _physicalLayer, uint16_t _address)
      : physicalLayer(_physicalLayer),
        address(_address){
      }

      void nRFTransportProtocol::begin(IMessageHandler* _messageHandler){
		physicalLayer->begin();
		messageHandler = _messageHandler;
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
#if(DEBUG_TL)
	Serial.println("Ping request sent!");
	pingMessage.header.printHeader();
#endif
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
        // a waitingForPingResponse ha nem nulla, akkor az az id� van benne, mikor az utols� ping �zenet k�ldve lett, ha nulla, akkor nem v�runk v�laszra
        if (waitingForPingResponse > 0){
        	checkForPingTimeOut();
        }

        if (available()){
          ByteBuffer bb(readBuffer);
          read(bb);
          readedType = Message::getTypeFromReadBuffer(readBuffer);
          readedIsResponse = Message::isResponseFromReadBuffer(readBuffer);
          handleMessage(bb,readedType, readedIsResponse);
        }
      }

      void nRFTransportProtocol::handleMessage(nRFTP::ByteBuffer& bb, uint8_t type, bool isResponse){
    	    bool forwardToApp = true;
            switch (type){
            	case Message::TYPE_PING:
            	  if (waitingForPingResponse != 0 && isResponse){
                    // v�laszra v�runk �s response, �rtes�ti az appot a messageHandleren kereszt�l, de nem az �ltal�nos handleMessageve, hanem a pingResponseArrived-del
                    messageHandler->pingResponseArrived((uint16_t)(millis() - waitingForPingResponse), currentlyPingingAddress );

                    waitingForPingResponse = 0;
                    currentlyPingingAddress = 0;
                    doPing = true; // TODO ping automatikus teszthez kell, torolheto ha mar nem kell
            	  }

				  if (!isResponse){
					// Objektum l�trehoz�sa, c�m cser�je
					PingMessage pingMessage(bb);
					uint16_t tmp = pingMessage.header.srcAddress;
					pingMessage.header.srcAddress = pingMessage.header.destAddress;
					pingMessage.header.destAddress = tmp;

					pingMessage.header.setFlag(Header::FLAG_IS_RESPONSE, true);

					// bytebuffer reset a copy el�tt, azt�n k�ld�s
					bb.reset();
					pingMessage.copyToByteBuffer(bb);
#if(DEBUG_TL)
	Serial.println("Ping response sent!");
	pingMessage.header.printHeader();
#endif
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
              messageHandler->handleMessage(bb, type,isResponse);
            }
      }

      void nRFTransportProtocol::checkForPingTimeOut(){
          if (millis() - waitingForPingResponse > PingMessage::MAX_WAIT_TIME){
            messageHandler->pingResponseArrived(PingMessage::TIMEOUT_VAL, currentlyPingingAddress );
            waitingForPingResponse = 0;
            currentlyPingingAddress = 0;
            doPing = true; // TODO ping automatikus teszthez kell, torolheto ha mar nem kell
          }
      }
}

