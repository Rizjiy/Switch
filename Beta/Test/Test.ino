/*
 Name:		Test.ino
 Created:	12/8/2018 6:21:40 PM
 Author:	rodal
*/
#include <Secret.h>

#include <RBD_Timer.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <string>
using namespace std;

//***Блок переменных
const char* ssid = WI_FI_SSID;
const char* wifiPass = WI_FI_PASSWORD;
const char* mqttServer = MQTT_SERVER;
const int mqttPort = MQTT_PORT; // Порт для подключения к серверу MQTT
const char* mqttUser = MQTT_USER;
const char* mqttPass = MQTT_PASSWORD;

const string deviceName = "switch4";

const int buttonPin = 14; //-1 - нет физической кнопки
const int mainPin = 12;

boolean levelButton = HIGH; // Сигнал в нормальном состоянии на кнопке или датчике касания

RBD::Timer reconnectTimer(60000); //пауза между реконнектами Wi-Fi
RBD::Timer debugTimer(3000); //3 sec для того, чтобы не забивать эфир
RBD::Timer lockTimer(30); // защита от дребезга
RBD::Timer lockTimer2(90); // защита от дребезга
//**


class ConnectionHelper {
public:
	ConnectionHelper(const char* ssid, const char* wifiPass, const char* mqttServer, const int mqttPort, const char* mqttUser, const char* mqttPass, string deviceName);
	void handle();
	void MqttCallback(char* topic, byte* payload, unsigned int length);
	RBD::Timer reconnectTimer; //пауза между реконнектами Wi-Fi

	WiFiClient wifiClient;
	PubSubClient mqttClient;

private:
	bool wifiConnect();
	bool mqttConnect();

	const char* _ssid;
	const char* _wifiPass;
	const char* _mqttServer;
	int _mqttPort;
	const char* _mqttUser;
	const char* _mqttPass;

	string _deviceName;
	string _baseTopic = "home/switches";

	string _topicSubscribe;


};

ConnectionHelper::ConnectionHelper(const char* ssid, const char* wifiPass, const char* mqttServer, const int mqttPort, const char* mqttUser, const char* mqttPass, string deviceName){
	_ssid = ssid;
	_wifiPass = wifiPass;
	_mqttServer = mqttServer;
	_mqttPort = mqttPort;
	_mqttUser = mqttUser;
	_mqttPass = mqttPass;

	_deviceName = deviceName;

	RBD::Timer reconnectTimer(60000); //пауза между реконнектами Wi-Fi

	WiFiClient wifiClient;
	PubSubClient mqttClient(wifiClient);
	//Mqtt setup
	mqttClient.setServer(_mqttServer, _mqttPort);
	mqttClient.setCallback(MqttCallback);


	_topicSubscribe = _baseTopic + "/" + _deviceName + "/#";		// home/switches/switch5/#
}

bool ConnectionHelper::wifiConnect()
{
	if (WiFi.status() != WL_CONNECTED) {
		Serial.print("Connecting to ");
		Serial.print(_ssid);
		Serial.println("...");
		WiFi.begin(_ssid, _wifiPass);

		if (WiFi.waitForConnectResult() != WL_CONNECTED)
			return false;
		Serial.println("WiFi connected");
	}

	return true;
}

bool ConnectionHelper::mqttConnect()
{
	// Loop until we're reconnected
	if (!mqttClient.connected())
	{
		Serial.print("Attempting MQTT connection to ");
		Serial.print(_mqttServer);
		Serial.println("...");
		// Attempt to connect
		//if (client.connect("ESP8266Client", mqtt_user, mqtt_pass))
		if (mqttClient.connect(_deviceName.c_str(), _mqttUser, _mqttPass))
		{
			Serial.println("connected");
			// Once connected, publish an announcement...
			string startPayload = _deviceName + ' ' + String(wifiClient.localIP()).c_str();
			mqttClient.publish("Start", startPayload.c_str());
			// ... and resubscribe
			mqttClient.subscribe(_topicSubscribe.c_str()); // подписываемся нв топики для этого устройства
		}
		else
		{
			Serial.print("failed, rc=");
			Serial.println(mqttClient.state());
			return false;
		}
	}

	return true;
}

void ConnectionHelper::handle() {

	// подключаемся к wi-fi
	if (reconnectTimer.isExpired())
	{
		if (wifiConnect())
			if (mqttConnect())
			{
				//Все ок
				mqttClient.loop();
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

}

// Функция получения данных от сервера
void ConnectionHelper::MqttCallback(char* topic, byte* payload, unsigned int length) {
	Serial.print("MQTT message arrived [");
	Serial.print(topic);
	Serial.print("] ");
	for (int i = 0; i < length; i++) {
		Serial.print((char)payload[i]);
	}
	Serial.println();

}


//class MqttButton {
//public:
//	MqttButton(byte relayPin, byte buttonPin, string string);
//private:
//	byte _relayPin;
//	byte _buttonPin;
//	string deviceName;
//	WiFiClient wifiClient;
//	PubSubClient mqttClient;
//
//
//};
//
//MqttButton::MqttButton(byte relayPin, byte buttonPin, string deviceName) {
//	_relayPin = relayPin;
//	_buttonPin = buttonPin;
//	deviceName = deviceName;
//
//	pinMode(_relayPin, OUTPUT);
//
//}



ConnectionHelper helper(ssid, wifiPass, mqttServer, mqttPort, mqttUser, mqttPass, "switch97");

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(115200);

}

// the loop function runs over and over again until power down or reset
void loop() {
	helper.handle();

}

