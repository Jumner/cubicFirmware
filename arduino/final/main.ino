#include "Motor.h"
#include "Cubic.h"
#include <PinChangeInterrupt.h>
#include <I2Cdev.h>
#include <MPU6050_6Axis_MotionApps20.h>
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif
#include <BasicLinearAlgebra.h>

using namespace BLA;

MPU6050 imu;
Cubic cube = Cubic();

// MPU control/status vars
uint8_t mpuIntStatus;		// holds actual interrupt status byte from MPU
uint8_t devStatus;			// return status after each device operation (0 = success, !0 = error)
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
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
	Wire.begin();
	Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
	Fastwire::setup(400, true);
#endif
	Serial.begin(38400);

	Serial.println("Initializing MPU-6050");
	imu.initialize();

	pinMode(2, INPUT); // 6050
	pinMode(8, INPUT); // tach 0
	pinMode(3, INPUT); // tach 1
	pinMode(4, INPUT); // tach 2

	if (!imu.testConnection())
		safe("Connection Failed");

	Serial.println("Connection Successfull");

	Serial.println("Configuring Gyro Range");
	imu.setFullScaleGyroRange(MPU6050_GYRO_FS_250);

	Serial.println("Initializing DMP");
	devStatus = imu.dmpInitialize();

	if (devStatus == 0)
	{

		Serial.println("Setting Offsets");
		imu.setXAccelOffset(-206);
		imu.setYAccelOffset(-453);
		imu.setZAccelOffset(5040);
		imu.setXGyroOffset(117);
		imu.setYGyroOffset(19);
		imu.setZGyroOffset(15);

		imu.CalibrateAccel(6);
		imu.CalibrateGyro(6);
		imu.PrintActiveOffsets();

		Serial.println("Enabling DMP");
		imu.setDMPEnabled(true);

		Serial.println("Enabling interrupts");

		// Attach tachometer interrupts
		attachInterrupt(digitalPinToInterrupt(2), dmpDataReady, RISING); // 6050
		attachPCINT(digitalPinToPCINT(8), int0, CHANGE);								 // Pin 2 is mpu interrupt
		attachPCINT(digitalPinToPCINT(3), int1, CHANGE);
		attachPCINT(digitalPinToPCINT(4), int2, CHANGE);

		mpuIntStatus = imu.getIntStatus();

		Serial.println("DMP setup complete");
		packetSize = imu.dmpGetFIFOPacketSize();
		delay(1000);
	}
	else
	{
		String err = "";
		err = "DMP initialization failed (code: " + String(devStatus) + ")";
		safe(err);
	}

	// // Matrix stuff
	Serial << cube.A << '\n';
	Serial << cube.B << '\n';
	Serial << cube.C << '\n';
	Serial << cube.D << '\n';
	Serial << cube.Q << '\n';
	Serial << cube.R << '\n';
}

void loop()
{
	safe("not gonna do stuff");
	if (imu.dmpGetCurrentFIFOPacket(fifoBuffer))
	{
		imu.dmpGetQuaternion(&q, fifoBuffer);
		imu.dmpGetEuler(euler, &q);

		imu.dmpGetGyro(&gyro, fifoBuffer);
		Serial.print("Theta\t");
		Serial.print(euler[0]);
		Serial.print("\t");
		Serial.print(euler[1]);
		Serial.print("\t");
		Serial.println(euler[2]);
		Serial.print("Theta Dot\t");
		Serial.print(gyro.x / 7509.87263606);
		Serial.print("\t");
		Serial.print(gyro.y / 7509.87263606);
		Serial.print("\t");
		Serial.println(gyro.z / 7509.87263606);
	}
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

void safe(String s)
{ // Put the chip into a safe mode is something unrecoverable goes wrong
	Serial.print("Fatal Err: ");
	Serial.println(s);
	Serial.println("Something went wrong. System is in safe mode");
	while (true)
	{
		delay(10000);
	}
}
