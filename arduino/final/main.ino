#include "Motor.h"
#include "Cubic.h"
#include "PinChangeInterrupt.h"
#include <Wire.h>
Cubic cube;
void setup()
{
	Serial.begin(9600);
	Wire.begin(); // Join bus as master

	delay(500);
	Wire.beginTransmission(0b1101000); // Connect to mpu 6050
	Wire.write(0x1b);
	Wire.write(0b00000000); // Set gyro to +- 250 deg/s
	// 000 00 000. 00 -> 250, 01 -> 500, 10 -> 1000, 11 -> 2000
	Wire.endTransmission();

	Wire.beginTransmission(0b1101000); // Connect to mpu 6050
	Wire.write(0x1c);
	Wire.write(0b00000000); // Set gyro to += 2g
	// 000 00 000. 00 -> 2g, 01 -> 4g, 10 -> 8g, 11 -> 16g
	Wire.endTransmission();
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

//Mpu must be in main for some reason