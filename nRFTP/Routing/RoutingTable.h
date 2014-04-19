/*
 * RoutingTable.h
 *
 *  Created on: 2014.04.15.
 *      Author: Bana Szabolcs
 */

#ifndef ROUTINGTABLE_H_
#define ROUTINGTABLE_H_

#include <Routing/RoutingTableElement.h>

namespace nRFTP {

	class RoutingTable {
		public:
			static const int size = 10; //size of the table
			static const int count = 0; //number of actual elements

			RoutingTable();
			virtual ~RoutingTable();

			void NewElement(uint16_t _destinationAddress, uint16_t _nextHop, uint8_t _rtt, uint8_t _ttl, uint8_t _lastActivity, uint8_t _reserved);
			void DeleteElement(uint16_t destinationAddress);

			RoutingTableElement tableElement;
	};

} /* namespace nRFTP */

#endif /* ROUTINGTABLE_H_ */
