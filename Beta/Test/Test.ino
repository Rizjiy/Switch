/**
 * An example showing how to put ESP8266 into Deep-sleep mode
 */
#include <RBD_Timer.h>
#include <string>
using namespace std;

const int buttonPin =  4; //-1 - нет физической кнопки
boolean levelButton = LOW; // Сигнал в нормальном состоянии на кнопке или датчике касания
RBD::Timer lockTimer(30); // защита от дребезга
RBD::Timer lockTimer2(90); // защита от дребезга

volatile bool lock = false;
volatile boolean rState = false; // В прерываниях всегда используем тип volatile для изменяемых переменных
volatile boolean flagChange = false; // Флаг нужен для того, чтобы опубликовать сообщение на брокер


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
	// Для прерывания. Если запущен флаг, то публикуем состояние на брокер
	if (flagChange) {


		flagChange = false;
	}

}

// Функция, вызываемая прерыванием, для кнопки без фиксации (button without fixing)
void Interrupt_WF() {

	//Защита от дребезга 
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

	lockTimer2.restart(); // защищаемся от э/м скачков в реле
	lock = false;
}

void OnBtnPress(bool state)
{
	if (debug)
	{
		Serial.println("OnBtnPress(" + String(state) + ")");
	}

	digitalWrite(mainPin, state);

	//меняем текущее состояние
	rState = state;
}
