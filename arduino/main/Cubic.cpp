#include "Motor.h"
#include "Cubic.h"
#include <BasicLinearAlgebra.h>

using namespace BLA;
Cubic::Cubic()
{
	// dont do nothin
}
Cubic::Cubic(bool construct)
{
	delay(1000);
	// Set timers
	// Serial.println("Initializing TCCR1B and TCCR2B timer registers");
	TCCR1B = TCCR1B & B11111000 | B00000001; // 9 & 10
	TCCR2B = TCCR2B & B11111000 | B00000001; // 11

	// Create state space model
	// Serial.println("Creating State Space Model");
	float s = -0.00001;								// Small negative
	float y = mass * 9.81 * l / i[1]; // Y Gravity
	float z = mass * 9.81 * l / i[2]; // Z Gravity
	A = {0, 0, 0, 1, 0, 0, 0, 0, 0,
			 0, 0, 0, 0, 1, 0, 0, 0, 0,
			 0, 0, 0, 0, 0, 1, 0, 0, 0,
			 0, 0, 0, 0, 0, 0, 0, 0, 0,
			 0, y, 0, 0, 0, 0, 0, 0, 0,
			 0, 0, z, 0, 0, 0, 0, 0, 0,
			 0, 0, 0, 0, 0, 0, s, 0, 0,
			 0, 0, 0, 0, 0, 0, 0, s, 0,
			 0, 0, 0, 0, 0, 0, 0, 0, s};
	float w = 1 / iw;								 // Wheel acceleration
	float x = -1 / (sqrt(3) * i[0]); // X Acceleration
	y = -2 / (sqrt(6) * i[1]);			 // Y Acceleration
	float h = 1 / (sqrt(6) * i[1]);	 // Half Y Acceleration
	z = -1 / (sqrt(2) * i[2]);			 // Z Acceleration
	float n = 1 / (sqrt(2) * i[2]);	 // Negative Z Acceleration (its acc positive)
	B = {0, 0, 0,
			 0, 0, 0,
			 0, 0, 0,
			 x, x, x,
			 y, h, h,
			 0, z, n,
			 w, 0, 0,
			 0, w, 0,
			 0, 0, w};
	C = BLA::Identity<9, 9>();
	// D is left at all zeros

	// Create controller
	// Serial.println("Creating Controller");
	Q(0, 0) = 0.1; // Theta x
	Q(1, 1) = 1;	 // Theta y
	Q(2, 2) = 1;	 // Theta z
	Q(3, 3) = 1;	 // Theta Dot x
	Q(4, 4) = 1;	 // Theta Dot y
	Q(5, 5) = 1;	 // Theta Dot z
	Q(6, 6) = 2;	 // W Dot 1
	Q(7, 7) = 2;	 // W Dot 2
	Q(8, 8) = 2;	 // W Dot 3

	R(0, 0) = 0.8; // Torque 1
	R(1, 1) = 0.8; // Torque 2
	R(2, 2) = 0.8; // Torque 3

	// Solve the ricotta 🧀

	// J is minimized by input -Kx (u = -Kx)
	// K = R^-1 BT P
	// AT P + P A - P B R^-1 BT P + Q = 0

	// BLA::Matrix<9, 9> X = solveCare(); // Continuous-time Algebraic Riccati Equation

	// Serial.println("Constructed Cubic");
}
Cubic::~Cubic()
{
	// Serial.println("Destructed Cubic");
}

BLA::Matrix<9, 9> Cubic::getQ()
{

	return Q;
}

BLA::Matrix<3, 3> Cubic::getR()
{
	BLA::Matrix<3, 3> R;
	return R;
}

BLA::Matrix<9, 9> Cubic::solveCare()
{
	// BLA::Matrix<9, 9> Q = getQ();
	// BLA::Matrix<3, 3> RInv = getR();
	// Invert(RInv);
	// BLA::Matrix<9, 9> P = Q;
	BLA::Matrix<9, 9> Pn = Q;
	// for (auto _ = 0; _ < 10; _++)
	// {
	// 	Pn = ~model->A * P + P * model->A - P * model->B * RInv * ~model->B * P + Q;
	// }
	return Pn;
	// https://en.wikipedia.org/wiki/Algebraic_Riccati_equation
	// https://github.com/giacomo-b/CppRobotics/blob/master/include/robotics/linear_control/lqr.hpp shhh 🤫
	// https://github.com/tomstewart89/StateSpaceControl
	// AT P + P A - P B R^-1 BT P + Q = 0
}