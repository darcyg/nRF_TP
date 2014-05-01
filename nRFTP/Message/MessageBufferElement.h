/*
 * MessageBufferElement.h
 *
 *  Created on: 2014.04.23.
 *      Author: Molnár Bálint
 */

#ifndef MESSAGEBUFFERELEMENT_H_
#define MESSAGEBUFFERELEMENT_H_

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <stdint.h>
#endif

namespace nRFTP{

  class MessageBufferElement{
    public:
	  uint8_t flagsAndType;
	  uint16_t messageId;
	  uint16_t srcAddress;
	  uint16_t destAddress;
	  MessageBufferElement(uint8_t _flagsAndType, uint16_t _messageId, uint16_t _srcAddress, uint16_t _destAddress);
	  MessageBufferElement();
  };

}



#endif /* MESSAGEBUFFERELEMENT_H_ */
