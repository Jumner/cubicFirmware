#include "Motor.h"
#include <MPU6050_6Axis_MotionApps20.h>
#include <BasicLinearAlgebra.h>

#pragma once
class Cubic
{
private:
public:
	Motor motors[3] = {Motor(0), Motor(1), Motor(2)};
	MPU6050 *imu = nullptr;
	BLA::Matrix<9, 9> A;
	BLA::Matrix<9, 3> B;
	BLA::Identity<9, 9> C;
	BLA::Zeros<9, 3> D;

	// Values
	float mass = 0.8;				// Mass in kg
	float l = 0.4;					// Length of pendulum arm
	float i[3] = {1, 1, 1}; // Inertia (x, y, z)
	float iw = 0.5;					// Wheen inertia

	void CalculateA();
	void CalculateB();
	Cubic();
	Cubic(MPU6050 *mpu);
	~Cubic();
};