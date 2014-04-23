/*
 * MessageBuffer.cpp
 *
 *  Created on: 2014.04.23.
 *      Author: Molnár Bálint
 */
#include <Message/MessageBuffer.h>

namespace nRFTP {

MessageBuffer::MessageBuffer() {
	elementNum = 0;
}

void MessageBuffer::newElement(uint8_t _flagsAndType, uint16_t _messageId, uint16_t _srcAddress, uint16_t _destAddress){

		if(elementNum < size) {
			elements[elementNum].flagsAndType = _flagsAndType;
			elements[elementNum].messageId = _messageId;
			elements[elementNum].srcAddress = _srcAddress;
			elements[elementNum].destAddress = _destAddress;
			elementNum++;
		}
}

void MessageBuffer::deleteElement(uint16_t _messageId) {

	for(int i= 0; i < MessageBuffer::size; i++)
	{
		if(elements[i].messageId == _messageId)
		{
			elements[i].flagsAndType = 0;
			elements[i].messageId = 0;
			elements[i].srcAddress = 0;
			elements[i].destAddress = 0;

			for(int j = i; j < elementNum-1; j++){
				elements[j].flagsAndType = elements[j+1].flagsAndType;
				elements[j].messageId = elements[j+1].messageId;
				elements[j].srcAddress = elements[j+1].srcAddress;
				elements[j].destAddress = elements[j+1].destAddress;
			}

			elementNum--;
		}
	}
}

bool MessageBuffer::isElement(uint16_t _messageId, uint16_t _srcAddress) {
	for(int i = 0; i < elementNum; i++) {
		if(elements[i].messageId == _messageId && elements[i].srcAddress == _srcAddress) {
			return true;
		}
	}
	return false;
}
}

