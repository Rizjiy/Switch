#include <ESP8266WiFi.h>
#include <SPI.h>
#include <nRF24L01.h>                                     // Подключаем файл настроек из библиотеки RF24
#include <RF24.h>                                         // Подключаем библиотеку для работы с nRF24L01+
RF24           radio(4, 15);                              // Создаём объект radio для работы с библиотекой RF24, указывая номера выводов nRF24L01+ (CE, CSN)
int            data[2];                                   // Создаём массив для передачи данных

void setup() {
	Serial.begin(115200);
	Serial.println("setup");
	SPI.setHwCs(true);
	SPI.begin();
	SPI.setDataMode(SPI_MODE0);
	SPI.setBitOrder(MSBFIRST);
	radio.begin();                                        // Инициируем работу nRF24L01+
	radio.setChannel(5);                                  // Указываем канал передачи данных (от 0 до 127), 5 - значит передача данных осуществляется на частоте 2,405 ГГц (на одном канале может быть только 1 приёмник и до 6 передатчиков)
	radio.setDataRate(RF24_1MBPS);                   // Указываем скорость передачи данных (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS), RF24_1MBPS - 1Мбит/сек
	radio.setPALevel(RF24_PA_HIGH);                 // Указываем мощность передатчика (RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm)
	radio.openWritingPipe(0x1234567890LL);               // Открываем трубу с идентификатором 0x1234567890 для передачи данных (на ожном канале может быть открыто до 6 разных труб, которые должны отличаться только последним байтом идентификатора)
}

void loop() {
	delay(5000);
	Serial.println("begin");
	data[0] = 1;                             // считываем показания Trema слайдера с вывода A1 и записываем их в 0 элемент массива data
	data[1] = 0;                             // считываем показания Trema потенциометра с вывода A2 и записываем их в 1 элемент массива data
	radio.write(&data, sizeof(data));                     // отправляем данные из массива data указывая сколько байт массива мы хотим отправить
	Serial.println("end");
}