#include "Motor.h"
#include <Arduino.h>
Motor::Motor(int n)
{
	tach = n == 0 ? 8 : 2 + n;
	cw = 5 + n;
	pwm = 9 + n;

	// Set pin modes
	pinMode(tach, INPUT_PULLUP);
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
	if (digitalRead(tach))
	{
		rps = 31415.9 / (micros() - oldTime);
		oldTime = micros();
	}
}

void Motor::setPwm(int val)
{
	analogWrite(pwm, val);
}

void Motor::setTorque(double t)
{
	if (t == 0.00)
	{
		setPwm(255);
		return;
	}
	double decimalVal = abs(t / (0.0625 - 0.000743 * rps - 0.00000348 * rps * rps + 0.0000000429 * rps * rps * rps));
	int val = 245 - 245 * max(min(decimalVal, 1), 0);
	setPwm(val);
	Serial.print(val);
	Serial.print(", ");
	Serial.println(rps);
}
// t = ((245-val)/245) * (0.0625 - 0.0007.43*rps - 0.00000348*rps*rps + 0.0000000429*rps*rps*rps)
// 245 - 245 * t / (0.0625 - 0.0007.43*rps - 0.00000348*rps*rps + 0.0000000429*rps*rps*rps) = val