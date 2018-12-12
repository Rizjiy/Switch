
#include "Arduino.h"
#include "MqttButton.h"
#include <RBD_Timer.h>
#include <string>
using namespace std;

MqttButton::MqttButton(byte buttonPin, byte relayPin, string deviceName) 
	:lockTimer(30), lockTimer2(90)
{

	this->buttonPin = buttonPin;
	this->relayPin = relayPin;
	this->deviceName = deviceName;

	pinMode(this->relayPin, OUTPUT);

	//if (this->buttonPin >= 0)
	//	attachInterrupt(digitalPinToInterrupt(buttonPin), Interrupt_WF, levelButton ? FALLING : RISING);

}

// Функция, вызываемая прерыванием, для кнопки без фиксации (button without fixing)
void MqttButton::Interrupt_WF() {

	//Защита от дребезга 
	if (_lock || !lockTimer2.isExpired())
		return;
	_lock = true;
	lockTimer.restart();
	while (!lockTimer.isExpired())
	{
	}

	if (digitalRead(buttonPin) != levelButton)
	{
		OnBtnPress(!_rState);
		_flagChange = true;
	}

	lockTimer2.restart(); // защищаемся от э/м скачков в реле
	_lock = false;
}

void MqttButton::OnBtnPress(bool state)
{
	Serial.println("OnBtnPress(" + String(state) + ")");

	digitalWrite(relayPin, state);

	//меняем текущее состояние
	_rState = state;
}
