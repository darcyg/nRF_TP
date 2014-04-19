/*
 * RoutingTable.cpp
 *
 *  Created on: 2014.04.15.
 *      Author: Bana Szabolcs
 */

#include <Routing/RoutingTable.h>

namespace nRFTP {

RoutingTable::RoutingTable() {
	count = 0;
	// TODO Auto-generated constructor stub

}

RoutingTable::~RoutingTable() {
	// TODO Auto-generated destructor stub
}

void RoutingTable::NewElement(uint16_t _destinationAddress, uint16_t _nextHop, uint8_t _rtt, uint8_t _ttl, uint8_t _lastActivity, uint8_t _reserved){

	routingTable[count].tableElement.destinationAddress = _destinationAddress;
	routingTable[count].tableElement.nextHop = _nextHop;
	routingTable[count].tableElement.rtt = _rtt;
	routingTable[count].tableElement.ttl = _ttl;
	routingTable[count].tableElement.lastActivity = _lastActivity;
	routingTable[count].tableElement.reserved = _reserved;

	count++;

}

void RoutingTable::DeleteElement(uint16_t _destinationAddress) {
	for(int i= 0; i <= RoutingTable::size; i++)
	{
		if(routingTable[i].tableElement.destinationAddress == _destinationAddress)
		{
			routingTable[i].tableElement.destinationAddress = 0;
			routingTable[i].tableElement.nextHop = 0;
			routingTable[i].tableElement.rtt = 0;
			routingTable[i].tableElement.ttl = 0;
			routingTable[i].tableElement.lastActivity = 0;
			routingTable[i].tableElement.reserved = 0;
		}
	}
	count--;
}

} /* namespace nRFTP */
