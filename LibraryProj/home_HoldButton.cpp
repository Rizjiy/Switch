#include "home_HoldButton.h"

HoldButton::HoldButton(byte buttonPin, byte relayPin, string buttonName)
	:MqttButton(buttonPin,relayPin,buttonName)
{
}

