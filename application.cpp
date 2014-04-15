#include <nRF24L01_PhysicalLayer.h>
#include <nRFTransportProtocol.h>
#include "Util/Util.h"
#include "SPI.h"
#include "Message/Message.h"
#include <PingMessage.h>
#include <IMessageHandler.h>

#define SELF_ADDRESS 12345

using namespace nRFTP;

class nRFTPExample : public IMessageHandler {
    void handleMessage(nRFTP::ByteBuffer& bb, uint8_t type){
      switch (type){
          case nRFTP::Message::TYPE_PING:
            break;

          case Message::TYPE_ROUTE_REQUEST:
            break;

          case Message::TYPE_SENSORDATA:
            break;

          default:
            break;
      }
    }

    void pingResponseArrived(uint16_t milis, uint16_t destAddress){
        Serial.print("Pinged "); Serial.print(destAddress); Serial.print(": "); Serial.print(milis); Serial.println(" ms");
    }
};

nRFTPExample example;

nRF24L01_PhysicalLayer pLayer(Util::TPAddress_to_nRF24L01Address(SELF_ADDRESS),9,10);
nRFTransportProtocol test_nRFTP(&pLayer, SELF_ADDRESS, &example);

void setup() {
  Serial.begin(57600);
  test_nRFTP.begin();

  Serial.println(sizeof(Header));
  Serial.println("csumpa");
}

void loop() {
  test_nRFTP.run();

  if ( Serial.available() )
  {
    char c = toupper(Serial.read());
    test_nRFTP.ping(1235);
  }
}
