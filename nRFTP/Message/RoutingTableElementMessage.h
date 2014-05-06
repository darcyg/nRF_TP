/*
 * RoutingTableElementMessage.h
 *
 *  Created on: May 6, 2014
 *      Author: gberes
 */

#ifndef ROUTINGTABLEELEMENTMESSAGE_H_
#define ROUTINGTABLEELEMENTMESSAGE_H_

#include "Routing/RoutingTableElement.h"

namespace nRFTP {

class RoutingTableElementMessage : public Message{
public:
	RoutingTableElementMessage(uint16_t srcAddress, uint16_t destAddress, RoutingTableElement& element);
	RoutingTableElementMessage(ByteBuffer& src);
	RoutingTableElementMessage(uint16_t srcAddress, uint16_t destAddress);
	virtual ~RoutingTableElementMessage();
	void copyToByteBuffer(ByteBuffer& dest);
	void setRoutingTableElement(RoutingTableElement& rte);

	uint8_t moreElements;
	RoutingTableElement routingTableElement;
};

} /* namespace nRFTP */

#endif /* ROUTINGTABLEELEMENTMESSAGE_H_ */
