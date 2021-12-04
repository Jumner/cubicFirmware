#pragma once
class Mpu6050
{
private:
	int address;
	void setRegister(int reg, int val);

public:
	long gyroX, gyroY, gyroZ;
	long accelX, accelY, accelZ;
	Mpu6050();
	~Mpu6050();
	void updateGyro();
	void updateAccel();
};
