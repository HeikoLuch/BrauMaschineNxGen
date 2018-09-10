#include "Arduino.h"

#ifndef err_handler_h
#define err_handler_h



#define EVENT_LEVEL_DEBUG 3
#define EVENT_LEVEL_INFO 6
#define EVENT_LEVEL_WARN 8
#define EVENT_LEVEL_ERROR 10

class ErrorHandler {
	
private:
	
public:
	
	ErrorHandler();
	void handleError(String s);
};

#endif
