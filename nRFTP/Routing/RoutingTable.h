/*
 * RoutingTable.h
 *
 *  Created on: 2014.04.15.
 *      Author: Bana Szabolcs
 */

#ifndef ROUTINGTABLE_H_
#define ROUTINGTABLE_H_

namespace nRFTP {

	class RoutingTable {
		public:
			static const int size = 10; //size of the table
			static const int count = 0; //number of actual elements

			RoutingTable();
			virtual ~RoutingTable();
	};

} /* namespace nRFTP */

#endif /* ROUTINGTABLE_H_ */
