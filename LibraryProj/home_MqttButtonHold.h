// MqttButtonHold.h

#ifndef _MQTTBUTTONHOLD_h
#define _MQTTBUTTONHOLD_h

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

class MqttButtonHold : public MqttButtonBase {
public:
	MqttButtonHold(byte buttonPin, byte relayPin, string buttonName, int holdTimeout);
	void btnPress() override;
	void btnPress(int timeout);
	void mqttCallback(char* topic, byte* payload, unsigned int length) override;
	void setup() override;
	void handle() override;
	int holdTimeout; //время удержания кнопки в сек

private:

};


#endif

