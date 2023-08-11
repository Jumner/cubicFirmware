# 1 "/tmp/tmpxi_8i4hv"
#include <Arduino.h>
# 1 "/home/jumner/Desktop/cubicFirmware/arduino/main/src/main.ino"
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


VectorInt16 gyro;
void setup();
void log();
void loop();
void int0(void);
void int1(void);
void int2(void);
void safe(String s);
#line 17 "/home/jumner/Desktop/cubicFirmware/arduino/main/src/main.ino"
void setup() {
  Wire.begin();
  Wire.setClock(400000);

  Wire.setWireTimeout(3000, true);
  Serial.begin(38400);
  delay(1000);
  imu.initialize();

  pinMode(8, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);

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


  attachPCINT(digitalPinToPCINT(cube.motors[0].tach), int2,
              RISING);
  attachPCINT(digitalPinToPCINT(cube.motors[1].tach), int1, RISING);
  attachPCINT(digitalPinToPCINT(cube.motors[2].tach), int0, RISING);

  if (LOGGING) {
    Serial.println("time,x,y,z,wx,wy,wz,m0,m1,m2,u0,u1,u2");
  }
  delay(1000);
  time = micros();
}

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
  Serial.println();
}

void loop() {

  imu.getRotation(&gyro.x, &gyro.y, &gyro.z);
  double dt = (micros() - time) / (1000000.0);
  time = micros();
  cube.run(gyro, dt);
  if (LOGGING)
    log();
  for (int i = 0; i < 3; i++) {
    if (cube.motors[i].rps > MAX_SPEED) {
      safe("Motor overspeed");
    } else if (i != 2 && abs(cube.X(i)) > 0.25) {
      safe("Fell over");
    }
  }
}

void int0(void) { cube.motors[0].interrupt(); }
void int1(void) { cube.motors[1].interrupt(); }
void int2(void) { cube.motors[2].interrupt(); }

void safe(String s) {

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