/*
 Name:		Test.ino
 Created:	12/8/2018 6:21:40 PM
 Author:	rodal
*/

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(115200);
	pinMode(14, OUTPUT);

	digitalWrite(14, 0);
}

// the loop function runs over and over again until power down or reset
void loop() {
  
	bool level = digitalRead(13);

	Serial.println(level);
	delay(500);

}
