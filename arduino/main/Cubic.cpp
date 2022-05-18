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
	VectorFloat ap;
	ap.x = a.x / 1670.03;
	ap.y = a.y / 1670.03;
	ap.z = -a.z / 1670.03;
	// Serial.println(t);

	t[0] = atan(ap.z / ap.y) - atan(1);
	t[1] = atan(ap.x / ap.z) - atan(1);
	t[2] = abs(atan(ap.y / ap.x)) - atan(1);

	// Calculate A priori (the predicted state based on model)
	// Note that this is the same as C * aPriori bc we using full state feedback
	BLA::Matrix<9> aPriori = X + (getA() * X + getB() * U*0.1) * dt;

	// State estimation
	float prioriGain = 0.05f; // turn up to prioritize prediction
	float yGain = 1.0 - prioriGain;
	t[0] *= yGain;
	t[0] += aPriori(0) * prioriGain;
	t[1] *= yGain;
	t[1] += aPriori(1) * prioriGain;
	t[2] *= yGain;
	t[2] += aPriori(2) * prioriGain;

  float spCorrectRate = 0.5;
  float maxAngle = 0.025;
//  float spCorrectRate = 0.5;
	for (int i = 0; i < 3; i++) {
    spCorrect[i] -= U(i) * spCorrectRate * dt;
    spCorrect[i] = constrain(spCorrect[i], -maxAngle, maxAngle);
		if (aPriori(i) < 0) {
//			spCorrect[i] = spCorrectRate;
//  spCorrect[i] -= spCorrectRate * dt;

		} else {
//			spCorrect[i] = -spCorrectRate;
//  spCorrect[i] += spCorrectRate * dt;

		}
	}

	measureY(t, td); // Measure the output/state (full state feedback (kinda?))
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
	for (int i = 0; i < 3; i++) {
		if (aPriori(6 + i) < 0 && motors[i].rps < 100) {
			Y(6 + i) = -Y(6 + i);
		}
	}
}
BLA::Matrix<3, 9> Cubic::getK()
{ // This was precomputed with octave (open sauce matlab)
	// return {-3.1908, 0, 0, -0.34042, 0, 0, -0.00031623, 0, 0,
	// 				0, -3.1908, 0, 0, -0.34042, 0, 0, -0.00031623, 0,
	// 				0, 0, -3.1908, 0, 0, -0.34042, 0, 0, -0.00031623};
	// return {-5, 0, 0, 0.5, 0, 0, 0.007, 0, 0,
	// 				0, -5, 0, 0, 0.5, 0, 0, 0.007, 0,
	// 				0, 0, -5, 0, 0, 0.5, 0, 0, 0.007};
	// return {-3, 0, 0, -1, 0, 0, 0.01, 0, 0,
	// 				0, -3, 0, 0, -1, 0, 0, 0.01, 0,
	// 				0, 0, -3, 0, 0, -1, 0, 0, 0.01};
	return { -0.5, 0, 0, -4, 0, 0, 0.001, 0, 0,
		0, -0.5, 0, 0, -4, 0, 0, 0.001, 0, // 2, 6, 0.001 works kinda
		0, 0, -3, 0, 0, -2, 0, 0, 0.001 };
}

void Cubic::measureY(float t[3], VectorInt16 td)
{
	Y = { t[0], t[1], t[2], td.x / 7509.87263606, td.y / 7509.87263606, -td.z / 7509.87263606, motors[0].rps, motors[1].rps, motors[2].rps };
}

void Cubic::calculateU(float dt)
{
	BLA::Matrix<3> pid = { 0.0, 0.0, 0.0 };
	float kp = 10.0;
	float ki = 40.0;
	float kd = 0.01;
	float kw = 0.0;
	for (int i = 0; i < 3; i++) {
//   float err = spCorrect[i]-X(i);
   float err = X(i);//-spCorrect[i];
//		float err = -X(i);
		pid(i) = (err)*kp + (X(3 + i)) * kd + (integral[i]) * ki + X(6 + i) * kw;
		integral[i] += (err)*dt;
	}

	float oldGain = 0.05f;
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
	// motors[1].setTorque(U(0), Y(7));
	printState();
	// motors[0].setTorque(U(0), X(6)); // X
	// motors[1].setTorque(U(1), X(7)); // Y
//	motors[2].setTorque(U(2), X(8)); // Z
  motors[2].setValue(U(2));
																	 // 🙏
}

void Cubic::printState()
{
	// Serial << X(0) << ',';
	// Serial << X(3) << ',';
	// Serial << X(6) << ',';
	// Serial << U(0) << '\n';
	// Serial << X(1) << ',';
	// Serial << X(4) << ',';
	// Serial << X(7) << ',';
	// Serial << U(1) << '\n';
	Serial << "Angle:" << X(2) << ',';
	Serial << "Rate:" << X(5) << ',';
	float rate = X(8) / 40.0;
	Serial << "WheelRate:" << rate << ',';
	float spRate = spCorrect[2] * 10.0;
	Serial << "SpCorrect:" << spRate << ',';
	Serial << "Output:" << U(2) << '\n';
	// Serial << X(0) << ',';
	// Serial << X(1) << ',';
	// Serial << X(2) << ',';
	// Serial << X(3) << ',';
	// Serial << X(4) << ',';
	// Serial << X(5) << ',';
	// Serial << X(6) << ',';
	// Serial << X(7) << ',';
	// Serial << X(8) << ',';
	// Serial << U(0) << ',';
	// Serial << U(1) << ',';
	// Serial << U(2) << '\n';
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

bool Cubic::stop() {
  // stop all motors
  bool cont = false;
//  for (int i = 0; i < 3; i++) {
  int i = 2; // 2d
    cont = cont || motors[i].stop(X(i+6));
//  }
  return cont;
}
