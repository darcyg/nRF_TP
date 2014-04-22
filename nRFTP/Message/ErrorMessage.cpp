/*
 * ErrorMessage.cpp
 *
 *  Created on: 2014.04.22.
 *      Author: Molnár Bálint
 */
#include <Message/ErrorMessage.h>
#include "Util/ByteBuffer.h"

namespace nRFTP {

	ErrorMessage::ErrorMessage(ByteBuffer& src)
    : Message(src){
		errorType = src.getByte();
    }

    ErrorMessage::ErrorMessage(uint16_t _srcAddress, uint16_t _destAddress)
    : Message(_srcAddress,_destAddress,Message::TYPE_ERROR){

    }

    void ErrorMessage::copyToByteBuffer(ByteBuffer& dest){
      Message::copyToByteBuffer(dest);
      dest.putByte(errorType);
    }

}



