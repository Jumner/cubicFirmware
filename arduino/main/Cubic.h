#include "Motor.h"
#include <BasicLinearAlgebra.h>

#pragma once
class Cubic
{
private:
public:
	Motor motors[3] = {Motor(0), Motor(1), Motor(2)};
	BLA::Matrix<3, 9> getK();
	Cubic();
	~Cubic();
};