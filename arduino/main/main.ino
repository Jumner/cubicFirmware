#include "Motor.h"
#include "Cubic.h"
#include <PinChangeInterrupt.h>
#include <I2Cdev.h>
#include <MPU6050_6Axis_MotionApps20.h>
#include "Wire.h"
#include <BasicLinearAlgebra.h>

using namespace BLA;

/*
Error codes:
0: Unassigned
1: imu connection failed
2: DMP init failed
*/

Cubic cube = Cubic();
MPU6050 imu;

// MPU control/status vars
uint8_t mpuIntStatus;		// holds actual interrupt status byte from MPU
uint16_t packetSize;		// expected DMP packet size (default is 42 bytes)
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;			// [w, x, y, z]         quaternion container
VectorInt16 gyro; // [x, y, z]
float euler[3];		// [psi, theta, phi]    Euler angle container

volatile bool mpuInterrupt = false; // indicates whether MPU interrupt pin has gone high
void dmpDataReady()
{
	mpuInterrupt = true;
}
void setup()
{
	Wire.begin();
	Wire.setClock(400000);					 // 400kHz I2C clock. Comment this line if having compilation difficulties
	Wire.setWireTimeout(3000, true); //timeout value in uSec (fixes hang issue)
	Serial.begin(38400);
	delay(1000);
	Serial.println("starting");
	imu.initialize();

	pinMode(2, INPUT); // 6050
	pinMode(8, INPUT); // tach 0
	pinMode(3, INPUT); // tach 1
	pinMode(4, INPUT); // tach 2

	if (!imu.testConnection())
		safe("1");

	imu.setFullScaleGyroRange(MPU6050_GYRO_FS_250);
	imu.setFullScaleAccelRange(MPU6050_ACCEL_FS_2);

	if (imu.dmpInitialize() != 0)
		safe("2");
	imu.setXAccelOffset(-648);
	imu.setYAccelOffset(483);
	imu.setZAccelOffset(4982);
	imu.setXGyroOffset(110);
	imu.setYGyroOffset(15);
	imu.setZGyroOffset(-29);

	// imu.setXAccelOffset(-412);
	// imu.setYAccelOffset(-675);
	// imu.setZAccelOffset(4652);
	// imu.setXGyroOffset(110);
	// imu.setYGyroOffset(15);
	// imu.setZGyroOffset(-29);

	// imu.CalibrateAccel();
	// imu.CalibrateGyro();
	// imu.PrintActiveOffsets();
	// safe("done calibrate");

	imu.setDMPEnabled(true);

	// Attach tachometer interrupts
	attachInterrupt(digitalPinToInterrupt(2), dmpDataReady, RISING); // 6050
	attachPCINT(digitalPinToPCINT(8), int0, CHANGE);								 // Pin 2 is mpu interrupt
	attachPCINT(digitalPinToPCINT(3), int1, CHANGE);
	attachPCINT(digitalPinToPCINT(4), int2, CHANGE);

	mpuIntStatus = imu.getIntStatus();

	packetSize = imu.dmpGetFIFOPacketSize();
	Serial.println("done");
	delay(1000);
}
unsigned long time = micros();
void loop()
{
	// safe("not gonna do stuff");
	if (imu.dmpGetCurrentFIFOPacket(fifoBuffer))
	{
		imu.dmpGetQuaternion(&q, fifoBuffer);
		imu.dmpGetEuler(euler, &q); // Get euler angles
		// imu.dmpGetGyro(&gyro, fifoBuffer);													// Get gyro
		imu.getRotation(&gyro.x, &gyro.y, &gyro.z);
		double dt = (micros() - time) / (1000000.0); // In secs
		time = micros();
		cube.run(euler, gyro, dt); // "It just works"
		// cube.motors[0].setPwm(200, true);
		// cube.motors[1].setPwm(200, true);
		// cube.motors[2].setPwm(200, true);
		// cube.printState();
		if (cube.motors[0].rps > 100)
		{
			safe("m0");
		}
		else if (cube.motors[1].rps > 100)
		{
			safe("m1");
		}
		else if (cube.motors[2].rps > 100)
		{
			safe("m2");
		}
	}
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

void safe(String s)
{ // Put the chip into a safe mode is something unrecoverable goes wrong
	cube.motors[0].setPwm(255, true);
	cube.motors[1].setPwm(255, true);
	cube.motors[2].setPwm(255, true);
	Serial.print("Fatal Err: ");
	Serial.println(s);
	while (true)
	{
		delay(10000);
	}
}
