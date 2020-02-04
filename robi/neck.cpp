#include "neck.h"

neck::neck()
{
	sh = new Servos(800, 1900, 1350);
	sv = new Servos(1200, 2100, 1850);

	//test();
}


neck::~neck()
{
}

void neck::move_to(float a, float b)
{
	sh->move_to_angle(a,1000);
	//OPTI Bewegung zusammen realisieren
	sv->move_to_angle(b,1000);
}

//void neck::move_to()
//{
//	sv->move_to_position();
//	sh->move_to_position();
//}

void neck::test()
{
	sv->test();
	sh->test();
}

//void neck::read_udp_data(float a, float b)
//{
//	sv->position = a;
//}
