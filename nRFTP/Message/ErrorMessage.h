/*
 * ErrorMessage.h
 *
 *  Created on: 2014.04.22.
 *      Author: Molnár Bálint
 */

#ifndef ERRORMESSAGE_H_
#define ERRORMESSAGE_H_

#include "Message/Message.h"

namespace nRFTP{

	class ByteBuffer;

    class ErrorMessage : public Message
    {
        public:
			static const uint8_t TYPE_TRANSPORTLAYER = 0;
			static const uint8_t TYPE_APPLICATIONLAYER = 1;

            ErrorMessage(ByteBuffer& src);
            ErrorMessage(uint16_t _srcAddress, uint16_t _destAddress);
            void copyToByteBuffer(ByteBuffer& dest);

            uint8_t errorType;

    };

}



#endif /* ERRORMESSAGE_H_ */
