/*MQTT:
включить реле: home/switches/switch5/pins/main payload:1
статус реле: home/switches/switch5/pins/main/state
установить уровень на пин 12: home/switches/switch5/pins/12
статус на пине 12: home/switches/switch5/pins/12/state
*/

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

const string deviceName = "switch2";

const int buttonPin = -1; //-1 - нет физической кнопки
const int mainPin = 14;

boolean levelButton = HIGH; // Сигнал в нормальном состоянии на кнопке или датчике касания

RBD::Timer reconnectTimer(60000); //пауза между реконнектами Wi-Fi
RBD::Timer debugTimer(3000); //3 sec для того, чтобы не забивать эфир
RBD::Timer lockTimer(30); // защита от дребезга
RBD::Timer lockTimer2(90); // защита от дребезга
//**

const string baseTopic = "home/switches";
const string strState = "state";
const string strMainPin = "main";
const string topicSubscribe = baseTopic + "/" + deviceName + "/#";		// home/switches/switch5/#
const string topicPins = baseTopic + "/" + deviceName + "/pins";			// home/switches/switch5/pins				
const string topicCmd = baseTopic + "/" + deviceName + "/cmd";			// home/switches/switch5/cmd

const int pinsOut[6] = { 16,14,12,13,15 }; //доступные пины для esp8266

bool debug = true;

volatile bool lock = false;
volatile boolean rState = false; // В прерываниях всегда используем тип volatile для изменяемых переменных
volatile boolean flagChange = false; // Флаг нужен для того, чтобы опубликовать сообщение на брокер после того

WiFiClient wclient;
PubSubClient mqttclient(wclient);

void setup()
{
	Serial.begin(115200);
	ArduinoOTA.setHostname(deviceName.c_str());
	ArduinoOTA.begin();

	//Начальное значение реле
	//RelaySwitch(false);

	//установим режим output для всех свободных пинов
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

		//отсекаем базу 
		string subTopic;
		subTopic.assign(topicStr, topicPins.length(), topicStr.length() - topicPins.length());
		Serial.println("subTopic: " + String(subTopic.c_str()));

		//***парсим topic (/main /12 /main/state /12/state)
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
			//если это стейт, то обновлем его фактическим значением и отправляем назад
			if (val != actualPinLevel)
				PublicPinState(pin, isMainPin, actualPinLevel);
		}
		else
		{
			// включаем или выключаем реле в зависимоти от полученных значений данных
			digitalWrite(pin, val);
			Serial.println("digitalWrite(" + String(pin) + "," + String(val) + ")");
			PublicPinState(pin, isMainPin, val);
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

	digitalWrite(mainPin, state);

	//меняем текущее состояние
	rState = state;
}

//Отправляет на сервер состояние текущее Gpio
void PublicPinState(uint8_t pin, bool isMainPin, bool val) {
	
	string topicState;
	if(isMainPin)
		topicState = topicPins +"/"+ strMainPin +"/"+ strState;
	else
		topicState = topicPins + "/" + String(pin).c_str() + "/" + strState;

	mqttclient.publish(topicState.c_str(), String(val).c_str(), true);
	Serial.println("MQTT publish: [" + String(topicState.c_str()) + "] " + String(val));

}
