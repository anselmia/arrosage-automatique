#ifndef MENU_h
#define MENU_h

#include "myeeprom.h"
#include <DS1307.h>
#include <Arduino.h>

class MENU
{
private:
public:
    MENU();
    DS1307 rtc;
    uint8_t sec, min, hour, day, month;
    uint16_t year;
    byte rtc_sec;
    byte rtc_day;
    byte actualScreen;
    byte actualLine;
    byte cursorPos;
    byte screenValue;
    void initClock(bool (&error)[2]);
    void getClock(bool (&error)[2]);
    void forward();
    void backward();
    void up();
    void down();
    void updateValue(byte dir, byte value = 0);
    void moveCursor(byte cursorPos);
    void selectEV(byte ev);
    MYEEPROM eeprom = MYEEPROM();
    byte selectedEV;
    bool delay;
    bool stop;
    bool manual;
    byte inactive;
    bool stop_all;
    byte manual_all;
};
#endif