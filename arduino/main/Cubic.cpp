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

	// Create state space model
	// Serial.println("Creating State Space Model");

	// Create controller
	// Serial.println("Creating Controller");

	// Solve the ricotta 🧀
	// J is minimized by input -Kx (u = -Kx)
	// K = R^-1 BT P
	// AT P + P A - P B R^-1 BT P + Q = 0

	// Serial.println("Constructed Cubic");
}
Cubic::~Cubic()
{
	// Serial.println("Destructed Cubic");
}

BLA::Matrix<3, 9> Cubic::getK()
{
	return BLA::Matrix<3, 9>();
}
