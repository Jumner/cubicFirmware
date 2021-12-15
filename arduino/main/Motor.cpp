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

void Motor::setTorque(double t, double vel)
{
	if (t < 0)
	{
		vel *= -1;
	}
	double decimalVal = abs(t / (0.0625 - 0.000743 * vel - 0.00000348 * vel * vel + 0.0000000429 * vel * vel * vel));
	int val = 245 - 245 * max(min(decimalVal, 0.5), 0);
	setPwm(val, t > 0);
	// Serial.print(t);
	// Serial.print(", ");
	// Serial.print(val);
	// Serial.print(", ");
	// Serial.println(vel);
}
// t = ((245-val)/245) * (0.0625 - 0.0007.43*rps - 0.00000348*rps*rps + 0.0000000429*rps*rps*rps)
// 245 - 245 * t / (0.0625 - 0.0007.43*rps - 0.00000348*rps*rps + 0.0000000429*rps*rps*rps) = val