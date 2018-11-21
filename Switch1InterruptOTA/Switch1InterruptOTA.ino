#include <RBD_Timer.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Secret.h>
#include <ArduinoOTA.h>

#include <string>
using namespace std;

//***Блок переменных
const char* ssid = WI_FI_SSID;
const char* password = WI_FI_PASSWORD;
const char* mqtt_server = MQTT_SERVER;
const int mqtt_port = MQTT_PORT; // Порт для подключения к серверу MQTT
const char* mqttUser = MQTT_USER;
const char* mqttPass = MQTT_PASSWORD;

string deviceName = "switch98";

const int buttonPin = -1; //-1 - нет физической кнопки
const int relayPin = 13;

boolean levelTrigger = LOW;
boolean levelButton = HIGH; // Сигнал в нормальном состоянии на кнопке или датчике касания

WiFiClient wclient;
PubSubClient mqttclient(wclient);

RBD::Timer reconnectTimer(60000); //пауза между реконнектами Wi-Fi
RBD::Timer debugTimer(3000); //3 sec для того, чтобы не забивать эфир
RBD::Timer lockTimer(30); // защита от дребезга
RBD::Timer lockTimer2(90); // защита от дребезга
//**

string baseTopic = "home/switches";
string strState = "/state";
string strMainPin = "/main";
string topicSubscribe;		// home/switches/switch5/#
string topicPins;			// home/switches/switch5/pins  /12 или /main
string topicPinsState;		// home/switches/switch5/pins/state  /12 или /main							
string topicCmd;			// home/switches/switch5/cmd

bool debug = true;

volatile bool lock = false;
volatile boolean rState = false; // В прерываниях всегда используем тип volatile для изменяемых переменных
volatile boolean flagChange = false; // Флаг нужен для того, чтобы опубликовать сообщение на брокер после того

void setup()
{
	Serial.begin(115200);
	ArduinoOTA.setHostname(deviceName.c_str());
	ArduinoOTA.begin();

	//Начальное значение реле
	RelaySwitch(false);

	//Mqtt setup
	mqttclient.setServer(mqtt_server, mqtt_port);
	mqttclient.setCallback(MqttCallback);

	pinMode(relayPin, OUTPUT);

	if(buttonPin>=0)
		attachInterrupt(digitalPinToInterrupt(buttonPin), Interrupt_WF, levelButton ? FALLING : RISING);

	if (WifiConnect())
		MqttConnect();

	//***подготавливаем топики
	topicSubscribe	= baseTopic + "/" + deviceName + "/#";
	topicPins		= baseTopic + "/" + deviceName + "/pins";
	topicPinsState	= baseTopic + "/" + deviceName + "/pins/state";
	topicCmd		= baseTopic + "/" + deviceName + "/cmd";
	//**

}

void loop()
{
	ArduinoOTA.handle();

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

	// Для прерывания. Если запущен флаг, то публикуем состояние на брокер
	if (flagChange) {
		PublicMainPinState();
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
		if (mqttclient.connect(deviceName.c_str(), mqttUser, mqttPass))
		{
			Serial.println("connected");
			// Once connected, publish an announcement...
			mqttclient.publish("Start", deviceName.c_str());
			// ... and resubscribe
			mqttclient.subscribe(topicSubscribe.c_str()); // подписываемся нв топики для этого устройства
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

//парсим строку (/main /12 /main/state /12/state) и вытаскиваем от туда Gpio. Если не удалось вкрнем -1 и считаем что это mainPin
int ParsePin(const std::string &substr) {

	int pin;
	string resStr;

	if (substr.rfind(strState) != -1)
	{
		//отсекаем state справа
		resStr.assign(substr, 0, substr.length());
	}

	//остается (/main /12)
	if (resStr.find("/") == 0)
		resStr.erase(0, 1);

	String strPin = resStr.c_str();
	pin = strPin.toInt();

	return pin;
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


	string topicStr(topic);

	if (topicStr.find(topicPins) != -1)
	{
		//значение должно быть 0 или 1
		bool val = false;
		if (payload[0] == '1')
			val = true;
		else
			val = false;


		string subTopic;
		//отсекаем базу (/main /12 /main/state /12/state)
		subTopic.assign(topicStr, topicPins.length(), topicStr.length() - topicPins.length());
		Serial.println("subTopic: " + String(subTopic.c_str()));

		//главный пин (всегда 1 - включить)
		if (subTopic.rfind(strMainPin) != -1)
		{
			if (subTopic.rfind(strState) != -1)
			{
				//обновляем статус других устройств, фактическим состоянием выключателя
				if (val != rState)
					PublicMainPinState();
			}
			else
			{
				// включаем или выключаем реле в зависимоти от полученных значений данных
				OnBtnPress(val);
				PublicMainPinState();
			}
		}
		else
		{
			//вытаскиваем пин
			int pin = ParsePin(subTopic);
			Serial.println("pin: " + String(pin));

			bool actualPinLevel = digitalRead(pin);

			if (subTopic.rfind(strState) != -1)
			{
				//если это стейт, то обновлем его и отправляем назад
				if (val != actualPinLevel)
					mqttclient.publish(topic, String(actualPinLevel).c_str(), true);
			}
			else
			{
				// включаем или выключаем реле в зависимоти от полученных значений данных
				digitalWrite(pin, val);
				Serial.println("digitalWrite(" + String(pin) + String(val) + ")");
				mqttclient.publish((topicStr+strState).c_str(), String(val).c_str(), true);
			}
		}
	}
}

// Функция, вызываемая прерыванием, для кнопки без фиксации (button without fixing)
void Interrupt_WF() {

	//Защита от дребезга 
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

	lockTimer2.restart(); // защищаемся от э/м скачков в реле
	lock = false;
}

void OnBtnPress(bool state)
{
	if (debug)
	{
		Serial.println("OnBtnPress(" + String(state) + ")");
	}

	RelaySwitch(state);

	//меняем текущее состояние
	rState = state;
}

//Щелкаем реле
void RelaySwitch(bool state)
{
	if (levelTrigger)
		digitalWrite(relayPin, state);
	else
		digitalWrite(relayPin, !state);
}

void PublicMainPinState() {
	string topicMainPinState = topicPinsState + strMainPin;
	mqttclient.publish(topicMainPinState.c_str(), String(rState).c_str(), true);
}





