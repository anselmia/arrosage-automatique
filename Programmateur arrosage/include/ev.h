#ifndef EV_h
#define EV_h
#include "Arduino.h"
#include <DS1307.h>
#include "myeeprom.h"

const byte max_time_on_ev = 20;

class EV
{
private:
    byte num;
    byte evPin;
    byte evState;
    int leap_year(int year);
    void calculate_next_day(MYEEPROM eeprom, byte day, byte month, int year);
    void ON();
    void OFF();

public:
    EV(byte pin, byte numEV);
    byte nextDayOn;
    int remainingTimeOn;
    void updateRemainingTime(MYEEPROM eeprom, byte hr, byte min, byte day, byte month, int year);
    void updateSeason(MYEEPROM eeprom, byte timeon, byte freq);
    void init();
    void update_state();
};
#endif