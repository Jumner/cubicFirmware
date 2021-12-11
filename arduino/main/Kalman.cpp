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
	Q(0, 0) = 0.0000435025685738519f;
	Q(1, 1) = 0.000122290036761239f;
	Q(2, 2) = 0.0000743543218f;
	Q(3, 3) = 0.000007711961108f;
	Q(4, 4) = 0.000009068932273f;
	Q(5, 5) = 0.000008164641257f;
	Q(6, 6) = 14.28797333f;
	Q(7, 7) = 14.28797333f;
	Q(8, 8) = 14.28797333f;
	return Q;
}

BLA::Matrix<9, 9> Kalman::getR()
{
	BLA::Matrix<9, 9> R = getQ();
	R(0, 0) = 0.0003f;
	R(1, 1) = 0.0003f;
	R(2, 2) = 0.0003f;
	R(3, 3) = 0.00005f;
	R(4, 4) = 0.00005f;
	R(5, 5) = 0.00005f;
	R(6, 6) = 4.0f;
	R(7, 7) = 4.0f;
	R(8, 8) = 4.0f;
	return R;
}

void Kalman::calculateP(BLA::Matrix<9, 9> A)
{
	// P = A * P * ~A + getQ();
	P = A * P;
	// P *= ~A;
	// P += getQ(); // This uses less memory (we only really have 1kb)
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
	calculateP(A); // Calculate the covariance matrix
	// BLA::Matrix<9> x = aPriori + getK() * (y - aPriori); // Profit 💸💸💸💸
	// finishP();																					 // Update the covariance matrix

	// return x;
	return BLA::Matrix<9>();
}
