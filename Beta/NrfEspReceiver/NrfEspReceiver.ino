#include <SPI.h>                                          // Подключаем библиотеку  для работы с шиной SPI
#include <RF24Network.h>
#include <RF24.h>                                         // Подключаем библиотеку  для работы с nRF24L01+

#define CE_PIN 2
#define CSN_PIN 15
#define RF24_CHANEL 62 // uint8_t
int led = 4;           // the PWM pin the LED is attached to


const uint16_t sensor_node = 1;
const uint16_t rx_node = 0;

RF24 radio(CE_PIN, CSN_PIN);                              // Создаём объект radio для работы с библиотекой RF24, указывая номера выводов nRF24L01+ (CE, CSN)
RF24Network network(radio);

RF24NetworkHeader header;

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(115200);
	Serial.println("* RF24Network RX *");

	pinMode(led, OUTPUT);

	SwitchPower(true);
	delay(500);
	SwitchPower(false);


	SPI.begin();
	radio.begin();
	network.begin(RF24_CHANEL, rx_node);

}

// the loop function runs over and over again until power down or reset
void loop() {
	network.update();
	while (network.available()) {
		Serial.print("Receive: ");
		bool state;
		network.read(header, &state, sizeof(state));

		Serial.print("Receive: ");
		Serial.println(state);

		SwitchPower(state);

	}
}

void SwitchPower(bool value)
{
	//меняем текущее состояние
	analogWrite(led, value);
}

