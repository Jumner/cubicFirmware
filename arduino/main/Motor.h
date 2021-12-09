#pragma once

class Motor
{
private:
	unsigned int tach : 4;		 // 0.5 bytes
	unsigned int cw : 4;			 // 0.5 bytes
	unsigned int pwm : 8;			 // 1 byte
	unsigned long int oldTime; // 8 bytes

public:
	Motor(int n);
	~Motor();
	void Print();
	void interrupt();
	void setPwm(int val);
	void setTorque(double t);
	float rps; // 4 bytes
};
