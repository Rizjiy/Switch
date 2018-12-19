#include "MqttButton.h"
#include <Secret.h>

#include <RBD_Timer.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "ConnectionHelper.h"
#include "MqttButton.h"

#include <string>
using namespace std;

//***Блок переменных
const char* ssid = WI_FI_SSID;
const char* wifiPass = WI_FI_PASSWORD;
const char* mqttServer = MQTT_SERVER;
const int mqttPort = MQTT_PORT; // Порт для подключения к серверу MQTT
const char* mqttUser = MQTT_USER;
const char* mqttPass = MQTT_PASSWORD;

WiFiClient wclient;
PubSubClient mqttclient(mqttServer, mqttPort, wclient);
ConnectionHelper helper(ssid, wifiPass, mqttclient, mqttUser, mqttPass, "switch97");

MqttButton button1(14, 12, "btn1", LOW);
MqttButton button2(13, 12, "btn2", LOW);
//**

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(115200);

	helper.setup();
	helper.addButton(&button1);
	helper.addButton(&button2);
}

// the loop function runs over and over again until power down or reset
void loop() {
	helper.handle();
}
