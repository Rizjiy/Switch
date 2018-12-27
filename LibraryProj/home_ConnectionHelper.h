
#ifndef _ConnectionHelper_h
#define _ConnectionHelper_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "home_ConnectionSettings.h"
#include "home_MqttButton.h"
#include "home_Sender.h"

#include <RBD_Timer.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>


#include <string>
using namespace std;

class ConnectionHelper {
public:
	ConnectionHelper(ConnectionSettings* settings);
	void setup();
	void handle();
	void addButton(MqttButton* button);

	Sender sender;
	ConnectionSettings* settings;
	WiFiClient wclient;
	PubSubClient mqttClient;
	string topicSubscribe;
	int reconnectTimeout = 60000; //����� ����� ������������ Wi-Fi mc

private:
	bool wifiConnect();
	bool mqttConnect();

	RBD::Timer _reconnectTimer;

	static MqttButton* _buttons[];
	static byte _buttonsCount;
	static void attachInterrupt1();
	static void attachInterrupt2();
	static void attachInterrupt3();
	static void attachInterrupt4();
	static void attachInterrupt5();


};


#endif

