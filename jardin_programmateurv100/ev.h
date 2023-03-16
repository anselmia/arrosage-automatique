#ifndef EV_h
#define EV_h
#include "myeeprom.h"

const int max_time_on_ev = 20;

class EV
{
private:
    int num;
    int evPin;
    int evState;
    MYEEPROM eeprom = MYEEPROM();
    int leap_year(int year);
    void calculate_next_day(int day, int month, int year);
    void ON();
    void OFF();

public:
    EV(int pin, int numEV);
    int nextDayOn;
    int remainingTimeOn;
    void updateRemainingTime(int hr, int day, int month, int year);
    void updateSeason(int timeon, int freq);
    void init();
    void update_state();
};
#endif