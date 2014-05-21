#include <nRF24L01_PhysicalLayer.h>
#include <nRFTransportProtocol.h>
#include "Util/Util.h"
#include "SPI.h"
#include "Message/Message.h"
#include "Message/SensorData.h"
#include "Message/Header.h"
#include <IMessageHandler.h>
#include "Util/ByteBuffer.h"
#include "Sensor/OneWire.h"
#include "Sensor/DallasTemperature.h"
#include <Message/MessageBuffer.h>

#define GATEWAY_NODE 0

using namespace nRFTP;

#if GATEWAY_NODE==1
	const uint16_t SELF_ADDRESS = 0;
#else
	const uint16_t SELF_ADDRESS = 33;
#endif

const uint16_t GATEWAY_ADDRESS = 0;
const uint16_t UNDEFINED_ADDRESS = 65000;

nRF24L01_PhysicalLayer pLayer(Util::TPAddress_to_nRF24L01Address(SELF_ADDRESS), 9, 10);

#if GATEWAY_NODE==1
	char readBuffer[Message::SIZE];
	char readAddresBuffer[sizeof(uint16_t)];
#else

#define LIGHT_PIN A5
#define BATTERY_PIN A4
#define BATT_MEASURE_EN 6
#define CURRENT_PIN A3
#define ONE_WIRE_BUS 2

bool doPing = false;
uint16_t counter = 0;
uint16_t lostpacket = 0;
uint16_t totalPacket = 500;

char addr[5];

nRFTransportProtocol transportProtocol(&pLayer, SELF_ADDRESS);

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);


class SensorNetworkMessageHandler : public IMessageHandler {
    void handleMessage(nRFTP::ByteBuffer& bb, uint8_t type, bool isResponse){
#if GATEWAY_NODE == 1
    	digitalWrite(13, HIGH);
    	//Serial.write(bb.data, Message::SIZE);
  	    delay(200);
  	    digitalWrite(13, LOW);
#endif
      switch (type){
          case nRFTP::Message::TYPE_PING:
            break;

          case Message::TYPE_ROUTE:
            break;

          case Message::TYPE_SENSORDATA:
          {
				SensorData sensorData(bb);
				if(!isResponse)		//This message was request.
				{
					uint16_t tmp = sensorData.header.srcAddress;
					sensorData.header.srcAddress = sensorData.header.destAddress;
					sensorData.header.destAddress = tmp;
					sensorData.header.setFlag(sensorData.header.FLAG_IS_RESPONSE, 1);

					switch(sensorData.sensorType){
						case SensorData::TYPE_BATTERY:
						{
							digitalWrite(BATT_MEASURE_EN, HIGH);
							sensorData.sensorData = analogRead(BATTERY_PIN) * 0.01299; 			// Const = 13.3 / 3.3 = 0.01299;
							digitalWrite(BATT_MEASURE_EN, LOW);
							break;
						}

						case SensorData::TYPE_LIGHT:
						{
							sensorData.sensorData = analogRead(LIGHT_PIN);
							break;
						}

						case SensorData::TYPE_CURRENT:
						{
							sensorData.sensorData = analogRead(CURRENT_PIN) * 0.03949;			//Const = (3.3 * 1000) / (1024*1.6*51) = 0.03949
							break;
						}

						case SensorData::TYPE_TEMPERATURE:
						{
							sensors.requestTemperatures();
							sensorData.sensorData = sensors.getTempCByIndex(0);
							break;
						}

						default:
							break;

					}
					bb.reset();
					sensorData.copyToByteBuffer(bb);
					delay(20);
					transportProtocol.sendMessage(bb, sensorData.header.destAddress);
				}
				else
				{
					// This message was response, the payload contains the sensor data and type.
				}


            break;
            }


          default:
            break;
      }
    }

    void pingResponseArrived(uint16_t milis, uint16_t destAddress){
    	if(milis == 9999) {
    		lostpacket++;
    	}
        RFLOG("Pinged "); Serial.print(destAddress); RFLOG(": "); RFLOGLN(milis);
    	//RFLOGLN(milis);

//        doPing = true; // TODO ping automatikus teszthez kell, torolheto ha mar nem kell
//
//        if(counter >= totalPacket) {
//          RFLOG("Pinged: "); RFLOGLN(destAddress);
//      	  RFLOG("Total packets: "); RFLOGLN(counter);
//      	  RFLOG("Lost packets: "); RFLOGLN(lostpacket);
//      	  counter = 0;
//      	  doPing = false;
//      	  lostpacket = 0;
//        }
    }

}sensorNetworkMessageHandler;
#endif


void setup() {
#if GATEWAY_NODE == 1
	pLayer.begin();
	Serial.begin(115200);
	Serial.setTimeout(20);
	pinMode(13, OUTPUT);
#else
  Serial.begin(57600);

  transportProtocol.begin(&sensorNetworkMessageHandler);

  pinMode(LIGHT_PIN, INPUT);
  pinMode(BATTERY_PIN, INPUT);
  pinMode(CURRENT_PIN, INPUT);
  pinMode(BATT_MEASURE_EN, OUTPUT);

  sensors.begin();
#endif
}

void loop() {


#if GATEWAY_NODE == 1
  if ( Serial.available() >= (sizeof(uint16_t)+ Message::SIZE) )
  {
	  Serial.readBytes(readAddresBuffer, sizeof(uint16_t));
	  Serial.readBytes(readBuffer, Message::SIZE);
	  uint16_t* pAddr =  reinterpret_cast<uint16_t*>(readAddresBuffer);

	  pLayer.write(readBuffer,Message::SIZE,*pAddr);
  }

  if (pLayer.available()){
	  pLayer.read(readBuffer, Message::SIZE);
	  Serial.write(readBuffer, Message::SIZE);
  }
#else
  transportProtocol.run();
    	  if ( Serial.available() )
    	  {
    		for (int i=0; i<5; i++) addr[i] = 0;
    		int i = 0;
    		delay(3);
    	    Serial.readBytes(addr, 5);
    	    if (addr[0] == 'r' && addr[1] == 't') {			//routing table
    	    	transportProtocol.routing.printRoutingTable();
    	    } else if (addr[0] == 'm' && addr[1] == 'b'){	//messagebuffer
    	    	transportProtocol.messageBuffer.printMessageBuffer();
    	    } else {
    	    	transportProtocol.ping((uint16_t)atoi(addr));
    	    //	doPing = true;
    	    }
    	    }
    	  if(doPing && (counter < totalPacket)){
    	      	doPing = false;
    	      	transportProtocol.ping((uint16_t)atoi(addr));
    	      	counter ++;
    	  }
#endif



}
