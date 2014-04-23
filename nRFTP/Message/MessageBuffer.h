/*
 * MessageBuffer.h
 *
 *  Created on: 2014.04.23.
 *      Author: Molnár Bálint
 */

#ifndef MESSAGEBUFFER_H_
#define MESSAGEBUFFER_H_

#include "MessageBufferElement.h"

namespace nRFTP {

	class MessageBuffer {
		public:
			static const int size = 5; //size of the table
			uint8_t elementNum; //number of actual elements

			MessageBuffer();

			void newElement(uint8_t _flagsAndType, uint16_t _messageId, uint16_t _srcAddress, uint16_t _destAddress);
			void deleteElement(uint16_t messageId);
			bool isElement (uint16_t destinationAddress);
			uint8_t getIndexById(uint16_t messageId);

			MessageBufferElement elements[size];
	};

} /* namespace nRFTP */



#endif /* MESSAGEBUFFER_H_ */
