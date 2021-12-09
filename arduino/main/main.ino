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
	Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
	Serial.begin(38400);
	delay(1000);

	imu.initialize();

	pinMode(2, INPUT); // 6050
	pinMode(8, INPUT); // tach 0
	pinMode(3, INPUT); // tach 1
	pinMode(4, INPUT); // tach 2

	if (!imu.testConnection())
		safe("1");

	imu.setFullScaleGyroRange(MPU6050_GYRO_FS_250);

	if (imu.dmpInitialize() != 0)
		safe("2");

	imu.setXAccelOffset(-206);
	imu.setYAccelOffset(-453);
	imu.setZAccelOffset(5040);
	imu.setXGyroOffset(117);
	imu.setYGyroOffset(19);
	imu.setZGyroOffset(15);

	imu.CalibrateAccel(6);
	imu.CalibrateGyro(6);
	imu.PrintActiveOffsets();

	imu.setDMPEnabled(true);

	// Attach tachometer interrupts
	attachInterrupt(digitalPinToInterrupt(2), dmpDataReady, RISING); // 6050
	attachPCINT(digitalPinToPCINT(8), int0, CHANGE);								 // Pin 2 is mpu interrupt
	attachPCINT(digitalPinToPCINT(3), int1, CHANGE);
	attachPCINT(digitalPinToPCINT(4), int2, CHANGE);

	mpuIntStatus = imu.getIntStatus();

	packetSize = imu.dmpGetFIFOPacketSize();
	delay(1000);
}
unsigned long time = micros();
void loop()
{
	safe("not gonna do stuff");
	if (imu.dmpGetCurrentFIFOPacket(fifoBuffer))
	{
		imu.dmpGetQuaternion(&q, fifoBuffer);
		imu.dmpGetEuler(euler, &q);				 // Get euler angles
		imu.dmpGetGyro(&gyro, fifoBuffer); // Get gyro
		// Kalman filter stuff
		double dt = (micros() - time) / (1000000.0); // In secs
		time = micros();

		cube.calculateX(euler, gyro, dt); // Kaaaaaaal?

		BLA::Matrix<3> U = cube.getU();
		cube.run(); // "It just works"
		cube.printState();
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
	Serial.print("Fatal Err: ");
	Serial.println(s);
	while (true)
	{
		delay(10000);
	}
}
