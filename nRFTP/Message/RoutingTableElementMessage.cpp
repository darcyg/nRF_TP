/*
 * RoutingTableElementMessage.cpp
 *
 *  Created on: May 6, 2014
 *      Author: gberes
 */

#include <Message/RoutingTableElementMessage.h>
#include "Util/ByteBuffer.h"
#include "Message/Message.h"

namespace nRFTP {

RoutingTableElementMessage::RoutingTableElementMessage(uint16_t srcAddress, uint16_t destAddress) :
		Message(srcAddress, destAddress, Message::TYPE_ROUTING_TABLE), moreElements(0){
}

RoutingTableElementMessage::RoutingTableElementMessage(uint16_t srcAddress, uint16_t destAddress, RoutingTableElement& src) :
		Message(srcAddress, destAddress, Message::TYPE_ROUTING_TABLE), moreElements(0){
	routingTableElement = src;
}

RoutingTableElementMessage::RoutingTableElementMessage(ByteBuffer& src) :
		Message(src), moreElements(src.getByte()), routingTableElement(src){
}

RoutingTableElementMessage::~RoutingTableElementMessage() {
}

void RoutingTableElementMessage::setRoutingTableElement(RoutingTableElement& rte){
	routingTableElement = rte;
}

void RoutingTableElementMessage::copyToByteBuffer(ByteBuffer& dest){
	Message::copyToByteBuffer(dest);
	dest.putByte(moreElements);
	routingTableElement.copyToByteBuffer(dest);
}

} /* namespace nRFTP */
