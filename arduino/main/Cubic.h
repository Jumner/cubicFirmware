#include "Motor.h"
#include <BasicLinearAlgebra.h>

#pragma once
class Cubic
{
private:
	BLA::Matrix<9, 9> solveCare();

public:
	Motor motors[3] = {Motor(0), Motor(1), Motor(2)};
	BLA::Matrix<9, 9> A;
	BLA::Matrix<9, 3> B;
	BLA::Matrix<9, 9> C;
	BLA::Matrix<9, 3> D;
	BLA::Matrix<9, 9> Q;
	BLA::Matrix<3, 3> R;

	// Values
	float mass = 0.8;				// Mass in kg
	float l = 0.4;					// Length of pendulum arm
	float i[3] = {1, 1, 1}; // Inertia (x, y, z)
	float iw = 0.5;					// Wheen inertia

	Cubic();
	Cubic(bool construct);
	~Cubic();
};