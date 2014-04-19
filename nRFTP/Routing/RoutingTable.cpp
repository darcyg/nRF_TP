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

void RoutingTable::NewElement(uint16_t _destinationAddress, uint16_t _nextHop, uint8_t _rtt, uint8_t _ttl, uint8_t _lastActivity, uint8_t _reserved){

	//TODO Ha tele van a t�bla, akkor megkeress�k a legritk�bban haszn�lt c�met, �s annak a hely�re �rjuk be az �jat.

	routingTable[elementNum].tableElement.destinationAddress = _destinationAddress;
	routingTable[elementNum].tableElement.nextHop = _nextHop;
	routingTable[elementNum].tableElement.rtt = _rtt;
	routingTable[elementNum].tableElement.ttl = _ttl;
	routingTable[elementNum].tableElement.lastActivity = _lastActivity;
	routingTable[elementNum].tableElement.reserved = _reserved;

	elementNum++;

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

void RoutingTable::SendRoutingTable() {
	//TODO Jelenleg sorosra k�ldi ki a t�bl�t, ezt kell majd �talak�tani, hogy az nRF-en kereszt�l menjen el.

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

bool RoutingTable::IsElement(uint16_t _destinationAddress) {
	for(int i = 0; i <= elementNum; i++) {
		if(routingTable[i].tableElement.destinationAddress == _destinationAddress) {
			return true;
		}
		else
			return false;
	}
}

} /* namespace nRFTP */
