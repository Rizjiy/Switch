
#include "home_Sender.h"
#include <PubSubClient.h>

#include <string>
using namespace std;

Sender::Sender(PubSubClient& mqttClient)
{
	this->mqttClient = &mqttClient;
}

void Sender::publish(string topic, const char* payload, boolean retained)
{
	print("Sender::publish: " + topic + " [" + payload + "]");
	mqttClient->publish(topic.c_str(), payload, retained);
}

void Sender::publish(string topic, bool payload, boolean retained)
{
	print("Sender::publish: " + topic + " [" + String(payload).c_str() + "]");
	const char* lpayload = String(payload).c_str();
	mqttClient->publish(topic.c_str(), lpayload, retained);
	//mqttClient->publish(topic.c_str(), "1", retained);
}


void Sender::print(string message)
{
	Serial.println(message.c_str());
}


