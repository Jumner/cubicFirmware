#pragma once
class Mpu6050
{
private:
	int address;
	void setRegister(int reg);
	void setRegister(int reg, int val);
	long gyroXRaw, gyroYRaw, gyroZRaw;
	long accelXRaw, accelYRaw, accelZRaw;

public:
	double gyroX, gyroY, gyroZ;
	double accelX, accelY, accelZ;
	Mpu6050();
	~Mpu6050();
	void updateAccel();
	void updateGyro();
	void updateAll()
	{
		updateAccel();
		updateGyro();
	}
	void printAll();
};
