#include "Motor.h"
#include "Cubic.h"
#include <BasicLinearAlgebra.h>

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

void Cubic::CalculateA()
{
	float a = 0;
	float s = -0.00001; // Small negative
	float y = mass * 9.81 * l / i[1];
	float z = mass * 9.81 * l / i[2];
	A = {0, 0, 0, 1, 0, 0, 0, 0, 0,
			 0, 0, 0, 0, 1, 0, 0, 0, 0,
			 0, 0, 0, 0, 0, 1, 0, 0, 0,
			 0, 0, 0, 0, 0, 0, 0, 0, 0,
			 0, y, 0, 0, 0, 0, 0, 0, 0,
			 0, 0, z, 0, 0, 0, 0, 0, 0,
			 0, 0, 0, 0, 0, 0, s, 0, 0,
			 0, 0, 0, 0, 0, 0, 0, s, 0,
			 0, 0, 0, 0, 0, 0, 0, 0, s};
}

void Cubic::CalculateB()
{
	float w = 1 / iw;								 // Wheel acceleration
	float x = -1 / (sqrt(3) * i[0]); // X Acceleration
	float y = -2 / (sqrt(6) * i[1]); // Y Acceleration
	float h = 1 / (sqrt(6) * i[1]);	 // Half Y Acceleration
	float z = -1 / (sqrt(2) * i[2]); // Z Acceleration
	float n = 1 / (sqrt(2) * i[2]);	 // Negative Z Acceleration (its acc positive)
	B = {0, 0, 0,
			 0, 0, 0,
			 0, 0, 0,
			 x, x, x,
			 y, h, h,
			 0, z, n,
			 w, 0, 0,
			 0, w, 0,
			 0, 0, w};
}