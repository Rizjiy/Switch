#include <RBD_Timer.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Secret.h>


const char* ssid = WI_FI_SSID;
const char* password = WI_FI_PASSWORD;
const char *mqtt_server = MQTT_SERVER;
const int mqtt_port = MQTT_PORT; // Порт для подключения к серверу MQTT
const char* mqttUser = MQTT_USER;
const char* mqttPass = MQTT_PASSWORD;

const char* clientName = "switch4";
const char *topicSwitch = "home/switches/4";
const char *topicSwitchState = "home/switches/4/status";

const int buttonPin = 12;
const int relayPin = 13;

boolean levelTrigger = HIGH;
boolean levelButton = HIGH; // Сигнал в нормальном состоянии на кнопке или датчике касания

WiFiClient wclient;
PubSubClient mqttclient(wclient);

RBD::Timer reconnectTimer(60000); //пауза между реконнектами Wi-Fi
RBD::Timer debugTimer(3000); //3 sec для того, чтобы не забивать эфир
RBD::Timer lockTimer(30); // защита от дребезга
RBD::Timer lockTimer2(90); // защита от дребезга
bool debug = true;

volatile bool lock = false;
volatile boolean rState = false; // В прерываниях всегда используем тип volatile для изменяемых переменных
volatile boolean flagChange = false; // Флаг нужен для того, чтобы опубликовать сообщение на брокер после того

void setup()
{
	Serial.begin(115200);

	//Начальное значение реле
	RelaySwitch(false);

	//Mqtt setup
	mqttclient.setServer(mqtt_server, mqtt_port);
	mqttclient.setCallback(MqttCallback);

	pinMode(relayPin, OUTPUT);

	attachInterrupt(digitalPinToInterrupt(buttonPin), Interrupt_WF, levelButton ? FALLING : RISING);

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

	// Если запущен флаг, то публикуем состояние на брокер
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
		mqttclient.publish(topicSwitchState, String(rState).c_str(), true);
	}
	else if (strcmp(topic, topicSwitchState) == 0)
	{
		//обновляем статус других устройств, фактическим состоянием выключателя
		if (val != rState)
			mqttclient.publish(topicSwitchState, String(rState).c_str(), true);

	}
}

// Функция, вызываемая прерыванием, для кнопки без фиксации (button without fixing)
void Interrupt_WF() {

	//Защита от дребезга 
	if (lock || !lockTimer2.isExpired())
		return;
	lock = true;
	lockTimer.restart();
	while(!lockTimer.isExpired())
	{}

	if (digitalRead(buttonPin) != levelButton)
	{
		OnBtnPress(!rState);
		flagChange = true;
	}

	lockTimer2.restart(); // защищаемся от э/м скачков в реле
	lock = false;
}

void OnBtnPress(bool state)
{
	if (debug)
	{
		Serial.print("OnBtnPress(");
		Serial.print(state);
		Serial.println(")");
	}

	RelaySwitch(state);

	//меняем текущее состояние
	rState = state;
}

//Щелкаем реле
void RelaySwitch(bool state)
{
	if(levelTrigger)
		digitalWrite(relayPin, state);
	else
		digitalWrite(relayPin, !state);
}







