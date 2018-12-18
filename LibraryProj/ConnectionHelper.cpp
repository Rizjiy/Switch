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
MqttButton* ConnectionHelper::_buttons[4]; //������� ������
byte ConnectionHelper::_buttonsCount = 0;

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

	//����������� callback ����� ��� ������ ��������
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
			mqttClient.subscribe(topicSubscribe.c_str()); // ������������� �� ������ ��� ����� ����������
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

	// ������������ � wi-fi
	if (_reconnectTimer.isExpired())
	{
		if (wifiConnect())
			if (mqttConnect())
			{
				//��� ��
				mqttClient.loop();
			}
			else
			{
				_reconnectTimer.restart();
				//Mqtt �� �����������
			}
		else
		{
			_reconnectTimer.restart();
			//Wi-fi �� �����������
		}
	}

}

void ConnectionHelper::addButton(MqttButton* button)
{
	_buttonsCount++;
	_buttons[_buttonsCount-1] = button;

	println("addButton " + button->deviceName);

	if (button->buttonPin >= 0) 
	{
		switch (_buttonsCount)
		{
		case 1:
			attachInterrupt(digitalPinToInterrupt(button->buttonPin), attachInterrupt1, button->levelButton ? FALLING : RISING);
			break;
		case 2:
			attachInterrupt(digitalPinToInterrupt(button->buttonPin), attachInterrupt2, button->levelButton ? FALLING : RISING);
			break;
		case 3:
			attachInterrupt(digitalPinToInterrupt(button->buttonPin), attachInterrupt3, button->levelButton ? FALLING : RISING);
			break;
		case 4:
			attachInterrupt(digitalPinToInterrupt(button->buttonPin), attachInterrupt4, button->levelButton ? FALLING : RISING);
			break;
		case 5:
			attachInterrupt(digitalPinToInterrupt(button->buttonPin), attachInterrupt5, button->levelButton ? FALLING : RISING);
			break;

		}
	}
}

void ConnectionHelper::attachInterrupt1()
{
	return _buttons[0]->interruptButtton();
}

void ConnectionHelper::attachInterrupt2()
{
	return _buttons[1]->interruptButtton();
}

void ConnectionHelper::attachInterrupt3()
{
	return _buttons[2]->interruptButtton();
}

void ConnectionHelper::attachInterrupt4()
{
	return _buttons[3]->interruptButtton();
}

void ConnectionHelper::attachInterrupt5()
{
	return _buttons[4]->interruptButtton();
}


void ConnectionHelper::println(string text)
{
	Serial.println(text.c_str());
}