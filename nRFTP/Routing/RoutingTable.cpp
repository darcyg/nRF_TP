/*
 * RoutingTable.cpp
 *
 *  Created on: 2014.04.15.
 *      Author: Bana Szabolcs
 */

#include <Routing/RoutingTable.h>

namespace nRFTP {

RoutingTable::RoutingTable() : routElement(){
	// TODO Auto-generated constructor stub

}

RoutingTable::~RoutingTable() {
	// TODO Auto-generated destructor stub
}

void RoutingTable::newElement(uint16_t destinationAddress, uint16_t nextHop, uint8_t rtt, uint8_t ttl, uint8_t lastActivity, uint8_t reserved){
	routElement.destinationAddress = destinationAddress;
	routElement.nextHop = nextHop;
	routElement.rtt = rtt;
	routElement.ttl = ttl;
	routElement.lastActivity = lastActivity;
	routElement.reserved = reserved;
}

void RoutingTable::deleteElement(uint16_t destinationAddress){
}

} /* namespace nRFTP */
