#include "Motor.h"
#include <cstddef>

#define PERIOD 100 //ms zyclus

using namespace std;

int Motor::number_of_motors = 0;
int* Motor::delay_time = NULL; 
bool* Motor::direction = NULL;

//void delay(int ms)
//{
//	std::chrono::duration<int, std::milli> timespan(ms);
//	std::this_thread::sleep_for(timespan);
//}


Motor::Motor(int p1, int p2)
{
	pin1 = p1;
	pin2 = p2;

	assert(number_of_motors <= MAX_MOTORS);

	if (number_of_motors == 0)
	{
		delay_time = new int[MAX_MOTORS];
		direction = new bool[MAX_MOTORS];
	}

	motor_number = number_of_motors++; //HACK Nummirierung faengt von 0 an

	delay_time[motor_number] = 99;
	direction[motor_number] = true;



	pinMode(pin1, OUTPUT);
	pinMode(pin2, OUTPUT);

	th1 = new thread (main_loop, pin1, pin2, motor_number);

	cout << "Motor " << motor_number << " started, Id: " << th1->get_id() << endl;
}


Motor::~Motor()
{

}

void Motor::main_loop(int pin1, int pin2, int motor_number)
{
	int p = pin1;

	digitalWrite(pin1, LOW);
	digitalWrite(pin2, LOW);

	//TODO optimieren
	//zyklus 300 ms duty 0 bis 100 %
	
	while (true)
	{
		p = (direction[motor_number]) ? pin1 : pin2;

		if (delay_time[motor_number] != 99)
		{
			digitalWrite(p, HIGH);	
			delay(delay_time[motor_number]);
		}

		digitalWrite(p, LOW);	
		delay(PERIOD - delay_time[motor_number]);
	}	
}

void Motor::rotate(float speed)
{
	if (speed == 0)
	{
		delay_time[motor_number] = 99;
		return;
	}

	direction[motor_number] = (speed > 0) ? true : false;

	float s = abs(speed);

	s = (speed > 1.0f) ? 1.0f : speed;

	// delay_time[motor_number] = (int)(1/abs(speed)*1000); //TODO Abhaengigket ermitteln.
	delay_time[motor_number] = (int)(s*(float)PERIOD); //TODO Abhaengigket ermitteln.

	//cout << "delay " << delay_time[motor_number] << " motor " << motor_number  << endl;

}

void Motor::move(float distance)
{
	//TODO ermitteln geschwindigkeit
	float time = distance * 100;
	rotate (distance *1000 / time);
	delay((uint)time);
	stop();
}

void Motor::test()
{
	cout << " test started \n";
	rotate(0.2f);
	delay(3000);
	rotate(0.5f);
	delay(3000);
	rotate(1.0f);
	delay(3000);
	stop();
}
