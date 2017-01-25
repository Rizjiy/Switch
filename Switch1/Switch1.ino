#include <RBD_Timer.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "MikroTik48";
const char* password = "microtest";
const char *mqtt_server = "192.168.88.237"; // Имя сервера MQTT
const int mqtt_port = 1883; // Порт для подключения к серверу MQTT

WiFiClient wclient;
PubSubClient client(wclient);

int relayPin = 2;
int buttonPin = 0;

RBD::Timer reconnectTimer(60000); //пауза между реконнектами Wi-Fi
RBD::Timer debugTimer(3000); //3 sec для того, чтобы не забивать эфир
bool isDebug = true;

boolean rState1 = false;
boolean btnPress = false;
boolean lastbtnStat = false;

const char *topicSwitch = "home/light1";

void setup()
{
	Serial.begin(115200);

	//Mqtt setup
	client.setServer(mqtt_server, mqtt_port);
	client.setCallback(callback);

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
				client.loop();
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

	if (isDebug && debugTimer.onRestart())
	{
		//считаем с gpio0 данные
		boolean current = digitalRead(buttonPin);    // Считать состояние кнопки
		Serial.print("Gpio0=");
		Serial.println(current);		
		//digitalWrite(relayPin, current);
	}

	buttonWF();

	//// часть кода для кнопки, реле и светодиода
	//currentButton = debounce(lastButton);
	//if (lastButton == LOW && currentButton == HIGH)   // Если нажатие (условие для реле)
	//{
	//	Serial.println("button pressed");
	//	relayState = !relayState;
	//	lastButton = currentButton;
	//	digitalWrite(relayPin, relayState);    // Изменить статус состояния реле
	//}


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
	if (!client.connected()) 
	{
		Serial.print("Attempting MQTT connection...");
		// Attempt to connect
		//if (client.connect("ESP8266Client", mqtt_user, mqtt_pass))
		if (client.connect("ESP8266Client")) 
		{
			Serial.println("connected");
			// Once connected, publish an announcement...
			client.publish("Start", "ESP8266Client is started");
			// ... and resubscribe
			client.subscribe(topicSwitch); // подписывааемся по топик с данными для светодиода
		}
		else 
		{
			Serial.print("failed, rc=");
			Serial.println(client.state());
			return false;
		}
	}

	return true;
}

// Функция получения данных от сервера
void callback(char* topic, byte* payload, unsigned int length) {
	Serial.print("MQTT message arrived [");
	Serial.print(topic);
	Serial.print("] ");
	for (int i = 0; i < length; i++) {
		Serial.print((char)payload[i]);
	}
	Serial.println();

	if ((char)payload[0] == '1') {
		digitalWrite(relayPin, HIGH); // включаем или выключаем светодиод в зависимоти от полученных значений данных
	}
	else
		digitalWrite(relayPin, LOW);
}

// button without fixing, кнопка без фиксации
void buttonWF() {
	btnPress = digitalRead(buttonPin);

	if (btnPress && !lastbtnStat) {
		delay(30); // защита от дребезга
		btnPress = digitalRead(buttonPin);

		if (btnPress) {
			rState1 = !rState1;
			digitalWrite(relayPin, rState1);
			// публикуем изменение состояния реле на брокер      
			client.publish(topicSwitch, String(rState1).c_str(), true);
		}
	}
	lastbtnStat = btnPress;
}







