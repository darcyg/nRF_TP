/*
 * MessageBuffer.cpp
 *
 *  Created on: 2014.04.23.
 *      Author: Molnár Bálint
 */
#include <Message/MessageBuffer.h>

namespace nRFTP {

MessageBuffer::MessageBuffer() {
}

void MessageBuffer::newElement(uint8_t _flagsAndType, uint16_t _messageId, uint16_t _srcAddress, uint16_t _destAddress){

		for(int j = size-1; j > 1; j--){
			elements[j].flagsAndType = elements[j-1].flagsAndType;
			elements[j].messageId = elements[j-1].messageId;
			elements[j].srcAddress = elements[j-1].srcAddress;
			elements[j].destAddress = elements[j-1].destAddress;
		}

			elements[0].flagsAndType = _flagsAndType;
			elements[0].messageId = _messageId;
			elements[0].srcAddress = _srcAddress;
			elements[0].destAddress = _destAddress;
}

bool MessageBuffer::isElement(uint16_t _messageId, uint16_t _srcAddress) {
	for(int i = 0; i < size; i++) {
		if(elements[i].messageId == _messageId && elements[i].srcAddress == _srcAddress) {
			return true;
		}
	}
	return false;
}

}

