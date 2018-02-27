#include <RBD_Timer.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <Secret.h>

const char* ssid = WI_FI_SSID;
const char* password = WI_FI_PASSWORD;
const char *mqtt_server = MQTT_SERVER;
const int mqtt_port = MQTT_PORT; // ���� ��� ����������� � ������� MQTT
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

const float deltaHmax = 20; //% ���������
const float deltaHmin = 5; //% ���������

WiFiClient wclient;
PubSubClient mqttclient(wclient);

RBD::Timer reconnectTimer(60000); //����� ����� ������������ Wi-Fi

bool debug = true;

// ���������� ������
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
  
	// ������������ � wi-fi
	if (reconnectTimer.isExpired())
	{
		if (WifiConnect())
			if (MqttConnect())
			{
				//��� ��
				mqttclient.loop();
			}
			else
			{
				reconnectTimer.restart();
				//Mqtt �� �����������
			}
		else
		{
			reconnectTimer.restart();
			//Wi-fi �� �����������
		}
	}

	// �������� ����� �����������
	delay(5000);

	//��������� ���������
	float h0 = dht0.readHumidity();
	//float h1 = dht1.readHumidity();
	
	//// �������� ������ ������ �� ����������.
	//if (!isnan(h0) && !isnan(h1))
	//{
	//	//��� ��������� ���� �����, ����� ������� ���� ������ ��������
	//	if (h1 - h0 >= deltaHmax)
	//	{

	//	}

	//	//��� ���������� ���� �����, ����� ������� ���� ������ ��������
	//	if (h1 - h0 <= deltaHmin)
	//	{

	//	}
	//}


	// ��������� �����������
	float t0 = dht0.readTemperature();

	mqttclient.publish("home/holl/temperature", String(t0).c_str(), false);
	mqttclient.publish("home/holl/humidity", String(h0).c_str(), false);


	if (debug)
	{
		Serial.print("���������: ");
		Serial.print(h0);
		Serial.print(" %\t �����������: ");
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
			//mqttclient.subscribe(topicSwitch); // ������������� �� ����� � �������
			//mqttclient.subscribe(topicSwitchState); // ������������� �� ����� �� ��������

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



