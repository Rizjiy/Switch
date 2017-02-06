#include <RBD_Timer.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "MikroTik48";
const char* password = "***";
const char *mqtt_server = "192.168.88.3"; // ����� ������� MQTT
const int mqtt_port = 1883; // ���� ��� ����������� � ������� MQTT
const char* clientName = "switch1";

WiFiClient wclient;
PubSubClient mqttclient(wclient);

int relayPin = 13;
int buttonPin = 12;

RBD::Timer reconnectTimer(60000); //����� ����� ������������ Wi-Fi
RBD::Timer debugTimer(3000); //3 sec ��� ����, ����� �� �������� ����
bool debug = true;

boolean rState1 = false;
boolean btnPress = false;
boolean lastbtnStat = false;

const char *topicSwitch = "home/switches/1";

void setup()
{
	Serial.begin(115200);

	//Mqtt setup
	mqttclient.setServer(mqtt_server, mqtt_port);
	mqttclient.setCallback(MqttCallback);

	pinMode(relayPin, OUTPUT);

	//digitalWrite(buttonPin, LOW);

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

	ButtonWf();

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
		Serial.print("Attempting MQTT connection...");
		// Attempt to connect
		//if (client.connect("ESP8266Client", mqtt_user, mqtt_pass))
		if (mqttclient.connect(clientName))
		{
			Serial.println("connected");
			// Once connected, publish an announcement...
			mqttclient.publish("Start", clientName);
			// ... and resubscribe
			mqttclient.subscribe(topicSwitch); // �������������� �� ����� � ������� ��� ����������
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

// ������� ��������� ������ �� �������
void MqttCallback(char* topic, byte* payload, unsigned int length) {
	Serial.print("MQTT message arrived [");
	Serial.print(topic);
	Serial.print("] ");
	for (int i = 0; i < length; i++) {
		Serial.print((char)payload[i]);
	}
	Serial.println();

	// �������� ��� ��������� ���� � ���������� �� ���������� �������� ������
	bool val = false;
	if (payload[0] == '1')
		val = true;
	else
		val = false;

	OnBtnPress(val);
}

// button without fixing, ������ ��� ��������
void ButtonWf() {
	btnPress = digitalRead(buttonPin);

	if (debug && debugTimer.onRestart())
	{
		Serial.print("btnPress=");
		Serial.println(btnPress);
		Serial.print("rState1=");
		Serial.println(rState1);
	}

	if (btnPress && !lastbtnStat) {
		delay(30); // ������ �� ��������
		btnPress = digitalRead(buttonPin);

		if (btnPress) {
			// ��������� ��������� ��������� ���� �� ������      
			if(!mqttclient.publish(topicSwitch, String(!rState1).c_str(), false))
				OnBtnPress(!rState1);
		}
	}
	lastbtnStat = btnPress;
}

void OnBtnPress(bool state)
{
	if (debug)
	{
		Serial.print("OnBtnPress(");
		Serial.print(state);
		Serial.println(")");
	}

	digitalWrite(relayPin, state);
	//������ ������� ���������
	rState1 = state;
}









