#include <RBD_Timer.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Secret.h>


const char* ssid = WI_FI_SSID;
const char* password = WI_FI_PASSWORD;
const char *mqtt_server = MQTT_SERVER;
const int mqtt_port = MQTT_PORT; // ���� ��� ����������� � ������� MQTT
const char* mqttUser = MQTT_USER;
const char* mqttPass = MQTT_PASSWORD;

const char* clientName = "kitchen/autowaterng";
const char *topicSwitch = "home/kitchen/autowatering/btn1";
const char *topicSwitchState = "home/kitchen/autowatering/btn1/state";

const int buttonPin = 12;
const int relayPin = 14;

boolean levelTrigger = HIGH;
boolean levelButton = HIGH; // ������ � ���������� ��������� �� ������ ��� ������� �������

WiFiClient wclient;
PubSubClient mqttclient(wclient);

RBD::Timer reconnectTimer(60000); //����� ����� ������������ Wi-Fi
RBD::Timer debugTimer(3000); //3 sec ��� ����, ����� �� �������� ����
RBD::Timer lockTimer(30); // ������ �� ��������
RBD::Timer lockTimer2(90); // ������ �� ��������
bool debug = true;

volatile bool lock = false;
volatile boolean rState = false; // � ����������� ������ ���������� ��� volatile ��� ���������� ����������
volatile boolean flagChange = false; // ���� ����� ��� ����, ����� ������������ ��������� �� ������ ����� ����

void setup()
{
	Serial.begin(115200);

	//��������� �������� ����
	RelaySwitch(false);

	//Mqtt setup
	mqttclient.setServer(mqtt_server, mqtt_port);
	mqttclient.setCallback(MqttCallback);

	pinMode(relayPin, OUTPUT);

	//attachInterrupt(digitalPinToInterrupt(buttonPin), Interrupt_WF, levelButton ? FALLING : RISING);

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

	// ���� ������� ����, �� ��������� ��������� �� ������
	if (flagChange) {
		mqttclient.publish(topicSwitchState, String(rState).c_str(), true);
		flagChange = false;
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
			mqttclient.subscribe(topicSwitch); // ������������� �� ����� � �������
			mqttclient.subscribe(topicSwitchState); // ������������� �� ����� �� ��������

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

	payload[length] = '\0';
	String strPayload = String((char*)payload);
	Serial.println(strPayload);


	int val = strPayload.toInt();

	if (strcmp(topic, topicSwitch) == 0)
	{
		// �������� ��� ��������� ���� � ���������� �� ���������� �������� ������
		OnBtnPress(val);
	}
	else if (strcmp(topic, topicSwitchState) == 0)
	{
		//��������� ������ ������ ���������, ����������� ���������� �����������
		//if (val != rState)
		//	mqttclient.publish(topicSwitchState, String(rState).c_str(), true);

	}
}

// �������, ���������� �����������, ��� ������ ��� �������� (button without fixing)
void Interrupt_WF() {

	//������ �� �������� 
	if (lock || !lockTimer2.isExpired())
		return;
	lock = true;
	lockTimer.restart();
	while (!lockTimer.isExpired())
	{
	}

	if (digitalRead(buttonPin) != levelButton)
	{
		OnBtnPress(!rState);
		flagChange = true;
	}

	lockTimer2.restart(); // ���������� �� �/� ������� � ����
	lock = false;
}

void OnBtnPress(int val)
{
	if (debug)
	{
		Serial.print("OnBtnPress(");
		Serial.print(val);
		Serial.println(")");
	}
	
	mqttclient.publish(topicSwitchState, String(val).c_str(), true);

	RelaySwitch(val);

	mqttclient.publish(topicSwitchState, String(false).c_str(), true);


}

//������� ����
void RelaySwitch(int val)
{
	if (levelTrigger)
		digitalWrite(relayPin, HIGH);
	else
		digitalWrite(relayPin, LOW);

	delay(val * 1000);

	if (levelTrigger)
		digitalWrite(relayPin, LOW);
	else
		digitalWrite(relayPin, HIGH);
}






