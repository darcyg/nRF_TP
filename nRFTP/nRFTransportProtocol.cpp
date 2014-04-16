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
        // a waitingForPingResponse ha nem nulla, akkor az az id� van benne, mikor az utols� ping �zenet k�ldve lett, ha nulla, akkor nem v�runk v�laszra
        if (waitingForPingResponse > 0){
        	// Ide akkor l�p be, ha v�laszra v�runk

        	// A millis a mostani id�, a waitingForPingResponse a k�ld�si id�, ha a kett� k�z�tt kevesebb id� telt el, mint a MAX_WAIT_TIME (2000ms), akkor megn�zz�k, hogy j�tt-e csomag
            if (millis() - waitingForPingResponse < PingMessage::MAX_WAIT_TIME){
              // ha ide bel�pett, akkor m�g nem telt el annyi id�, hogy timeout legyen, de v�r a v�laszra

              // itt n�zi, hogy van e csomag
              if (available()){
            	// van csomag

            	// beolvassa a csomag payloadj�t a readBuffer-be, �s megn�zi a t�pus�t
                ByteBuffer bb(readBuffer);
                read(bb);
                readedType = Message::getTypeFromReadBuffer(readBuffer);

                // Ha ping csomag �rkezett
                if (readedType == Message::TYPE_PING){

                  // ping csomag �rkezett, �rtes�ti az appot a messageHandleren kereszt�l, de nem az �ltal�nos handleMessageve, hanem a pingResponseArroved-del
                  // TODO: Ez �gy m�g nem j�, mert att�l hogy ping csomag, lehet hogy egy m�sik node pingelt minket, ilyenkor nem v�lasznak kell kezelni, hanem v�laszolni r�, tov�bb�tani a csomagokt a protokoll f� csomagkezel�j�be (ami m�g nincs k�sz)
                  messageHandler->pingResponseArrived((uint16_t)(millis() - waitingForPingResponse), currentlyPingingAddress );

                  // Ezeket null�zza, a waitingForPingResponse-et az�rt, mert m�r nem v�rakozunk, �rtelemszer�en a currentlyPingingAddress-t is
                  waitingForPingResponse = 0;
                  currentlyPingingAddress = 0;
                } else {
                  // J�tt egy csomag v�rakoz�s k�zben, de nem ping t�pus�, ez�rt tov�bb�tjuk az appnak
                  // TODO: Nem j� �gy, mert a protokoll csomagkezel�j�t �gy kiker�li, ez�rt a protokoll f� csomagkezel�j�be kell tov�bb�tani, ami m�g nincs meg
                  messageHandler->handleMessage(bb, readedType);
                }
              }
            } else {
              // Ide akkor j�n, ha mostani id� �s a waitingForPingResponse k�l�nbs�ge nagyobb, mint a MAX_WAIT_TIME. Ekkor az alkalmaz�snak k�ld egy timeoutot (a 9999-es v�lasz), �s abbahagyja a v�rakoz�st (null�z�s)
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
              // J�tt egy ping �zenet

              // Csak akkor v�laszol r�, ha jelenleg nem v�rakozunk
              // TODO: Ez �gy nem j�, akkor is v�laszolni kell, ha v�rakozunk, k�zbe pedig minket ping requestelnek
              // TODO: A run met�dus elej�n l�v� pinges r�szt amennyire lehet ide �t kellene rakni, csak a timeout jelz�s k�pess�ge maradjon meg
              if (waitingForPingResponse == 0){
            	// Objektum l�trehoz�sa, c�m cser�je
                PingMessage pingMessage(bb);
                uint16_t tmp = pingMessage.header.srcAddress;
                pingMessage.header.srcAddress = pingMessage.header.destAddress;
                pingMessage.header.destAddress = tmp;

                // bytebuffer reset a copy el�tt, azt�n k�ld�s
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

