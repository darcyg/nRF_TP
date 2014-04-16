#include <nRFTransportProtocol.h>

namespace nRFTP {

	  nRFTransportProtocol::nRFTransportProtocol(IPhysicalLayer* _physicalLayer, uint16_t _address)
      : physicalLayer(_physicalLayer),
        address(_address){
      }

      void nRFTransportProtocol::begin(IMessageHandler* _messageHandler){
		physicalLayer->begin();
		messageHandler = _messageHandler;
      }

      void nRFTransportProtocol::requestRouteTo(uint16_t destAddress){

      }


      void nRFTransportProtocol::ping(uint16_t destAddress){
		  PingMessage pingMessage(address, destAddress);
		  uint8_t sendBuffer[Message::SIZE];
		  ByteBuffer bb(sendBuffer);
		  pingMessage.copyToByteBuffer(bb);

		  waitingForPingResponse = millis();
		  currentlyPingingAddress = destAddress;

		  bool res = sendMessage(bb, destAddress);
      }


      void nRFTransportProtocol::discoverNeighbourhood(){

      }


      void nRFTransportProtocol::sendRoutingTable(uint16_t destAddress){

      }


      void nRFTransportProtocol::sendNeighbourhoodList(uint16_t destAddress){

      }


      bool nRFTransportProtocol::sendMessage(ByteBuffer& bb, uint16_t destAddress){
        return physicalLayer->write((const void*)bb.data, Message::SIZE, Util::TPAddress_to_nRF24L01Address(destAddress));
      }


      bool nRFTransportProtocol::available(void){
        return physicalLayer->available();
      }

      bool nRFTransportProtocol::read( ByteBuffer& buf ){
        return physicalLayer->read((void*)buf.data, Message::SIZE);
      }

      void nRFTransportProtocol::run(void){
        // a waitingForPingResponse ha nem nulla, akkor az az idõ van benne, mikor az utolsó ping üzenet küldve lett, ha nulla, akkor nem várunk válaszra
        if (waitingForPingResponse > 0){
        	// Ide akkor lép be, ha válaszra várunk

        	// A millis a mostani idõ, a waitingForPingResponse a küldési idõ, ha a kettõ között kevesebb idõ telt el, mint a MAX_WAIT_TIME (2000ms), akkor megnézzük, hogy jött-e csomag
            if (millis() - waitingForPingResponse < PingMessage::MAX_WAIT_TIME){
              // ha ide belépett, akkor még nem telt el annyi idõ, hogy timeout legyen, de vár a válaszra

              // itt nézi, hogy van e csomag
              if (available()){
            	// van csomag

            	// beolvassa a csomag payloadját a readBuffer-be, és megnézi a típusát
                ByteBuffer bb(readBuffer);
                read(bb);
                readedType = Message::getTypeFromReadBuffer(readBuffer);

                // Ha ping csomag érkezett
                if (readedType == Message::TYPE_PING){

                  // ping csomag érkezett, értesíti az appot a messageHandleren keresztül, de nem az általános handleMessageve, hanem a pingResponseArroved-del
                  // TODO: Ez így még nem jó, mert attól hogy ping csomag, lehet hogy egy másik node pingelt minket, ilyenkor nem válasznak kell kezelni, hanem válaszolni rá, továbbítani a csomagokt a protokoll fõ csomagkezelõjébe (ami még nincs kész)
                  messageHandler->pingResponseArrived((uint16_t)(millis() - waitingForPingResponse), currentlyPingingAddress );

                  // Ezeket nullázza, a waitingForPingResponse-et azért, mert már nem várakozunk, értelemszerûen a currentlyPingingAddress-t is
                  waitingForPingResponse = 0;
                  currentlyPingingAddress = 0;
                } else {
                  // Jött egy csomag várakozás közben, de nem ping típusú, ezért továbbítjuk az appnak
                  // TODO: Nem jó így, mert a protokoll csomagkezelõjét így kikerüli, ezért a protokoll fõ csomagkezelõjébe kell továbbítani, ami még nincs meg
                  messageHandler->handleMessage(bb, readedType);
                }
              }
            } else {
              // Ide akkor jön, ha mostani idõ és a waitingForPingResponse különbsége nagyobb, mint a MAX_WAIT_TIME. Ekkor az alkalmazásnak küld egy timeoutot (a 9999-es válasz), és abbahagyja a várakozást (nullázás)
              messageHandler->pingResponseArrived(PingMessage::TIMEOUT_VAL, currentlyPingingAddress );
              waitingForPingResponse = 0;
              currentlyPingingAddress = 0;
            }
        }

        if (available()){
          ByteBuffer bb(readBuffer);
          read(bb);
          readedType = Message::getTypeFromReadBuffer(readBuffer);

#if DEBUG_NRFTP == 1
          ByteBuffer dbgBb(readBuffer);
          Header h(dbgBb);
          h.printHeader();
#endif // DEBUG_NRFTP

          bool forwardToApp = true;

          switch (readedType){
            case Message::TYPE_PING:
              // Jött egy ping üzenet

              // Csak akkor válaszol rá, ha jelenleg nem várakozunk
              // TODO: Ez így nem jó, akkor is válaszolni kell, ha várakozunk, közbe pedig minket ping requestelnek
              // TODO: A run metódus elején lévõ pinges részt amennyire lehet ide át kellene rakni, csak a timeout jelzés képessége maradjon meg
              if (waitingForPingResponse == 0){
            	// Objektum létrehozása, cím cseréje
                PingMessage pingMessage(bb);
                uint16_t tmp = pingMessage.header.srcAddress;
                pingMessage.header.srcAddress = pingMessage.header.destAddress;
                pingMessage.header.destAddress = tmp;

                // bytebuffer reset a copy elõtt, aztán küldés
                bb.reset();
                pingMessage.copyToByteBuffer(bb);
                sendMessage(bb, pingMessage.header.destAddress);
              }
            break;

            case Message::TYPE_ROUTE:
            {
            	RouteMessage routeMessage(bb);
            	if(routeMessage.header.getFlag(Header::FLAG_IS_RESPONSE)){ //if FLAG == 1 then its a response, else its a request


            	}
            }
            break;

            case Message::TYPE_SENSORDATA:
            {
            	break;
            }

            default:
            break;
          }

          if (forwardToApp){
            messageHandler->handleMessage(bb, readedType);
          }
        }
      }
}

