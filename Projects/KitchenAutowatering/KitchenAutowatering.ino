/*	WeMos D1 R2 & mini
	Автополив на кухне
*/

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
	"kitchen/autowaterng1"
);

ConnectionHelper helper(&settings);

MqttButton button1(12, 14, "btn1");


void setup()
{
	Serial.begin(115200);
	helper.setup();
	button1.levelButton = HIGH;
	helper.addButton(&button1);

}

void loop()
{
	helper.handle();
}



