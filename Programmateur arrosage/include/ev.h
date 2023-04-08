#ifndef EV_h
#define EV_h
#include "myeeprom.h"
#include "Arduino.h"
#include <DS1307.h>

const byte max_time_on_ev = 20;

class EV
{
private:
    byte num;
    byte evPin;
    byte evState;
    MYEEPROM eeprom = MYEEPROM();
    int leap_year(int year);
    void calculate_next_day(byte day, byte month, int year);
    void ON();
    void OFF();

public:
    EV(byte pin, byte numEV);
    byte nextDayOn;
    int remainingTimeOn;
    void updateRemainingTime(byte hr, byte min, byte day, byte month, int year);
    void updateSeason(byte timeon, byte freq);
    void init();
    void update_state();
};
#endif