/*
	turn on:
		home/testproj/btn1 1
		home/testproj/btn1 {"action":"on"}
	turn off:
		home/testproj/btn1 0
		home/testproj/btn1 {"action":"off"}
	hold 10 sec:
		home/testproj/btn1 {"action":"hold", "duration":10000}
*/

//#include "Arduino.h"
#include "home_MqttButton.h"
#include "home_Sender.h"

#include <RBD_Timer.h>
#include <vector>
#include <string>
#include <ArduinoJson.h>
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

	//��� �����
	if (_flagHold) {
		if (_holdTimer.isExpired()) {

			//���������
			relaySwitch(false);

			//��������� ������
			_sender->publish(topicSwitchState, false, true);

			_flagHold = false;
		}
	}
}

void MqttButton::mqttCallback(char* topic, byte* payload, unsigned int length)
{
	if (strcmp(topic, topicSwitch.c_str()) == 0)
	{
		onTopicSwitch(payload, length);
	}
	else if (strcmp(topic, topicSwitchState.c_str()) == 0)
	{
		onTopicSwitchState(payload, length);
	}
}

void MqttButton::onTopicSwitch(byte* payload, unsigned int length)
{
	//simple request
	if (length == 1) {

		bool val = false;
		if (payload[0] == '1')
			val = true;
		else
			val = false;

		_holdTimer.stop();
		relaySwitch(val);
	}
	else {

		//deserialize json
		StaticJsonBuffer<200> jsonBuffer;
		JsonObject& root = jsonBuffer.parseObject(payload);

		if (!root.success()) {
			Serial.println("parseObject() failed");
			return;
		}

		const char* action = root["action"];

		// �������� ��� ��������� ���� � ���������� �� ���������� �������� ������
		if (strcmp(action, "on") == 0)
		{
			_holdTimer.stop();
			relaySwitch(true);
		}
		else if (strcmp(action, "off") == 0)
		{
			_holdTimer.stop();
			relaySwitch(false);
		}
		else if (strcmp(action, "hold") == 0)
		{
			//���������� ��������� ������
			relaySwitch(true);

			long timeout = root["duration"];

			if (timeout > 0)
				_holdTimer.setTimeout(timeout);
			else
				_holdTimer.setTimeout(holdTimeout);

			_holdTimer.restart();
			_flagHold = true;

		}
	}

	_sender->publish(topicSwitchState, getState(), true);
}

void MqttButton::onTopicSwitchState(byte* payload, unsigned int length)
{
	bool val = false;
	if (payload[0] == '1')
		val = true;
	else
		val = false;

	//��������� ������ ������ ���������, ����������� ���������� �����������
	bool curState = getState();
	if (val != curState)
		_sender->publish(topicSwitchState, curState, true);

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



