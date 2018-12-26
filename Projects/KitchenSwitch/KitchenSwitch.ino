/*
	��������� �������� ������� �����������
*/
#include "home_Sender.h"
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
	"kitchen"
);

ConnectionHelper helper(&settings);

MqttButton button1(13, 14, "btn1", HIGH);


// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(115200);
	helper.setup();

	button1.lockTimout2 = 180;
	button1.addTopic("home/switches/switch2/pins/14");
	helper.addButton(&button1);

}

// the loop function runs over and over again until power down or reset
void loop() {
	helper.handle();
}
