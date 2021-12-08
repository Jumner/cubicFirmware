#include "Motor.h"
#include "Kalman.h"
#include <MPU6050_6Axis_MotionApps20.h>
#include <BasicLinearAlgebra.h>
using namespace BLA;
#pragma once

class Cubic
{
public:
	Kalman kalman;
	BLA::Matrix<9> X;
	BLA::Matrix<9> Y;
	BLA::Matrix<3> U;
	Motor motors[3] = {Motor(0), Motor(1), Motor(2)};
	BLA::Matrix<3, 9> getK();
	void measureY(float t[3], VectorInt16 td);
	BLA::Matrix<3> getU();
	BLA::Matrix<9, 9> getA();
	BLA::Matrix<9, 3> getB();

	Cubic();
	~Cubic();
};