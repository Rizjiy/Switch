#include <RBD_Timer.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "MikroTik48";
const char* password = "";// 
const char *mqtt_server = "192.168.88.3"; // адрес сервера MQTT
const int mqtt_port = 1883; // Порт для подключения к серверу MQTT
const char* clientName = "switch1";
const char* mqttUser = "mqtt";
const char* mqttPass = "";

WiFiClient wclient;
PubSubClient mqttclient(wclient);

int relayPin = 13;
int buttonPin = 12;

RBD::Timer reconnectTimer(60000); //пауза между реконнектами Wi-Fi
RBD::Timer debugTimer(3000); //3 sec для того, чтобы не забивать эфир
bool debug = false;

boolean rState1 = false;
boolean btnPress = false;
boolean lastbtnStat = false;

const char *topicSwitch = "home/switches/1";
const char *topicSwitchState = "home/switches/1/status";

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
		if (mqttclient.connect(clientName, mqttUser, mqttPass))
		{
			Serial.println("connected");
			// Once connected, publish an announcement...
			mqttclient.publish("Start", clientName);
			// ... and resubscribe
			mqttclient.subscribe(topicSwitch); // подписываемся нв топик с данными
			mqttclient.subscribe(topicSwitchState); // подписываемся на топик со статусом

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

// Функция получения данных от сервера
void MqttCallback(char* topic, byte* payload, unsigned int length) {
	Serial.print("MQTT message arrived [");
	Serial.print(topic);
	Serial.print("] ");
	for (int i = 0; i < length; i++) {
		Serial.print((char)payload[i]);
	}
	Serial.println();

	bool val = false;
	if (payload[0] == '1')
		val = true;
	else
		val = false;

	if (strcmp(topic, topicSwitch) == 0)
	{
		// включаем или выключаем реле в зависимоти от полученных значений данных
		OnBtnPress(val);
		mqttclient.publish(topicSwitchState, String(rState1).c_str(), true);
	}
	else if (strcmp(topic, topicSwitchState) == 0)
	{
		//обновляем статус других устройств, фактияеским состоянием выключателя
		if (val != rState1)
			mqttclient.publish(topicSwitchState, String(rState1).c_str(), true);

	}
}

// button without fixing, кнопка без фиксации
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
		delay(30); // защита от дребезга
		btnPress = digitalRead(buttonPin);

		if (btnPress) {  
			OnBtnPress(!rState1);
			// публикуем изменение состояния реле на брокер (возвратное)   
			mqttclient.publish(topicSwitchState, String(rState1).c_str(), true);
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
	//меняем текущее состояние
	rState1 = state;
}









