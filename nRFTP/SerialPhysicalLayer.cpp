/*
 * SerialPhysicalLayer.cpp
 *
 *  Created on: May 6, 2014
 *      Author: gberes
 */

#ifndef ARDUINO

#include <unistd.h>
#include <SerialPhysicalLayer.h>
#include <cstring>
#include <Util/Util.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <fcntl.h>

int __read(int   fd,  void  *buf,  int  len ){
	read(fd, buf, len);
}

int  __write(  int  fd,  const void  *buf,  int  len  ){
	write(fd, buf, len);
}

namespace nRFTP {

Serial_PhysicalLayer::Serial_PhysicalLayer(const char* sd) {
	strcpy(serialDevice, sd);
}

Serial_PhysicalLayer::~Serial_PhysicalLayer() {
}


void Serial_PhysicalLayer::begin(void){
//	file = fopen(serialDevice,"r");
	fd = open(serialDevice, O_RDWR | O_NOCTTY);
	struct termios options;
	tcgetattr(fd, &options);
	cfsetispeed(&options, B115200);
	cfsetospeed(&options, B115200);
	tcsetattr(fd, TCSANOW, &options);

	 timeout.tv_sec = 0;
	 timeout.tv_usec = 1000;

	 FD_ZERO(&set); /* clear the set */
	 FD_SET(fd, &set); /* add our file descriptor to the set */
}


bool Serial_PhysicalLayer::write( const void* buf, uint8_t len, uint16_t destAddress ){
	int count;
//	fclose(file);
//	file = fopen(serialDevice,"w");
//	count = fwrite(buf,1,len,file);
//	fclose(file);
//	file = fopen(serialDevice,"r");
	__write(fd, buf, len );
	return (count == len);
}

bool Serial_PhysicalLayer::available(void){
//	if (fread(buffer,1,1,file)>0){
//		RFDELAY(2);
//		fread(buffer+1,1,Message::SIZE-1,file);
//		return true;
//	}
	int rv = select(fd + 1, &set, NULL, NULL, &timeout);
	if (rv > 0){
		return true;
	}
	return false;
}

bool Serial_PhysicalLayer::read( void* buf, uint8_t len ){
	__read(fd, buf, len);
	return true;
}

} /* namespace nRFTP */

#endif
