#include "nRF24L01_PhysicalLayer.h"
//#include "Header.h"

#define DEBUG_PL 0

namespace nRFTP{

  nRF24L01_PhysicalLayer::nRF24L01_PhysicalLayer(uint64_t _address, uint8_t _cepin, uint8_t _cspin) : radio(_cepin, _cspin), address(_address) {

  }

  void nRF24L01_PhysicalLayer::begin(void){
    radio.begin();
    radio.setRetries(15,15);
    radio.setPayloadSize(Message::SIZE);
    radio.openReadingPipe(0,address);
    radio.startListening();

#if DEBUG_PL == 1
    Serial.println("Physical layer begin logical address, address: ");
    Serial.println((long)((address&0xffffLL)));
#endif
  }


  bool nRF24L01_PhysicalLayer::write( const void* buf, uint8_t len, uint64_t destAddress ){
    bool result;

    radio.stopListening();
    radio.openWritingPipe(destAddress);
    result = radio.write(buf,len);

    radio.openReadingPipe(0,address);
    radio.startListening();

    return result;
  }

  bool nRF24L01_PhysicalLayer::available(void){
    return radio.available();
  }

  bool nRF24L01_PhysicalLayer::read( void* buf, uint8_t len ){
    return radio.read(buf, len);
  }
}
