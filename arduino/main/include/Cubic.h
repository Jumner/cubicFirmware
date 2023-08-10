#include "Motor.h"
#include <BasicLinearAlgebra.h>
#include <MPU6050_6Axis_MotionApps20.h>
using namespace BLA;
#pragma once

class Cubic // 462 bytes
{
	public:
	BLA::Matrix<9> X;																		// 36 bytes
	BLA::Matrix<9> Y;																		// 36 bytes
	BLA::Matrix<3> U;																		// 12 bytes
	Motor motors[3] = { Motor(0), Motor(1), Motor(2) }; // 54 bytes
	float spCorrect[2] = { 0.0, 0.0 };
	float integral[3] = { 0.0, 0.0 };
	void measureY(BLA::Matrix<9> aPriori, VectorInt16 td);
	void calculateU(float dt);
	bool stop();
	BLA::Matrix<9, 9> getA();
	BLA::Matrix<9, 3> getB();
	void calculateX(VectorInt16 td, float dt);
	void run(VectorInt16 td, float dt);

	Cubic();
};
