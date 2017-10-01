#include <SPI.h>                                          // ���������� ����������  ��� ������ � ����� SPI
#include <nRF24L01.h>                                     // ���������� ���� �������� �� ���������� RF24
#include <RF24.h>                                         // ���������� ����������  ��� ������ � nRF24L01+

RF24           radio(9, 10);                              // ������ ������ radio   ��� ������ � ����������� RF24, �������� ������ ������� nRF24L01+ (CE, CSN)
int            data[2];                                   // ������ ������ ��� ����� ������

void setup() {
	Serial.begin(115200);
	Serial.println("setup receiver");
	Serial.print(data[0]);
	Serial.println(data[1]);

	delay(1000);
	radio.begin();                                        // ���������� ������ nRF24L01+
	radio.setChannel(5);                                  // ��������� ����� ����� ������ (�� 0 �� 127), 5 - ������ ���� ������ �������������� �� ������� 2,405 ��� (�� ����� ������ ����� ���� ������ 1 ������� � �� 6 ������������)
	radio.setDataRate(RF24_1MBPS);                   // ��������� �������� �������� ������ (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS), RF24_1MBPS - 1����/���
	radio.setPALevel(RF24_PA_HIGH);                 // ��������� �������� ����������� (RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm)
	radio.openReadingPipe(1, 0x1234567890LL);            // ��������� 1 ����� � ��������������� 0x1234567890 ��� ������ ������ (�� ����� ������ ����� ���� ������� �� 6 ������ ����, ������� ������ ���������� ������ ��������� ������ ��������������)
	radio.startListening();                             // �������� ��������, �������� ������������ �������� �����
														//  radio.stopListening   ();                             // ��������� �������, ���� ����������� �������� ������
}

void loop() {
	delay(5000);
	Serial.println("begin receiv");

	if (radio.available()) {                                // ���� � ������ ������� �������� ������
		radio.read(&data, sizeof(data));                  // ������ ������ � ������ data � ��������� ������� ���� ������
		Serial.print(data[0]);
		Serial.println(data[1]);
	}

	Serial.println("end");

}