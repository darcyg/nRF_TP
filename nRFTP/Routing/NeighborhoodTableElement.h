/*
 * NeighborhoodTableElement.h
 *
 *  Created on: 2014.04.15.
 *      Author: Bana Szabolcs
 */

#ifndef NEIGHBORHOODTABLEELEMENT_H_
#define NEIGHBORHOODTABLEELEMENT_H_

#ifdef ARDUINO
	#include <Arduino.h>
#else
#include <stdint.h>
#endif

namespace nRFTP {

	class NeighborhoodTableElement
	{
		public:
			uint16_t neighborAddress;
			uint16_t rtt; //response time
			uint8_t reliability;

			NeighborhoodTableElement();
			virtual ~NeighborhoodTableElement();
	};

} /* namespace nRFTP */

#endif /* NEIGHBORHOODTABLEELEMENT_H_ */
