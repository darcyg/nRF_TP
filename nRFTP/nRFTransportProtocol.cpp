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
    	  if(routing.isElement(destAddress)) {
    		  return physicalLayer->write((const void*)bb.data, Message::SIZE, Util::TPAddress_to_nRF24L01Address(routing.getNextHopAddress(destAddress)));
    	  }
    	  else {
    		  if(destAddress == BROADCAST_ADDRESS) {
    			 return physicalLayer->write((const void*)bb.data, Message::SIZE, Util::TPAddress_to_nRF24L01Address(BROADCAST_ADDRESS));
    		  } else {
				  RouteMessage routeMessage(bb);
				  routeMessage.header.srcAddress = SELF_ADDRESS;
				  routeMessage.header.destAddress = destAddress;
				  routeMessage.header.setFlag(routeMessage.header.FLAG_IS_RESPONSE, 0);
				  routeMessage.fromAddress = SELF_ADDRESS;

				  bb.reset();
				  delay(20);
#if(DEBUG_TL)
	Serial.println("New destAddress. Route request sent!");
	routeMessage.header.printHeader();
#endif
				  return physicalLayer->write((const void*)bb.data, Message::SIZE, Util::TPAddress_to_nRF24L01Address(BROADCAST_ADDRESS));
    		  }
    	  }
      }


      bool nRFTransportProtocol::available(void){
        return physicalLayer->available();
      }

      bool nRFTransportProtocol::read( ByteBuffer& buf ){
        return physicalLayer->read((void*)buf.data, Message::SIZE);
      }

      void nRFTransportProtocol::run(void){
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
                    messageHandler->pingResponseArrived((uint16_t)(millis() - waitingForPingResponse), currentlyPingingAddress );

                    waitingForPingResponse = 0;
                    currentlyPingingAddress = 0;
                    doPing = true; // TODO ping automatikus teszthez kell, torolheto ha mar nem kell
            	  }

				  if (!isResponse){
					PingMessage pingMessage(bb);
					uint16_t tmp = pingMessage.header.srcAddress;
					pingMessage.header.srcAddress = pingMessage.header.destAddress;
					pingMessage.header.destAddress = tmp;

					pingMessage.header.setFlag(Header::FLAG_IS_RESPONSE, true);

					// bytebuffer reset a copy elõtt aztán küldés
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
                	if(!isResponse){ 		//Request
                		if(routeMessage.header.destAddress == SELF_ADDRESS){		//Mi voltunk a címzettek, response üzenetet küldünk visszafelé.
                			if(!routing.isElement(routeMessage.header.srcAddress)){
                				routing.newElement(routeMessage.header.srcAddress, routeMessage.fromAddress, 0, 0, 0, 0);
                			}
                			uint16_t tmp = routeMessage.header.srcAddress;
                			routeMessage.header.srcAddress = routeMessage.header.destAddress;
                			routeMessage.header.destAddress = tmp;
                			routeMessage.header.setFlag(Header::FLAG_IS_RESPONSE, true);
                			routeMessage.fromAddress = SELF_ADDRESS;

                			bb.reset();
                			routeMessage.copyToByteBuffer(bb);
                			delay(20);
                			sendMessage(bb, routeMessage.header.destAddress);
#if(DEBUG_TL)
	Serial.println("Request arrived. Route response sent!");
	routeMessage.header.printHeader();
#endif
                		}
                		else{													//Request, broadcast csatornán továbbküldjük.
                			if(!routing.isElement(routeMessage.header.srcAddress)){
                			    routing.newElement(routeMessage.header.srcAddress, routeMessage.fromAddress, 0, 0, 0, 0);
                			}
                			routeMessage.fromAddress = SELF_ADDRESS;
                			bb.reset();
                			routeMessage.copyToByteBuffer(bb);
                			delay(20);
                			sendMessage(bb, BROADCAST_ADDRESS);
#if(DEBUG_TL)
	Serial.println("Route request sent broadcast!");
	routeMessage.header.printHeader();
#endif
                		}

                	}
                	else {		//Response
                		if(routeMessage.header.destAddress == SELF_ADDRESS){		//Response jött, mi voltunk a cél, felépült az útvonal.
                			if(!routing.isElement(routeMessage.header.srcAddress)){
                			    routing.newElement(routeMessage.header.srcAddress, routeMessage.fromAddress, 0, 0, 0, 0);
                			}
#if(DEBUG_TL)
	Serial.println("Route complete!");
#endif
                		}
                		else {														//Response, tovább küldjük a cél felé.
                			if(!routing.isElement(routeMessage.header.srcAddress)){
                			    routing.newElement(routeMessage.header.srcAddress, routeMessage.fromAddress, 0, 0, 0, 0);
                			}
                			routeMessage.fromAddress = SELF_ADDRESS;
                			bb.reset();
                			routeMessage.copyToByteBuffer(bb);
                			delay(20);
                			sendMessage(bb, routeMessage.header.destAddress);
#if(DEBUG_TL)
	Serial.println("Route response sent!");
	routeMessage.header.printHeader();
#endif
                		}
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

