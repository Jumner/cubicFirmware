#include "Motor.h"
#include "Cubic.h"
#include "PinChangeInterrupt.h"
#include "Mpu6050.h"
#include <Wire.h>
Cubic cube;
void setup()
{
	Serial.begin(9600);
	delay(500);

	Serial.println("yo");

	cube = Cubic();
	Mpu6050 imu = Mpu6050();
	// Attach tachometer interrupts
	attachPCINT(digitalPinToPCINT(2), int0, CHANGE);
	attachPCINT(digitalPinToPCINT(3), int1, CHANGE);
	attachPCINT(digitalPinToPCINT(4), int2, CHANGE);

	// cube.motors[0].Print();
	// Serial.println(cube.motors[0].rps);
}

void loop()
{
	// cube.motors[0].setTorque(0.005);
}

void int0(void)
{
	cube.motors[0].interrupt();
}
void int1(void)
{
	cube.motors[1].interrupt();
}
void int2(void)
{
	cube.motors[2].interrupt();
}

//Mpu must be in main for some reason
Mpu6050::Mpu6050()
{
	address = 0b1101000;
	Wire.begin();						 // Join bus as master
	setRegister(0x6B, 0x00); // Disable sleep from power management register
	setRegister(0x1B, 0x00); // Set gyro to +- 250 deg/s
	// 000 00 000. 00 -> 250, 01 -> 500, 10 -> 1000, 11 -> 2000
	setRegister(0x1C, 0x00); // Set accelerometer to +- 2g
	// 000 00 000. 00 -> 2g, 01 -> 4g, 10 -> 8g, 11 -> 16g
	updateGyro();
	updateAccel();
	Serial.println(accelX);
	Serial.println(accelY);
	Serial.println(accelZ);
	Serial.println(gyroX);
	Serial.println(gyroY);
	Serial.println(gyroZ);
	// Wire.end();
	Serial.println("Constructed Mpu-6050");
}

Mpu6050::~Mpu6050()
{
	// Wire.end();
	Serial.println("Destructed Mpu-6050");
}

void Mpu6050::setRegister(int reg, int val)
{
	Wire.beginTransmission(address); // Connect to mpu 6050
	Wire.write(reg);								 // Write register
	Wire.write(val);								 // Write value
	Wire.endTransmission();					 // Close connection
}

void Mpu6050::setRegister(int reg)
{
	Wire.beginTransmission(address); // Connect to mpu 6050
	Wire.write(reg);								 // write register
	Wire.endTransmission();					 // Close connection
}

void Mpu6050::updateAccel()
{
	setRegister(0x3B);

	Wire.requestFrom(address, 6);
	while (Wire.available() < 6)
	{
	}
	accelX = Wire.read() << 8 | Wire.read();
	accelY = Wire.read() << 8 | Wire.read();
	accelZ = Wire.read() << 8 | Wire.read();
}
void Mpu6050::updateGyro()
{
	setRegister(0x43);

	Wire.requestFrom(address, 6);
	while (Wire.available() < 6)
	{
	}
	gyroX = Wire.read() << 8 | Wire.read();
	gyroY = Wire.read() << 8 | Wire.read();
	gyroZ = Wire.read() << 8 | Wire.read();
}