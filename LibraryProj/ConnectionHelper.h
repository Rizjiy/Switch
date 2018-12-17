/*
 Name:		ConnectionHelper.h
 Created:	12/11/2018 12:30:47 PM
 Author:	lukich
 Editor:	http://www.visualmicro.com
*/

#ifndef _ConnectionHelper_h
#define _ConnectionHelper_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <Arduino.h>
#include <RBD_Timer.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "MqttButton.h"

#include <string>
using namespace std;

class ConnectionHelper {
public:
	//ConnectionHelper();
	//ConnectionHelper(string deviceName);
	ConnectionHelper(const char* ssid, const char* wifiPass, PubSubClient& mqttClient, const char* mqttUser, const char* mqttPass, string deviceName);
	void setup();
	void handle();
	void addButton(MqttButton* button);

	const char* ssid;
	const char* wifiPass;
	string deviceName;
	string topicBase = "home/switches";
	PubSubClient mqttClient;
	const char* mqttServer;
	int mqttPort;
	const char* mqttUser;
	const char* mqttPass;
	string topicSubscribe;
	int reconnectTimeout = 60000; //пауза между реконнектами Wi-Fi mc

	static MqttButton* buttons;
	static void attachButtonInterrupt();
private:
	bool wifiConnect();
	bool mqttConnect();

	RBD::Timer _reconnectTimer;


};


#endif

