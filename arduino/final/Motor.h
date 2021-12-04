#pragma once

class Motor
{
private:
	int n;
	int tach;
	int cw;
	int pwm;
	unsigned long int oldTime;

public:
	Motor(int num);
	~Motor();
	void Print();
	void interrupt();
	void setPwm(int val);
	void setTorque(double t);
	double rps;
};
