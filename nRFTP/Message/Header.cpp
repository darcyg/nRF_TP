#include "Message/header.h"

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

  void Header::setFlag(uint8_t flagNum, boolean value){
    if (value){
      bitSet(flagsAndType,8-flagNum);
    } else {
      bitClear(flagsAndType,8-flagNum);
    }
  }

  boolean Header::getFlag(uint8_t flagNum){
    return bitRead(flagsAndType,8-flagNum) > 0;
  }

  uint8_t Header::getType(){
    return flagsAndType & typeMask;
  }

  uint8_t Header::getTypeFromFirstByte( uint8_t firstByteOfReadBuffer){
    return firstByteOfReadBuffer & typeMask;
  }

#if DEBUG_HEADER == 1
     void Header::printHeader(){
       Serial.println("-------- print header -------");
       Serial.print("Flags and type: "); Serial.println(flagsAndType, BIN);
       Serial.print("Src addres: "); Serial.println(srcAddress);
       Serial.print("Dest address: "); Serial.println(destAddress);
       Serial.print("Msg id: "); Serial.println(messageId);
       Serial.print("Fragment offset: "); Serial.println(fragmentOffset, BIN);
       Serial.println("-----------------------------");
     }
#endif

}
