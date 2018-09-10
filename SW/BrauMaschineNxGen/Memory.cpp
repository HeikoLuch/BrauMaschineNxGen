#include "Memory.h"

Memory::Memory()
{
}

unsigned long Memory::freeRam() {
  return ESP.getFreeHeap();
  return 0;
}

void Memory::serialPrintInfo() {
  //Serial.println ("=== Memory ===");
  Serial.print ("getFreeHeap() : "); Serial.print ( ESP.getFreeHeap()); Serial.println(" Bytes");
  //Serial.print ("getFlashChipSize() : "); Serial.print ( ESP.getFlashChipSize()); Serial.println(" Bytes"); //gets the size of the flash as set by the compiler
  //Serial.println ("=== End: Memory ===");
}
