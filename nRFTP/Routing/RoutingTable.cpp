/*
 * RoutingTable.cpp
 *
 *  Created on: 2014.04.15.
 *      Author: Bana Szabolcs
 */

#include <Routing/RoutingTable.h>
#include "Util/Util.h"

namespace nRFTP {

RoutingTable::RoutingTable() {
	elementNum = 0;


}

RoutingTable::~RoutingTable() {
}

void RoutingTable::newElement(uint16_t _destinationAddress, uint16_t _nextHop, uint8_t _rtt, uint8_t _ttl, uint8_t _lastActivity, uint8_t _reserved){

	//TODO Ha tele van a t�bla, akkor megkeress�k a legritk�bban haszn�lt c�met, �s annak a hely�re �rjuk be az �jat.

	elements[elementNum].destinationAddress = _destinationAddress;
	elements[elementNum].nextHop = _nextHop;
	elements[elementNum].rtt = _rtt;
	elements[elementNum].ttl = _ttl;
	elements[elementNum].lastActivity = _lastActivity;
	elements[elementNum].reserved = _reserved;

	elementNum++;

}

void RoutingTable::deleteElement(uint16_t _destinationAddress) {

	for(int i= 0; i <= RoutingTable::size; i++)
	{
		if(elements[i].destinationAddress == _destinationAddress)
		{
			elements[i].destinationAddress = 0;
			elements[i].nextHop = 0;
			elements[i].rtt = 0;
			elements[i].ttl = 0;
			elements[i].lastActivity = 0;
			elements[i].reserved = 0;

			for(int j = i; j <= elementNum; j++){
				elements[j].destinationAddress = elements[j+1].destinationAddress;
				elements[j].nextHop = elements[j+1].nextHop;
				elements[j].rtt = elements[j+1].rtt;
				elements[j].ttl = elements[j+1].ttl;
				elements[j].lastActivity = elements[j+1].lastActivity;
				elements[j].reserved = elements[j+1].reserved;
			}

			elementNum--;
		}
	}
}

void RoutingTable::printRoutingTable() {

	for(int i = 0; i < RoutingTable::elementNum; i++) {
		RFLOGLN("-------- print routing -------");
		RFLOG("Dest:"); 			RFLOGLN(elements[i].destinationAddress);
		RFLOG("Next hop:"); 		RFLOGLN(elements[i].nextHop);
		RFLOG("RTT:"); 			RFLOGLN(elements[i].rtt);
		RFLOG("TTL:"); 			RFLOGLN(elements[i].ttl);
		RFLOG("Last Activity:"); RFLOGLN(elements[i].lastActivity);
		RFLOGLN("------------------------------");
	}
}

bool RoutingTable::isElement(uint16_t _destinationAddress) {
	for(int i = 0; i <= elementNum; i++) {
		if(elements[i].destinationAddress == _destinationAddress) {
			return true;
		}
		else
			return false;
	}
	return false;
}

uint16_t RoutingTable::getNextHopAddress(uint16_t destinationAddress){
	for(int i=0; i < elementNum; i++)
	{
		if(destinationAddress == elements[i].destinationAddress)
		{
			return elements[i].nextHop;
		}
	}
	return -1;
}

} /* namespace nRFTP */
