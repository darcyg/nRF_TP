#include "nRFTransportProtocol.h"
#include "Message/PingMessage.h"
#include "Message/RouteMessage.h"
#include "Util/Util.h"
#include "Util/ByteBuffer.h"
#include "IPhysicalLayer.h"
#include "IMessageHandler.h"
#include <Message/MessageBuffer.h>


#define DEBUG_TL 1

#if DEBUG_TL == 1
#else
#define RFLOGLN(x)
#endif

namespace nRFTP {

      bool nRFTransportProtocol::doPing;

#ifndef ARDUINO
      uint64_t nRFTransportProtocol::startTime;
#endif

	  nRFTransportProtocol::nRFTransportProtocol(IPhysicalLayer* _physicalLayer, uint16_t _address)
      : address(_address),
        physicalLayer(_physicalLayer),
        messageHandler(0),
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
		  RFLOGLN("Ping request sent!");
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

			  RFLOGLN("New destAddress. Route request sent!");
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
          if (header.destAddress == address || header.destAddress == broadcastAddress || header.getType() == Message::TYPE_ROUTE){
			  handleMessage(bb,header.getType(), header.getFlag(Header::FLAG_IS_RESPONSE));
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
					  RFLOGLN("Ping response sent!");
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
                				RFLOGLN("New element in the routing table!");
                			}
                			uint16_t tmp = routeMessage.header.srcAddress;
                			routeMessage.header.srcAddress = address;
                			routeMessage.header.destAddress = tmp;
                			routeMessage.header.setFlag(Header::FLAG_IS_RESPONSE, true);
                			routeMessage.fromAddress = address;

                			bb.reset();
                			routeMessage.copyToByteBuffer(bb);
                			sendMessage(bb, routeMessage.header.destAddress);
                			RFLOGLN("Route request arrived. Route response sent!");

                		}
                		else{
                			if(!routing.isElement(routeMessage.header.srcAddress)){
                			    routing.newElement(routeMessage.header.srcAddress, routeMessage.fromAddress, 0, 0, 255, 0);
                			    RFLOGLN("New element in the routing table!");
                			}
                    		if(!messageBuffer.isElement(routeMessage.header.messageId, routeMessage.header.srcAddress)) {
                    			messageBuffer.newElement(routeMessage.header.flagsAndType, routeMessage.header.messageId, routeMessage.header.srcAddress, routeMessage.header.destAddress);

								routeMessage.fromAddress = address;
								bb.reset();
								routeMessage.copyToByteBuffer(bb);
								sendMessage(bb, broadcastAddress);
								RFLOGLN("Route request sent broadcast!");
                    		}
                		}

                	}
                	else {		//Response
                		if(routeMessage.header.destAddress == address){
                			if(!routing.isElement(routeMessage.header.srcAddress)){
                			    routing.newElement(routeMessage.header.srcAddress, routeMessage.fromAddress, 0, 0, 255, 0);
                			    RFLOGLN("New element in the routing table!");
                			}
                			RFLOGLN("Route complete!");
                		}
                		else {
                			if(!routing.isElement(routeMessage.header.srcAddress)){
                			    routing.newElement(routeMessage.header.srcAddress, routeMessage.fromAddress, 0, 0, 255, 0);
                			    RFLOGLN("New element in the routing table!");
                			}
                			routeMessage.fromAddress = address;
                			bb.reset();
                			routeMessage.copyToByteBuffer(bb);
                			sendMessage(bb, routeMessage.header.destAddress);
                			RFLOGLN("Route response sent!");
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

