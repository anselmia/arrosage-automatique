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