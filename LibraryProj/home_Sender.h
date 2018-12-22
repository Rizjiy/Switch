// home_Sender.h

#ifndef _HOME_SENDER_h
#define _HOME_SENDER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include <PubSubClient.h>

class Sender
{
public:
	Sender(PubSubClient& mqttClient);
};

#endif

