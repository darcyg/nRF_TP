#include "nRFTransportProtocol.h"
#include "Message/PingMessage.h"
#include "Message/RouteMessage.h"
#include "Util/Util.h"
#include "Util/ByteBuffer.h"
#include "IPhysicalLayer.h"
#include "IMessageHandler.h"
#include <Message/MessageBuffer.h>
#include "Message/RoutingTableElementMessage.h"


#define DEBUG_TL 1

#if DEBUG_TL == 1
#else
#define RFLOGLN(x)
#endif

namespace nRFTP {

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
    	  //RFLOG( "nRFTransportProtocol::ping/"); RFLOG( __LINE__); RFLOG(": begin dest: "); RFLOGLN(destAddress);
		  PingMessage pingMessage(address, destAddress);
		  uint8_t sendBuffer[Message::SIZE];
		  ByteBuffer bb(sendBuffer);
		  pingMessage.copyToByteBuffer(bb);
		  bb.reset();

		  waitingForPingResponse = RFMILLIS();
		  currentlyPingingAddress = destAddress;
		  //RFLOG( "nRFTransportProtocol::ping/"); RFLOG( __LINE__); RFLOGLN(": s7");
		  //RFLOG( "nRFTransportProtocol::ping/"); RFLOG( __LINE__); RFLOG(": end dest: "); RFLOGLN(destAddress);
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
    		  //RFLOG( "nRFTransportProtocol::sendMessage/"); RFLOG( __LINE__); RFLOGLN(": send 0");
    		  routing.resetActivity(destAddress);
    		  return physicalLayer->write((const void*)bb.data, Message::SIZE, routing.getNextHopAddress(destAddress));
    		  //RFLOG( "nRFTransportProtocol::sendMessage/"); RFLOG( __LINE__); RFLOGLN(": send 1");
    	  } else if(destAddress == broadcastAddress){
    		  return physicalLayer->write((const void*)bb.data, Message::SIZE, broadcastAddress);
    	  } else {
    		  //RFLOG( "nRFTransportProtocol::sendMessage/"); RFLOG( __LINE__); RFLOGLN(": send 2");
    		  Header header(bb);
		      RouteMessage routeMessage(address, broadcastAddress);
			  routeMessage.header.setFlag(routeMessage.header.FLAG_IS_RESPONSE, 0);
			  routeMessage.fromAddress = address;
			  routeMessage.targetAddress = destAddress;
			  routeMessage.header.crc=header.crc;

  			RFLOGLN("Route request: ");
  			RFLOG("  target: "); RFLOGLN(routeMessage.targetAddress);
  			RFLOG("  from  : "); RFLOGLN(routeMessage.fromAddress);
  			RFLOG("  dbg   : "); RFLOGLN(routeMessage.header.crc);
  			routeMessage.header.printHeader();

			  bb.reset();
			  routeMessage.copyToByteBuffer(bb);

			  if(!messageBuffer.isElement(routeMessage.header.messageId, routeMessage.header.srcAddress)) {
				  messageBuffer.newElement(routeMessage.header.flagsAndType, routeMessage.header.messageId, routeMessage.header.srcAddress, routeMessage.header.destAddress);
			  }

			  RFLOGLN("New destAddress. Route request sent!");
			  return physicalLayer->write((const void*)bb.data, Message::SIZE, routeMessage.header.destAddress);
    	  }
      }

      ByteBuffer nRFTransportProtocol::sendMessageSynchronous(ByteBuffer& bb, uint16_t destAddress) {
    	  RFLOG("sendsync begin: "); RFLOGLN(RFMILLIS());
    	uint8_t msgBuff[Message::SIZE];
    	int timeOut = 0;
      	if (routing.isElement(destAddress)) {
      		RFLOG("sendsync a: "); RFLOGLN(RFMILLIS());
      		routing.resetActivity(destAddress);
      		physicalLayer->write((const void*) bb.data, Message::SIZE, routing.getNextHopAddress(destAddress));
      	} else if (destAddress != broadcastAddress) {
      		RFLOG("sendsync b: "); RFLOGLN(RFMILLIS());
      		for (int i = 0; i < Message::SIZE; i++) {
      			msgBuff[i] = bb.data[i];
      		}

      		RouteMessage routeMessage(address, broadcastAddress);
      		routeMessage.header.setFlag(routeMessage.header.FLAG_IS_RESPONSE, 0);
      		routeMessage.fromAddress = address;
      		routeMessage.targetAddress = destAddress;
      		routeMessage.copyToByteBuffer(bb);

      		physicalLayer->write((const void*) bb.data, Message::SIZE, routeMessage.header.destAddress);
      		bool routeDone = false;
      		while (!routeDone) {
      			if (timeOut > 1000){
      				return ByteBuffer(NULL);
      			}
      			RFDELAY(5);
      			if (available()) {
      				ByteBuffer readbb(readBuffer);
      				read(readbb);
      				RouteMessage responseRouteMessage(readbb);
      				if (responseRouteMessage.header.destAddress == address
      						&& responseRouteMessage.header.srcAddress == routeMessage.targetAddress
      						&& responseRouteMessage.header.getType() == Message::TYPE_ROUTE) {
      					routing.newElement(responseRouteMessage.header.srcAddress, responseRouteMessage.fromAddress, 0, 0, 255, 0);
      					routeDone = true;
      				}
      			}
      			timeOut+=5;
      		}
      		physicalLayer->write((const void*)msgBuff, Message::SIZE, routing.getNextHopAddress(destAddress));
      		RFLOG("sendsync c: "); RFLOGLN(RFMILLIS());
      	}

      	RFLOG("sendsync d: "); RFLOGLN(RFMILLIS());
      	timeOut = 0;
      	bb.reset();
      	Header header(bb);
      	bool responseArrived = false;
      	while (!responseArrived){
  			if (timeOut > 1000){
  				return ByteBuffer(NULL);
  			}
      		RFDELAY(5);
      		if (available()) {
      			ByteBuffer readbb(readBuffer);
      			read(readbb);
      			Header responseHeader(readbb);
      			if (responseHeader.destAddress == address
      					&& responseHeader.srcAddress == header.destAddress
      					&& responseHeader.getType() == header.getType()) {
      				responseArrived = true;
      			}
      		}
      		timeOut+=5;
      	}

      	RFLOG("sendsync end: "); RFLOGLN(RFMILLIS());
      	return ByteBuffer(readBuffer);
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
				   //RFLOG( "nRFTransportProtocol::run/"); RFLOG( __LINE__); RFLOGLN(": s1");
				  sendMessage(bb, header.destAddress);
			   }

        }
        	/*
        	ByteBuffer bb(readBuffer);
        	read(bb);
        	Header header(bb);
        	bb.reset();
        	if(address == UNDEFINED_ADDRESS && header.destAddress == broadcastAddress // do not have an address yet, waiting for a broadcast message
        			&& header.getFlag(Header::FLAG_IS_RESPONSE) // which is a reply
        			&& header.getType() == Message::TYPE_ADDRESS){ // to our dynamic address request
        		AddressMessage addressMessage(bb);	// then the content of the message received, will be the new address
        		address = addressMessage.address; // the reply should contain the new address
        	} else if (address == GATEWAY_ADDRESS  // if this node is the gateway
        			&& header.getFlag(Header::FLAG_IS_RESPONSE) == false // and we received a request
        			&& header.destAddress == GATEWAY_ADDRESS // and the message is sent to the gateway
        			&& header.getType() == Message::TYPE_ADDRESS){ // and the type is address
        		//TODO: the new generated address should be put into the payload of the message
        	} else if (address != 0 && // only if we have a valid address
        			(header.destAddress == address
        					|| header.destAddress == broadcastAddress
        					|| header.getType() == Message::TYPE_ROUTE)){
        		handleMessage(bb,header.getType(), header.getFlag(Header::FLAG_IS_RESPONSE));
        	} else if (address != 0){ // only if we have a valid address
        		sendMessage(bb, header.destAddress);
        	}
        }*/
      }

      // TODO ttl kezel�s

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

					  }

					if (!isResponse){
					  uint16_t tmp = pingMessage.header.srcAddress;
					  pingMessage.header.srcAddress = pingMessage.header.destAddress;
					  pingMessage.header.destAddress = tmp;

					  pingMessage.header.setFlag(Header::FLAG_IS_RESPONSE, true);
					  pingMessage.header.crc=2;


					  bb.reset();
					  pingMessage.copyToByteBuffer(bb);
					  RFLOGLN("Ping response sent!");
					  RFDELAY(6);
					  //RFLOG( "nRFTransportProtocol::handleMessage/"); RFLOG( __LINE__); RFLOGLN(": s2");
					  sendMessage(bb, pingMessage.header.destAddress);
					}
				break;
            	}
                case Message::TYPE_ROUTE:
                {
                	RouteMessage routeMessage(bb);
                	if(!isResponse){ 		//Request
                		if(routeMessage.targetAddress == address || routing.isElement(routeMessage.targetAddress)){
                			RFLOGLN("Route request: ");
                			RFLOG("  target: "); RFLOGLN(routeMessage.targetAddress);
                			RFLOG("  from  : "); RFLOGLN(routeMessage.fromAddress);
                			RFLOG("  dbg   : "); RFLOGLN(routeMessage.header.crc);
                			routeMessage.header.printHeader();
                			if(!routing.isElement(routeMessage.header.srcAddress)){
                				routing.newElement(routeMessage.header.srcAddress, routeMessage.fromAddress, 0, 0, 255, 0);
                				RFLOGLN("New element in the routing table!");
                			}

                			uint16_t tmp = routeMessage.header.srcAddress;
                			routeMessage.header.srcAddress = address;
                			routeMessage.header.destAddress = tmp;
                			routeMessage.header.setFlag(Header::FLAG_IS_RESPONSE, true);
                			routeMessage.fromAddress = address;
                			routeMessage.header.crc = 3;

                			bb.reset();
                			routeMessage.copyToByteBuffer(bb);
                			//RFLOG( "nRFTransportProtocol::handleMessage/"); RFLOG( __LINE__); RFLOGLN(": s3");
                			sendMessage(bb, routeMessage.header.destAddress);
                			RFLOGLN("Route request arrived. Route response sent!");

                		}
                		else{
                    		if(!messageBuffer.isElement(routeMessage.header.messageId, routeMessage.header.srcAddress)) {
                    			messageBuffer.newElement(routeMessage.header.flagsAndType, routeMessage.header.messageId, routeMessage.header.srcAddress, routeMessage.header.destAddress);

                    			if(!routing.isElement(routeMessage.header.srcAddress)){
                    			    routing.newElement(routeMessage.header.srcAddress, routeMessage.fromAddress, 0, 0, 255, 0);
                    			    RFLOGLN("New element in the routing table!");
                    			}

								routeMessage.fromAddress = address;
								bb.reset();
								routeMessage.copyToByteBuffer(bb);
								//RFLOG( "nRFTransportProtocol::handleMessage/"); RFLOG( __LINE__); RFLOGLN(": s4");
								routeMessage.header.crc = 4;
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
                			//RFLOG( "nRFTransportProtocol::handleMessage/"); RFLOG( __LINE__); RFLOGLN(": s5");
                			routeMessage.header.crc = 5;
                			sendMessage(bb, routeMessage.header.destAddress);
                			RFLOGLN("Route response sent!");
                		}
                	}
                }
                break;

                case Message::TYPE_SENSORDATA:
                  break;

                case Message::TYPE_ROUTING_TABLE:
                	if (!isResponse){
                		RoutingTableElementMessage rtem(bb);
                		rtem.header.setFlag(Header::FLAG_IS_RESPONSE, true);
                		uint16_t tmp = rtem.header.srcAddress;
                		rtem.header.srcAddress = address;
                		rtem.header.destAddress = tmp;

                		for (int i =0; i<routing.elementNum; i++){
                			rtem.moreElements = routing.elementNum-1-i;
                			rtem.setRoutingTableElement(routing.elements[i]);
                			bb.reset();
                			RFLOG( "nRFTransportProtocol::handleMessage/"); RFLOG( __LINE__); RFLOGLN(": route element:");
                			RFLOG( "nRFTransportProtocol::handleMessage/"); RFLOG( __LINE__); RFLOGLN(": dest:");
                			RFLOG( "nRFTransportProtocol::handleMessage/"); RFLOG( __LINE__); RFLOGLN(rtem.routingTableElement.destinationAddress);
                			RFLOG( "nRFTransportProtocol::handleMessage/"); RFLOG( __LINE__); RFLOGLN(": next:");
                			RFLOG( "nRFTransportProtocol::handleMessage/"); RFLOG( __LINE__); RFLOGLN(rtem.routingTableElement.nextHop);
                			rtem.copyToByteBuffer(bb);
                			//RFLOG( "nRFTransportProtocol::handleMessage/"); RFLOG( __LINE__); RFLOGLN(": s6");
                			rtem.header.crc = 6;
                			sendMessage(bb, rtem.header.destAddress);
                			RFDELAY(15);
                		}
                	}
                	break;

                default:
                  break;
            }

            if (forwardToApp){
              bb.reset();
              messageHandler->handleMessage(bb, type,isResponse);
            }
      }

      void nRFTransportProtocol::checkForPingTimeOut(){
          if (RFMILLIS() - waitingForPingResponse > PingMessage::MAX_WAIT_TIME){
            messageHandler->pingResponseArrived(PingMessage::TIMEOUT_VAL, currentlyPingingAddress );
            waitingForPingResponse = 0;
            currentlyPingingAddress = 0;
          }
      }
}

