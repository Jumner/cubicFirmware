#include "Cubic.h"
#include "Motor.h"
#include <BasicLinearAlgebra.h>

// Values
#define mass 1.199			 // Mass (kg)
#define l 0.119511505722 // Length of pendulum arm (m)
#define ix 0.015				 // Inertia x
#define iy 0.015				 // Inertia y
#define iz 0.015				 // Inertia z
#define iw 0.00025			 // Wheel inertia (kg*m^2)

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

void Cubic::calculateX(VectorInt16 a, VectorInt16 td, float dt)
{
	float t[3]; // theta
	t[0] = atan((double)a.x / a.z);
	t[1] = atan((double)a.y / a.z);
	// Calculate A priori (the predicted state based on model)
	// Note that this is the same as C * aPriori bc we using full state feedback
	BLA::Matrix<9> aPriori = X + (getA() * X + getB() * U) * dt;
	t[2] = aPriori(2); // Twist is not directly observable
	measureY(t, td); // Measure the output/state (full state feedback (kinda?))
	signY(aPriori);
	// State estimation
	// float prioriGain = 0.05f; // turn up to prioritize prediction
	float prioriGain = 0.0f; // Disable state estimation
	float yGain = 1.0 - prioriGain;
	X = Y * yGain + aPriori * prioriGain; // Sadge no Kaaal
}

void Cubic::signY(BLA::Matrix<9> aPriori) // We measure speed not velocity so we must add a sign
{
	for (int i = 0; i < 3; i++) {
		if (aPriori(6 + i) < 0) {
			Y(6 + i) = -Y(6 + i);
		}
	}
}

void Cubic::measureY(float t[3], VectorInt16 td)
{
	Y = { t[0], t[1], t[2], td.y / 7509.87263606, td.x / 7509.87263606, td.z / 7509.87263606, motors[0].rps, motors[1].rps, motors[2].rps };
}

void Cubic::calculateU(float dt)
{
	BLA::Matrix<3> pid = { 0.0, 0.0, 0.0 };
	for (int i = 0; i < 3; i++) {
    float err = X(i);
		pid(i) = (err)*pidw[0] + (X(3 + i)) * pidw[2] + (integral[i]) * pidw[1] - X(6 + i) * pidw[3];
		integral[i] += (err)*dt;
	}

	float oldGain = 0.00f;
	float newGain = 1.0f - oldGain;
	U = pid * newGain + U * oldGain;
}

BLA::Matrix<9, 9> Cubic::getA()
{
	float x = mass * 9.81 * l / (ix * 3); // X Gravity
	float y = mass * 9.81 * l / (iy * 3); // Y Gravity
	float z = mass * 9.81 * l / (iz * 3); // Z Gravity
	return { 0, 0, 0, 1, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 1, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 1, 0, 0, 0,
		x, 0, 0, 0, 0, 0, 0, 0, 0,
		0, y, 0, 0, 0, 0, 0, 0, 0,
		0, 0, z, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0 };
}

BLA::Matrix<9, 3> Cubic::getB()
{
	float w = 1 / iw;	 // Wheel acceleration
	float x = -1 / ix; // X Acceleration
	float y = -1 / iy; // Y Acceleration
	float z = -1 / iz; // Z Acceleration
	return { 0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		x, 0, 0,
		0, y, 0,
		0, 0, z,
		w, 0, 0,
		0, w, 0,
		0, 0, w };
}

void Cubic::run(VectorInt16 a, VectorInt16 td, float dt)
{

	calculateX(a, td, dt); // Kaaaaaaal?
	calculateU(dt);
	motors[0].setTorque(U(0), X(6)); // X
	motors[1].setTorque(U(1), X(7)); // Y
	motors[2].setTorque(U(2), X(8)); // Z
}

bool Cubic::stop() {
  // stop all motors
  bool cont = false;
  for (int i = 0; i < 3; i++) {
    cont = cont || motors[i].stop(X(i+6));
  }
  return cont;
}