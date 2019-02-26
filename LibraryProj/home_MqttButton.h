// MqttButton.h

#ifndef _MQTTBUTTON_h
#define _MQTTBUTTON_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "home_MqttButtonBase.h"
#include "home_Sender.h"
#include <RBD_Timer.h>
#include <vector>
#include <string>
using namespace std;

class MqttButton: public MqttButtonBase {
public:
	MqttButton(byte buttonPin, byte relayPin, string buttonName);
	void btnPress() override;
	void mqttCallback(char* topic, byte* payload, unsigned int length) override;
	void setup() override;

};

#endif

