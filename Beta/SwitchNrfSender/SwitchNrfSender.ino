#include <SPI.h>
#include <nRF24L01.h>                                     // ���������� ���� �������� �� ���������� RF24
#include <RF24.h>                                         // ���������� ���������� ��� ������ � nRF24L01+

const int buttonPin = 4;
RF24 radio(2, 15);                              // ������ ������ radio ��� ������ � ����������� RF24, �������� ������ ������� nRF24L01+ (CE, CSN)
boolean rState1 = false;
boolean btnPress = false;
boolean lastbtnStat = false;
bool            data[1];                                   // ������ ������ ��� �������� ������



// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(115200);

	//��������� nRF24
	SPI.setHwCs(true);
	SPI.begin();
	SPI.setDataMode(SPI_MODE0);
	SPI.setBitOrder(MSBFIRST);
	radio.begin();                                        // ���������� ������ nRF24L01+
	radio.setChannel(5);                                  // ��������� ����� �������� ������ (�� 0 �� 127), 5 - ������ �������� ������ �������������� �� ������� 2,405 ��� (�� ����� ������ ����� ���� ������ 1 ������� � �� 6 ������������)
	radio.setDataRate(RF24_1MBPS);                   // ��������� �������� �������� ������ (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS), RF24_1MBPS - 1����/���
	radio.setPALevel(RF24_PA_HIGH);                 // ��������� �������� ����������� (RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm)
	radio.openWritingPipe(0x1234567890LL);               // ��������� ����� � ��������������� 0x1234567890 ��� �������� ������ (�� ����� ������ ����� ���� ������� �� 6 ������ ����, ������� ������ ���������� ������ ��������� ������ ��������������)
	//

}

// the loop function runs over and over again until power down or reset
void loop() {

	ButtonWf();

}

void ButtonWf() {
	btnPress = digitalRead(buttonPin);

	if (btnPress && !lastbtnStat) {
		delay(30); // ������ �� ��������
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

	//���������� �� ����� ������� ���������
	radio.write(data, sizeof(data));

	//digitalWrite(relayPin, state);
	rState1 = state;
}
