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
	Motor motors[3] = { Motor(2), Motor(1), Motor(0) }; // 54 bytes
	float spCorrect[3] = { 0.0, 0.0, 0.0 };
  float pidw[3] = {1.0, 6.0, 0.0001 };
	BLA::Matrix<3, 9> getK();
	void measureY(float t[3], VectorInt16 td);
  bool stop();
	void calculateU(float dt);
	BLA::Matrix<9, 9> getA();
	BLA::Matrix<9, 3> getB();
	void calculateX(VectorInt16 a, VectorInt16 td, float dt);
	void signY(BLA::Matrix<9> aPriori);
	void run(VectorInt16 a, VectorInt16 td, float dt);
	void printState();

	Cubic();

 void getCost(float dt);
 float cost;
 float avgCost;
 float dCost;
 float avgdCost;
};
