/*
 Name:		ConnectionHelper.cpp
 Created:	12/11/2018 12:30:47 PM
 Author:	lukich
 Editor:	http://www.visualmicro.com
*/

#include "ConnectionHelper.h"
#include "Arduino.h"

#include <RBD_Timer.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <string>
using namespace std;


ConnectionHelper::ConnectionHelper(const char* ssid, const char* wifiPass, const char* mqttServer, const int mqttPort, const char* mqttUser, const char* mqttPass, string deviceName)
	:wifiClient(), mqttClient(wifiClient)
{
	_ssid = ssid;
	_wifiPass = wifiPass;
	_mqttServer = mqttServer;
	_mqttPort = mqttPort;
	_mqttUser = mqttUser;
	_mqttPass = mqttPass;

	_deviceName = deviceName;

	RBD::Timer reconnectTimer(60000); //пауза между реконнектами Wi-Fi

	//Mqtt setup
	mqttClient.setServer(mqttServer, mqttPort);
	mqttClient.setCallback(MqttCallback);


	topicSubscribe = _baseTopic + "/" + _deviceName + "/#";		// home/switches/switch5/#
}

bool ConnectionHelper::wifiConnect()
{
	if (WiFi.status() != WL_CONNECTED) {
		Serial.print("Connecting to ");
		Serial.print(_ssid);
		Serial.println("...");
		WiFi.begin(_ssid, _wifiPass);

		if (WiFi.waitForConnectResult() != WL_CONNECTED)
			return false;
		Serial.println("WiFi connected");
	}

	return true;
}

bool ConnectionHelper::mqttConnect()
{
	// Loop until we're reconnected
	if (!mqttClient.connected())
	{
		Serial.print("Attempting MQTT connection to ");
		Serial.print(_mqttServer);
		Serial.println("...");
		// Attempt to connect
		//if (client.connect("ESP8266Client", mqtt_user, mqtt_pass))
		if (mqttClient.connect(_deviceName.c_str(), _mqttUser, _mqttPass))
		{
			Serial.println("connected");
			// Once connected, publish an announcement...
			string startPayload = _deviceName + ' ' + String(wifiClient.localIP()).c_str();
			mqttClient.publish("Start", startPayload.c_str());
			// ... and resubscribe
			mqttClient.subscribe(topicSubscribe.c_str()); // подписываемся на топики для этого устройства
		}
		else
		{
			Serial.print("failed, rc=");
			Serial.println(mqttClient.state());
			return false;
		}
	}

	return true;
}

void ConnectionHelper::handle() {

	// подключаемся к wi-fi
	if (reconnectTimer.isExpired())
	{
		if (wifiConnect())
			if (mqttConnect())
			{
				//Все ок
				mqttClient.loop();
			}
			else
			{
				reconnectTimer.restart();
				//Mqtt не подключился
			}
		else
		{
			reconnectTimer.restart();
			//Wi-fi не подключился
		}
	}

}

// Функция получения данных от сервера
void ConnectionHelper::MqttCallback(char* topic, byte* payload, unsigned int length) {
	Serial.print("MQTT message arrived [");
	Serial.print(topic);
	Serial.print("] ");
	for (int i = 0; i < length; i++) {
		Serial.print((char)payload[i]);
	}
	Serial.println();

}
