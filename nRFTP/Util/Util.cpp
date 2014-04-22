#include "Util.h"
#include <sys/time.h>

namespace nRFTP {

	uint64_t Util::TPAddress_to_nRF24L01Address(uint16_t address) {
		return 0xF0F0F00000LL | address;
	}


#ifndef ARDUINO
      uint64_t Util::millisSinceEpoch(){
    	  struct timeval tv;
    	  if(gettimeofday(&tv, NULL) != 0) return 0;
    	  return (unsigned long)((tv.tv_sec * 1000ul) + (tv.tv_usec / 1000ul));
      }

      uint64_t Util::millisSinceStart(){
    	  return millisSinceEpoch() - nRFTP::nRFTransportProtocol::startTime;
      }

#endif
}
