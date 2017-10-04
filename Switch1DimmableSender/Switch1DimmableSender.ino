#include <RBD_Timer.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <nRF24L01.h>                                     // Подключаем файл настроек из библиотеки RF24
#include <RF24.h>                                         // Подключаем библиотеку для работы с nRF24L01+
#include <Secret.h>


const char* ssid = WI_FI_SSID;
const char* password = WI_FI_PASSWORD;
const char *mqtt_server = MQTT_SERVER;
const int mqtt_port = MQTT_PORT; // Порт для подключения к серверу MQTT
const char* mqttUser = MQTT_USER;
const char* mqttPass = MQTT_PASSWORD;

const char* clientName = "switch3";
const char *topicSwitch = "home/switches/3";
const char *topicSwitchState = "home/switches/3/status";

//const int relayPin = 13;
const int buttonPin = 12;

WiFiClient wclient;
PubSubClient mqttclient(wclient);

RF24           radio(4, 15);                              // Создаём объект radio для работы с библиотекой RF24, указывая номера выводов nRF24L01+ (CE, CSN)

RBD::Timer reconnectTimer(60000); //пауза между реконнектами Wi-Fi
RBD::Timer lockTimer(30); // защита от дребезга
RBD::Timer lockTimer2(90); // защита от дребезга
bool debug = true;

int            data[1];                                   // Создаём массив для передачи данных
volatile bool lock = false;
volatile boolean rState1 = false; // В прерываниях всегда используем тип volatile для изменяемых переменных
volatile boolean flagChange = false; // Флаг нужен для того, чтобы опубликовать сообщение на брокер после того

boolean btnPress = false;
boolean lastbtnStat = false;

void setup()
{
	Serial.begin(115200);

	//настройки nRF24
	SPI.setHwCs(true);
	SPI.begin();
	SPI.setDataMode(SPI_MODE0);
	SPI.setBitOrder(MSBFIRST);
	radio.begin();                                        // Инициируем работу nRF24L01+
	radio.setChannel(5);                                  // Указываем канал передачи данных (от 0 до 127), 5 - значит передача данных осуществляется на частоте 2,405 ГГц (на одном канале может быть только 1 приёмник и до 6 передатчиков)
	radio.setDataRate(RF24_1MBPS);                   // Указываем скорость передачи данных (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS), RF24_1MBPS - 1Мбит/сек
	radio.setPALevel(RF24_PA_HIGH);                 // Указываем мощность передатчика (RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm)
	radio.openWritingPipe(0x1234567890LL);               // Открываем трубу с идентификатором 0x1234567890 для передачи данных (на ожном канале может быть открыто до 6 разных труб, которые должны отличаться только последним байтом идентификатора)
	//

	//Mqtt setup
	mqttclient.setServer(mqtt_server, mqtt_port);
	mqttclient.setCallback(MqttCallback);

//	pinMode(relayPin, OUTPUT);

	attachInterrupt(digitalPinToInterrupt(buttonPin), Interrupt_WF, RISING);

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

	// Если запущен флаг, то публикуем сообщение на брокер
	if (flagChange) {
		mqttclient.publish(topicSwitch, String(rState1).c_str(), true);
		flagChange = false;
	}

	//ButtonWf();

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

	if (digitalRead(buttonPin))
	{
		OnBtnPress(!rState1);
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
	
	uint8_t power = 0;
	if (state)
		power = 255;


	//Отправляем по трубе текущее состояние
	//data
	radio.write(&power, sizeof(uint8_t));

	//digitalWrite(relayPin, state);
	rState1 = state;
}










