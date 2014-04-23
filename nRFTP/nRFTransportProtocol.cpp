#include "nRFTransportProtocol.h"
#include "Message/PingMessage.h"
#include "Message/RouteMessage.h"
#include "Util/Util.h"
#include "Util/ByteBuffer.h"
#include "IPhysicalLayer.h"
#include "IMessageHandler.h"
#include <Message/MessageBuffer.h>


#define DEBUG_TL 1

namespace nRFTP {

      bool nRFTransportProtocol::doPing;

#ifndef ARDUINO
      uint64_t nRFTransportProtocol::startTime;
#endif

	  nRFTransportProtocol::nRFTransportProtocol(IPhysicalLayer* _physicalLayer, uint16_t _address)
      : address(_address),
        physicalLayer(_physicalLayer),
        messageHandler(0),
        readedType(0),
        readedIsResponse(false),
        waitingForPingResponse(0),
        currentlyPingingAddress(0){
#ifndef ARDUINO
		  startTime = Util::millisSinceEpoch();
#endif
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

		  waitingForPingResponse = RFMILLIS();
		  currentlyPingingAddress = destAddress;
#if(DEBUG_TL)
	RFLOGLN("Ping request sent!");
	pingMessage.header.printHeader();
#endif
		  sendMessage(bb, destAddress);
      }


      void nRFTransportProtocol::discoverNeighbourhood(){

      }


      void nRFTransportProtocol::sendRoutingTable(uint16_t destAddress){

      }


      void nRFTransportProtocol::sendNeighbourhoodList(uint16_t destAddress){

      }


      bool nRFTransportProtocol::sendMessage(ByteBuffer& bb, uint16_t destAddress){

    	  activity_counter++;

    	  routing.printRoutingTable();

    	  if(routing.isElement(destAddress)) {
    		  routing.resetActivity(destAddress);
    		  return physicalLayer->write((const void*)bb.data, Message::SIZE, Util::TPAddress_to_nRF24L01Address(routing.getNextHopAddress(destAddress)));
    	  }
    	  else {
    		  if(destAddress == broadcastAddress) {
    			 return physicalLayer->write((const void*)bb.data, Message::SIZE, Util::TPAddress_to_nRF24L01Address(broadcastAddress));
    		  } else {
				  RouteMessage routeMessage(address, destAddress);
				  routeMessage.header.setFlag(routeMessage.header.FLAG_IS_RESPONSE, 0);
				  routeMessage.fromAddress = address;

				  bb.reset();
				  routeMessage.copyToByteBuffer(bb);

#if(DEBUG_TL)
	RFLOGLN("New destAddress. Route request sent!");
	routeMessage.header.printHeader();
#endif
				  return physicalLayer->write((const void*)bb.data, Message::SIZE, Util::TPAddress_to_nRF24L01Address(broadcastAddress));
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

    	if(activity_counter >= 2)
    	{
    		routing.decreaseActivity();
    		activity_counter = 0;
    	}

        if (waitingForPingResponse > 0){
        	checkForPingTimeOut();
        }

        if (available()){
#if(DEBUG_TL)
    RFLOGLN("New message!");
#endif
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
                    messageHandler->pingResponseArrived((uint16_t)((uint16_t)RFMILLIS() - (uint16_t)waitingForPingResponse), currentlyPingingAddress );

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


					bb.reset();
					pingMessage.copyToByteBuffer(bb);
#if(DEBUG_TL)
	RFLOGLN("Ping response sent!");
	pingMessage.header.printHeader();
#endif

					RFDELAY(20);
					sendMessage(bb, pingMessage.header.destAddress);
				  }
				break;

                case Message::TYPE_ROUTE:
                {
                	RouteMessage routeMessage(bb);
                	if(!isResponse){ 		//Request
                		if(routeMessage.header.destAddress == address){
                			if(!routing.isElement(routeMessage.header.srcAddress)){
                				routing.newElement(routeMessage.header.srcAddress, routeMessage.fromAddress, 0, 0, 255, 0);
#if(DEBUG_TL)
    RFLOGLN("New element in the table!");
#endif
                			}
                			uint16_t tmp = routeMessage.header.srcAddress;
                			routeMessage.header.srcAddress = routeMessage.header.destAddress;
                			routeMessage.header.destAddress = tmp;
                			routeMessage.header.setFlag(Header::FLAG_IS_RESPONSE, true);
                			routeMessage.fromAddress = address;

                			bb.reset();
                			routeMessage.copyToByteBuffer(bb);
                			RFDELAY(20);
                			sendMessage(bb, routeMessage.header.destAddress);
#if(DEBUG_TL)
    RFLOGLN("Request arrived. Route response sent!");
	routeMessage.header.printHeader();
#endif
                		}
                		else{
                			if(!routing.isElement(routeMessage.header.srcAddress)){
                			    routing.newElement(routeMessage.header.srcAddress, routeMessage.fromAddress, 0, 0, 255, 0);
#if(DEBUG_TL)
    RFLOGLN("New element in the table!");
#endif
                			}
                    		if(!messageBuffer.isElement(routeMessage.header.messageId, routeMessage.header.srcAddress)) {
                    			messageBuffer.newElement(routeMessage.header.flagsAndType, routeMessage.header.messageId, routeMessage.header.srcAddress, routeMessage.header.destAddress);

								routeMessage.fromAddress = address;
								bb.reset();
								routeMessage.copyToByteBuffer(bb);
								RFDELAY(20);
								sendMessage(bb, broadcastAddress);
#if(DEBUG_TL)
    RFLOGLN("Route request sent broadcast!");
	routeMessage.header.printHeader();
#endif
                    		}
                		}

                	}
                	else {		//Response
                		if(routeMessage.header.destAddress == address){
                			if(!routing.isElement(routeMessage.header.srcAddress)){
                			    routing.newElement(routeMessage.header.srcAddress, routeMessage.fromAddress, 0, 0, 255, 0);
#if(DEBUG_TL)
    RFLOGLN("New element in the table!");
#endif
                			}
#if(DEBUG_TL)
    RFLOGLN("Route complete!");
#endif
                		}
                		else {
                			if(!routing.isElement(routeMessage.header.srcAddress)){
                			    routing.newElement(routeMessage.header.srcAddress, routeMessage.fromAddress, 0, 0, 255, 0);
#if(DEBUG_TL)
    RFLOGLN("New element in the table!");
#endif
                			}
                			routeMessage.fromAddress = address;
                			bb.reset();
                			routeMessage.copyToByteBuffer(bb);
                			RFDELAY(20);
                			sendMessage(bb, routeMessage.header.destAddress);
#if(DEBUG_TL)
    RFLOGLN("Route response sent!");
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
          if (RFMILLIS() - waitingForPingResponse > PingMessage::MAX_WAIT_TIME){
            messageHandler->pingResponseArrived(PingMessage::TIMEOUT_VAL, currentlyPingingAddress );
            waitingForPingResponse = 0;
            currentlyPingingAddress = 0;
            doPing = true; // TODO ping automatikus teszthez kell, torolheto ha mar nem kell
          }
      }
}

