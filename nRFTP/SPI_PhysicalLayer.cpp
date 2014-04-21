#include "SPI_PhysicalLayer.h"
//#include "Header.h"

#define DEBUG_PL 0

namespace nRFTP{

  SPI_PhysicalLayer::SPI_PhysicalLayer( uint8_t _cepin, uint8_t _cspin) {

  }

  void SPI_PhysicalLayer::begin(void){
  }


  bool SPI_PhysicalLayer::write( const void* buf, uint8_t len, uint64_t destAddress ){
	  return false;
  }

  bool SPI_PhysicalLayer::available(void){
	  return false;
  }

  bool SPI_PhysicalLayer::read( void* buf, uint8_t len ){
	  return false;
  }
}
