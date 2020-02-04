#pragma once

/* https://github.com/pjueon/JetsonGPIO */

#define INPUT	0
#define OUTPUT	1

#define LOW		0
#define HIGH	1

void init_gpio();

int pinMode(int pin, int direction);

void digitalWrite(int pin, int status);

void close_gpio();

