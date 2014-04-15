#include "util.h"

namespace nRFTP {
  uint64_t Util::TPAddress_to_nRF24L01Address(uint16_t address){
    return 0xF0F0F00000LL | address;
  }
}
