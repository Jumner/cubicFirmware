#include "Motor.h"
#include "Cubic.h"
#include <BasicLinearAlgebra.h>
#include <StateSpaceControl.h>

Cubic::Cubic()
{

	// Set timers
	TCCR1B = TCCR1B & B11111000 | B00000001; // 9 & 10
	TCCR2B = TCCR2B & B11111000 | B00000001; // 11

	// Create state space model

	model = new Model<9, 3, 9>;
	model->A = getA();
	model->B = getB();
	model->C = getC();
	model->D = getD();

	// Solve the ricotta 🧀

	// J is minimized by input -Kx (u = -Kx)
	// K = R^-1 BT P
	// AT P + P A - P B R^-1 BT P + Q = 0

	BLA::Matrix<9, 9> X = solveCare(); // Continuous-time Algebraic Riccati Equation

	// Create controller

	controller = new StateSpaceController<9, 3>(*model);

	Serial.println("Constructed Cubic");
}
Cubic::~Cubic()
{
	delete model;
	delete controller; // Unallocated that damn memory (even if the program ends right after this lol)
	Serial.println("Destructed Cubic");
}

BLA::Matrix<9, 9> Cubic::getA()
{
	float a = 0;
	float s = -0.00001; // Small negative
	float y = mass * 9.81 * l / i[1];
	float z = mass * 9.81 * l / i[2];
	BLA::Matrix<9, 9> A =
			{0, 0, 0, 1, 0, 0, 0, 0, 0,
			 0, 0, 0, 0, 1, 0, 0, 0, 0,
			 0, 0, 0, 0, 0, 1, 0, 0, 0,
			 0, 0, 0, 0, 0, 0, 0, 0, 0,
			 0, y, 0, 0, 0, 0, 0, 0, 0,
			 0, 0, z, 0, 0, 0, 0, 0, 0,
			 0, 0, 0, 0, 0, 0, s, 0, 0,
			 0, 0, 0, 0, 0, 0, 0, s, 0,
			 0, 0, 0, 0, 0, 0, 0, 0, s};
	return A;
}

BLA::Matrix<9, 3> Cubic::getB()
{
	float w = 1 / iw;								 // Wheel acceleration
	float x = -1 / (sqrt(3) * i[0]); // X Acceleration
	float y = -2 / (sqrt(6) * i[1]); // Y Acceleration
	float h = 1 / (sqrt(6) * i[1]);	 // Half Y Acceleration
	float z = -1 / (sqrt(2) * i[2]); // Z Acceleration
	float n = 1 / (sqrt(2) * i[2]);	 // Negative Z Acceleration (its acc positive)
	BLA::Matrix<9, 3> B =
			{0, 0, 0,
			 0, 0, 0,
			 0, 0, 0,
			 x, x, x,
			 y, h, h,
			 0, z, n,
			 w, 0, 0,
			 0, w, 0,
			 0, 0, w};
	return B;
}

BLA::Matrix<9, 9> Cubic::getC()
{
	BLA::Identity<9, 9> C;
	return C;
}

BLA::Matrix<9, 3> Cubic::getD()
{
	BLA::Matrix<9, 3> D;
	return D;
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

BLA::Matrix<9, 9> Cubic::solveCare()
{
	BLA::Matrix<9, 9> P;
	// https: //en.wikipedia.org/wiki/Algebraic_Riccati_equation
	// AT P + P A - P B R^-1 BT P + Q = 0
}