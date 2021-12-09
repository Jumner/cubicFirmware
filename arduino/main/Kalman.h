#include <BasicLinearAlgebra.h>

#pragma once

class Kalman // 324 bytes
{
private:
	BLA::Matrix<9, 9> P; // 324 bytes
	BLA::Matrix<9, 9> getQ();
	BLA::Matrix<9, 9> getR();
	void calculateP(BLA::Matrix<9, 9> A);
	BLA::Matrix<9, 9> getK();
	void finishP();

public:
	Kalman();
	~Kalman();
	BLA::Matrix<9> getPosterior(BLA::Matrix<9> aPriori, BLA::Matrix<9> y, BLA::Matrix<9, 9> A);
};