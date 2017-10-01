#include <ESP8266WiFi.h>
#include <SPI.h>
#include <nRF24L01.h>                                     // ���������� ���� �������� �� ���������� RF24
#include <RF24.h>                                         // ���������� ���������� ��� ������ � nRF24L01+
RF24           radio(4, 15);                              // ������ ������ radio ��� ������ � ����������� RF24, �������� ������ ������� nRF24L01+ (CE, CSN)
int            data[2];                                   // ������ ������ ��� �������� ������

void setup() {
	Serial.begin(115200);
	Serial.println("setup");
	SPI.setHwCs(true);
	SPI.begin();
	SPI.setDataMode(SPI_MODE0);
	SPI.setBitOrder(MSBFIRST);
	radio.begin();                                        // ���������� ������ nRF24L01+
	radio.setChannel(5);                                  // ��������� ����� �������� ������ (�� 0 �� 127), 5 - ������ �������� ������ �������������� �� ������� 2,405 ��� (�� ����� ������ ����� ���� ������ 1 ������� � �� 6 ������������)
	radio.setDataRate(RF24_1MBPS);                   // ��������� �������� �������� ������ (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS), RF24_1MBPS - 1����/���
	radio.setPALevel(RF24_PA_HIGH);                 // ��������� �������� ����������� (RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm)
	radio.openWritingPipe(0x1234567890LL);               // ��������� ����� � ��������������� 0x1234567890 ��� �������� ������ (�� ����� ������ ����� ���� ������� �� 6 ������ ����, ������� ������ ���������� ������ ��������� ������ ��������������)
}

void loop() {
	delay(5000);
	Serial.println("begin");
	data[0] = 1;                             // ��������� ��������� Trema �������� � ������ A1 � ���������� �� � 0 ������� ������� data
	data[1] = 0;                             // ��������� ��������� Trema ������������� � ������ A2 � ���������� �� � 1 ������� ������� data
	radio.write(&data, sizeof(data));                     // ���������� ������ �� ������� data �������� ������� ���� ������� �� ����� ���������
	Serial.println("end");
}