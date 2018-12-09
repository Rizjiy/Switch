/*
 Name:		Test.ino
 Created:	12/8/2018 6:21:40 PM
 Author:	rodal
*/
#include <RBD_Timer.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <string>
using namespace std;

class ConnectionHelper {
public:
	ConnectionHelper(const char* ssid, const char* wifiPass, const char* mqttServer, const int mqttPort, const char* mqttUser, const char* mqttPass);
	void MqttConnect();
private:
	const char* _ssid;
	const char* _wifiPass;
	const char* _mqttServer;
	int _mqttPort;
	const char* _mqttUser;
	const char* _mqttPass;
};

ConnectionHelper::ConnectionHelper(const char* ssid, const char* wifiPass, const char* mqttServer, const int mqttPort, const char* mqttUser, const char* mqttPass){
	_ssid = ssid;
	_wifiPass = wifiPass;
	_mqttServer = mqttServer;
	_mqttPort = mqttPort;
	_mqttUser = mqttUser;
	_mqttPass = mqttPass;
}



class MqttSwitch {
public:
	MqttSwitch(byte relayPin, byte buttonPin, string string);
	bool MqttConnect();
private:
	byte _relayPin;
	byte _buttonPin;
	string _deviceName;
	WiFiClient _wclient;
	PubSubClient _mqttclient;


};

MqttSwitch::MqttSwitch(byte relayPin, byte buttonPin, string deviceName) {
	_relayPin = relayPin;
	_buttonPin = buttonPin;
	_deviceName = deviceName;

	pinMode(_relayPin, OUTPUT);


}

bool MqttSwitch::MqttConnect()
{
	// Loop until we're reconnected
	if (!mqttclient.connected())
	{
		Serial.print("Attempting MQTT connection to ");
		Serial.print(mqtt_server);
		Serial.println("...");
		// Attempt to connect
		//if (client.connect("ESP8266Client", mqtt_user, mqtt_pass))
		if (mqttclient.connect(_deviceName.c_str(), mqttUser, mqttPass))
		{
			Serial.println("connected");
			// Once connected, publish an announcement...
			string startPayload = _deviceName + ' ' + String(wclient.localIP()).c_str();
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




// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(115200);
	pinMode(14, OUTPUT);

	digitalWrite(14, 0);
}

// the loop function runs over and over again until power down or reset
void loop() {
  
	bool level = digitalRead(13);

	Serial.println(level);
	delay(500);

}
