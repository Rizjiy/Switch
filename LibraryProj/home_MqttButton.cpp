
//#include "Arduino.h"
#include "home_MqttButton.h"
#include "home_MqttButtonBase.h"
#include "home_Sender.h"

#include <RBD_Timer.h>
#include <vector>
#include <string>
using namespace std;

MqttButton::MqttButton(byte buttonPin, byte relayPin, string buttonName)
	:MqttButtonBase(buttonPin, relayPin,buttonName)
{
}

void MqttButton::setup()
{
	MqttButtonBase::setup();

	//выключаем реле
	relaySwitch(LOW);

}

void MqttButton::mqttCallback(char* topic, byte* payload, unsigned int length) 
{
	bool val = false;
	if (payload[0] == '1')
		val = true;
	else
		val = false;

	if (strcmp(topic, topicSwitch.c_str()) == 0)
	{
		// включаем или выключаем реле в зависимоти от полученных значений данных
		relaySwitch(val);
		_sender->publish(topicSwitchState, getState(), true);
	}
	else if (strcmp(topic, topicSwitchState.c_str()) == 0)
	{
		//обновляем статус других устройств, фактическим состоянием выключателя
		bool curState = getState();
		if (val != curState)
			_sender->publish(topicSwitchState, curState, true);
	}
}


void MqttButton::btnPress() 
{
	int state = !getState();

	Serial.println((buttonName + ' ' + "BtnPress(" + String(state).c_str() + ")").c_str());

	relaySwitch(state);
}

