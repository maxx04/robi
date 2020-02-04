#include "Servos.h"
//#pragma warning(disable : 4996)
SerialPort* Servos::sp = NULL;
char Servos::m[100];
int Servos::amount = 0;


//OPTI Fehlerbearbeitung(Servo kam nicht in position) realisieren

Servos::Servos(float min, float max, float null)
{
	null_position = null;
	max_position = max;
	min_position = min;
	grad_to_step = 10.0f;

	if (sp == NULL)
	{
		sp = new SerialPort("/dev/ttyTHS1");
	}

	number = ++amount;
	
	cout << "servo: " << number << " opened \n";

	move_to_position(null_position, 2000);
	//test();

}


Servos::~Servos()
{

}

void Servos::test()
{
	move_to_position(max_position, 2000);
	move_to_position(min_position, 2000);
	move_to_position(null_position, 2000);

}

void Servos::read_udp_data(float x)
{
	position = x;
}


//OPTI mit OK kann man grenzen ueberpruefen 

void Servos::move_to_angle(float a, int time)
{
	float p = null_position + a * grad_to_step;

	move_to_position(p, time);
}

void Servos::move_to_position(float p, int time)
{
	position = (p > max_position) ? max_position : p;
	position = (position < min_position) ? min_position : position;

	//sprintf(m, "#1P%04.0f#2P%04.0fT%04.0f\r\n", position.x, position.y, (float)time);
	sprintf(m, "#%1iP%04.0fT%04.0f\r\n", number, position, (float)time);

	std::cout << m << endl;

	sp->writeSerialPort(m);

	wait_on_position(time);

	in_move = true;
}

bool Servos::wait_on_position(const int time)
{
	tm.reset();
	tm.start();

	//cout << "waiting on servo ";

	while (sp->readSerialPort(m) == 0) // Antwort "OK"
	{

		if (tm.getTimeMilli() > (double)time)
		{
			//cerr << format("Kein Antwort Servo - position %f \n", position);
			return false;
		}
		tm.stop();
		tm.start();
	}

	 //cout << tm.getTimeMilli() << " serial : " << m[0] << m[1] << endl;
	 m[0] = 0;
	 m[1] = 0;
	return true;
}

