// MqttButton.h

#ifndef _MQTTBUTTON_h
#define _MQTTBUTTON_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

//#include "ConnectionHelper.h"
#include <RBD_Timer.h>
#include <string>
using namespace std;

class MqttButton {
public:
	MqttButton(byte buttonPin, byte relayPin, string buttonName, bool levelButton);
	void interruptButtton();
	void btnPress(bool state);
	void setup(); 
	void handle();
	byte buttonPin = -1; //-1 - ��� ���������� ������
	byte relayPin;
	string buttonName;
	//ConnectionHelper* connectionHelper;
	boolean levelButton = HIGH; // ������ � ���������� ��������� �� ������ ��� ������� �������

	int lockTimout = 30;
	int lockTimout2 = 90;

private:
	volatile boolean _lock = false;
	volatile boolean _rState = false; // � ����������� ������ ���������� ��� volatile ��� ���������� ����������
	volatile boolean _flagChange = false; // ���� ����� ��� ����, ����� ������������ ��������� �� ������

	RBD::Timer _lockTimer; // ������ �� �������� ��
	RBD::Timer _lockTimer2; // ������ �� �������� �����


};

#endif

