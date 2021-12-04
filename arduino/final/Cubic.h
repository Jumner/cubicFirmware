#include "Motor.h"
#include "Mpu6050.h"
#pragma once
class Cubic
{
private:
public:
	Motor motors[3] = {Motor(0), Motor(1), Motor(2)};
	// Mpu6050 imu;
	Cubic();
	~Cubic();
};