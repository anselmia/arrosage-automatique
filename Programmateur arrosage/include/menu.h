#ifndef MENU_h
#define MENU_h

#include "myeeprom.h"
// #include "clock.h"
#include <DS1307.h>

class MENU
{
private:
public:
    MENU();
    DS1307 rtc;
    uint8_t sec, min, hour, day, month;
    uint16_t year;
    int rtc_min;
    int rtc_day;
    int actualScreen;
    int actualLine;
    int cursorPos;
    int screenValue;
    void initClock(int (&error)[2]);
    void getClock(int (&error)[2]);
    void forward();
    void backward();
    void up();
    void down();
    void updateValue(int dir, int value = 0);
    void moveCursor(int cursorPos);
    void selectEV(int ev);
    MYEEPROM eeprom = MYEEPROM();
    int selectedEV;
    bool delay;
    bool stop;
    bool manual;
    int inactive;
    bool stop_all;
    int manual_all;
};
#endif