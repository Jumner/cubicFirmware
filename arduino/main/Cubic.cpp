#include "Motor.h"
#include "Cubic.h"
#include <BasicLinearAlgebra.h>

// Values
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
	TCCR1B = TCCR1B & B11111000 | B00000001; // 9 & 10
	TCCR2B = TCCR2B & B11111000 | B00000001; // 11
}
Cubic::~Cubic()
{
}

void Cubic::calculateX(float t[3], VectorInt16 td, float dt)
{
	measureY(t, td); // Measure the output/state (full state feedback)

	// Calculate A priori (the predicted state based on model)
	// Note that this is the same as C * aPriori bc we using full state feedback
	BLA::Matrix<9> aPriori = X + (getB() * U) * dt;
	// BLA::Matrix<9> aPriori = X + (getA() * X + getB() * U) * dt;
	signY(aPriori);
	// Serial.print("Estimated State: ");
	// Serial << X << " U: " << U << '\n';
	// Serial << "U" << U << '\n';
	// Serial.print("aPriori: ");
	// Serial << aPriori << '\n';
	// Serial.print("U: ");
	// Serial << U << '\n';
	X = Y; // Sadge
}

void Cubic::signY(BLA::Matrix<9> aPriori) // We measure speed not velocity so we must add a sign
{
	if (aPriori(6) < 0)
	{
		Y(6) = -Y(6);
	}
	if (aPriori(7) < 0)
	{
		Y(7) = -Y(7);
	}
	if (aPriori(8) < 0)
	{
		Y(8) = -Y(8);
	}
}
BLA::Matrix<3, 9> Cubic::getK()
{ // This was precomputed with octave (open sauce matlab)
	return {-7.2292e-08, -2.6568, -3.0579e-10, -0.0033118, -0.33108, -2.0317e-11, -0.00021725, 9.8982e-05, 9.8981e-05,
					-7.2285e-08, 1.3284, 2.3013, -0.0033174, 0.16554, 0.28717, 9.8791e-05, -0.00021744, 9.8791e-05,
					-7.2292e-08, 1.3284, -2.3013, -0.0033148, 0.16554, -0.28717, 9.8879e-05, 9.8879e-05, -0.00021735};
	// return {-0.57735, -1144.7, 1.1682e-08, -9.8869, -142.66, 1.46e-09, -0.66664, 0.33336, 0.33336,
	// 				-0.57735, 572.37, -992.73, -9.8869, 71.328, -123.88, 0.33336, -0.66664, 0.33336,
	// 				-0.57735, 572.37, 992.73, -9.8869, 71.328, 123.88, 0.33336, 0.33336, -0.66664};
}

void Cubic::measureY(float t[3], VectorInt16 td)
{
	Y = {t[0], t[1], t[2], td.x / 7509.87263606, td.y / 7509.87263606, td.z / 7509.87263606, motors[0].rps, motors[1].rps, motors[2].rps};
}

void Cubic::calculateU()
{
	U = -getK() * X;
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
					0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0};
}

BLA::Matrix<9, 3> Cubic::getB()
{
	float w = 1 / iw;							 // Wheel acceleration
	float x = -1 / (sqrt(3) * ix); // X Acceleration
	float y = -2 / (sqrt(6) * iy); // Y Acceleration
	float h = 1 / (sqrt(6) * iy);	 // Half Y Acceleration
	float z = 1 / (sqrt(2) * iz);	 // Z Acceleration
	float n = -1 / (sqrt(2) * iz); // Negative Z Acceleration (its acc positive)
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

void Cubic::run(float t[3], VectorInt16 td, float dt)
{

	calculateX(t, td, dt); // Kaaaaaaal?
	calculateU();
	// motors[1].setTorque(U(0), Y(7));
	printState();
	motors[0].setTorque(U(0), X(6));
	motors[1].setTorque(U(1), X(7));
	motors[2].setTorque(U(2), X(8));
	// 🙏
}

void Cubic::printState()
{
	Serial << X(0) << ',';
	Serial << X(1) << ',';
	Serial << X(2) << ',';
	Serial << X(3) << ',';
	Serial << X(4) << ',';
	Serial << X(5) << ',';
	Serial << X(6) << ',';
	Serial << X(7) << ',';
	Serial << X(8) << ',';
	Serial << U(0) << ',';
	Serial << U(1) << ',';
	Serial << U(2) << '\n';
	// Serial.print("Estimated State: ");
	// Serial.print(Y(0));
	// Serial.print(',');
	// Serial.print(Y(1));
	// Serial.print(',');
	// Serial.print(Y(2));
	// Serial.print(',');
	// Serial.print(Y(3));
	// Serial.print(',');
	// Serial.print(Y(4));
	// Serial.print(',');
	// Serial.print(Y(5));
	// Serial.print(',');
	// Serial.print(Y(6));
	// Serial.print(',');
	// Serial.print(Y(7));
	// Serial.print(',');
	// Serial.println(Y(8));
}