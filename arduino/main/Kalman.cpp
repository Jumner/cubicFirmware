#include <BasicLinearAlgebra.h>
#include "Kalman.h"

using namespace BLA;

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
	// P = A * P * ~A + getQ();
	P = A * P;
	P *= ~A;
	P += getQ(); // This uses less memory (we only really have 1kb)
}

BLA::Matrix<9, 9> Kalman::getK()
{
	return P * Inverse(P + getR()); // 😅
}

void Kalman::finishP()
{
	P = (BLA::Identity<9, 9>() - getK()) * P; // Calculate the kalman gain matrix (not the feedback gain matrix from lqr)
}

BLA::Matrix<9> Kalman::getPosterior(BLA::Matrix<9> aPriori, BLA::Matrix<9> y, BLA::Matrix<9, 9> A)
{
	calculateP(A);																			 // Calculate the covariance matrix
	BLA::Matrix<9> x = aPriori + getK() * (y - aPriori); // Profit 💸💸💸💸
	finishP();																					 // Update the covariance matrix

	return x;
}
