/*
 * RoutingTable.h
 *
 *  Created on: 2014.04.15.
 *      Author: Bana Szabolcs
 */

#ifndef ROUTINGTABLE_H_
#define ROUTINGTABLE_H_

#include <Routing/RoutingTableElement.h>
#include <Arduino.h>

namespace nRFTP {

	class RoutingTable {
		public:
			static const int size = 10; //size of the table
			static const int count = 0; //number of actual elements

			RoutingTable();
			virtual ~RoutingTable();

			void newElement(uint16_t destinationAddress, uint16_t nextHop, uint8_t rtt, uint8_t ttl, uint8_t lastActivity, uint8_t reserved);
			void deleteElement(uint16_t destinationAddress);

			RoutingTableElement routElement;
			uint8_t *element;
	};

} /* namespace nRFTP */

#endif /* ROUTINGTABLE_H_ */
