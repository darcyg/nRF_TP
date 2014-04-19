/*
 * RoutingTable.cpp
 *
 *  Created on: 2014.04.15.
 *      Author: Bana Szabolcs
 */

#include <Routing/RoutingTable.h>

namespace nRFTP {

RoutingTable::RoutingTable() {
	elementNum = 0;
	// TODO Auto-generated constructor stub

}

RoutingTable::~RoutingTable() {
	// TODO Auto-generated destructor stub
}

void RoutingTable::newElement(uint16_t _destinationAddress, uint16_t _nextHop, uint8_t _rtt, uint8_t _ttl, uint8_t _lastActivity, uint8_t _reserved){

	//TODO Ha tele van a tábla, akkor megkeressük a legritkábban használt címet, és annak a helyére írjuk be az újat.

	routingTable[elementNum].tableElement.destinationAddress = _destinationAddress;
	routingTable[elementNum].tableElement.nextHop = _nextHop;
	routingTable[elementNum].tableElement.rtt = _rtt;
	routingTable[elementNum].tableElement.ttl = _ttl;
	routingTable[elementNum].tableElement.lastActivity = _lastActivity;
	routingTable[elementNum].tableElement.reserved = _reserved;

	elementNum++;

}

void RoutingTable::deleteElement(uint16_t _destinationAddress) {

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

			for(int j = i; j <= elementNum; j++){
				routingTable[j].tableElement.destinationAddress = routingTable[j+1].tableElement.destinationAddress;
				routingTable[j].tableElement.nextHop = routingTable[j+1].tableElement.nextHop;
				routingTable[j].tableElement.rtt = routingTable[j+1].tableElement.rtt;
				routingTable[j].tableElement.ttl = routingTable[j+1].tableElement.ttl;
				routingTable[j].tableElement.lastActivity = routingTable[j+1].tableElement.lastActivity;
				routingTable[j].tableElement.reserved = routingTable[j+1].tableElement.reserved;
			}

			elementNum--;
		}
	}
}

void RoutingTable::printRoutingTable() {

	for(int i = 0; i < RoutingTable::elementNum; i++) {
		Serial.println("-------- print routing -------");
		Serial.print("Dest:"); 			Serial.println(routingTable[i].tableElement.destinationAddress);
		Serial.print("Next hop:"); 		Serial.println(routingTable[i].tableElement.nextHop);
		Serial.print("RTT:"); 			Serial.println(routingTable[i].tableElement.rtt);
		Serial.print("TTL:"); 			Serial.println(routingTable[i].tableElement.ttl);
		Serial.print("Last Activity:"); Serial.println(routingTable[i].tableElement.lastActivity);
		Serial.println("------------------------------");
	}
}

bool RoutingTable::isElement(uint16_t _destinationAddress) {
	for(int i = 0; i <= elementNum; i++) {
		if(routingTable[i].tableElement.destinationAddress == _destinationAddress) {
			return true;
		}
		else
			return false;
	}
}

uint16_t RoutingTable::getNextHopAddress(uint16_t destinationAddress){
	for(int i=0; i < elementNum; i++)
	{
		if(destinationAddress == routingTable[i].tableElement.destinationAddress)
		{
			return routingTable[i].tableElement.nextHop;
		}
	}
}

} /* namespace nRFTP */
