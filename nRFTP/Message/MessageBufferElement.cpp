/*
 * MessageBufferElement.cpp
 *
 *  Created on: 2014.04.23.
 *      Author: Moln�r B�lint
 */
#include "MessageBufferElement.h"

namespace nRFTP{

	MessageBufferElement::MessageBufferElement() {

	}

	MessageBufferElement::MessageBufferElement(uint8_t _flagsAndType, uint16_t _messageId, uint16_t _srcAddress, uint16_t _destAddress):
			  flagsAndType(_flagsAndType),
			  messageId(_messageId),
			  srcAddress(_srcAddress),
			  destAddress(_destAddress)
	{
	}

}
