#include "Cubic.h"
#include "Motor.h"
#include "Wire.h"
#include <BasicLinearAlgebra.h>
#include <I2Cdev.h>
#include <MPU6050_6Axis_MotionApps20.h>
#include <PinChangeInterrupt.h>

using namespace BLA;

/*
Error codes:
0: Unassigned
1: imu connection failed
2: DMP init failed
*/

Cubic cube = Cubic();
unsigned long time;
MPU6050 imu;

// MPU control/status vars
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;      // [w, x, y, z]         quaternion container
VectorInt16 gyro;  // [x, y, z]
VectorInt16 accel; // [x, y, z]
float euler[3];    // [psi, theta, phi]    Euler angle container

volatile bool mpuInterrupt =
    false; // indicates whether MPU interrupt pin has gone high
void dmpDataReady() { mpuInterrupt = true; }
void setup() {
  Wire.begin();
  Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having
                         // compilation difficulties
  Wire.setWireTimeout(3000, true); // timeout value in uSec (fixes hang issue)
  Serial.begin(38400);
  delay(1000);
  Serial.println("Starting");
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
  imu.setXAccelOffset(806);
  imu.setYAccelOffset(572);
  imu.setZAccelOffset(5510);
  imu.setXGyroOffset(125);
  imu.setYGyroOffset(11);
  imu.setZGyroOffset(-5);

  // imu.CalibrateAccel();
  // imu.CalibrateGyro();
  // imu.PrintActiveOffsets();
  // safe("done calibrate");

  imu.setDMPEnabled(true);

  // Attach tachometer interrupts
  // attachInterrupt(digitalPinToInterrupt(2), dmpDataReady, RISING); // 6050
  attachPCINT(digitalPinToPCINT(cube.motors[0].tach), int2,
              RISING); // Pin 2 is mpu interrupt
  attachPCINT(digitalPinToPCINT(cube.motors[1].tach), int1, RISING);
  attachPCINT(digitalPinToPCINT(cube.motors[2].tach), int0, RISING);

  mpuIntStatus = imu.getIntStatus();

  packetSize = imu.dmpGetFIFOPacketSize();
  Serial.println("done");
  Serial.println("time,motor0,motor1,motor2,x,y,z,wx,wy,wz,ux,uy,uz");
  delay(1000);
  time = micros();
}

void log() {
  Serial.print(time);
  for (int i = 0; i < 9; i++) {
    Serial.print(",");
    Serial.print(cube.X(i));
  }
  for (int i = 0; i < 3; i++) {
    Serial.print(",");
    Serial.print(cube.U(i));
  }
}

void loop() {
  safe("not gonna do stuff");
  imu.getAcceleration(&accel.x, &accel.y, &accel.z);
  imu.getRotation(&gyro.x, &gyro.y, &gyro.z);
  double dt = (micros() - time) / (1000000.0); // In secs
  time = micros();
  cube.run(accel, gyro, dt); // "It just works"
  for (int i = 0; i < 3; i++) {
    if (cube.motors[i].rps > 90) {
      safe("Motor overspeed");
    } else if (abs(cube.X(i)) > 0.6) {
      safe("Fell over");
    }
  }
  log();
}

void int0(void) { cube.motors[0].interrupt(); }
void int1(void) { cube.motors[1].interrupt(); }
void int2(void) { cube.motors[2].interrupt(); }

void safe(String s) { // Put the chip into a safe mode is something
                      // unrecoverable goes wrong
  Serial.print("Fatal Err: ");
  Serial.println(s);
  while (cube.stop()) {
    double dt = (micros() - time) / (1000000.0); // In secs
    time = micros();
    cube.calculateX(accel, gyro, dt);
  }
  while (true) {
    delay(10000);
  }
}
