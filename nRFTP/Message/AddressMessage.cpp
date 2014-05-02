/*
 * AddressMessage.cpp
 *
 *  Created on: 2014.05.02.
 *      Author: Bana Szabolcs
 */

#include <Message/AddressMessage.h>
#include "Util/ByteBuffer.h"

namespace nRFTP {

	AddressMessage::AddressMessage() : Message(0, GATEWAY_ADDRESS, Message::TYPE_ADDRESS) {
	}

	AddressMessage::AddressMessage(ByteBuffer& src) : Message(src){
		address = src.getWord();
	}

	void AddressMessage::copyToByteBuffer(ByteBuffer& dest){
		Message::copyToByteBuffer(dest);
		dest.putWord(address);
	}

} /* namespace nRFTP */
