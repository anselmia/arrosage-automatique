#ifndef EV_h
#define EV_h
#include "myeeprom.h"

class EV
{
private:
    int num;
    int evPin;
    int evState;
    MYEEPROM eeprom = MYEEPROM();
    int leap_year(int year);
    const int max_time_on_ev = 20;

public:
    EV(int pin, int numEV);
    int nextDayOn;
    int getState();
    int remainingTimeOn;
    void ON();
    void OFF();
    void calculate_next_day(int day, int month, int year);
    void updateRemainingTime(int day, int month, int year);
};
#endif