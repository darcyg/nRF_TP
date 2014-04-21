#include "Message/Header.h"
#include "Util/Util.h"
#include "Util/ByteBuffer.h"


namespace nRFTP {

  Header::Header() : fragmentOffset(0){
  }

  Header::Header( ByteBuffer& src )
  : flagsAndType(src.getByte()),
    srcAddress(src.getWord()),
    destAddress(src.getWord()),
    messageId(src.getWord()),
    fragmentOffset(src.getByte()),
    crc(src.getUint32_t())   {
  }

  void Header::copyToByteBuffer(ByteBuffer& dest){
    dest.putByte(flagsAndType);
    dest.putWord(srcAddress);
    dest.putWord(destAddress);
    dest.putWord(messageId);
    dest.putByte(fragmentOffset);
    dest.putUint32_t(crc);
  }

  void Header::setType(uint8_t type){
    type = type & typeMask;
    flagsAndType = flagsAndType & (~typeMask);
    flagsAndType = flagsAndType | type;
  }

  void Header::setFlag(uint8_t flagNum, bool value){
#ifdef ARDUINO
    if (value){
      bitSet(flagsAndType,8-flagNum);
    } else {
      bitClear(flagsAndType,8-flagNum);
    }
#endif
  }

  bool Header::getFlag(uint8_t flagNum){
#ifdef ARDUINO
    return bitRead(flagsAndType,8-flagNum) > 0;
#endif
  }

  uint8_t Header::getType(){
    return flagsAndType & typeMask;
  }

  uint8_t Header::getTypeFromFirstByte( uint8_t firstByteOfReadBuffer){
    return firstByteOfReadBuffer & typeMask;
  }

  bool Header::isResponseFromFirstByte( uint8_t firstByteOfReadBuffer){
#ifdef ARDUINO
	  return bitRead(firstByteOfReadBuffer,8-FLAG_IS_RESPONSE) > 0;
#endif
  }

#if DEBUG_HEADER == 1
     void Header::printHeader(){
       RFLOGLN("-------- print header -------");
       RFLOG("Flags and type: "); RFLOGLN(flagsAndType);
       RFLOG("Src addres: "); RFLOGLN(srcAddress);
       RFLOG("Dest address: "); RFLOGLN(destAddress);
       RFLOG("Msg id: "); RFLOGLN(messageId);
       RFLOG("Fragment offset: "); RFLOGLN(fragmentOffset);
       RFLOGLN("-----------------------------");
     }
#endif

}
