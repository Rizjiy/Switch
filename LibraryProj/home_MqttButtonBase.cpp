// 
// 
// 

#include "home_MqttButtonBase.h"
#include "home_Sender.h"

#include <vector>
#include <string>
using namespace std;

MqttButtonBase::MqttButtonBase(byte buttonPin, byte relayPin, string buttonName)
{
	this->buttonPin = buttonPin;
	this->relayPin = relayPin;
	this->buttonName = buttonName;

	pinMode(this->relayPin, OUTPUT);

}

void MqttButtonBase::setup()
{
	_lockTimer.setTimeout(lockTimout);
	_lockTimer2.setTimeout(lockTimout2);

}


void MqttButtonBase::handle()
{
	// Для прерывания. Если запущен флаг, то публикуем состояние на брокер
	if (_flagChange) {

		int curState = getState();

		//состояние кнопки
		_sender->publish(topicSwitchState, curState, true);

		//отсылаем все топики
		for (int i = 0; i < _publishTopics.size(); i++)
			_sender->publish(_publishTopics[i], curState, false);

		_flagChange = false;
	}

}

// Функция, вызываемая прерыванием, для кнопки без фиксации (button without fixing)
void MqttButtonBase::interruptButtton() {
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
		btnPress();
		_flagChange = true;
	}

	_lockTimer2.restart(); // защищаемся от э/м скачков в реле
	_lock = false;
}


//добавить publish topic
void MqttButtonBase::addTopic(string topic)
{
	_publishTopics.push_back(topic);
}

void MqttButtonBase::setSender(Sender& sender)
{
	_sender = &sender;
}

//читаем текущее состояние реле
bool MqttButtonBase::getState()
{
	int curState = digitalRead(relayPin);
	if (levelTrigger)
		return curState;
	else
		return !curState;
}

void MqttButtonBase::relaySwitch(bool state)
{
	if (levelTrigger)
		digitalWrite(relayPin, state);
	else
		digitalWrite(relayPin, !state);
}
