/*
 * RoutingTableElement.cpp
 *
 *  Created on: 2014.04.15.
 *      Author: Bana Szabolcsz
 */

#include <Routing/RoutingTableElement.h>

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

RoutingTableElement::~RoutingTableElement() {
	// TODO Auto-generated destructor stub
}

} /* namespace nRFTP */
