
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
	// ��� ����������. ���� ������� ����, �� ��������� ��������� �� ������
	if (_flagChange) {
		
		bool curState = getState();

		//��������� ������
		_sender->publish(topicSwitchState, curState, true);

		//�������� ��� ������
		for(int i=0; i<_publishTopics.size();i++)
			_sender->publish(_publishTopics[i], curState, false);

		_flagChange = false;
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
		btnPress(!getState());
		_flagChange = true;
	}

	_lockTimer2.restart(); // ���������� �� �/� ������� � ����
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
		// �������� ��� ��������� ���� � ���������� �� ���������� �������� ������
		btnPress(val);
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


void MqttButton::btnPress(bool state)
{
	Serial.println((buttonName + ' ' + "OnBtnPress(" + String(state).c_str() + ")").c_str());

	digitalWrite(relayPin, state);

}

//�������� publish tipic
void MqttButton::addTopic(string topic)
{
	_publishTopics.push_back(topic);
}

void MqttButton::setSender(Sender& sender)
{
	Serial.print("setSender");
	_sender = &sender;
}

//������ ������� ��������� ����
bool MqttButton::getState()
{
	return digitalRead(relayPin);
}