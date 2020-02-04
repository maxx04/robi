#include "driver.h"

#define MOTOR_RECHTS_FW 16
#define MOTOR_RECHTS_BW 18
#define MOTOR_LINKS_FW 22
#define MOTOR_LINKS_BW 13

using namespace std;

driver::driver()
{
	init_gpio();

	motor_rechts = new Motor(MOTOR_RECHTS_FW, MOTOR_RECHTS_BW);
	motor_links = new Motor(MOTOR_LINKS_FW, MOTOR_LINKS_BW);

	cout << "driver started \n";

	//motor_links->test();
	//motor_rechts->test();

}


driver::~driver()
{
	close_gpio();
}

void driver::move(float distance, float duty)
{
	ramp(200, duty);
	motor_links->rotate(duty);
	motor_rechts->rotate(duty);
	delay((int)distance);
	ramp_down(200, duty);
}

void driver::start_move(float duty)
{
//	ramp(200, duty);
	motor_links->rotate(duty);
	motor_rechts->rotate(duty);
}

void driver::stop_move( float duty)
{
	ramp_down(200, duty);
}

void driver::change_direction(float angle)
{
	int time = (int)abs(angle)*10; //TODO ermitteln ungaefer

	if (angle == 0.0) return;

	if (angle < 0)
	{
		motor_links->rotate(0.5);
		delay(time);
		motor_links->stop();
	}
	else
	{
		motor_rechts->rotate(0.5);
		delay(time);
		motor_rechts->stop();
	}

}
// time in ms
void driver::ramp(float time, float duty)
{
	float time_step = 10.0;
	float d = 0.0;
	if (time <= 0.0 )
	{
		cerr << "driver: ramp time negativ or null \n";
		return;
	}

	float duty_step = duty/(time/ time_step);
	while (d < duty)
	{
		motor_links->rotate(d);
		motor_rechts->rotate(d);
		d += duty_step;
		delay((int)time_step);
	}
}

void driver::ramp_down(float time, float duty)
{
	float time_step = 10.0;
	float d = duty;
	if (time <= 0.0)
	{
		cerr << "driver: ramp time negativ or null \n";
		return;
	}

	float duty_step = duty / (time / time_step);
	while (d > 0.0)
	{
		motor_links->rotate(d);
		motor_rechts->rotate(d);
		d -= duty_step;
		delay((int)time_step);
	}
	motor_links->stop();
	motor_rechts->stop();
}
