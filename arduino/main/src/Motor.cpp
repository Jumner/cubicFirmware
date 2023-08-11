#include "Motor.h"
#include <Arduino.h>
Motor::Motor(int n) {
  tach = n == 0 ? 8 : 5 - n;
  cw = 5 + n;
  pwm = 9 + n;

  // Set pin modes
  pinMode(cw, OUTPUT);
  pinMode(pwm, OUTPUT);

  // Set default values
  currentDir = LOW; // Counter Clockwise (-torque)
  digitalWrite(cw, currentDir);
  analogWrite(pwm, 255); // Off
}

void Motor::interrupt(void) {
  rps = 31415.9 / (micros() - oldTime);
  oldTime = micros();
}

void Motor::setPwm(int val, bool dir) {
  if (dir != currentDir) { // Changed Directions
    // Write new direction (HIGH == True == 0x1, LOW == False == 0x0)
    digitalWrite(cw, dir);
    currentDir = dir; // Update current dir
  }
  analogWrite(pwm, val); // Write pwm value
}

bool Motor::stop(float vel) {
  // Stop the motor
  if (rps < 20) {
    setPwm(255, currentDir);
    return false;
  }
  setTorque(rps * 0.001 * (vel > 0 ? -1.0 : 1.0), vel > 0 ? rps : -rps);
  return true; // Not done, keep calling
}

int Motor::maxTorque(double vel) {
  return 0.0625 - 0.000743 * vel - 0.00000348 * vel * vel +
         0.0000000429 * vel * vel * vel;
}

void Motor::setTorque(double t, double vel) {
  double decimalVal =
      abs(t / maxTorque(t > 0 ? vel : -vel)); // This ratio might be negative
                                              // (maxtorque is always > 0)
  // Also negate velocity when negative torques are needed to make it relative
  int val = 245 - 245 * max(min(decimalVal, 1), 0);
  setPwm(val, t > 0);
}
