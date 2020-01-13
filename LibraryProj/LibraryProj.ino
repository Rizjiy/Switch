/*
	Подсветка кухонной рабочей поверхности
*/
#include "home_ConnectionSettings.h"
#include "home_ConnectionHelper.h"
#include "home_MqttButton.h"
#include <Secret.h>

#include <string>
using namespace std;

//***Блок переменных
const char* ssid = "uAP";
const char* wifiPass = "66666666";
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
	"testproj"
);

ConnectionHelper helper(&settings);

MqttButton button1(14, 12, "mirror");
MqttButton button2(-1, 13, "fan1");
MqttButton button3(4, 5, "fan2");


// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(115200);
	helper.setup();

	button1.levelButton = HIGH;
	button1.levelTrigger = LOW;
	helper.addButton(&button1);

	button2.levelTrigger = LOW;
	helper.addButton(&button2);

	button3.levelButton = LOW;
	button3.levelTrigger = LOW;
	button3.isHoldButton = true;
	helper.addButton(&button3);

}

// the loop function runs over and over again until power down or reset
void loop() {
	helper.handle();
}
