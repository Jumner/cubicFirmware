#include "Mpu6050.h"
#include <Arduino.h>

Mpu6050::Mpu6050()
{
	Serial.println("Constructed Mpu-6050");
}

Mpu6050::~Mpu6050()
{
	Serial.println("Destructed Mpu-6050");
}
