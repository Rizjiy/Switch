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
	MqttButton(byte relayPin, byte buttonPin, string deviceName, bool levelButton);
	void interruptButtton();
	void btnPress(bool state);
	void setup(); 
	byte buttonPin = -1; //-1 - нет физической кнопки
	byte relayPin;
	string deviceName;

	boolean levelButton = HIGH; // —игнал в нормальном состо€нии на кнопке или датчике касани€

	int lockTimout = 30;
	int lockTimout2 = 90;

private:
	volatile boolean _lock = false;
	volatile boolean _rState = false; // ¬ прерывани€х всегда используем тип volatile дл€ измен€емых переменных
	volatile boolean _flagChange = false; // ‘лаг нужен дл€ того, чтобы опубликовать сообщение на брокер

	RBD::Timer _lockTimer; // защита от дребезга до
	RBD::Timer _lockTimer2; // защита от дребезга после


};

#endif

