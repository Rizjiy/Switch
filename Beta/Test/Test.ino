/*
 Name:		Test.ino
 Created:	12/8/2018 6:21:40 PM
 Author:	rodal
*/

#include <Secret.h>

#include <RBD_Timer.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "MqttHelper.h"

#include <string>
using namespace std;

//***���� ����������
const char* ssid = WI_FI_SSID;
const char* wifiPass = WI_FI_PASSWORD;
const char* mqttServer = MQTT_SERVER;
const int mqttPort = MQTT_PORT; // ���� ��� ����������� � ������� MQTT
const char* mqttUser = MQTT_USER;
const char* mqttPass = MQTT_PASSWORD;

const string deviceName = "switch4";

const int buttonPin = 14; //-1 - ��� ���������� ������
const int mainPin = 12;

boolean levelButton = HIGH; // ������ � ���������� ��������� �� ������ ��� ������� �������

RBD::Timer reconnectTimer(60000); //����� ����� ������������ Wi-Fi
RBD::Timer debugTimer(3000); //3 sec ��� ����, ����� �� �������� ����
RBD::Timer lockTimer(30); // ������ �� ��������
RBD::Timer lockTimer2(90); // ������ �� ��������
//**



//class MqttButton {
//public:
//	MqttButton(byte relayPin, byte buttonPin, string string);
//private:
//	byte _relayPin;
//	byte _buttonPin;
//	string deviceName;
//	WiFiClient wifiClient;
//	PubSubClient mqttClient;
//
//
//};
//
//MqttButton::MqttButton(byte relayPin, byte buttonPin, string deviceName) {
//	_relayPin = relayPin;
//	_buttonPin = buttonPin;
//	deviceName = deviceName;
//
//	pinMode(_relayPin, OUTPUT);
//
//}



MqttHelper helper(ssid, wifiPass, mqttServer, mqttPort, mqttUser, mqttPass, "switch97");

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(115200);

}

// the loop function runs over and over again until power down or reset
void loop() {
	helper.handle();

}

