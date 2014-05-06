/*
 * SerialPhysicalLayer.cpp
 *
 *  Created on: May 6, 2014
 *      Author: gberes
 */

#ifndef ARDUINO

#include <SerialPhysicalLayer.h>
#include <cstring>
#include <Util/Util.h>

namespace nRFTP {

Serial_PhysicalLayer::Serial_PhysicalLayer(const char* sd) {
	strcpy(serialDevice, sd);
}

Serial_PhysicalLayer::~Serial_PhysicalLayer() {
}


void Serial_PhysicalLayer::begin(void){
	file = fopen(serialDevice,"r");
}


bool Serial_PhysicalLayer::write( const void* buf, uint8_t len, uint16_t destAddress ){
	int count;
	fclose(file);
	file = fopen(serialDevice,"w");
	count = fwrite(buf,1,len,file);
	fclose(file);
	file = fopen(serialDevice,"r");
	return (count == len);
}

bool Serial_PhysicalLayer::available(void){
	if (fread(buffer,1,1,file)>0){
		RFDELAY(2);
		fread(buffer+1,1,Message::SIZE-1,file);
		return true;
	}
	return false;
}

bool Serial_PhysicalLayer::read( void* buf, uint8_t len ){
	memcpy(buf, buffer, len);
	return true;
}

} /* namespace nRFTP */

#endif
