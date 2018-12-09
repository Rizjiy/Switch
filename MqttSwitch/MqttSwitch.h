/*
 Name:		MqttSwitch.h
 Created:	12/9/2018 9:50:57 PM
 Author:	rodal
 Editor:	http://www.visualmicro.com
*/

#ifndef _MqttSwitch_h
#define _MqttSwitch_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <string>
using namespace std;

class MqttSwitch {
public:
	MqttSwitch(byte relayPin, byte buttonPin, string string);
	//bool MqttConnect();
private:
	byte _relayPin;
	byte _buttonPin;
	string _deviceName;
};


#endif



