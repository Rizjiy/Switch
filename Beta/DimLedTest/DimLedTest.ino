
//int led = 13;           // the PWM pin the LED is attached to
int led = 9;           // the PWM pin the LED is attached to
int buttonPin = 12;

boolean btnPress = false;
boolean lastbtnStat = false;

int pwmChannel[7] = { 255, 200, 150, 100, 50, 20, 0 };
int curChanel = 0;

bool debug = true;


void setup()
{
	Serial.begin(115200);
	delay(30);
	Serial.print("Setup");
	pinMode(led, OUTPUT);

	// Ў»ћ 8 разр¤дов, 62,5 к√ц
	//TCCR1A = TCCR1A & 0xe0 | 1;
	//TCCR1B = TCCR1B & 0xe0 | 0x09;

	//10 бит, 244,14 √ц 
	TCCR1A = TCCR1A & 0xe0 | 3;
	TCCR1B = TCCR1B & 0xe0 | 0x0b;


	analogWrite(led, pwmChannel[curChanel]);


}

void loop()
{

	ButtonWf();

}

void ButtonWf() {
	btnPress = digitalRead(buttonPin);

	if (btnPress && !lastbtnStat) {
		delay(30); // защита от дребезга
		btnPress = digitalRead(buttonPin);

		if (btnPress) {
			//определ¤ем следующий режим
			curChanel++;

			if (curChanel >= 7)
				curChanel = 0;

			int brightness = pwmChannel[curChanel];

			OnBtnPress(brightness);
		}
	}
	lastbtnStat = btnPress;
}

void OnBtnPress(int brightness)
{
	
	if (debug)
	{
		Serial.print("OnBtnPress(");
		Serial.print(brightness);
		Serial.println(")");
	}

	//мен¤ем текущее состо¤ние
	analogWrite(led, brightness);
}

