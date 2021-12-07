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
