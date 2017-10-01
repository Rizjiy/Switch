#include <SPI.h>                                          // Подключаем библиотеку  для работы с шиной SPI
#include <nRF24L01.h>                                     // Подключаем файл настроек из библиотеки RF24
#include <RF24.h>                                         // Подключаем библиотеку  для работы с nRF24L01+

RF24           radio(9, 10);                              // Создаём объект radio   для работы с библиотекой RF24, указывая номера выводов nRF24L01+ (CE, CSN)
int            data[2];                                   // Создаём массив для приёма данных

void setup() {
	Serial.begin(115200);
	Serial.println("setup receiver");
	Serial.print(data[0]);
	Serial.println(data[1]);

	delay(1000);
	radio.begin();                                        // Инициируем работу nRF24L01+
	radio.setChannel(5);                                  // Указываем канал приёма данных (от 0 до 127), 5 - значит приём данных осуществляется на частоте 2,405 ГГц (на одном канале может быть только 1 приёмник и до 6 передатчиков)
	radio.setDataRate(RF24_1MBPS);                   // Указываем скорость передачи данных (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS), RF24_1MBPS - 1Мбит/сек
	radio.setPALevel(RF24_PA_HIGH);                 // Указываем мощность передатчика (RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm)
	radio.openReadingPipe(1, 0x1234567890LL);            // Открываем 1 трубу с идентификатором 0x1234567890 для приема данных (на ожном канале может быть открыто до 6 разных труб, которые должны отличаться только последним байтом идентификатора)
	radio.startListening();                             // Включаем приемник, начинаем прослушивать открытую трубу
														//  radio.stopListening   ();                             // Выключаем приёмник, если потребуется передать данные
}

void loop() {
	delay(5000);
	Serial.println("begin receiv");

	if (radio.available()) {                                // Если в буфере имеются принятые данные
		radio.read(&data, sizeof(data));                  // Читаем данные в массив data и указываем сколько байт читать
		Serial.print(data[0]);
		Serial.println(data[1]);
	}

	Serial.println("end");

}