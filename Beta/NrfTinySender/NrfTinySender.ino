
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <RF24Network.h>
#include <RF24.h>

	// Routines to set and claer bits (used in the sleep code)
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define CE_PIN 3
#define CSN_PIN 4 //Since we are using 3 pin configuration we will use same pin for both CE and CSN
#define RF24_CHANEL 62 // uint8_t
const uint16_t sensor_node = 1;
const uint16_t rx_node = 0;

int buttonPin = 5;
boolean rState1 = false;
boolean btnPress = false;
boolean lastbtnStat = false;
bool            data[1];                                   // —оздаЄм массив дл¤ передачи данных
long debugMillis = 0;


RF24 radio(CE_PIN, CSN_PIN);
RF24Network network(radio);
RF24NetworkHeader header(rx_node, 'L');

// the setup function runs once when you press reset or power the board
void setup() {
	//Serial.begin(115200);
	//Serial.println("* Sender setup *");

	// »нициализаци¤ радиомодул¤
	radio.begin();
	network.begin(RF24_CHANEL, sensor_node);
}

// the loop function runs over and over again until power down or reset
void loop() {
	ButtonWf();

}

void ButtonWf() {
	btnPress = digitalRead(buttonPin);

	if (millis() - debugMillis > 2000)
	{
		Serial.println(btnPress);
		debugMillis = millis();
	}

	if (btnPress && !lastbtnStat) {
		delay(30); // защита от дребезга
		btnPress = digitalRead(buttonPin);

		if (btnPress) {
			OnBtnPress(!rState1);
		}
	}
	lastbtnStat = btnPress;
}


void OnBtnPress(bool state)
{
	Serial.print("OnBtnPress(");
	Serial.print(state);
	Serial.println(")");

	//uint8_t power = 0;
	//if (state)
	//	power = 255;
	data[0] = state;

	//ќтправл¤ем по трубе текущее состо¤ние
	radio.powerUp();
	network.write(header, data, sizeof(data));
	radio.powerDown();

	//digitalWrite(relayPin, state);
	rState1 = state;
}
