
int buttonPin = 12;

boolean btnPress = false;
boolean lastbtnStat = false;

bool debug = true;


void setup()
{

  /* add setup code here */

}

void loop()
{

  /* add main program code here */

}

void ButtonWf() {
	btnPress = digitalRead(buttonPin);

	if (btnPress && !lastbtnStat) {
		delay(30); // защита от дребезга
		btnPress = digitalRead(buttonPin);

		if (btnPress) {
			OnBtnPress();
		}
	}
	lastbtnStat = btnPress;
}

void OnBtnPress()
{
	
	
	if (debug)
	{
		Serial.print("OnBtnPress(");
		Serial.print(state);
		Serial.println(")");
	}

	//меняем текущее состояние
	rState1 = state;
}

