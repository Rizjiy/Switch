/**
 * An example showing how to put ESP8266 into Deep-sleep mode
 */
#include <RBD_Timer.h>
#include <string>
using namespace std;

const int buttonPin =  4; //-1 - ��� ���������� ������
boolean levelButton = LOW; // ������ � ���������� ��������� �� ������ ��� ������� �������
RBD::Timer lockTimer(30); // ������ �� ��������
RBD::Timer lockTimer2(90); // ������ �� ��������

volatile bool lock = false;
volatile boolean rState = false; // � ����������� ������ ���������� ��� volatile ��� ���������� ����������
volatile boolean flagChange = false; // ���� ����� ��� ����, ����� ������������ ��������� �� ������


void setup() {
	Serial.begin(115200);
	Serial.setTimeout(2000);

	if (buttonPin >= 0)
		attachInterrupt(digitalPinToInterrupt(buttonPin), Interrupt_WF, levelButton ? FALLING : RISING);

	// Wait for serial to initialize.
	while (!Serial) {}

	Serial.println("I'm awake.");

	Serial.println("Going into deep sleep for 20 seconds");
	ESP.deepSleep(20e6); // 20e6 is 20 microseconds
}

void loop() {
	// ��� ����������. ���� ������� ����, �� ��������� ��������� �� ������
	if (flagChange) {


		flagChange = false;
	}

}

// �������, ���������� �����������, ��� ������ ��� �������� (button without fixing)
void Interrupt_WF() {

	//������ �� �������� 
	if (lock || !lockTimer2.isExpired())
		return;
	lock = true;
	lockTimer.restart();
	while (!lockTimer.isExpired())
	{
	}

	if (digitalRead(buttonPin) != levelButton)
	{
		OnBtnPress(!rState);
		flagChange = true;
	}

	lockTimer2.restart(); // ���������� �� �/� ������� � ����
	lock = false;
}

void OnBtnPress(bool state)
{
	if (debug)
	{
		Serial.println("OnBtnPress(" + String(state) + ")");
	}

	digitalWrite(mainPin, state);

	//������ ������� ���������
	rState = state;
}
