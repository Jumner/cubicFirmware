#include "Motor.h"
#include "Cubic.h"

Cubic::Cubic()
{
	// Set timers
	TCCR1B = TCCR1B & B11111000 | B00000001; // 9 & 10
	TCCR2B = TCCR2B & B11111000 | B00000001; // 11

	Serial.println("Constructed Cubic");
}
Cubic::~Cubic()
{
	Serial.println("Destructed Cubic");
}