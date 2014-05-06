/*
 * RoutingTableElement.cpp
 *
 *  Created on: 2014.04.15.
 *      Author: Bana Szabolcsz
 */

#include <Routing/RoutingTableElement.h>
#include "Util/ByteBuffer.h"

namespace nRFTP {

RoutingTableElement::RoutingTableElement():
					 destinationAddress(0),
					 nextHop(0),
					 rtt(0),
					 ttl(0),
					 lastActivity(0),
					 reserved(0)  // not assigned to anything yet, available for use if needed
		{
	// TODO Auto-generated constructor stub

}

RoutingTableElement::RoutingTableElement(ByteBuffer& dest):
							 destinationAddress(dest.getWord()),
							 nextHop(dest.getWord()),
							 rtt(dest.getWord()),
							 ttl(dest.getByte()),
							 lastActivity(dest.getByte()),
							 reserved(dest.getByte())
	{
	}

void RoutingTableElement::copyToByteBuffer(ByteBuffer& dest){
	dest.putWord(destinationAddress);
	dest.putWord(nextHop);
	dest.putWord(rtt);
	dest.putByte(ttl);
	dest.putByte(lastActivity);
	dest.putByte(reserved);
}

RoutingTableElement::~RoutingTableElement() {
	// TODO Auto-generated destructor stub
}

} /* namespace nRFTP */
