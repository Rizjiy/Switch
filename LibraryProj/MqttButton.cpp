
#include "Arduino.h"
#include "MqttButton.h"
//#include "ConnectionHelper.h"
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
		//PublicPinState(mainPin, true, rState);
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

void MqttButton::btnPress(bool state)
{
	Serial.println((buttonName + ' ' + "OnBtnPress(" + String(state).c_str() + ")").c_str());

	digitalWrite(relayPin, state);

	//меняем текущее состояние
	_rState = state;
}
