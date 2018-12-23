
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
	Serial.print("Sender::publish: ");
	Serial.println(topic.c_str());
	mqttClient->publish(topic.c_str(), payload, retained);
}

void Sender::print(string message)
{
	Serial.println(message.c_str());
}


