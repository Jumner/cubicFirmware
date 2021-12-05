#include "Motor.h"
#include "Cubic.h"
#include "Mpu6050.h"
Cubic::Cubic()
{
	// Bruh
}
Cubic::Cubic(Mpu6050 *mpu)
{
	// Set timers
	TCCR1B = TCCR1B & B11111000 | B00000001; // 9 & 10
	TCCR2B = TCCR2B & B11111000 | B00000001; // 11

	// Construct imu
	imu = mpu;

	Serial.println("Constructed Cubic");
}
Cubic::~Cubic()
{
	Serial.println("Destructed Cubic");
}