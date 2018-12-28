#include "home_Sender.h"
#include "home_ConnectionSettings.h"
#include "home_ConnectionHelper.h"
#include "home_MqttButton.h"
#include <Secret.h>

#include <string>
using namespace std;

//***Блок переменных
const char* ssid = WI_FI_SSID;
const char* wifiPass = WI_FI_PASSWORD;
const char* mqttServer = MQTT_SERVER;
const int mqttPort = MQTT_PORT; // Порт для подключения к серверу MQTT
const char* mqttUser = MQTT_USER;
const char* mqttPass = MQTT_PASSWORD;

ConnectionSettings settings(
	ssid,
	wifiPass,
	mqttServer,
	mqttPort,
	mqttUser,
	mqttPass,
	"switch97"
);

ConnectionHelper helper(&settings);

MqttButton button1(14, 12, "btn1");
//MqttButton button2(13, 12, "btn2");
//**

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(115200);

	button1.levelTrigger = LOW;

	helper.addButton(&button1);
	//helper.addButton(&button2);

	helper.setup();

}

// the loop function runs over and over again until power down or reset
void loop() {
	helper.handle();
}
