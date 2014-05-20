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
#include <iostream>

int __read(int fd, void *buf, int len ) {
	read(fd, buf, len);
}

int __write( int fd, const void *buf, int len ) {
	write(fd, buf, len);
}

namespace nRFTP {

	Serial_PhysicalLayer::Serial_PhysicalLayer(const char* sd) {
		strcpy(serialDevice, sd);
	}

	Serial_PhysicalLayer::~Serial_PhysicalLayer() {
	}

	void Serial_PhysicalLayer::begin(void) {
		fd = open(serialDevice, O_RDWR | O_NOCTTY );

		struct termios options;
		tcgetattr(fd, &options);

		options.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
		options.c_iflag = IGNPAR;
		options.c_oflag = 0;
		options.c_lflag = 0;

		tcflush(fd, TCIFLUSH);
		tcsetattr(fd, TCSANOW, &options);
	}

	bool Serial_PhysicalLayer::write( const void* buf, uint8_t len, uint16_t destAddress ) {
		__write(fd, &destAddress, sizeof(uint16_t) );
		__write(fd, buf, len );
		return true;
	}

	bool Serial_PhysicalLayer::available(void) {
		FD_ZERO(&input);
		FD_SET(fd, &input);
		timeout.tv_sec = 0;
		timeout.tv_usec = 1;
		int selected = select(fd+1, &input, NULL, NULL, &timeout);
		return selected > 0;
	}

	bool Serial_PhysicalLayer::read( void* buf, uint8_t len ) {
		std::cout<<"read..."<<std::endl;
		__read(fd, buf, len);
		return true;
	}

} /* namespace nRFTP */

#endif
