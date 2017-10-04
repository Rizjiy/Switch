//for Arduino Pro or Pro Mini w/ ATmega328

#include <SPI.h>                                          // ���������� ����������  ��� ������ � ����� SPI
#include <nRF24L01.h>                                     // ���������� ���� �������� �� ���������� RF24
#include <RF24.h>                                         // ���������� ����������  ��� ������ � nRF24L01+

int led = 9;           // the PWM pin the LED is attached to
RF24           radio(7, 8);                              // ������ ������ radio   ��� ������ � ����������� RF24, �������� ������ ������� nRF24L01+ (CE, CSN)


void setup() {
	Serial.begin(115200);
	Serial.print("Setup");
	pinMode(led, OUTPUT);

	//10 ���, 244,14 Gz 
	TCCR1A = TCCR1A & 0xe0 | 3;
	TCCR1B = TCCR1B & 0xe0 | 0x0b;

	radio.begin();                                        // ���������� ������ nRF24L01+
	radio.setChannel(5);                                  // ��������� ����� ����� ������ (�� 0 �� 127), 5 - ������ ���� ������ �������������� �� ������� 2,405 ��� (�� ����� ������ ����� ���� ������ 1 ������� � �� 6 ������������)
	radio.setDataRate(RF24_1MBPS);                   // ��������� �������� �������� ������ (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS), RF24_1MBPS - 1����/���
	radio.setPALevel(RF24_PA_HIGH);                 // ��������� �������� ����������� (RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm)
	radio.openReadingPipe(1, 0x1234567890LL);            // ��������� 1 ����� � ��������������� 0x1234567890 ��� ������ ������ (�� ����� ������ ����� ���� ������� �� 6 ������ ����, ������� ������ ���������� ������ ��������� ������ ��������������)
	radio.startListening();                             // �������� ��������, �������� ������������ �������� �����


}

// the loop function runs over and over again until power down or reset
void loop() {
	if (radio.available()) {                                // ���� � ������ ������� �������� ������
		
		uint8_t power;
		radio.read(&power, sizeof(uint8_t));

		Serial.print("Receive: ");
		Serial.println(power);

		ChangePower(power);
	}
}

void ChangePower(int brightness)
{
	//������ ������� ���������
	analogWrite(led, brightness);
}

