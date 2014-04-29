#include "SPI_PhysicalLayer.h"
#include "Util/ByteBuffer.h"
#include "Util/Util.h"

#define DEBUG_PL 0

namespace nRFTP{

  SPI_PhysicalLayer::SPI_PhysicalLayer( uint8_t _cepin, uint8_t _cspin):
		  test_readed_messages(0),
		  test_available_counter(0) {

  }

  void SPI_PhysicalLayer::begin(void){

	  for (int i=0; i<15; i++){
		  test_data[i] = SensorData(SensorData::TYPE_LIGHT, 10*i, 0, 0);
	  }
  }


  bool SPI_PhysicalLayer::write( const void* buf, uint8_t len, uint16_t destAddress ){
	  return false;
  }

  bool SPI_PhysicalLayer::available(void){
	  if (this->test_readed_messages < 15){
		  test_available_counter++;
	  	  if (test_available_counter %10 == 0){
	  		  return true;
	  	  }
	  }
	  return false;
  }

  bool SPI_PhysicalLayer::read( void* buf, uint8_t len ){
	  ByteBuffer bb((uint8_t*)buf);
	  test_data[this->test_readed_messages++].copyToByteBuffer(bb);
	  return false;
  }
}
