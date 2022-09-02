#include "Motor.h"
#include <BasicLinearAlgebra.h>
#include <MPU6050_6Axis_MotionApps20.h>
using namespace BLA;
#pragma once

class Cubic // 462 bytes
{
	public:
	BLA::Matrix<6> X;																		// 36 bytes
	BLA::Matrix<6> Y;																		// 36 bytes
	BLA::Matrix<2> U;																		// 12 bytes
	Motor motors[3] = { Motor(2), Motor(1), Motor(0) }; // 54 bytes
	float spCorrect[2] = { 0.0, 0.0 };
  float pidw[3] = {1.0, 6.0, 0.0000 };
	void measureY(float t[2], VectorInt16 td);
  bool stop();
	void calculateU(float dt);
	BLA::Matrix<6, 6> getA();
	BLA::Matrix<6, 2> getB();
	void calculateX(VectorInt16 a, VectorInt16 td, float dt);
	void signY(BLA::Matrix<6> aPriori);
	void run(VectorInt16 a, VectorInt16 td, float dt);
	void printState();

	Cubic();
};
