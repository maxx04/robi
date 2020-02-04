#pragma once
#include <iostream>


#include "Motor.h"
#include "gpio_control.h"



class driver
{
	Motor* motor_links;
	Motor* motor_rechts;

public:
	driver();
	~driver();
	// bewegt sich uengefer distance
	void move(float distance, float duty);
	void start_move(float duty);
	void stop_move(float duty);
	void change_direction(float angle);
	void ramp(float time, float duty);
	void ramp_down(float time, float duty);
};

