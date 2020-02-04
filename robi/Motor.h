#pragma once
//#include "stddef.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <assert.h>
#include <math.h>
#include "gpio_control.h"
//#include "i2c.h"

#define MAX_MOTORS 2
#include <unistd.h>

inline void delay(int ms) { usleep(1000 * ms); }

class Motor
{
	static int number_of_motors;
	int motor_number;
	int pin1, pin2;

	static int* delay_time;
	static bool* direction;

	std::thread* th1;

	static void main_loop(int pin1, int pin2, int motor_number);

public:
	Motor(int p1, int p2);
	~Motor();

	void rotate(float speed);
	void move(float distance);
	void stop() { rotate(0); }
	void test();
};

