#include <RBD_Timer.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <Secret.h>

const char* ssid = WI_FI_SSID;
const char* password = WI_FI_PASSWORD;
const char *mqtt_server = MQTT_SERVER;
const int mqtt_port = MQTT_PORT; // Порт для подключения к серверу MQTT
const char* mqttUser = MQTT_USER;
const char* mqttPass = MQTT_PASSWORD;

const char* clientName = "fan";
//const char *topicSwitch = "home/fans/1";
//const char *topicSwitchState = "home/fans/1/status";

const int dhtPin0 = 5;
//const int dhtPin1 = 5;
//const int dhtPin2 = 15;
//
//const int relayPin1 = 13; //for fun1

const float deltaHmax = 20; //% влажности
const float deltaHmin = 5; //% влажности

WiFiClient wclient;
PubSubClient mqttclient(wclient);

RBD::Timer reconnectTimer(60000); //пауза между реконнектами Wi-Fi

bool debug = true;

// Инициируем датчик
DHT dht0(dhtPin0, DHT22);
//DHT dht1(dhtPin1, DHT22);
//DHT dht3(dhtPin2, DHT22);

void setup() 
{

	Serial.begin(115200);

	//Mqtt setup
	mqttclient.setServer(mqtt_server, mqtt_port);
	//mqttclient.setCallback(MqttCallback);

	if (WifiConnect())
		MqttConnect();
}

void loop() 
{
  
	// подключаемся к wi-fi
	if (reconnectTimer.isExpired())
	{
		if (WifiConnect())
			if (MqttConnect())
			{
				//Все ок
				mqttclient.loop();
			}
			else
			{
				reconnectTimer.restart();
				//Mqtt не подключился
			}
		else
		{
			reconnectTimer.restart();
			//Wi-fi не подключился
		}
	}

	// Задержка между измерениями
	delay(5000);

	//Считываем влажность
	float h0 = dht0.readHumidity();
	//float h1 = dht1.readHumidity();
	
	//// Проверка удачно прошло ли считывание.
	//if (!isnan(h0) && !isnan(h1))
	//{
	//	//для включения реле нужно, чтобы разница была больше заданной
	//	if (h1 - h0 >= deltaHmax)
	//	{

	//	}

	//	//для выключения реле нужно, чтобы разница была меньше заданной
	//	if (h1 - h0 <= deltaHmin)
	//	{

	//	}
	//}


	// Считываем температуру
	float t0 = dht0.readTemperature();

	mqttclient.publish("home/holl/temperature", String(t0).c_str(), false);
	mqttclient.publish("home/holl/humidity", String(h0).c_str(), false);


	if (debug)
	{
		Serial.print("Влажность: ");
		Serial.print(h0);
		Serial.print(" %\t Температура: ");
		Serial.print(t0);
		Serial.print(" *C ");
		Serial.println();
	}
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
			//mqttclient.subscribe(topicSwitch); // подписываемся нв топик с данными
			//mqttclient.subscribe(topicSwitchState); // подписываемся на топик со статусом

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



