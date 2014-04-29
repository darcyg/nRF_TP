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

    	  if(routing.isElement(destAddress)) {
    		  routing.resetActivity(destAddress);
    		  return physicalLayer->write((const void*)bb.data, Message::SIZE, routing.getNextHopAddress(destAddress));
    	  } else if(destAddress == broadcastAddress){
    		  return physicalLayer->write((const void*)bb.data, Message::SIZE, broadcastAddress);
    	  } else {
		      RouteMessage routeMessage(address, broadcastAddress);
			  routeMessage.header.setFlag(routeMessage.header.FLAG_IS_RESPONSE, 0);
			  routeMessage.fromAddress = address;
			  routeMessage.targetAddress = destAddress;

			  bb.reset();
			  routeMessage.copyToByteBuffer(bb);

			  if(!messageBuffer.isElement(routeMessage.header.messageId, routeMessage.header.srcAddress)) {
				  messageBuffer.newElement(routeMessage.header.flagsAndType, routeMessage.header.messageId, routeMessage.header.srcAddress, routeMessage.header.destAddress);
			  }

#if(DEBUG_TL)
	RFLOGLN("New destAddress. Route request sent!");
#endif
			  return physicalLayer->write((const void*)bb.data, Message::SIZE, routeMessage.header.destAddress);
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
          ByteBuffer bb(readBuffer);
          read(bb);
          Header header(bb);
		  bb.reset();
          if (header.destAddress == address || header.destAddress == broadcastAddress){
			  readedType = header.getType();
			  readedIsResponse = header.getFlag(Header::FLAG_IS_RESPONSE);
			  handleMessage(bb,readedType, readedIsResponse);
          } else {
        	  sendMessage(bb, header.destAddress);
          }
        }
      }

      void nRFTransportProtocol::handleMessage(nRFTP::ByteBuffer& bb, uint8_t type, bool isResponse){
    	    bool forwardToApp = true;
            switch (type){
            	case Message::TYPE_PING:
            	{
					PingMessage pingMessage(bb);
					  if (waitingForPingResponse != 0 && isResponse){
						messageHandler->pingResponseArrived((uint16_t)((uint16_t)RFMILLIS() - (uint16_t)waitingForPingResponse), currentlyPingingAddress );

						waitingForPingResponse = 0;
						currentlyPingingAddress = 0;
						doPing = true; // TODO ping automatikus teszthez kell, torolheto ha mar nem kell
					  }

					if (!isResponse){
					  uint16_t tmp = pingMessage.header.srcAddress;
					  pingMessage.header.srcAddress = pingMessage.header.destAddress;
					  pingMessage.header.destAddress = tmp;

					  pingMessage.header.setFlag(Header::FLAG_IS_RESPONSE, true);


					  bb.reset();
					  pingMessage.copyToByteBuffer(bb);
#if(DEBUG_TL)
	RFLOGLN("Ping response sent!");
#endif

					  RFDELAY(20);
					  sendMessage(bb, pingMessage.header.destAddress);
					}
				break;
            	}
                case Message::TYPE_ROUTE:
                {
                	RouteMessage routeMessage(bb);
                	if(!isResponse){ 		//Request
                		if(routeMessage.targetAddress == address){
                			if(!routing.isElement(routeMessage.header.srcAddress)){
                				routing.newElement(routeMessage.header.srcAddress, routeMessage.fromAddress, 0, 0, 255, 0);
#if(DEBUG_TL)
    RFLOGLN("New element in the routing table!");
#endif
                			}
                			uint16_t tmp = routeMessage.header.srcAddress;
                			routeMessage.header.srcAddress = address;
                			routeMessage.header.destAddress = tmp;
                			routeMessage.header.setFlag(Header::FLAG_IS_RESPONSE, true);
                			routeMessage.fromAddress = address;

                			bb.reset();
                			routeMessage.copyToByteBuffer(bb);
                			sendMessage(bb, routeMessage.header.destAddress);
#if(DEBUG_TL)
    RFLOGLN("Route request arrived. Route response sent!");
#endif
                		}
                		else{
                			if(!routing.isElement(routeMessage.header.srcAddress)){
                			    routing.newElement(routeMessage.header.srcAddress, routeMessage.fromAddress, 0, 0, 255, 0);
#if(DEBUG_TL)
    RFLOGLN("New element in the routing table!");
#endif
                			}
                    		if(!messageBuffer.isElement(routeMessage.header.messageId, routeMessage.header.srcAddress)) {
                    			messageBuffer.newElement(routeMessage.header.flagsAndType, routeMessage.header.messageId, routeMessage.header.srcAddress, routeMessage.header.destAddress);

								routeMessage.fromAddress = address;
								bb.reset();
								routeMessage.copyToByteBuffer(bb);
								sendMessage(bb, broadcastAddress);
#if(DEBUG_TL)
    RFLOGLN("Route request sent broadcast!");
#endif
                    		}
                		}

                	}
                	else {		//Response
                		if(routeMessage.header.destAddress == address){
                			if(!routing.isElement(routeMessage.header.srcAddress)){
                			    routing.newElement(routeMessage.header.srcAddress, routeMessage.fromAddress, 0, 0, 255, 0);
#if(DEBUG_TL)
    RFLOGLN("New element in the routing table!");
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
    RFLOGLN("New element in the routing table!");
#endif
                			}
                			routeMessage.fromAddress = address;
                			bb.reset();
                			routeMessage.copyToByteBuffer(bb);
                			sendMessage(bb, routeMessage.header.destAddress);
#if(DEBUG_TL)
    RFLOGLN("Route response sent!");
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

