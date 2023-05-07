#include "Cubic.h"
#include "Motor.h"
#include <BasicLinearAlgebra.h>

// Values
#define mass 1.219			 // Mass (kg)
#define l 0.1233757111 // Length of pendulum arm (m)
#define ix 0.02330				 // Inertia x
#define iy 0.02335				 // Inertia y
#define iz 0.004126				 // Inertia z
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
	float prioriGain = 0.5f; // Disable state estimation
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
	Y = { -t[0], t[1], t[2], -(double)td.y / 7509.87263606, -(double)td.x / 7509.87263606, (double)td.z / 7509.87263606, motors[0].rps, motors[1].rps, motors[2].rps };
}

void Cubic::calculateU(float dt)
{
	BLA::Matrix<2> pid = { 0.0, 0.0 };
	for (int i = 0; i < 2; i++) {
		pid(i) = (X(i)) * pidw[0] + (integral[i]) * pidw[1] + (X(3 + i)) * pidw[2];
		integral[i] += (X(i))*dt;
	}
	// Transform x,y torque to motor torques
	// Note my convention here (this will haunt me) x/0 is the rotation around the Y axis. y/1 is the rotation around the X axis
	BLA::Matrix<3,2> transform = {
		 -sqrt(6)/6, sqrt(2)/2, // sqrt(3)/3
		 sqrt(6)/3 ,  0.0     ,
		 -sqrt(6)/6, -sqrt(2)/2 
	};
	U = -transform * pid; // Apply transform to required torques, for controls sake, pid is kx so u = -kx but then gets transformed
	// Handle spinup, we want the average wheel velocity to always be zero. U is calculated to not change the average wheel velocity
	double sum = 0;
	for(int i = 6; i < 9; i ++) {
		sum += X(i); // Sum wheel rates
	}
	Matrix<3> antiSpinup = { 1.0, 1.0, 1.0 };
	antiSpinup *= sum * 0.0001; // Simple proportional controller, good performance doesn't matter we just want to avoid spinup
	U -= antiSpinup; // Apply anti Spinup
}

BLA::Matrix<9, 9> Cubic::getA()
{
	float x = mass * 9.81 * l / (ix * 3); // X Gravity
	float y = mass * 9.81 * l / (iy * 3); // Y Gravity
	return { 
		0, 0, 0, 1, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 1, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 1, 0, 0, 0,
		x, 0, 0, 0, 0, 0, 0, 0, 0,
		0, y, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0 };
}

BLA::Matrix<9, 3> Cubic::getB()
{
	float w = 1 / iw;	 // Wheel acceleration
	float x = 1 / ix; // X Acceleration
	float y = 1 / iy; // Y Acceleration
	float z = 1 / iz; // Z Acceleration
	return {
		0         , 0        , 0         ,
		0         , 0        , 0         ,
		0         , 0        , 0         ,
    -x*sqrt(6)/6, x*sqrt(6)/3, -x*sqrt(6)/6,
    y*sqrt(2)/2 , 0        , -y*sqrt(2)/2,
    z*sqrt(3)/3 , z*sqrt(3)/3, z*sqrt(3)/3 ,
		w         , 0        , 0         ,
		0         , w        , 0         ,
		0         , 0        , w};

		 // -sqrt(6)/6, sqrt(2)/2,  sqrt(3)/3
		 // sqrt(6)/3,  0.0,        sqrt(3)/3
		 // -sqrt(6)/6, -sqrt(2)/2, sqrt(3)/3
	
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
