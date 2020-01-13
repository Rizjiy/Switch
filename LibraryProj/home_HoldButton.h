#pragma once
#include "home_MqttButton.h"
class HoldButton :
	public MqttButton
{
	public:
		HoldButton(byte buttonPin, byte relayPin, string buttonName);
};

