
#include "Arduino.h"
#include "MqttButton.h"
#include <RBD_Timer.h>
#include <string>
using namespace std;

MqttButton::MqttButton(byte relayPin, byte buttonPin, string deviceName, bool levelButton)
{
	this->buttonPin = buttonPin;
	this->relayPin = relayPin;
	this->deviceName = deviceName;
	this->levelButton = levelButton;
}

void MqttButton::setup()
{
	pinMode(this->relayPin, OUTPUT);

	_lockTimer.setTimeout(lockTimout);
	_lockTimer2.setTimeout(lockTimout2);

	////���������� ����������
	//if (buttonPin >= 0) {
	//	//std::function<void()> callback(
	//	auto callback =
	//		[](int buttonPin)
	//	{
	//		Serial.print("button pin: ");
	//		Serial.println(buttonPin);
	//	}
	//	;

	//	attachInterrupt(digitalPinToInterrupt(buttonPin), callback, levelButton ? FALLING : RISING);

	//}
}

// �������, ���������� �����������, ��� ������ ��� �������� (button without fixing)
void MqttButton::interruptButtton() {

	//������ �� �������� 
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

	_lockTimer2.restart(); // ���������� �� �/� ������� � ����
	_lock = false;
}

void MqttButton::btnPress(bool state)
{
	Serial.println((deviceName + ' ' + "OnBtnPress(" + String(state).c_str() + ")").c_str());

	digitalWrite(relayPin, state);

	//������ ������� ���������
	_rState = state;
}
