#pragma once

class Motor // 18 bytes
{
private:
	unsigned int tach : 4;		 // 0.5 bytes
	unsigned int cw : 4;			 // 0.5 bytes
	unsigned int pwm : 8;			 // 1 byte
	unsigned long int oldTime; // 8 bytes

public:
	Motor(int n);
	~Motor();
	void interrupt();
	void setPwm(int val, bool dir);
	void setTorque(double t, double vel);
	double rps; // 8 bytes
};
