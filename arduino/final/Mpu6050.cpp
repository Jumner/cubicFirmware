#include <Arduino.h>
#include "Mpu6050.h"
#include <Wire.h>

Mpu6050::Mpu6050()
{

	// Wire.beginTransmission(0b1101000); //This is the I2C address of the MPU (b1101000/b1101001 for AC0 low/high datasheet sec. 9.2)
	// Wire.write(0x6B);									 //Accessing the register 6B - Power Management (Sec. 4.28)
	// Wire.write(0b00000000);						 //Setting SLEEP register to 0. (Required; see Note on p. 9)
	// Wire.endTransmission();
	// Wire.beginTransmission(0b1101000); //I2C address of the MPU
	// Wire.write(0x1B);									 //Accessing the register 1B - Gyroscope Configuration (Sec. 4.4)
	// Wire.write(0x00000000);						 //Setting the gyro to full scale +/- 250deg./s
	// Wire.endTransmission();
	// Wire.beginTransmission(0b1101000); //I2C address of the MPU
	// Wire.write(0x1C);									 //Accessing the register 1C - Acccelerometer Configuration (Sec. 4.5)
	// Wire.write(0b00000000);						 //Setting the accel to +/- 2g
	// Wire.endTransmission();
	Wire.beginTransmission(0b1101000); //I2C address of the MPU
	Wire.write(0x3B);
	Wire.endTransmission();

	Wire.requestFrom(0b1101000, 6); //Request Accel Registers (3B - 40)
	while (Wire.available() < 6)
		;
	Serial.println(Wire.read() << 8 | Wire.read()); //Store first two bytes into accelX
	Serial.println(Wire.read() << 8 | Wire.read()); //Store middle two bytes into accelY
	Serial.println(Wire.read() << 8 | Wire.read()); //Store last two bytes into accelZ
	Serial.println("Constructed Mpu-6050");
}

Mpu6050::~Mpu6050()
{
	// Wire.end();
	Serial.println("Destructed Mpu-6050");
}
