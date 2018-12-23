
//#include "Arduino.h"
#include "home_MqttButton.h"
#include "home_Sender.h"

#include <RBD_Timer.h>
#include <string>
using namespace std;
//ConnectionHelper* connectionHelper;

MqttButton::MqttButton(byte buttonPin, byte relayPin, string buttonName, bool levelButton)
{
	this->buttonPin = buttonPin;
	this->relayPin = relayPin;
	this->buttonName = buttonName;
	this->levelButton = levelButton;
}

void MqttButton::setup()
{
	pinMode(this->relayPin, OUTPUT);

	_lockTimer.setTimeout(lockTimout);
	_lockTimer2.setTimeout(lockTimout2);

}

void MqttButton::handle()
{
	// Для прерывания. Если запущен флаг, то публикуем состояние на брокер
	if (_flagChange) {
		Serial.println("_flagChange = true");
		_sender->publish(_publishTopics[0], String(_rState).c_str(), false);
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
		btnPress(!_rState);
		_flagChange = true;
	}

	_lockTimer2.restart(); // защищаемся от э/м скачков в реле
	_lock = false;
}

void MqttButton::mqttCallback(char* topic, byte* payload, unsigned int length) {




}


void MqttButton::btnPress(bool state)
{
	Serial.println((buttonName + ' ' + "OnBtnPress(" + String(state).c_str() + ")").c_str());

	digitalWrite(relayPin, state);

	//меняем текущее состояние
	_rState = state;
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