// MqttButton.h

#ifndef _MQTTBUTTON_h
#define _MQTTBUTTON_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <RBD_Timer.h>
#include <string>
using namespace std;

class MqttButton {
public:
	MqttButton(byte relayPin, byte buttonPin, string deviceName);
	void Interrupt_WF();
	void OnBtnPress(bool state);

	byte buttonPin = -1; //-1 - ��� ���������� ������
	byte relayPin;
	string deviceName;

	boolean levelButton = HIGH; // ������ � ���������� ��������� �� ������ ��� ������� �������

	RBD::Timer lockTimer; // ������ �� �������� ��
	RBD::Timer lockTimer2; // ������ �� �������� �����

private:

	volatile boolean _lock = false;
	volatile boolean _rState = false; // � ����������� ������ ���������� ��� volatile ��� ���������� ����������
	volatile boolean _flagChange = false; // ���� ����� ��� ����, ����� ������������ ��������� �� ������


};

#endif

