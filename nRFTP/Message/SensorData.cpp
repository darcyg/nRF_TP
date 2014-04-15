/*
 * SensorData.cpp
 *
 *  Created on: 2014.04.15.
 *      Author: Molnár Bálint
 */
#include "Message/SensorData.h"

namespace nRFTP{

	SensorData::SensorData(ByteBuffer& src)
		: Message(src){
			sensorType = src.getByte();
			sensorData = src.getUint32_t();
		}

	SensorData::SensorData(uint8_t _sensorType, uint32_t _sensorData, uint16_t _srcAddress, uint16_t _destAddress)
	    : Message(_srcAddress,_destAddress,Message::TYPE_SENSORDATA), sensorType(_sensorType), sensorData(_sensorData){
	    }

	void SensorData::copyToByteBuffer(ByteBuffer& dest){
		  Message::copyToByteBuffer(dest);
		  dest.putByte(sensorType);
		  dest.putUint32_t(sensorData);
		}

}



