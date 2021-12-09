#include "Motor.h"
#include "Cubic.h"
#include <BasicLinearAlgebra.h>

// Values
#define s -0.00001			 // Small neg value
#define mass 1.199			 // Mass (kg)
#define l 0.119511505722 // Length of pendulum arm (m)
#define ix 0.004281			 // Inertia x
#define iy 0.02183			 // Inertia y
#define iz 0.02189			 // Inertia z
#define iw 0.0002504		 // Wheen inertia (kg*m^2)

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

void Cubic::calculateX(float t[3], VectorInt16 td, float dt)
{
	measureY(t, td); // Measure the output/state (full state feedback)

	// Calculate A priori (the predicted state based on model)
	// Note that this is the same as C * aPriori bc we using full state feedback
	BLA::Matrix<9> aPriori = X + (getA() * X + getB() * U) * dt;
	X = kalman.getPosterior(aPriori, Y, getA()); // Kalman
}

BLA::Matrix<3, 9> Cubic::getK()
{ // This was precomputed with octave (open sauce matlab)
	return {-0.57735, -1144.7, 1.1682e-08, -9.8869, -142.66, 1.46e-09, -0.66664, 0.33336, 0.33336,
					-0.57735, 572.37, -992.73, -9.8869, 71.328, -123.88, 0.33336, -0.66664, 0.33336,
					-0.57735, 572.37, 992.73, -9.8869, 71.328, 123.88, 0.33336, 0.33336, -0.66664};
	// return {-0.57735, -11.007, -1.1515e-11, -1.5275, -6.2491, -9.3816e-12, -0.66665, 0.33333, 0.33333,
	// 				-0.57735, 5.5037, -9.5326, -1.5275, 3.1245, -5.4118, 0.33333, -0.66665, 0.33333,
	// 				-0.57735, 5.5037, 9.5326, -1.5275, 3.1245, 5.4118, 0.33333, 0.33333, -0.66665};
}
void Cubic::measureY(float t[3], VectorInt16 td)
{
	Y = {t[0], t[1], t[2], td.x / 7509.87263606, td.y / 7509.87263606, td.z / 7509.87263606, motors[0].rps, motors[1].rps, motors[2].rps};
}
BLA::Matrix<3> Cubic::getU()
{
	BLA::Matrix<3> U = -getK() * X;
	return U;
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

void Cubic::run()
{
	BLA::Matrix<3> U = getU();
	motors[0].setTorque(U(0));
	motors[1].setTorque(U(1));
	motors[2].setTorque(U(2));
	// 🙏
}

void Cubic::printState()
{
	Serial.print("Estimated State: ");
	Serial << X << '\n';
}