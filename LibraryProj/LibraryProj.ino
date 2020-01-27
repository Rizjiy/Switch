/*
	Подсветка кухонной рабочей поверхности
*/
#include "home_ConnectionSettings.h"
#include "home_ConnectionHelper.h"
#include "home_MqttButton.h"
#include <Secret.h>
#include <string>

#include <DHT.h>
#include <ArduinoJson.h>

using namespace std;

//***Блок переменных
const char* ssid = WI_FI_SSID;
const char* wifiPass = WI_FI_PASSWORD;
const char* mqttServer = MQTT_SERVER;
const int mqttPort = MQTT_PORT;
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

//dht
DHT dht1(0, DHT22);
DHT dht2(2, DHT22);
RBD::Timer dhtRepeatTimer(15 * 1000);
RBD::Timer dhtPulseTimer(2 * 1000);
const int bufCount = 5;
float t1buf[bufCount];
float h1buf[bufCount];
float t2buf[bufCount];
float h2buf[bufCount];
const bool debug = true;

int i = 0;


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

	if (dhtRepeatTimer.isExpired())
	{
		if (dhtPulseTimer.isExpired())
		{
			//do this
			// Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
			float h1 = dht1.readHumidity();
			// Read temperature as Celsius (the default)
			float t1 = dht1.readTemperature();

			if (isnan(h1) || isnan(t1))
			{
				Serial.println("ERROR: Failed to read from DHT sensor!");
				//todo: ?
			}
			else
			{
				h1buf[i] = h1;
				t1buf[i] = t1;
				if (debug)
				{
					Serial.println(h1);
					Serial.println(t1);
				}

				i++;
			}


			dhtPulseTimer.restart();

			if (i >= bufCount)
			{
				sort(t1buf);
				sort(h1buf);

				publishData(t1buf[bufCount / 2 + 1], h1buf[bufCount / 2 + 1], "bathroom");

				//compare values

				i = 0;
				dhtRepeatTimer.restart();

			}
		}

	}
}

void sort(float a[])
{
	float temp = 0;
	for (int i = 0; i < bufCount; i++)
	{
		for (int j = 0; j < bufCount; j++)
		{
			if (a[i] > a[j])
			{
				temp = a[i];
				a[i] = a[j];
				a[j] = temp;
			}
		}
	}
}

// function called to publish the temperature and the humidity
void publishData(float p_temperature, float p_humidity, string roomName) {
	// create a JSON object
	// doc : https://github.com/bblanchon/ArduinoJson/wiki/API%20Reference
	//deserialize json
	StaticJsonDocument<200> doc;
	doc["temperature"] = (String)p_temperature;
	doc["humidity"] = (String)p_humidity;

	//doc[(string("temperature_") + roomName).c_str()] = (String)p_temperature;
	//doc[(string("humidity_") +roomName).c_str()] = (String)p_humidity;
	//serializeJsonPretty(doc, Serial); //unfortunately it is not work
	/*
	{
	"temperature": "23.20" ,
	"humidity": "43.70"
	}
	*/
	char data[200];
	serializeJson(doc, data);
	helper.sender.publish(settings.topicBase + "/" + settings.deviceName, data, true);

}

