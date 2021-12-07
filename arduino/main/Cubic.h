#include "Motor.h"
#include <MPU6050_6Axis_MotionApps20.h>
#include <BasicLinearAlgebra.h>
using namespace BLA;
#pragma once
class Cubic
{
private:
	BLA::Matrix<3, 9> K;

public:
	Motor motors[3] = {Motor(0), Motor(1), Motor(2)};
	BLA::Matrix<3, 9> getK();
	BLA::Matrix<9> getX(float t[3], VectorInt16 td);
	BLA::Matrix<3> getU(float t[3], VectorInt16 td);
	Cubic();
	~Cubic();
};