/*
 * RoutingTableElement.h
 *
 *  Created on: 2014.04.15.
 *      Author: Bana Szabolcs
 */

#ifndef ROUTINGTABLEELEMENT_H_
#define ROUTINGTABLEELEMENT_H_

#ifdef ARDUINO
	#include <Arduino.h>
#else
#include <stdint.h>
#endif

namespace nRFTP {

	class RoutingTableElement
	{
		public:
			uint16_t destinationAddress;
			uint16_t nextHop;
			uint16_t rtt; //response time
			uint8_t ttl;
			uint8_t lastActivity;
			uint8_t reserved;  // not assigned to anything yet, available for use if needed

			void copyToByteBuffer(ByteBuffer& dest);

			RoutingTableElement();
			RoutingTableElement(ByteBuffer& src);
			virtual ~RoutingTableElement();
	};

} /* namespace nRFTP */

#endif /* ROUTINGTABLEELEMENT_H_ */
