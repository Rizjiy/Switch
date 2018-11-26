#include <RBD_Timer.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Secret.h>
#include <ArduinoOTA.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

//***Блок переменных
const char* ssid = WI_FI_SSID;
const char* password = WI_FI_PASSWORD;
const char *mqtt_server = MQTT_SERVER;
const int mqtt_port = MQTT_PORT; // Порт для подключения к серверу MQTT
const char* mqttUser = MQTT_USER;
const char* mqttPass = MQTT_PASSWORD;

const char* clientName = "switch6";
const char *topicSwitch = "home/switches/6";
const char *topicSwitchState = "home/switches/6/status";

const int buttonPin = 12; //-1 - нет физической кнопки
const int relayPin = 13;

boolean levelTrigger = LOW;
boolean levelButton = HIGH; // Сигнал в нормальном состоянии на кнопке или датчике касания

WiFiClient wclient;
PubSubClient mqttclient(wclient);

RBD::Timer reconnectTimer(60000); //пауза между реконнектами Wi-Fi
RBD::Timer debugTimer(3000); //3 sec для того, чтобы не забивать эфир
RBD::Timer lockTimer(30); // защита от дребезга
RBD::Timer lockTimer2(90); // защита от дребезга

#define BME280_ADDRESS  (0x77)
#define BME280_ADDRESS2  (0x76)
//**



Adafruit_BME280 bme; // I2C
Adafruit_BME280 bme2; // I2C


const long delayTime = 3000;

void setup() {
	
	Serial.begin(115200);

	Serial.print(F("BME280 start "));
	bool status = bme.begin(BME280_ADDRESS);
	Serial.println(status);

	Serial.print(F("BME280-2 start "));
	bool status2 = bme2.begin(BME280_ADDRESS2);
	Serial.println(status);


	Serial.println("-- Default Test --");

	Serial.println();
}


void loop() {
	printValues();
	delay(delayTime);
}


void printValues() {

	Serial.print("Temperature = ");
	Serial.print(bme.readTemperature());
	Serial.println(" *C");
	Serial.print("Temperature2 = ");
	Serial.print(bme2.readTemperature());
	Serial.println(" *C");

	Serial.print("Humidity = ");
	Serial.print(bme.readHumidity());
	Serial.println(" %");
	Serial.print("Humidity2 = ");
	Serial.print(bme2.readHumidity());
	Serial.println(" %");


	Serial.println();

}