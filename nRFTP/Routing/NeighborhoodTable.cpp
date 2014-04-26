/*
 * NeighborhoodTable.cpp
 *
 *  Created on: 2014.04.15.
 *      Author: Bana Szabolcs
 */

#include <Routing/NeighborhoodTable.h>
#include "Util/Util.h"

namespace nRFTP {

	NeighborhoodTable::NeighborhoodTable() {
		elementNum = 0;
	}

	NeighborhoodTable::~NeighborhoodTable() {}

	void NeighborhoodTable::newElement(uint16_t _address, uint8_t _rtt, uint8_t _lastActivity, uint8_t _reliability, uint8_t _reserved){

		int tmp = 0;

			if(elementNum < size) {
				elements[elementNum].neighborAddress = _address;
				elements[elementNum].rtt = _rtt;
				elements[elementNum].lastActivity = _lastActivity;
				elements[elementNum].reliability = _reliability;
				elements[elementNum].reserved = _reserved;

				elementNum++;
			} else {
				for(int i = 0; i < elementNum; i++) {
					if(elements[i].lastActivity < elements[tmp].lastActivity) {
						tmp = i;
					}
				}
				elements[tmp].neighborAddress = _address;
				elements[tmp].rtt = _rtt;
				elements[tmp].lastActivity = _lastActivity;
				elements[tmp].reliability = _reliability;
				elements[tmp].reserved = _reserved;
			}
	}

	void NeighborhoodTable::deleteElement(uint16_t address) {

		for(int i= 0; i < NeighborhoodTable::size; i++)
		{
			if(elements[i].neighborAddress == address)
			{
				elements[i].neighborAddress = 0;
				elements[i].rtt = 0;
				elements[i].lastActivity = 0;
				elements[i].reliability = 0;
				elements[i].reserved = 0;

				for(int j = i; j < elementNum-1; j++){
					elements[j].neighborAddress = elements[j+1].neighborAddress;
					elements[j].rtt = elements[j+1].rtt;
					elements[j].reliability = elements[j+1].reliability;
					elements[j].lastActivity = elements[j+1].lastActivity;
					elements[j].reserved = elements[j+1].reserved;
				}
				elementNum--;
			}
		}
	}

	void NeighborhoodTable::printNeighborhoodTable() {

		RFLOGLN("-------- print neighborhood -------");
		for(int i = 0; i < NeighborhoodTable::elementNum; i++) {
			RFLOG("Number: "); 			RFLOGLN(i);
			RFLOG("Address:"); 			RFLOGLN(elements[i].neighborAddress);
			RFLOG("Reliability:"); 		RFLOGLN(elements[i].reliability);
			RFLOG("RTT:"); 				RFLOGLN(elements[i].rtt);
			RFLOG("Last Activity:"); 	RFLOGLN(elements[i].lastActivity);
			RFLOGLN("");
		}
		RFLOGLN("---------------END---------------");
	}

	bool NeighborhoodTable::isElement(uint16_t address) {
		for(int i = 0; i < elementNum; i++) {
			if(elements[i].neighborAddress == address) {
				return true;
			}
		}
		return false;
	}

	void NeighborhoodTable::resetActivity(uint16_t address) {
		for(int i=0; i < elementNum; i++)
		{
			if(address == elements[i].neighborAddress)
			{
				elements[i].lastActivity = 255;
			}
		}
	}

	void NeighborhoodTable::decreaseActivity()
	{
		for(int i=0; i < elementNum; i++)
		{
			elements[i].lastActivity--;
		}
	}

} /* namespace nRFTP */
