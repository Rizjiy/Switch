/*
	Подсветка растений на кухне и свет над столиком
*/
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
	"plants"
);

ConnectionHelper helper(&settings);

MqttButton button1(-1, 12, "btn1", HIGH);
MqttButton button2(-1, 14, "btn2", HIGH);


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
