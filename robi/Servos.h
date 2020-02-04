#ifndef _Servos
#define _Servos


#include <iostream>
#include <opencv2/core.hpp>
#include "SerialPort_nano.h"

using namespace std;
using namespace cv;

class Servos
{
	// nummer der servos
	int number;
	// anzahl der servos
	static int amount;
	TickMeter tm;
	static SerialPort* sp;
	static char m[100];
	float max_position;
	float min_position;

public:
	//aktuelle soll position 
	float position;
	float null_position;
	float grad_to_step;

	/// ???
	//float servo_delta;

	bool in_move;

	Servos(float max, float min, float null);
	~Servos();
	void test();
	void read_udp_data(float x);
	void move_to_angle(float a, int time = 300);
	//void correction(Point2f p);
	void move_to_position(float p, int time = 600);
	bool wait_on_position(const int time);
	//void seek();
};

#endif // !_Servos


