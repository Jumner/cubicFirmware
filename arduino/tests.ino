#include "PinChangeInterrupt.h"
void setup()
{
	// put your setup code here, to run once:
	TCCR1B = TCCR1B & B11111000 | B00000001; // 9 & 10
	TCCR2B = TCCR2B & B11111000 | B00000001; // 11

	pinMode(2, INPUT_PULLUP); // Tackometer 1
	pinMode(3, INPUT_PULLUP); // Tackometer 2
	pinMode(4, INPUT_PULLUP); // Tackometer 3

	pinMode(5, OUTPUT); // CW/CCw 1
	pinMode(6, OUTPUT); // CW/CCw 2
	pinMode(7, OUTPUT); // CW/CCw 3

	pinMode(9, OUTPUT);	 // PWM 1
	pinMode(10, OUTPUT); // PWM 2
	pinMode(11, OUTPUT); // PWM 3

	pinMode(LED_BUILTIN, OUTPUT);
	Serial.begin(9600);
	attachPCINT(digitalPinToPCINT(2), test1, CHANGE);
	attachPCINT(digitalPinToPCINT(3), test2, CHANGE);
	attachPCINT(digitalPinToPCINT(4), test3, CHANGE);
	digitalWrite(5, HIGH);
	digitalWrite(6, HIGH);
	digitalWrite(7, HIGH);
	delay(1000);
}
float rps1 = 0;
float rps2 = 0;
float rps3 = 0;
unsigned long oldTime1 = micros();
unsigned long oldTime2 = micros();
unsigned long oldTime3 = micros();

void test1(void)
{
	if (digitalRead(2))
	{
		rps1 = 31415.9 / (micros() - oldTime1);
		oldTime1 = micros();
	}
}

void test2(void)
{
	if (digitalRead(3))
	{
		rps2 = 31415.9 / (micros() - oldTime2);
		oldTime2 = micros();
	}
}

void test3(void)
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
	// delay(100);
	Serial.print(rps1);
	Serial.print(" ");
	Serial.print(rps2);
	Serial.print(" ");
	Serial.println(rps3);
	val += inc;
	if (val * val > 255 || val * val < 0)
	{
		inc = -inc;
		val += inc;
	}
	analogWrite(9, val * val);
	analogWrite(10, val * val);
	analogWrite(11, val * val);
	// Serial.print("RPM: ");
	// Serial.println(rpm);
	// Serial.print("Val: ");
	// Serial.println(val);
}