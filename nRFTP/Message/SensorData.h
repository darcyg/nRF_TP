/*
 * SensorData.h
 *
 *  Created on: 2014.04.15.
 *      Author: Moln�r B�lint
 */

#ifndef SENSORDATA_H_
#define SENSORDATA_H_

#include "Message/Message.h"

namespace nRFTP{

	class ByteBuffer;

	class SensorData : public Message
    {
		public:
			static const uint8_t TYPE_TEMPERATURE = 0;
			static const uint8_t TYPE_LIGHT = 1;
			static const uint8_t TYPE_BATTERY = 2;
			static const uint8_t TYPE_CURRENT = 3;

			uint8_t sensorType;
			uint32_t sensorData;

			SensorData();
			SensorData(ByteBuffer& src);
			SensorData(uint8_t _sensorType, uint32_t _sensorData, uint16_t _srcAddress, uint16_t _destAddress);
			void copyToByteBuffer(ByteBuffer& dest);
    };

}


#endif /* SENSORDATA_H_ */
