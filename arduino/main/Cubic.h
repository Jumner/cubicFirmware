#include "Motor.h"
#include "Kalman.h"
#include <MPU6050_6Axis_MotionApps20.h>
#include <BasicLinearAlgebra.h>
using namespace BLA;
#pragma once

class Cubic // 462 bytes
{
public:
	Kalman kalman;																		// 324 bytes
	BLA::Matrix<9> X;																	// 36 bytes
	BLA::Matrix<9> Y;																	// 36 bytes
	BLA::Matrix<3> U;																	// 12 bytes
	Motor motors[3] = {Motor(0), Motor(1), Motor(2)}; // 54 bytes
	BLA::Matrix<3, 9> getK();
	void measureY(float t[3], VectorInt16 td);
	BLA::Matrix<3> getU();
	BLA::Matrix<9, 9> getA();
	BLA::Matrix<9, 3> getB();
	void calculateX(float t[3], VectorInt16 td, float dt);
	void run();
	void printState();

	Cubic();
	~Cubic();
};