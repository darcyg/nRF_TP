/*
 * NeighborhoodTable.h
 *
 *  Created on: 2014.04.15.
 *      Author: Bana Szabolcs
 */

#ifndef NEIGHBORHOODTABLE_H_
#define NEIGHBORHOODTABLE_H_

#include <Routing/NeighborhoodTableElement.h>

namespace nRFTP {

	class NeighborhoodTable
	{
		public:
			static const uint8_t size = 10;	//size of the table
			uint8_t elementNum;	//number of actual elements

			NeighborhoodTable();
			virtual ~NeighborhoodTable();

			void newElement(uint16_t _address, uint8_t _rtt, uint8_t _lastActivity, uint8_t _reliability, uint8_t _reserved);
			void deleteElement(uint16_t address);
			void printNeighborhoodTable();	//Csak debughoz kell, sorosra kuld.
			bool isElement (uint16_t address);
			void resetActivity(uint16_t address);
			void decreaseActivity();
			uint16_t getNextHopAddress(uint16_t address);

			NeighborhoodTableElement elements[size];
	};

} /* namespace nRFTP */

#endif /* NEIGHBORHOODTABLE_H_ */
