#include <BasicLinearAlgebra.h>

#pragma once

class Kalman
{
private:
	BLA::Matrix<9, 9> P;
	BLA::Matrix<9, 9> K;
	BLA::Matrix<9, 9> getQ();
	BLA::Matrix<9, 9> getR();
	void calculateP(BLA::Matrix<9, 9> A);
	void calculateK();
	void finishP();

public:
	Kalman();
	~Kalman();
	BLA::Matrix<9> getPosterior(BLA::Matrix<9> aPriori, BLA::Matrix<9> y, BLA::Matrix<9, 9> A);
};