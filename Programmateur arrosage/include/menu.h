#ifndef MENU_h
#define MENU_h

#include <DS1307.h>
#include <Arduino.h>
#include "myeeprom.h"

class MENU
{
private:
public:
    MENU();
    DS1307 rtc;
    uint8_t sec, min, hour, day, month;
    uint16_t year;
    uint8_t rtc_sec;
    uint8_t rtc_day;
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
    void updateValue(MYEEPROM eeprom, byte dir, byte value = 0);
    void moveCursor(byte cursorPos);
    void selectEV(byte ev);
    byte selectedEV;
    bool delay;
    bool stop;
    bool manual;
    bool stop_all;
    byte manual_all;
};
#endif