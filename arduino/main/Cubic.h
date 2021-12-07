#include "Motor.h"
#include <BasicLinearAlgebra.h>

#pragma once
class Cubic
{
private:
public:
	BLA::Matrix<9, 9> solveCare();
	Motor motors[3] = {Motor(0), Motor(1), Motor(2)};
	BLA::Matrix<3, 9> solveK();
	BLA::Matrix<9, 9> getA();
	BLA::Matrix<9, 3> getB();
	BLA::Matrix<9, 9> getC();
	BLA::Matrix<9, 3> getD();
	BLA::Matrix<9, 9> getQ();
	BLA::Matrix<3, 3> getR();
	Cubic();
	~Cubic();
};