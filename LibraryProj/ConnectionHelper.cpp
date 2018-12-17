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
#include "MqttButton.h"

#include <string>
using namespace std;
//ConnectionHelper::ConnectionHelper() {}
//
//ConnectionHelper::ConnectionHelper(string deviceName)
//{
//	this->deviceName = deviceName;
//}


ConnectionHelper::ConnectionHelper(const char* ssid, const char* wifiPass, PubSubClient& mqttClient, const char* mqttUser, const char* mqttPass, string deviceName)
{
	this->ssid = ssid;
	this->wifiPass = wifiPass;
	this->deviceName = deviceName;

	this->mqttClient = mqttClient;
	this->mqttUser = mqttUser;
	this->mqttPass = mqttPass;

	//Mqtt setup
	/*WiFiClient wclient;
	mqttClient.setClient(wclient);
	mqttClient.setServer(mqttServer, mqttPort);*/

}

void ConnectionHelper::setup()
{
	_reconnectTimer.setTimeout(reconnectTimeout);

	//if (!&mqttClient)
	//{
	//	Serial.println("create mqttClient");
	//	WiFiClient wclient;
	//	PubSubClient mqttclient(mqttServer, mqttPort, wclient);
	//}

	//подписываем callback таким вот хитрым способом
	MQTT_CALLBACK_SIGNATURE(
		[this](char* topic, byte* payload, unsigned int length)
	{
		Serial.print("MQTT message arrived [");
		Serial.print(topic);
		Serial.print("] ");
		for (int i = 0; i < length; i++) {
			Serial.print((char)payload[i]);
		}
		Serial.println();
	}
	);

	mqttClient.setCallback(callback);

	topicSubscribe = topicBase + "/" + deviceName + "/#";		// home/switches/switch5/#

}

bool ConnectionHelper::wifiConnect()
{
	if (WiFi.status() != WL_CONNECTED) {
		Serial.print("Connecting to ");
		Serial.print(WiFi.SSID());
		Serial.println("...");
		WiFi.begin(ssid, wifiPass);

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
		Serial.print(mqttServer);
		Serial.println("...");
		// Attempt to connect
		if (mqttClient.connect(deviceName.c_str(), mqttUser, mqttPass))
		{
			Serial.println("connected");
			// Once connected, publish an announcement...
			mqttClient.publish("Start", deviceName.c_str());
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
	if (_reconnectTimer.isExpired())
	{
		if (wifiConnect())
			if (mqttConnect())
			{
				//Все ок
				mqttClient.loop();
			}
			else
			{
				_reconnectTimer.restart();
				//Mqtt не подключился
			}
		else
		{
			_reconnectTimer.restart();
			//Wi-fi не подключился
		}
	}

}

MqttButton* ConnectionHelper::buttons;

void ConnectionHelper::addButton(MqttButton* button)
{
	buttons = button;

	if(button->buttonPin>=0)
		attachInterrupt(digitalPinToInterrupt(button->buttonPin), attachButtonInterrupt, button->levelButton ? FALLING : RISING);

}

void ConnectionHelper::attachButtonInterrupt()
{
	return buttons->interruptButtton();
}