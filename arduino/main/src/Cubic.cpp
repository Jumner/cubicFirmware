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
#define kp 6.374398250567869
#define kd 6.877099164695932
#define kw 0.004203415590498105
#define sp 0.0006934966326738897
// #define kp 0.0
// #define kd 0.0
// #define kw 0.0
// #define sp 0.0

Cubic::Cubic()
{
	// Set timers
	TCCR1B = (TCCR1B & B11111000) | B00000001; // 9 & 10
	TCCR2B = (TCCR2B & B11111000) | B00000001; // 11
}

void Cubic::calculateX(VectorInt16 td, float dt)
{
	// Calculate A priori (the predicted state based on model)
	// Note that this is the same as C * aPriori bc we using full state feedback
	BLA::Matrix<9> aPriori = X + (getA() * X + getB() * U) * dt;

	// Note Theta is not directly observed
	measureY(aPriori, td); // Measure the output/state (full state feedback (kinda?))
	// State estimation
	BLA::Matrix<9> prioriGain = {1.0, 1.0, 1.0, 0.2, 0.2, 0.2, 0.1, 0.1, 0.1};
	for(int i = 0; i < 9; i ++) {
		X(i) = Y(i) + prioriGain(i) * (aPriori(i) - Y(i)); // Sadge no Kaaal
	}
}


void Cubic::measureY(BLA::Matrix<9> aPriori, VectorInt16 td)
{
	Y = { aPriori(0), aPriori(1), aPriori(2), -(double)td.y / 7509.87263606, -(double)td.x / 7509.87263606, (double)td.z / 7509.87263606, motors[0].rps, motors[1].rps, motors[2].rps };
	// We measure wheel speed not velocity so we infere direction from the aPriori
	for (int i = 0; i < 3; i++) {
		if (aPriori(6 + i) < 0) {
			Y(6 + i) = -Y(6 + i);
		}
	}
}

void Cubic::calculateU(float dt)
{
	BLA::Matrix<3> pid = { 0.0, 0.0, 0.0 };
	BLA::Matrix<6> state = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }; // State used for control x_angle,y_angle,x_dot,y_dot,x_wheelspeed,y_wheelspeed
	BLA::Matrix<2> gravity = { 0.0, 0.0 };
	
	BLA::Matrix<3,3> wheelTransform = {
    -sqrt(6)/6, sqrt(6)/3, -sqrt(6)/6,
    sqrt(2)/2 , 0.0      , -sqrt(2)/2,
		sqrt(3)/3, sqrt(3)/3, sqrt(3)/3
	};

	BLA::Matrix<3> wheelMotor = {X(6), X(7), X(8)};
	BLA::Matrix<3> wheelAxis = wheelTransform * wheelMotor; // Decompose the three wheel velocities into its X and Y components

	for (int i = 0; i < 2; i++) {
		state(i)   = X(i);      // X/Y angle
		state(i+2) = X(i + 3);  // X/Y dot
		state(i+4) = wheelAxis(i); // X/Y Wheel

		// X/Y Control
		// First we feed forward gravity to linearize the control response
		gravity(i) = mass * 9.81 * l * sin(state(i) - spCorrect[i]);

		// We nudge error dependant on wheel speed (essentially moving the setpoint to slow wheels)
		// We also nudge it by sp correct which accounts for any tilt in the sensor in ~10 sec
		// We then counter velocity with the d term

		pid(i) = - kp * (state(i) - kw * state(i+4) - spCorrect[i]) - kd * state(i+2) - gravity(i);
		// Use the wheel speed to infer sensor tilt
		spCorrect[i] += state(i + 4) * sp * dt; // Move the set point correct
	}
	// Handle spinup, we want the average wheel velocity to always be zero. U is calculated to not change the average wheel velocity
	pid(2) = wheelAxis(2) * -0.0001;
	// Transform x,y,z torque to motor torques
	// Note my convention here (this will haunt me) x/0 is the rotation around the Y axis. y/1 is the rotation around the X axis
	BLA::Matrix<3,2> transform = {
		 -sqrt(6)/6, sqrt(2)/2, sqrt(3)/3,
		 sqrt(6)/3 ,  0.0      , sqrt(3)3,
		 -sqrt(6)/6, -sqrt(2)/2, sqrt(3)/3
	};
	float uGain = 0.25; // Reduce the impact of high frequency oscillations in the motors (they kinda click)
	U = U - U * uGain + transform * pid * uGain; // Apply transform to get required torques
	// Limit max torque to improve apriori accuracy
	for(int i = 0; i < 3; i ++) {
		if(U(i) > 0) { // Positive Torque
			U(i) = min(Motor::maxTorque(X(6+i)), U(i));
		} else { // Negative Torque
			U(i) = max(-Motor::maxTorque(-X(6+i)), U(i));
		}
	}
}

BLA::Matrix<9, 9> Cubic::getA()
{
	float x = mass * 9.81 * l / (ix); // X Gravity
	float y = mass * 9.81 * l / (iy); // Y Gravity
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
		0           , 0          , 0           ,
		0           , 0          , 0           ,
		0           , 0          , 0           ,
    -x*sqrt(6)/6, x*sqrt(6)/3, -x*sqrt(6)/6,
    y*sqrt(2)/2 , 0          , -y*sqrt(2)/2,
    z*sqrt(3)/3 , z*sqrt(3)/3, z*sqrt(3)/3 ,
		w           , 0          , 0           ,
		0           , w          , 0           ,
		0           , 0          , w};
}

void Cubic::run(VectorInt16 td, float dt)
{
	calculateX(td, dt); // Kaaaaaaal?
	calculateU(dt);
	motors[0].setTorque(U(0), X(6)); // Left
	motors[1].setTorque(U(1), X(7)); // Back
	motors[2].setTorque(U(2), X(8)); // Right
}

bool Cubic::stop(float initialRps[3]) {
  // stop all motors
  bool cont = false;
  for (int i = 0; i < 3; i++) {
    cont = cont || motors[i].stop(X(i+6));
  }
  return cont;
}
