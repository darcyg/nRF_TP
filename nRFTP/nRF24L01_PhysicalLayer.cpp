#ifdef ARDUINO
#include "nRF24L01_PhysicalLayer.h"
#include "Message/Message.h"
#include <nRF24L01.h>
#include <nRFTransportProtocol.h>
#include "Util/Util.h"


#define DEBUG_PL 1

namespace nRFTP{

  nRF24L01_PhysicalLayer::nRF24L01_PhysicalLayer(uint64_t _selfAddress, uint8_t _cepin, uint8_t _cspin) : radio(_cepin, _cspin), selfAddress(_selfAddress) {

  }
  void nRF24L01_PhysicalLayer::begin(void){
    radio.begin();
    radio.setRetries(15,15);
    radio.setDataRate(RF24_2MBPS);
    radio.setPALevel(RF24_PA_HIGH);
    radio.setPayloadSize(Message::SIZE);
    radio.openReadingPipe(0,selfAddress);
    radio.openReadingPipe(1,Util::TPAddress_to_nRF24L01Address(nRFTransportProtocol::broadcastAddress));
    //radio.setAutoAck(1, false);
    radio.startListening();

#if DEBUG_PL == 1
    RFLOGLN("Physical layer begin logical address, address: ");
    RFLOGLN((long)((selfAddress&0xffffLL)));
    RFLOGLN((long)((nRFTransportProtocol::broadcastAddress&0xffffLL)));
#endif
  }


  bool nRF24L01_PhysicalLayer::write( const void* buf, uint8_t len, uint16_t destAddress ){
    bool result;

    radio.stopListening();

    radio.openWritingPipe(Util::TPAddress_to_nRF24L01Address(destAddress));
    result = radio.write(buf,len);

    radio.openReadingPipe(0,selfAddress);
    radio.openReadingPipe(1,Util::TPAddress_to_nRF24L01Address(nRFTransportProtocol::broadcastAddress));
    //radio.setAutoAck(1, false);
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
#endif
