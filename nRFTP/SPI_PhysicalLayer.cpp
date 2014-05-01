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

	  for (int i=0; i<3; i++){
		  test_data[i] = SensorData(SensorData::TYPE_LIGHT, 10*i, 965, 0);
	  }
	  for (int i=3; i<5; i++){
		  test_data[i] = SensorData(SensorData::TYPE_LIGHT, 10*i, 1023, 0);
	  }
	  for (int i=5; i<10; i++){
		  test_data[i] = SensorData(SensorData::TYPE_LIGHT, 10*i, 2250, 0);
	  }
	  for (int i=10; i<15; i++){
		  test_data[i] = SensorData(SensorData::TYPE_LIGHT, 10*i, 1234, 0);
	  }

	  for (int i=15; i<20; i++){
		  test_data[i] = SensorData(SensorData::TYPE_TEMPERATURE, 10*i, 965, 0);
	  }
	  for (int i=20; i<23; i++){
		  test_data[i] = SensorData(SensorData::TYPE_BATTERY, 10*i, 1023, 0);
	  }
	  for (int i=23; i<28; i++){
		  test_data[i] = SensorData(SensorData::TYPE_BATTERY, 10*i, 2250, 0);
	  }
	  for (int i=28; i<35; i++){
		  test_data[i] = SensorData(SensorData::TYPE_CURRENT, 10*i, 1234, 0);
	  }
  }


  bool SPI_PhysicalLayer::write( const void* buf, uint8_t len, uint16_t destAddress ){
	  return false;
  }

  bool SPI_PhysicalLayer::available(void){
	  if (this->test_readed_messages < 35 && testing){
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
