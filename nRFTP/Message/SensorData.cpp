/*
 * SensorData.cpp
 *
 *  Created on: 2014.04.15.
 *      Author: Moln�r B�lint
 */
#include "Message/SensorData.h"
#include "Util/ByteBuffer.h"

namespace nRFTP{

	SensorData::SensorData(ByteBuffer& src)
		: Message(src){
			sensorType = src.getByte();
			sensorData = src.getFloat();
		}

	SensorData::SensorData(uint8_t _sensorType, uint32_t _sensorData, uint16_t _srcAddress, uint16_t _destAddress)
	    : Message(_srcAddress,_destAddress,Message::TYPE_SENSORDATA), sensorType(_sensorType), sensorData(_sensorData){
	    }

	SensorData::SensorData():Message(0,0,Message::TYPE_SENSORDATA){}

	void SensorData::copyToByteBuffer(ByteBuffer& dest){
		  Message::copyToByteBuffer(dest);
		  dest.putByte(sensorType);
		  dest.putFloat(sensorData);
		}

}



