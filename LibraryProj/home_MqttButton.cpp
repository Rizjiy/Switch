
//#include "Arduino.h"
#include "home_MqttButton.h"
#include "home_Sender.h"

#include <RBD_Timer.h>
#include <string>
using namespace std;

MqttButton::MqttButton(byte buttonPin, byte relayPin, string buttonName, bool levelButton)
{
	this->buttonPin = buttonPin;
	this->relayPin = relayPin;
	this->buttonName = buttonName;
	this->levelButton = levelButton;

	pinMode(this->relayPin, OUTPUT);

}

void MqttButton::setup()
{
	_lockTimer.setTimeout(lockTimout);
	_lockTimer2.setTimeout(lockTimout2);

}

void MqttButton::handle()
{
	// Для прерывания. Если запущен флаг, то публикуем состояние на брокер
	if (_flagChange) {
		
		bool curState = getState();

		//состояние кнопки
		_sender->publish(topicSwitchState, curState, true);

		//отсылаем все топики
		for(int i=0; i<_publishTopics.size();i++)
			_sender->publish(_publishTopics[i], curState, false);

		_flagChange = false;
	}

}


// Функция, вызываемая прерыванием, для кнопки без фиксации (button without fixing)
void MqttButton::interruptButtton() {

	//Защита от дребезга 
	if (_lock || !_lockTimer2.isExpired())
		return;
	_lock = true;
	_lockTimer.restart();
	while (!_lockTimer.isExpired())
	{
	}

	if (digitalRead(buttonPin) != levelButton)
	{
		btnPress(!getState());
		_flagChange = true;
	}

	_lockTimer2.restart(); // защищаемся от э/м скачков в реле
	_lock = false;
}

void MqttButton::mqttCallback(char* topic, byte* payload, unsigned int length) 
{
	//_sender->print(buttonName + " mqttCallback: " + topic);

	bool val = false;
	if (payload[0] == '1')
		val = true;
	else
		val = false;

	if (strcmp(topic, topicSwitch.c_str()) == 0)
	{
		// включаем или выключаем реле в зависимоти от полученных значений данных
		btnPress(val);
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


void MqttButton::btnPress(bool state)
{
	Serial.println((buttonName + ' ' + "OnBtnPress(" + String(state).c_str() + ")").c_str());

	digitalWrite(relayPin, state);

}

//добавить publish tipic
void MqttButton::addTopic(string topic)
{
	_publishTopics.push_back(topic);
}

void MqttButton::setSender(Sender& sender)
{
	Serial.print("setSender");
	_sender = &sender;
}

//читаем текущее состояние реле
bool MqttButton::getState()
{
	return digitalRead(relayPin);
}