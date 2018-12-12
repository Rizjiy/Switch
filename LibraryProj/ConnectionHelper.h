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

#include <string>
using namespace std;

class ConnectionHelper {
public:
	ConnectionHelper(const char* ssid, const char* wifiPass, const char* mqttServer, const int mqttPort, const char* mqttUser, const char* mqttPass, string deviceName);
	void handle();
	//std::function<void(char*, uint8_t*, unsigned int)> MqttCallback;
	static void MqttCallback(char* topic, byte* payload, unsigned int length);

	RBD::Timer reconnectTimer; //пауза между реконнектами Wi-Fi

	WiFiClient wifiClient;
	PubSubClient mqttClient;
	string topicSubscribe;


private:
	bool wifiConnect();
	bool mqttConnect();

	const char* _ssid;
	const char* _wifiPass;
	const char* _mqttServer;
	int _mqttPort;
	const char* _mqttUser;
	const char* _mqttPass;

	string _deviceName;
	string _baseTopic = "home/switches";



};


#endif

