
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define BME280_ADDRESS  (0x77)
#define BME280_ADDRESS2  (0x76)

Adafruit_BME280 bme; // I2C
Adafruit_BME280 bme2; // I2C


unsigned long delayTime;

void setup() {
	\
	Serial.begin(115200);

	// default settings
	// (you can also pass in a Wire library object like &Wire2)
	Serial.print(F("BME280 start "));
	bool status = bme.begin(BME280_ADDRESS);
	Serial.println(status);

	Serial.print(F("BME280-2 start "));
	bool status2 = bme2.begin(BME280_ADDRESS2);
	Serial.println(status);


	Serial.println("-- Default Test --");
	delayTime = 3000;

	Serial.println();
}


void loop() {
	printValues();
	delay(delayTime);
}


void printValues() {
	Serial.print("Temperature = ");
	Serial.print(bme.readTemperature());
	Serial.println(" *C");
	Serial.print("Temperature2 = ");
	Serial.print(bme2.readTemperature());
	Serial.println(" *C");


	Serial.print("Pressure = ");
	Serial.print(bme.readPressure() / 100.0F);
	Serial.println(" hPa");
	Serial.print("Pressure2 = ");
	Serial.print(bme2.readPressure() / 100.0F);
	Serial.println(" hPa");


	//Serial.print("Approx. Altitude = ");
	//Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
	//Serial.println(" m");

	Serial.print("Humidity = ");
	Serial.print(bme.readHumidity());
	Serial.println(" %");
	Serial.print("Humidity2 = ");
	Serial.print(bme2.readHumidity());
	Serial.println(" %");


	Serial.println();

}