/*
 * NeighborhoodTable.h
 *
 *  Created on: 2014.04.15.
 *      Author: Bana Szabolcs
 */

#ifndef NEIGHBORHOODTABLE_H_
#define NEIGHBORHOODTABLE_H_

namespace nRFTP {

	class NeighborhoodTable
	{
		public:
			static const int size = 10; //size of the table
			static const int count = 0; //number of actual elements

			NeighborhoodTable();
			virtual ~NeighborhoodTable();
	};

} /* namespace nRFTP */

#endif /* NEIGHBORHOODTABLE_H_ */
