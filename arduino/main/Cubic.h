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

	Cubic();
	Cubic(bool construct);
	~Cubic();
};