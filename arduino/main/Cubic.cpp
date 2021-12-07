#include "Motor.h"
#include "Cubic.h"
#include <BasicLinearAlgebra.h>

using namespace BLA;
Cubic::Cubic()
{
	// Set timers
	// Serial.println("Initializing TCCR1B and TCCR2B timer registers");
	TCCR1B = TCCR1B & B11111000 | B00000001; // 9 & 10
	TCCR2B = TCCR2B & B11111000 | B00000001; // 11
	K = getK();
	// Serial.println("Constructed Cubic");
}
Cubic::~Cubic()
{
	// Serial.println("Destructed Cubic");
}

BLA::Matrix<3, 9> Cubic::getK()
{ // This was precomputed with octave (open sauce matlab)
	return {-0.57735, -11.007, -1.1515e-11, -1.5275, -6.2491, -9.3816e-12, -0.66665, 0.33333, 0.33333,
					-0.57735, 5.5037, -9.5326, -1.5275, 3.1245, -5.4118, 0.33333, -0.66665, 0.33333,
					-0.57735, 5.5037, 9.5326, -1.5275, 3.1245, 5.4118, 0.33333, 0.33333, -0.66665};
}
BLA::Matrix<9> Cubic::getX(float t[3], VectorInt16 td)
{
	BLA::Matrix<9> X = {t[0], t[1], t[2], td.x / 7509.87263606, td.y / 7509.87263606, td.z / 7509.87263606, motors[0].rps, motors[1].rps, motors[2].rps};
	return X;
}
BLA::Matrix<3> Cubic::getU(float t[3], VectorInt16 td)
{
	BLA::Matrix<9> X = getX(t, td); // Note: Arrays are not matrices
	BLA::Matrix<3> U = -K * X;
	return U;
}
