#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <RBD_Timer.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <Secret.h>

const char* ssid = WI_FI_SSID;
const char* password = WI_FI_PASSWORD;
const char *mqtt_server = MQTT_SERVER;
const int mqtt_port = MQTT_PORT; // Порт для подключения к серверу MQTT
const char* mqttUser = MQTT_USER;
const char* mqttPass = MQTT_PASSWORD;


const char* clientName = "dht1";
const char* topic = "home/dht/1";
const int dhtPin = 5;
const int sleepingTimeSecond = 120; //сколько спать
const int bufCount = 5;

WiFiClient wclient;
PubSubClient mqttclient(wclient);
DHT dht(dhtPin, DHT22);

bool debug = true;

// the setup function runs once when you press reset or power the board
void setup() {

	Serial.begin(115200);

	//Mqtt setup
	mqttclient.setServer(mqtt_server, mqtt_port);
	
	if (WifiConnect())
		MqttConnect();
}

// the loop function runs over and over again until power down or reset
void loop() 
{
	// подключаемся к wi-fi
	if (!WifiConnect() && !MqttConnect())
	{
		delay(60000);
		return;
	}
	//mqttclient.loop(); //todo: what

	int i= bufCount;

	float* tbuf = new float[bufCount];
	float* hbuf = new float[bufCount];

	while(i > 0)
	{
		delay(1000);
		// Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
		float h = dht.readHumidity();
		// Read temperature as Celsius (the default)
		float t = dht.readTemperature();

		if (isnan(h) || isnan(t))
		{
			Serial.println("ERROR: Failed to read from DHT sensor!");
			continue;
		}
		else
		{
			i--;
			tbuf[i] = t;
			hbuf[i] = h;
			if (debug)
			{
				Serial.println(h);
				Serial.println(t);
			}

		}
	}

	sort(tbuf);
	sort(hbuf);

	publishData(tbuf[bufCount/2], hbuf[bufCount/2]);

	delete[] tbuf;
	delete[] hbuf;

	mqttclient.disconnect();
	WiFi.disconnect();

	//ESP.deepSleep(sleepingTimeSecond * 1000000, WAKE_RF_DEFAULT);
	//delay(500); // wait for deep sleep to happen
	delay(sleepingTimeSecond * 1000);

}

bool WifiConnect()
{
	if (WiFi.status() != WL_CONNECTED) {
		Serial.print("Connecting to ");
		Serial.print(ssid);
		Serial.println("...");
		WiFi.begin(ssid, password);

		if (WiFi.waitForConnectResult() != WL_CONNECTED)
			return false;
		Serial.println("WiFi connected");
	}

	return true;
}

bool MqttConnect()
{
	// Loop until we're reconnected
	if (!mqttclient.connected())
	{
		Serial.print("Attempting MQTT connection to ");
		Serial.print(mqtt_server);
		Serial.println("...");
		// Attempt to connect
		//if (client.connect("ESP8266Client", mqtt_user, mqtt_pass))
		if (mqttclient.connect(clientName, mqttUser, mqttPass))
		{
			Serial.println("connected");
			// Once connected, publish an announcement...
			mqttclient.publish("Start", clientName);
			// ... and resubscribe

		}
		else
		{
			Serial.print("failed, rc=");
			Serial.println(mqttclient.state());
			return false;
		}
	}
	
	return true;
}


// function called to publish the temperature and the humidity
void publishData(float p_temperature, float p_humidity) {
	// create a JSON object
	// doc : https://github.com/bblanchon/ArduinoJson/wiki/API%20Reference
	StaticJsonBuffer<200> jsonBuffer;
	JsonObject& root = jsonBuffer.createObject();
	// INFO: the data must be converted into a string; a problem occurs when using floats...
	root["temperature"] = (String)p_temperature;
	root["humidity"] = (String)p_humidity;
	root.prettyPrintTo(Serial);
	Serial.println("");
	/*
	{
	"temperature": "23.20" ,
	"humidity": "43.70"
	}
	*/
	char data[200];
	root.printTo(data, root.measureLength() + 1);
	mqttclient.publish(topic, data, true);
	yield();
}

void sort(float a[])
{
	float temp = 0;
	for (int i = 0; i<bufCount; i++)
	{
		for (int j = 0; j<bufCount; j++)
		{
			if (a[i]>a[j])
			{
				temp = a[i];
				a[i] = a[j];
				a[j] = temp;
			}
		}
	}

}