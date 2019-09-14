#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include <Wire.h>
//#include <LiquidCrystal_I2C.h> // LCD+I2C

RF24 radio(2, 15);  // указать номера пинов, куда подключаются CE и CSN
const byte rxAddr[6] = "00001"; // адрес

int count = 0;

//LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{
	Serial.begin(115200);
	Serial.println("Start");

	//lcd.init();     // Инициализация lcd   
	//lcd.backlight();   // Включаем подсветку
	//lcd.clear();
	////lcd.setBacklight(LOW);
	//lcd.setCursor(0, 0);
	//lcd.print("Arduino");
	//lcd.setCursor(0, 1);
	//lcd.print("started");
	delay(500);

	radio.begin();
	radio.openReadingPipe(0, rxAddr);
	radio.startListening();

	//lcd.clear();
	//lcd.setCursor(0, 0);
	//lcd.print("listening...");
	delay(500);
}

void loop()
{
	if (radio.available())
	{
		Serial.print("Reseived:");

		char text[32] = { 0 };
		radio.read(&text, sizeof(text));
		Serial.println(text);
		//lcd.clear();
		//lcd.setCursor(0, 0);
		//lcd.print(text);
		//lcd.setCursor(0, 1);
		//lcd.print("count ");
		//lcd.print(count++);
	}
	delay(20);
}