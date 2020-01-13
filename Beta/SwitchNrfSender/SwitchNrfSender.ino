#include <SPI.h>
#include <nRF24L01.h>                                     // Подключаем файл настроек из библиотеки RF24
#include <RF24.h>                                         // Подключаем библиотеку для работы с nRF24L01+

const int buttonPin = 4;
RF24 radio(2, 15);                              // Создаём объект radio для работы с библиотекой RF24, указывая номера выводов nRF24L01+ (CE, CSN)
boolean rState1 = false;
boolean btnPress = false;
boolean lastbtnStat = false;
bool            data[1];                                   // Создаём массив для передачи данных



// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(115200);
	Serial.println("Sender start");

	//настройки nRF24
	SPI.setHwCs(true);
	SPI.begin();
	SPI.setDataMode(SPI_MODE0);
	SPI.setBitOrder(MSBFIRST);
	radio.begin();                                        // Инициируем работу nRF24L01+
	radio.setChannel(5);                                  // Указываем канал передачи данных (от 0 до 127), 5 - значит передача данных осуществляется на частоте 2,405 ГГц (на одном канале может быть только 1 приёмник и до 6 передатчиков)
	radio.setDataRate(RF24_1MBPS);                   // Указываем скорость передачи данных (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS), RF24_1MBPS - 1Мбит/сек
	radio.setPALevel(RF24_PA_HIGH);                 // Указываем мощность передатчика (RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm)
	radio.openWritingPipe(0x1234567890LL);               // Открываем трубу с идентификатором 0x1234567890 для передачи данных (на ожном канале может быть открыто до 6 разных труб, которые должны отличаться только последним байтом идентификатора)
	//

}

// the loop function runs over and over again until power down or reset
void loop() {

	ButtonWf();

}

void ButtonWf() {
	btnPress = digitalRead(buttonPin);

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

	//Отправляем по трубе текущее состояние
	radio.write(data, sizeof(data));

	//digitalWrite(relayPin, state);
	rState1 = state;
}
