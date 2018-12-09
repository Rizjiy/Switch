/*
 Name:		MqttSwitch.cpp
 Created:	12/9/2018 9:50:57 PM
 Author:	rodal
 Editor:	http://www.visualmicro.com
*/

#include "MqttSwitch.h"

MqttSwitch::MqttSwitch(byte relayPin, byte buttonPin, string deviceName) {
	_relayPin = relayPin;
	_buttonPin = buttonPin;
	_deviceName = deviceName;

	pinMode(_relayPin, OUTPUT);
}

