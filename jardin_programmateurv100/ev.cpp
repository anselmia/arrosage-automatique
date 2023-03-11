// LED.cpp
#include "ev.h"

EV::EV(int pin)
{
    evPin = pin;
    evState = 0;
    remainingTimeOn = 0;
    nextDayOn = 0;
}

int EV::getState()
{
    return evState;
}

void EV::turnON()
{
    evState = 1;
}

void EV::turnOFF()
{
    evState = 0;
}