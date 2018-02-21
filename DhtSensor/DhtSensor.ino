#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
#include <ArduinoJson.h>

// the setup function runs once when you press reset or power the board
void setup() {

}

// the loop function runs over and over again until power down or reset
void loop() {
  
}


// function called to publish the temperature and the humidity
void publishData(float p_temperature, float p_humidity) {
	// create a JSON object
	// doc : https://github.com/bblanchon/ArduinoJson/wiki/API%20Reference
	StaticJsonBuffer<200> jsonBuffer;
	JsonObject& root = jsonBuffer.createObject();
	// INFO: the data must be converted into a string; a problem occurs when using floats...
	root["temperature"] = (String)p_temperature;
	root["humidity"] = (String)p_humidity;
	root.prettyPrintTo(Serial);
	Serial.println("");
	/*
	{
	"temperature": "23.20" ,
	"humidity": "43.70"
	}
	*/
	char data[200];
	root.printTo(data, root.measureLength() + 1);
	client.publish(MQTT_SENSOR_TOPIC, data, true);
	yield();
}