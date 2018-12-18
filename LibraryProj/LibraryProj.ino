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
//
//const string deviceName = "switch97";

const byte buttonPin = 14; //-1 - нет физической кнопки
const byte relayPin = 12;

const byte button2Pin = 13; //-1 - нет физической кнопки
const byte relay2Pin = 12;


boolean levelButton = HIGH; // Сигнал в нормальном состоянии на кнопке или датчике касания

RBD::Timer reconnectTimer(60000); //пауза между реконнектами Wi-Fi
RBD::Timer debugTimer(3000); //3 sec для того, чтобы не забивать эфир
RBD::Timer lockTimer(30); // защита от дребезга
RBD::Timer lockTimer2(90); // защита от дребезга
//**

WiFiClient wclient;
PubSubClient mqttclient(mqttServer, mqttPort, wclient);
ConnectionHelper helper(ssid, wifiPass, mqttclient, mqttUser, mqttPass, "switch97");
MqttButton button1(relayPin, buttonPin, "btn1", LOW);
MqttButton button2(relay2Pin, button2Pin, "btn2", LOW);
//ConnectionHelper helper;

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(115200);

	//helper.ssid = WI_FI_SSID;
	//helper.wifiPass = WI_FI_PASSWORD;
	//helper.mqttServer = MQTT_SERVER;
	//helper.mqttPort = MQTT_PORT;
	//helper.mqttUser = MQTT_USER;
	//helper.mqttPass = MQTT_PASSWORD;
	//helper.deviceName = "switch97";
	
	//helper.reconnectTimer = reconnectTimer;


	helper.setup();
	helper.addButton(&button1);
	helper.addButton(&button2);
}

// the loop function runs over and over again until power down or reset
void loop() {
	helper.handle();
}
