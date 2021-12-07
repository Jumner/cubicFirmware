// #include "I2Cdev.h"
// #include "MPU6050_6Axis_MotionApps20.h"
// #include "Wire.h"

// MPU6050 mpu;

// #define INTERRUPT_PIN 2 // use pin 2 on Arduino Uno & most boards

// // MPU control/status vars
// bool dmpReady = false;	// set true if DMP init was successful
// uint8_t mpuIntStatus;		// holds actual interrupt status byte from MPU
// uint8_t devStatus;			// return status after each device operation (0 = success, !0 = error)
// uint16_t packetSize;		// expected DMP packet size (default is 42 bytes)
// uint8_t fifoBuffer[64]; // FIFO storage buffer

// // orientation/motion vars
// Quaternion q;				 // [w, x, y, z]         quaternion container
// VectorFloat gravity; // [x, y, z]            gravity vector
// float euler[3];			 // [psi, theta, phi]    Euler angle container

// volatile bool mpuInterrupt = false; // indicates whether MPU interrupt pin has gone high
// void dmpDataReady()
// {
// 	mpuInterrupt = true;
// }

// void setup()
// {
// 	// join I2C bus (I2Cdev library doesn't do this automatically)
// 	Wire.begin();
// 	Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties

// 	// initialize serial communication
// 	Serial.begin(115200);
// 	while (!Serial)
// 		;
// 	// initialize device
// 	Serial.println(F("Initializing I2C devices..."));
// 	mpu.initialize();
// 	pinMode(INTERRUPT_PIN, INPUT);

// 	// verify connection
// 	Serial.println(F("Testing device connections..."));
// 	Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

// 	// load and configure the DMP
// 	Serial.println(F("Initializing DMP..."));
// 	devStatus = mpu.dmpInitialize();

// 	// supply your own gyro offsets here, scaled for min sensitivity
// 	mpu.setXGyroOffset(220);
// 	mpu.setYGyroOffset(76);
// 	mpu.setZGyroOffset(-85);
// 	mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

// 	// make sure it worked (returns 0 if so)
// 	if (devStatus == 0)
// 	{
// 		// Calibration Time: generate offsets and calibrate our MPU6050
// 		mpu.CalibrateAccel(6);
// 		mpu.CalibrateGyro(6);
// 		mpu.PrintActiveOffsets();
// 		// turn on the DMP, now that it's ready
// 		Serial.println(F("Enabling DMP..."));
// 		mpu.setDMPEnabled(true);

// 		// enable Arduino interrupt detection
// 		Serial.print(F("Enabling interrupt detection (Arduino external interrupt "));
// 		Serial.print(digitalPinToInterrupt(INTERRUPT_PIN));
// 		Serial.println(F(")..."));
// 		attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
// 		mpuIntStatus = mpu.getIntStatus();

// 		// set our DMP Ready flag so the main loop() function knows it's okay to use it
// 		Serial.println(F("DMP ready! Waiting for first interrupt..."));
// 		dmpReady = true;

// 		// get expected DMP packet size for later comparison
// 		packetSize = mpu.dmpGetFIFOPacketSize();
// 	}
// 	else
// 	{
// 		// ERROR!
// 		// 1 = initial memory load failed
// 		// 2 = DMP configuration updates failed
// 		// (if it's going to break, usually the code will be 1)
// 		Serial.print(F("DMP Initialization failed (code "));
// 		Serial.print(devStatus);
// 		Serial.println(F(")"));
// 	}
// }

// void loop()
// {
// 	// if programming failed, don't try to do anything
// 	if (!dmpReady)
// 		return;
// 	// read a packet from FIFO
// 	if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer))
// 	{ // Get the Latest packet

// 		// display Euler angles x,y,z
// 		mpu.dmpGetQuaternion(&q, fifoBuffer);
// 		// mpu.dmpGetGravity(&gravity, &q);
// 		mpu.dmpGetEuler(euler, &q);
// 		Serial.print("euler\t");
// 		Serial.print(euler[0]);
// 		Serial.print("\t");
// 		Serial.print(euler[1]);
// 		Serial.print("\t");
// 		Serial.println(euler[2]);
// 	}
// }