#include "PinChangeInterrupt.h"
#include "Adafruit_MPU6050.h"

Adafruit_MPU6050 mpu;
Adafruit_Sensor *mpu_temp, *mpu_accel, *mpu_gyro;

void setup()
{
  Serial.begin(9600);
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

  pinMode(9, OUTPUT);  // PWM 1
  pinMode(10, OUTPUT); // PWM 2
  pinMode(11, OUTPUT); // PWM 3

  Serial.println("Setup pin modes");

  attachPCINT(digitalPinToPCINT(2), int1, CHANGE);
  attachPCINT(digitalPinToPCINT(3), int2, CHANGE);
  attachPCINT(digitalPinToPCINT(4), int3, CHANGE);

  Serial.println("Setup interrupts");

  Serial.println("Setup Cubic, waiting 5s");
  delay(5000);

  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
}

float rps1 = 0;
float rps2 = 0;
float rps3 = 0;
unsigned long oldTime1 = micros();
unsigned long oldTime2 = micros();
unsigned long oldTime3 = micros();

void int1(void)
{
  if (digitalRead(2))
  {
    rps1 = 31415.9 / (micros() - oldTime1);
    oldTime1 = micros();
  }
}

void int2(void)
{
  if (digitalRead(3))
  {
    rps2 = 31415.9 / (micros() - oldTime2);
    oldTime2 = micros();
  }
}

void int3(void)
{
  if (digitalRead(4))
  {
    rps3 = 31415.9 / (micros() - oldTime3);
    oldTime3 = micros();
  }
}

int val = 0;
int inc = 1;
void loop()
{
  delay(1000);
  Serial.print("\n\t\tTachometer 1: ");
  Serial.print(rps1);
  Serial.print(" \t2: ");
  Serial.print(rps2);
  Serial.print(" \t3: ");
  Serial.print(rps3);
  Serial.println(" radians/s");
  Serial.println();

  val += inc;
  if (val * val > 255 || val * val < 0)
  {
    inc = -inc;
    val += inc;
  }
  analogWrite(9, val * val);
  analogWrite(10, val * val);
  analogWrite(11, val * val);

  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  mpu_temp->getEvent(&temp);
  mpu_accel->getEvent(&accel);
  mpu_gyro->getEvent(&gyro);
  Serial.print("\n\t\tTemperature ");
  Serial.print(temp.temperature);
  Serial.println(" deg C");

  Serial.print("\n\t\tAccelerometer X: ");
  Serial.print(accel.acceleration.x);
  Serial.print(" \tY: ");
  Serial.print(accel.acceleration.y);
  Serial.print(" \tZ: ");
  Serial.print(accel.acceleration.z);
  Serial.println(" m/s^2");

  Serial.print("\n\t\tGyro X: ");
  Serial.print(gyro.gyro.x);
  Serial.print(" \tY: ");
  Serial.print(gyro.gyro.y);
  Serial.print(" \tZ: ");
  Serial.print(gyro.gyro.z);
  Serial.println(" radians/s");
}