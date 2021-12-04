#include "Motor.h"
#include "Cubic.h"
#include "PinChangeInterrupt.h"
Cubic cube;
void setup()
{
	Serial.begin(9600);
	delay(500);
	Serial.println("yo");

	cube = Cubic();
	// Attach tachometer interrupts
	attachPCINT(digitalPinToPCINT(2), int0, CHANGE);
	attachPCINT(digitalPinToPCINT(3), int1, CHANGE);
	attachPCINT(digitalPinToPCINT(4), int2, CHANGE);

	// cube.motors[0].Print();
	// Serial.println(cube.motors[0].rps);
}

void loop()
{
	// cube.motors[0].setTorque(0.005);
}

void int0(void)
{
	cube.motors[0].interrupt();
}
void int1(void)
{
	cube.motors[1].interrupt();
}
void int2(void)
{
	cube.motors[2].interrupt();
}