
//#include "Arduino.h"
#include "home_MqttButtonHold.h"
#include "home_MqttButtonBase.h"
#include "home_Sender.h"

#include <RBD_Timer.h>
#include <vector>
#include <string>
using namespace std;

MqttButtonHold::MqttButtonHold(byte buttonPin, byte relayPin, string buttonName, int holdTimeout)
	:MqttButtonBase(buttonPin, relayPin, buttonName)
{
	this->holdTimeout = holdTimeout;
}

void MqttButtonHold::setup()
{
	MqttButtonBase::setup();

	//выключаем реле
	relaySwitch(LOW);

}

void MqttButtonHold::handle()
{
	MqttButtonBase::handle();


}


void MqttButtonHold::mqttCallback(char* topic, byte* payload, unsigned int length)
{
	payload[length] = '\0';
	String strPayload = String((char*)payload);
	Serial.println(strPayload);


	int val = strPayload.toInt();

	if (strcmp(topic, topicSwitch.c_str()) == 0)
	{
		// включаем или выключаем реле в зависимоти от полученных значений данных
			//кнопка уже нажата
		if (_lock || getState())
			return;

		_lock = true;
		btnPress(val);
		_lock = false;

	}
	else if (strcmp(topic, topicSwitchState.c_str()) == 0)
	{
		//обновляем статус других устройств, фактическим состоянием выключателя
		//if (val != rState)
		//	mqttclient.publish(topicSwitchState, String(rState).c_str(), true);

	}
}


void MqttButtonHold::btnPress(int timeout)
{
	Serial.println((buttonName + ' ' + "BtnPress(" + String(timeout).c_str() + ")").c_str());

	RBD::Timer timeoutTimer(timeout * 1000);

	relaySwitch(true);

	timeoutTimer.restart();
	while (!timeoutTimer.isExpired())
	{
	}

	//delay(timeout * 1000);
	relaySwitch(false);
}

void MqttButtonHold::btnPress()
{
	btnPress(holdTimeout);
}


