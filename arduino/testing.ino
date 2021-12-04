#include "PinChangeInterrupt.h"
#include "Adafruit_MPU6050.h"

Adafruit_MPU6050 mpu;
Adafruit_Sensor *mpu_temp, *mpu_accel, *mpu_gyro;
unsigned long int mils = 0;

void setup()
{
	Serial.begin(9600);

	analogWrite(9, 255);
	analogWrite(10, 255);
	analogWrite(11, 255);
	delay(1000);
	// Setup MPU-6050
	if (!mpu.begin())
	{
		Serial.println("Cannot find MP-6050");
		while (true)
		{
			delay(10);
		}
	}
	Serial.println("Found MPU-6050");
	mpu_temp = mpu.getTemperatureSensor();
	mpu_temp->printSensorDetails();

	mpu_accel = mpu.getAccelerometerSensor();
	mpu_accel->printSensorDetails();

	mpu_gyro = mpu.getGyroSensor();
	mpu_gyro->printSensorDetails();

	Serial.println("Done Setup MPU-6050");

	// Setup Motors
	TCCR1B = TCCR1B & B11111000 | B00000001; // 9 & 10
	TCCR2B = TCCR2B & B11111000 | B00000001; // 11

	Serial.println("Configured Timer registers");

	pinMode(2, INPUT_PULLUP); // Tackometer 1
	pinMode(3, INPUT_PULLUP); // Tackometer 2
	pinMode(4, INPUT_PULLUP); // Tackometer 3

	pinMode(5, OUTPUT); // CW/CCw 1
	pinMode(6, OUTPUT); // CW/CCw 2
	pinMode(7, OUTPUT); // CW/CCw 3

	pinMode(9, OUTPUT);	 // PWM 1
	pinMode(10, OUTPUT); // PWM 2
	pinMode(11, OUTPUT); // PWM 3

	Serial.println("Setup pin modes");
	attachPCINT(digitalPinToPCINT(2), int1, CHANGE);

	Serial.println("Setup interrupts");

	Serial.println("Setup Cubic, waiting 5s");
	delay(5000);

	digitalWrite(5, LOW);
	digitalWrite(6, LOW);
	digitalWrite(7, LOW);
	analogWrite(9, 127);
	mils = millis();
}

float rps1 = 0;
unsigned long oldTime1 = micros();

void int1(void)
{
	if (digitalRead(2))
	{
		rps1 = 31415.9 / (micros() - oldTime1);
		oldTime1 = micros();
	}
}

void loop()
{
	if (millis() - mils < 5000)
	{
		Serial.print(127);
	}
	else
	{
		Serial.print(-127);
		digitalWrite(5, HIGH);
	}
	Serial.print(",");
	Serial.print(rps1);
	Serial.print(",");
	Serial.print(millis() - mils);
	Serial.println();
}
// (240-val) * (0.0625 - 0.0007.43*vel - 0.00000348*vel*vel + 0.0000000429*vel*vel*vel)