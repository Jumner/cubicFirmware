#include "Motor.h"
#include "Cubic.h"
#include <BasicLinearAlgebra.h>

// Values
#define s -0.00001 // Small neg value
#define mass 0.8	 // Mass in kg
#define l 0.4			 // Length of pendulum arm
#define ix 1			 // Inertia x
#define iy 1			 // Inertia y
#define iz 1			 // Inertia z
#define iw 0.5		 // Wheen inertia

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

BLA::Matrix<9, 9> Cubic::getA()
{
	float y = mass * 9.81 * l / iy; // Y Gravity
	float z = mass * 9.81 * l / iz; // Z Gravity
	return {0, 0, 0, 1, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 1, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 1, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, y, 0, 0, 0, 0, 0, 0, 0,
					0, 0, z, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, s, 0, 0,
					0, 0, 0, 0, 0, 0, 0, s, 0,
					0, 0, 0, 0, 0, 0, 0, 0, s};
}

BLA::Matrix<9, 3> Cubic::getB()
{
	float w = 1 / iw;							 // Wheel acceleration
	float x = -1 / (sqrt(3) * ix); // X Acceleration
	float y = -2 / (sqrt(6) * iy); // Y Acceleration
	float h = 1 / (sqrt(6) * iy);	 // Half Y Acceleration
	float z = -1 / (sqrt(2) * iz); // Z Acceleration
	float n = 1 / (sqrt(2) * iz);	 // Negative Z Acceleration (its acc positive)
	return {0, 0, 0,
					0, 0, 0,
					0, 0, 0,
					x, x, x,
					y, h, h,
					0, z, n,
					w, 0, 0,
					0, w, 0,
					0, 0, w};
}

BLA::Matrix<9, 9> Cubic::getC()
{
	return BLA::Identity<9, 9>();
}

BLA::Matrix<9, 3> Cubic::getD()
{
	return BLA::Matrix<9, 3>(); // Defaults to 0
}

BLA::Matrix<9, 9> Cubic::getQ()
{
	BLA::Matrix<9, 9> Q;
	Q(0, 0) = 0.1; // Theta x
	Q(1, 1) = 1;	 // Theta y
	Q(2, 2) = 1;	 // Theta z
	Q(3, 3) = 1;	 // Theta Dot x
	Q(4, 4) = 1;	 // Theta Dot y
	Q(5, 5) = 1;	 // Theta Dot z
	Q(6, 6) = 2;	 // W Dot 1
	Q(7, 7) = 2;	 // W Dot 2
	Q(8, 8) = 2;	 // W Dot 3
	return Q;
}

BLA::Matrix<3, 3> Cubic::getR()
{
	BLA::Matrix<3, 3> R;
	R(0, 0) = 0.8; // Torque 1
	R(1, 1) = 0.8; // Torque 2
	R(2, 2) = 0.8; // Torque 3
	return R;
}

BLA::Matrix<3, 9> Cubic::solveK()
{
	return (Inverse(getR()) * ~getB() * solveCare());
	// K = R^-1 BT P
}

BLA::Matrix<9, 9> Cubic::solveCare()
{
	BLA::Matrix<9, 9> P;

	for (uint8_t _ = 0; _ < 10; _++)
	{
		P = ~getA() * P + P * getA() - P * getB() * Inverse(getR()) * ~getB() * P + getQ();
	}
	return P;
	// https://en.wikipedia.org/wiki/Algebraic_Riccati_equation
	// https://github.com/giacomo-b/CppRobotics/blob/master/include/robotics/linear_control/lqr.hpp shhh 🤫
	// https://github.com/tomstewart89/StateSpaceControl
	// AT P + P A - P B R^-1 BT P + Q = 0
}