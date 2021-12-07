#pragma once

class Motor
{
private:
	int tach : 4;
	int cw : 4;
	int pwm : 8;
	unsigned long int oldTime;

public:
	Motor(int n);
	~Motor();
	void Print();
	void interrupt();
	void setPwm(int val);
	void setTorque(double t);
	double rps;
};
