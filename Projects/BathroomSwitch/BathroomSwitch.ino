/*	WeMos D1 R2 & mini
	���� � ������ ��� ��������
*/
#include "home_ConnectionSettings.h"
#include "home_ConnectionHelper.h"
#include "home_MqttButton.h"
#include <Secret.h>

#include <string>
using namespace std;

//***���� ����������
const char* ssid = WI_FI_SSID;
const char* wifiPass = WI_FI_PASSWORD;
const char* mqttServer = MQTT_SERVER;
const int mqttPort = MQTT_PORT; // ���� ��� ����������� � ������� MQTT
const char* mqttUser = MQTT_USER;
const char* mqttPass = MQTT_PASSWORD;

ConnectionSettings settings(
	ssid,
	wifiPass,
	mqttServer,
	mqttPort,
	mqttUser,
	mqttPass,
	"bathroom/switch"
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

	button3.levelTrigger = LOW;
	button3.levelTrigger = LOW;
	helper.addButton(&button3);

}

// the loop function runs over and over again until power down or reset
void loop() {
	helper.handle();
}
