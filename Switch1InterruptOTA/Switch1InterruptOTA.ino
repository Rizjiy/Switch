/*MQTT:
�������� ����: home/switches/switch5/pins/main payload:1
������ ����: home/switches/switch5/pins/main/state
���������� ������� �� ��� 12: home/switches/switch5/pins/12
������ �� ���� 12: home/switches/switch5/pins/12/state
*/

#include <RBD_Timer.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Secret.h>
#include <ArduinoOTA.h>

#include <string>
using namespace std;

//***���� ����������
const char* ssid = WI_FI_SSID;
const char* password = WI_FI_PASSWORD;
const char* mqtt_server = MQTT_SERVER;
const int mqtt_port = MQTT_PORT; // ���� ��� ����������� � ������� MQTT
const char* mqttUser = MQTT_USER;
const char* mqttPass = MQTT_PASSWORD;

const string deviceName = "switch2";

const int buttonPin = -1; //-1 - ��� ���������� ������
const int mainPin = 14;

boolean levelButton = HIGH; // ������ � ���������� ��������� �� ������ ��� ������� �������

RBD::Timer reconnectTimer(60000); //����� ����� ������������ Wi-Fi
RBD::Timer debugTimer(3000); //3 sec ��� ����, ����� �� �������� ����
RBD::Timer lockTimer(30); // ������ �� ��������
RBD::Timer lockTimer2(90); // ������ �� ��������
//**

const string baseTopic = "home/switches";
const string strState = "state";
const string strMainPin = "main";
const string topicSubscribe = baseTopic + "/" + deviceName + "/#";		// home/switches/switch5/#
const string topicPins = baseTopic + "/" + deviceName + "/pins";			// home/switches/switch5/pins				
const string topicCmd = baseTopic + "/" + deviceName + "/cmd";			// home/switches/switch5/cmd

const int pinsOut[6] = { 16,14,12,13,15 }; //��������� ���� ��� esp8266

bool debug = true;

volatile bool lock = false;
volatile boolean rState = false; // � ����������� ������ ���������� ��� volatile ��� ���������� ����������
volatile boolean flagChange = false; // ���� ����� ��� ����, ����� ������������ ��������� �� ������ ����� ����

WiFiClient wclient;
PubSubClient mqttclient(wclient);

void setup()
{
	Serial.begin(115200);
	ArduinoOTA.setHostname(deviceName.c_str());
	ArduinoOTA.begin();

	//��������� �������� ����
	//RelaySwitch(false);

	//��������� ����� output ��� ���� ��������� �����
	SetOutputPins();

	//Mqtt setup
	mqttclient.setServer(mqtt_server, mqtt_port);
	mqttclient.setCallback(MqttCallback);

	if(buttonPin>=0)
		attachInterrupt(digitalPinToInterrupt(buttonPin), Interrupt_WF, levelButton ? FALLING : RISING);

	if (WifiConnect())
		MqttConnect();

}

void loop()
{
	ArduinoOTA.handle();

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

	// ��� ����������. ���� ������� ����, �� ��������� ��������� �� ������
	if (flagChange) {
		PublicPinState(mainPin, true, rState);
		flagChange = false;
	}

}

void SetOutputPins()
{
	for (int i = 0; i < sizeof(pinsOut) / sizeof(pinsOut[0]); ++i)
	{
		if (pinsOut[i] == buttonPin)
			continue;

		pinMode(pinsOut[i], OUTPUT);
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
		if (mqttclient.connect(deviceName.c_str(), mqttUser, mqttPass))
		{
			Serial.println("connected");
			// Once connected, publish an announcement...
			string startPayload = deviceName + ' ' + String(wclient.localIP()).c_str();
			mqttclient.publish("Start", startPayload.c_str());
			// ... and resubscribe
			mqttclient.subscribe(topicSubscribe.c_str()); // ������������� �� ������ ��� ����� ����������
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


	string topicStr(topic);

	if (topicStr.find(topicPins) != -1)
	{
		//�������� ������ ���� 0 ��� 1
		bool val = false;
		if (payload[0] == '1')
			val = true;
		else
			val = false;

		//�������� ���� 
		string subTopic;
		subTopic.assign(topicStr, topicPins.length(), topicStr.length() - topicPins.length());
		Serial.println("subTopic: " + String(subTopic.c_str()));

		//***������ topic (/main /12 /main/state /12/state)
		size_t pinStart = subTopic.find('/');
		size_t stateStart = subTopic.find('/', pinStart + 1);

		size_t pinEnd = stateStart;
		if (stateStart == string::npos)
			pinEnd = subTopic.length();

		string pinSection = subTopic.substr(pinStart + 1, pinEnd - pinStart - 1);

		string stateSection;
		if(stateStart != string::npos)
			stateSection = subTopic.substr(stateStart + 1, subTopic.length() - 1);

		//Serial.println("pinStart: " + String(pinStart) + " pinEnd: " + String(pinEnd) + " stateStart: " + String(stateStart));
		Serial.println("pinSection: " + String(pinSection.c_str()) + " stateSection: " + String(stateSection.c_str()));

		bool isMainPin;
		int pin;
		if (pinSection == strMainPin)
		{
			isMainPin = true;
			pin = mainPin;
		}
		else
		{
			isMainPin = false;
			String strPin = pinSection.c_str();
			pin = strPin.toInt();

			if(pinSection != "0" && pin == 0)
				Serial.println("Error! ParseTopic: " + String(subTopic.c_str()));

		}

		bool isState = stateSection == strState;
		//**

		bool actualPinLevel = digitalRead(pin);

		if (isState)
		{
			//���� ��� �����, �� �������� ��� ����������� ��������� � ���������� �����
			if (val != actualPinLevel)
				PublicPinState(pin, isMainPin, actualPinLevel);
		}
		else
		{
			// �������� ��� ��������� ���� � ���������� �� ���������� �������� ������
			digitalWrite(pin, val);
			Serial.println("digitalWrite(" + String(pin) + "," + String(val) + ")");
			PublicPinState(pin, isMainPin, val);
		}

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

void OnBtnPress(bool state)
{
	if (debug)
	{
		Serial.println("OnBtnPress(" + String(state) + ")");
	}

	digitalWrite(mainPin, state);

	//������ ������� ���������
	rState = state;
}

//���������� �� ������ ��������� ������� Gpio
void PublicPinState(uint8_t pin, bool isMainPin, bool val) {
	
	string topicState;
	if(isMainPin)
		topicState = topicPins +"/"+ strMainPin +"/"+ strState;
	else
		topicState = topicPins + "/" + String(pin).c_str() + "/" + strState;

	mqttclient.publish(topicState.c_str(), String(val).c_str(), true);
	Serial.println("MQTT publish: [" + String(topicState.c_str()) + "] " + String(val));

}





