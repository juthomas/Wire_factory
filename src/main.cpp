#include <Arduino.h>
#include <Servo.h>
Servo myServo;
int motor_pin_1 = 5;
int motor_pin_2 = 6;
int motor_pin_3 = 7;
int motor_pin_4 = 8;

void start_motor(uint32_t hz)
{
	noInterrupts();
	// Clear registers
	TCCR2A = 0;
	TCCR2B = 0;
	TCNT2 = 0;

	// (16000000 / 1024 / 200 Hz) - 1
	// 200.32051282051282 Hz (16000000/((77+1)*1024))
	OCR2A = 16000000 / 1024 / hz;
	// CTC
	TCCR2A |= (1 << WGM21);
	// Prescaler 1024
	TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);
	// Output Compare Match A Interrupt Enable
	TIMSK2 |= (1 << OCIE2A);
	interrupts();
}

void stop_motor()
{
	noInterrupts();
	// Clear registers
	TCCR2A = 0;
	TCCR2B = 0;
	TCNT2 = 0;
	interrupts();
}

void setup()
{
	pinMode(motor_pin_1, OUTPUT);
	pinMode(motor_pin_2, OUTPUT);
	pinMode(motor_pin_3, OUTPUT);
	pinMode(motor_pin_4, OUTPUT);
	start_motor(500);
	myServo.attach(9);
}

uint8_t steps[4][4] = {
	{1, 0, 0, 0},
	{0, 1, 0, 0},
	{0, 0, 1, 0},
	{0, 0, 0, 1}};

int8_t sens = 1;

void update_motor()
{
	static uint8_t current_step = 0;
	if (sens == -1)
	{
		current_step = current_step == 0 ? 3 : current_step - 1;
	}
	else
	{
		current_step = current_step == 3 ? 0 : current_step + 1;
	}
	digitalWrite(motor_pin_1, steps[current_step][0]);
	digitalWrite(motor_pin_2, steps[current_step][1]);
	digitalWrite(motor_pin_3, steps[current_step][2]);
	digitalWrite(motor_pin_4, steps[current_step][3]);
}

ISR(TIMER2_COMPA_vect)
{
	update_motor();
}

void loop()
{
	static int servoPosition = 15;
	static int servoSens = 1;
	myServo.write(servoPosition);

	if (servoPosition >= 177)
	{
		servoSens = -1;
	}
	if (servoPosition <= 14)
	{
		servoSens = 1;
	}
	servoPosition += servoSens;
	if (servoPosition <= 40 || servoPosition >= 140)
	{
		delay(750);
	}
	else
	{
		delay(1500);
	}
}