// MqttButton.h

#ifndef _MQTTBUTTON_h
#define _MQTTBUTTON_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "home_Sender.h"
#include <RBD_Timer.h>
#include <string>
using namespace std;

class MqttButton {
public:
	MqttButton(byte buttonPin, byte relayPin, string buttonName, bool levelButton);
	void btnPress(bool state);
	void interruptButtton();
	void mqttCallback(char* topic, byte* payload, unsigned int length);
	void setup(); 
	void handle();
	void addTopic(string topic);
	void setSender(Sender& sender);
	bool getState();
	byte buttonPin = -1; //-1 - ��� ���������� ������
	byte relayPin;
	string buttonName;
	boolean levelButton = HIGH; // ������ � ���������� ��������� �� ������ ��� ������� �������
	string topicSwitch; //������� ����������� ����
	string topicSwitchState; //������� �������� ������� ����

	int lockTimout = 30;
	int lockTimout2 = 90;

private:
	vector<string> _publishTopics;
	Sender* _sender;
	volatile boolean _lock = false;
	volatile boolean _flagChange = false; // ���� ����� ��� ����, ����� ������������ ��������� �� ������

	RBD::Timer _lockTimer; // ������ �� �������� ��
	RBD::Timer _lockTimer2; // ������ �� �������� �����


};

#endif
