
#ifndef _ConnectionHelper_h
#define _ConnectionHelper_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "home_ConnectionSettings.h"
#include "MqttButton.h"

#include <RBD_Timer.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>


#include <string>
using namespace std;

class ConnectionHelper {
public:
	//ConnectionHelper();
	//ConnectionHelper(string deviceName);
	ConnectionHelper(ConnectionSettings* settings);
	void setup();
	void handle();
	void addButton(MqttButton* button);

	ConnectionSettings* settings;
	WiFiClient wclient;
	PubSubClient mqttClient;
	string topicSubscribe;
	int reconnectTimeout = 60000; //пауза между реконнектами Wi-Fi mc

private:
	bool wifiConnect();
	bool mqttConnect();
	static void println(string text);

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

