
#include "home_ConnectionHelper.h"
#include "home_ConnectionSettings.h"
#include "MqttButton.h"

#include "Arduino.h"
#include <RBD_Timer.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>

#include <string>
using namespace std;

MqttButton* ConnectionHelper::_buttons[4]; //макимум кнопок
byte ConnectionHelper::_buttonsCount = 0;

ConnectionHelper::ConnectionHelper(ConnectionSettings* settings)
	:wclient(), mqttClient(settings->mqttServer, settings->mqttPort, wclient)
{
	this->settings = settings;

	//почему-то не получилось создать здесь :(
	//WiFiClient wclient;
	//mqttClient = new PubSubClient(settings->mqttServer, settings->mqttPort, wclient);



}

void ConnectionHelper::setup()
{
	ArduinoOTA.setHostname(settings->deviceName.c_str());
	ArduinoOTA.begin();

	_reconnectTimer.setTimeout(reconnectTimeout);

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

	topicSubscribe = settings->topicBase + "/" + settings->deviceName + "/#";		// home/switches/switch5/#

}

bool ConnectionHelper::wifiConnect()
{
	if (WiFi.status() != WL_CONNECTED) {
		Serial.print("Connecting to ");
		Serial.print(settings->ssid);
		Serial.println("...");
		WiFi.begin(settings->ssid, settings->wifiPass);

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
		Serial.print(settings->mqttServer);
		Serial.println("...");
		// Attempt to connect
		if (mqttClient.connect(settings->deviceName.c_str(), settings->mqttUser, settings->mqttPass))
		{
			Serial.println("connected");
			// Once connected, publish an announcement...
			mqttClient.publish("Start", settings->deviceName.c_str());
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

	ArduinoOTA.handle();

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

void ConnectionHelper::addButton(MqttButton* button)
{
	_buttonsCount++;
	_buttons[_buttonsCount-1] = button;
	//button->connectionHelper = this;

	println("addButton " + button->buttonName);

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