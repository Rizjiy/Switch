
//#include "Arduino.h"
#include "home_MqttButton.h"
#include "home_MqttButtonBase.h"
#include "home_Sender.h"

#include <RBD_Timer.h>
#include <vector>
#include <string>
using namespace std;

MqttButton::MqttButton(byte buttonPin, byte relayPin, string buttonName)
{
	this->buttonPin = buttonPin;
	this->relayPin = relayPin;
	this->buttonName = buttonName;

	pinMode(this->relayPin, OUTPUT);

}

void MqttButton::setup()
{
	_lockTimer.setTimeout(lockTimout);
	_lockTimer2.setTimeout(lockTimout2);

	//��������� ����
	relaySwitch(LOW);

}

void MqttButton::handle()
{
	// ��� ����������. ���� ������� ����, �� ��������� ��������� �� ������
	if (_flagChange) {

		int curState = getState();

		//��������� ������
		_sender->publish(topicSwitchState, curState, true);

		//�������� ��� ������
		for (int i = 0; i < _publishTopics.size(); i++)
			_sender->publish(_publishTopics[i], curState, false);

		_flagChange = false;
	}
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
		// �������� ��� ��������� ���� � ���������� �� ���������� �������� ������
		relaySwitch(val);
		_sender->publish(topicSwitchState, getState(), true);
	}
	else if (strcmp(topic, topicSwitchState.c_str()) == 0)
	{
		//��������� ������ ������ ���������, ����������� ���������� �����������
		bool curState = getState();
		if (val != curState)
			_sender->publish(topicSwitchState, curState, true);
	}
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
		btnPress();
		_flagChange = true;
	}

	_lockTimer2.restart(); // ���������� �� �/� ������� � ����
	_lock = false;
}

//�������� publish topic
void MqttButton::addTopic(string topic)
{
	_publishTopics.push_back(topic);
}

void MqttButton::setSender(Sender& sender)
{
	_sender = &sender;
}

void MqttButton::btnPress() 
{
	int state = !getState();

	Serial.println((buttonName + ' ' + "BtnPress(" + String(state).c_str() + ")").c_str());

	relaySwitch(state);
}

//������ ������� ��������� ����
bool MqttButton::getState()
{
	int curState = digitalRead(relayPin);
	if (levelTrigger)
		return curState;
	else
		return !curState;
}

void MqttButton::relaySwitch(bool state)
{
	if (levelTrigger)
		digitalWrite(relayPin, state);
	else
		digitalWrite(relayPin, !state);
}



