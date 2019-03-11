/*
	Датчик температуры и влажности в гостинной
*/
#include "home_ConnectionSettings.h"
#include "home_ConnectionHelper.h"
#include <Secret.h>

#include <DHT.h>
#include <ArduinoJson.h>


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
	"test"
);

ConnectionHelper helper(&settings);

DHT dht(5, DHT22);
const int sleepingTimeSecond = 20; //сколько спать
const int bufCount = 5;
const bool debug = true;
RBD::Timer reconnectTimer(sleepingTimeSecond * 1000);



int iter = 0;
unsigned long timing; // Переменная для хранения точки отсчета



// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(115200);
	helper.setup();

}

// the loop function runs over and over again until power down or reset
void loop() {
	helper.handle();
	//delay(sleepingTimeSecond * 1000);

	int i = bufCount;

	//float* tbuf = new float[bufCount];
	//float* hbuf = new float[bufCount];

	//while (i > 0)
	//{
	//	delay(1000);
	//	// Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
	//	float h = dht.readHumidity();
	//	// Read temperature as Celsius (the default)
	//	float t = dht.readTemperature();

	//	if (isnan(h) || isnan(t))
	//	{
	//		Serial.println("ERROR: Failed to read from DHT sensor!");
	//		continue;
	//	}
	//	else
	//	{
	//		i--;
	//		tbuf[i] = t;
	//		hbuf[i] = h;
	//		if (debug)
	//		{
	//			Serial.println(h);
	//			Serial.println(t);
	//		}

	//	}
	//}

	if (reconnectTimer.isExpired())
	{
		reconnectTimer.restart();
		iter++;
		Serial.println(iter);
		string str("iter=");
		helper.sender.publish(settings.deviceName, str + String(iter).c_str(), true);
	}

}
