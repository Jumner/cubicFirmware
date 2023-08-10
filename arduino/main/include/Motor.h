#pragma once

class Motor // 26 bytes
{
private:
  unsigned int cw : 4;       // 0.5 bytes
  unsigned int pwm : 8;      // 1 byte
  unsigned long int oldTime; // 8 bytes

public:
  unsigned int tach : 4; // 0.5 bytes (Exposed to remove interrupt headache)
  Motor(int n);
  ~Motor();
  void interrupt();
  void setPwm(int val, bool dir);
  void setTorque(double t, double vel);
  static int maxTorque(double vel);
  bool stop(float vel);
  double rps;    // 8 bytes
  double oldrps; // 8 bytes
};
