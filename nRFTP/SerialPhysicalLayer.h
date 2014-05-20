/*
 * SerialPhysicalLayer.h
 *
 *  Created on: May 6, 2014
 *      Author: gberes
 */
#ifndef ARDUINO

#ifndef SERIALPHYSICALLAYER_H_
#define SERIALPHYSICALLAYER_H_

#include "IPhysicalLayer.h"
#include "Message/Message.h"
#include <sys/select.h>
#include <cstdio>

namespace nRFTP {

class Serial_PhysicalLayer : public IPhysicalLayer{
public:
	Serial_PhysicalLayer(const char* serialDevice);
	virtual ~Serial_PhysicalLayer();

    bool write( const void* buf, uint8_t len, uint16_t destAddress);
    bool available(void);
    bool read( void* buf, uint8_t len );
    void begin(void);

protected:
    int fd;
    uint8_t buffer[Message::SIZE];
    char serialDevice[15];

    struct timeval timeout;
    fd_set input;
};

} /* namespace nRFTP */

#endif /* SERIALPHYSICALLAYER_H_ */

#endif
