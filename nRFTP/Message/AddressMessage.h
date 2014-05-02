/*
 * AddressMessage.h
 *
 *  Created on: 2014.05.02.
 *      Author: Bana Szabolcs
 */

#ifndef ADDRESSMESSAGE_H_
#define ADDRESSMESSAGE_H_

#include "Message/Message.h"

namespace nRFTP {

	class ByteBuffer;

	class AddressMessage : public Message{
		public:
		AddressMessage();

		AddressMessage(ByteBuffer& src);
        void copyToByteBuffer(ByteBuffer& dest);

		protected:
		uint16_t address;
	};

} /* namespace nRFTP */

#endif /* ADDRESSMESSAGE_H_ */
