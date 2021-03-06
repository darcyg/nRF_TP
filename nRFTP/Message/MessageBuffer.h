/*
 * MessageBuffer.h
 *
 *  Created on: 2014.04.23.
 *      Author: Moln�r B�lint
 */

#ifndef MESSAGEBUFFER_H_
#define MESSAGEBUFFER_H_

#include "MessageBufferElement.h"

namespace nRFTP {

	class MessageBuffer {
		public:
			static const int size = 5; //size of the table

			MessageBuffer();

			void newElement(uint8_t _flagsAndType, uint16_t _messageId, uint16_t _srcAddress, uint16_t _destAddress);
			void deleteElement(uint16_t messageId);
			bool isElement (uint16_t messageId, uint16_t srcAddress);
			uint8_t getIndexById(uint16_t messageId);
			void printMessageBuffer();

			MessageBufferElement elements[size];
	};

} /* namespace nRFTP */



#endif /* MESSAGEBUFFER_H_ */
