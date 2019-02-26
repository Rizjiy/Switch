// MqttButtonBase.h

#ifndef _MQTTBUTTONBASE_h
#define _MQTTBUTTONBASE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "home_Sender.h"
#include <RBD_Timer.h>
#include <vector>
#include <string>
using namespace std;

class MqttButtonBase {
public:
	MqttButtonBase(byte buttonPin, byte relayPin, string buttonName);
	virtual void btnPress() = 0;
	void interruptButtton();
	virtual void mqttCallback(char* topic, byte* payload, unsigned int length) = 0;
	virtual void setup();
	virtual void handle();
	void addTopic(string topic);
	void setSender(Sender& sender);
	bool getState();
	byte buttonPin = -1; //-1 - нет физической кнопки
	byte relayPin;
	string buttonName;
	bool levelButton = LOW; // Сигнал в нормальном состоянии на кнопке или датчике касания
	bool levelTrigger = HIGH; //сигнал срабатывания реле
	string topicSwitch; //команда преключения реле
	string topicSwitchState; //команда проверки статуса реле

	int lockTimout = 30;
	int lockTimout2 = 90;

protected:
	volatile boolean _lock = false;
	vector<string> _publishTopics;
	Sender* _sender;
	void relaySwitch(bool state);

private:
	volatile boolean _flagChange = false; // Флаг нужен для того, чтобы опубликовать сообщение на брокер
	RBD::Timer _lockTimer; // защита от дребезга до
	RBD::Timer _lockTimer2; // защита от дребезга после

};


#endif

