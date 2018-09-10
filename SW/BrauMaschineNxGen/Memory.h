#include "Arduino.h"

#ifndef memory_h
#define memory_h



class Memory {
	
private:
	
public:
	
	Memory();
	unsigned long freeRam(); 
  void serialPrintInfo(); //print some infos regarding memory state
};



#endif
