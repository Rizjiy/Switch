// MqttButton.h

#ifndef _MQTTBUTTON_h
#define _MQTTBUTTON_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "home_MqttButtonBase.h"
#include "home_Sender.h"
#include <RBD_Timer.h>
#include <vector>
#include <string>
using namespace std;

class MqttButton {
public:
	MqttButton(byte buttonPin, byte relayPin, string buttonName);
	void btnPress();
	void interruptButtton();
	void mqttCallback(char* topic, byte* payload, unsigned int length);
	void setup();
	void handle();
	bool getState();
	void addTopic(string topic);
	void setSender(Sender& sender);
	byte buttonPin = -1; //-1 - нет физической кнопки
	byte relayPin;
	string buttonName;
	bool levelButton = LOW; // Сигнал в нормальном состоянии на кнопке или датчике касания
	bool levelTrigger = HIGH; //сигнал срабатывания реле
	string topicSwitch; //команда преключения реле
	string topicSwitchState; //команда проверки статуса реле

	int lockTimout = 30;
	int lockTimout2 = 90;

private:
	volatile boolean _lock = false;
	vector<string> _publishTopics;
	Sender* _sender;

	volatile boolean _flagChange = false; // Флаг нужен для того, чтобы опубликовать сообщение на брокер
	void relaySwitch(bool state);

	RBD::Timer _lockTimer; // защита от дребезга до
	RBD::Timer _lockTimer2; // защита от дребезга после

};

#endif

