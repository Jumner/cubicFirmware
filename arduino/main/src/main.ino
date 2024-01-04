#include "Cubic.h"
#include "Motor.h"
#include "Wire.h"
#include "flags.h"
#include <BasicLinearAlgebra.h>
#include <I2Cdev.h>
#include <MPU6050_6Axis_MotionApps20.h>
#include <PinChangeInterrupt.h>

Cubic cube = Cubic();
unsigned long time;
MPU6050 imu;

// orientation/motion vars
VectorInt16 gyro; // [x, y, z]

void setup() {
  Wire.begin();
  Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having
                         // compilation difficulties
  Wire.setWireTimeout(3000, true); // timeout value in uSec (fixes hang issue)
  Serial.begin(38400);
  delay(1000);
  imu.initialize();

  pinMode(8, INPUT); // tach 0
  pinMode(3, INPUT); // tach 1
  pinMode(4, INPUT); // tach 2

  if (!imu.testConnection())
    safe("IMU");

  imu.setFullScaleGyroRange(MPU6050_GYRO_FS_250);
  imu.setFullScaleAccelRange(MPU6050_ACCEL_FS_2);

  imu.setXAccelOffset(806);
  imu.setYAccelOffset(572);
  imu.setZAccelOffset(5510);
  imu.setXGyroOffset(125);
  imu.setYGyroOffset(11);
  imu.setZGyroOffset(-5);

  // Attach tachometer interrupts (Pin 2 is mpu interrupt)

  attachPCINT(digitalPinToPCINT(cube.motors[0].tach), int0, RISING); // 8
  attachPCINT(digitalPinToPCINT(cube.motors[1].tach), int1, RISING); // 4
  attachPCINT(digitalPinToPCINT(cube.motors[2].tach), int2, RISING); // 3

  if (LOGGING) {
    Serial.println("time,x,y,z,wx,wy,wz,m0,m1,m2,u0,u1,u2,spx,spy");
  }
  delay(1000);
  time = micros();
}

void int0(void) { cube.motors[0].interrupt(); }
void int1(void) { cube.motors[1].interrupt(); }
void int2(void) { cube.motors[2].interrupt(); }

void log() {
  Serial.print(((double)time / 1000000) - 2);
  for (int i = 0; i < 9; i++) {
    Serial.print(",");
    Serial.print(cube.X(i), 6);
  }
  for (int i = 0; i < 3; i++) {
    Serial.print(",");
    Serial.print(cube.U(i), 6);
  }
  for(int i = 0; i < 2; i ++) {
    Serial.print(",");
    Serial.print(cube.spCorrect[i], 6);
  }
  Serial.println();
}

void loop() {
  // safe("not gonna do stuff");
  imu.getRotation(&gyro.x, &gyro.y, &gyro.z);
  double dt = (micros() - time) / (1000000.0); // In secs
  time = micros();
  cube.run(gyro, dt); // "It just works"
  if (LOGGING)
    log();
  for (int i = 0; i < 3; i++) {
    if (cube.motors[i].rps > MAX_SPEED) { // 90
      safe("Motor overspeed");
    } else if (i != 2 && abs(cube.X(i)) > 0.25) { // 3rd is twist which is fine
      safe("Fell over");
    }
  }
}

void safe(String s) { // Put the chip into a safe mode is something
                      // unrecoverable goes wrong
  Serial.print("Fatal Err: ");
  Serial.println(s);
  float initialRps[3];
  for(int i = 0; i < 3; i ++) {
    initialRps[i] = cube.motors[i].rps;
  }
  while (cube.stop(initialRps)) {
    continue;
  }
  Serial.println("Cubic Safed");
  while (true) {
    delay(10000);
  }
}
