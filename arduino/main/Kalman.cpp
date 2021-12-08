#include <BasicLinearAlgebra.h>
#include "Kalman.h"

Kalman::Kalman()
{
}

Kalman::~Kalman()
{
}

BLA::Matrix<9, 9> Kalman::getQ()
{
	BLA::Matrix<9, 9> Q;
	return Q;
}

BLA::Matrix<9, 9> Kalman::getR()
{
	BLA::Matrix<9, 9> R;
	return R;
}

void Kalman::calculateP(BLA::Matrix<9, 9> A)
{
	P = A * P * ~A + getQ();
}

void Kalman::calculateK()
{
	K = (P) / (P + getR()); // whyyyy 😭
}

void Kalman::finishP()
{
	P = (BLA::Identity<9, 9>() - K) * P;
}

BLA::Matrix<9> Kalman::getPosterior(BLA::Matrix<9> aPriori, BLA::Matrix<9> y, BLA::Matrix<9, 9> A)
{
	calculateP(A); // Calculate the covariance matrix
	calculateK();	 // Calculate the kalman gain matrix (not the feedback gain matrix from lqr)
	finishP();		 // Update the covariance matrix

	BLA::Matrix<9> x = aPriori + K * (y - aPriori); // Profit 💸💸💸💸
	return x;
}

/*
public:
	Kalman();
	~Kalman();
	BLA::Matrix<9, 9> calculateP();
	*/