#include "Motor.h"
#include <Arduino.h>
Motor::Motor(int n)
{
  tach = n == 2 ? 8 : 3 + n;
  cw = 5 + n;
  pwm = 9 + n;

  // Set pin modes
  pinMode(cw, OUTPUT);
  pinMode(pwm, OUTPUT);

  // Set default values
  digitalWrite(cw, LOW);
  analogWrite(pwm, 255); // Off
}

Motor::~Motor()
{
  // Serial.println("Destructed Motor");
}

void Motor::interrupt(void)
{
  // if (digitalRead(tach))
  // {
  oldrps = rps;
  rps = 31415.9 / (micros() - oldTime);
  oldTime = micros();
  // }
}

void Motor::setPwm(int val, bool dir)
{
  if (dir)
  {
    digitalWrite(cw, HIGH);
  }
  else
  {
    digitalWrite(cw, LOW);
  }
  analogWrite(pwm, val);
}

bool Motor::stop(float vel) {
  // Stop the motor
  setPwm(255,false);
  if (rps < 10) {
    return false;  
  }
  return true; // Not done, keep calling
}

void Motor::setTorque(double t, double vel)
{
  if (t < 0)
  {
    vel *= -1; // make velocity relative
  }
  double decimalVal = abs(t / (0.0625 - 0.000743 * vel - 0.00000348 * vel * vel + 0.0000000429 * vel * vel * vel));
  int val = 245 - 245 * max(min(decimalVal, 1), 0);
  setPwm(val, t > 0);
}
