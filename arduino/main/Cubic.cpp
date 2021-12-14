#include "Motor.h"
#include "Cubic.h"
#include <BasicLinearAlgebra.h>

// Values
#define mass 1.199			 // Mass (kg)
#define l 0.119511505722 // Length of pendulum arm (m)
#define ix 0.015				 // Inertia x
#define iy 0.015				 // Inertia y
#define iz 0.015				 // Inertia z
#define iw 0.00025			 // Wheen inertia (kg*m^2)

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
	return {-1.6298, 0, 0, -0.30117, 0, 0, -0.001, 0, 0,
					0, -1.6298, 0, 0, -0.30117, 0, 0, -0.001, 0,
					0, 0, -1.6298, 0, 0, -0.30117, 0, 0, -0.001};
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
	float x = mass * 9.81 * l / ix; // X Gravity
	float y = mass * 9.81 * l / iy; // Y Gravity
	float z = mass * 9.81 * l / iz; // Z Gravity
	return {0, 0, 0, 1, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 1, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 1, 0, 0, 0,
					x, 0, 0, 0, 0, 0, 0, 0, 0,
					0, y, 0, 0, 0, 0, 0, 0, 0,
					0, 0, z, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0};
}

BLA::Matrix<9, 3> Cubic::getB()
{
	float w = 1 / iw;	 // Wheel acceleration
	float x = -1 / ix; // X Acceleration
	float y = -1 / iy; // Y Acceleration
	float z = -1 / iz; // Z Acceleration
	return {0, 0, 0,
					0, 0, 0,
					0, 0, 0,
					x, 0, 0,
					0, y, 0,
					0, 0, z,
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
	motors[0].setTorque(U(0), X(6)); // X
	motors[1].setTorque(U(1), X(7)); // Y
	motors[2].setTorque(U(2), X(8)); // Z
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