#include "JetsonGPIO.h"
//#include "gpio_control.h"


using namespace GPIO;

void init_gpio()
{
	// Ausgangsnummerierung auf dem Board
	setmode(GPIO::BOARD);

	std::string info = GPIO::JETSON_INFO;
}

int pinMode(int pin, int direction)
{
	GPIO::setup(pin, ((direction == 1) ? GPIO::OUT : GPIO::IN  ));
	return 0;
}

void digitalWrite(int pin, int status)
{
	GPIO::output(pin, ((status == 1) ? GPIO::HIGH : GPIO::LOW));
}

void close_gpio()
{
	GPIO::cleanup();
}
