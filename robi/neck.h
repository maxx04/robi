#pragma once
#include "Servos.h"

class neck
{
	Servos* sh;
	Servos*	sv;

public:
	neck();
	~neck();
	//@param a horizontale winkel von gerade ausrichtung zu kameraausrictung
	//@param b vertikale winkel von gerade ausrichtung zu kameraausrictung
	void move_to(float a, float b);
	//move in position die sind in servos gespeichert servos;
	//void move_to();
	void test();
	//void read_udp_data(float a, float b);
};

