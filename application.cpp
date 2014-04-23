#include <nRF24L01_PhysicalLayer.h>
#include <nRFTransportProtocol.h>
#include "Util/Util.h"
#include "SPI.h"
#include "Message/Message.h"
#include "Message/SensorData.h"
#include <IMessageHandler.h>
#include "Util/ByteBuffer.h"

#define LIGHT_PIN A5
#define BATTERY_PIN A4
#define BATT_MEASURE_EN 6
#define CURRENT_PIN A3
#define TEMPERATURE_PIN 2

using namespace nRFTP;

const uint16_t SELF_ADDRESS = 11;

nRF24L01_PhysicalLayer pLayer(Util::TPAddress_to_nRF24L01Address(SELF_ADDRESS), 9, 10);
nRFTransportProtocol transportProtocol(&pLayer, SELF_ADDRESS);

class SensorNetworkMessageHandler : public IMessageHandler {
    void handleMessage(nRFTP::ByteBuffer& bb, uint8_t type, bool isResponse){
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
							sensorData.sensorData = analogRead(BATTERY_PIN); 			// Const = 13.3 / 3.3 = 0.01299;
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
							sensorData.sensorData = analogRead(CURRENT_PIN);			//Const = (3.3 * 1000) / (1024*1.6*51) = 0.03949
							break;
						}

						case SensorData::TYPE_TEMPERATURE:
						{
							// TODO OneWire protocol implement
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
        Serial.print("Pinged "); Serial.print(destAddress); Serial.print(": "); Serial.print(milis); Serial.println(" ms");
    }

}sensorNetworkMessageHandler;

void timerInt() {

}

void setup() {
  Serial.begin(57600);
  transportProtocol.begin(&sensorNetworkMessageHandler);

  Serial.println(sizeof(Header));
  Serial.println("csumpa");

  pinMode(LIGHT_PIN, INPUT);
  pinMode(BATTERY_PIN, INPUT);
  pinMode(CURRENT_PIN, INPUT);
  pinMode(BATT_MEASURE_EN, OUTPUT);

}

void loop() {
  transportProtocol.run();

  if ( Serial.available() )
  {
	int i = 0;
	delay(3);
	char addr[5];
    Serial.readBytes(addr, 5);
    transportProtocol.ping((uint16_t)atoi(addr));
   }
}


