#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(2, 15); // CE and CSN
const byte rxAddr[6] = "00001";  // адрес

void setup()
{
	Serial.begin(115200);

	delay(500);
	radio.begin();
	radio.setRetries(15, 15);
	radio.openWritingPipe(rxAddr);
	radio.stopListening();
}

void loop()
{
	const char text[] = "Hello World";
	radio.write(&text, sizeof(text));
	Serial.println("message sended");


	delay(2000);
}